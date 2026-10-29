package mmx.x6;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Screen;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.file.Path;
import mmx.x6.audio.CachedSfx;
import mmx.x6.audio.GdxMusicPlayer;
import mmx.x6.audio.GdxSounds;
import mmx.x6.audio.Sfx;
import mmx.x6.audio.SoundBanks;
import mmx.x6.game.Progress;
import mmx.x6.input.Bindings;
import mmx.x6.input.Controls;
import mmx.x6.input.GdxPads;
import mmx.x6.render.Assets;
import mmx.x6.screen.FrontEndScreen;
import mmx.x6.screen.LevelErrorScreen;
import mmx.x6.screen.SelectScreen;
import mmx.x6.screen.StageScreen;
import mmx.x6.stage.LoadedStage;
import mmx.x6.stage.StageDef;
import mmx.x6.stage.StageLoader;
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
    private Controls controls;

    public X6Game(DataPaths paths) {
        this.paths = paths;
    }

    @Override
    public void create() {
        assets = new Assets(paths);
        controls = new Controls(new Bindings(), new GdxPads());
        music = new GdxMusicPlayer(paths);
        flow = new GameFlow(System.getProperty("mmx.stage"), System.getProperty("mmx.front"),
                System.getProperty("mmx.level"));
        if (flow.levelPath() != null) {
            startLevelFile(flow.levelPath());
            return;
        }
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
        swap(new FrontEndScreen(paths, systemSfx(), controls, flow.frontStart(),
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
        swap(new SelectScreen(controls, Stages.BOSSES, lastIndex, s -> Gdx.app.postRunnable(() -> startStage(s))));
    }

    private void startStage(StageDef def) {
        lastIndex = Math.max(0, Stages.BOSSES.indexOf(def));
        LoadedStage loaded;
        try {
            loaded = StageLoader.builtIn(paths, def);
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        play(loaded, def, () -> show(flow.afterStageEscape(def)), () -> show(flow.afterStageClear(def)));
    }

    private void startLevelFile(String file) {
        LoadedStage loaded;
        try {
            loaded = StageLoader.file(paths, Path.of(file));
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        if (loaded.stage() == null) {
            swap(new LevelErrorScreen(loaded.report().findings(), () -> Gdx.app.exit()));
            return;
        }
        progress = new Progress();
        play(loaded, loaded.stage(), () -> Gdx.app.exit(), () -> Gdx.app.exit());
    }

    private void play(LoadedStage loaded, StageDef def, Runnable exit, Runnable cleared) {
        StageDef stage = loaded.stage();
        Path tiles = loaded.tilesPng() != null ? paths.tilesPngNamed(loaded.tilesPng())
                : paths.tilesPng(stage.tmxFile());
        assets.useTiles(tiles);
        if (sfx != null) {
            sfx.dispose();
        }
        sfx = new CachedSfx(SoundBanks.load(paths, stage.bank2(), stage.bank5(), stage.bossBank2()),
                GdxSounds::load, System.err::println);
        swap(new StageScreen(assets, controls, stage, loaded.level(), sfx, music, progress,
                () -> Gdx.app.postRunnable(exit), () -> Gdx.app.postRunnable(cleared)));
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
