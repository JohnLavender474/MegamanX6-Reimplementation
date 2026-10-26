package mmx.x6.game.enemy;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.player.Player;
import mmx.x6.game.player.PlayerConst;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.TerrainMover;

public final class IceSlide extends Entity {

    public enum State { ARM, SLIDE }

    private static final int ICE = 60;
    private static final float SLOPE_PUSH = 3.5f;
    private static final float SLOW = 0.09375f;
    private static final float DASH_GAIN = 0.15625f;
    private static final float DASH_RUN_OUT = 0.0234375f;

    private State state = State.ARM;
    private float v;
    private float a;
    private int timer;
    private boolean dashMode;
    private int lastValue;
    private float lastMove;
    private PlayerStep lastStep;

    public IceSlide(World w, RecordDef r) {
        this.record = r;
    }

    public State state() {
        return state;
    }

    public float speed() {
        return v;
    }

    private static boolean risesRight(int value) {
        return value >= 17 && value <= 20 || value == 25 || value == 26;
    }

    private static boolean fallsRight(int value) {
        return value >= 21 && value <= 24 || value == 27 || value == 28;
    }

    private static int valueUnder(World w, Player p) {
        float feet = p.body().bottom();
        int v = w.grid.raw((int) Math.floor(p.x / 16f), (int) Math.floor(feet / 16f));
        if (v == 16) {
            v = w.grid.raw((int) Math.floor(p.x / 16f), (int) Math.floor(feet / 16f) - 1);
        }
        return v;
    }

    @Override
    public void update(World w) {
        Player p = w.player;
        PlayerStep step = p.step();
        float move = p.x - p.prevX;
        if (p.dying() || p.locked() || !p.grounded()) {
            state = State.ARM;
            remember(0, move, step);
            return;
        }
        int value = valueUnder(w, p);
        if (risesRight(value) || fallsRight(value)) {
            boolean upRight = risesRight(value);
            float newMove = upRight ? Math.max(move - SLOPE_PUSH, -PlayerConst.DASH_START)
                    : Math.min(move + SLOPE_PUSH, PlayerConst.DASH_START);
            boolean facingUp = upRight == p.faceRight;
            if (lastValue == ICE && facingUp && step != PlayerStep.DASH) {
                newMove = 0;
            }
            p.slide(newMove - move);
            state = State.ARM;
            remember(value, newMove, step);
            return;
        }
        if (value != ICE) {
            state = State.ARM;
            remember(value, move, step);
            return;
        }
        if (state == State.ARM) {
            arm(p, step, move);
        } else {
            slideStep(p, step, move);
        }
        remember(value, move, step);
    }

    private void remember(int value, float move, PlayerStep step) {
        lastValue = value;
        lastMove = move;
        lastStep = step;
    }

    private void arm(Player p, PlayerStep step, float move) {
        // TODO: the slide starts on a change of the player step; the real arm state reads the step each update (docs/gaps.md, "Ice slide")
        if (step == lastStep) {
            return;
        }
        dashMode = false;
        if (step == PlayerStep.STAND || step == PlayerStep.CROUCH) {
            v = lastMove;
            if (lastStep == PlayerStep.WALK_START) {
                v = Math.signum(lastMove) * 0.5f;
            } else if (lastStep == PlayerStep.WALK) {
                v = Math.signum(lastMove) * 2.0f;
            }
            a = -Math.signum(v) * SLOW;
        } else if (step == PlayerStep.LAND) {
            v = lastMove;
            if (lastStep == PlayerStep.FALCON_DASH) {
                v = Math.signum(lastMove) * 2.0f;
            }
            a = -Math.signum(v) * SLOW;
        } else if (step == PlayerStep.DASH) {
            v = 0;
            timer = (int) (PlayerConst.DASH_START / 0.375f);
            a = (p.faceRight ? 1 : -1) * DASH_GAIN;
            dashMode = true;
        } else {
            return;
        }
        if (v == 0 && !dashMode) {
            return;
        }
        v *= 4f / 3f;
        state = State.SLIDE;
    }

    private void slideStep(Player p, PlayerStep step, float move) {
        if (step == PlayerStep.JUMP || step == PlayerStep.WALL_GRAB) {
            state = State.ARM;
            return;
        }
        if (v != 0 && move != 0 && Math.signum(move) != Math.signum(v) && !dashMode) {
            state = State.ARM;
            return;
        }
        float before = v;
        int f = p.slide(v);
        v += a;
        if ((f & (TerrainMover.LEFT | TerrainMover.RIGHT)) != 0) {
            state = State.ARM;
            return;
        }
        if (dashMode) {
            if (--timer < 0) {
                a = -Math.signum(v) * DASH_RUN_OUT;
                dashMode = false;
            }
        } else if (before != 0 && Math.signum(v) != Math.signum(before)) {
            state = State.ARM;
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

    @Override
    public boolean runsInPause() {
        return false;
    }
}
