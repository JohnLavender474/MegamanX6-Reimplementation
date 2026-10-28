package mmx.x6.frontend;

import com.badlogic.gdx.Input.Keys;
import java.util.ArrayList;
import java.util.List;
import mmx.x6.audio.Sfx;
import mmx.x6.input.Bindings;
import mmx.x6.input.Buttons;
import mmx.x6.input.Pad;
import mmx.x6.input.PadButton;
import mmx.x6.input.RawInput;

public final class OptionsMenu {

    public enum Page { OPTIONS, KEYBOARD, CONTROLLER }

    public static final int REBIND_DELAY = 15;
    public static final int MESSAGE_UPDATES = 120;
    public static final int SOUND_ERROR = 6;
    private static final int PICK = Buttons.START | Buttons.JUMP;
    private static final int ROW_BACK = 0;
    private static final int ROW_CONTROLLER = 1;
    private static final int ROW_KEYBOARD = 2;
    private static final int ROW_RESET = 1;
    private static final int FIRST_BUTTON_ROW = 2;
    private static final PadButton[] BUTTONS = PadButton.values();

    private final Sfx sfx;
    private final Bindings bindings;
    private Page page;
    private int row;
    private PadButton waiting;
    private boolean waitFresh;
    private int delay;
    private String message;
    private int messageTimer;
    private Pad pad;

    public OptionsMenu(Sfx sfx, Bindings bindings) {
        this.sfx = sfx;
        this.bindings = bindings;
        open();
    }

    public void open() {
        page = Page.OPTIONS;
        row = 0;
        waiting = null;
        delay = 0;
        message = null;
    }

    public Page page() {
        return page;
    }

    public int row() {
        return row;
    }

    public PadButton waiting() {
        return waiting;
    }

    public String message() {
        return message;
    }

    public boolean update(int pressed, RawInput raw) {
        pad = raw.pad();
        if (messageTimer > 0 && --messageTimer == 0) {
            message = null;
        }
        if (page == Page.CONTROLLER && pad == null) {
            waiting = null;
            delay = 0;
            showPage(Page.OPTIONS, ROW_CONTROLLER);
            return false;
        }
        if (waiting != null) {
            if (waitFresh) {
                waitFresh = false;
            } else {
                bindFrom(raw);
            }
            return false;
        }
        if (delay > 0) {
            delay--;
            return false;
        }
        int rows = rowCount();
        if ((pressed & Buttons.UP) != 0) {
            row = (row + rows - 1) % rows;
            sfx.play(0, FrontEnd.SOUND_MOVE);
        } else if ((pressed & Buttons.DOWN) != 0) {
            row = (row + 1) % rows;
            sfx.play(0, FrontEnd.SOUND_MOVE);
        } else if ((pressed & PICK) != 0) {
            return pick();
        }
        return false;
    }

    private boolean pick() {
        if (page == Page.OPTIONS) {
            if (row == ROW_BACK) {
                sfx.play(0, FrontEnd.SOUND_PICK);
                return true;
            }
            if (row == ROW_CONTROLLER && pad == null) {
                sfx.play(0, SOUND_ERROR);
                message = "NO CONTROLLER CONNECTED";
                messageTimer = MESSAGE_UPDATES;
                return false;
            }
            sfx.play(0, FrontEnd.SOUND_PICK);
            showPage(row == ROW_CONTROLLER ? Page.CONTROLLER : Page.KEYBOARD, 0);
            return false;
        }
        sfx.play(0, FrontEnd.SOUND_PICK);
        if (row == ROW_BACK) {
            showPage(Page.OPTIONS, page == Page.CONTROLLER ? ROW_CONTROLLER : ROW_KEYBOARD);
        } else if (row == ROW_RESET) {
            if (page == Page.KEYBOARD) {
                bindings.resetKeys();
            } else {
                bindings.resetPad(pad);
            }
        } else {
            waiting = BUTTONS[row - FIRST_BUTTON_ROW];
            waitFresh = true;
        }
        return false;
    }

    private void bindFrom(RawInput raw) {
        if (page == Page.KEYBOARD && raw.key() != Bindings.NONE) {
            bindings.bindKey(waiting, raw.key());
        } else if (page == Page.CONTROLLER && raw.padButton() != Bindings.NONE) {
            bindings.bindPad(pad, waiting, raw.padButton());
        } else {
            return;
        }
        sfx.play(0, FrontEnd.SOUND_PICK);
        waiting = null;
        delay = REBIND_DELAY;
    }

    private void showPage(Page next, int nextRow) {
        page = next;
        row = nextRow;
    }

    private int rowCount() {
        return page == Page.OPTIONS ? 3 : FIRST_BUTTON_ROW + BUTTONS.length;
    }

    public List<String> rows() {
        if (page == Page.OPTIONS) {
            return List.of("BACK", "CONTROLLER SETTINGS", "KEYBOARD SETTINGS");
        }
        List<String> out = new ArrayList<>();
        out.add("BACK");
        out.add("RESET TO DEFAULTS");
        for (PadButton b : BUTTONS) {
            out.add(b.label + ": " + codeName(b));
        }
        return out;
    }

    public String prompt() {
        if (waiting == null) {
            return null;
        }
        return (page == Page.KEYBOARD ? "Press a key for " : "Press a controller button for ") + waiting.label;
    }

    private String codeName(PadButton b) {
        if (page == Page.KEYBOARD) {
            int key = bindings.key(b);
            String name = key == Bindings.NONE ? null : Keys.toString(key);
            return name == null ? "-" : name;
        }
        if (pad == null) {
            return "-";
        }
        int code = bindings.padCode(pad, b);
        if (code == Bindings.NONE) {
            return "-";
        }
        for (PadButton d : BUTTONS) {
            if (pad.defaultCode(d) == code) {
                return d.name();
            }
        }
        return "BUTTON " + code;
    }
}
