package mmx.x6.physics;

public record Rect(float x, float y, float w, float h) {

    public static Rect ofDoc(float ox, float oy, float dx, float dy, float w, float h, boolean faceRight) {
        float left = faceRight ? ox - dx - w : ox + dx;
        return new Rect(left, oy + dy, w, h);
    }

    public float right() {
        return x + w;
    }

    public float bottom() {
        return y + h;
    }

    public float centerX() {
        return x + w / 2f;
    }

    public float centerY() {
        return y + h / 2f;
    }

    public boolean overlaps(Rect o) {
        return o != null && x < o.right() && o.x < right() && y < o.bottom() && o.y < bottom();
    }

    public boolean contains(float px, float py) {
        return px >= x && px < right() && py >= y && py < bottom();
    }
}
