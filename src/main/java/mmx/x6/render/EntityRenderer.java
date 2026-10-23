package mmx.x6.render;

import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public final class EntityRenderer {

    private EntityRenderer() {
    }

    public static void draw(SpriteBatch batch, Assets.SpriteFrame f, float x, float y, boolean faceRight,
                            float camX, float camY) {
        if (f == null) {
            return;
        }
        TextureRegion r = f.region();
        int sx = (int) Math.floor(x) - (int) Math.floor(camX);
        int sy = (int) Math.floor(y) - (int) Math.floor(camY);
        int w = r.getRegionWidth();
        int h = r.getRegionHeight();
        float top = sy - f.originY();
        if (faceRight) {
            batch.draw(r, sx + f.originX(), top, -w, h);
        } else {
            batch.draw(r, sx - f.originX(), top, w, h);
        }
    }
}
