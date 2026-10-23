package mmx.x6.anim;

import java.util.List;
import java.util.Map;
import java.util.TreeMap;

public final class SabreBlockAnims {

    public static final AnimSet SET;

    static {
        Map<Integer, AnimDef> anims = new TreeMap<>();
        for (int i = 0; i < 4; i++) {
            anims.put(i, new AnimDef(i, List.of(new AnimDef.Frame(i, 1, 0)), null));
        }
        SET = new AnimSet(64, 64, 32, 32, anims);
    }

    private SabreBlockAnims() {
    }
}
