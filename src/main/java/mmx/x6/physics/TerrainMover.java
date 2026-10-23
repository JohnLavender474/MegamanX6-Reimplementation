package mmx.x6.physics;

import java.util.List;
import mmx.x6.level.CollisionGrid;

public final class TerrainMover {

    public static final int RIGHT = 1;
    public static final int LEFT = 2;
    public static final int CEILING = 4;
    public static final int FLOOR = 8;

    private static final float EPS = 0.001f;
    private static final float MAX_FLAT_STEP_DOWN = 8;

    private final CollisionGrid grid;

    public boolean lastWallCling;
    public boolean lastSpike;
    public boolean lastOnSlope;
    public boolean lastProbeCling;

    public TerrainMover(CollisionGrid grid) {
        this.grid = grid;
    }

    public int move(Body b, float dx, float dy, boolean grounded, List<Rect> solids) {
        lastWallCling = false;
        lastSpike = false;
        lastOnSlope = false;
        int flags = 0;
        if (dx != 0) {
            b.x += dx;
            flags |= resolveX(b, dx > 0 ? RIGHT : LEFT, solids);
        }
        float prevBottom = b.bottom();
        b.y += dy;
        if (dy >= 0) {
            flags |= resolveFloor(b, prevBottom, grounded, solids);
        } else {
            flags |= resolveCeiling(b, solids);
        }
        return flags;
    }

    public boolean blocked(Body b, int side, List<Rect> solids) {
        lastProbeCling = false;
        float edge = side == RIGHT ? b.right() : b.left() - 1;
        for (float py : sidePoints(b)) {
            int v = grid.at(edge, py);
            if (TileRules.isWall(v)) {
                lastProbeCling = TileRules.allowsWallCling(v);
                return true;
            }
        }
        Rect probe = new Rect(side == RIGHT ? b.right() : b.left() - 1, b.top() + 2, 1, b.boxHalfH * 2 - 6);
        for (Rect r : solids) {
            if (r.overlaps(probe)) {
                lastProbeCling = true;
                return true;
            }
        }
        return false;
    }

    public boolean onFloor(Body b, List<Rect> solids) {
        float y = b.y;
        boolean slope = lastOnSlope;
        boolean spike = lastSpike;
        float prevBottom = b.bottom();
        b.y += 1;
        boolean found = floorAt(b, prevBottom, false, solids);
        b.y = y;
        lastOnSlope = slope;
        lastSpike = spike;
        return found;
    }

    private float[] sidePoints(Body b) {
        return new float[] {b.top() + 2, b.centerY(), b.bottom() - 4};
    }

    private int resolveX(Body b, int side, List<Rect> solids) {
        float edge = side == RIGHT ? b.right() - EPS : b.left();
        for (float py : sidePoints(b)) {
            int v = grid.at(edge, py);
            if (TileRules.isSpike(v)) {
                lastSpike = true;
            }
            if (TileRules.isWall(v)) {
                int cell = (int) Math.floor(edge / 16f);
                if (side == RIGHT) {
                    b.x -= b.right() - cell * 16f;
                } else {
                    b.x += (cell + 1) * 16f - b.left();
                }
                lastWallCling = TileRules.allowsWallCling(v);
                return side;
            }
        }
        Rect inner = new Rect(b.left(), b.top() + 2, b.boxHalfW * 2, b.boxHalfH * 2 - 6);
        for (Rect r : solids) {
            if (r.overlaps(inner)) {
                if (side == RIGHT) {
                    b.x -= b.right() - r.x();
                } else {
                    b.x += r.right() - b.left();
                }
                lastWallCling = true;
                return side;
            }
        }
        return 0;
    }

    private int resolveFloor(Body b, float prevBottom, boolean grounded, List<Rect> solids) {
        if (floorAt(b, prevBottom, grounded, solids)) {
            return FLOOR;
        }
        if (grounded) {
            float y = b.y;
            float bottom = b.bottom();
            b.y += 16;
            if (floorAt(b, prevBottom, false, solids) && (lastOnSlope || b.bottom() - bottom <= MAX_FLAT_STEP_DOWN)) {
                return FLOOR;
            }
            lastOnSlope = false;
            b.y = y;
        }
        return 0;
    }

    private boolean floorAt(Body b, float prevBottom, boolean grounded, List<Rect> solids) {
        float feet = b.bottom();
        float cx = b.centerX();
        int cellX = (int) Math.floor(cx / 16f);
        int cellY = (int) Math.floor(feet / 16f);
        int v = grid.raw(cellX, cellY);
        if (v == 16) {
            cellY -= 1;
            v = grid.raw(cellX, cellY);
        }
        if (TileRules.isSlope(v)) {
            float surface = cellY * 16f + TileRules.slopeSurface(v, (int) Math.floor(cx) - cellX * 16);
            if (grounded || feet >= surface) {
                b.y -= feet - surface;
                lastOnSlope = true;
                return true;
            }
            return false;
        }
        for (float px : new float[] {b.left() + 1, cx, b.right() - 1}) {
            int fv = grid.at(px, feet);
            if (!TileRules.isFlatFloor(fv)) {
                continue;
            }
            float cellTop = (float) Math.floor(feet / 16f) * 16f;
            if (TileRules.isOneWay(fv) && prevBottom > cellTop + EPS) {
                continue;
            }
            if (TileRules.isSpike(fv)) {
                lastSpike = true;
            }
            b.y -= feet - cellTop;
            return true;
        }
        Rect box = b.box();
        for (Rect r : solids) {
            if (box.overlaps(r) || (feet == r.y() && box.x() < r.right() && r.x() < box.right())) {
                if (prevBottom <= r.y() + EPS) {
                    b.y -= feet - r.y();
                    return true;
                }
            }
        }
        return false;
    }

    private int resolveCeiling(Body b, List<Rect> solids) {
        float top = b.top();
        for (float px : new float[] {b.left() + 1, b.centerX(), b.right() - 1}) {
            int v = grid.at(px, top);
            if (TileRules.isCeiling(v)) {
                if (TileRules.isSpike(v)) {
                    lastSpike = true;
                }
                b.y += (float) (Math.floor(top / 16f) + 1) * 16f - top;
                return CEILING;
            }
        }
        Rect box = b.box();
        for (Rect r : solids) {
            if (box.overlaps(r)) {
                b.y += r.bottom() - top;
                return CEILING;
            }
        }
        return 0;
    }
}
