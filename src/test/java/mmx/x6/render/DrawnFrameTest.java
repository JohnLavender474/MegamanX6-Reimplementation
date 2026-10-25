package mmx.x6.render;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;

import org.junit.jupiter.api.Test;

class DrawnFrameTest {

    @Test
    void airDashStartDrawsTheFirstAirDashFrame() {
        assertArrayEquals(new int[] {171, 0}, Assets.drawnFrame("x-falcon", 169, 3));
    }

    @Test
    void otherAnimationsAreDrawnAsThemselves() {
        assertArrayEquals(new int[] {171, 2}, Assets.drawnFrame("x-falcon", 171, 2));
        assertArrayEquals(new int[] {169, 1}, Assets.drawnFrame("x-sabre", 169, 1));
    }
}
