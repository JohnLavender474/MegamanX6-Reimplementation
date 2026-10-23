package mmx.x6.game.fx;

import mmx.x6.game.Entity;
import mmx.x6.game.World;

public final class BossExplosion extends Entity {

    public static final int LONG_HOLD = 0x80;

    private static final int WHITE_MAX = 4095;
    private static final int WHITE_STEP = 25;
    private static final int BURST_UPDATES = 9;
    private static final int BURST_EXPLOSIONS = 8;
    private static final int RAIN_UPDATES = 100;
    private static final int RAIN_TAIL_UPDATES = 10;
    private static final int RAIN_EVERY = 4;
    private static final int RAIN_SPREAD = 96;
    private static final int LONG_HOLD_UPDATES = 201;
    private static final int SHORT_HOLD_UPDATES = 41;

    private final int flags;
    private int state = 1;
    private int timer;
    private int white;
    private boolean signalled;

    public BossExplosion(float x, float y, int flags) {
        this.x = x;
        this.y = y;
        this.flags = flags;
        // TODO: the rays (ray types 4, 6 and 8) have no sprite data and are not drawn (docs/gaps.md, "Boss explosion")
    }

    public boolean white() {
        return signalled;
    }

    public int fadeUpdates() {
        return state >= 4 && !signalled ? timer : 0;
    }

    @Override
    public void update(World w) {
        timer++;
        switch (state) {
            case 1 -> {
                if (timer <= BURST_EXPLOSIONS) {
                    w.add(new Explosion(w, x, y));
                }
                if (timer == BURST_UPDATES) {
                    if (flags != 1) {
                        w.music.stop();
                    }
                    next(2);
                }
            }
            case 2 -> {
                rain(w);
                if (timer == RAIN_UPDATES) {
                    next(3);
                }
            }
            case 3 -> {
                rain(w);
                if (timer == RAIN_TAIL_UPDATES) {
                    next(4);
                }
            }
            case 4 -> {
                rain(w);
                setWhite(w, white + WHITE_STEP);
                if (white == WHITE_MAX) {
                    signalled = true;
                    w.bossBarShown = false;
                    w.sfx.play(0, 5);
                    next(5);
                }
            }
            case 5 -> {
                if (timer > ((flags & LONG_HOLD) != 0 ? LONG_HOLD_UPDATES : SHORT_HOLD_UPDATES)) {
                    setWhite(w, white - WHITE_STEP);
                    next(6);
                }
            }
            case 6 -> {
                setWhite(w, white - WHITE_STEP);
                if (white == 0) {
                    dead = true;
                }
            }
            default -> {
            }
        }
    }

    private void next(int s) {
        state = s;
        timer = 0;
    }

    private void rain(World w) {
        if (timer % RAIN_EVERY == 0) {
            float dx = w.rng.nextInt(2 * RAIN_SPREAD + 1) - RAIN_SPREAD;
            float dy = w.rng.nextInt(2 * RAIN_SPREAD + 1) - RAIN_SPREAD;
            w.add(new Explosion(w, x + dx, y + dy));
        }
    }

    private void setWhite(World w, int level) {
        white = Math.max(0, Math.min(WHITE_MAX, level));
        w.whiteLevel = white / (float) WHITE_MAX;
    }

    @Override
    public boolean visible() {
        return false;
    }
}
