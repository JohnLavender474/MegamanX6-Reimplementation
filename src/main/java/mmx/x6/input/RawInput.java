package mmx.x6.input;

public record RawInput(int key, int padButton, Pad pad) {

    public static final RawInput NONE = new RawInput(Bindings.NONE, Bindings.NONE, null);
}
