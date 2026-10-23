package mmx.x6;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import java.io.IOException;
import java.io.UncheckedIOException;
import mmx.x6.audio.CachedSfx;
import mmx.x6.audio.GdxMusicPlayer;
import mmx.x6.audio.SoundBanks;
import mmx.x6.level.Level;
import mmx.x6.level.TmxLoader;
import mmx.x6.render.Assets;
import mmx.x6.screen.StageScreen;

public final class X6Game extends Game {

    private final DataPaths paths;
    private Assets assets;
    private CachedSfx sfx;
    private GdxMusicPlayer music;

    public X6Game(DataPaths paths) {
        this.paths = paths;
    }

    @Override
    public void create() {
        Level level;
        try {
            level = TmxLoader.load(paths.tmx());
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        assets = new Assets(paths);
        sfx = new CachedSfx(SoundBanks.load(paths), p -> Gdx.audio.newSound(Gdx.files.absolute(p.toString())),
                System.err::println);
        music = new GdxMusicPlayer(paths);
        setScreen(new StageScreen(assets, level, sfx, music));
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
