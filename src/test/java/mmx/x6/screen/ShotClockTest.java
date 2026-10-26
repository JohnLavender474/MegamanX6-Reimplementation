package mmx.x6.screen;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class ShotClockTest {

    @Test
    void eachTimeGivesOneShotThenTheClockFinishes() {
        ShotClock c = new ShotClock("/s/a.png", "30,60");
        assertTrue(c.active());
        assertNull(c.due(29));
        assertEquals("/s/a-30.png", c.due(30));
        assertNull(c.due(31));
        assertFalse(c.finished());
        assertEquals("/s/a-60.png", c.due(60));
        assertTrue(c.finished());
        assertNull(c.due(61));
    }

    @Test
    void noPathMeansNoShots() {
        ShotClock c = new ShotClock(null, "30");
        assertFalse(c.active());
        assertNull(c.due(30));
        assertFalse(c.finished());
    }
}
