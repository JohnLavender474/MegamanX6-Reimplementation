package mmx.x6.anim;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

public final class BankSheetAnims {

    public record Sheet(String stage, String state, int bank, AnimSet set, Map<Integer, Integer> bottomFromOrigin) {
    }

    public static final int CELL_W = 68;
    public static final int CELL_H = 107;
    public static final int COLUMNS = 8;
    public static final int LABEL_X = 2;
    public static final int LABEL_H = 11;

    public static final String SCARAVICH_ROCKS = "scaravich-rocks";

    public static final Map<String, Sheet> SHEETS = Map.of(SCARAVICH_ROCKS,
            new Sheet("scaravich", "scaravich-3_section", 5, scaravichRocks(), Map.of(17, 40, 18, 40, 19, 16)));

    private BankSheetAnims() {
    }

    private static AnimSet scaravichRocks() {
        Map<Integer, AnimDef> anims = new TreeMap<>();
        anims.put(0, hold(0, 0));
        List<AnimDef.Frame> boulder = new ArrayList<>();
        for (int s = 1; s <= 8; s++) {
            boulder.add(new AnimDef.Frame(s, 6, 0));
        }
        anims.put(1, new AnimDef(1, List.copyOf(boulder), 0));
        for (int a = 2; a <= 18; a++) {
            anims.put(a, hold(a, a + 7));
        }
        return new AnimSet(CELL_W, CELL_H, 0, 0, anims);
    }

    private static AnimDef hold(int anim, int sprite) {
        return new AnimDef(anim, List.of(new AnimDef.Frame(sprite, 256, 0)), null);
    }
}
