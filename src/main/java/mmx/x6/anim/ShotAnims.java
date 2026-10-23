package mmx.x6.anim;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

public final class ShotAnims {

    public static final Map<Integer, String> FILES;
    public static final AnimSet SET;

    static {
        Map<Integer, AnimDef> anims = new TreeMap<>();
        Map<Integer, String> files = new HashMap<>();
        add(anims, files, 21, "anim-21-lemon.png", new int[] {1, 2, 2, 2, 2, 2}, -1, 1);
        add(anims, files, 26, "anim-26-half-charge.png", new int[] {2, 2, 2, 2}, -1, 0);
        add(anims, files, 27, "anim-27-full-charge-18.png", new int[] {3, 3, 3}, -1, 0);
        add(anims, files, 22, "anim-22-half-charge-flash.png", new int[] {1, 1, 1, 1, 1, 1}, 3, null);
        add(anims, files, 25, "anim-25-full-charge-flash-falcon.png", new int[] {1, 1, 1, 1, 1, 1, 1, 1}, 5, null);
        add(anims, files, 39, "anim-39-lemon-flash.png", new int[] {1, 1, 1, 1}, -1, null);
        FILES = Map.copyOf(files);
        SET = new AnimSet(0, 0, 0, 0, anims);
    }

    private ShotAnims() {
    }

    private static void add(Map<Integer, AnimDef> anims, Map<Integer, String> files, int index, String file,
                            int[] ticks, int flagFrame, Integer loopTo) {
        List<AnimDef.Frame> frames = new ArrayList<>();
        for (int i = 0; i < ticks.length; i++) {
            frames.add(new AnimDef.Frame(i, ticks[i], i == flagFrame ? 1 : 0));
        }
        anims.put(index, new AnimDef(index, List.copyOf(frames), loopTo));
        files.put(index, file);
    }
}
