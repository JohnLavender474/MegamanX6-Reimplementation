package mmx.x6.input;

public interface Pad {

    String name();

    boolean button(int code);

    int buttonCount();

    int defaultCode(PadButton b);
}
