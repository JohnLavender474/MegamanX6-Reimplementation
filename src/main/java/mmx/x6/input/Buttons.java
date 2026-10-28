package mmx.x6.input;

public final class Buttons {

    public static final int RIGHT = 0x1;
    public static final int LEFT = 0x2;
    public static final int UP = 0x4;
    public static final int DOWN = 0x8;
    public static final int WEAPON = 0x10;
    public static final int BUSTER = 0x20;
    public static final int JUMP = 0x80;
    public static final int DASH = 0x100;
    public static final int WEAPON_NEXT = 0x200;
    public static final int WEAPON_PREV = 0x400;
    public static final int START = 0x800;

    private int held;
    private int before;

    public void set(int now) {
        if ((now & (LEFT | RIGHT)) == (LEFT | RIGHT)) {
            now &= ~(LEFT | RIGHT);
        }
        if ((now & (UP | DOWN)) == (UP | DOWN)) {
            now &= ~(UP | DOWN);
        }
        before = held;
        held = now;
    }

    public void clear() {
        before = 0;
        held = 0;
    }

    public int held() {
        return held;
    }

    public int pressed() {
        return held & ~before;
    }

    public int released() {
        return before & ~held;
    }

    public boolean isHeld(int bit) {
        return (held & bit) != 0;
    }

    public boolean isPressed(int bit) {
        return (pressed() & bit) != 0;
    }

    public boolean isReleased(int bit) {
        return (released() & bit) != 0;
    }
}
