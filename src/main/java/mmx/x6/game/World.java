package mmx.x6.game;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.Random;
import java.util.function.Function;
import mmx.x6.anim.AnimSet;
import mmx.x6.audio.MusicPlayer;
import mmx.x6.audio.Sfx;
import mmx.x6.game.player.Player;
import mmx.x6.input.Buttons;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.CollisionGrid;
import mmx.x6.level.Level;
import mmx.x6.level.RecordDef;
import mmx.x6.level.TileLayer;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;
import mmx.x6.stage.StageDef;
import mmx.x6.stage.Stages;

public final class World {

    public static final int DIFFICULTY = 1;

    public final StageDef stage;
    public final Level level;
    public final CollisionGrid grid;
    public final TerrainMover mover;
    public final Sfx sfx;
    public final MusicPlayer music;
    public final Buttons buttons = new Buttons();
    public final CameraController camera = new CameraController();
    public final CameraZones zones;
    public final List<Entity> entities = new ArrayList<>();
    public final Random rng = new Random(6);
    public final StageFlow flow;
    public Progress progress = new Progress();
    public Player player;
    public Spawner spawner;
    public int checkpoint;
    public int lockCount;
    public long frame;
    public int bossHp;
    public int bossHpMax;
    public boolean bossBarShown;
    public float whiteLevel;
    public boolean flag26;
    public boolean flag28;
    public boolean healHold;
    public Entity pauseOwner;
    public final Set<RecordDef> soulsTaken = new HashSet<>();
    public String message;
    public int messageTimer;
    public float layer1Alpha = 1;
    public boolean layer1Relative;
    public float layer2Alpha = 1;
    public boolean layer2Front;

    private final Function<String, AnimSet> sets;
    private final List<Entity> pending = new ArrayList<>();
    private final CollisionGrid pristineGrid;
    private final int[] pristineArt;
    private int frozen;

    public World(Level level, Function<String, AnimSet> sets, Sfx sfx, MusicPlayer music) {
        this(Stages.INTRO, level, sets, sfx, music);
    }

    public World(StageDef stage, Level level, Function<String, AnimSet> sets, Sfx sfx, MusicPlayer music) {
        this.stage = stage;
        this.level = level;
        this.grid = level.collision();
        this.pristineGrid = grid.copy();
        this.pristineArt = level.art0Main().gids().clone();
        this.mover = new TerrainMover(grid);
        this.sets = sets;
        this.sfx = sfx;
        this.music = music;
        this.zones = new CameraZones(level.cameraZones());
        this.flow = new StageFlow(this);
        this.spawner = new Spawner(level.records(), level.fixedRecords(), DIFFICULTY, stage.factory().get());
        TileLayer l1 = level.art1();
        TileLayer l2 = level.art2();
        if (l1.x6Mode() > 0 && l2.x6Mode() > 0) {
            camera.configure(l1.x6Mode(), l1.x6OffsetX(), l1.x6OffsetY(), l2.x6Mode(), l2.x6OffsetX(),
                    l2.x6OffsetY());
        }
        this.player = new Player(this);
        Checkpoint cp = stage.checkpoints().get(0);
        camera.reset(cp);
        layer1Alpha = camera.layer1Mode() == 5 ? 0 : 1;
        player.respawn(cp);
    }

    public static World forGrid(CollisionGrid g, Function<String, AnimSet> sets, Sfx sfx, MusicPlayer music) {
        int w = g.width();
        int h = g.height();
        TileLayer empty = new TileLayer("empty", w, h, new int[w * h], 0, 0, 0);
        Level level = new Level(w, h, empty, empty, empty, empty, g, List.of(), List.of());
        return new World(level, sets, sfx, music);
    }

    public AnimSet animSet(String name) {
        return sets.apply(name);
    }

    public void add(Entity e) {
        pending.add(e);
    }

    public void freeze(int updates) {
        frozen = Math.max(frozen, updates);
    }

    public void showMessage(String text, int updates) {
        message = text;
        messageTimer = updates;
    }

    public void update() {
        frame++;
        if (messageTimer > 0 && --messageTimer == 0) {
            message = null;
        }
        flow.update();
        player.update(this);
        if (frozen > 0) {
            frozen--;
        } else {
            List<Rect> before = solidsBefore();
            if (pauseOwner != null && pauseOwner.dead) {
                pauseOwner = null;
            }
            for (Entity e : entities) {
                if (!e.dead && (pauseOwner == null || e == pauseOwner || e.runsInPause())) {
                    e.prevX = e.x;
                    e.prevY = e.y;
                    if (e.flash > 0) {
                        e.flash--;
                    }
                    e.update(this);
                }
            }
            carry(before);
            if (pauseOwner == null) {
                resolveWeapons();
                resolveContacts();
            }
        }
        entities.removeIf(e -> e.dead);
        camera.update(player.x, player.y);
        zones.update(player.x, player.y, camera);
        spawner.update(this);
        entities.addAll(pending);
        pending.clear();
    }

    private List<Rect> solidsBefore() {
        List<Rect> out = new ArrayList<>();
        for (Entity e : entities) {
            out.add(e instanceof Solid s && !e.dead ? s.solidRect() : null);
        }
        return out;
    }

    private void carry(List<Rect> before) {
        if (player.dying() || !player.body().hasBox) {
            return;
        }
        Rect feet = player.body().box();
        for (int i = 0; i < before.size(); i++) {
            Rect old = before.get(i);
            Entity e = entities.get(i);
            if (old == null || e.dead || !(e instanceof Solid s)) {
                continue;
            }
            Rect now = s.solidRect();
            if (now == null || !s.carries()) {
                continue;
            }
            float dx = now.x() - old.x();
            float dy = now.y() - old.y();
            boolean onTop = Math.abs(feet.bottom() - old.y()) <= 1.5f && feet.right() > old.x()
                    && feet.x() < old.right();
            if (onTop && (dx != 0 || dy != 0)) {
                player.carry(dx, dy);
                return;
            }
        }
    }

    private void resolveWeapons() {
        for (Entity e : entities) {
            if (e.dead || !(e instanceof Weapon wp)) {
                continue;
            }
            for (Entity t : entities) {
                Rect box = wp.attackBox();
                if (box == null || e.dead) {
                    break;
                }
                if (t == e || t.dead || t.damageTable == null || t instanceof Weapon) {
                    continue;
                }
                Rect hb = t.hurtBox();
                if (hb == null || !box.overlaps(hb) || !wp.canHit(t)) {
                    continue;
                }
                int r = Damage.hitObject(t, wp.weaponId());
                if (r == Damage.NO_HIT) {
                    continue;
                }
                if (r == Damage.BLOCKED) {
                    sfx.play(0, 6);
                    t.onDeflect(this, wp.weaponId());
                } else if (r < 0) {
                    t.onKilled(this, wp.weaponId());
                } else {
                    t.onHurt(this, wp.weaponId());
                }
                wp.onResult(this, t, r);
            }
        }
    }

    private void resolveContacts() {
        Rect hurt = player.hurtBox();
        if (hurt == null) {
            return;
        }
        for (Entity e : entities) {
            if (e.dead) {
                continue;
            }
            Rect c = e.contactBox();
            if (c != null && c.overlaps(hurt) && player.hurt(this, e.contactDamage, e.x)) {
                e.onTouchPlayer(this);
            }
        }
    }

    public void clearEnemies() {
        for (Entity e : entities) {
            if (!e.dead && e.clearedByGate()) {
                e.dead = true;
                spawner.released(e.record);
            }
        }
        for (Entity e : pending) {
            if (e.clearedByGate()) {
                e.dead = true;
                spawner.released(e.record);
            }
        }
    }

    public void resetTo(int checkpointIndex, Checkpoint cp) {
        entities.clear();
        pending.clear();
        lockCount = 0;
        frozen = 0;
        checkpoint = checkpointIndex;
        grid.restoreFrom(pristineGrid);
        System.arraycopy(pristineArt, 0, level.art0Main().gids(), 0, pristineArt.length);
        camera.reset(cp);
        zones.reset();
        spawner.reset();
        bossHp = 0;
        bossBarShown = false;
        whiteLevel = 0;
        flag26 = false;
        flag28 = false;
        healHold = false;
        layer1Alpha = camera.layer1Mode() == 5 ? 0 : 1;
        layer1Relative = false;
        layer2Alpha = 1;
        layer2Front = false;
        pauseOwner = null;
        sfx.useBossBank(false);
        music.play(stage.music());
        player.respawn(cp);
    }

    public void copyTiles(int srcX, int srcY, int cellsW, int cellsH, int dstX, int dstY) {
        TileLayer art = level.art0Main();
        int sc = srcX / 16;
        int sr = srcY / 16;
        int dc = dstX / 16;
        int dr = dstY / 16;
        for (int r = 0; r < cellsH; r++) {
            for (int c = 0; c < cellsW; c++) {
                grid.set(dc + c, dr + r, grid.raw(sc + c, sr + r));
                art.setGid(dc + c, dr + r, art.gid(sc + c, sr + r));
            }
        }
    }

    public List<Rect> solids() {
        List<Rect> out = new ArrayList<>();
        for (Entity e : entities) {
            if (!e.dead && e instanceof Solid s) {
                Rect r = s.solidRect();
                if (r != null) {
                    out.add(r);
                }
            }
        }
        return out;
    }

    public boolean locked() {
        return lockCount > 0;
    }

    public boolean offScreen(float x, float y, float mx, float my) {
        float cx = camera.x();
        float cy = camera.y();
        return x < cx - mx || x >= cx + CameraController.VIEW_W + mx || y < cy - my
                || y >= cy + CameraController.VIEW_H + my;
    }
}
