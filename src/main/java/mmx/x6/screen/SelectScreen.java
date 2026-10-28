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
import java.util.function.Consumer;
import mmx.x6.AutoRun;
import mmx.x6.input.Buttons;
import mmx.x6.input.Controls;
import mmx.x6.stage.StageDef;

public final class SelectScreen extends ScreenAdapter {

    private static final int W = 960;
    private static final int H = 720;

    private static final int PICK = Buttons.START | Buttons.JUMP | Buttons.BUSTER;

    private final Controls controls;
    private final Buttons buttons = new Buttons();
    private final List<StageDef> stages;
    private final Consumer<StageDef> start;
    private final SpriteBatch batch = new SpriteBatch();
    private final BitmapFont font = new BitmapFont();
    private final GlyphLayout layout = new GlyphLayout();
    private final OrthographicCamera cam = new OrthographicCamera();
    private final ShapeRenderer shapes = new ShapeRenderer();
    private final ShotClock shots = ShotClock.fromProperties();
    private int updates;
    private int index;

    public SelectScreen(Controls controls, List<StageDef> stages, int index, Consumer<StageDef> start) {
        this.controls = controls;
        this.stages = stages;
        this.index = index;
        this.start = start;
        font.getData().setScale(3);
        cam.setToOrtho(false, W, H);
        controls.poll();
        buttons.set(controls.held(Gdx.input::isKeyPressed));
    }

    @Override
    public void render(float delta) {
        controls.poll();
        if (!AutoRun.active() && keys()) {
            return;
        }
        ScreenUtils.clear(0.02f, 0.03f, 0.08f, 1);
        batch.setProjectionMatrix(cam.combined);
        batch.begin();
        centred("STAGE SELECT", H - 120, 0.6f, 0.85f, 1f);
        for (int i = 0; i < stages.size(); i++) {
            boolean on = i == index;
            String text = (on ? "> " : "  ") + stages.get(i).title() + (on ? " <" : "  ");
            centred(text, H - 220 - i * 62, on ? 1f : 0.55f, on ? 1f : 0.6f, on ? 0.4f : 0.7f);
        }
        centred("UP / DOWN  CHOOSE     START  PLAY", 60, 0.5f, 0.6f, 0.7f);
        batch.end();
        Shots.saveWindow(shots, ++updates);
        AutoRun.drawBorder(shapes, cam);
    }

    private boolean keys() {
        if (Gdx.input.isKeyJustPressed(Keys.ESCAPE)) {
            Gdx.app.exit();
            return true;
        }
        buttons.set(controls.held(Gdx.input::isKeyPressed));
        if (buttons.isPressed(Buttons.UP)) {
            index = (index + stages.size() - 1) % stages.size();
        }
        if (buttons.isPressed(Buttons.DOWN)) {
            index = (index + 1) % stages.size();
        }
        if ((buttons.pressed() & PICK) != 0) {
            start.accept(stages.get(index));
            return true;
        }
        return false;
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
