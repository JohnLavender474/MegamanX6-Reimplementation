package mmx.x6;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.utils.ScreenUtils;

public final class X6Game extends Game {

    private final DataPaths paths;

    public X6Game(DataPaths paths) {
        this.paths = paths;
    }

    public DataPaths paths() {
        return paths;
    }

    @Override
    public void create() {
    }

    @Override
    public void render() {
        ScreenUtils.clear(0, 0, 0, 1);
        super.render();
    }
}
