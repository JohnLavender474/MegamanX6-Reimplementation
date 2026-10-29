package mmx.x6.stage;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.util.Map;
import java.util.TreeMap;
import mmx.x6.DataPaths;
import mmx.x6.audio.Sfx;
import mmx.x6.game.Entity;
import mmx.x6.game.Sim;
import mmx.x6.game.World;
import mmx.x6.input.Buttons;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class StageSmokeTest {

    private static final int UPDATES = 2000;

    @ParameterizedTest
    @ValueSource(strings = {"intro", "yammark", "wolfang", "heatnix", "scaravich", "turtloid", "sheldon"})
    void everyCheckpointRuns(String key) throws IOException {
        assumeTrue(DataPaths.assetsExist());
        StageDef stage = Stages.byKey(key);
        DataPaths p = DataPaths.assets();
        Map<String, Integer> seen = new TreeMap<>();
        int count = StageLoader.builtIn(p, stage).stage().checkpoints().size();
        for (int cp = 0; cp < count; cp++) {
            LoadedStage loaded = StageLoader.builtIn(p, stage);
            World w = new World(loaded.stage(), loaded.level(), Sim::set, Sfx.NONE, new Sim.RecordingMusic());
            w.flow.restart(cp);
            float startX = w.player.x;
            float maxX = startX;
            for (int i = 0; i < UPDATES; i++) {
                int b = Buttons.RIGHT;
                if (i % 40 < 25) {
                    b |= Buttons.JUMP;
                }
                if (i % 10 < 5) {
                    b |= Buttons.BUSTER;
                }
                w.buttons.set(b);
                w.update();
                maxX = Math.max(maxX, w.player.x);
                for (Entity e : w.entities) {
                    seen.merge(e.getClass().getSimpleName(), 1, Integer::sum);
                }
            }
            System.out.printf("%s cp%d start x %.0f max x %.0f hp %d checkpoint %d%n", key, cp, startX, maxX,
                    w.player.hp, w.checkpoint);
        }
        System.out.println(key + " objects: " + seen.keySet());
        assertTrue(true);
    }
}
