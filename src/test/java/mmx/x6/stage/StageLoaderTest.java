package mmx.x6.stage;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import mmx.x6.DataPaths;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.Finding;
import mmx.x6.level.ValidationReport;
import mmx.x6.level.Catalog;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

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

    @Test
    void fileLevelInOtherFolderUsesThemeTiles(@TempDir Path dir) throws IOException {
        assumeTrue(DataPaths.assetsExist());
        DataPaths p = DataPaths.assets();
        Path copy = dir.resolve("my-level.tmx");
        Files.writeString(copy, Files.readString(p.tmx("intro.tmx"), StandardCharsets.UTF_8), StandardCharsets.UTF_8);
        LoadedStage s = StageLoader.file(p, copy);
        assertEquals(ValidationReport.Status.PLAYABLE, s.report().status());
        assertEquals("my-level.tmx", s.stage().tmxFile());
        assertEquals("intro-tiles.png", Themes.tilesPng(Catalog.load(p.catalogJson()), "intro"));
        assertEquals("intro-tiles.png", s.tilesPng());
        assertEquals(StageLoader.builtIn(p, Stages.INTRO).stage().checkpoints(), s.stage().checkpoints());
    }

    @Test
    void rejectedFileHasNoStage(@TempDir Path dir) throws IOException {
        assumeTrue(DataPaths.assetsExist());
        DataPaths p = DataPaths.assets();
        String text = Files.readString(p.tmx("intro.tmx"), StandardCharsets.UTF_8);
        String cut = text.replaceAll("(?s)<layer [^>]*name=\"collision\".*?</layer>", "");
        Path copy = dir.resolve("bad.tmx");
        Files.writeString(copy, cut, StandardCharsets.UTF_8);
        LoadedStage s = StageLoader.file(p, copy);
        assertEquals(ValidationReport.Status.REJECTED, s.report().status());
        assertNull(s.stage());
        assertNull(s.level());
    }

    private static void assertRejectedWith(LoadedStage s, String code) {
        assertEquals(ValidationReport.Status.REJECTED, s.report().status());
        assertTrue(s.report().findings().stream().anyMatch(f -> f.result() == Finding.Result.REJECT
                && f.code().equals(code)), s.report().findings().toString());
        assertNull(s.stage());
        assertNull(s.level());
    }

    @Test
    void missingFileIsRejectedWithRead(@TempDir Path dir) throws IOException {
        assumeTrue(DataPaths.assetsExist());
        assertRejectedWith(StageLoader.file(DataPaths.assets(), dir.resolve("none.tmx")), "read");
    }

    @Test
    void fileThatIsNotUtf8IsRejectedWithRead(@TempDir Path dir) throws IOException {
        assumeTrue(DataPaths.assetsExist());
        Path bad = dir.resolve("bad.tmx");
        Files.write(bad, new byte[] {'<', 'm', (byte) 0xC3, (byte) 0x28, '>'});
        assertRejectedWith(StageLoader.file(DataPaths.assets(), bad), "read");
    }

    @Test
    void loaderFailureAfterValidationIsRejectedWithLoad(@TempDir Path dir) throws IOException {
        assumeTrue(DataPaths.assetsExist());
        DataPaths p = DataPaths.assets();
        String text = Files.readString(p.tmx("intro.tmx"), StandardCharsets.UTF_8);
        String bad = text.replaceFirst("<object id=\"1\" name=\"CheckpointMarker\"",
                "<object id=\"abc\" name=\"CheckpointMarker\"");
        assertNotEquals(text, bad);
        Path copy = dir.resolve("bad.tmx");
        Files.writeString(copy, bad, StandardCharsets.UTF_8);
        assertRejectedWith(StageLoader.file(p, copy), "load");
    }
}
