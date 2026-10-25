package mmx.x6.game.player;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class BusterPointsTest {

    @Test
    void standSprite() {
        assertEquals(21, BusterPoints.forward(88, 85));
        assertEquals(-9, BusterPoints.down(88, 85));
    }

    @Test
    void dashSprite() {
        assertEquals(42, BusterPoints.forward(128, 72));
        assertEquals(5, BusterPoints.down(128, 72));
    }

    @Test
    void frameSet1Sprite() {
        assertEquals(26, BusterPoints.forward(144, 3));
        assertEquals(-10, BusterPoints.down(144, 3));
    }

    @Test
    void spriteWithNoPointUsesTheStandPoint() {
        assertEquals(21, BusterPoints.forward(5, 0));
        assertEquals(-9, BusterPoints.down(5, 0));
    }
}
