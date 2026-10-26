package mmx.x6.frontend;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.function.IntFunction;

public final class MovieFrames {

    private final IntFunction<Path> frames;
    private Texture texture;
    private int index = -1;

    public MovieFrames(IntFunction<Path> frames) {
        this.frames = frames;
    }

    public Texture frame(int i) {
        if (i != index) {
            Path p = frames.apply(i);
            if (Files.isRegularFile(p)) {
                dispose();
                texture = new Texture(Gdx.files.absolute(p.toString()));
                texture.setFilter(Texture.TextureFilter.Nearest, Texture.TextureFilter.Nearest);
            }
            index = i;
        }
        return texture;
    }

    public void dispose() {
        if (texture != null) {
            texture.dispose();
            texture = null;
        }
    }
}
