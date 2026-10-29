package mmx.x6.level;

import com.badlogic.gdx.utils.GdxRuntimeException;
import com.badlogic.gdx.utils.XmlReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public final class TmxLoader {

    private TmxLoader() {
    }

    public static Level load(Path tmx) throws IOException {
        return parse(Files.readString(tmx, StandardCharsets.UTF_8), tmx.toString());
    }

    public static Level parse(String tmxText, String name) throws IOException {
        XmlReader.Element map;
        try {
            map = new XmlReader().parse(tmxText);
        } catch (GdxRuntimeException | IllegalArgumentException e) {
            throw new IOException("cannot parse " + name + ": " + e.getMessage(), e);
        }
        return fromElement(map, name);
    }

    public static Level fromElement(XmlReader.Element map, String name) throws IOException {
        int width = intAttr(map, "width");
        int height = intAttr(map, "height");

        int collisionFirstGid = -1;
        for (XmlReader.Element ts : children(map, "tileset")) {
            if (isCollisionTileset(ts)) {
                collisionFirstGid = intAttr(ts, "firstgid");
            }
        }
        if (collisionFirstGid < 0) {
            throw new IOException(name + " has no collision tileset");
        }

        Map<String, TileLayer> layers = new HashMap<>();
        for (XmlReader.Element layer : children(map, "layer")) {
            int lw = intAttr(layer, "width");
            int lh = intAttr(layer, "height");
            Map<String, String> props = properties(layer);
            List<XmlReader.Element> data = children(layer, "data");
            int[] gids = parseCsv(data.isEmpty() ? "" : data.get(0).getText(), lw * lh);
            String layerName = attr(layer, "name");
            layers.put(layerName, new TileLayer(layerName, lw, lh, gids,
                    intOr(props, "x6_mode", 0), intOr(props, "x6_offset_x", 0), intOr(props, "x6_offset_y", 0)));
        }
        TileLayer collisionLayer = require(layers, "collision", name);
        int[] values = new int[width * height];
        for (int i = 0; i < values.length; i++) {
            int gid = collisionLayer.gids()[i] & 0x1FFFFFFF;
            values[i] = gid == 0 ? 0 : gid - collisionFirstGid;
        }

        List<RecordDef> records = new ArrayList<>();
        List<RecordDef> fixed = new ArrayList<>();
        List<CameraZoneDef> zones = new ArrayList<>();
        List<Object[]> checkpointRows = new ArrayList<>();
        List<GoalDef> goals = new ArrayList<>();
        for (XmlReader.Element group : children(map, "objectgroup")) {
            String groupName = attr(group, "name");
            for (XmlReader.Element o : children(group, "object")) {
                Map<String, String> p = properties(o);
                int x = (int) Double.parseDouble(attr(o, "x"));
                int y = (int) Double.parseDouble(attr(o, "y"));
                if ("x6 checkpoints".equals(groupName)) {
                    checkpointRows.add(new Object[] {intOr(p, "checkpoint", 0), checkpoint(p, x, y)});
                } else if ("records".equals(groupName) || "fixed records".equals(groupName)) {
                    RecordDef r = new RecordDef(intOr(p, "index", -1), attr(o, "name"), attr(o, "type"),
                            intOr(p, "kind", 0), intOr(p, "b", 0), intOr(p, "id", 0), intOr(p, "a", 0), x, y,
                            objectId(o), intOr(p, "goal", 0), intOr(p, "opened_by", 0));
                    ("records".equals(groupName) ? records : fixed).add(r);
                } else if ("goals".equals(groupName) && "Goal".equals(attr(o, "type"))) {
                    int w = (int) Double.parseDouble(attr(o, "width"));
                    int h = (int) Double.parseDouble(attr(o, "height"));
                    goals.add(new GoalDef(objectId(o), x, y, x + w - 1, y + h - 1,
                            "any".equals(p.get("mode")) ? GoalMode.ANY : GoalMode.ALL,
                            "true".equals(p.get("ends_stage")), intOr(p, "goal", 0)));
                } else if ("camera zones".equals(groupName)) {
                    int w = (int) Double.parseDouble(attr(o, "width"));
                    int h = (int) Double.parseDouble(attr(o, "height"));
                    zones.add(new CameraZoneDef(intOr(p, "record", -1), intOr(p, "zone_id", -1) & 0x7F,
                            x, x + w - 1, y, y + h - 1, intOr(p, "min_x", -1), intOr(p, "max_x", -1),
                            intOr(p, "min_y", -1), intOr(p, "max_y", -1), intOr(p, "texture_swap", 0) != 0));
                }
            }
        }

        checkpointRows.sort(Comparator.comparingInt(row -> (Integer) row[0]));
        List<Checkpoint> checkpoints = new ArrayList<>();
        for (Object[] row : checkpointRows) {
            checkpoints.add((Checkpoint) row[1]);
        }

        return new Level(width, height, require(layers, "art 2 background", name),
                require(layers, "art 1 decoration", name), require(layers, "art 0 main", name),
                require(layers, "art 0 front", name), new CollisionGrid(width, height, values),
                List.copyOf(records), List.copyOf(zones), List.copyOf(fixed), Map.copyOf(properties(map)),
                List.copyOf(checkpoints), List.copyOf(goals));
    }

    private static Checkpoint checkpoint(Map<String, String> p, int x, int y) {
        Point layer1 = p.containsKey("layer1_x") && p.containsKey("layer1_y")
                ? new Point(intOr(p, "layer1_x", 0), intOr(p, "layer1_y", 0)) : null;
        Point layer2 = p.containsKey("layer2_offset_x") && p.containsKey("layer2_offset_y")
                ? new Point(intOr(p, "layer2_offset_x", 0), intOr(p, "layer2_offset_y", 0)) : null;
        String face = p.getOrDefault("face_right", "false");
        return new Checkpoint(x, y, intOr(p, "cam_x", 0), intOr(p, "cam_y", 0), intOr(p, "min_x", 0),
                intOr(p, "max_x", 0), intOr(p, "min_y", 0), intOr(p, "max_y", 0),
                "true".equals(face) || "1".equals(face), intOr(p, "beam_thru", 0), layer1, layer2);
    }

    private static boolean isCollisionTileset(XmlReader.Element ts) {
        String source = attr(ts, "source");
        if (source.isEmpty()) {
            return "collision".equals(attr(ts, "name"));
        }
        String base = source.substring(Math.max(source.lastIndexOf('/'), source.lastIndexOf('\\')) + 1);
        return "collision.tsx".equals(base);
    }

    private static TileLayer require(Map<String, TileLayer> layers, String layerName, String name) throws IOException {
        TileLayer l = layers.get(layerName);
        if (l == null) {
            throw new IOException(name + " has no layer '" + layerName + "'");
        }
        return l;
    }

    private static int[] parseCsv(String text, int count) {
        int[] out = new int[count];
        int i = 0;
        for (String part : (text == null ? "" : text).split(",")) {
            String s = part.trim();
            if (!s.isEmpty() && i < count) {
                out[i++] = (int) Long.parseLong(s);
            }
        }
        return out;
    }

    private static Map<String, String> properties(XmlReader.Element e) {
        Map<String, String> out = new HashMap<>();
        for (XmlReader.Element props : children(e, "properties")) {
            for (XmlReader.Element p : children(props, "property")) {
                out.put(attr(p, "name"), attr(p, "value"));
            }
        }
        return out;
    }

    private static int intOr(Map<String, String> props, String key, int fallback) {
        String v = props.get(key);
        return v == null || v.trim().isEmpty() ? fallback : Integer.parseInt(v.trim());
    }

    private static int objectId(XmlReader.Element o) {
        String v = attr(o, "id").trim();
        return v.isEmpty() ? 0 : Integer.parseInt(v);
    }

    private static String attr(XmlReader.Element e, String name) {
        return e.get(name, "");
    }

    private static int intAttr(XmlReader.Element e, String name) {
        return Integer.parseInt(attr(e, name).trim());
    }

    private static List<XmlReader.Element> children(XmlReader.Element parent, String tag) {
        List<XmlReader.Element> out = new ArrayList<>();
        for (int i = 0; i < parent.getChildCount(); i++) {
            XmlReader.Element c = parent.getChild(i);
            if (tag.equals(c.getName())) {
                out.add(c);
            }
        }
        return out;
    }
}
