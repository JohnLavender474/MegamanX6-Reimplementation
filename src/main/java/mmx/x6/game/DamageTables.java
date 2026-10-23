package mmx.x6.game;

public final class DamageTables {

    public static final int DEFLECT = -1;
    public static final int NO_EFFECT = -2;

    private static final int D = DEFLECT;
    private static final int N = NO_EFFECT;

    public enum Table { NORMAL, SABRE_ONLY, D1000, DEFLECT, MINOR }

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
            case DEFLECT -> weaponId == 7 || weaponId == 23 || weaponId == 79 ? NO_EFFECT : DEFLECT;
        };
    }
}
