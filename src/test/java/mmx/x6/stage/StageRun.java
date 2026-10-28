package mmx.x6.stage;

import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.function.Predicate;
import mmx.x6.DataPaths;
import mmx.x6.game.Entity;
import mmx.x6.game.Sim;
import mmx.x6.game.World;
import mmx.x6.level.TmxLoader;

public final class StageRun {

    public final World world;
    public final Sim.RecordingSfx sfx = new Sim.RecordingSfx();
    public final Sim.RecordingMusic music = new Sim.RecordingMusic();

    private StageRun(StageDef stage, int checkpoint) {
        assumeTrue(DataPaths.assetsExist());
        DataPaths p = DataPaths.assets();
        try {
            world = new World(stage, TmxLoader.load(p.tmx(stage.tmxFile())), Sim::set, sfx, music);
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        world.flow.restart(checkpoint);
    }

    public static StageRun at(StageDef stage, int checkpoint) {
        return new StageRun(stage, checkpoint);
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

    public boolean until(int max, int buttons, Predicate<World> done) {
        for (int i = 0; i < max; i++) {
            if (done.test(world)) {
                return true;
            }
            step(buttons);
        }
        return done.test(world);
    }

    public boolean runRight(int max, Predicate<World> done) {
        for (int i = 0; i < max; i++) {
            if (done.test(world)) {
                return true;
            }
            step(mmx.x6.input.Buttons.RIGHT | (i % 40 < 25 ? mmx.x6.input.Buttons.JUMP : 0));
        }
        return done.test(world);
    }

    public <T extends Entity> T find(Class<T> type) {
        for (Entity e : world.entities) {
            if (type.isInstance(e) && !e.dead) {
                return type.cast(e);
            }
        }
        return null;
    }

    public long count(Class<? extends Entity> type) {
        return world.entities.stream().filter(e -> type.isInstance(e) && !e.dead).count();
    }
}
