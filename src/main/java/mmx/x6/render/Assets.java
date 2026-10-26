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
import mmx.x6.anim.BankSheetAnims;
import mmx.x6.anim.D1000Anims;
import mmx.x6.anim.SabreBlockAnims;
import mmx.x6.anim.ShotAnims;
import mmx.x6.anim.SpriteBounds;

public final class Assets implements Disposable {

    public record SpriteFrame(TextureRegion region, int originX, int originY) {
    }

    private record Source(Path dir, AnimSet set, Map<Integer, String> files) {
    }

    private static final int AIR_DASH_START = 169;
    private static final int AIR_DASH = 171;

    private final DataPaths paths;
    private final Map<String, AnimSet> sets = new HashMap<>();
    private final Map<String, Map<Integer, Source>> sources = new HashMap<>();
    private final Map<String, Texture> textures = new HashMap<>();
    private final Map<String, SpriteFrame[]> strips = new HashMap<>();
    private final List<SpriteFrame> sabreBlock = new ArrayList<>();
    private final Map<String, SpriteFrame> bankSprites = new HashMap<>();
    private Path tilesPath;
    private Texture tiles;
    private TextureRegion[] tileRegions;
    private Texture white;

    public Assets(DataPaths paths) {
        this.paths = paths;
        this.tilesPath = paths.tilesPng();
    }

    public void useTiles(Path p) {
        if (p.equals(tilesPath)) {
            return;
        }
        if (tiles != null) {
            textures.remove(tilesPath.toString());
            tiles.dispose();
        }
        tilesPath = p;
        tiles = null;
        tileRegions = null;
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
        BankSheetAnims.Sheet sheet = BankSheetAnims.SHEETS.get(set);
        if (sheet != null) {
            AnimDef d = sheet.set().get(anim);
            if (d == null) {
                return null;
            }
            int sprite = d.frames().get(Math.max(0, Math.min(frameIndex, d.frames().size() - 1))).sprite();
            return bankSprite(sheet, sprite);
        }
        anim(set);
        int[] drawn = drawnFrame(set, anim, frameIndex);
        anim = drawn[0];
        frameIndex = drawn[1];
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

    static int[] drawnFrame(String set, int anim, int frameIndex) {
        if ("x-falcon".equals(set) && anim == AIR_DASH_START) {
            // TODO: the export of animation 169 (frame set 1) shows other sprites; the first air dash frame (171) is drawn instead (docs/gaps.md, "Air dash start")
            return new int[] {AIR_DASH, 0};
        }
        return new int[] {anim, frameIndex};
    }

    public SpriteFrame sabreBlockSprite(int sprite) {
        if (sabreBlock.isEmpty()) {
            cutSabreBlock();
        }
        return sprite < sabreBlock.size() ? sabreBlock.get(sprite) : null;
    }

    public TextureRegion tile(int gid) {
        if (tiles == null) {
            tiles = texture(tilesPath);
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
        BankSheetAnims.Sheet sheet = BankSheetAnims.SHEETS.get(set);
        if (sheet != null) {
            return sheet.set();
        }
        return switch (set) {
            case "x-falcon" -> loadDir(set, paths.playerDir("x-falcon"));
            case "d1000" -> loadD1000();
            case "shots" -> loadShots();
            case "sabre-block" -> SabreBlockAnims.SET;
            case "x-sabre" -> withBounds(loadDir(set, paths.enemyDir(set)), paths.enemyDir(set));
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

    private static AnimSet withBounds(AnimSet set, Path dir) {
        try {
            return SpriteBounds.attach(set, dir);
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
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
            return cutSheetCell(src, anim);
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

    private SpriteFrame[] cutSheetCell(Source src, int sprite) {
        Path p = src.dir().resolve("sprites.png");
        if (!Files.exists(p)) {
            return new SpriteFrame[0];
        }
        Texture t = texture(p);
        int cellW = src.set().cellW();
        int cellH = src.set().cellH();
        int cols = Math.max(1, t.getWidth() / cellW);
        TextureRegion r = new TextureRegion(t, (sprite % cols) * cellW, (sprite / cols) * cellH, cellW, cellH);
        r.flip(false, true);
        return new SpriteFrame[] {new SpriteFrame(r, src.set().originX(), src.set().originY())};
    }

    private SpriteFrame bankSprite(BankSheetAnims.Sheet sheet, int sprite) {
        String key = sheet.stage() + "/" + sheet.state() + "/" + sheet.bank() + "#" + sprite;
        if (bankSprites.containsKey(key)) {
            return bankSprites.get(key);
        }
        SpriteFrame f = cutBankSprite(sheet, sprite);
        bankSprites.put(key, f);
        return f;
    }

    private SpriteFrame cutBankSprite(BankSheetAnims.Sheet sheet, int sprite) {
        Path p = paths.bankSheet(sheet.stage(), sheet.state(), sheet.bank());
        if (!Files.exists(p)) {
            return null;
        }
        Texture t = textures.get(p + "#transparent");
        Pixmap pix = new Pixmap(new FileHandle(p.toFile()));
        int background = pix.getPixel(pix.getWidth() - 1, pix.getHeight() - 1);
        int cx = (sprite % BankSheetAnims.COLUMNS) * BankSheetAnims.CELL_W;
        int cy = (sprite / BankSheetAnims.COLUMNS) * BankSheetAnims.CELL_H;
        int x0 = Integer.MAX_VALUE;
        int y0 = Integer.MAX_VALUE;
        int x1 = -1;
        int y1 = -1;
        for (int y = cy + BankSheetAnims.LABEL_H; y < Math.min(cy + BankSheetAnims.CELL_H, pix.getHeight()); y++) {
            for (int x = cx; x < Math.min(cx + BankSheetAnims.CELL_W, pix.getWidth()); x++) {
                if (pix.getPixel(x, y) != background) {
                    x0 = Math.min(x0, x);
                    y0 = Math.min(y0, y);
                    x1 = Math.max(x1, x);
                    y1 = Math.max(y1, y);
                }
            }
        }
        if (t == null) {
            pix.setBlending(Pixmap.Blending.None);
            for (int y = 0; y < pix.getHeight(); y++) {
                for (int x = 0; x < pix.getWidth(); x++) {
                    if (pix.getPixel(x, y) == background) {
                        pix.drawPixel(x, y, 0);
                    }
                }
            }
            t = new Texture(pix);
            t.setFilter(Texture.TextureFilter.Nearest, Texture.TextureFilter.Nearest);
            textures.put(p + "#transparent", t);
        }
        pix.dispose();
        if (x1 < 0) {
            return null;
        }
        int w = x1 - x0 + 1;
        int h = y1 - y0 + 1;
        TextureRegion r = new TextureRegion(t, x0, y0, w, h);
        r.flip(false, true);
        Integer bottom = sheet.bottomFromOrigin().get(sprite);
        return new SpriteFrame(r, w / 2, bottom == null ? h / 2 : h - bottom);
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
