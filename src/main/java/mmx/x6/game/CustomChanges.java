package mmx.x6.game;

import mmx.x6.level.CollisionGrid;
import mmx.x6.stage.StageDef;
import mmx.x6.stage.Stages;

public final class CustomChanges {

    public static final float SABRE_DAMAGE_SCALE = 1.5f;
    public static final int SABRE_RECOVERY_SPEED = 2;
    public static final int D1000_LANDING_DROP = 12;

    private static final int TURTLOID_CEILING_ROW = 208 / 16;
    private static final int TURTLOID_OPEN_FROM = 4128 / 16;
    private static final int TURTLOID_OPEN_TO = 4992 / 16;

    private CustomChanges() {
    }

    public static void apply(StageDef stage, CollisionGrid grid) {
        if (stage == Stages.TURTLOID) {
            for (int cx = TURTLOID_OPEN_FROM; cx < TURTLOID_OPEN_TO; cx++) {
                if (grid.raw(cx, TURTLOID_CEILING_ROW) == 57) {
                    grid.set(cx, TURTLOID_CEILING_ROW, 0);
                }
            }
        }
    }
}
