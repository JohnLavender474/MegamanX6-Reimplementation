package mmx.x6.level;

public final class CollisionGrid {

    private final int width;
    private final int height;
    private final int[] values;

    public CollisionGrid(int width, int height, int[] values) {
        this.width = width;
        this.height = height;
        this.values = values;
    }

    public int width() {
        return width;
    }

    public int height() {
        return height;
    }

    public int raw(int cx, int cy) {
        if (cx < 0 || cy < 0 || cx >= width || cy >= height) {
            return 0;
        }
        return values[cy * width + cx];
    }

    public int at(float px, float py) {
        int v = raw((int) Math.floor(px / 16f), (int) Math.floor(py / 16f));
        return v == 59 ? 56 : v;
    }

    public void set(int cx, int cy, int v) {
        if (cx >= 0 && cy >= 0 && cx < width && cy < height) {
            values[cy * width + cx] = v;
        }
    }

    public CollisionGrid copy() {
        return new CollisionGrid(width, height, values.clone());
    }
}
