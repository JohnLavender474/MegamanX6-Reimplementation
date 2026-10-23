package mmx.x6.game;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class StepClockTest {

    @Test
    void stepClockCapsAtFour() {
        StepClock c = new StepClock();
        assertEquals(4, c.advance(1.0f));
        assertEquals(0, c.advance(0.001f));
    }

    @Test
    void stepClockSixtyPerSecond() {
        StepClock c = new StepClock();
        int total = 0;
        for (int i = 0; i < 60; i++) {
            total += c.advance(1f / 60f);
        }
        assertEquals(60, total);
    }

    @Test
    void slowFramesRunTwoUpdates() {
        StepClock c = new StepClock();
        assertEquals(2, c.advance(1f / 30f));
    }
}
