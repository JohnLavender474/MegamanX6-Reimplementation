package mmx.x6.audio;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.audio.Music;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Map;
import mmx.x6.DataPaths;

public final class GdxMusicPlayer implements MusicPlayer {

    private final DataPaths paths;
    private final Map<Integer, MusicTrack> tracks;
    private Music music;
    private MusicTrack current;

    public GdxMusicPlayer(DataPaths paths) {
        this.paths = paths;
        this.tracks = MusicTracks.load(paths);
    }

    @Override
    public void play(int track) {
        if (current != null && current.track() == track && music != null && music.isPlaying()) {
            return;
        }
        stop();
        MusicTrack t = tracks.get(track);
        if (t == null) {
            System.err.println("music track " + track + " not found");
            return;
        }
        Path file = paths.musicRecord(t.record());
        if (!Files.isRegularFile(file)) {
            System.err.println("music file " + file + " not found");
            return;
        }
        music = Gdx.audio.newMusic(Gdx.files.absolute(file.toString()));
        music.setVolume(0.6f);
        music.play();
        if (t.start() > 0) {
            music.setPosition((float) t.start());
        }
        current = t;
    }

    @Override
    public void stop() {
        if (music != null) {
            music.stop();
            music.dispose();
            music = null;
        }
        current = null;
    }

    @Override
    public boolean playing() {
        return music != null && music.isPlaying();
    }

    @Override
    public int track() {
        return current == null ? -1 : current.track();
    }

    @Override
    public void update() {
        if (music == null || current == null) {
            return;
        }
        if (music.getPosition() >= current.end() - 0.02 || !music.isPlaying()) {
            if (current.loop()) {
                if (!music.isPlaying()) {
                    music.play();
                }
                music.setPosition((float) current.start());
            } else {
                stop();
            }
        }
    }
}
