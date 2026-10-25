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

public final class WormSpider extends Enemy {

    public enum Step { WAIT, EMERGE, ATTACK, SWAY, RETRACT }

    static final String SET = "worm-spider";
    private static final int[] HEIGHTS = {2, 4, 12, 12, 12, 12, 16, 16, 16, 16, 20, 28, 28, 28, 28, 36, 36, 36, 44,
            44, 44, 44, 46, 48, 48, 48, 48, 48};
    private static final int RANGE = 96;
    private static final float SHOT_SPEED = 5;

    private final boolean ceiling;
    private Step step = Step.WAIT;
    private int sub;
    private int timer;
    private boolean hit;
    private Rect own;

    public WormSpider(World w, RecordDef r) {
        super(w, r, SET);
        ceiling = r.id() == 1;
        // TODO: the water jumper (id 2) is not made; no record of these stages uses it (docs/gaps.md, "Worm Spider")
        y += ceiling ? 32 : -32;
        hp = 16;
        contactDamage = 4;
        damageTable = DamageTables.Table.NORMAL;
        offMarginX = 80;
        offMarginY = 160;
        facePlayer(w);
        anim.play(ceiling ? 14 : 0);
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (step) {
            case WAIT -> {
                if (Math.abs(w.player.x - x) <= RANGE && Math.abs(w.player.y - y) <= RANGE) {
                    startEmerge(w);
                }
            }
            case EMERGE -> updateEmerge(w);
            case ATTACK -> updateAttack(w);
            case SWAY -> {
                timer--;
                if (timer <= 0) {
                    startAttack();
                } else if (hit) {
                    w.sfx.play(2, 6);
                    own = null;
                    anim.play(ceiling ? 23 : 22);
                    step = Step.RETRACT;
                    sub = 0;
                }
            }
            case RETRACT -> {
                if (sub == 0 && anim.entered(1)) {
                    sub = 1;
                    timer = 120;
                } else if (sub == 1 && --timer <= 0) {
                    startEmerge(w);
                }
            }
        }
        hit = false;
        checkOffScreen(w);
    }

    private void startEmerge(World w) {
        w.sfx.play(2, 6);
        step = Step.EMERGE;
        sub = 1;
        own = box(2);
        anim.play(ceiling ? 16 : 2);
        w.add(new Projectile(w, SET, ceiling ? 15 : 1, x, y, 0, 0, 0, 0, 0).throughTerrain().life(56)
                .priority(6).facing(faceRight));
    }

    private Rect box(int h) {
        float top = ceiling ? y - 30 : y + 26 - h;
        return new Rect(x - 4, top, 8, h);
    }

    private void updateEmerge(World w) {
        if (sub == 1) {
            int f = anim.flag();
            if (f >= 1 && f <= 28) {
                own = box(HEIGHTS[f - 1]);
            }
            if (f == 28) {
                own = new Rect(x - 4, y - 26, 8, 52);
                anim.play(ceiling ? 18 : 4);
                sub = 2;
                timer = 30;
            }
            return;
        }
        if (--timer <= 0) {
            startAttack();
        }
    }

    private void startAttack() {
        step = Step.ATTACK;
        anim.play(ceiling ? 21 : 8);
        sub = 1;
        timer = 30;
    }

    private void updateAttack(World w) {
        if (--timer > 0) {
            return;
        }
        if (sub == 1) {
            w.sfx.play(2, 7);
            float sx = ceiling ? x + (faceRight ? -12 : 12) : x + (faceRight ? 12 : -12);
            float sy = ceiling ? y + 30 : y - 30;
            w.add(new Spark(w, sx, sy, faceRight));
            sub = 2;
            timer = 20;
            return;
        }
        step = Step.SWAY;
        anim.play(ceiling ? 19 : 5);
        timer = 90;
    }

    @Override
    public void onHurt(World w, int weaponId) {
        hit = true;
    }

    @Override
    protected void onDeath(World w) {
        w.add(new Explosion(w, x, y));
    }

    @Override
    public Rect hurtBox() {
        return own;
    }

    @Override
    public Rect contactBox() {
        return own;
    }

    @Override
    public int drawPriority() {
        return 7;
    }

    private static final class Spark extends Entity {

        Spark(World w, float x, float y, boolean faceRight) {
            this.x = x;
            this.y = y;
            this.faceRight = faceRight;
            this.anim = new Animator(w.animSet(SET));
            anim.play(9);
        }

        @Override
        public void update(World w) {
            anim.tick();
            if (anim.entered(2)) {
                float[] v = Projectile.aim(x, y, w.player.x, w.player.y, SHOT_SPEED);
                // TODO: the ball has 1 HP for the weapons that can hit shots (+0x9C); those are not modelled, so it cannot be hit (docs/gaps.md, "Shots that hit shots")
                w.add(new Projectile(w, SET, 10, x, y, v[0], v[1], 4, 3, 3).priority(5));
            }
            if (anim.entered(1) && anim.atLastFrame()) {
                dead = true;
            }
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
