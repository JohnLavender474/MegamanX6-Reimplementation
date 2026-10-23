package mmx.x6.game;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.function.Function;
import mmx.x6.anim.AnimSet;
import mmx.x6.audio.MusicPlayer;
import mmx.x6.audio.Sfx;
import mmx.x6.game.player.Player;
import mmx.x6.input.Buttons;
import mmx.x6.level.CollisionGrid;
import mmx.x6.level.IntroCheckpoints;
import mmx.x6.level.Level;
import mmx.x6.level.TileLayer;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;

public final class World {

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
    public Player player;
    public int checkpoint;
    public int lockCount;
    public long frame;

    private final Function<String, AnimSet> sets;
    private final List<Entity> pending = new ArrayList<>();

    public World(Level level, Function<String, AnimSet> sets, Sfx sfx, MusicPlayer music) {
        this.level = level;
        this.grid = level.collision();
        this.mover = new TerrainMover(grid);
        this.sets = sets;
        this.sfx = sfx;
        this.music = music;
        this.zones = new CameraZones(level.cameraZones());
        this.player = new Player(this);
        camera.reset(IntroCheckpoints.ALL.get(0));
        camera.setFollow(true);
        player.placeStanding(IntroCheckpoints.ALL.get(0).playerX(), IntroCheckpoints.ALL.get(0).playerY());
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

    public void update() {
        frame++;
        player.update(this);
        for (Entity e : entities) {
            if (!e.dead) {
                e.prevX = e.x;
                e.prevY = e.y;
                e.update(this);
            }
        }
        entities.removeIf(e -> e.dead);
        entities.addAll(pending);
        pending.clear();
        camera.update(player.x, player.y);
        zones.update(player.x, player.y, camera);
    }

    public List<Rect> solids() {
        return List.of();
    }

    public boolean locked() {
        return lockCount > 0;
    }
}
