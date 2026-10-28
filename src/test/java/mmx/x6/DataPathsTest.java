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
    void theGameReadsTheAssetsFolder() {
        assertEquals(Path.of("assets"), DataPaths.assets().root());
        assertEquals(Path.of("assets/tiled/tmx/intro.tmx"), DataPaths.assets().tmx());
    }

    @Test
    void movieAndFrontEndPaths() {
        DataPaths p = new DataPaths(Path.of("/d"));
        assertEquals(Path.of("/d/movies/x6op/frame-0007.png"), p.movieFrame("x6op", 7));
        assertEquals(Path.of("/d/movies/caplogo/audio.wav"), p.movieAudio("caplogo"));
        assertEquals(Path.of("/d/sprites/frontend/a.png"), p.frontEndImage("a.png"));
    }

    @Test
    void frontEndIsMissingInAnEmptyRoot(@TempDir Path dir) {
        assertFalse(new DataPaths(dir).frontEndExists());
    }

    @Test
    void realFrontEndExists() {
        Assumptions.assumeTrue(DataPaths.assetsExist());
        DataPaths p = DataPaths.assets();
        assertTrue(p.frontEndExists());
        assertTrue(Files.isRegularFile(p.movieFrame("x6op", 1517)));
    }

    @Test
    void validateNamesTheMissingPathAndTheFix(@TempDir Path dir) {
        DataRootException e = assertThrows(DataRootException.class, () -> new DataPaths(dir).validate());
        assertTrue(e.getMessage().contains(dir.toString()));
        assertTrue(e.getMessage().contains("intro.tmx"));
        assertTrue(e.getMessage().contains("make assets"));
    }

    @Test
    void pathsUnderTheRoot() {
        DataPaths p = new DataPaths(Path.of("/r"));
        assertEquals(Path.of("/r/tiled/tmx/ground-scaravich.tmx"), p.tmx("ground-scaravich.tmx"));
        assertEquals(Path.of("/r/tiled/png/intro-tiles.png"), p.tilesPng());
        assertEquals(Path.of("/r/tiled/png/ground-scaravich-tiles.png"), p.tilesPng("ground-scaravich.tmx"));
        assertEquals(Path.of("/r/sprites/players/x-falcon"), p.playerDir("x-falcon"));
        assertEquals(Path.of("/r/sprites/enemies/junkroid"), p.enemyDir("junkroid"));
        assertEquals(Path.of("/r/sprites/banks/intro/intro-1_start/bank-1.png"), p.bankSheet("intro-1_start", 1));
        assertEquals(Path.of("/r/sounds/m087-e11"), p.soundBank("m087-e11"));
        assertEquals(Path.of("/r/music/songs.json"), p.songsJson());
        assertEquals(Path.of("/r/music/c07-t02.ogg"), p.musicRecord("c07-t02.ogg"));
    }
}
