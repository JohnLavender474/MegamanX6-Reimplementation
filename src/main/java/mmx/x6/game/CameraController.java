package mmx.x6.game;

import mmx.x6.level.Checkpoint;
import mmx.x6.level.IntroCheckpoints;

public final class CameraController {

    public static final int VIEW_W = 320;
    public static final int VIEW_H = 240;

    private float x;
    private float y;
    private int minX;
    private int maxX;
    private int minY;
    private int maxY;
    private int tMinX;
    private int tMaxX;
    private int tMinY;
    private int tMaxY;
    private float layer1X;
    private float layer1Y;
    private float layer2X;
    private float layer2Y;
    private boolean follow;

    public float easeSpeed = 2;

    public void reset(Checkpoint cp) {
        x = cp.camX();
        y = cp.camY();
        minX = tMinX = cp.minX();
        maxX = tMaxX = cp.maxX();
        minY = tMinY = cp.minY();
        maxY = tMaxY = cp.maxY();
        easeSpeed = 2;
        follow = false;
        layer1X = layer1TargetX();
        layer1Y = layer1TargetY();
        layer2X = layer2TargetX();
        layer2Y = layer2TargetY();
    }

    public void setTargets(int minX, int maxX, int minY, int maxY) {
        if (minX >= 0) {
            tMinX = minX;
        }
        if (maxX >= 0) {
            tMaxX = maxX;
        }
        if (minY >= 0) {
            tMinY = minY;
        }
        if (maxY >= 0) {
            tMaxY = maxY;
        }
    }

    public void setFollow(boolean follow) {
        this.follow = follow;
    }

    public boolean follow() {
        return follow;
    }

    public void update(float px, float py) {
        int step = Math.max(1, Math.round(easeSpeed));
        minX = ease(minX, tMinX, step);
        maxX = ease(maxX, tMaxX, step);
        minY = ease(minY, tMinY, step);
        maxY = ease(maxY, tMaxY, step);
        float tx = follow ? px - VIEW_W / 2f : x;
        // TODO: the X6 follow rule is not read; this keeps X centred (docs/gaps.md, "Camera follow")
        float ty = follow ? py - 128 : y;
        x = clamp(tx, minX, Math.max(minX, maxX));
        y = clamp(ty, minY, Math.max(minY, maxY));
        layer1X = cap(layer1X, layer1TargetX());
        layer1Y = cap(layer1Y, layer1TargetY());
        layer2X = cap(layer2X, layer2TargetX());
        layer2Y = cap(layer2Y, layer2TargetY());
    }

    private float layer1TargetX() {
        return Math.floorDiv((int) Math.floor(x), 2) + IntroCheckpoints.L1_OX;
    }

    private float layer1TargetY() {
        return Math.floorDiv((int) Math.floor(y), 2) + IntroCheckpoints.L1_OY;
    }

    private float layer2TargetX() {
        return Math.floorDiv((int) Math.floor(x), 4) + IntroCheckpoints.L2_OX;
    }

    private float layer2TargetY() {
        return Math.floorDiv((int) Math.floor(y), 4) + IntroCheckpoints.L2_OY;
    }

    static float cap(float old, float target) {
        float d = target - old;
        float a = Math.abs(d);
        if (a < 16) {
            return target;
        }
        if (a < 328) {
            return old + Math.signum(d) * 16;
        }
        return target - Math.signum(d) * 320;
    }

    private static int ease(int current, int target, int step) {
        if (current < target) {
            return Math.min(target, current + step);
        }
        if (current > target) {
            return Math.max(target, current - step);
        }
        return current;
    }

    private static float clamp(float v, float lo, float hi) {
        return Math.max(lo, Math.min(hi, v));
    }

    public float x() {
        return x;
    }

    public float y() {
        return y;
    }

    public int minX() {
        return minX;
    }

    public int maxX() {
        return maxX;
    }

    public int minY() {
        return minY;
    }

    public int maxY() {
        return maxY;
    }

    public int targetMinX() {
        return tMinX;
    }

    public int targetMaxX() {
        return tMaxX;
    }

    public int targetMinY() {
        return tMinY;
    }

    public int targetMaxY() {
        return tMaxY;
    }

    public float layer1X() {
        return layer1X;
    }

    public float layer1Y() {
        return layer1Y;
    }

    public float layer2X() {
        return layer2X;
    }

    public float layer2Y() {
        return layer2Y;
    }
}
