package mmx.x6.game.enemy.yammark;

import mmx.x6.game.DamageTables;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Enemy;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;
import mmx.x6.physics.TileRules;

public final class Caterpirade extends Enemy implements Solid {

    public enum Step { WALK, CURL, FALL, TURN, BALL }

    private static final float GRAVITY = 0.125f;
    private static final float FALL_MAX = 6.5f;
    private static final float CRAWL = 0.5f;
    private static final float ROLL = 1;
    private static final float ROLL_DECEL = 0.0078125f;

    private final Body body;
    private Step step = Step.FALL;
    private int sub;
    private int timer;
    private float speedX;
    private float speedY;
    private boolean onFloor;

    public Caterpirade(World w, RecordDef r) {
        super(w, r, "caterpirade");
        hp = 12;
        contactDamage = 4;
        damageTable = DamageTables.Table.NORMAL;
        body = new Body(x, y, 0, 0, 14, 14);
        anim.play(0);
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        switch (step) {
            case WALK -> {
                anim.tick();
                move(w);
                float probeX = x + (faceRight ? 14 : -14);
                boolean ledge = !TileRules.isObjectSolid(w.grid.at(probeX, y + 22)) && !TileRules.isSlope(w.grid.at(probeX, y + 22));
                boolean wall = w.mover.blocked(body, faceRight ? TerrainMover.RIGHT : TerrainMover.LEFT, w.solids());
                if (!onFloor) {
                    step = Step.FALL;
                } else if (ledge || wall) {
                    step = Step.TURN;
                    sub = 0;
                    timer = 40;
                }
            }
            case TURN -> {
                if (sub == 0) {
                    if (--timer <= 0) {
                        anim.play(1);
                        sub = 1;
                    }
                } else {
                    anim.tick();
                    if (anim.entered(1)) {
                        anim.play(0);
                        faceRight = !faceRight;
                        speedX = faceRight ? CRAWL : -CRAWL;
                        step = Step.WALK;
                    }
                }
            }
            case FALL -> {
                move(w);
                if (onFloor) {
                    facePlayer(w);
                    speedX = faceRight ? CRAWL : -CRAWL;
                    step = Step.WALK;
                }
            }
            case CURL -> {
                anim.tick();
                speedX = 0;
                move(w);
                if (sub == 0 && anim.entered(1)) {
                    sub = 1;
                    timer = 40;
                } else if (sub == 1 && --timer <= 0) {
                    anim.play(3);
                    facePlayer(w);
                    speedX = faceRight ? ROLL : -ROLL;
                    step = Step.BALL;
                    sub = 0;
                    timer = 128;
                }
            }
            case BALL -> updateBall(w);
        }
        checkOffScreen(w);
    }

    private void updateBall(World w) {
        anim.tick();
        move(w);
        switch (sub) {
            case 0 -> {
                if (--timer <= 0) {
                    anim.play(5);
                    sub = 1;
                    timer = 90;
                }
            }
            case 1 -> {
                speedX -= Math.signum(speedX) * ROLL_DECEL;
                if (--timer <= 0) {
                    speedX = 0;
                    sub = 2;
                }
            }
            case 2 -> {
                if (anim.sprite() == 23 || anim.sprite() == 16) {
                    w.sfx.play(2, 9);
                    damageTable = DamageTables.Table.NORMAL;
                    anim.play(4);
                    sub = 3;
                }
            }
            default -> {
                if (anim.entered(1)) {
                    anim.play(0);
                    step = Step.FALL;
                }
            }
        }
    }

    private void move(World w) {
        body.x = x;
        body.y = y;
        speedY = Math.min(FALL_MAX, speedY + GRAVITY);
        int f = w.mover.move(body, speedX, speedY, onFloor, w.solids());
        onFloor = (f & TerrainMover.FLOOR) != 0;
        if (onFloor) {
            speedY = 0;
        }
        if ((f & (TerrainMover.LEFT | TerrainMover.RIGHT)) != 0 && step == Step.BALL) {
            speedX = 0;
        }
        x = body.x;
        y = body.y;
    }

    private boolean ball() {
        return step == Step.CURL || step == Step.BALL && sub < 3;
    }

    @Override
    public void onHurt(World w, int weaponId) {
        w.sfx.play(2, 8);
        speedX = 0;
        step = Step.CURL;
        sub = 0;
        anim.play(2);
        damageTable = DamageTables.Table.DEFLECT;
    }

    @Override
    protected void onDeath(World w) {
        w.add(new Explosion(w, x, y));
    }

    @Override
    public Rect solidRect() {
        return ball() || dead ? null : new Rect(x - 14, y - 14, 28, 28);
    }

    @Override
    public boolean carries() {
        return true;
    }

    @Override
    public Rect hurtBox() {
        return ball() ? new Rect(x - 8, y - 8, 16, 18) : new Rect(x - 12, y - 8, 24, 18);
    }

    @Override
    public Rect contactBox() {
        return ball() ? new Rect(x - 10, y - 8, 20, 16) : new Rect(x - 18, y - 7, 36, 20);
    }

    @Override
    public int drawPriority() {
        return 7;
    }
}
