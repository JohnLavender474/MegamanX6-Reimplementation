package mmx.x6.stage;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import mmx.x6.DataPaths;
import mmx.x6.level.Level;
import mmx.x6.level.TmxLoader;
import org.junit.jupiter.api.Test;

class StagesTest {

    @Test
    void keysFindStages() {
        assertSame(Stages.TURTLOID, Stages.byKey("Turtloid"));
        assertNull(Stages.byKey("gate"));
    }

    @Test
    void everyStageLoads() throws IOException {
        assumeTrue(DataPaths.assetsExist());
        DataPaths p = DataPaths.assets();
        for (StageDef s : Stages.ALL) {
            LoadedStage loaded = StageLoader.builtIn(p, s);
            Level level = loaded.level();
            assertFalse(level.records().isEmpty(), s.key());
            long markers = level.records().stream().filter(r -> r.name().equals("CheckpointMarker")).count();
            assertEquals(loaded.stage().checkpoints().size() - 1, markers, s.key());
        }
    }

    @Test
    void heatnixFixedRecords() throws IOException {
        assumeTrue(DataPaths.assetsExist());
        DataPaths p = DataPaths.assets();
        Level level = TmxLoader.load(p.tmx(Stages.HEATNIX.tmxFile()));
        assertEquals(3, level.fixedRecords().size());
        assertEquals("BossGate", level.fixedRecords().get(2).name());
        assertEquals(7, level.fixedRecords().get(2).id());
    }
}
