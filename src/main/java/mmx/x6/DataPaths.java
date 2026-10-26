package mmx.x6;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

public final class DataPaths {

    private static final Path LOCAL_TMX = Path.of("assets", "tmx");

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
        return tmx("m094-intro", "intro.tmx");
    }

    public Path tilesPng() {
        return tilesPng("m094-intro");
    }

    public Path tmx(String dir, String file) {
        return root.resolve("tmx").resolve(dir).resolve(file);
    }

    public Path tilesPng(String dir) {
        return root.resolve("tmx").resolve(dir).resolve("tiles.png");
    }

    public Path stageTmx(String dir, String file) {
        Path local = LOCAL_TMX.resolve(dir).resolve(file);
        return Files.isRegularFile(local) ? local : tmx(dir, file);
    }

    public Path stageTiles(String dir) {
        Path local = LOCAL_TMX.resolve(dir).resolve("tiles.png");
        return Files.isRegularFile(local) ? local : tilesPng(dir);
    }

    public Path playerDir(String name) {
        return root.resolve("sprites/players").resolve(name);
    }

    public Path enemyDir(String name) {
        return root.resolve("sprites/enemies").resolve(name);
    }

    public Path bankSheet(String state, int bank) {
        return bankSheet("intro", state, bank);
    }

    public Path bankSheet(String stage, String state, int bank) {
        return root.resolve("sprites/banks").resolve(stage).resolve(state).resolve("bank-" + bank + ".png");
    }

    public Path soundBank(String folder) {
        return root.resolve("sounds").resolve(folder);
    }

    public Path movieDir(String name) {
        return root.resolve("movies").resolve(name);
    }

    public Path movieFrame(String name, int i) {
        return movieDir(name).resolve(String.format("frame-%04d.png", i));
    }

    public Path movieAudio(String name) {
        return movieDir(name).resolve("audio.wav");
    }

    public Path frontEndImage(String file) {
        return root.resolve("re/front-end/images").resolve(file);
    }

    public boolean frontEndExists() {
        for (Path p : List.of(movieFrame("caplogo", 0), movieFrame("x6op", 0), movieAudio("caplogo"),
                movieAudio("x6op"), frontEndImage("stage14-area0-layer0.png"), frontEndImage("misc19-sprites.png"),
                frontEndImage("vram-512-256-clut7800.png"))) {
            if (!Files.isRegularFile(p)) {
                return false;
            }
        }
        return true;
    }

    public Path songsJson() {
        return root.resolve("music/bgm/songs.json");
    }

    public Path musicRecord(String file) {
        return root.resolve("music/bgm").resolve(file);
    }
}
