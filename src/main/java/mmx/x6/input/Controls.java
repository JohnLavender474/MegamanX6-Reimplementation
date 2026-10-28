package mmx.x6.input;

import java.util.function.IntPredicate;
import java.util.function.Supplier;

public final class Controls {

    private final Bindings bindings;
    private final Supplier<Pad> source;
    private Pad pad;
    private boolean[] down = new boolean[0];
    private int padPressed = Bindings.NONE;

    public Controls(Bindings bindings, Supplier<Pad> source) {
        this.bindings = bindings;
        this.source = source;
    }

    public void poll() {
        Pad now = source.get();
        boolean fresh = now != pad;
        pad = now;
        padPressed = Bindings.NONE;
        if (pad == null) {
            down = new boolean[0];
            return;
        }
        if (fresh || down.length != pad.buttonCount()) {
            down = new boolean[pad.buttonCount()];
            fresh = true;
        }
        for (int code = 0; code < down.length; code++) {
            boolean d = pad.button(code);
            if (d && !down[code] && !fresh && padPressed == Bindings.NONE) {
                padPressed = code;
            }
            down[code] = d;
        }
    }

    public Bindings bindings() {
        return bindings;
    }

    public Pad pad() {
        return pad;
    }

    public int padPressed() {
        return padPressed;
    }

    public int held(IntPredicate keyDown) {
        return bindings.keyHeld(keyDown) | bindings.padHeld(pad);
    }
}
