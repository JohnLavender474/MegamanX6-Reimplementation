package mmx.x6.input;

import static org.junit.jupiter.api.Assertions.assertEquals;

import com.badlogic.gdx.Input.Keys;
import java.util.Set;
import org.junit.jupiter.api.Test;

class BindingsTest {

    @Test
    void defaultKeysAreTheOldKeyMap() {
        Bindings b = new Bindings();
        assertEquals(Buttons.BUSTER | Buttons.DASH | Buttons.WEAPON | Buttons.DOWN | Buttons.RIGHT,
                b.keyHeld(Set.of(Keys.J, Keys.L, Keys.I, Keys.S, Keys.D)::contains));
        assertEquals(Buttons.JUMP | Buttons.WEAPON_PREV | Buttons.WEAPON_NEXT | Buttons.UP | Buttons.LEFT,
                b.keyHeld(Set.of(Keys.K, Keys.Q, Keys.O, Keys.W, Keys.A)::contains));
        assertEquals(Buttons.START, b.keyHeld(k -> k == Keys.ENTER));
    }

    @Test
    void aKeyBoundToAnotherButtonSwaps() {
        Bindings b = new Bindings();
        b.bindKey(PadButton.CROSS, Keys.L);
        assertEquals(Keys.L, b.key(PadButton.CROSS));
        assertEquals(Keys.K, b.key(PadButton.CIRCLE));
        assertEquals(Buttons.JUMP, b.keyHeld(k -> k == Keys.L));
        b.bindKey(PadButton.CROSS, Keys.SPACE);
        assertEquals(Buttons.JUMP, b.keyHeld(k -> k == Keys.SPACE));
        assertEquals(0, b.keyHeld(k -> k == Keys.L));
    }

    @Test
    void resetKeys() {
        Bindings b = new Bindings();
        b.bindKey(PadButton.START, Keys.SPACE);
        b.resetKeys();
        assertEquals(Keys.ENTER, b.key(PadButton.START));
    }

    @Test
    void padStartsWithItsDefaultLayout() {
        Bindings b = new Bindings();
        FakePad pad = new FakePad("pad");
        pad.down.add(PadButton.CROSS.ordinal());
        pad.down.add(PadButton.L1.ordinal());
        assertEquals(Buttons.JUMP | Buttons.WEAPON_PREV, b.padHeld(pad));
        assertEquals(0, b.padHeld(null));
    }

    @Test
    void padRebindSwapsAndIsKeptByName() {
        Bindings b = new Bindings();
        FakePad pad = new FakePad("pad");
        b.bindPad(pad, PadButton.CROSS, PadButton.SQUARE.ordinal());
        assertEquals(PadButton.SQUARE.ordinal(), b.padCode(pad, PadButton.CROSS));
        assertEquals(PadButton.CROSS.ordinal(), b.padCode(pad, PadButton.SQUARE));
        FakePad again = new FakePad("pad");
        assertEquals(PadButton.SQUARE.ordinal(), b.padCode(again, PadButton.CROSS));
        FakePad other = new FakePad("other");
        assertEquals(PadButton.CROSS.ordinal(), b.padCode(other, PadButton.CROSS));
        b.resetPad(pad);
        assertEquals(PadButton.CROSS.ordinal(), b.padCode(pad, PadButton.CROSS));
    }
}
