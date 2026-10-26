package mmx.x6.game;

public final class DamageTables {

    public static final int DEFLECT = -1;
    public static final int NO_EFFECT = -2;

    private static final int D = DEFLECT;
    private static final int N = NO_EFFECT;

    public enum Table { NORMAL, SABRE_ONLY, D1000, DEFLECT, MINOR, YAMMARK, SNAKE, GREY_BLOCK, ONE_HIT, TURTLOID, WOLFANG, ICE_BLOCK, SHADOW, SCARAVICH, SCARAVICH_BALL, REFLECTOR, REFLECTOR_WAIT, NIGHTMARE_BUG, SHELDON }

    private static final int[] NORMAL = {
            3, 5, 4, 6, 6, 6, 5, N, 4, 7, 6, 6, 6, 6, 8, 6,
            8, 8, 15, 15, 16, 1, 32, 1, 4, 6, 4, 4, 4, 4, 4, 4,
            3, 3, 6, 8, 8, 6, 4, 4, 4, 4, 4, 5, 8, 4, 15, 8,
            4, 4, 4, 4, 3, 6, 4, 127, 3, 8, 4, 4, 15, 5, 15, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, N};

    private static final int[] SABRE_ONLY = {
            D, D, D, D, D, D, D, N, D, D, D, D, D, D, D, D,
            D, D, D, D, D, D, D, D, 4, 6, 4, 4, 4, 4, 4, 4,
            3, 3, 8, 8, 8, 6, 4, 4, 4, 4, 4, D, 8, D, 4, 6,
            D, D, 4, 6, D, D, D, 127, 1, 1, D, D, D, D, D, D,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D};

    private static final int[] D1000 = {
            D, 2, 3, 1, 1, 1, 1, N, 1, D, 1, 3, 1, 1, 1, 1,
            1, 1, 5, 1, 4, D, 8, 1, 2, 2, 3, 2, 2, 2, 1, 2,
            1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 2, 1, 1, 2,
            1, 1, 1, 1, 1, 1, 1, N, 1, 0, 1, D, D, D, D, 0,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D};

    private static final int[] MINOR = {
            3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 7, 7, 7, 7,
            7, 7, 15, 4, 16, 2, 32, 4, 4, 6, 4, 4, 4, 4, 4, 4,
            3, 3, 6, 8, 8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 4, 4, 4, 4, 4, 4, N, 1, 1, 4, 127, 127, 127, 127, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D};

    private static final int[] YAMMARK = {
            1, D, 2, 2, 2, 2, 2, N, 5, 1, D, 2, 2, 2, 2, 2,
            2, 8, 4, 4, 4, D, 8, D, 2, 2, 5, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8, 1, 4, 8,
            2, 1, 2, 2, 1, 2, D, 127, 3, 8, 4, 4, 15, 5, 15, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, 127};

    private static final int[] SNAKE = {
            1, 2, 5, D, 5, 5, 4, N, 2, 1, 4, 8, D, 8, 8, 8,
            2, 8, 4, 4, 4, D, 8, D, 3, 3, 5, 2, 2, 2, 2, 2,
            2, 5, D, 5, 6, 3, 2, 2, 2, 2, 4, 2, 8, 1, 4, 8,
            4, 4, 3, 3, 1, 5, 1, 127, 3, 8, 4, 4, 15, 5, 15, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, 127};

    private static final int[] SHELDON = {
            1, 2, 2, 2, 5, 2, 2, N, 2, 1, 2, 2, 2, 8, 2, 2,
            D, 8, 4, 4, 4, D, 8, D, 2, 2, 5, 2, 2, 2, 2, 2,
            2, 2, 2, 8, 2, 2, 2, 2, 2, 2, 2, 2, D, 1, 4, 8,
            2, 1, 2, 2, 1, D, 1, 127, 3, 8, 4, 4, 15, 5, 15, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, 127};

    private static final int[] NIGHTMARE_BUG = {
            D, 4, D, D, D, D, D, D, D, D, 4, D, D, D, D, D,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,
            D, D, D, D, D, D, 4, N, D, D, D, N, N, D, D, N,
            N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N};

    private static final int[] REFLECTOR = {
            3, 3, 3, 4, 6, 5, 7, N, 3, 7, 6, 6, 6, 8, 8, 8,
            8, 8, 15, 10, 16, 1, 32, N, 4, 6, 4, 4, 4, 4, 4, 4,
            3, 3, 6, 8, 8, 6, 4, 4, 4, 4, 4, 5, 8, 4, 7, 8,
            4, 4, 2, 3, 6, 4, 4, 127, 3, 8, 4, 4, 15, 5, 15, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, N};

    private static final int[] REFLECTOR_WAIT = {
            3, N, N, N, N, N, N, N, N, 7, N, N, N, N, N, N,
            N, N, 15, N, N, N, N, N, N, N, N, N, N, N, N, N,
            N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
            N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N,
            N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N};

    private static final int[] SCARAVICH = {
            1, 5, 2, 2, 2, D, 2, N, 2, 1, 8, 2, 2, 2, D, 2,
            2, 8, 4, 4, 4, D, 8, D, 1, 1, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, D, 2, 2, 2, 2, 2, 2, 2, D, 1, 4, 8,
            2, 1, 2, 2, 1, 2, 5, 127, 3, 8, 4, 4, 15, 5, 15, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, 127};

    private static final int[] SCARAVICH_BALL = {
            1, 2, 2, 2, 2, 2, 2, N, 2, 1, 2, 2, 2, 2, 2, 2,
            2, 8, 4, 4, 4, D, 8, D, 2, 2, 5, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8, 1, 4, 8,
            2, 1, 2, 2, 1, 2, 1, 127, 3, 8, 4, 4, 15, 5, 15, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, 127};

    private static final int[] TURTLOID = {
            1, 2, 5, 2, 2, 2, D, N, 2, 1, 2, 8, 2, 2, 2, D,
            2, 8, 4, 4, 4, D, 8, D, 1, 1, 2, 2, 2, 2, 2, 2,
            2, 5, 2, 2, 2, 2, 2, 2, 2, 2, D, 2, D, 1, 4, 8,
            2, 1, 2, 2, 3, 2, 1, 127, 3, 8, 4, 4, 15, 5, 15, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, 127};

    private static final int[] WOLFANG = {
            1, 2, D, 5, 2, 2, 2, N, 2, 1, 2, D, 8, 2, 2, 2,
            2, 8, 4, 4, 4, D, 8, D, 1, 1, 2, 2, 2, 2, 2, 2,
            2, 2, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, D, 1, 4, 8,
            2, 1, 2, 2, D, 2, 1, 127, 3, 8, 4, 4, 15, 5, 15, 3,
            D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, 127};

    private DamageTables() {
    }

    public static int value(Table t, int weaponId) {
        if (weaponId < 0 || weaponId >= 80) {
            return DEFLECT;
        }
        return switch (t) {
            case NORMAL -> NORMAL[weaponId];
            case SABRE_ONLY -> SABRE_ONLY[weaponId];
            case D1000 -> D1000[weaponId];
            case MINOR -> MINOR[weaponId];
            case YAMMARK -> YAMMARK[weaponId];
            case SNAKE -> SNAKE[weaponId];
            case TURTLOID -> TURTLOID[weaponId];
            case SCARAVICH -> SCARAVICH[weaponId];
            case SCARAVICH_BALL -> SCARAVICH_BALL[weaponId];
            case REFLECTOR -> REFLECTOR[weaponId];
            case REFLECTOR_WAIT -> REFLECTOR_WAIT[weaponId];
            case NIGHTMARE_BUG -> NIGHTMARE_BUG[weaponId];
            case SHELDON -> SHELDON[weaponId];
            case WOLFANG -> WOLFANG[weaponId];
            case ICE_BLOCK -> switch (weaponId) {
                case 4, 13 -> 3;
                case 55 -> 127;
                case 7, 23 -> NO_EFFECT;
                default -> DEFLECT;
            };
            case SHADOW -> switch (weaponId) {
                case 7 -> 4;
                case 16 -> 6;
                default -> NO_EFFECT;
            };
            case ONE_HIT -> 127;
            case GREY_BLOCK -> switch (weaponId) {
                case 4, 35 -> 4;
                case 55 -> 127;
                case 7, 23 -> NO_EFFECT;
                default -> DEFLECT;
            };
            case DEFLECT -> weaponId == 7 || weaponId == 23 || weaponId == 79 ? NO_EFFECT : DEFLECT;
        };
    }
}
