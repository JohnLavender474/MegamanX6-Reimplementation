package mmx.x6.anim;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.imageio.ImageIO;
import mmx.x6.physics.Rect;

public final class SpriteBounds {

    private SpriteBounds() {
    }

    public static AnimSet attach(AnimSet set, Path dir) throws IOException {
        Map<Integer, List<Rect>> out = new HashMap<>();
        for (int index : set.indices()) {
            Path png = dir.resolve("anim-" + index + ".png");
            if (!Files.exists(png)) {
                continue;
            }
            BufferedImage strip = ImageIO.read(png.toFile());
            List<Rect> frames = new ArrayList<>();
            for (int f = 0; f < set.get(index).frames().size(); f++) {
                frames.add(cellBounds(strip, f * set.cellW(), set));
            }
            out.put(index, frames);
        }
        return set.withBounds(out);
    }

    private static Rect cellBounds(BufferedImage strip, int left, AnimSet set) {
        int minX = Integer.MAX_VALUE;
        int minY = Integer.MAX_VALUE;
        int maxX = -1;
        int maxY = -1;
        int right = Math.min(strip.getWidth(), left + set.cellW());
        int bottom = Math.min(strip.getHeight(), set.cellH());
        for (int y = 0; y < bottom; y++) {
            for (int x = left; x < right; x++) {
                if ((strip.getRGB(x, y) >>> 24) != 0) {
                    minX = Math.min(minX, x - left);
                    maxX = Math.max(maxX, x - left);
                    minY = Math.min(minY, y);
                    maxY = Math.max(maxY, y);
                }
            }
        }
        if (maxX < 0) {
            return null;
        }
        return new Rect(minX - set.originX(), minY - set.originY(), maxX - minX + 1, maxY - minY + 1);
    }
}
