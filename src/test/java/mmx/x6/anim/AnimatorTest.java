package mmx.x6.anim;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class AnimatorTest {

    private static final String JSON = """
            {"cell": [10, 10], "origin": [5, 5], "animations": [
              {"index": 1, "loop_to": null, "frames": [
                {"sprite": 1, "ticks": 6, "flag": 0},
                {"sprite": 2, "ticks": 4, "flag": 0},
                {"sprite": 3, "ticks": 1, "flag": 1}]},
              {"index": 2, "loop_to": 1, "frames": [
                {"sprite": 4, "ticks": 1, "flag": 0},
                {"sprite": 5, "ticks": 1, "flag": 0},
                {"sprite": 6, "ticks": 1, "flag": 0}]}
            ]}
            """;

    @Test
    void animatorTicksDurations() {
        Animator a = new Animator(AnimSet.parse(JSON));
        a.play(1);
        for (int i = 0; i < 6; i++) {
            assertEquals(1, a.sprite());
            a.tick();
        }
        assertEquals(2, a.sprite());
        assertFalse(a.flagEntered());
        for (int i = 0; i < 4; i++) {
            a.tick();
        }
        assertEquals(3, a.sprite());
        assertTrue(a.flagEntered());
        assertEquals(1, a.flag());
        assertTrue(a.atLastFrame());
        assertFalse(a.finished());
        a.tick();
        assertTrue(a.finished());
        assertFalse(a.flagEntered());
        assertEquals(3, a.sprite());
    }

    @Test
    void animatorLoopsTo() {
        Animator a = new Animator(AnimSet.parse(JSON));
        a.play(2);
        a.tick();
        a.tick();
        assertEquals(6, a.sprite());
        a.tick();
        assertEquals(5, a.sprite());
        assertEquals(1, a.frameIndex());
        assertFalse(a.finished());
    }

    @Test
    void enteredTestsAMask() {
        String json = """
                {"cell": [1, 1], "origin": [0, 0], "animations": [
                  {"index": 0, "loop_to": null, "frames": [
                    {"sprite": 0, "ticks": 1, "flag": 1},
                    {"sprite": 1, "ticks": 1, "flag": 129}]}]}
                """;
        Animator a = new Animator(AnimSet.parse(json));
        a.play(0);
        assertFalse(a.entered(0x80));
        a.tick();
        assertTrue(a.entered(0x80));
        assertFalse(a.entered(0x40));
        a.tick();
        assertFalse(a.entered(0x80));
    }

    @Test
    void playAtFrame() {
        Animator a = new Animator(AnimSet.parse(JSON));
        a.play(1, 1);
        assertEquals(2, a.sprite());
        assertEquals(1, a.anim());
    }

    @Test
    void parsesCellAndOrigin() {
        AnimSet s = AnimSet.parse(JSON);
        assertEquals(10, s.cellW());
        assertEquals(5, s.originY());
        assertEquals(3, s.get(1).frames().size());
        assertEquals(null, s.get(7));
    }
}
