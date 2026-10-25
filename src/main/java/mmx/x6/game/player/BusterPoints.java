package mmx.x6.game.player;

public final class BusterPoints {

    private static final int STAND_FORWARD = 21;
    private static final int STAND_DOWN = -9;
    private static final int SPRITES = 228;

    private static final int[] SET_0 = {
            2, 28, -9, 4, 28, -10, 6, 28, -10, 8, 28, -9, 10, 28, -10, 12, 29, -11,
            14, 29, -12, 16, 30, -12, 18, 30, -11, 20, 31, -10, 22, 31, -9, 24, 31, -10,
            26, 30, -11, 28, 30, -12, 30, 29, -12, 32, 29, -11, 34, 26, -10, 36, 26, -10,
            38, 24, -10, 40, 24, -11, 42, 23, -11, 44, 22, -10, 46, 23, -9, 48, 23, -9,
            50, 26, -11, 52, 27, -7, 54, 27, -6, 56, 24, -7, 58, 24, -8, 60, 24, -9,
            62, 26, -9, 64, 26, -8, 66, 25, -8, 67, 31, -6, 68, 31, -6, 69, 37, 1,
            70, 37, 1, 71, 42, 5, 72, 42, 5, 73, 42, 5, 74, 42, 5, 75, 42, 5,
            76, 42, 5, 77, 33, -7, 78, 33, -7, 79, 28, -8, 80, 28, -8, 81, 20, -6,
            82, 20, -6, 83, 32, -6, 84, 32, -6, 85, 21, -9, 86, 21, -9, 87, 21, -9,
            88, 12, -9, 89, 21, -9, 90, 21, -9, 91, 21, -9, 92, 21, -9, 150, 26, -9,
            155, 26, -9, 196, 27, -9, 198, 29, -8, 200, 20, -10, 202, 17, -10, 204, 24, -11,
            209, 31, 4, 210, 31, 4, 211, 31, 4, 212, 31, 4, 213, 31, 4, 214, 31, 4,
            215, 31, 4, 216, 31, 4, 217, 31, 4, 218, 31, 4,
    };

    private static final int[] SET_1 = {
            0, 26, -10, 1, 26, -10, 2, 26, -10, 3, 26, -10, 4, 26, -10, 5, 26, -10,
            6, 26, -10, 7, 26, -10, 8, 26, -10, 9, 26, -10, 10, 26, -10, 11, 26, -10,
            12, 26, -10, 13, 26, -10, 14, 26, -10, 15, 26, -10, 16, 26, -10, 17, 26, -10,
            18, 26, -10, 19, 26, -10, 20, 26, -10, 21, 26, -10, 22, 26, -10, 23, 26, -10,
            24, 26, -10, 25, 26, -10, 26, 26, -10, 27, 26, -10, 28, 26, -10, 29, 26, -10,
            30, 26, -10, 31, 26, -10, 32, 26, -10, 33, 26, -10, 34, 26, -10, 35, 26, -10,
            36, 26, -10, 37, 26, -10, 38, 26, -10, 39, 26, -10, 40, 26, -10, 41, 26, -10,
            42, 26, -10, 43, 26, -10, 44, 26, -10, 45, 26, -10, 46, 26, -10,
    };

    private static final int[][] FORWARD = new int[2][SPRITES];
    private static final int[][] DOWN = new int[2][SPRITES];
    private static final boolean[][] SET = new boolean[2][SPRITES];

    static {
        fill(0, SET_0);
        fill(1, SET_1);
    }

    private BusterPoints() {
    }

    private static void fill(int set, int[] rows) {
        for (int i = 0; i + 2 < rows.length; i += 3) {
            FORWARD[set][rows[i]] = rows[i + 1];
            DOWN[set][rows[i]] = rows[i + 2];
            SET[set][rows[i]] = true;
        }
    }

    static int frameSet(int anim) {
        return anim >= 83 && anim <= 85 || anim == 140 || anim >= 144 && anim <= 192 ? 1 : 0;
    }

    public static int forward(int anim, int sprite) {
        int set = frameSet(anim);
        return has(set, sprite) ? FORWARD[set][sprite] : STAND_FORWARD;
    }

    public static int down(int anim, int sprite) {
        int set = frameSet(anim);
        return has(set, sprite) ? DOWN[set][sprite] : STAND_DOWN;
    }

    private static boolean has(int set, int sprite) {
        return sprite >= 0 && sprite < SPRITES && SET[set][sprite];
    }
}
