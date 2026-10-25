package mmx.x6.game.enemy.yammark;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Enemy;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class KillerMoth extends Enemy {

    public enum Step { HOVER, SWOOP, CIRCLE }

    private static final String SET = "killer-moth";
    private static final int[] FLAME_ANIMS = {-1, 1, 3, 5};
    private static final double TURN = Math.PI * 2 * 32 / 4096;

    private Step step = Step.HOVER;
    private int sub;
    private int timer;
    private int flameMode = 1;
    private float cx;
    private float cy;
    private double angle;
    private double turn;
    private int shots;

    public KillerMoth(World w, RecordDef r) {
        super(w, r, SET);
        // TODO: the ENTER variant (id not 0) has no record in these stages and is not made (docs/gaps.md, "Killer Moth")
        hp = 8;
        contactDamage = 4;
        damageTable = DamageTables.Table.NORMAL;
        offMarginX = 80;
        offMarginY = 160;
        facePlayer(w);
        startHover();
        w.add(new Flame(w, this));
    }

    public Step step() {
        return step;
    }

    private void startHover() {
        step = Step.HOVER;
        anim.play(0);
        flameMode = 1;
        timer = 180;
        sub = 1;
    }

    @Override
    public void update(World w) {
        facePlayer(w);
        anim.tick();
        switch (step) {
            case HOVER -> {
                if (sub == 1) {
                    timer--;
                    if (timer <= 0 || Math.abs(w.player.x - x) < 48) {
                        anim.play(2);
                        flameMode = 2;
                        sub = 2;
                    }
                } else if (anim.entered(1)) {
                    step = Step.SWOOP;
                    anim.play(4);
                    flameMode = 3;
                }
            }
            case SWOOP -> {
                float tx = w.player.x;
                float ty = w.player.y - 48;
                if (Math.abs(tx - x) < 3 && Math.abs(ty - y) < 3) {
                    x = tx;
                    y = ty;
                    startCircle();
                    break;
                }
                double a = Math.atan2(ty - y, tx - x);
                x += (float) (Math.cos(a) * 2.5);
                y += (float) (Math.sin(a) * 1.25);
            }
            case CIRCLE -> {
                angle += turn;
                x = cx + (float) (Math.cos(angle) * 16);
                y = cy - (float) (Math.sin(angle) * 16);
                if (--timer <= 0) {
                    w.sfx.play(2, 5);
                    w.add(new Projectile(w, SET, 7, x, y, 0, 0.25f, 4, 6, 6).priority(4).margin(32));
                    timer = 20;
                    if (--shots <= 0) {
                        startHover();
                    }
                }
            }
        }
        checkOffScreen(w);
    }

    private void startCircle() {
        step = Step.CIRCLE;
        cx = x;
        cy = y - 16;
        angle = Math.PI * 1.5;
        turn = faceRight ? TURN : -TURN;
        anim.play(6);
        flameMode = 1;
        timer = 20;
        shots = 8;
    }

    @Override
    protected void onDeath(World w) {
        w.add(new Explosion(w, x, y));
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 8, y - 8, 16, 16);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public int drawPriority() {
        return 5;
    }

    private static final class Flame extends Entity {

        private final KillerMoth moth;
        private int mode = -1;

        Flame(World w, KillerMoth moth) {
            this.moth = moth;
            this.anim = new Animator(w.animSet(SET));
        }

        @Override
        public void update(World w) {
            if (moth.dead) {
                dead = true;
                return;
            }
            x = moth.x;
            y = moth.y;
            faceRight = moth.faceRight;
            if (moth.flameMode != mode) {
                mode = moth.flameMode;
                if (FLAME_ANIMS[mode] >= 0) {
                    anim.play(FLAME_ANIMS[mode]);
                }
            }
            anim.tick();
        }

        @Override
        public boolean visible() {
            return mode > 0;
        }

        @Override
        public boolean clearedByGate() {
            return true;
        }

        @Override
        public String spriteSet() {
            return SET;
        }

        @Override
        public int drawPriority() {
            return 6;
        }
    }
}
