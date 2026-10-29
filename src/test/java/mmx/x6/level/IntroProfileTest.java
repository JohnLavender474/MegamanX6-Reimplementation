package mmx.x6.level;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import mmx.x6.DataPaths;
import org.junit.jupiter.api.Test;

class IntroProfileTest {

    private static String intro() throws IOException {
        assumeTrue(DataPaths.assetsExist());
        return Files.readString(DataPaths.assets().tmx("intro.tmx"), StandardCharsets.UTF_8);
    }

    private static ValidationReport validate(String text) throws IOException {
        DataPaths p = DataPaths.assets();
        Catalog catalog = Catalog.load(p.catalogJson());
        return LevelValidator.validate(text, catalog, name -> {
            try {
                return Files.readString(p.tmx(name), StandardCharsets.UTF_8);
            } catch (IOException e) {
                throw new UncheckedIOException(e);
            }
        });
    }

    @Test
    void introIsPlayableWithLocksOnly() throws IOException {
        ValidationReport r = validate(intro());
        assertEquals(ValidationReport.Status.PLAYABLE, r.status(), r.findings().toString());
        for (Finding f : r.findings()) {
            assertEquals(Finding.Result.LOCK, f.result(), f.toString());
            assertEquals("stage-bound", f.code());
        }
    }

    @Test
    void movedD1000IsRejected() throws IOException {
        String text = intro();
        String moved = text.replaceFirst("(name=\"D1000\"[^>]*?) x=\"6162\"", "$1 x=\"6000\"");
        assertFalse(text.equals(moved));
        ValidationReport r = validate(moved);
        assertTrue(r.findings().stream().anyMatch(f -> f.result() == Finding.Result.REJECT
                && f.code().equals("fixed-region")), r.findings().toString());
    }

    @Test
    void editOutsideRegionIsAccepted() throws IOException {
        String text = intro();
        int a = text.indexOf("<layer id=\"3\"");
        assertTrue(a >= 0);
        int data = text.indexOf("<data encoding=\"csv\">", a) + "<data encoding=\"csv\">".length();
        int rowStart = data + 1;
        int[] commas = new int[11];
        int pos = rowStart;
        for (int i = 0; i < 11; i++) {
            pos = text.indexOf(',', pos) + 1;
            commas[i] = pos;
        }
        int cellStart = commas[9];
        int cellEnd = commas[10] - 1;
        String old = text.substring(cellStart, cellEnd);
        String edited = text.substring(0, cellStart) + (old.equals("1") ? "2" : "1") + text.substring(cellEnd);
        ValidationReport r = validate(edited);
        assertTrue(r.findings().stream().noneMatch(f -> f.result() == Finding.Result.REJECT),
                r.findings().toString());
    }

    @Test
    void deletedCheckpointWithItsMarkerLeftIsRejected() throws IOException {
        String text = intro();
        String cut = text.replaceFirst("(?s)<object id=\"83\".*?</object>\\s*", "");
        assertFalse(text.equals(cut));
        ValidationReport r = validate(cut);
        assertTrue(r.findings().stream().anyMatch(f -> f.result() == Finding.Result.REJECT
                && f.code().equals("checkpoint")), r.findings().toString());
    }

    @Test
    void deletedGoalIsRepairedWithoutAFixedRegionReject() throws IOException {
        String text = intro();
        String cut = text.replaceFirst("(?s)<object id=\"80\".*?</object>\\s*", "");
        assertFalse(text.equals(cut));
        ValidationReport r = validate(cut);
        assertTrue(r.findings().stream().noneMatch(f -> f.code().equals("fixed-region")), r.findings().toString());
        assertTrue(r.findings().stream().anyMatch(f -> f.result() == Finding.Result.REPAIR
                && f.code().equals("goal-ref")), r.findings().toString());
        assertTrue(r.findings().stream().anyMatch(f -> f.result() == Finding.Result.WARN
                && f.code().equals("goal-ref")), r.findings().toString());
        assertEquals(ValidationReport.Status.PLAYABLE, r.status());
    }
}
