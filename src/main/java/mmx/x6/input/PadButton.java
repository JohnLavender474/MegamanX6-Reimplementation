package mmx.x6.input;

import com.badlogic.gdx.Input.Keys;

public enum PadButton {
    UP(Buttons.UP, "UP", Keys.W),
    DOWN(Buttons.DOWN, "DOWN", Keys.S),
    LEFT(Buttons.LEFT, "LEFT", Keys.A),
    RIGHT(Buttons.RIGHT, "RIGHT", Keys.D),
    CROSS(Buttons.JUMP, "CROSS (JUMP)", Keys.K),
    CIRCLE(Buttons.DASH, "CIRCLE (DASH)", Keys.L),
    SQUARE(Buttons.BUSTER, "SQUARE (BUSTER)", Keys.J),
    TRIANGLE(Buttons.WEAPON, "TRIANGLE (WEAPON)", Keys.I),
    L1(Buttons.WEAPON_PREV, "L1 (PREV WEAPON)", Keys.Q),
    R1(Buttons.WEAPON_NEXT, "R1 (NEXT WEAPON)", Keys.O),
    START(Buttons.START, "START", Keys.ENTER);

    public final int bit;
    public final String label;
    public final int defaultKey;

    PadButton(int bit, String label, int defaultKey) {
        this.bit = bit;
        this.label = label;
        this.defaultKey = defaultKey;
    }
}
