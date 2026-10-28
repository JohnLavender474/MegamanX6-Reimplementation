package mmx.x6.level;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

public final class TmxLoader {

    private TmxLoader() {
    }

    public static Level load(Path tmx) throws IOException {
        Element map = parse(tmx);
        int width = intAttr(map, "width");
        int height = intAttr(map, "height");

        int collisionFirstGid = -1;
        for (Element ts : children(map, "tileset")) {
            if ("collision".equals(tilesetName(tmx, ts))) {
                collisionFirstGid = intAttr(ts, "firstgid");
            }
        }
        if (collisionFirstGid < 0) {
            throw new IOException(tmx + " has no collision tileset");
        }

        Map<String, TileLayer> layers = new HashMap<>();
        for (Element layer : children(map, "layer")) {
            int lw = intAttr(layer, "width");
            int lh = intAttr(layer, "height");
            Map<String, String> props = properties(layer);
            int[] gids = parseCsv(children(layer, "data").get(0).getTextContent(), lw * lh);
            layers.put(layer.getAttribute("name"), new TileLayer(layer.getAttribute("name"), lw, lh, gids,
                    intOr(props, "x6_mode", 0), intOr(props, "x6_offset_x", 0), intOr(props, "x6_offset_y", 0)));
        }
        TileLayer collisionLayer = require(layers, "collision", tmx);
        int[] values = new int[width * height];
        for (int i = 0; i < values.length; i++) {
            int gid = collisionLayer.gids()[i] & 0x1FFFFFFF;
            values[i] = gid == 0 ? 0 : gid - collisionFirstGid;
        }

        List<RecordDef> records = new ArrayList<>();
        List<RecordDef> fixed = new ArrayList<>();
        List<CameraZoneDef> zones = new ArrayList<>();
        for (Element group : children(map, "objectgroup")) {
            String groupName = group.getAttribute("name");
            for (Element o : children(group, "object")) {
                Map<String, String> p = properties(o);
                int x = (int) Double.parseDouble(o.getAttribute("x"));
                int y = (int) Double.parseDouble(o.getAttribute("y"));
                if ("records".equals(groupName) || "fixed records".equals(groupName)) {
                    RecordDef r = new RecordDef(intOr(p, "index", -1), o.getAttribute("name"), o.getAttribute("type"),
                            intOr(p, "kind", 0), intOr(p, "b", 0), intOr(p, "id", 0), intOr(p, "a", 0), x, y);
                    ("records".equals(groupName) ? records : fixed).add(r);
                } else if ("camera zones".equals(groupName)) {
                    int w = (int) Double.parseDouble(o.getAttribute("width"));
                    int h = (int) Double.parseDouble(o.getAttribute("height"));
                    zones.add(new CameraZoneDef(intOr(p, "record", -1), intOr(p, "zone_id", -1) & 0x7F,
                            x, x + w - 1, y, y + h - 1, intOr(p, "min_x", -1), intOr(p, "max_x", -1),
                            intOr(p, "min_y", -1), intOr(p, "max_y", -1), intOr(p, "texture_swap", 0) != 0));
                }
            }
        }

        return new Level(width, height, require(layers, "art 2 background", tmx),
                require(layers, "art 1 decoration", tmx), require(layers, "art 0 main", tmx),
                require(layers, "art 0 front", tmx), new CollisionGrid(width, height, values),
                List.copyOf(records), List.copyOf(zones), List.copyOf(fixed));
    }

    private static Element parse(Path xml) throws IOException {
        try (InputStream in = Files.newInputStream(xml)) {
            DocumentBuilderFactory f = DocumentBuilderFactory.newInstance();
            f.setFeature("http://apache.org/xml/features/disallow-doctype-decl", true);
            Document doc = f.newDocumentBuilder().parse(in);
            return doc.getDocumentElement();
        } catch (ParserConfigurationException | SAXException e) {
            throw new IOException("cannot parse " + xml + ": " + e.getMessage(), e);
        }
    }

    private static String tilesetName(Path tmx, Element ts) throws IOException {
        String source = ts.getAttribute("source");
        if (source.isEmpty()) {
            return ts.getAttribute("name");
        }
        return parse(tmx.resolveSibling(source).normalize()).getAttribute("name");
    }

    private static TileLayer require(Map<String, TileLayer> layers, String name, Path tmx) throws IOException {
        TileLayer l = layers.get(name);
        if (l == null) {
            throw new IOException(tmx + " has no layer '" + name + "'");
        }
        return l;
    }

    private static int[] parseCsv(String text, int count) {
        int[] out = new int[count];
        int i = 0;
        for (String part : text.split(",")) {
            String s = part.trim();
            if (!s.isEmpty() && i < count) {
                out[i++] = (int) Long.parseLong(s);
            }
        }
        return out;
    }

    private static Map<String, String> properties(Element e) {
        Map<String, String> out = new HashMap<>();
        for (Element props : children(e, "properties")) {
            for (Element p : children(props, "property")) {
                out.put(p.getAttribute("name"), p.getAttribute("value"));
            }
        }
        return out;
    }

    private static int intOr(Map<String, String> props, String key, int fallback) {
        String v = props.get(key);
        return v == null || v.isEmpty() ? fallback : Integer.parseInt(v);
    }

    private static int intAttr(Element e, String name) {
        return Integer.parseInt(e.getAttribute(name));
    }

    private static List<Element> children(Element parent, String tag) {
        List<Element> out = new ArrayList<>();
        NodeList nodes = parent.getChildNodes();
        for (int i = 0; i < nodes.getLength(); i++) {
            Node n = nodes.item(i);
            if (n instanceof Element el && tag.equals(el.getTagName())) {
                out.add(el);
            }
        }
        return out;
    }
}
