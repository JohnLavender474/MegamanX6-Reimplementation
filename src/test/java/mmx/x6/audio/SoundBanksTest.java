package mmx.x6.audio;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import mmx.x6.DataPaths;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

class SoundBanksTest {

    private static SoundBanks real() {
        assumeTrue(DataPaths.defaultRootExists());
        return SoundBanks.load(DataPaths.resolve(new String[0]));
    }

    @Test
    void playerJumpSound() {
        Path p = real().wav(1, 1).orElseThrow();
        assertTrue(Files.isRegularFile(p));
    }

    @Test
    void landSound() {
        assertTrue(real().wav(1, 2).isPresent());
    }

    @Test
    void sameAsBankResolvesToTheOtherFolder() {
        SoundBanks b = real();
        b.wav(0, 16).ifPresent(p -> {
            assertTrue(p.toString().contains("m085-e04"));
            assertTrue(Files.isRegularFile(p));
        });
    }

    @Test
    void unknownIdIsEmpty() {
        assertTrue(real().wav(1, 999).isEmpty());
    }

    @Test
    void unknownBankIsEmpty() {
        assertTrue(real().wav(4, 0).isEmpty());
    }

    @Test
    void bossBankSwitch() {
        SoundBanks banks = real();
        banks.useBossBank(true);
        banks.wav(2, 5).ifPresent(p -> assertTrue(p.toString().contains("m123-e01")));
        banks.useBossBank(false);
        banks.wav(2, 26).ifPresent(p -> assertTrue(p.toString().contains("m094-e05")));
    }

    @Test
    void missingSoundLoggedOnce(@TempDir Path tmp) {
        List<String> log = new ArrayList<>();
        Sfx s = new CachedSfx(SoundBanks.load(new DataPaths(tmp)), p -> {
            throw new AssertionError("no sound should load");
        }, log::add);
        s.play(1, 5);
        s.play(1, 5);
        s.play(2, 9);
        assertEquals(List.of("sound 1,5 not found", "sound 2,9 not found"), log);
    }
}
