package mmx.x6.game.enemy.sheldon;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;

public final class SheldonBgSwitch extends Entity {

    private static final int LAYER1_OFFSET_Y = 320;
    private static final int LAYER2_Y = 448;

    private boolean switched;

    public SheldonBgSwitch(World w, RecordDef r) {
        this.record = r;
    }

    public boolean switched() {
        return switched;
    }

    @Override
    public void update(World w) {
        if (!switched) {
            if (w.checkpoint == 0) {
                return;
            }
            switched = true;
            w.camera.setLayer1Offset(0, LAYER1_OFFSET_Y);
            w.camera.setLayer2Mode(5, 0, 0);
        }
        w.camera.setLayer2(w.camera.layer1X() / 4, LAYER2_Y);
    }

    @Override
    public boolean visible() {
        return false;
    }

    @Override
    public boolean runsInPause() {
        return true;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }
}
