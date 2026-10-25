package mmx.x6.audio;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.audio.Sound;
import com.badlogic.gdx.files.FileHandle;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UncheckedIOException;
import java.nio.file.Files;

public final class GdxSounds {

    private GdxSounds() {
    }

    public static Sound load(SoundBanks.Clip clip) {
        byte[] bytes;
        try {
            bytes = WavRate.withRate(Files.readAllBytes(clip.wav()), clip.rate());
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        FileHandle inMemory = new FileHandle(clip.wav().toFile()) {
            @Override
            public InputStream read() {
                return new ByteArrayInputStream(bytes);
            }

            @Override
            public long length() {
                return bytes.length;
            }
        };
        return Gdx.audio.newSound(inMemory);
    }
}
