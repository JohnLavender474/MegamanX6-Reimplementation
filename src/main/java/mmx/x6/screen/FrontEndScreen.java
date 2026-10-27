package mmx.x6.screen;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.ScreenAdapter;
import com.badlogic.gdx.audio.Music;
import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.GlyphLayout;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.utils.ScreenUtils;
import java.nio.file.Path;
import mmx.x6.AutoRun;
import mmx.x6.DataPaths;
import mmx.x6.audio.Sfx;
import mmx.x6.frontend.FrontEnd;
import mmx.x6.frontend.FrontEndText;
import mmx.x6.frontend.MovieFrames;
import mmx.x6.frontend.Phase;
import mmx.x6.game.StepClock;
import mmx.x6.input.Buttons;

public final class FrontEndScreen extends ScreenAdapter {

    private static final int W = 960;
    private static final int H = 720;
    private static final float MOVIE_SCALE = 3;
    private static final float WIDE_X = W / 512f;
    private static final float WIDE_Y = H / 240f;
    private static final int LOADING_MARGIN = 16;

    private final DataPaths paths;
    private final FrontEnd frontEnd;
    private final Runnable startGame;
    private final StepClock clock = new StepClock();
    private final Buttons buttons = new Buttons();
    private final SpriteBatch batch = new SpriteBatch();
    private final ShapeRenderer shapes = new ShapeRenderer();
    private final BitmapFont font = new BitmapFont();
    private final OrthographicCamera cam = new OrthographicCamera();
    private final MovieFrames logo;
    private final MovieFrames opening;
    private final Texture background;
    private final Texture textSheet;
    private final Texture vram;
    private final TextureRegion backgroundRegion;
    private final TextureRegion loadingRegion;
    private final ShotClock shots = ShotClock.fromProperties();
    private final int scriptedHold = Integer.getInteger("mmx.hold", -1);
    private final int jumpEvery = Integer.getInteger("mmx.jumpEvery", 0);
    private int updates;
    private Phase shownPhase;
    private Music music;
    private boolean gameStarted;

    public FrontEndScreen(DataPaths paths, Sfx sfx, Phase start, Runnable startGame) {
        this.paths = paths;
        this.frontEnd = new FrontEnd(sfx, start);
        this.startGame = startGame;
        cam.setToOrtho(false, W, H);
        font.getData().setScale(3);
        logo = new MovieFrames(i -> paths.movieFrame("caplogo", i));
        opening = new MovieFrames(i -> paths.movieFrame("x6op", i));
        background = load(paths.frontEndImage("stage14-area0-layer0.png"), false);
        textSheet = load(paths.frontEndImage("misc19-sprites.png"), true);
        vram = load(paths.frontEndImage("vram-512-256-clut7800.png"), false);
        backgroundRegion = region(background, FrontEndText.BACKGROUND_RECT);
        loadingRegion = region(vram, FrontEndText.LOADING_RECT);
    }

    private static Texture load(Path p, boolean keyOutBackground) {
        Pixmap pix = new Pixmap(new FileHandle(p.toFile()));
        if (keyOutBackground) {
            int bg = pix.getPixel(0, 0);
            pix.setBlending(Pixmap.Blending.None);
            for (int y = 0; y < pix.getHeight(); y++) {
                for (int x = 0; x < pix.getWidth(); x++) {
                    if (pix.getPixel(x, y) == bg) {
                        pix.drawPixel(x, y, 0);
                    }
                }
            }
        }
        Texture t = new Texture(pix);
        t.setFilter(Texture.TextureFilter.Nearest, Texture.TextureFilter.Nearest);
        pix.dispose();
        return t;
    }

    private static TextureRegion region(Texture t, int[] r) {
        return new TextureRegion(t, r[0], r[1], r[2], r[3]);
    }

    @Override
    public void render(float delta) {
        if (!AutoRun.active() && Gdx.input.isKeyJustPressed(Keys.ESCAPE)) {
            Gdx.app.exit();
            return;
        }
        int n = shots.active() ? 1 : clock.advance(delta);
        for (int i = 0; i < n && !gameStarted; i++) {
            frontEnd.update(held());
            updates++;
            followPhase();
            if (frontEnd.startGame()) {
                gameStarted = true;
                stopMusic();
                startGame.run();
            }
        }
        draw();
        Shots.saveWindow(shots, updates);
        AutoRun.drawBorder(shapes, cam);
    }

    private int held() {
        if (scriptedHold < 0) {
            if (AutoRun.active()) {
                return 0;
            }
            buttons.sample(Gdx.input::isKeyPressed);
            return buttons.held();
        }
        int dirs = scriptedHold & (Buttons.LEFT | Buttons.RIGHT | Buttons.UP | Buttons.DOWN);
        if (jumpEvery > 0) {
            boolean on = updates % jumpEvery < jumpEvery * 2 / 3;
            return dirs | (on ? (scriptedHold & ~dirs) | Buttons.JUMP : 0);
        }
        return scriptedHold;
    }

    private void followPhase() {
        Phase p = frontEnd.phase();
        if (p == shownPhase) {
            return;
        }
        shownPhase = p;
        stopMusic();
        if (p == Phase.LOGO || p == Phase.OPENING) {
            Path wav = paths.movieAudio(p == Phase.LOGO ? "caplogo" : "x6op");
            // TODO: the movie frames run on the update count (4 per frame) and the sound on its own clock; they can drift apart (docs/gaps.md, "Front end")
            music = Gdx.audio.newMusic(Gdx.files.absolute(wav.toString()));
            music.setVolume(0.6f);
            music.play();
        }
        if (p != Phase.LOGO) {
            logo.dispose();
        }
        if (p != Phase.OPENING) {
            opening.dispose();
        }
    }

    private void draw() {
        ScreenUtils.clear(0, 0, 0, 1);
        batch.setProjectionMatrix(cam.combined);
        batch.begin();
        Phase p = frontEnd.phase();
        if (p == Phase.READY) {
            drawCentered("Ready to play?", H / 2f + 60);
            drawCentered("Press START", H / 2f - 30);
        } else if (p == Phase.LOGO || p == Phase.OPENING) {
            Texture frame = (p == Phase.LOGO ? logo : opening).frame(frontEnd.movieFrame());
            if (frame != null) {
                batch.draw(frame, 0, 0, W, H);
            }
        } else if (p == Phase.LOAD) {
            int[] r = FrontEndText.LOADING_RECT;
            // TODO: the place of "Now Loading..." (bottom right, 16 px in) is a guess; no state shows it (docs/gaps.md, "Front end")
            float left = (320 - LOADING_MARGIN - r[2]) * MOVIE_SCALE;
            batch.draw(loadingRegion, left, LOADING_MARGIN * MOVIE_SCALE, r[2] * MOVIE_SCALE, r[3] * MOVIE_SCALE);
        } else {
            batch.draw(backgroundRegion, 0, 0, W, H);
            for (int v : p == Phase.TITLE ? FrontEndText.TITLE_VARS : FrontEndText.MENU_VARS) {
                if (frontEnd.textShown(v)) {
                    drawText(FrontEndText.text(v, frontEnd.row()));
                }
            }
        }
        batch.end();
        float dark = frontEnd.darkness();
        if (dark > 0) {
            Gdx.gl.glEnable(GL20.GL_BLEND);
            shapes.setProjectionMatrix(cam.combined);
            shapes.begin(ShapeRenderer.ShapeType.Filled);
            shapes.setColor(0, 0, 0, dark);
            shapes.rect(0, 0, W, H);
            shapes.end();
        }
    }

    private void drawCentered(String text, float y) {
        GlyphLayout layout = new GlyphLayout(font, text);
        font.draw(batch, layout, (W - layout.width) / 2, y);
    }

    private void drawText(FrontEndText.Text t) {
        FrontEndText.Sprite s = FrontEndText.sprite(t.sprite());
        int[] r = FrontEndText.sheetRect(t.sprite(), t.orange());
        float left = (t.x() - s.ox()) * WIDE_X;
        float top = (t.y() - s.oy()) * WIDE_Y;
        TextureRegion region = new TextureRegion(textSheet, r[0], r[1], r[2], r[3]);
        batch.draw(region, left, H - top - s.h() * WIDE_Y, s.w() * WIDE_X, s.h() * WIDE_Y);
    }

    private void stopMusic() {
        if (music != null) {
            music.stop();
            music.dispose();
            music = null;
        }
    }

    @Override
    public void dispose() {
        stopMusic();
        logo.dispose();
        opening.dispose();
        background.dispose();
        textSheet.dispose();
        vram.dispose();
        batch.dispose();
        shapes.dispose();
        font.dispose();
    }
}
