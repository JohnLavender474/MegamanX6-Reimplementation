package mmx.x6.anim;

import com.badlogic.gdx.utils.JsonReader;
import com.badlogic.gdx.utils.JsonValue;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import mmx.x6.physics.Rect;

public final class AnimSet {

    private final int cellW;
    private final int cellH;
    private final int originX;
    private final int originY;
    private final Map<Integer, AnimDef> anims;
    private final Map<Integer, List<Rect>> bounds;

    public AnimSet(int cellW, int cellH, int originX, int originY, Map<Integer, AnimDef> anims) {
        this(cellW, cellH, originX, originY, anims, Map.of());
    }

    private AnimSet(int cellW, int cellH, int originX, int originY, Map<Integer, AnimDef> anims,
                    Map<Integer, List<Rect>> bounds) {
        this.cellW = cellW;
        this.cellH = cellH;
        this.originX = originX;
        this.originY = originY;
        this.anims = anims;
        this.bounds = bounds;
    }

    public AnimSet withBounds(Map<Integer, List<Rect>> bounds) {
        return new AnimSet(cellW, cellH, originX, originY, anims, Map.copyOf(bounds));
    }

    public static AnimSet parse(String json) {
        JsonValue root = new JsonReader().parse(json);
        JsonValue cell = root.get("cell");
        JsonValue origin = root.get("origin");
        Map<Integer, AnimDef> anims = new TreeMap<>();
        JsonValue list = root.get("animations");
        if (list != null) {
            for (JsonValue a = list.child; a != null; a = a.next) {
                List<AnimDef.Frame> frames = new ArrayList<>();
                for (JsonValue f = a.get("frames").child; f != null; f = f.next) {
                    int ticks = f.getInt("ticks");
                    frames.add(new AnimDef.Frame(f.getInt("sprite"), ticks <= 0 ? 256 : ticks, f.getInt("flag", 0)));
                }
                JsonValue loop = a.get("loop_to");
                Integer loopTo = loop == null || loop.isNull() ? null : loop.asInt();
                anims.put(a.getInt("index"), new AnimDef(a.getInt("index"), List.copyOf(frames), loopTo));
            }
        }
        return new AnimSet(cell.getInt(0), cell.getInt(1), origin.getInt(0), origin.getInt(1), anims);
    }

    public int cellW() {
        return cellW;
    }

    public int cellH() {
        return cellH;
    }

    public int originX() {
        return originX;
    }

    public int originY() {
        return originY;
    }

    public AnimDef get(int index) {
        return anims.get(index);
    }

    public Rect bounds(int anim, int frame) {
        List<Rect> frames = bounds.get(anim);
        return frames == null || frame < 0 || frame >= frames.size() ? null : frames.get(frame);
    }

    public Set<Integer> indices() {
        return Collections.unmodifiableSet(anims.keySet());
    }
}
