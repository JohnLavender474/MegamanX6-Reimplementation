package mmx.x6.input;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class ButtonsTest {

    @Test
    void buttonsPressedAndReleased() {
        Buttons b = new Buttons();
        b.set(Buttons.JUMP);
        assertEquals(Buttons.JUMP, b.pressed() & Buttons.JUMP);
        assertEquals(Buttons.JUMP, b.held() & Buttons.JUMP);
        b.set(Buttons.JUMP);
        assertEquals(0, b.pressed());
        b.set(0);
        assertEquals(Buttons.JUMP, b.released());
        b.set(0);
        assertEquals(0, b.released());
    }

    @Test
    void oppositeDirectionsCancel() {
        Buttons b = new Buttons();
        b.set(Buttons.LEFT | Buttons.RIGHT | Buttons.UP);
        assertEquals(Buttons.UP, b.held());
    }

    @Test
    void scriptedHeld() {
        Buttons b = new Buttons();
        b.set(Buttons.RIGHT | Buttons.JUMP);
        assertEquals(Buttons.RIGHT | Buttons.JUMP, b.pressed());
        b.set(Buttons.RIGHT);
        assertEquals(Buttons.JUMP, b.released());
    }
}
