package mmx.x6.input;

import java.util.HashSet;
import java.util.Set;

public final class FakePad implements Pad {

    public final Set<Integer> down = new HashSet<>();
    private final String name;

    public FakePad(String name) {
        this.name = name;
    }

    @Override
    public String name() {
        return name;
    }

    @Override
    public boolean button(int code) {
        return down.contains(code);
    }

    @Override
    public int buttonCount() {
        return 16;
    }

    @Override
    public int defaultCode(PadButton b) {
        return b.ordinal();
    }
}
