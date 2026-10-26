package mmx.x6;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Screen;
import java.io.IOException;
import java.io.UncheckedIOException;
import mmx.x6.audio.CachedSfx;
import mmx.x6.audio.GdxMusicPlayer;
import mmx.x6.audio.GdxSounds;
import mmx.x6.audio.Sfx;
import mmx.x6.audio.SoundBanks;
import mmx.x6.game.Progress;
import mmx.x6.level.Level;
import mmx.x6.level.TmxLoader;
import mmx.x6.render.Assets;
import mmx.x6.screen.FrontEndScreen;
import mmx.x6.screen.SelectScreen;
import mmx.x6.screen.StageScreen;
import mmx.x6.stage.StageDef;
import mmx.x6.stage.Stages;

public final class X6Game extends Game {

    private static final int SELECT_MUSIC = 1;

    private final DataPaths paths;
    private Progress progress = new Progress();
    private GameFlow flow;
    private Assets assets;
    private CachedSfx sfx;
    private GdxMusicPlayer music;
    private int lastIndex;

    public X6Game(DataPaths paths) {
        this.paths = paths;
    }

    @Override
    public void create() {
        assets = new Assets(paths);
        music = new GdxMusicPlayer(paths);
        flow = new GameFlow(System.getProperty("mmx.stage"), System.getProperty("mmx.front"));
        switch (flow.first()) {
            case STAGE -> startStage(flow.stage());
            case SELECT -> showSelect();
            case FRONT_END -> showFrontEnd();
        }
    }

    private void show(GameFlow.Screen screen) {
        if (screen == GameFlow.Screen.FRONT_END) {
            showFrontEnd();
        } else {
            showSelect();
        }
    }

    private void showFrontEnd() {
        if (flow.fallback(paths.frontEndExists())) {
            System.err.println("front end data not found under " + paths.root() + "; showing the stage select");
            showSelect();
            return;
        }
        music.stop();
        swap(new FrontEndScreen(paths, systemSfx(), flow.frontStart(),
                () -> Gdx.app.postRunnable(this::startGame)));
    }

    private Sfx systemSfx() {
        if (sfx != null) {
            sfx.dispose();
        }
        sfx = new CachedSfx(SoundBanks.load(paths), GdxSounds::load, System.err::println);
        return sfx;
    }

    private void startGame() {
        progress = new Progress();
        startStage(Stages.INTRO);
    }

    private void showSelect() {
        music.play(SELECT_MUSIC);
        swap(new SelectScreen(Stages.BOSSES, lastIndex, s -> Gdx.app.postRunnable(() -> startStage(s))));
    }

    private void startStage(StageDef stage) {
        lastIndex = Math.max(0, Stages.BOSSES.indexOf(stage));
        Level level;
        try {
            level = TmxLoader.load(paths.stageTmx(stage.tmxDir(), stage.tmxFile()));
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        assets.useTiles(paths.stageTiles(stage.tmxDir()));
        if (sfx != null) {
            sfx.dispose();
        }
        sfx = new CachedSfx(SoundBanks.load(paths, stage.bank2(), stage.bank5(), stage.bossBank2()),
                GdxSounds::load, System.err::println);
        swap(new StageScreen(assets, stage, level, sfx, music, progress,
                () -> Gdx.app.postRunnable(() -> show(flow.afterStageEscape(stage))),
                () -> Gdx.app.postRunnable(() -> show(flow.afterStageClear(stage)))));
    }

    private void swap(Screen next) {
        Screen old = getScreen();
        setScreen(next);
        if (old != null) {
            old.dispose();
        }
    }

    @Override
    public void dispose() {
        if (getScreen() != null) {
            getScreen().dispose();
        }
        if (assets != null) {
            assets.dispose();
        }
        if (sfx != null) {
            sfx.dispose();
        }
        if (music != null) {
            music.stop();
        }
    }
}
