package mmx.x6.level;

import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public final class TmxFixture {

    public static final String CATALOG = """
            {
              "themes": {
                "intro": {
                  "art_source": "intro-tiles.tsx",
                  "art_tilecount": 3264,
                  "tiles_png": "intro-tiles.png",
                  "properties": {
                    "x6_music": "9", "x6_boss_music": "21", "x6_bank2": "m094-e05", "x6_bank5": "m094-e03",
                    "x6_boss_bank2": "m123-e01", "x6_nightmare_effect": "0"
                  }
                }
              },
              "entities": {
                "MetaWheelF": {"themes": ["intro"], "stage_bound": false, "defeatable": true, "openable": false},
                "BossGate": {"themes": ["intro"], "stage_bound": true, "defeatable": false, "openable": true},
                "D1000": {"themes": ["intro"], "stage_bound": true, "defeatable": true, "openable": false}
              }
            }
            """;

    private static final List<String> TILE_LAYERS = List.of("art 2 background", "art 1 decoration",
            "art 0 main", "art 0 front", "collision");

    private static final class Layer {
        final Map<String, String> attrs = new LinkedHashMap<>();
        String csv;
    }

    private final Map<String, String> mapAttrs = new LinkedHashMap<>();
    private final Map<String, String[]> mapProperties = new LinkedHashMap<>();
    private final Map<String, Layer> tileLayers = new LinkedHashMap<>();
    private final Map<String, String> objectLayers = new LinkedHashMap<>();
    private String tilesets = "<tileset firstgid=\"1\" source=\"../tsx/intro-tiles.tsx\"/>\n"
            + "<tileset firstgid=\"3265\" source=\"../tsx/collision.tsx\"/>\n";
    private String raw = "";

    private TmxFixture() {
    }

    public static TmxFixture valid() {
        TmxFixture f = new TmxFixture();
        f.mapAttrs.put("orientation", "orthogonal");
        f.mapAttrs.put("width", "16");
        f.mapAttrs.put("height", "16");
        f.mapAttrs.put("tilewidth", "16");
        f.mapAttrs.put("tileheight", "16");
        f.mapAttrs.put("infinite", "0");
        f.mapProperty("x6_profile", "int", "1");
        f.mapProperty("x6_theme", "string", "intro");
        f.mapProperty("x6_music", "int", "9");
        f.mapProperty("x6_boss_music", "int", "21");
        f.mapProperty("x6_bank2", "string", "m094-e05");
        f.mapProperty("x6_bank5", "string", "m094-e03");
        f.mapProperty("x6_boss_bank2", "string", "m123-e01");
        f.mapProperty("x6_nightmare_effect", "int", "0");
        String zeros = String.join(",", java.util.Collections.nCopies(256, "0"));
        for (String name : TILE_LAYERS) {
            Layer l = new Layer();
            l.attrs.put("width", "16");
            l.attrs.put("height", "16");
            l.csv = zeros;
            f.tileLayers.put(name, l);
        }
        f.objectLayers.put("records", "");
        f.objectLayers.put("fixed records", "");
        f.objectLayers.put("camera zones", "");
        f.objectLayers.put("x6 checkpoints", "<object id=\"1\" name=\"checkpoint 0\" x=\"32\" y=\"32\">"
                + "<properties>"
                + "<property name=\"checkpoint\" type=\"int\" value=\"0\"/>"
                + "<property name=\"cam_x\" type=\"int\" value=\"0\"/>"
                + "<property name=\"cam_y\" type=\"int\" value=\"0\"/>"
                + "<property name=\"min_x\" type=\"int\" value=\"0\"/>"
                + "<property name=\"max_x\" type=\"int\" value=\"256\"/>"
                + "<property name=\"min_y\" type=\"int\" value=\"0\"/>"
                + "<property name=\"max_y\" type=\"int\" value=\"256\"/>"
                + "<property name=\"face_right\" type=\"bool\" value=\"true\"/>"
                + "<property name=\"beam_thru\" type=\"int\" value=\"0\"/>"
                + "</properties><point/></object>");
        return f;
    }

    public TmxFixture mapAttr(String name, String value) {
        mapAttrs.put(name, value);
        return this;
    }

    public TmxFixture mapProperty(String name, String type, String value) {
        mapProperties.put(name, new String[] {type, value});
        return this;
    }

    public TmxFixture removeMapProperty(String name) {
        mapProperties.remove(name);
        return this;
    }

    public TmxFixture tilesets(String xml) {
        tilesets = xml;
        return this;
    }

    public TmxFixture removeLayer(String name) {
        tileLayers.remove(name);
        objectLayers.remove(name);
        return this;
    }

    public TmxFixture layerData(String layer, String csv) {
        tileLayers.get(layer).csv = csv;
        return this;
    }

    public TmxFixture layerAttr(String layer, String attr, String value) {
        tileLayers.get(layer).attrs.put(attr, value);
        return this;
    }

    public TmxFixture objectLayer(String name, String objectsXml) {
        objectLayers.put(name, objectsXml);
        return this;
    }

    public TmxFixture raw(String xmlBeforeMapEnd) {
        raw += xmlBeforeMapEnd;
        return this;
    }

    public String build() {
        StringBuilder sb = new StringBuilder("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<map");
        mapAttrs.forEach((k, v) -> sb.append(' ').append(k).append("=\"").append(v).append('"'));
        sb.append(">\n<properties>\n");
        mapProperties.forEach((k, v) -> sb.append("<property name=\"").append(k).append("\" type=\"")
                .append(v[0]).append("\" value=\"").append(v[1]).append("\"/>\n"));
        sb.append("</properties>\n").append(tilesets);
        int id = 1;
        for (Map.Entry<String, Layer> e : tileLayers.entrySet()) {
            sb.append("<layer id=\"").append(id++).append("\" name=\"").append(e.getKey()).append('"');
            e.getValue().attrs.forEach((k, v) -> sb.append(' ').append(k).append("=\"").append(v).append('"'));
            sb.append(">\n<data encoding=\"csv\">\n").append(e.getValue().csv).append("\n</data>\n</layer>\n");
        }
        for (Map.Entry<String, String> e : objectLayers.entrySet()) {
            sb.append("<objectgroup id=\"").append(id++).append("\" name=\"").append(e.getKey()).append("\">\n")
                    .append(e.getValue()).append("\n</objectgroup>\n");
        }
        sb.append(raw).append("</map>\n");
        return sb.toString();
    }
}
