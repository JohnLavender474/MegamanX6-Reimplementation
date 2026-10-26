package mmx.x6.screen;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.PixmapIO;

public final class Shots {

    private Shots() {
    }

    public static int[] times(String list) {
        String[] parts = list.split(",");
        int[] out = new int[parts.length];
        for (int i = 0; i < parts.length; i++) {
            out[i] = Integer.parseInt(parts[i].trim());
        }
        return out;
    }

    public static String path(String base, int[] times, int i) {
        return times.length == 1 ? base : base.replace(".png", "-" + times[i] + ".png");
    }

    static void saveWindow(ShotClock shots, int updates) {
        String shot = shots.due(updates);
        if (shot == null) {
            return;
        }
        write(shot, Pixmap.createFromFrameBuffer(0, 0, Gdx.graphics.getBackBufferWidth(),
                Gdx.graphics.getBackBufferHeight()));
        if (shots.finished()) {
            Gdx.app.exit();
        }
    }

    static void write(String path, Pixmap p) {
        PixmapIO.writePNG(new FileHandle(path), p, 0, true);
        p.dispose();
    }
}
