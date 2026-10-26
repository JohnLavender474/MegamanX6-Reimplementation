package mmx.x6.game;

import mmx.x6.level.Checkpoint;
import mmx.x6.level.Point;

public final class CameraController {

    public static final int VIEW_W = 320;
    public static final int VIEW_H = 240;
    public static final int FOLLOW_X = 160;
    public static final int FOLLOW_Y = 171;
    public static final int TOP_Y = 96;
    public static final int FOLLOW_SPEED = 8;
    public static final int START_EASE = 4;

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
    private float lastPx = Float.NaN;
    private float lastPy = Float.NaN;
    private int layer1Mode = 1;
    private int layer2Mode = 3;
    private int layer1Ox;
    private int layer1Oy = 560;
    private int layer2Ox;
    private int layer2Oy = 640;
    private int tmxLayer2Ox;
    private int tmxLayer2Oy = 640;

    public float easeSpeed = START_EASE;

    public void configure(int mode1, int ox1, int oy1, int mode2, int ox2, int oy2) {
        layer1Mode = mode1;
        layer1Ox = ox1;
        layer1Oy = oy1;
        layer2Mode = mode2;
        layer2Ox = tmxLayer2Ox = ox2;
        layer2Oy = tmxLayer2Oy = oy2;
    }

    public void reset(Checkpoint cp) {
        x = cp.camX();
        y = cp.camY();
        minX = tMinX = cp.minX();
        maxX = tMaxX = cp.maxX();
        minY = tMinY = cp.minY();
        maxY = tMaxY = cp.maxY();
        easeSpeed = START_EASE;
        follow = false;
        lastPx = Float.NaN;
        lastPy = Float.NaN;
        Point off = cp.layer2Offset();
        layer2Ox = off == null ? tmxLayer2Ox : off.x();
        layer2Oy = off == null ? tmxLayer2Oy : off.y();
        Point fixed = cp.layer1();
        layer1X = fixed == null ? (layer1Mode == 5 ? x : target(layer1Mode, x, layer1Ox)) : fixed.x();
        layer1Y = fixed == null ? (layer1Mode == 5 ? y : target(layer1Mode, y, layer1Oy, true)) : fixed.y();
        layer2X = layer2Mode == 5 ? x : target(layer2Mode, x, layer2Ox);
        layer2Y = layer2Mode == 5 ? y : target(layer2Mode, y, layer2Oy, true);
    }

    public void setLayer1(float lx, float ly) {
        layer1X = lx;
        layer1Y = ly;
    }

    public void setLayer2(float lx, float ly) {
        layer2X = lx;
        layer2Y = ly;
    }

    public void setLayer2Mode(int mode, int ox, int oy) {
        layer2Mode = mode;
        layer2Ox = ox;
        layer2Oy = oy;
        if (mode != 5 && mode != 9) {
            layer2X = target(mode, x, ox);
            layer2Y = target(mode, y, oy, true);
        }
    }

    public int layer2Mode() {
        return layer2Mode;
    }

    public void setLayer1Offset(int ox, int oy) {
        layer1Ox = ox;
        layer1Oy = oy;
        layer1X = target(layer1Mode, x, ox);
        layer1Y = target(layer1Mode, y, oy, true);
    }

    public void setLayer2Offset(int ox, int oy) {
        layer2Ox = ox;
        layer2Oy = oy;
    }

    public int layer1Mode() {
        return layer1Mode;
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
        int ipx = (int) Math.floor(px);
        int ipy = (int) Math.floor(py);
        int dx = Float.isNaN(lastPx) ? 0 : ipx - (int) Math.floor(lastPx);
        int dy = Float.isNaN(lastPy) ? 0 : ipy - (int) Math.floor(lastPy);
        lastPx = px;
        lastPy = py;
        if (follow) {
            int cx = (int) x;
            int cy = (int) y;
            cx += speedCap(followMove(ipx - cx, FOLLOW_X, FOLLOW_X));
            cy += speedCap(followMove(ipy - cy, TOP_Y, FOLLOW_Y));
            x = clamp(cx, minX, Math.max(minX, maxX));
            y = clamp(cy, minY, Math.max(minY, maxY));
            int cx2 = (int) x;
            int cy2 = (int) y;
            minX = easeMinX(minX, tMinX, cx2, ipx - cx2, dx, step);
            maxX = easeMaxX(maxX, tMaxX, cx2, ipx - cx2, dx, step);
            minY = easeMinY(minY, tMinY, cy2, ipy - cy2, dy, step);
            maxY = easeMaxY(maxY, tMaxY, cy2, ipy - cy2, dy, step);
        }
        layer1X = step(layer1Mode, layer1X, target(layer1Mode, x, layer1Ox), true);
        if (layer1Mode != 9) {
            layer1Y = step(layer1Mode, layer1Y, target(layer1Mode, y, layer1Oy, true), true);
        }
        layer2X = step(layer2Mode, layer2X, target(layer2Mode, x, layer2Ox), false);
        if (layer2Mode != 9) {
            layer2Y = step(layer2Mode, layer2Y, target(layer2Mode, y, layer2Oy, true), false);
        }
    }

    private static float step(int mode, float old, float target, boolean layer1) {
        if (mode == 5) {
            return old;
        }
        boolean capped = mode == 3 || mode == 6 || mode == 7 || mode == 8 || mode == 10
                || layer1 && (mode == 1 || mode == 4);
        return capped ? cap(old, target) : target;
    }

    private static float target(int mode, float pos, int offset) {
        return target(mode, pos, offset, false);
    }

    private static float target(int mode, float pos, int offset, boolean vertical) {
        int p = (int) Math.floor(pos);
        return switch (mode) {
            case 1 -> Math.floorDiv(p, 2) + offset;
            case 3 -> Math.floorDiv(p, 4) + offset;
            case 4 -> (vertical ? p : Math.floorDiv(p, 2)) + offset;
            case 6 -> p * 3 / 4 + offset;
            case 7 -> p + Math.floorDiv(p, 4) + offset;
            case 8 -> p + Math.floorDiv(p, 2) + offset;
            case 10 -> (vertical ? p : Math.floorDiv(p, 4)) + offset;
            default -> p + offset;
        };
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

    static int followMove(int onScreen, int low, int high) {
        if (onScreen - low < 0) {
            return onScreen - low;
        }
        return Math.max(0, onScreen - high);
    }

    static int speedCap(int move) {
        return Math.max(-FOLLOW_SPEED, Math.min(FOLLOW_SPEED, move));
    }

    static int easeMinX(int current, int target, int cam, int onScreen, int move, int step) {
        if (current == target || Math.abs(target - current) <= step) {
            return target;
        }
        if (target >= cam) {
            return Math.min(cam + step, target);
        }
        if (onScreen - FOLLOW_X < 0) {
            return target;
        }
        int eased = cam + Math.min(0, move) - step;
        return eased < target ? eased : target;
    }

    static int easeMaxX(int current, int target, int cam, int onScreen, int move, int step) {
        if (current == target || Math.abs(target - current) <= step) {
            return target;
        }
        if (target < cam) {
            return Math.max(cam - step, target);
        }
        if (onScreen - FOLLOW_X + 2 >= 4) {
            return target;
        }
        int eased = cam + Math.max(0, move) + step;
        return eased > target ? eased : target;
    }

    static int easeMinY(int current, int target, int cam, int onScreen, int move, int step) {
        if (current == target || Math.abs(target - current) <= step) {
            return target;
        }
        if (target >= cam) {
            return Math.min(cam + step, target);
        }
        if (onScreen - TOP_Y >= 0) {
            return onScreen - TOP_Y;
        }
        return Math.max(cam + Math.min(0, move) - step, target);
    }

    static int easeMaxY(int current, int target, int cam, int onScreen, int move, int step) {
        if (current == target) {
            return current;
        }
        if (target < cam) {
            return Math.max(cam - step, target);
        }
        if (onScreen - FOLLOW_Y + 2 >= 4) {
            return target;
        }
        return Math.min(cam + Math.max(0, move) + step, target);
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
