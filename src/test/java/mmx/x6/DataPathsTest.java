package mmx.x6;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.file.Files;
import java.nio.file.Path;
import org.junit.jupiter.api.Assumptions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

class DataPathsTest {

    @Test
    void argumentWinsOverDefault() {
        DataPaths p = DataPaths.resolve(new String[] {"/tmp/x6data"});
        assertEquals(Path.of("/tmp/x6data/tmx/m094-intro/intro.tmx"), p.tmx());
    }

    @Test
    void systemPropertyWinsOverArgument() {
        System.setProperty("mmx.data", "/opt/d");
        try {
            assertEquals(Path.of("/opt/d"), DataPaths.resolve(new String[] {"/tmp/a"}).root());
        } finally {
            System.clearProperty("mmx.data");
        }
    }

    @Test
    void movieFramePath() {
        DataPaths p = new DataPaths(Path.of("/d"));
        assertEquals(Path.of("/d/movies/x6op/frame-0007.png"), p.movieFrame("x6op", 7));
        assertEquals(Path.of("/d/movies/caplogo/audio.wav"), p.movieAudio("caplogo"));
        assertEquals(Path.of("/d/re/front-end/images/a.png"), p.frontEndImage("a.png"));
    }

    @Test
    void frontEndIsMissingInAnEmptyRoot(@TempDir Path dir) {
        assertFalse(new DataPaths(dir).frontEndExists());
    }

    @Test
    void realFrontEndExists() {
        Assumptions.assumeTrue(DataPaths.defaultRootExists());
        DataPaths p = DataPaths.resolve(new String[0]);
        assertTrue(p.frontEndExists());
        assertTrue(Files.isRegularFile(p.movieFrame("x6op", 1517)));
    }

    @Test
    void defaultIsHomeMmxData() {
        assertEquals(Path.of(System.getProperty("user.home"), "mmx-data", "x6"),
                DataPaths.resolve(new String[0]).root());
    }

    @Test
    void validateNamesTheMissingPath(@TempDir Path dir) {
        DataRootException e = assertThrows(DataRootException.class, () -> new DataPaths(dir).validate());
        assertTrue(e.getMessage().contains(dir.toString()));
        assertTrue(e.getMessage().contains("intro.tmx"));
    }

    @Test
    void pathsUnderTheRoot() {
        DataPaths p = new DataPaths(Path.of("/r"));
        assertEquals(Path.of("/r/tmx/m094-intro/tiles.png"), p.tilesPng());
        assertEquals(Path.of("/r/sprites/players/x-falcon"), p.playerDir("x-falcon"));
        assertEquals(Path.of("/r/sprites/enemies/junkroid"), p.enemyDir("junkroid"));
        assertEquals(Path.of("/r/sprites/banks/intro/intro-1_start/bank-1.png"), p.bankSheet("intro-1_start", 1));
        assertEquals(Path.of("/r/sounds/m087-e11"), p.soundBank("m087-e11"));
        assertEquals(Path.of("/r/music/bgm/songs.json"), p.songsJson());
        assertEquals(Path.of("/r/music/bgm/c07-t02.wav"), p.musicRecord("c07-t02.wav"));
    }
}
