package mmx.x6.audio;

import com.badlogic.gdx.audio.Sound;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.function.Consumer;
import java.util.function.Function;

public final class CachedSfx implements Sfx {

    private final SoundBanks banks;
    private final Function<SoundBanks.Clip, Sound> loader;
    private final Consumer<String> log;
    private final Map<SoundBanks.Clip, Sound> cache = new HashMap<>();
    private final Map<Long, Long> playing = new HashMap<>();
    private final Set<Long> reported = new HashSet<>();

    public CachedSfx(SoundBanks banks, Function<SoundBanks.Clip, Sound> loader, Consumer<String> log) {
        this.banks = banks;
        this.loader = loader;
        this.log = log;
    }

    @Override
    public void play(int bank, int id) {
        Sound s = sound(bank, id);
        if (s != null) {
            playing.put(key(bank, id), s.play());
        }
    }

    @Override
    public void useBossBank(boolean on) {
        banks.useBossBank(on);
    }

    @Override
    public void stop(int bank, int id) {
        Optional<SoundBanks.Clip> p = banks.clip(bank, id);
        Long handle = playing.remove(key(bank, id));
        if (p.isPresent() && handle != null && cache.containsKey(p.get())) {
            cache.get(p.get()).stop(handle);
        }
    }

    private Sound sound(int bank, int id) {
        Optional<SoundBanks.Clip> p = banks.clip(bank, id);
        if (p.isPresent()) {
            Sound cached = cache.get(p.get());
            if (cached != null) {
                return cached;
            }
            try {
                Sound s = loader.apply(p.get());
                cache.put(p.get(), s);
                return s;
            } catch (RuntimeException e) {
                report(bank, id);
                return null;
            }
        }
        report(bank, id);
        return null;
    }

    private void report(int bank, int id) {
        if (reported.add(key(bank, id))) {
            log.accept("sound " + bank + "," + id + " not found");
        }
    }

    private static long key(int bank, int id) {
        return ((long) bank << 32) | (id & 0xFFFFFFFFL);
    }

    public void dispose() {
        cache.values().forEach(Sound::dispose);
        cache.clear();
    }
}
