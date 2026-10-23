package mmx.x6.physics;

import mmx.x6.level.CollisionGrid;

public final class Grids {

    private Grids() {
    }

    public static CollisionGrid grid(String... rows) {
        int h = rows.length;
        int w = rows[0].length();
        int[] v = new int[w * h];
        for (int y = 0; y < h; y++) {
            int rise = 0;
            int fall = 0;
            for (int x = 0; x < w; x++) {
                char c = rows[y].charAt(x);
                int value = switch (c) {
                    case '#' -> 56;
                    case 'n' -> 57;
                    case 'u' -> 16;
                    case 'T' -> 33;
                    case 't' -> 34;
                    case '^' -> 63;
                    case 'L' -> 32;
                    case '=' -> 45;
                    case 'g' -> 59;
                    case '/' -> 1 + (rise++ % 4);
                    case '\\' -> 5 + (fall++ % 4);
                    default -> 0;
                };
                if (c != '/') {
                    rise = 0;
                }
                if (c != '\\') {
                    fall = 0;
                }
                v[y * w + x] = value;
            }
        }
        return new CollisionGrid(w, h, v);
    }

    public static Body xBody(float x, float y) {
        return new Body(x, y, 0, 2, 11, 19);
    }
}
