package mmx.x6.render;

import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.GlyphLayout;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.utils.Disposable;
import mmx.x6.game.StageFlow;
import mmx.x6.game.World;
import mmx.x6.game.boss.BossWarning;
import mmx.x6.game.player.PlayerConst;

public final class Hud implements Disposable {

    private static final int BAR_TOP = 40;
    private static final int UNIT = 2;

    private final BitmapFont font = new BitmapFont(true);
    private final GlyphLayout layout = new GlyphLayout();

    public void drawShapes(ShapeRenderer sr, World w) {
        drawBar(sr, 12, w.player.hp, PlayerConst.MAX_HP, 0.2f, 0.85f, 1f);
        if (w.bossBarShown) {
            drawBar(sr, LevelRenderer.VIEW_W - 20, w.bossHp, Math.max(1, w.bossHpMax), 1f, 0.35f, 0.2f);
        }
        if (w.whiteLevel > 0) {
            sr.setColor(1, 1, 1, w.whiteLevel);
            sr.rect(0, 0, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H);
        }
        if (w.flow.state() == StageFlow.State.CLEAR) {
            sr.setColor(0, 0, 0, 0.5f);
            sr.rect(0, 96, LevelRenderer.VIEW_W, 48);
        }
    }

    private static void drawBar(ShapeRenderer sr, int x, int value, int max, float r, float g, float b) {
        int height = max * UNIT;
        sr.setColor(0.08f, 0.08f, 0.12f, 0.9f);
        sr.rect(x - 2, BAR_TOP - 2, 12, height + 4);
        sr.setColor(0.25f, 0.25f, 0.3f, 0.9f);
        sr.rect(x, BAR_TOP, 8, height);
        int filled = Math.max(0, Math.min(max, value)) * UNIT;
        sr.setColor(r, g, b, 1);
        sr.rect(x, BAR_TOP + height - filled, 8, filled);
    }

    public void drawText(SpriteBatch batch, World w) {
        boolean warning = w.entities.stream()
                .anyMatch(e -> e instanceof BossWarning bw && !bw.dead && bw.textVisible());
        if (warning) {
            centred(batch, "WARNING", 112, 1f, 0.2f, 0.2f);
        }
        if (w.message != null) {
            centred(batch, w.message, 64, 1f, 1f, 0.6f);
        }
        font.setColor(0.85f, 0.9f, 1f, 1);
        font.draw(batch, "x" + w.progress.lives, 8, BAR_TOP + PlayerConst.MAX_HP * UNIT + 6);
        font.draw(batch, "SOULS " + w.progress.souls, LevelRenderer.VIEW_W - 76, 6);
        if (w.flow.state() == StageFlow.State.CLEAR) {
            centred(batch, "STAGE CLEAR", 102, 1f, 1f, 1f);
            centred(batch, "PRESS ANY KEY", 122, 0.8f, 0.9f, 1f);
        }
    }

    private void centred(SpriteBatch batch, String text, float y, float r, float g, float b) {
        font.setColor(r, g, b, 1);
        layout.setText(font, text);
        font.draw(batch, text, (LevelRenderer.VIEW_W - layout.width) / 2, y);
    }

    @Override
    public void dispose() {
        font.dispose();
    }
}
