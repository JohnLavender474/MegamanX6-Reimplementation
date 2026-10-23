package mmx.x6.render;

import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.utils.Disposable;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import mmx.x6.DataPaths;
import mmx.x6.anim.AnimDef;
import mmx.x6.anim.AnimSet;
import mmx.x6.anim.D1000Anims;
import mmx.x6.anim.SabreBlockAnims;
import mmx.x6.anim.ShotAnims;

public final class Assets implements Disposable {

    public record SpriteFrame(TextureRegion region, int originX, int originY) {
    }

    private record Source(Path dir, AnimSet set, Map<Integer, String> files) {
    }

    private final DataPaths paths;
    private final Map<String, AnimSet> sets = new HashMap<>();
    private final Map<String, Map<Integer, Source>> sources = new HashMap<>();
    private final Map<String, Texture> textures = new HashMap<>();
    private final Map<String, SpriteFrame[]> strips = new HashMap<>();
    private final List<SpriteFrame> sabreBlock = new ArrayList<>();
    private Texture tiles;
    private TextureRegion[] tileRegions;
    private Texture white;

    public Assets(DataPaths paths) {
        this.paths = paths;
    }

    public AnimSet anim(String set) {
        AnimSet s = sets.get(set);
        if (s == null) {
            s = load(set);
            sets.put(set, s);
        }
        return s;
    }

    public SpriteFrame frame(String set, int anim, int frameIndex) {
        if ("sabre-block".equals(set)) {
            return sabreBlockSprite(anim);
        }
        anim(set);
        Source src = sources.get(set).get(anim);
        if (src == null) {
            return null;
        }
        String key = set + "#" + anim;
        SpriteFrame[] frames = strips.get(key);
        if (frames == null) {
            frames = cutStrip(src, anim);
            strips.put(key, frames);
        }
        if (frames.length == 0) {
            return null;
        }
        return frames[Math.max(0, Math.min(frameIndex, frames.length - 1))];
    }

    public SpriteFrame sabreBlockSprite(int sprite) {
        if (sabreBlock.isEmpty()) {
            cutSabreBlock();
        }
        return sprite < sabreBlock.size() ? sabreBlock.get(sprite) : null;
    }

    public TextureRegion tile(int gid) {
        if (tiles == null) {
            tiles = texture(paths.tilesPng());
            int cols = tiles.getWidth() / 16;
            int rows = tiles.getHeight() / 16;
            tileRegions = new TextureRegion[cols * rows];
            for (int i = 0; i < tileRegions.length; i++) {
                TextureRegion r = new TextureRegion(tiles, (i % cols) * 16, (i / cols) * 16, 16, 16);
                r.flip(false, true);
                tileRegions[i] = r;
            }
        }
        int i = gid - 1;
        return i >= 0 && i < tileRegions.length ? tileRegions[i] : null;
    }

    public Texture white() {
        if (white == null) {
            Pixmap p = new Pixmap(1, 1, Pixmap.Format.RGBA8888);
            p.setColor(1, 1, 1, 1);
            p.fill();
            white = new Texture(p);
            p.dispose();
        }
        return white;
    }

    private AnimSet load(String set) {
        return switch (set) {
            case "x-falcon" -> loadDir(set, paths.playerDir("x-falcon"));
            case "d1000" -> loadD1000();
            case "shots" -> loadShots();
            case "sabre-block" -> SabreBlockAnims.SET;
            default -> loadDir(set, paths.enemyDir(set));
        };
    }

    private AnimSet loadDir(String set, Path dir) {
        AnimSet s = parse(dir.resolve("animations.json"));
        Map<Integer, Source> bySet = new HashMap<>();
        Source src = new Source(dir, s, Map.of());
        for (int i : s.indices()) {
            bySet.put(i, src);
        }
        sources.put(set, bySet);
        return s;
    }

    private AnimSet loadD1000() {
        AnimSet body = parse(paths.enemyDir("d-1000").resolve("animations.json"));
        AnimSet parts = parse(paths.enemyDir("d-1000-parts").resolve("animations.json"));
        AnimSet merged = D1000Anims.merge(body, parts);
        Map<Integer, Source> bySet = new HashMap<>();
        Source bodySrc = new Source(paths.enemyDir("d-1000"), body, Map.of());
        Source partsSrc = new Source(paths.enemyDir("d-1000-parts"), parts, Map.of());
        for (int i : merged.indices()) {
            bySet.put(i, i <= D1000Anims.LAST_BODY_ANIM ? bodySrc : partsSrc);
        }
        sources.put("d1000", bySet);
        return merged;
    }

    private AnimSet loadShots() {
        AnimSet s = ShotAnims.SET;
        Map<Integer, Source> bySet = new HashMap<>();
        Source src = new Source(paths.playerDir("x-shots"), s, ShotAnims.FILES);
        for (int i : s.indices()) {
            bySet.put(i, src);
        }
        sources.put("shots", bySet);
        return s;
    }

    private SpriteFrame[] cutStrip(Source src, int anim) {
        String file = src.files().getOrDefault(anim, "anim-" + anim + ".png");
        Path p = src.dir().resolve(file);
        if (!Files.exists(p)) {
            return new SpriteFrame[0];
        }
        Texture t = texture(p);
        AnimDef d = src.set().get(anim);
        int count = d == null ? 1 : d.frames().size();
        boolean handMade = !src.files().isEmpty();
        int cellW = handMade ? t.getWidth() / count : src.set().cellW();
        int cellH = handMade ? t.getHeight() : src.set().cellH();
        int ox = handMade ? cellW / 2 : src.set().originX();
        int oy = handMade ? cellH / 2 : src.set().originY();
        int cells = Math.max(1, t.getWidth() / cellW);
        SpriteFrame[] out = new SpriteFrame[Math.min(count, cells)];
        for (int i = 0; i < out.length; i++) {
            TextureRegion r = new TextureRegion(t, i * cellW, 0, cellW, cellH);
            r.flip(false, true);
            out[i] = new SpriteFrame(r, ox, oy);
        }
        return out;
    }

    private void cutSabreBlock() {
        Path p = paths.bankSheet("intro-1_start", 1);
        if (!Files.exists(p)) {
            return;
        }
        Pixmap pix = new Pixmap(new FileHandle(p.toFile()));
        int w = pix.getWidth();
        int h = pix.getHeight();
        int background = pix.getPixel(0, 0);
        pix.setBlending(Pixmap.Blending.None);
        for (int py = 0; py < h; py++) {
            for (int px = 0; px < w; px++) {
                if (pix.getPixel(px, py) == background) {
                    pix.drawPixel(px, py, 0);
                }
            }
        }
        Texture t = new Texture(pix);
        t.setFilter(Texture.TextureFilter.Nearest, Texture.TextureFilter.Nearest);
        textures.put(p + "#transparent", t);
        int x = 0;
        while (x < w && sabreBlock.size() < 4) {
            while (x < w && !opaqueColumn(pix, x, h)) {
                x++;
            }
            int start = x;
            while (x < w && opaqueColumn(pix, x, h)) {
                x++;
            }
            if (x > start) {
                int top = h;
                int bottom = 0;
                for (int cx = start; cx < x; cx++) {
                    for (int cy = 10; cy < h; cy++) {
                        if ((pix.getPixel(cx, cy) & 0xFF) > 0) {
                            top = Math.min(top, cy);
                            bottom = Math.max(bottom, cy);
                        }
                    }
                }
                int rw = x - start;
                int rh = bottom - top + 1;
                TextureRegion r = new TextureRegion(t, start, top, rw, rh);
                r.flip(false, true);
                sabreBlock.add(new SpriteFrame(r, rw / 2, rh / 2));
            }
        }
        pix.dispose();
    }

    private static boolean opaqueColumn(Pixmap pix, int x, int h) {
        for (int y = 10; y < h; y++) {
            if ((pix.getPixel(x, y) & 0xFF) > 0) {
                return true;
            }
        }
        return false;
    }

    private Texture texture(Path p) {
        return textures.computeIfAbsent(p.toString(), k -> {
            Texture t = new Texture(new FileHandle(p.toFile()));
            t.setFilter(Texture.TextureFilter.Nearest, Texture.TextureFilter.Nearest);
            return t;
        });
    }

    private static AnimSet parse(Path json) {
        try {
            return AnimSet.parse(Files.readString(json));
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    @Override
    public void dispose() {
        textures.values().forEach(Texture::dispose);
        textures.clear();
        if (white != null) {
            white.dispose();
        }
    }
}
