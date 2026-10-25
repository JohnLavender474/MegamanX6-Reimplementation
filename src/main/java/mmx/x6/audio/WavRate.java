package mmx.x6.audio;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public final class WavRate {

    private WavRate() {
    }

    public static byte[] withRate(byte[] wav, int rate) {
        byte[] out = wav.clone();
        ByteBuffer b = ByteBuffer.wrap(out).order(ByteOrder.LITTLE_ENDIAN);
        int pos = 12;
        while (pos + 8 <= out.length) {
            int size = b.getInt(pos + 4);
            if (out[pos] == 'f' && out[pos + 1] == 'm' && out[pos + 2] == 't' && out[pos + 3] == ' ') {
                int blockAlign = b.getShort(pos + 20) & 0xFFFF;
                b.putInt(pos + 12, rate);
                b.putInt(pos + 16, rate * blockAlign);
                return out;
            }
            pos += 8 + size + (size & 1);
        }
        throw new IllegalArgumentException("no fmt chunk");
    }
}
