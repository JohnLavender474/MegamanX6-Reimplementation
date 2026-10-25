package mmx.x6.audio;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class WavRateTest {

    private static byte[] wav(int rate, short... samples) {
        ByteBuffer b = ByteBuffer.allocate(44 + samples.length * 2).order(ByteOrder.LITTLE_ENDIAN);
        b.put("RIFF".getBytes()).putInt(36 + samples.length * 2).put("WAVE".getBytes());
        b.put("fmt ".getBytes()).putInt(16).putShort((short) 1).putShort((short) 1)
                .putInt(rate).putInt(rate * 2).putShort((short) 2).putShort((short) 16);
        b.put("data".getBytes()).putInt(samples.length * 2);
        for (short s : samples) {
            b.putShort(s);
        }
        return b.array();
    }

    @Test
    void rewritesTheRateAndTheByteRate() {
        byte[] out = WavRate.withRate(wav(44100, (short) 1, (short) -2), 16000);
        ByteBuffer b = ByteBuffer.wrap(out).order(ByteOrder.LITTLE_ENDIAN);
        assertEquals(16000, b.getInt(24));
        assertEquals(32000, b.getInt(28));
        assertEquals(1, b.getShort(44));
        assertEquals(-2, b.getShort(46));
    }

    @Test
    void sameRateIsUnchanged() {
        byte[] in = wav(44100, (short) 5);
        assertArrayEquals(in, WavRate.withRate(in, 44100));
    }
}
