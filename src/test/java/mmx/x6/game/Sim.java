package mmx.x6.game;

import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import mmx.x6.DataPaths;
import mmx.x6.anim.AnimSet;
import mmx.x6.anim.BankSheetAnims;
import mmx.x6.anim.D1000Anims;
import mmx.x6.anim.SabreBlockAnims;
import mmx.x6.anim.ShotAnims;
import mmx.x6.anim.SpriteBounds;
import mmx.x6.audio.MusicPlayer;
import mmx.x6.audio.Sfx;
import mmx.x6.level.CollisionGrid;
import mmx.x6.level.Checkpoint;

public final class Sim {

    public static final class RecordingSfx implements Sfx {
        public final List<String> played = new ArrayList<>();
        public final List<String> stopped = new ArrayList<>();

        @Override
        public void play(int bank, int id) {
            played.add(bank + "," + id);
        }

        @Override
        public void stop(int bank, int id) {
            stopped.add(bank + "," + id);
        }

        public int count(int bank, int id) {
            return (int) played.stream().filter((bank + "," + id)::equals).count();
        }
    }

    public static final class RecordingMusic implements MusicPlayer {
        public final List<Integer> requests = new ArrayList<>();
        private int track = -1;

        @Override
        public void play(int t) {
            requests.add(t);
            track = t;
        }

        @Override
        public void stop() {
            track = -1;
        }

        @Override
        public boolean playing() {
            return track >= 0;
        }

        @Override
        public int track() {
            return track;
        }

        @Override
        public void update() {
        }
    }

    private static final Map<String, AnimSet> SETS = new HashMap<>();

    public final World world;
    public final RecordingSfx sfx = new RecordingSfx();
    public final RecordingMusic music = new RecordingMusic();

    private Sim(CollisionGrid grid) {
        world = World.forGrid(grid, Sim::set, sfx, music);
    }

    public static AnimSet set(String name) {
        assumeTrue(DataPaths.assetsExist());
        if (name.equals("shots")) {
            return ShotAnims.SET;
        }
        if (name.equals("sabre-block")) {
            return SabreBlockAnims.SET;
        }
        if (BankSheetAnims.SHEETS.containsKey(name)) {
            return BankSheetAnims.SHEETS.get(name).set();
        }
        if (name.equals("d1000")) {
            AnimSet merged = SETS.get(name);
            if (merged == null) {
                merged = D1000Anims.merge(set("d-1000"), set("d-1000-parts"));
                SETS.put(name, merged);
            }
            return merged;
        }
        return SETS.computeIfAbsent(name, n -> {
            DataPaths p = DataPaths.assets();
            try {
                java.nio.file.Path dir = n.equals("x-falcon") ? p.playerDir(n) : p.enemyDir(n);
                AnimSet parsed = AnimSet.parse(Files.readString(dir.resolve("animations.json")));
                return n.equals("x-sabre") ? SpriteBounds.attach(parsed, dir) : parsed;
            } catch (IOException e) {
                throw new UncheckedIOException(e);
            }
        });
    }

    public static Sim on(CollisionGrid grid) {
        return new Sim(grid);
    }

    public Sim playerAt(float x, float feetY) {
        int w = world.grid.width() * 16;
        int h = world.grid.height() * 16;
        world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, Math.max(0, w - 320), 0, Math.max(0, h - 240), true, 1));
        world.camera.setFollow(true);
        world.player.placeStanding(x, feetY - 21);
        return this;
    }

    public void step(int buttons) {
        world.buttons.set(buttons);
        world.update();
    }

    public void steps(int n, int buttons) {
        for (int i = 0; i < n; i++) {
            step(buttons);
        }
    }
}
