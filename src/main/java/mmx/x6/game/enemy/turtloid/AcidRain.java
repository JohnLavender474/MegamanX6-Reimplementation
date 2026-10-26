package mmx.x6.game.enemy.turtloid;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;

public final class AcidRain extends Entity {

    public enum State { DRY, FADE_IN, RAIN, FADE_OUT }

    private static final int FADE = 30;
    private static final int DRAIN = 180;
    private static final float CURTAIN_ALPHA = 0.5f;

    private State state = State.DRY;
    private int fade;
    private int drain;
    private int frameTimer;
    private boolean curtain;

    public AcidRain(World w, RecordDef r) {
        this.record = r;
        w.layer1Alpha = 0;
        // TODO: the docs do not give the order of the rain layer and layer 0; it is drawn in front of the tiles and the objects (docs/gaps.md, "Acid rain")
        w.layer1Front = true;
    }

    public State state() {
        return state;
    }

    @Override
    public void update(World w) {
        if (curtain && ++frameTimer % 4 == 0) {
            w.camera.setLayer1(0, (frameTimer / 4 % 5) * 256);
        }
        if (w.healHold) {
            drain = 0;
        }
        switch (state) {
            case DRY -> {
                if (w.flag26) {
                    w.sfx.play(5, 4);
                    curtain = true;
                    w.camera.setLayer1(0, 0);
                    drain = 0;
                    state = State.FADE_IN;
                }
            }
            case FADE_IN -> {
                if (!w.flag26) {
                    state = State.FADE_OUT;
                } else if (++fade >= FADE) {
                    fade = FADE;
                    state = State.RAIN;
                }
            }
            case RAIN -> {
                if (!w.flag26) {
                    state = State.FADE_OUT;
                } else if (!w.healHold && w.player.hp > 0 && !w.player.dying() && ++drain >= DRAIN) {
                    drain = 0;
                    if (--w.player.hp <= 0) {
                        w.player.kill();
                    }
                }
            }
            case FADE_OUT -> {
                if (w.flag26) {
                    state = State.FADE_IN;
                } else if (--fade <= 0) {
                    fade = 0;
                    w.sfx.play(5, 3);
                    state = State.DRY;
                }
            }
        }
        // TODO: the rain fades the stage palettes toward their rain colours (0x800ED18C, PaletteDarken); only the curtain's alpha fades here (docs/gaps.md, "Acid rain")
        w.layer1Alpha = CURTAIN_ALPHA * fade / FADE;
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
        return true;
    }
}
