package mmx.x6;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

public final class DataPaths {

    private final Path root;

    public DataPaths(Path root) {
        this.root = root;
    }

    public static DataPaths resolve(String[] args) {
        String property = System.getProperty("mmx.data");
        if (property != null && !property.isBlank()) {
            return new DataPaths(Path.of(property));
        }
        if (args.length > 0 && !args[0].isBlank()) {
            return new DataPaths(Path.of(args[0]));
        }
        return new DataPaths(defaultRoot());
    }

    public static boolean defaultRootExists() {
        return Files.isRegularFile(new DataPaths(defaultRoot()).tmx());
    }

    private static Path defaultRoot() {
        return Path.of(System.getProperty("user.home"), "mmx-data", "x6");
    }

    public void validate() throws DataRootException {
        for (Path p : List.of(tmx(), tilesPng(), playerDir("x-falcon"), songsJson())) {
            if (!Files.exists(p)) {
                throw new DataRootException("data root " + root + " is missing " + p);
            }
        }
    }

    public Path root() {
        return root;
    }

    public Path tmx() {
        return root.resolve("tmx/m094-intro/intro.tmx");
    }

    public Path tilesPng() {
        return root.resolve("tmx/m094-intro/tiles.png");
    }

    public Path playerDir(String name) {
        return root.resolve("sprites/players").resolve(name);
    }

    public Path enemyDir(String name) {
        return root.resolve("sprites/enemies").resolve(name);
    }

    public Path bankSheet(String state, int bank) {
        return root.resolve("sprites/banks/intro").resolve(state).resolve("bank-" + bank + ".png");
    }

    public Path soundBank(String folder) {
        return root.resolve("sounds").resolve(folder);
    }

    public Path songsJson() {
        return root.resolve("music/bgm/songs.json");
    }

    public Path musicRecord(String file) {
        return root.resolve("music/bgm").resolve(file);
    }
}
