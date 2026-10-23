package mmx.x6.physics;

public final class Body {

    public float x;
    public float y;
    public float boxOffX;
    public float boxOffY;
    public float boxHalfW;
    public float boxHalfH;
    public boolean hasBox = true;

    public Body(float x, float y, float boxOffX, float boxOffY, float boxHalfW, float boxHalfH) {
        this.x = x;
        this.y = y;
        this.boxOffX = boxOffX;
        this.boxOffY = boxOffY;
        this.boxHalfW = boxHalfW;
        this.boxHalfH = boxHalfH;
    }

    public float centerX() {
        return x + boxOffX;
    }

    public float centerY() {
        return y + boxOffY;
    }

    public float left() {
        return centerX() - boxHalfW;
    }

    public float right() {
        return centerX() + boxHalfW;
    }

    public float top() {
        return centerY() - boxHalfH;
    }

    public float bottom() {
        return centerY() + boxHalfH;
    }

    public Rect box() {
        return new Rect(left(), top(), boxHalfW * 2, boxHalfH * 2);
    }
}
