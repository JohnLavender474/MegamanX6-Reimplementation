package mmx.x6.audio;

import com.badlogic.gdx.utils.JsonReader;
import com.badlogic.gdx.utils.JsonValue;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import mmx.x6.DataPaths;

public final class SoundBanks {

    public record Clip(Path wav, int rate) {
    }

    private static final int SPU_RATE = 44100;

    private final Map<Integer, String> folders = new HashMap<>(Map.of(0, "m087-e04", 1, "m087-e11", 3, "m087-e13"));
    private final Map<String, Map<Integer, Clip>> banks = new HashMap<>();
    private String bossBank2;
    private boolean boss;

    private SoundBanks() {
    }

    public static SoundBanks load(DataPaths p) {
        return load(p, "m094-e05", "m094-e03", "m123-e01");
    }

    public static SoundBanks load(DataPaths p, String bank2, String bank5, String bossBank2) {
        SoundBanks b = new SoundBanks();
        b.folders.put(2, bank2);
        if (bank5 != null) {
            b.folders.put(5, bank5);
        }
        b.bossBank2 = bossBank2;
        for (String folder : b.folders.values()) {
            b.banks.put(folder, readBank(p, folder));
        }
        if (bossBank2 != null) {
            b.banks.put(bossBank2, readBank(p, bossBank2));
        }
        return b;
    }

    private static Map<Integer, Clip> readBank(DataPaths p, String folder) {
        Map<Integer, Clip> out = new HashMap<>();
        Path json = p.soundBank(folder).resolve("bank.json");
        if (!Files.isRegularFile(json)) {
            return out;
        }
        try {
            JsonValue root = new JsonReader().parse(Files.readString(json));
            String wavFolder = root.has("same_as") && !root.get("same_as").isNull()
                    ? root.getString("same_as") : folder;
            JsonValue ids = root.get("ids");
            if (ids == null) {
                return out;
            }
            for (JsonValue e = ids.child; e != null; e = e.next) {
                JsonValue file = e.get("file");
                if (file != null && !file.isNull()) {
                    Path wav = p.soundBank(wavFolder).resolve(file.asString());
                    if (Files.isRegularFile(wav)) {
                        out.put(e.getInt("id"), new Clip(wav, rate(root, e)));
                    }
                }
            }
        } catch (IOException | RuntimeException e) {
            return out;
        }
        return out;
    }

    private static int rate(JsonValue root, JsonValue id) {
        JsonValue programs = root.get("programs");
        if (programs == null) {
            return SPU_RATE;
        }
        for (JsonValue prog = programs.child; prog != null; prog = prog.next) {
            if (prog.getInt("number") != id.getInt("program", -1)) {
                continue;
            }
            JsonValue tone = prog.get("tones").get(id.getInt("tone", -1));
            if (tone == null) {
                return SPU_RATE;
            }
            double semitones = tone.getInt("note_min") - tone.getInt("center") + tone.getInt("fine") / 128.0;
            return (int) Math.round(SPU_RATE * Math.pow(2, semitones / 12));
        }
        return SPU_RATE;
    }

    public void useBossBank(boolean on) {
        boss = on;
    }

    public Optional<Clip> clip(int bank, int id) {
        String folder = bank == 2 && boss && bossBank2 != null ? bossBank2 : folders.get(bank);
        if (folder == null) {
            return Optional.empty();
        }
        return Optional.ofNullable(banks.getOrDefault(folder, Map.of()).get(id));
    }
}
