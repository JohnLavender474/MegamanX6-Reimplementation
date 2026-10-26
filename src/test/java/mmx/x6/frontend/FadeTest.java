package mmx.x6.frontend;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class FadeTest {

    @Test
    void fadeOutTakesThirtyTwoUpdatesAtSpeedEight() {
        Fade f = new Fade();
        f.out(8);
        for (int i = 0; i < 31; i++) {
            f.update();
            assertTrue(f.busy(), "update " + i);
        }
        f.update();
        assertFalse(f.busy());
        assertEquals(1f, f.darkness());
    }

    @Test
    void fadeInEndsClear() {
        Fade f = new Fade();
        f.in(8);
        assertEquals(1f, f.darkness());
        for (int i = 0; i < 32; i++) {
            f.update();
        }
        assertFalse(f.busy());
        assertEquals(0f, f.darkness());
    }

    @Test
    void clearStopsAFadeAtNoDarkness() {
        Fade f = new Fade();
        f.out(8);
        f.update();
        f.clear();
        assertFalse(f.busy());
        assertEquals(0f, f.darkness());
    }

    @Test
    void newFadeIsClearAndIdle() {
        Fade f = new Fade();
        assertFalse(f.busy());
        assertEquals(0f, f.darkness());
    }
}
