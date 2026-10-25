package mmx.x6.game.enemy;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class BossGateTableTest {

    @Test
    void yammarkGates() {
        assertArrayEquals(new int[] {4800, 4800, 1904, 1904}, BossGate.limits(67 & 0x3F, 4848, 1968));
        assertEquals(0.828125f, BossGate.push(3));
        assertArrayEquals(new int[] {5104, 5136, -1, -1}, BossGate.limits(4, 5104, 2016));
    }

    @Test
    void turtloidBossGate() {
        assertArrayEquals(new int[] {8944, 9040, 640, 688}, BossGate.limits(23, 8944, 688));
        assertArrayEquals(new int[] {8624, 8624, -1, -1}, BossGate.limits(34, 8624, 688));
    }

    @Test
    void heatnixGates() {
        assertArrayEquals(new int[] {944, 976, 448, 2624}, BossGate.limits(7, 944, 608));
        assertArrayEquals(new int[] {2544, 2544, 944, -1}, BossGate.limits(10, 2736, 2976));
        assertEquals(3.84375f, BossGate.push(12));
        assertArrayEquals(new int[] {4112, 4144, 944, 944}, BossGate.limits(16, 4112, 1104));
    }

    @Test
    void introGatesKeepTheirValues() {
        assertArrayEquals(new int[] {5744, 5744, 224, 224}, BossGate.limits(2, 5744, 384));
        assertArrayEquals(new int[] {6064, 6144, -1, -1}, BossGate.limits(1, 6064, 384));
    }
}
