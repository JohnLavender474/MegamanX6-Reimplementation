package mmx.x6.screen;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.ScreenAdapter;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.GlyphLayout;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.utils.ScreenUtils;
import java.util.List;
import mmx.x6.AutoRun;
import mmx.x6.level.Finding;

public final class LevelErrorScreen extends ScreenAdapter {

    private static final int W = 960;
    private static final int H = 720;
    private static final int LINES = 14;

    private final List<String> lines;
    private final Runnable close;
    private final SpriteBatch batch = new SpriteBatch();
    private final BitmapFont font = new BitmapFont();
    private final GlyphLayout layout = new GlyphLayout();
    private final OrthographicCamera cam = new OrthographicCamera();
    private final ShapeRenderer shapes = new ShapeRenderer();
    private final ShotClock shots = ShotClock.fromProperties();
    private int updates;

    public LevelErrorScreen(List<Finding> findings, Runnable close) {
        this.close = close;
        this.lines = findings.stream()
                .filter(f -> f.result() == Finding.Result.REJECT)
                .map(f -> f.code() + ": " + f.message())
                .toList();
        font.getData().setScale(1.4f);
        cam.setToOrtho(false, W, H);
    }

    @Override
    public void render(float delta) {
        if (!AutoRun.active() && Gdx.input.isKeyJustPressed(Keys.ANY_KEY)) {
            close.run();
            return;
        }
        ScreenUtils.clear(0.1f, 0.02f, 0.02f, 1);
        batch.setProjectionMatrix(cam.combined);
        batch.begin();
        font.getData().setScale(3);
        centred("LEVEL REJECTED", H - 80, 1f, 0.4f, 0.4f);
        font.getData().setScale(1.4f);
        font.setColor(1, 1, 1, 1);
        int shown = Math.min(lines.size(), LINES);
        for (int i = 0; i < shown; i++) {
            font.draw(batch, lines.get(i), 30, H - 170 - i * 36, W - 60, -1, true);
        }
        if (lines.size() > shown) {
            font.draw(batch, "... and " + (lines.size() - shown) + " more", 30, H - 170 - shown * 36);
        }
        centred("PRESS ANY KEY TO CLOSE", 50, 0.6f, 0.7f, 0.8f);
        batch.end();
        Shots.saveWindow(shots, ++updates);
        AutoRun.drawBorder(shapes, cam);
    }

    private void centred(String text, float y, float r, float g, float b) {
        font.setColor(r, g, b, 1);
        layout.setText(font, text);
        font.draw(batch, text, (W - layout.width) / 2, y);
    }

    @Override
    public void dispose() {
        batch.dispose();
        font.dispose();
        shapes.dispose();
    }
}
