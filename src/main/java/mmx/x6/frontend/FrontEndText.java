package mmx.x6.frontend;

public final class FrontEndText {

    public record Text(int textVar, int x, int y, int sprite, boolean orange) {
    }

    public record Sprite(int w, int h, int ox, int oy) {
    }

    public static final int[] TITLE_VARS = {4};
    public static final int[] MENU_VARS = {0, 1, 2, 5, 7, 6};
    // TODO: "Now Loading..." is cut with the menu CLUT 0x7800; its real CLUT and the code that draws it are not known (docs/gaps.md, "Front end")
    public static final int[] LOADING_RECT = {1280, 3, 126, 15};
    public static final int[] BACKGROUND_RECT = {0, 0, 512, 240};

    private static final int CELL_W = 260;
    private static final int CELL_H = 23;
    private static final int CELL_PAD = 4;
    private static final int[][] TABLE = {
            {258, 142, 0}, {258, 158, 1}, {258, 174, 2}, {258, 182, 3},
            {258, 160, 4}, {220, 192, 5}, {258, 134, 0}, {476, 192, 6}};
    private static final Sprite[] SPRITES = {
            new Sprite(134, 12, 64, 4), new Sprite(107, 12, 61, 4), new Sprite(82, 12, 45, 4),
            new Sprite(96, 12, 47, 4), new Sprite(198, 9, 98, 2), new Sprite(252, 15, 124, 7),
            new Sprite(60, 9, 128, 2)};

    private FrontEndText() {
    }

    public static Text text(int textVar, int row) {
        if (textVar == 6) {
            int r = row & 3;
            int sprite = r < 3 ? r : 3;
            return new Text(6, 258, 142 + r * 16, sprite, true);
        }
        int[] t = TABLE[textVar];
        return new Text(textVar, t[0], t[1], t[2], false);
    }

    public static Sprite sprite(int n) {
        return SPRITES[n];
    }

    public static int[] sheetRect(int sprite, boolean orange) {
        Sprite s = SPRITES[sprite];
        return new int[] {(orange ? CELL_W : 0) + CELL_PAD, sprite * CELL_H + CELL_PAD, s.w(), s.h()};
    }
}
