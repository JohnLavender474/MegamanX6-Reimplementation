package mmx.x6.stage;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.util.List;
import mmx.x6.DataPaths;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.ValidationReport;
import org.junit.jupiter.api.Test;

class StageLoaderTest {

    @Test
    void introDataComesFromTheMap() throws IOException {
        assumeTrue(DataPaths.assetsExist());
        LoadedStage s = StageLoader.builtIn(DataPaths.assets(), Stages.INTRO);
        StageDef d = s.stage();
        assertEquals(List.of(new Checkpoint(144, 976, 0, 944, 0, 5120, 896, 944, true, 1),
                new Checkpoint(2112, 1072, 1968, 944, 1968, 5120, 944, 944, true, 6),
                new Checkpoint(5904, 364, 5744, 224, 5744, 5744, 224, 224, true, 1)), d.checkpoints());
        assertEquals(9, d.music());
        assertEquals(21, d.bossMusic());
        assertEquals("m094-e05", d.bank2());
        assertEquals("m094-e03", d.bank5());
        assertEquals("m123-e01", d.bossBank2());
        assertEquals(0, d.nightmareEffect());
        assertEquals(ValidationReport.Status.PLAYABLE, s.report().status());
    }

    @Test
    void yammarkIsNotValidated() throws IOException {
        assumeTrue(DataPaths.assetsExist());
        LoadedStage s = StageLoader.builtIn(DataPaths.assets(), Stages.YAMMARK);
        assertNull(s.report());
        assertSame(Stages.YAMMARK, s.stage());
    }
}
