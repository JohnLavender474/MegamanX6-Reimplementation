package mmx.x6.frontend;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import com.badlogic.gdx.Input.Keys;
import java.util.ArrayList;
import java.util.List;
import mmx.x6.audio.Sfx;
import mmx.x6.input.Bindings;
import mmx.x6.input.Buttons;
import mmx.x6.input.FakePad;
import mmx.x6.input.PadButton;
import mmx.x6.input.RawInput;
import org.junit.jupiter.api.Test;

class OptionsMenuTest {

    private final List<String> sounds = new ArrayList<>();
    private final Sfx sfx = new Sfx() {
        @Override
        public void play(int bank, int id) {
            sounds.add(bank + "/" + id);
        }

        @Override
        public void stop(int bank, int id) {
        }
    };
    private final Bindings bindings = new Bindings();
    private final OptionsMenu menu = new OptionsMenu(sfx, bindings);
    private FakePad pad;

    private RawInput raw(int key, int padButton) {
        return new RawInput(key, padButton, pad);
    }

    private boolean press(int buttons) {
        return menu.update(buttons, raw(Bindings.NONE, Bindings.NONE));
    }

    private void idle(int n) {
        for (int i = 0; i < n; i++) {
            press(0);
        }
    }

    private void down(int rows) {
        for (int i = 0; i < rows; i++) {
            press(Buttons.DOWN);
        }
    }

    @Test
    void optionsRowsAndBack() {
        assertEquals(OptionsMenu.Page.OPTIONS, menu.page());
        assertEquals(List.of("BACK", "CONTROLLER SETTINGS", "KEYBOARD SETTINGS"), menu.rows());
        press(Buttons.UP);
        assertEquals(2, menu.row());
        assertEquals(List.of("0/12"), sounds);
        press(Buttons.DOWN);
        assertEquals(0, menu.row());
        assertTrue(press(Buttons.JUMP), "BACK goes to the main menu");
        assertEquals("0/34", sounds.get(sounds.size() - 1));
    }

    @Test
    void keyboardSettingsListEveryPadButton() {
        down(2);
        assertFalse(press(Buttons.START));
        assertEquals(OptionsMenu.Page.KEYBOARD, menu.page());
        assertEquals(0, menu.row());
        List<String> rows = menu.rows();
        assertEquals(2 + PadButton.values().length, rows.size());
        assertEquals("BACK", rows.get(0));
        assertEquals("RESET TO DEFAULTS", rows.get(1));
        assertEquals("CROSS (JUMP): K", rows.get(2 + PadButton.CROSS.ordinal()));
        assertEquals("START: Enter", rows.get(2 + PadButton.START.ordinal()));
        press(Buttons.START);
        assertEquals(OptionsMenu.Page.OPTIONS, menu.page());
        assertEquals(2, menu.row(), "BACK returns to the row that opened the page");
    }

    @Test
    void rebindAKeyThenWaitBeforeTheMenuReadsInputAgain() {
        down(2);
        press(Buttons.START);
        down(2 + PadButton.CROSS.ordinal());
        assertFalse(menu.update(Buttons.START, raw(Keys.ENTER, Bindings.NONE)));
        assertEquals(PadButton.CROSS, menu.waiting());
        assertEquals(Keys.K, bindings.key(PadButton.CROSS), "the press that picks the row is not bound");
        idle(30);
        assertEquals(PadButton.CROSS, menu.waiting());
        menu.update(0, raw(Keys.L, Bindings.NONE));
        assertNull(menu.waiting());
        assertEquals(Keys.L, bindings.key(PadButton.CROSS));
        assertEquals(Keys.K, bindings.key(PadButton.CIRCLE), "the two rows swap");
        int row = menu.row();
        for (int i = 0; i < OptionsMenu.REBIND_DELAY; i++) {
            press(Buttons.DOWN);
        }
        assertEquals(row, menu.row());
        press(0);
        press(Buttons.DOWN);
        assertEquals(row + 1, menu.row());
    }

    @Test
    void resetKeysToDefaults() {
        bindings.bindKey(PadButton.START, Keys.SPACE);
        down(2);
        press(Buttons.START);
        down(1);
        press(Buttons.START);
        assertEquals(Keys.ENTER, bindings.key(PadButton.START));
    }

    @Test
    void controllerSettingsNeedAController() {
        down(1);
        press(Buttons.START);
        assertEquals(OptionsMenu.Page.OPTIONS, menu.page());
        assertEquals("0/" + OptionsMenu.SOUND_ERROR, sounds.get(sounds.size() - 1));
        assertNotNull(menu.message());
        idle(OptionsMenu.MESSAGE_UPDATES);
        assertNull(menu.message());
    }

    @Test
    void rebindAControllerButton() {
        pad = new FakePad("pad");
        down(1);
        press(Buttons.START);
        assertEquals(OptionsMenu.Page.CONTROLLER, menu.page());
        assertEquals("CROSS (JUMP): CROSS", menu.rows().get(2 + PadButton.CROSS.ordinal()));
        down(2 + PadButton.CROSS.ordinal());
        press(Buttons.START);
        menu.update(0, raw(Keys.L, Bindings.NONE));
        assertEquals(PadButton.CROSS, menu.waiting(), "a key does not bind on the controller page");
        menu.update(0, raw(Bindings.NONE, 12));
        assertNull(menu.waiting());
        assertEquals(12, bindings.padCode(pad, PadButton.CROSS));
        assertEquals("CROSS (JUMP): BUTTON 12", menu.rows().get(2 + PadButton.CROSS.ordinal()));
        menu.update(0, raw(Bindings.NONE, Bindings.NONE));
        bindings.bindPad(pad, PadButton.CROSS, PadButton.SQUARE.ordinal());
        assertEquals("CROSS (JUMP): SQUARE", menu.rows().get(2 + PadButton.CROSS.ordinal()));
    }

    @Test
    void unplugOnTheControllerPageGoesBack() {
        pad = new FakePad("pad");
        down(1);
        press(Buttons.START);
        down(3);
        press(Buttons.START);
        assertNotNull(menu.waiting());
        pad = null;
        press(0);
        assertEquals(OptionsMenu.Page.OPTIONS, menu.page());
        assertEquals(1, menu.row());
        assertNull(menu.waiting());
    }
}
