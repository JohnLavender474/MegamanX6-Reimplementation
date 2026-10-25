package mmx.x6.game.enemy.heatnix;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class FlameJet extends Entity {

    public enum Step { FIRE, PAUSE }

    private static final int[] HEIGHT = {16, 40, 56, 72, 88, 88, 88, 88, 72, 56, 24, 8, 8, 0};
    private static final int LONG_HOLD = 180;
    private static final int SHORT_HOLD = 14;

    private final boolean up;
    private final int bursts;
    private Step step;
    private int sub;
    private int timer;
    private int left;

    public FlameJet(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        this.up = (r.id() & 0xF0) == 0;
        this.bursts = r.id() & 0x0F;
        this.contactDamage = 16;
        this.anim = new Animator(w.animSet("flame-jet"));
        startFire(w);
    }

    public Step step() {
        return step;
    }

    private int hold() {
        return bursts == 0 ? LONG_HOLD : SHORT_HOLD;
    }

    private void startFire(World w) {
        step = Step.FIRE;
        left = bursts == 0 ? 1 : bursts >= 8 ? w.rng.nextInt(4) + 1 : bursts;
        flame(w);
    }

    private void flame(World w) {
        w.sfx.play(2, 22);
        anim.play(up ? 0 : 2);
        timer = hold();
        sub = 1;
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (step) {
            case FIRE -> {
                if (sub == 1 && anim.entered(1)) {
                    sub = 2;
                } else if (sub == 2 && --timer <= 0) {
                    anim.play(up ? 1 : 3);
                    sub = 3;
                } else if (sub == 3 && anim.entered(1)) {
                    if (--left <= 0) {
                        anim.play(4);
                        step = Step.PAUSE;
                        timer = 60;
                    } else {
                        flame(w);
                    }
                }
            }
            case PAUSE -> {
                if (--timer <= 0) {
                    startFire(w);
                }
            }
        }
        if (w.offScreen(x, y, 160, 160)) {
            dead = true;
            w.spawner.released(record);
        }
    }

    @Override
    public Rect contactBox() {
        int s = anim.sprite();
        int h = s < 0 ? 0 : HEIGHT[s % 14];
        if (h == 0 || step == Step.PAUSE) {
            return null;
        }
        return up ? new Rect(x - 8, y - h, 16, h) : new Rect(x - 8, y, 16, h);
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public String spriteSet() {
        return "flame-jet";
    }

    @Override
    public int drawPriority() {
        return 7;
    }
}
