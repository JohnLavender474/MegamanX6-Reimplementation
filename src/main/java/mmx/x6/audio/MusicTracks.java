package mmx.x6.audio;

import com.badlogic.gdx.utils.JsonReader;
import com.badlogic.gdx.utils.JsonValue;
import java.io.IOException;
import java.nio.file.Files;
import java.util.HashMap;
import java.util.Map;
import mmx.x6.DataPaths;

public final class MusicTracks {

    private MusicTracks() {
    }

    public static Map<Integer, MusicTrack> load(DataPaths p) {
        Map<Integer, MusicTrack> out = new HashMap<>();
        try {
            JsonValue root = new JsonReader().parse(Files.readString(p.songsJson()));
            for (JsonValue t = root.child; t != null; t = t.next) {
                out.put(t.getInt("track"), new MusicTrack(t.getInt("track"), t.getString("record"),
                        t.getDouble("start_seconds"), t.getDouble("end_seconds"), t.getBoolean("loop", true)));
            }
        } catch (IOException | RuntimeException e) {
            return out;
        }
        return out;
    }
}
