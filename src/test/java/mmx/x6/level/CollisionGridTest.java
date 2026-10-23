package mmx.x6.level;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class CollisionGridTest {

    private static CollisionGrid grid() {
        int[] v = new int[4 * 2];
        v[1] = 59;
        v[4 + 2] = 56;
        return new CollisionGrid(4, 2, v);
    }

    @Test
    void outsideIsZero() {
        CollisionGrid g = grid();
        assertEquals(0, g.at(-1, 0));
        assertEquals(0, g.at(16 * 4, 0));
        assertEquals(0, g.at(0, -0.5f));
        assertEquals(0, g.raw(9, 9));
    }

    @Test
    void gateReadsAsSolidThroughAt() {
        CollisionGrid g = grid();
        assertEquals(56, g.at(20, 3));
        assertEquals(59, g.raw(1, 0));
        assertEquals(56, g.at(40, 20));
    }

    @Test
    void restoreFromCopiesTheValuesBack() {
        CollisionGrid g = grid();
        CollisionGrid pristine = g.copy();
        g.set(0, 0, 63);
        g.restoreFrom(pristine);
        assertEquals(0, g.raw(0, 0));
        assertEquals(59, g.raw(1, 0));
    }

    @Test
    void copyIsIndependent() {
        CollisionGrid g = grid();
        g.set(3, 1, 63);
        CollisionGrid c = g.copy();
        assertEquals(63, c.raw(3, 1));
        c.set(3, 1, 0);
        assertEquals(63, g.raw(3, 1));
    }
}
