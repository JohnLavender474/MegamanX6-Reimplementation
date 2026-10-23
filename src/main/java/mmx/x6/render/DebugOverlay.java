package mmx.x6.render;

import com.badlogic.gdx.graphics.Color;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import mmx.x6.level.CollisionGrid;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class DebugOverlay {

    private DebugOverlay() {
    }

    public static Color colorOf(int v) {
        if (v == 0) {
            return null;
        }
        if (TileRules.isSlope(v)) {
            return Color.YELLOW;
        }
        if (TileRules.isSpike(v)) {
            return Color.RED;
        }
        if (v == 59) {
            return Color.MAGENTA;
        }
        if (TileRules.isLadder(v)) {
            return Color.GREEN;
        }
        if (TileRules.isOneWay(v)) {
            return Color.CYAN;
        }
        if (TileRules.isBar(v)) {
            return Color.ORANGE;
        }
        if (TileRules.isWall(v)) {
            return v == 57 ? Color.LIGHT_GRAY : Color.WHITE;
        }
        if (v == 16) {
            return Color.OLIVE;
        }
        return Color.PURPLE;
    }

    public static void drawGrid(ShapeRenderer sr, CollisionGrid grid, float camX, float camY) {
        int lx = (int) Math.floor(camX);
        int ly = (int) Math.floor(camY);
        int c0 = Math.floorDiv(lx, 16);
        int r0 = Math.floorDiv(ly, 16);
        for (int r = r0; r <= r0 + 16; r++) {
            for (int c = c0; c <= c0 + 21; c++) {
                Color col = colorOf(grid.raw(c, r));
                if (col != null) {
                    sr.setColor(col.r, col.g, col.b, 0.5f);
                    sr.rect(c * 16 - lx, r * 16 - ly, 16, 16);
                }
            }
        }
    }

    public static void drawBox(ShapeRenderer sr, Rect r, Color c, float camX, float camY) {
        if (r == null) {
            return;
        }
        sr.setColor(c);
        sr.rect(r.x() - (int) Math.floor(camX), r.y() - (int) Math.floor(camY), r.w(), r.h());
    }
}
