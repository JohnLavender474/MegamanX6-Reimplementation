package mmx.x6;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;

class AutoRunTest {

    @AfterEach
    void clear() {
        System.clearProperty("mmx.shot");
    }

    @Test
    void aShotRunIsAutomated() {
        System.setProperty("mmx.shot", "/tmp/x.png");
        assertTrue(AutoRun.active());
        assertEquals("AUTOMATED RUN! NO INPUT", AutoRun.title("MMX6"));
    }

    @Test
    void aNormalRunKeepsItsTitle() {
        assertFalse(AutoRun.active());
        assertEquals("MMX6", AutoRun.title("MMX6"));
    }
}
