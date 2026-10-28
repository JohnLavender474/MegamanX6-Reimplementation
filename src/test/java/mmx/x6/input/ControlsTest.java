package mmx.x6.input;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertSame;

import com.badlogic.gdx.Input.Keys;
import org.junit.jupiter.api.Test;

class ControlsTest {

    private Pad plugged;
    private final Controls controls = new Controls(new Bindings(), () -> plugged);

    @Test
    void keyboardAndPadWorkTogether() {
        FakePad pad = new FakePad("pad");
        plugged = pad;
        pad.down.add(PadButton.CIRCLE.ordinal());
        controls.poll();
        assertEquals(Buttons.DASH | Buttons.JUMP, controls.held(k -> k == Keys.K));
    }

    @Test
    void unplugAndReplugKeepsTheRebinds() {
        FakePad pad = new FakePad("pad");
        plugged = pad;
        controls.poll();
        controls.bindings().bindPad(pad, PadButton.CROSS, 9);
        pad.down.add(9);
        assertEquals(Buttons.JUMP, controls.held(k -> false));
        plugged = null;
        controls.poll();
        assertNull(controls.pad());
        assertEquals(0, controls.held(k -> false));
        FakePad again = new FakePad("pad");
        again.down.add(9);
        plugged = again;
        controls.poll();
        assertSame(again, controls.pad());
        assertEquals(Buttons.JUMP, controls.held(k -> false));
    }

    @Test
    void padPressIsTheFirstButtonThatWentDownThisFrame() {
        FakePad pad = new FakePad("pad");
        plugged = pad;
        controls.poll();
        assertEquals(Bindings.NONE, controls.padPressed());
        pad.down.add(7);
        controls.poll();
        assertEquals(7, controls.padPressed());
        controls.poll();
        assertEquals(Bindings.NONE, controls.padPressed(), "a held button is not a new press");
        pad.down.add(3);
        controls.poll();
        assertEquals(3, controls.padPressed());
    }

    @Test
    void buttonsHeldWhenThePadIsPluggedInAreNotPresses() {
        FakePad pad = new FakePad("pad");
        pad.down.add(5);
        plugged = pad;
        controls.poll();
        assertEquals(Bindings.NONE, controls.padPressed());
    }
}
