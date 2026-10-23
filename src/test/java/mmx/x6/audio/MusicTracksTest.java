package mmx.x6.audio;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.util.Map;
import mmx.x6.DataPaths;
import org.junit.jupiter.api.Test;

class MusicTracksTest {

    private static Map<Integer, MusicTrack> tracks() {
        assumeTrue(DataPaths.defaultRootExists());
        return MusicTracks.load(DataPaths.resolve(new String[0]));
    }

    @Test
    void stageTrack() {
        MusicTrack t = tracks().get(9);
        assertEquals("c07-t02.wav", t.record());
        assertTrue(t.loop());
        assertEquals(359.52, t.end(), 0.01);
    }

    @Test
    void bossTrack() {
        assertEquals("c05-t02.wav", tracks().get(21).record());
    }

    @Test
    void gateTrack() {
        assertEquals("c03-t02.wav", tracks().get(28).record());
    }
}
