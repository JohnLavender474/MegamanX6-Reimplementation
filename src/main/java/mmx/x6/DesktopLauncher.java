package mmx.x6;

import com.badlogic.gdx.backends.lwjgl3.Lwjgl3Application;
import com.badlogic.gdx.backends.lwjgl3.Lwjgl3ApplicationConfiguration;

public final class DesktopLauncher {

    private DesktopLauncher() {
    }

    public static void main(String[] args) {
        DataPaths paths = DataPaths.resolve(args);
        try {
            paths.validate();
        } catch (DataRootException e) {
            System.err.println("mmx-x6: " + e.getMessage());
            System.exit(1);
            return;
        }
        Lwjgl3ApplicationConfiguration config = new Lwjgl3ApplicationConfiguration();
        config.setTitle(AutoRun.title("Mega Man X6 (vibe-coded)"));
        config.setWindowedMode(960, 720);
        config.setResizable(false);
        config.useVsync(true);
        config.setForegroundFPS(60);
        new Lwjgl3Application(new X6Game(paths), config);
    }
}
