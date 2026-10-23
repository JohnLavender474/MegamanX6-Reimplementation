package mmx.x6.physics;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class TileRulesTest {

    @Test
    void slopeProfiles() {
        assertEquals(15, TileRules.slopeSurface(1, 0));
        assertEquals(12, TileRules.slopeSurface(1, 15));
        assertEquals(3, TileRules.slopeSurface(4, 0));
        assertEquals(0, TileRules.slopeSurface(4, 15));
        assertEquals(0, TileRules.slopeSurface(5, 0));
        assertEquals(3, TileRules.slopeSurface(5, 15));
        assertEquals(12, TileRules.slopeSurface(8, 0));
        assertEquals(15, TileRules.slopeSurface(8, 15));
        assertEquals(15, TileRules.slopeSurface(9, 0));
        assertEquals(8, TileRules.slopeSurface(9, 15));
        assertEquals(7, TileRules.slopeSurface(10, 0));
        assertEquals(0, TileRules.slopeSurface(10, 15));
        assertEquals(0, TileRules.slopeSurface(11, 0));
        assertEquals(7, TileRules.slopeSurface(11, 15));
        assertEquals(TileRules.slopeSurface(3, 9), TileRules.slopeSurface(19, 9));
    }

    @Test
    void slopesAreNeverWalls() {
        for (int v = 1; v <= 12; v++) {
            assertFalse(TileRules.isWall(v));
            assertFalse(TileRules.isCeiling(v));
            assertTrue(TileRules.isSlope(v));
        }
        assertFalse(TileRules.isSlope(16));
    }

    @Test
    void wallCling() {
        assertTrue(TileRules.allowsWallCling(56));
        assertTrue(TileRules.allowsWallCling(58));
        assertFalse(TileRules.allowsWallCling(57));
        assertFalse(TileRules.allowsWallCling(60));
    }

    @Test
    void valueSets() {
        assertTrue(TileRules.isFlatFloor(33));
        assertTrue(TileRules.isOneWay(34));
        assertFalse(TileRules.isWall(33));
        assertTrue(TileRules.isSpike(63));
        assertTrue(TileRules.isWall(63));
        assertTrue(TileRules.isObjectSolid(16));
        assertTrue(TileRules.isObjectSolid(4));
        assertFalse(TileRules.isObjectSolid(1));
        assertFalse(TileRules.isObjectSolid(20));
        assertTrue(TileRules.isLadder(32));
        assertTrue(TileRules.isLadderTop(33));
        assertTrue(TileRules.isBar(45));
    }

    @Test
    void docBoxMirrorsWhenFacingRight() {
        Rect left = Rect.ofDoc(100, 50, -9, -9, 23, 18, false);
        assertEquals(91, left.x());
        assertEquals(114, left.right());
        Rect right = Rect.ofDoc(100, 50, -9, -9, 23, 18, true);
        assertEquals(86, right.x());
        assertEquals(109, right.right());
        assertEquals(41, right.y());
    }
}
