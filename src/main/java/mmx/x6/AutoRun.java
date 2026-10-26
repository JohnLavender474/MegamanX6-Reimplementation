package mmx.x6;

import com.badlogic.gdx.graphics.Camera;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;

public final class AutoRun {

    private static final float BORDER = 8;

    private AutoRun() {
    }

    public static boolean active() {
        return System.getProperty("mmx.shot") != null;
    }

    public static String title(String base) {
        return active() ? base + " - AUTOMATED RUN! NO INPUT" : base;
    }

    public static void drawBorder(ShapeRenderer shapes, Camera cam) {
        if (!active()) {
            return;
        }
        float w = cam.viewportWidth;
        float h = cam.viewportHeight;
        shapes.setProjectionMatrix(cam.combined);
        shapes.begin(ShapeRenderer.ShapeType.Filled);
        shapes.setColor(1, 0, 0, 1);
        shapes.rect(0, 0, w, BORDER);
        shapes.rect(0, h - BORDER, w, BORDER);
        shapes.rect(0, 0, BORDER, h);
        shapes.rect(w - BORDER, 0, BORDER, h);
        shapes.end();
    }
}
