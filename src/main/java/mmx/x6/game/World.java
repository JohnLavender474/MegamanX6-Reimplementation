package mmx.x6.game;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.function.Function;
import mmx.x6.anim.AnimSet;
import mmx.x6.audio.MusicPlayer;
import mmx.x6.audio.Sfx;
import mmx.x6.game.enemy.IntroEntityFactory;
import mmx.x6.game.player.Player;
import mmx.x6.input.Buttons;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.CollisionGrid;
import mmx.x6.level.IntroCheckpoints;
import mmx.x6.level.Level;
import mmx.x6.level.TileLayer;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;

public final class World {

    public static final int DIFFICULTY = 1;

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
    public Player player;
    public Spawner spawner;
    public int checkpoint;
    public int lockCount;
    public long frame;

    private final Function<String, AnimSet> sets;
    private final List<Entity> pending = new ArrayList<>();
    private final CollisionGrid pristineGrid;
    private final int[] pristineArt;
    private int frozen;

    public World(Level level, Function<String, AnimSet> sets, Sfx sfx, MusicPlayer music) {
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
        this.spawner = new Spawner(level.records(), DIFFICULTY, new IntroEntityFactory());
        this.player = new Player(this);
        Checkpoint cp = IntroCheckpoints.ALL.get(0);
        camera.reset(cp);
        camera.setFollow(true);
        player.placeStanding(cp.playerX(), cp.playerY());
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

    public void update() {
        frame++;
        flow.update();
        player.update(this);
        if (frozen > 0) {
            frozen--;
        } else {
            for (Entity e : entities) {
                if (!e.dead) {
                    e.prevX = e.x;
                    e.prevY = e.y;
                    if (e.flash > 0) {
                        e.flash--;
                    }
                    e.update(this);
                }
            }
            resolveWeapons();
            resolveContacts();
        }
        entities.removeIf(e -> e.dead);
        camera.update(player.x, player.y);
        zones.update(player.x, player.y, camera);
        spawner.update(this);
        entities.addAll(pending);
        pending.clear();
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

    public void resetTo(int checkpointIndex, Checkpoint cp) {
        entities.clear();
        pending.clear();
        lockCount = 0;
        frozen = 0;
        checkpoint = checkpointIndex;
        grid.restoreFrom(pristineGrid);
        System.arraycopy(pristineArt, 0, level.art0Main().gids(), 0, pristineArt.length);
        camera.reset(cp);
        camera.setFollow(true);
        zones.reset();
        spawner.reset();
        music.play(9);
        player.respawn(cp);
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
