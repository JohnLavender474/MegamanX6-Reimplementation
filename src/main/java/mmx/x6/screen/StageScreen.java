package mmx.x6.screen;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.ScreenAdapter;
import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.PixmapIO;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.glutils.FrameBuffer;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.utils.ScreenUtils;
import mmx.x6.game.StepClock;
import mmx.x6.input.Buttons;
import mmx.x6.level.Level;
import mmx.x6.render.Assets;
import mmx.x6.render.DebugOverlay;
import mmx.x6.render.LevelRenderer;

public final class StageScreen extends ScreenAdapter {

    private static final int SCALE = 3;

    private final Assets assets;
    private final Level level;
    private final LevelRenderer levelRenderer;
    private final StepClock clock = new StepClock();
    private final Buttons buttons = new Buttons();
    private final FrameBuffer fbo;
    private final SpriteBatch batch = new SpriteBatch();
    private final SpriteBatch screenBatch = new SpriteBatch();
    private final ShapeRenderer shapes = new ShapeRenderer();
    private final OrthographicCamera viewCam = new OrthographicCamera();
    private final OrthographicCamera screenCam = new OrthographicCamera();
    private final String shotPath = System.getProperty("mmx.shot");
    private final int shotAfter = Integer.getInteger("mmx.shotAfter", 30);

    private boolean debug = Boolean.getBoolean("mmx.debug");
    private float camX = Integer.getInteger("mmx.camX", 0);
    private float camY = Integer.getInteger("mmx.camY", 944);
    private int updates;

    public StageScreen(Assets assets, Level level) {
        this.assets = assets;
        this.level = level;
        this.levelRenderer = new LevelRenderer(assets);
        fbo = new FrameBuffer(Pixmap.Format.RGBA8888, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H, false);
        fbo.getColorBufferTexture().setFilter(Texture.TextureFilter.Nearest, Texture.TextureFilter.Nearest);
        viewCam.setToOrtho(true, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H);
        screenCam.setToOrtho(false, LevelRenderer.VIEW_W * SCALE, LevelRenderer.VIEW_H * SCALE);
    }

    @Override
    public void render(float delta) {
        if (Gdx.input.isKeyJustPressed(Keys.ESCAPE)) {
            Gdx.app.exit();
            return;
        }
        if (Gdx.input.isKeyJustPressed(Keys.F1)) {
            debug = !debug;
        }
        int n = shotPath != null ? 1 : clock.advance(delta);
        for (int i = 0; i < n; i++) {
            buttons.sample(Gdx.input::isKeyPressed);
            update();
            updates++;
        }
        drawWorld();
        if (shotPath != null && updates >= shotAfter) {
            saveShot();
            Gdx.app.exit();
        }
        ScreenUtils.clear(0, 0, 0, 1);
        screenBatch.setProjectionMatrix(screenCam.combined);
        screenBatch.begin();
        screenBatch.draw(fbo.getColorBufferTexture(), 0, 0, LevelRenderer.VIEW_W * SCALE,
                LevelRenderer.VIEW_H * SCALE, 0, 0, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H, false, true);
        screenBatch.end();
    }

    private void update() {
        float speed = 8;
        if (buttons.isHeld(Buttons.RIGHT)) {
            camX += speed;
        }
        if (buttons.isHeld(Buttons.LEFT)) {
            camX -= speed;
        }
        if (buttons.isHeld(Buttons.DOWN)) {
            camY += speed;
        }
        if (buttons.isHeld(Buttons.UP)) {
            camY -= speed;
        }
    }

    private void drawWorld() {
        fbo.begin();
        ScreenUtils.clear(0, 0, 0, 1);
        batch.setProjectionMatrix(viewCam.combined);
        batch.begin();
        levelRenderer.draw(batch, level.art2(), camX, camY);
        levelRenderer.draw(batch, level.art1(), camX, camY);
        levelRenderer.draw(batch, level.art0Main(), camX, camY);
        levelRenderer.draw(batch, level.art0Front(), camX, camY);
        batch.end();
        if (debug) {
            Gdx.gl.glEnable(GL20.GL_BLEND);
            shapes.setProjectionMatrix(viewCam.combined);
            shapes.begin(ShapeRenderer.ShapeType.Filled);
            DebugOverlay.drawGrid(shapes, level.collision(), camX, camY);
            shapes.end();
        }
        fbo.end();
    }

    private void saveShot() {
        fbo.begin();
        Pixmap p = Pixmap.createFromFrameBuffer(0, 0, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H);
        fbo.end();
        PixmapIO.writePNG(new FileHandle(shotPath), p, 0, true);
        p.dispose();
    }

    @Override
    public void dispose() {
        fbo.dispose();
        batch.dispose();
        screenBatch.dispose();
        shapes.dispose();
    }
}
