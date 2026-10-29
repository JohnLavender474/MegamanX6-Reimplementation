package mmx.x6.level;

import com.badlogic.gdx.utils.XmlReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.function.Function;

final class ContentRules {

    private static final List<String> TILE_LAYERS = List.of("art 2 background", "art 1 decoration",
            "art 0 main", "art 0 front", "collision");
    private static final List<String> THEME_PROPERTIES = List.of("x6_music", "x6_boss_music", "x6_bank2",
            "x6_bank5", "x6_boss_bank2", "x6_nightmare_effect");
    private static final List<String> RECORD_INTS = List.of("kind", "b", "id", "a", "index");
    private static final List<String> CHECKPOINT_REQUIRED = List.of("checkpoint", "cam_x", "cam_y", "min_x",
            "max_x", "min_y", "max_y", "face_right", "beam_thru");
    private static final List<String> CHECKPOINT_OPTIONAL_INTS = List.of("layer1_x", "layer1_y",
            "layer2_offset_x", "layer2_offset_y");
    private static final long FLIP_BITS = 0xE0000000L;

    private ContentRules() {
    }

    static void check(XmlReader.Element map, Catalog catalog, Catalog.Theme theme,
                      Function<String, String> templates, List<Finding> findings) {
        Map<String, String> mapProps = properties(map);
        checkProfile(mapProps, findings);
        if (theme != null) {
            checkThemeProperties(mapProps, theme, findings);
        }
        checkCheckpoints(map, findings);
        checkRecordProperties(map, findings);
        checkRecordIndex(map, findings);
        checkEntities(map, catalog, theme, findings);
        if (theme != null && theme.fixedRegion() != null) {
            checkFixedRegion(map, theme.fixedRegion(), templates, findings);
        }
    }

    private static Map<String, String> properties(XmlReader.Element e) {
        Map<String, String> out = new LinkedHashMap<>();
        XmlReader.Element props = e.getChildByName("properties");
        if (props != null) {
            for (XmlReader.Element p : props.getChildrenByName("property")) {
                out.put(p.getAttribute("name", ""), p.getAttribute("value", ""));
            }
        }
        return out;
    }

    private static boolean isInt(String text) {
        if (text == null) {
            return false;
        }
        try {
            Integer.parseInt(text.trim());
            return true;
        } catch (NumberFormatException e) {
            return false;
        }
    }

    private static boolean isBool(String text) {
        return "true".equals(text) || "false".equals(text);
    }

    private static XmlReader.Element objectGroup(XmlReader.Element map, String name) {
        for (XmlReader.Element g : map.getChildrenByName("objectgroup")) {
            if (name.equals(g.getAttribute("name", ""))) {
                return g;
            }
        }
        return null;
    }

    private static List<XmlReader.Element> recordObjects(XmlReader.Element map, boolean fixedToo) {
        List<XmlReader.Element> out = new ArrayList<>();
        XmlReader.Element records = objectGroup(map, "records");
        if (records != null) {
            records.getChildrenByName("object").forEach(out::add);
        }
        if (fixedToo) {
            XmlReader.Element fixed = objectGroup(map, "fixed records");
            if (fixed != null) {
                fixed.getChildrenByName("object").forEach(out::add);
            }
        }
        return out;
    }

    private static void checkProfile(Map<String, String> mapProps, List<Finding> findings) {
        String v = mapProps.get("x6_profile");
        if (v == null || !isInt(v) || Integer.parseInt(v.trim()) != 1) {
            findings.add(LevelValidator.reject("profile", "The map property x6_profile must be the int 1."));
        }
    }

    private static void checkThemeProperties(Map<String, String> mapProps, Catalog.Theme theme,
                                             List<Finding> findings) {
        for (String key : THEME_PROPERTIES) {
            String expected = theme.properties().get(key);
            String actual = mapProps.get(key);
            if (actual == null) {
                findings.add(LevelValidator.reject("property", "The map property " + key + " is missing."));
            } else if (expected != null && !expected.equals(actual.trim())) {
                findings.add(LevelValidator.reject("property", "The map property " + key + " must be "
                        + expected + " for the theme " + theme.key() + "."));
            }
        }
    }

    private static void checkCheckpoints(XmlReader.Element map, List<Finding> findings) {
        XmlReader.Element group = objectGroup(map, "x6 checkpoints");
        if (group == null) {
            findings.add(LevelValidator.reject("checkpoint", "The object layer \"x6 checkpoints\" is missing."));
            return;
        }
        List<Integer> numbers = new ArrayList<>();
        boolean numbersValid = true;
        for (XmlReader.Element o : group.getChildrenByName("object")) {
            String label = o.getAttribute("name", "?");
            if (o.getChildByName("point") == null) {
                findings.add(LevelValidator.reject("checkpoint", "The checkpoint object \"" + label
                        + "\" is not a point."));
            }
            Map<String, String> p = properties(o);
            for (String key : CHECKPOINT_REQUIRED) {
                if (!p.containsKey(key)) {
                    findings.add(LevelValidator.reject("checkpoint", "The checkpoint object \"" + label
                            + "\" has no property " + key + "."));
                }
            }
            for (Map.Entry<String, String> e : p.entrySet()) {
                String key = e.getKey();
                boolean ok;
                if ("face_right".equals(key)) {
                    ok = isBool(e.getValue());
                } else if (CHECKPOINT_REQUIRED.contains(key) || CHECKPOINT_OPTIONAL_INTS.contains(key)) {
                    ok = isInt(e.getValue());
                } else {
                    continue;
                }
                if (!ok) {
                    findings.add(LevelValidator.reject("property-type", "The checkpoint property " + key
                            + " of \"" + label + "\" has the wrong type."));
                    if ("checkpoint".equals(key)) {
                        numbersValid = false;
                    }
                }
            }
            if (p.containsKey("checkpoint") && isInt(p.get("checkpoint"))) {
                numbers.add(Integer.parseInt(p.get("checkpoint").trim()));
            }
        }
        if (numbersValid) {
            List<Integer> sorted = new ArrayList<>(numbers);
            Collections.sort(sorted);
            boolean sequence = !sorted.isEmpty();
            for (int i = 0; i < sorted.size(); i++) {
                if (sorted.get(i) != i) {
                    sequence = false;
                    break;
                }
            }
            if (!sequence) {
                findings.add(LevelValidator.reject("checkpoint",
                        "The checkpoint numbers must be unique and exactly 0 to n-1."));
            }
        }
    }

    private static void checkRecordProperties(XmlReader.Element map, List<Finding> findings) {
        for (XmlReader.Element o : recordObjects(map, true)) {
            Map<String, String> p = properties(o);
            for (String key : RECORD_INTS) {
                String v = p.get(key);
                if (v != null && !isInt(v)) {
                    findings.add(LevelValidator.reject("property-type", "The record property " + key + " of \""
                            + o.getAttribute("name", "?") + "\" is not an int."));
                }
            }
        }
    }

    private static void checkRecordIndex(XmlReader.Element map, List<Finding> findings) {
        XmlReader.Element group = objectGroup(map, "records");
        if (group == null) {
            return;
        }
        List<XmlReader.Element> objects = new ArrayList<>();
        group.getChildrenByName("object").forEach(objects::add);
        List<Integer> indices = new ArrayList<>();
        for (XmlReader.Element o : objects) {
            String v = properties(o).get("index");
            if (v != null && !isInt(v)) {
                return;
            }
            indices.add(v == null ? null : Integer.parseInt(v.trim()));
        }
        boolean ok = true;
        boolean[] seen = new boolean[objects.size()];
        for (Integer i : indices) {
            if (i == null || i < 0 || i >= seen.length || seen[i]) {
                ok = false;
                break;
            }
            seen[i] = true;
        }
        if (ok) {
            return;
        }
        Map<Integer, Integer> remap = new HashMap<>();
        for (int n = 0; n < objects.size(); n++) {
            Integer old = indices.get(n);
            if (old != null) {
                remap.putIfAbsent(old, n);
            }
            setProperty(objects.get(n), "index", "int", String.valueOf(n));
        }
        XmlReader.Element zones = objectGroup(map, "camera zones");
        if (zones != null) {
            for (XmlReader.Element z : zones.getChildrenByName("object")) {
                String v = properties(z).get("record");
                if (v != null && isInt(v) && remap.containsKey(Integer.parseInt(v.trim()))) {
                    setProperty(z, "record", "int", String.valueOf(remap.get(Integer.parseInt(v.trim()))));
                }
            }
        }
        findings.add(new Finding(Finding.Result.REPAIR, "record-index",
                "The record index values are renumbered in document order."));
    }

    private static void setProperty(XmlReader.Element object, String name, String type, String value) {
        XmlReader.Element props = object.getChildByName("properties");
        if (props == null) {
            props = new XmlReader.Element("properties", object);
            object.addChild(props);
        }
        for (XmlReader.Element p : props.getChildrenByName("property")) {
            if (name.equals(p.getAttribute("name", ""))) {
                p.setAttribute("value", value);
                return;
            }
        }
        XmlReader.Element p = new XmlReader.Element("property", props);
        p.setAttribute("name", name);
        p.setAttribute("type", type);
        p.setAttribute("value", value);
        props.addChild(p);
    }

    private static void checkEntities(XmlReader.Element map, Catalog catalog, Catalog.Theme theme,
                                      List<Finding> findings) {
        for (XmlReader.Element o : recordObjects(map, true)) {
            String name = o.getAttribute("name", "");
            Catalog.Entity entity = catalog.entity(name);
            if (entity == null || (theme != null && !entity.themes().contains(theme.key()))) {
                findings.add(new Finding(Finding.Result.WARN, "entity-unknown",
                        "The object \"" + name + "\" is not known for this theme."));
            } else if (entity.stageBound()) {
                findings.add(new Finding(Finding.Result.LOCK, "stage-bound",
                        "The object \"" + name + "\" is stage-bound and cannot be changed."));
            }
        }
    }

    private static void checkFixedRegion(XmlReader.Element map, Catalog.Region region,
                                         Function<String, String> templates, List<Finding> findings) {
        String text = templates == null ? null : templates.apply(region.template());
        if (text == null) {
            findings.add(LevelValidator.reject("fixed-region", "template not found"));
            return;
        }
        XmlReader.Element template;
        try {
            template = new XmlReader().parse(text);
        } catch (RuntimeException e) {
            findings.add(LevelValidator.reject("fixed-region", "The template cannot be read."));
            return;
        }
        List<String> problems = new ArrayList<>();
        if (LevelValidator.intAttr(map, "width") != LevelValidator.intAttr(template, "width")
                || LevelValidator.intAttr(map, "height") != LevelValidator.intAttr(template, "height")) {
            problems.add("the map size differs");
        }
        int c0 = region.x0() / 16;
        int c1 = region.x1() / 16;
        int width = LevelValidator.intAttr(map, "width");
        for (String name : TILE_LAYERS) {
            if (!sameTiles(LevelValidator.findLayer(map, name), LevelValidator.findLayer(template, name),
                    width, c0, c1)) {
                problems.add("tiles of \"" + name + "\" differ");
            }
        }
        for (String name : List.of("records", "fixed records", "camera zones")) {
            if (!objects(objectGroup(map, name), region).equals(objects(objectGroup(template, name), region))) {
                problems.add("objects of \"" + name + "\" differ");
            }
        }
        if (!problems.isEmpty()) {
            findings.add(LevelValidator.reject("fixed-region", "The fixed region x " + region.x0() + " to "
                    + region.x1() + " differs from the template: " + String.join(", ", problems) + "."));
        }
    }

    private static long[] gids(XmlReader.Element layer) {
        if (layer == null) {
            return null;
        }
        XmlReader.Element data = layer.getChildByName("data");
        if (data == null || data.getText() == null) {
            return null;
        }
        String t = data.getText().trim();
        if (t.isEmpty()) {
            return new long[0];
        }
        String[] cells = t.split("\\s*,\\s*");
        long[] out = new long[cells.length];
        try {
            for (int i = 0; i < cells.length; i++) {
                out[i] = Long.parseLong(cells[i]) & ~FLIP_BITS;
            }
        } catch (NumberFormatException e) {
            return null;
        }
        return out;
    }

    private static boolean sameTiles(XmlReader.Element a, XmlReader.Element b, int width, int c0, int c1) {
        long[] ga = gids(a);
        long[] gb = gids(b);
        if (ga == null || gb == null || ga.length != gb.length) {
            return false;
        }
        if (width <= 0) {
            return true;
        }
        for (int i = 0; i < ga.length; i++) {
            int col = i % width;
            if (col >= c0 && col <= c1 && ga[i] != gb[i]) {
                return false;
            }
        }
        return true;
    }

    private static double number(XmlReader.Element e, String attr) {
        try {
            return Double.parseDouble(e.getAttribute(attr, "0"));
        } catch (NumberFormatException ex) {
            return Double.NaN;
        }
    }

    private static List<String> objects(XmlReader.Element group, Catalog.Region region) {
        List<String> out = new ArrayList<>();
        if (group == null) {
            return out;
        }
        for (XmlReader.Element o : group.getChildrenByName("object")) {
            double x = number(o, "x");
            if (x >= region.x0() && x <= region.x1()) {
                out.add(o.getAttribute("name", "") + "|" + o.getAttribute("type", "") + "|" + x + "|"
                        + number(o, "y") + "|" + number(o, "width") + "|" + number(o, "height") + "|"
                        + new TreeMap<>(properties(o)));
            }
        }
        Collections.sort(out);
        return out;
    }
}
