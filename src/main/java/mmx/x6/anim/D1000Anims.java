package mmx.x6.anim;

import java.util.Map;
import java.util.TreeMap;

public final class D1000Anims {

    public static final int LAST_BODY_ANIM = 5;
    public static final int LAST_ANIM = 23;

    private D1000Anims() {
    }

    public static AnimSet merge(AnimSet body, AnimSet parts) {
        Map<Integer, AnimDef> merged = new TreeMap<>();
        for (int i = 0; i <= LAST_ANIM; i++) {
            AnimDef d = (i <= LAST_BODY_ANIM ? body : parts).get(i);
            if (d != null) {
                merged.put(i, d);
            }
        }
        return new AnimSet(body.cellW(), body.cellH(), body.originX(), body.originY(), merged);
    }
}
