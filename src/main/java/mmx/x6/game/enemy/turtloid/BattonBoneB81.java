package mmx.x6.game.enemy.turtloid;

import mmx.x6.game.DamageTables;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Enemy;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class BattonBoneB81 extends Enemy {

    public enum Step { CHASE, DROP, FLY_AWAY, STRAIGHT, SCATTER, HOMING, RISE }

    static final String SET = "batton-bone-b81";

    private final int id;
    private Step step;
    private int sub;
    private int timer;
    private boolean bomb;
    private float vx;
    private float vy;
    private float ay;

    public BattonBoneB81(World w, RecordDef r) {
        super(w, r, SET);
        id = r.id();
        hp = 3;
        contactDamage = 3;
        damageTable = DamageTables.Table.NORMAL;
        offMarginX = 32;
        offMarginY = 32;
        // TODO: the set-up leaves flip at 0, so a bat from a record faces left; how the stage places the straight and scatter bats was not checked (docs/gaps.md, "Batton Bone B81")
        faceRight = false;
        switch (id) {
            case 0 -> {
                bomb = true;
                anim.play(0);
                step = Step.CHASE;
                timer = 200;
            }
            case 1 -> {
                anim.play(3);
                step = Step.STRAIGHT;
                vx = -3;
            }
            case 4 -> {
                anim.play(3);
                step = Step.HOMING;
                timer = 300;
            }
            default -> {
                anim.play(3);
                step = Step.SCATTER;
                vx = (faceRight ? 3.5f : -3.5f) + (w.rng.nextBoolean() ? 1 : -1) * w.rng.nextFloat();
                vy = -1.5f;
                ay = -0.00390625f;
                timer = id == 2 ? w.rng.nextInt(64) : 0;
            }
        }
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        switch (step) {
            case CHASE -> {
                faceRight = w.player.x > x;
                anim.tick();
                x += vx;
                y += vy;
                float[] v = Projectile.aim(x, y, w.player.x, w.player.y - 80, 1);
                vx = v[0] * 1.5f;
                vy = v[1];
                if (Math.abs(w.player.x - x) <= Math.abs(vx) + 2 || --timer <= 0) {
                    step = Step.DROP;
                    sub = 0;
                }
            }
            case DROP -> {
                if (sub == 0) {
                    anim.play(1);
                    vx = vy = 0;
                    sub = 1;
                    timer = 5;
                    return;
                }
                anim.tick();
                if (--timer > 0) {
                    break;
                }
                if (sub == 1) {
                    dropBomb(w);
                    sub = 2;
                    timer = 13;
                } else {
                    anim.play(3);
                    vx = faceRight ? 3 : -3;
                    vy = -0.3125f;
                    step = Step.FLY_AWAY;
                }
            }
            case FLY_AWAY, STRAIGHT, RISE -> {
                anim.tick();
                x += vx;
                y += vy;
            }
            case SCATTER -> {
                if (timer > 0) {
                    timer--;
                    break;
                }
                anim.tick();
                x += vx;
                y += vy;
                vy += ay;
            }
            case HOMING -> {
                faceRight = w.player.x > x;
                anim.tick();
                if (++sub % 4 == 0) {
                    float[] v = Projectile.aim(x, y, w.player.x, w.player.y, 1);
                    vx = v[0];
                    vy = v[1];
                }
                x += vx;
                y += vy;
                if (--timer <= 0) {
                    rise();
                }
            }
        }
        checkOffScreen(w);
    }

    private void rise() {
        step = Step.RISE;
        vx = 0;
        vy = -2;
    }

    private void dropBomb(World w) {
        bomb = false;
        w.add(new Projectile(w, SET, 2, x - 2, y + 22, 0, 1, 5, 7, 14)
                .hittable(1, DamageTables.Table.MINOR).gravity(0.1875f, 6.5f).explodes().endsOnPlayer()
                .priority(6));
    }

    @Override
    public void onTouchPlayer(World w) {
        if (step == Step.HOMING) {
            rise();
        }
    }

    @Override
    protected void onDeath(World w) {
        if (bomb) {
            dropBomb(w);
        }
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, SET, new int[] {4, 5, 6, 7}, x, y);
    }

    @Override
    public Rect hurtBox() {
        return bomb ? new Rect(x - 11, y - 10, 20, 42) : new Rect(x - 11, y - 10, 21, 23);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public int drawPriority() {
        return 6;
    }
}
