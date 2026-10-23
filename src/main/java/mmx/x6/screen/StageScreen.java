package mmx.x6.screen;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.ScreenAdapter;
import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.graphics.Color;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.PixmapIO;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.glutils.FrameBuffer;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.utils.ScreenUtils;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import mmx.x6.audio.MusicPlayer;
import mmx.x6.audio.Sfx;
import mmx.x6.game.Entity;
import mmx.x6.game.StepClock;
import mmx.x6.game.World;
import mmx.x6.level.Level;
import mmx.x6.render.Assets;
import mmx.x6.render.DebugOverlay;
import mmx.x6.render.EntityRenderer;
import mmx.x6.render.LevelRenderer;

public final class StageScreen extends ScreenAdapter {

    private static final int SCALE = 3;

    private final Assets assets;
    private final Level level;
    private final MusicPlayer music;
    private final World world;
    private final LevelRenderer levelRenderer;
    private final StepClock clock = new StepClock();
    private final FrameBuffer fbo;
    private final SpriteBatch batch = new SpriteBatch();
    private final SpriteBatch screenBatch = new SpriteBatch();
    private final ShapeRenderer shapes = new ShapeRenderer();
    private final OrthographicCamera viewCam = new OrthographicCamera();
    private final OrthographicCamera screenCam = new OrthographicCamera();
    private final String shotPath = System.getProperty("mmx.shot");
    private final int shotAfter = Integer.getInteger("mmx.shotAfter", 30);
    private final int scriptedHold = Integer.getInteger("mmx.hold", -1);

    private boolean debug = Boolean.getBoolean("mmx.debug");
    private int updates;

    public StageScreen(Assets assets, Level level, Sfx sfx, MusicPlayer music) {
        this.assets = assets;
        this.level = level;
        this.music = music;
        this.world = new World(level, assets::anim, sfx, music);
        Integer startX = Integer.getInteger("mmx.startX");
        Integer startY = Integer.getInteger("mmx.startY");
        if (startX != null && startY != null) {
            world.player.placeStanding(startX, startY);
        }
        music.play(9);
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
            if (scriptedHold >= 0) {
                world.buttons.set(scriptedHold);
            } else {
                world.buttons.sample(Gdx.input::isKeyPressed);
            }
            world.update();
            updates++;
        }
        music.update();
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

    private void drawWorld() {
        float cx = world.camera.x();
        float cy = world.camera.y();
        fbo.begin();
        ScreenUtils.clear(0, 0, 0, 1);
        batch.setProjectionMatrix(viewCam.combined);
        batch.begin();
        levelRenderer.draw(batch, level.art2(), world.camera.layer2X(), world.camera.layer2Y());
        levelRenderer.draw(batch, level.art1(), world.camera.layer1X(), world.camera.layer1Y());
        levelRenderer.draw(batch, level.art0Main(), cx, cy);
        List<Entity> drawn = new ArrayList<>(world.entities);
        drawn.sort(Comparator.comparingInt(Entity::drawPriority).reversed());
        for (Entity e : drawn) {
            drawEntity(e, cx, cy);
        }
        drawEntity(world.player, cx, cy);
        levelRenderer.draw(batch, level.art0Front(), cx, cy);
        batch.end();
        if (debug) {
            Gdx.gl.glEnable(GL20.GL_BLEND);
            shapes.setProjectionMatrix(viewCam.combined);
            shapes.begin(ShapeRenderer.ShapeType.Filled);
            DebugOverlay.drawGrid(shapes, level.collision(), cx, cy);
            shapes.end();
            shapes.begin(ShapeRenderer.ShapeType.Line);
            DebugOverlay.drawBox(shapes, world.player.hurtBox(), Color.GREEN, cx, cy);
            for (Entity e : world.entities) {
                DebugOverlay.drawBox(shapes, e.hurtBox(), Color.BLUE, cx, cy);
                DebugOverlay.drawBox(shapes, e.contactBox(), Color.RED, cx, cy);
            }
            shapes.end();
        }
        fbo.end();
    }

    private void drawEntity(Entity e, float cx, float cy) {
        if (!e.visible() || e.spriteSet() == null || e.anim() == null || e.anim().anim() < 0) {
            return;
        }
        Assets.SpriteFrame f = assets.frame(e.spriteSet(), e.anim().anim(), e.anim().frameIndex());
        EntityRenderer.draw(batch, f, e.x, e.y, e.faceRight, cx, cy);
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
