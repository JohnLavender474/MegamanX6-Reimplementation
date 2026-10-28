package mmx.x6.input;

import java.util.HashMap;
import java.util.Map;
import java.util.function.IntPredicate;

public final class Bindings {

    public static final int NONE = -1;

    private static final PadButton[] ALL = PadButton.values();

    private final int[] keys = new int[ALL.length];
    private final Map<String, int[]> pads = new HashMap<>();

    public Bindings() {
        resetKeys();
    }

    public int key(PadButton b) {
        return keys[b.ordinal()];
    }

    public void bindKey(PadButton b, int code) {
        bind(keys, b, code);
    }

    public void resetKeys() {
        for (PadButton b : ALL) {
            keys[b.ordinal()] = b.defaultKey;
        }
    }

    public int keyHeld(IntPredicate keyDown) {
        int held = 0;
        for (PadButton b : ALL) {
            if (keyDown.test(keys[b.ordinal()])) {
                held |= b.bit;
            }
        }
        return held;
    }

    public int padCode(Pad pad, PadButton b) {
        return codes(pad)[b.ordinal()];
    }

    public void bindPad(Pad pad, PadButton b, int code) {
        bind(codes(pad), b, code);
    }

    public void resetPad(Pad pad) {
        pads.remove(pad.name());
    }

    public int padHeld(Pad pad) {
        if (pad == null) {
            return 0;
        }
        int[] codes = codes(pad);
        int held = 0;
        for (PadButton b : ALL) {
            int code = codes[b.ordinal()];
            if (code != NONE && pad.button(code)) {
                held |= b.bit;
            }
        }
        return held;
    }

    private int[] codes(Pad pad) {
        return pads.computeIfAbsent(pad.name(), n -> {
            int[] codes = new int[ALL.length];
            for (PadButton b : ALL) {
                codes[b.ordinal()] = pad.defaultCode(b);
            }
            return codes;
        });
    }

    private static void bind(int[] codes, PadButton b, int code) {
        int old = codes[b.ordinal()];
        for (int i = 0; i < codes.length; i++) {
            if (codes[i] == code) {
                codes[i] = old;
            }
        }
        codes[b.ordinal()] = code;
    }
}
