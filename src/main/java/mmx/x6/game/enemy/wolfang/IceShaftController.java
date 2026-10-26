package mmx.x6.game.enemy.wolfang;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;

public final class IceShaftController extends Entity {

    public enum State { WAIT, FIRST_ROWS, ROWS }

    private static final float ROW_X = 5880;
    private static final int[] FIRST_Y = {1032, 888, 744, 600, 456};
    private static final int MAX_ROWS = 16;
    private static final int PERIOD = 144;

    private State state = State.WAIT;
    private int rows;
    private int timer;

    public IceShaftController(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
    }

    public State state() {
        return state;
    }

    void rowGone() {
        rows--;
    }

    @Override
    public void update(World w) {
        switch (state) {
            case WAIT -> {
                if (w.player.x >= 5953 && w.player.y < 1244) {
                    w.copyTiles(6144, 1264, 3, 3, 5952, 1264);
                    w.camera.setTargets(-1, -1, 320, 1072);
                    for (Entity e : w.entities) {
                        if (e instanceof IceStackBlock) {
                            e.dead = true;
                        }
                    }
                    state = State.FIRST_ROWS;
                }
            }
            case FIRST_ROWS -> {
                for (int ry : FIRST_Y) {
                    w.add(new IceStackBlock.IceRow(w, this, w.rng.nextInt(5) + 1, ROW_X, ry));
                }
                rows = FIRST_Y.length;
                timer = 0;
                state = State.ROWS;
            }
            case ROWS -> {
                if (rows < MAX_ROWS && timer % PERIOD == 0) {
                    w.add(new IceStackBlock.IceRow(w, this, w.rng.nextInt(5) + 1, ROW_X, 312));
                    rows++;
                }
                timer++;
                // TODO: the camera field +0x2C (+48 / -48 in the shaft) is not read; it is not done (docs/gaps.md, "Ice stack block")
            }
        }
    }

    @Override
    public boolean visible() {
        return false;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }
}
