package mmx.x6.render;

import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import mmx.x6.level.TileLayer;

public final class LevelRenderer {

    public static final int VIEW_W = 320;
    public static final int VIEW_H = 240;

    private final Assets assets;

    public LevelRenderer(Assets assets) {
        this.assets = assets;
    }

    public void draw(SpriteBatch batch, TileLayer layer, float camX, float camY) {
        int lx = (int) Math.floor(camX);
        int ly = (int) Math.floor(camY);
        int c0 = Math.floorDiv(lx, 16);
        int r0 = Math.floorDiv(ly, 16);
        for (int r = r0; r <= r0 + VIEW_H / 16 + 1; r++) {
            for (int c = c0; c <= c0 + VIEW_W / 16 + 1; c++) {
                int gid = layer.gid(c, r);
                if (gid == 0) {
                    continue;
                }
                TextureRegion t = assets.tile(gid);
                if (t != null) {
                    batch.draw(t, c * 16 - lx, r * 16 - ly);
                }
            }
        }
    }
}
