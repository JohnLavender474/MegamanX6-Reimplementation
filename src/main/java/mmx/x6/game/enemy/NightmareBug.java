package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Explosion;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class NightmareBug extends Entity {

    public enum Step { FOLLOW, LEAVE }

    static final String SET = "nightmare-bug";
    private static final int EFFECT = 1;
    private static final int[] OFFSET_X = {-96, -80, -64, -48, 48, 64, 80, 96};
    private static final int[] OFFSET_Y = {0, 16, 32};
    private static final float SPEED = 5;
    private static final float BOB = 11;

    private final int offX;
    private final int offY;
    private Step step = Step.FOLLOW;
    private int blocks;
    private boolean stopped;
    private float stopY;
    private int bob;

    public NightmareBug(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        this.hp = 8;
        this.contactDamage = 2;
        this.damageTable = DamageTables.Table.NIGHTMARE_BUG;
        this.offX = OFFSET_X[w.rng.nextInt(8)];
        this.offY = OFFSET_Y[w.rng.nextInt(3)];
        this.anim = new Animator(w.animSet(SET));
        anim.play(0);
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        if (w.stage.nightmareEffect() != EFFECT) {
            dead = true;
            w.spawner.kept(record);
            return;
        }
        if (w.player.step() == PlayerStep.GATE_WALK) {
            dead = true;
            w.spawner.kept(record);
            return;
        }
        anim.tick();
        if (step == Step.FOLLOW) {
            follow(w);
        } else {
            y -= SPEED;
        }
        if (w.offScreen(x, y, 64, 64)) {
            dead = true;
            w.spawner.released(record);
        }
    }

    private void follow(World w) {
        faceRight = x < w.player.x;
        float tx = w.player.x + offX;
        float ty = w.player.y + offY;
        if (Math.abs(tx - x) <= 16 && Math.abs(ty - y) <= 32) {
            if (!stopped) {
                stopped = true;
                stopY = y;
                bob = 0;
            }
            if ((w.frame & 1) == 0) {
                bob = (bob + 2) % 64;
                y = stopY + (float) (BOB * (1 - Math.cos(bob * Math.PI * 2 / 64)) / 2);
            }
            return;
        }
        stopped = false;
        double a = Math.atan2(ty - y, tx - x);
        x += (float) (Math.cos(a) * SPEED);
        y += (float) (Math.sin(a) * SPEED);
    }

    @Override
    public void onDeflect(World w, int weaponId) {
        if (blocks++ >= 5) {
            step = Step.LEAVE;
        }
    }

    @Override
    public void onKilled(World w, int weaponId) {
        w.add(new Explosion(w, x, y));
        // TODO: the drop roll (table 18) is not made; there are no item drops (docs/gaps.md, "Skipped on purpose")
        dead = true;
        w.spawner.kept(record);
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 6, y - 6, 11, 11);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public boolean isEnemy() {
        return true;
    }

    @Override
    public String spriteSet() {
        return SET;
    }

    @Override
    public int drawPriority() {
        return 5;
    }
}
