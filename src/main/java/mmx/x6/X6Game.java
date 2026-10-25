package mmx.x6;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Screen;
import java.io.IOException;
import java.io.UncheckedIOException;
import mmx.x6.audio.CachedSfx;
import mmx.x6.audio.GdxMusicPlayer;
import mmx.x6.audio.GdxSounds;
import mmx.x6.audio.SoundBanks;
import mmx.x6.game.Progress;
import mmx.x6.level.Level;
import mmx.x6.level.TmxLoader;
import mmx.x6.render.Assets;
import mmx.x6.screen.SelectScreen;
import mmx.x6.screen.StageScreen;
import mmx.x6.stage.StageDef;
import mmx.x6.stage.Stages;

public final class X6Game extends Game {

    private static final int SELECT_MUSIC = 1;

    private final DataPaths paths;
    private final Progress progress = new Progress();
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
        String key = System.getProperty("mmx.stage");
        StageDef stage = key == null ? null : Stages.byKey(key);
        if (stage != null) {
            startStage(stage);
        } else {
            showSelect();
        }
    }

    private void showSelect() {
        music.play(SELECT_MUSIC);
        swap(new SelectScreen(Stages.ALL, lastIndex, s -> Gdx.app.postRunnable(() -> startStage(s))));
    }

    private void startStage(StageDef stage) {
        lastIndex = Math.max(0, Stages.ALL.indexOf(stage));
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
                () -> Gdx.app.postRunnable(this::showSelect)));
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
