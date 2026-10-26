package mmx.x6.screen;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class ShotsTest {

    @Test
    void timesAreAList() {
        assertArrayEquals(new int[] {30, 600, 900}, Shots.times("30, 600,900"));
    }

    @Test
    void oneTimeKeepsThePath() {
        assertEquals("/s/a.png", Shots.path("/s/a.png", new int[] {30}, 0));
    }

    @Test
    void severalTimesNumberThePaths() {
        assertEquals("/s/a-900.png", Shots.path("/s/a.png", new int[] {30, 900}, 1));
    }
}
