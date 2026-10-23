package mmx.x6.physics;

public final class TileRules {

    private TileRules() {
    }

    public static boolean isSlope(int v) {
        return (v >= 1 && v <= 12) || (v >= 17 && v <= 28);
    }

    public static int slopeSurface(int v, int xInCell) {
        int base = v > 16 ? v - 16 : v;
        int x = Math.max(0, Math.min(15, xInCell));
        return switch (base) {
            case 1 -> 15 - x / 4;
            case 2 -> 11 - x / 4;
            case 3 -> 7 - x / 4;
            case 4 -> 3 - x / 4;
            case 5 -> x / 4;
            case 6 -> 4 + x / 4;
            case 7 -> 8 + x / 4;
            case 8 -> 12 + x / 4;
            case 9 -> 15 - x / 2;
            case 10 -> 7 - x / 2;
            case 11 -> x / 2;
            case 12 -> 8 + x / 2;
            default -> 16;
        };
    }

    public static boolean isFlatFloor(int v) {
        return v == 33 || v == 34 || isWall(v);
    }

    public static boolean isOneWay(int v) {
        return v == 33 || v == 34;
    }

    public static boolean isWall(int v) {
        return v == 56 || v == 57 || v == 58 || v == 60 || v == 62 || v == 63;
    }

    public static boolean isCeiling(int v) {
        return isWall(v);
    }

    public static boolean allowsWallCling(int v) {
        return v == 56 || v == 58;
    }

    public static boolean isSpike(int v) {
        return v == 62 || v == 63;
    }

    public static boolean isObjectSolid(int v) {
        return (v >= 3 && v <= 6) || v == 10 || v == 11 || v == 16 || isWall(v);
    }

    public static boolean isLadder(int v) {
        return v == 32;
    }

    public static boolean isLadderTop(int v) {
        return v == 33;
    }

    public static boolean isBar(int v) {
        return v >= 37 && v <= 45;
    }
}
