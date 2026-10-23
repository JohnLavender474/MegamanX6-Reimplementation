package mmx.x6.physics;

import static mmx.x6.physics.Grids.grid;
import static mmx.x6.physics.Grids.xBody;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import mmx.x6.level.CollisionGrid;
import org.junit.jupiter.api.Test;

class TerrainMoverTest {

    private static final List<Rect> NONE = List.of();

    private static boolean has(int flags, int bit) {
        return (flags & bit) != 0;
    }

    @Test
    void landsOnFlatFloor() {
        CollisionGrid g = grid(
                "........",
                "........",
                "........",
                "........",
                "########");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(64, 20);
        int flags = 0;
        for (int i = 0; i < 20 && !has(flags, TerrainMover.FLOOR); i++) {
            flags = m.move(b, 0, 4, false, NONE);
        }
        assertTrue(has(flags, TerrainMover.FLOOR));
        assertEquals(64, b.bottom(), 0.001);
    }

    @Test
    void stopsAtWallAndFlagsCling() {
        CollisionGrid g = grid(
                ".....#..",
                ".....#..",
                ".....#..",
                ".....#..",
                "########");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(50, 43);
        int flags = 0;
        for (int i = 0; i < 20 && !has(flags, TerrainMover.RIGHT); i++) {
            flags = m.move(b, 2, 0, true, NONE);
        }
        assertTrue(has(flags, TerrainMover.RIGHT));
        assertEquals(80, b.right(), 0.001);
        assertTrue(m.lastWallCling);
    }

    @Test
    void noClingOn57() {
        CollisionGrid g = grid(
                ".....n..",
                ".....n..",
                ".....n..",
                ".....n..",
                "########");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(50, 43);
        int flags = 0;
        for (int i = 0; i < 20 && !has(flags, TerrainMover.RIGHT); i++) {
            flags = m.move(b, 2, 0, true, NONE);
        }
        assertTrue(has(flags, TerrainMover.RIGHT));
        assertFalse(m.lastWallCling);
    }

    @Test
    void walksUpAFourCellSlope() {
        CollisionGrid g = grid(
                "..........",
                "..........",
                "..........",
                "..........",
                "..////####",
                "##uuuu####");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(12, 80 - 21);
        for (int i = 0; i < 40; i++) {
            int flags = m.move(b, 2, 0.5f, true, List.of());
            assertTrue(has(flags, TerrainMover.FLOOR), "update " + i);
            assertFalse(has(flags, TerrainMover.RIGHT), "update " + i);
            float cx = b.x;
            int cell = (int) Math.floor(cx / 16);
            if (cell >= 2 && cell <= 5) {
                int v = g.raw(cell, 4);
                float surface = 64 + TileRules.slopeSurface(v, (int) Math.floor(cx) - cell * 16);
                assertEquals(surface, b.bottom(), 1.0, "update " + i);
            }
        }
        assertTrue(b.x > 80);
        assertEquals(64, b.bottom(), 0.001);
    }

    @Test
    void staysOnSlopeGoingDown() {
        CollisionGrid g = grid(
                "..........",
                "..........",
                "..........",
                "..........",
                "####\\\\\\\\..",
                "####uuuu##");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(40, 64 - 21);
        for (int i = 0; i < 45; i++) {
            int flags = m.move(b, 2, 0.5f, true, NONE);
            assertTrue(has(flags, TerrainMover.FLOOR), "update " + i);
        }
        assertEquals(130, b.x, 0.001);
        assertEquals(80, b.bottom(), 0.001);
    }

    @Test
    void cellUnderSlopeUsesCellAbove() {
        CollisionGrid g = grid(
                "......",
                "......",
                "......",
                "..//..",
                "##uu##");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(56, 64 - 21);
        int flags = m.move(b, 0, 0.5f, true, NONE);
        assertTrue(has(flags, TerrainMover.FLOOR));
        assertEquals(48 + TileRules.slopeSurface(2, 8), b.bottom(), 0.001);
    }

    @Test
    void slopeIsNotACeiling() {
        CollisionGrid g = grid(
                "......",
                "..//..",
                "......",
                "......",
                "......",
                "######");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(40, 60);
        int flags = m.move(b, 0, -20, false, NONE);
        assertFalse(has(flags, TerrainMover.CEILING));
        assertEquals(40, b.y, 0.001);
    }

    @Test
    void oneWayFloorFromAboveOnly() {
        CollisionGrid g = grid(
                "......",
                "......",
                "......",
                "TTTTTT",
                "......",
                "......",
                "......",
                "######");
        TerrainMover m = new TerrainMover(g);
        Body up = xBody(40, 100);
        int flags = m.move(up, 0, -6, false, NONE);
        assertFalse(has(flags, TerrainMover.CEILING));
        Body b = xBody(40, 20);
        flags = 0;
        for (int i = 0; i < 20 && !has(flags, TerrainMover.FLOOR); i++) {
            flags = m.move(b, 0, 4, false, NONE);
        }
        assertEquals(48, b.bottom(), 0.001);
        Body rising = xBody(40, 60);
        for (int i = 0; i < 4; i++) {
            m.move(rising, 0, -5, false, NONE);
        }
        flags = m.move(rising, 0, 1, false, NONE);
        assertFalse(has(flags, TerrainMover.FLOOR));
    }

    @Test
    void spikeFlag() {
        CollisionGrid g = grid(
                "......",
                "......",
                "......",
                "^^^^^^");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(40, 20);
        int flags = 0;
        for (int i = 0; i < 20 && !has(flags, TerrainMover.FLOOR); i++) {
            flags = m.move(b, 0, 4, false, NONE);
        }
        assertTrue(has(flags, TerrainMover.FLOOR));
        assertTrue(m.lastSpike);
    }

    @Test
    void solidRectIsAWallAndAFloor() {
        CollisionGrid g = grid(
                "............",
                "............",
                "............",
                "............",
                "............",
                "############");
        TerrainMover m = new TerrainMover(g);
        List<Rect> solids = List.of(new Rect(96, 16, 64, 64));
        Body b = xBody(60, 80 - 21);
        int flags = 0;
        for (int i = 0; i < 30 && !has(flags, TerrainMover.RIGHT); i++) {
            flags = m.move(b, 2, 0.5f, true, solids);
        }
        assertTrue(has(flags, TerrainMover.RIGHT));
        assertEquals(96, b.right(), 0.001);
        assertTrue(m.lastWallCling);
        Body top = xBody(128, -20);
        flags = 0;
        for (int i = 0; i < 20 && !has(flags, TerrainMover.FLOOR); i++) {
            flags = m.move(top, 0, 4, false, solids);
        }
        assertEquals(16, top.bottom(), 0.001);
    }

    @Test
    void blockedProbe() {
        CollisionGrid g = grid(
                ".....#..",
                ".....#..",
                ".....#..",
                ".....#..",
                "########");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(69, 43);
        assertTrue(m.blocked(b, TerrainMover.RIGHT, NONE));
        assertFalse(m.blocked(b, TerrainMover.LEFT, NONE));
        assertEquals(69, b.x, 0.001);
    }

    @Test
    void blockedProbeReportsCling() {
        CollisionGrid g = grid(
                "n....#..",
                "n....#..",
                "n....#..",
                "n....#..",
                "########");
        TerrainMover m = new TerrainMover(g);
        assertTrue(m.blocked(xBody(69, 43), TerrainMover.RIGHT, NONE));
        assertTrue(m.lastProbeCling);
        assertTrue(m.blocked(xBody(27, 43), TerrainMover.LEFT, NONE));
        assertFalse(m.lastProbeCling);
    }

    @Test
    void groundedDoesNotSnapDownASixteenPixelStep() {
        CollisionGrid g = grid(
                "........",
                "........",
                "........",
                "##......",
                "########");
        TerrainMover m = new TerrainMover(g);
        Body b = xBody(46, 48 - 21);
        int flags = m.move(b, 2, 1, true, NONE);
        assertFalse(has(flags, TerrainMover.FLOOR));
    }

    @Test
    void onFloorProbeDoesNotMove() {
        CollisionGrid g = grid(
                "........",
                "........",
                "........",
                "........",
                "########");
        TerrainMover m = new TerrainMover(g);
        Body standing = xBody(40, 64 - 21);
        assertTrue(m.onFloor(standing, NONE));
        assertEquals(43, standing.y, 0.001);
        Body air = xBody(40, 20);
        assertFalse(m.onFloor(air, NONE));
        assertEquals(20, air.y, 0.001);
    }
}
