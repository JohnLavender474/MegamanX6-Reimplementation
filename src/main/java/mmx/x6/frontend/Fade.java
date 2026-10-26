package mmx.x6.frontend;

public final class Fade {

    private static final int MAX = 255;

    private int level;
    private int step;

    public void in(int speed) {
        level = MAX;
        step = -speed;
    }

    public void out(int speed) {
        level = 0;
        step = speed;
    }

    public void clear() {
        level = 0;
        step = 0;
    }

    public void update() {
        if (step != 0) {
            level = Math.max(0, Math.min(MAX, level + step));
            if (level == 0 || level == MAX) {
                step = 0;
            }
        }
    }

    public boolean busy() {
        return step != 0;
    }

    public float darkness() {
        // TODO: the fade is drawn as black with alpha; X6 subtracts the level from each colour (docs/gaps.md, "Front end")
        return level / (float) MAX;
    }
}
