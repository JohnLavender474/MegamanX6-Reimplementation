package mmx.x6.screen;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.ScreenAdapter;
import com.badlogic.gdx.graphics.Color;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.glutils.ShaderProgram;
import com.badlogic.gdx.graphics.glutils.FrameBuffer;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.utils.ScreenUtils;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import mmx.x6.AutoRun;
import mmx.x6.audio.MusicPlayer;
import mmx.x6.audio.Sfx;
import mmx.x6.game.Entity;
import mmx.x6.game.Progress;
import mmx.x6.game.ShapeFx;
import mmx.x6.game.StageFlow;
import mmx.x6.render.Hud;
import mmx.x6.game.player.Player;
import mmx.x6.game.StepClock;
import mmx.x6.game.Weapon;
import mmx.x6.game.World;
import mmx.x6.input.Bindings;
import mmx.x6.input.Buttons;
import mmx.x6.input.Controls;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.Level;
import mmx.x6.render.Assets;
import mmx.x6.render.DebugOverlay;
import mmx.x6.render.EntityRenderer;
import mmx.x6.render.FlashShader;
import mmx.x6.render.LevelRenderer;
import mmx.x6.stage.StageDef;

public final class StageScreen extends ScreenAdapter {

    private static final int SCALE = 3;

    private final Assets assets;
    private final Controls controls;
    private final Level level;
    private final MusicPlayer music;
    private final World world;
    private final LevelRenderer levelRenderer;
    private final StepClock clock = new StepClock();
    private final FrameBuffer fbo;
    private final SpriteBatch batch = new SpriteBatch();
    private final SpriteBatch screenBatch = new SpriteBatch();
    private final ShaderProgram flashShader = FlashShader.create();
    private final ShapeRenderer shapes = new ShapeRenderer();
    private final OrthographicCamera viewCam = new OrthographicCamera();
    private final OrthographicCamera screenCam = new OrthographicCamera();
    private final ShotClock shots = ShotClock.fromProperties();
    private final int scriptedHold = Integer.getInteger("mmx.hold", -1);
    private final int jumpEvery = Integer.getInteger("mmx.jumpEvery", 0);

    private final Runnable exit;
    private final Runnable cleared;
    private final boolean auto = AutoRun.active();
    private final Hud hud = new Hud();
    private boolean debug = Boolean.getBoolean("mmx.debug");
    private int updates;

    public StageScreen(Assets assets, Controls controls, StageDef stage, Level level, Sfx sfx, MusicPlayer music, Progress progress,
                       Runnable exit, Runnable cleared) {
        this.assets = assets;
        this.controls = controls;
        this.level = level;
        this.music = music;
        this.exit = exit;
        this.cleared = cleared;
        this.world = new World(stage, level, assets::anim, sfx, music);
        world.progress = progress;
        Integer startCheckpoint = Integer.getInteger("mmx.checkpoint");
        int checkpoints = stage.checkpoints().size();
        if (startCheckpoint != null && (startCheckpoint < 0 || startCheckpoint >= checkpoints)) {
            System.err.println("mmx.checkpoint=" + startCheckpoint + " is ignored: the stage has checkpoints 0 to "
                    + (checkpoints - 1));
        } else if (startCheckpoint != null) {
            world.flow.restart(startCheckpoint);
        }
        Integer startX = Integer.getInteger("mmx.startX");
        Integer startY = Integer.getInteger("mmx.startY");
        if (startX != null && startY != null) {
            world.camera.reset(new Checkpoint(startX, startY, startX - 160, startY - 128, 0, level.width() * 16 - 320,
                    0, level.height() * 16 - 240, true, 1));
            world.camera.setFollow(true);
            world.player.placeStanding(startX, startY);
        }
        music.play(stage.music());
        this.levelRenderer = new LevelRenderer(assets);
        fbo = new FrameBuffer(Pixmap.Format.RGBA8888, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H, false);
        fbo.getColorBufferTexture().setFilter(Texture.TextureFilter.Nearest, Texture.TextureFilter.Nearest);
        viewCam.setToOrtho(true, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H);
        screenCam.setToOrtho(false, LevelRenderer.VIEW_W * SCALE, LevelRenderer.VIEW_H * SCALE);
    }

    @Override
    public void render(float delta) {
        controls.poll();
        if (!auto && keys()) {
            return;
        }
        int n = shots.active() ? 1 : clock.advance(delta);
        for (int i = 0; i < n; i++) {
            if (scriptedHold >= 0) {
                int dirs = scriptedHold & (Buttons.LEFT | Buttons.RIGHT | Buttons.UP | Buttons.DOWN);
                if (jumpEvery > 0) {
                    boolean on = updates % jumpEvery < jumpEvery * 2 / 3;
                    world.buttons.set(dirs | (on ? (scriptedHold & ~dirs) | Buttons.JUMP : 0));
                } else {
                    world.buttons.set(scriptedHold);
                }
            } else if (auto) {
                world.buttons.set(0);
            } else {
                world.buttons.set(controls.held(Gdx.input::isKeyPressed));
            }
            world.update();
            updates++;
        }
        music.update();
        drawWorld();
        String shot = shots.due(updates);
        if (shot != null) {
            saveShot(shot);
            if (shots.finished()) {
                Gdx.app.exit();
            }
        }
        ScreenUtils.clear(0, 0, 0, 1);
        screenBatch.setProjectionMatrix(screenCam.combined);
        screenBatch.begin();
        screenBatch.draw(fbo.getColorBufferTexture(), 0, 0, LevelRenderer.VIEW_W * SCALE,
                LevelRenderer.VIEW_H * SCALE, 0, 0, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H, false, true);
        screenBatch.end();
        AutoRun.drawBorder(shapes, screenCam);
    }

    private boolean keys() {
        if (Gdx.input.isKeyJustPressed(Keys.ESCAPE)) {
            exit.run();
            return true;
        }
        if (Gdx.input.isKeyJustPressed(Keys.F1)) {
            debug = !debug;
        }
        if (world.flow.state() == StageFlow.State.CLEAR) {
            if (Gdx.input.isKeyJustPressed(Keys.ANY_KEY) || controls.padPressed() != Bindings.NONE) {
                cleared.run();
                return true;
            }
        } else if (Gdx.input.isKeyJustPressed(Keys.R)) {
            world.flow.restart(0);
        }
        return false;
    }

    private void drawWorld() {
        float cx = world.camera.x();
        float cy = world.camera.y();
        fbo.begin();
        ScreenUtils.clear(0, 0, 0, 1);
        batch.setProjectionMatrix(viewCam.combined);
        batch.begin();
        if (!world.layer2Front) {
            drawLayer2();
        }
        if (!world.layer1Front && !world.layer1OverMain) {
            drawLayer1(cx, cy);
        }
        levelRenderer.draw(batch, level.art0Main(), cx, cy);
        if (world.layer1OverMain) {
            drawLayer1(cx, cy);
        }
        if (world.bgDark > 0) {
            batch.setColor(0, 0, 0, world.bgDark);
            batch.draw(assets.white(), 0, 0, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H);
            batch.setColor(1, 1, 1, 1);
        }
        List<Entity> drawn = new ArrayList<>(world.entities);
        drawn.sort(Comparator.comparingInt(Entity::drawPriority).reversed());
        // TODO: the docs do not say which draw priorities are in front of X; negative ones are drawn in front (docs/gaps.md, "Draw order")
        for (Entity e : drawn) {
            if (e.drawPriority() >= 0) {
                drawEntity(e, cx, cy);
            }
        }
        drawEntity(world.player, cx, cy);
        for (Entity e : drawn) {
            if (e.drawPriority() < 0) {
                drawEntity(e, cx, cy);
            }
        }
        levelRenderer.draw(batch, level.art0Front(), cx, cy);
        if (world.layer1Front) {
            drawLayer1(cx, cy);
        }
        if (world.layer2Front) {
            drawLayer2();
        }
        batch.end();
        drawShapes(cx, cy);
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
                if (e instanceof Weapon wp) {
                    DebugOverlay.drawBox(shapes, wp.attackBox(), Color.YELLOW, cx, cy);
                }
            }
            shapes.end();
        }
        fbo.end();
    }

    private void drawLayer1(float cx, float cy) {
        if (world.layer1Alpha <= 0) {
            return;
        }
        float l1x = world.camera.layer1X() + (world.layer1Relative ? cx : 0);
        float l1y = world.camera.layer1Y() + (world.layer1Relative ? cy : 0);
        batch.setColor(1, 1, 1, world.layer1Alpha);
        levelRenderer.draw(batch, level.art1(), l1x, l1y);
        batch.setColor(1, 1, 1, 1);
    }

    private void drawLayer2() {
        if (world.layer2Alpha <= 0) {
            return;
        }
        batch.setColor(1, 1, 1, world.layer2Alpha);
        levelRenderer.draw(batch, level.art2(), world.camera.layer2X(), world.camera.layer2Y());
        batch.setColor(1, 1, 1, 1);
    }

    private void drawShapes(float cx, float cy) {
        Gdx.gl.glEnable(GL20.GL_BLEND);
        Gdx.gl.glBlendFunc(GL20.GL_SRC_ALPHA, GL20.GL_ONE_MINUS_SRC_ALPHA);
        shapes.setProjectionMatrix(viewCam.combined);
        shapes.begin(ShapeRenderer.ShapeType.Filled);
        int ix = (int) Math.floor(cx);
        int iy = (int) Math.floor(cy);
        for (Entity e : world.entities) {
            for (ShapeFx s : e.shapes()) {
                shapes.setColor(s.r(), s.g(), s.b(), s.a());
                shapes.circle(s.x() - ix, s.y() - iy, s.radius(), 16);
            }
        }
        Player p = world.player;
        if (p.chargeLevel() > 0 && p.visible()) {
            float speed = p.chargeLevel() == 2 ? 0.35f : 0.2f;
            float base = world.frame * speed;
            float radius = 18;
            for (int i = 0; i < 4; i++) {
                double a = base + i * Math.PI / 2;
                float ox = (float) Math.cos(a) * radius;
                float oy = (float) Math.sin(a) * radius * 0.8f;
                shapes.setColor(0.3f, 0.6f, 1f, 0.45f);
                shapes.circle(p.x + ox - ix, p.y + oy - iy, 4, 10);
                shapes.setColor(0.8f, 0.95f, 1f, 0.9f);
                shapes.circle(p.x + ox - ix, p.y + oy - iy, 2, 8);
            }
        }
        hud.drawShapes(shapes, world);
        float fade = Math.max(world.flow.fade(), world.screenFade);
        if (fade > 0) {
            shapes.setColor(0, 0, 0, fade);
            shapes.rect(0, 0, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H);
        }
        shapes.end();
        batch.begin();
        hud.drawText(batch, world);
        batch.end();
    }

    private void drawEntity(Entity e, float cx, float cy) {
        if (!e.visible() || e.spriteSet() == null || e.anim() == null || e.anim().anim() < 0) {
            return;
        }
        Assets.SpriteFrame f = assets.frame(e.spriteSet(), e.anim().anim(), e.anim().frameIndex());
        if (e.flash > 0) {
            // TODO: the exports are RGBA, so the flash CLUT 0x7840 is applied by brightness, not by palette index (docs/gaps.md, "Hit flash")
            batch.setShader(flashShader);
            EntityRenderer.draw(batch, f, e.x, e.y, e.faceRight, cx, cy);
            batch.setShader(null);
            return;
        }
        EntityRenderer.draw(batch, f, e.x, e.y, e.faceRight, cx, cy);
    }

    private void saveShot(String path) {
        fbo.begin();
        Pixmap p = Pixmap.createFromFrameBuffer(0, 0, LevelRenderer.VIEW_W, LevelRenderer.VIEW_H);
        fbo.end();
        Shots.write(path, p);
    }

    @Override
    public void dispose() {
        fbo.dispose();
        batch.dispose();
        flashShader.dispose();
        screenBatch.dispose();
        shapes.dispose();
        hud.dispose();
    }
}
