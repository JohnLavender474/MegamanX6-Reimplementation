package mmx.x6.level;

import com.badlogic.gdx.utils.JsonReader;
import com.badlogic.gdx.utils.JsonValue;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public final class Catalog {

    public record Region(int x0, int x1, String template) {
    }

    public record Theme(String key, String artSource, int artTilecount, String tilesPng,
                        Map<String, String> properties, Region fixedRegion) {
    }

    public record Entity(String name, List<String> themes, boolean stageBound, boolean defeatable,
                         boolean openable) {
    }

    private final Map<String, Theme> themes;
    private final Map<String, Entity> entities;

    private Catalog(Map<String, Theme> themes, Map<String, Entity> entities) {
        this.themes = themes;
        this.entities = entities;
    }

    public static Catalog load(Path file) throws IOException {
        return parse(Files.readString(file, StandardCharsets.UTF_8));
    }

    public static Catalog parse(String json) {
        JsonValue root = new JsonReader().parse(json);
        Map<String, Theme> themes = new LinkedHashMap<>();
        JsonValue themeNode = root.get("themes");
        if (themeNode != null) {
            for (JsonValue t = themeNode.child; t != null; t = t.next) {
                Map<String, String> props = new LinkedHashMap<>();
                JsonValue p = t.get("properties");
                if (p != null) {
                    for (JsonValue v = p.child; v != null; v = v.next) {
                        props.put(v.name, v.asString());
                    }
                }
                Region region = null;
                JsonValue fr = t.get("fixed_region");
                if (fr != null) {
                    region = new Region(fr.getInt("x0"), fr.getInt("x1"), fr.getString("template"));
                }
                themes.put(t.name, new Theme(t.name, t.getString("art_source"), t.getInt("art_tilecount"),
                        t.getString("tiles_png"), props, region));
            }
        }
        Map<String, Entity> entities = new LinkedHashMap<>();
        JsonValue entityNode = root.get("entities");
        if (entityNode != null) {
            for (JsonValue e = entityNode.child; e != null; e = e.next) {
                List<String> list = new ArrayList<>();
                JsonValue th = e.get("themes");
                if (th != null) {
                    for (JsonValue v = th.child; v != null; v = v.next) {
                        list.add(v.asString());
                    }
                }
                entities.put(e.name, new Entity(e.name, List.copyOf(list), e.getBoolean("stage_bound", false),
                        e.getBoolean("defeatable", false), e.getBoolean("openable", false)));
            }
        }
        return new Catalog(themes, entities);
    }

    public Theme theme(String key) {
        return themes.get(key);
    }

    public Entity entity(String name) {
        return entities.get(name);
    }

    public Set<String> themeKeys() {
        return themes.keySet();
    }
}
