package mmx.x6.game.enemy.turtloid;

import mmx.x6.game.DamageTables;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Enemy;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class SeaAttackerST extends Enemy {

    public enum Step { WAIT, FIRE, REST, CURL, RISE, HOVER, DASH, DROP }

    static final String SET = "sea-attacker-st";
    private static final float BALL_SPEED = 3.75f;

    private Step step;
    private int timer;
    private int faceTimer = 30;
    private int margin;
    private float vx;
    private float vy;
    private boolean ball;

    public SeaAttackerST(World w, RecordDef r) {
        super(w, r, SET);
        hp = 6;
        contactDamage = 4;
        damageTable = DamageTables.Table.NORMAL;
        facePlayer(w);
        if (r.id() == 0) {
            step = Step.WAIT;
            anim.play(0);
        } else {
            step = Step.CURL;
            anim.play(3);
        }
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (step) {
            case WAIT -> {
                if (Math.abs(w.player.x - x) <= 96) {
                    facePlayer(w);
                    anim.play(1);
                    step = Step.FIRE;
                }
            }
            case FIRE -> {
                if (anim.entered(1) && anim.flag() == 1) {
                    w.sfx.play(2, 26);
                    w.add(new Bubble(w, x + (faceRight ? -6 : 6), y - 20));
                }
                if (anim.entered(2)) {
                    anim.play(0);
                    step = Step.REST;
                    timer = 20;
                }
            }
            case REST -> {
                turnTimer(w);
                if (--timer <= 0) {
                    step = Step.WAIT;
                }
            }
            case CURL -> {
                if (anim.entered(1)) {
                    ball = true;
                    vy = -BALL_SPEED;
                    margin = w.rng.nextInt(32);
                    step = Step.RISE;
                }
            }
            case RISE -> {
                if (w.player.y - y >= margin) {
                    facePlayer(w);
                    timer = 40;
                    vy = 0;
                    step = Step.HOVER;
                }
            }
            case HOVER -> {
                turnTimer(w);
                if (--timer <= 0) {
                    facePlayer(w);
                    vx = faceRight ? BALL_SPEED : -BALL_SPEED;
                    step = Step.DASH;
                }
            }
            case DASH -> {
                if (Math.abs(w.player.x - x) <= 1.5f) {
                    vx = 0;
                    vy = BALL_SPEED;
                    step = Step.DROP;
                }
            }
            case DROP -> {
            }
        }
        x += vx;
        y += vy;
        checkOffScreen(w);
    }

    private void turnTimer(World w) {
        if (--faceTimer <= 0) {
            facePlayer(w);
            faceTimer = 30;
        }
    }

    @Override
    protected void onDeath(World w) {
        Debris.spawn(w, SET, new int[] {4, 5, 6, 7, 8}, x, y);
        w.add(new Explosion(w, x, y));
    }

    @Override
    public Rect hurtBox() {
        if (ball) {
            return new Rect(x - 8, y - 8, 16, 16);
        }
        return faceRight ? new Rect(x - 9, y - 16, 11, 30) : new Rect(x - 2, y - 16, 11, 30);
    }

    @Override
    public Rect contactBox() {
        if (ball) {
            return new Rect(x - 8, y - 8, 16, 16);
        }
        return faceRight ? new Rect(x - 9, y - 16, 10, 28) : new Rect(x - 1, y - 16, 10, 28);
    }

    @Override
    public int drawPriority() {
        return 7;
    }

    private static final class Bubble extends Projectile {

        private int part;
        private int wait;

        Bubble(World w, float x, float y) {
            super(w, SET, 2, x, y, 0, -4, 4, 2, 2);
            priority(5);
            // TODO: the bubble has 1 HP for the weapons that can hit shots (+0x9C); those are not modelled, so it cannot be hit (docs/gaps.md, "Shots that hit shots")
        }

        @Override
        public void update(World w) {
            switch (part) {
                case 0 -> {
                    if (y <= w.player.y) {
                        vy = 0;
                        anim.play(9);
                        wait = 40;
                        part = 1;
                    }
                }
                case 1 -> {
                    if (--wait <= 0) {
                        anim.play(2);
                        vx = w.player.x < x ? -4 : 4;
                        part = 2;
                    }
                }
                default -> {
                }
            }
            super.update(w);
        }
    }
}
