package mmx.x6;

import com.badlogic.gdx.Game;
import java.io.IOException;
import java.io.UncheckedIOException;
import mmx.x6.level.Level;
import mmx.x6.level.TmxLoader;
import mmx.x6.render.Assets;
import mmx.x6.screen.StageScreen;

public final class X6Game extends Game {

    private final DataPaths paths;
    private Assets assets;

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
        setScreen(new StageScreen(assets, level));
    }

    @Override
    public void dispose() {
        if (getScreen() != null) {
            getScreen().dispose();
        }
        if (assets != null) {
            assets.dispose();
        }
    }
}
