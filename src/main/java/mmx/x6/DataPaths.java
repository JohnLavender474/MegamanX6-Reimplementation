package mmx.x6;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

public final class DataPaths {

    private static final Path ASSETS = Path.of("assets");
    private static final String INTRO_TMX = "intro.tmx";

    private final Path root;

    public DataPaths(Path root) {
        this.root = root;
    }

    public static DataPaths assets() {
        return new DataPaths(ASSETS);
    }

    public static boolean assetsExist() {
        return Files.isRegularFile(assets().tmx());
    }

    public void validate() throws DataRootException {
        for (Path p : List.of(tmx(), tilesPng(), playerDir("x-falcon"), songsJson())) {
            if (!Files.exists(p)) {
                throw new DataRootException("assets folder " + root + " is missing " + p
                        + "; copy the assets with `make assets`");
            }
        }
    }

    public Path root() {
        return root;
    }

    public Path tmx() {
        return tmx(INTRO_TMX);
    }

    public Path tilesPng() {
        return tilesPng(INTRO_TMX);
    }

    public Path tmx(String file) {
        return root.resolve("tiled/tmx").resolve(file);
    }

    public Path tilesPng(String tmxFile) {
        return root.resolve("tiled/png").resolve(tmxFile.replaceFirst("\\.tmx$", "") + "-tiles.png");
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
        return root.resolve("sprites/frontend").resolve(file);
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
        return root.resolve("music/songs.json");
    }

    public Path musicRecord(String file) {
        return root.resolve("music").resolve(file);
    }
}
