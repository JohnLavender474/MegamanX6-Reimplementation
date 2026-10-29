package mmx.x6.level;

import com.badlogic.gdx.utils.XmlReader;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Function;

public final class LevelValidator {

    private static final List<String> TILE_LAYERS = List.of("art 2 background", "art 1 decoration",
            "art 0 main", "art 0 front", "collision");
    private static final long FLIP_BITS = 0xE0000000L;

    private LevelValidator() {
    }

    public static ValidationReport validate(String tmxText, Catalog catalog, Function<String, String> templates) {
        List<Finding> findings = new ArrayList<>();
        XmlReader.Element map = null;
        try {
            map = new XmlReader().parse(tmxText == null ? "" : tmxText);
        } catch (RuntimeException e) {
            findings.add(reject("parse", "The text is not well-formed XML: " + e.getMessage()));
            return new ValidationReport(findings, null);
        }
        if (map == null || !"map".equals(map.getName())) {
            findings.add(reject("parse", "The root element is not map."));
            return new ValidationReport(findings, null);
        }
        try {
            checkStructure(map, catalog, findings);
        } catch (RuntimeException e) {
            findings.add(reject("parse", "The document cannot be read: " + e.getMessage()));
        }
        return new ValidationReport(findings, map);
    }

    private static void checkStructure(XmlReader.Element map, Catalog catalog, List<Finding> findings) {
        int width = intAttr(map, "width");
        int height = intAttr(map, "height");
        checkMap(map, width, height, findings);

        if (!map.getChildrenByName("group").isEmpty()) {
            findings.add(reject("group-layer", "Group layers are not supported."));
        }

        Catalog.Theme theme = theme(map, catalog);
        if (theme != null) {
            checkTilesets(map, theme, findings);
        }
        checkLayers(map, width, height, theme, findings);
    }

    private static void checkMap(XmlReader.Element map, int width, int height, List<Finding> findings) {
        List<String> problems = new ArrayList<>();
        if (!"orthogonal".equals(map.getAttribute("orientation", ""))) {
            problems.add("orientation is not orthogonal");
        }
        if (intAttr(map, "tilewidth") != 16 || intAttr(map, "tileheight") != 16) {
            problems.add("tile size is not 16 by 16");
        }
        if (intAttr(map, "infinite") != 0) {
            problems.add("the map is infinite");
        }
        if (width <= 0 || width % 16 != 0 || height <= 0 || height % 16 != 0) {
            problems.add("width and height are not a multiple of 16");
        }
        if (!problems.isEmpty()) {
            findings.add(reject("map", "Map attributes: " + String.join(", ", problems) + "."));
        }
    }

    private static Catalog.Theme theme(XmlReader.Element map, Catalog catalog) {
        XmlReader.Element props = map.getChildByName("properties");
        if (props == null) {
            return null;
        }
        for (XmlReader.Element p : props.getChildrenByName("property")) {
            if ("x6_theme".equals(p.getAttribute("name", ""))) {
                return catalog.theme(p.getAttribute("value", ""));
            }
        }
        return null;
    }

    private static void checkTilesets(XmlReader.Element map, Catalog.Theme theme, List<Finding> findings) {
        List<XmlReader.Element> sets = new ArrayList<>();
        map.getChildrenByName("tileset").forEach(sets::add);
        if (sets.size() != 2) {
            findings.add(reject("tileset", "The map needs exactly 2 tilesets, found " + sets.size() + "."));
            return;
        }
        for (XmlReader.Element ts : sets) {
            if (ts.getAttribute("source", "").isEmpty()) {
                findings.add(reject("tileset", "An embedded tileset is not supported."));
                return;
            }
        }
        XmlReader.Element art = sets.get(0);
        XmlReader.Element collision = sets.get(1);
        if (intAttr(art, "firstgid") != 1 || !theme.artSource().equals(baseName(art.getAttribute("source", "")))) {
            findings.add(reject("tileset", "The first tileset must be " + theme.artSource() + " at firstgid 1."));
        }
        if (!"collision.tsx".equals(baseName(collision.getAttribute("source", "")))
                || intAttr(collision, "firstgid") != theme.artTilecount() + 1) {
            findings.add(reject("tileset", "The second tileset must be collision.tsx at firstgid "
                    + (theme.artTilecount() + 1) + "."));
        }
    }

    private static void checkLayers(XmlReader.Element map, int width, int height, Catalog.Theme theme,
                                    List<Finding> findings) {
        for (String name : TILE_LAYERS) {
            XmlReader.Element layer = findLayer(map, name);
            if (layer == null) {
                findings.add(reject("layer-missing", "The tile layer \"" + name + "\" is missing."));
                continue;
            }
            checkLayer(layer, name, width, height, theme, findings);
        }
    }

    private static void checkLayer(XmlReader.Element layer, String name, int width, int height,
                                   Catalog.Theme theme, List<Finding> findings) {
        if (intAttr(layer, "width") != width || intAttr(layer, "height") != height) {
            findings.add(reject("layer", "The layer \"" + name + "\" differs in size from the map."));
            return;
        }
        XmlReader.Element data = layer.getChildByName("data");
        if (data == null || !"csv".equals(data.getAttribute("encoding", ""))
                || data.getAttribute("compression", null) != null) {
            findings.add(reject("layer", "The layer \"" + name + "\" data must be CSV with no compression."));
            return;
        }
        String text = data.getText() == null ? "" : data.getText();
        String[] cells = text.trim().isEmpty() ? new String[0] : text.trim().split("\\s*,\\s*");
        if ((long) cells.length != (long) width * height) {
            findings.add(reject("layer", "The layer \"" + name + "\" does not hold width times height tiles."));
            return;
        }
        long[] gids = new long[cells.length];
        boolean flipped = false;
        for (int i = 0; i < cells.length; i++) {
            try {
                gids[i] = Long.parseLong(cells[i]);
            } catch (NumberFormatException e) {
                findings.add(reject("layer", "The layer \"" + name + "\" has a value that is not a number."));
                return;
            }
            if (gids[i] < 0 || gids[i] > 0xFFFFFFFFL) {
                findings.add(reject("layer", "The layer \"" + name + "\" has a value out of range."));
                return;
            }
            if ((gids[i] & FLIP_BITS) != 0) {
                flipped = true;
                gids[i] &= ~FLIP_BITS;
            }
        }
        if (flipped) {
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < gids.length; i++) {
                if (i > 0) {
                    sb.append(',');
                }
                sb.append(gids[i]);
            }
            data.setText(sb.toString());
            findings.add(new Finding(Finding.Result.REPAIR, "flip-bits",
                    "The layer \"" + name + "\" has flipped tiles; the flip bits are cleared."));
        }
        if (theme == null) {
            return;
        }
        int art = theme.artTilecount();
        boolean collision = "collision".equals(name);
        for (long gid : gids) {
            if (collision ? gid != 0 && (gid < art + 1 || gid > art + 64) : gid > art) {
                findings.add(reject("gid-range", "The layer \"" + name + "\" has the tile id " + gid
                        + " out of range."));
                return;
            }
        }
    }

    private static XmlReader.Element findLayer(XmlReader.Element map, String name) {
        for (XmlReader.Element l : map.getChildrenByName("layer")) {
            if (name.equals(l.getAttribute("name", ""))) {
                return l;
            }
        }
        return null;
    }

    private static int intAttr(XmlReader.Element e, String name) {
        try {
            return Integer.parseInt(e.getAttribute(name, "").trim());
        } catch (NumberFormatException ex) {
            return -1;
        }
    }

    private static String baseName(String path) {
        return path.substring(path.lastIndexOf('/') + 1);
    }

    private static Finding reject(String code, String message) {
        return new Finding(Finding.Result.REJECT, code, message);
    }
}
