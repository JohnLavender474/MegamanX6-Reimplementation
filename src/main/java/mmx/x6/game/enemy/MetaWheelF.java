package mmx.x6.game.enemy;

import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;

public final class MetaWheelF extends Enemy {

    public enum Step { ROLL, FALL, TURN }

    private static final float ACCEL = 0.046875f;
    private static final float TOP_SPEED = 0.75f;
    private static final float GRAVITY = 0.125f;
    private static final float FALL_MAX = -6.5f;
    private static final int TURN_TIME = 111;

    private final Body body;
    private Step step = Step.FALL;
    private boolean rollRight;
    private float speedX;
    private float speedY;
    private int timer;

    public MetaWheelF(World w, RecordDef r) {
        super(w, r, "meta-wheel-f");
        hp = 32;
        contactDamage = 4;
        damageTable = DamageTables.Table.SABRE_ONLY;
        rollRight = r.id() == 0;
        body = new Body(x, y, 0, 0, 30, 30);
        offMarginX = 128;
        offMarginY = 128;
        anim.play(rollRight ? 2 : 0);
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        body.x = x;
        body.y = y;
        if (step == Step.TURN) {
            if (++timer >= TURN_TIME) {
                int frame = anim.frameIndex();
                rollRight = !rollRight;
                anim.play(rollRight ? 2 : 0, 7 - frame);
                step = Step.FALL;
                speedY = 0;
            }
        } else {
            float sign = rollRight ? 1 : -1;
            speedX = Math.max(-TOP_SPEED, Math.min(TOP_SPEED, speedX + sign * ACCEL));
            int f = w.mover.move(body, speedX, -speedY, step == Step.ROLL, w.solids());
            x = body.x;
            y = body.y;
            if (step == Step.FALL) {
                speedY = Math.max(FALL_MAX, speedY - GRAVITY);
            }
            int ahead = rollRight ? TerrainMover.RIGHT : TerrainMover.LEFT;
            if ((f & ahead) != 0 || wheelAhead(w)) {
                startTurn(w);
            } else if (step == Step.ROLL && (f & TerrainMover.FLOOR) == 0) {
                step = Step.FALL;
                speedY = 0;
            } else if (step == Step.FALL && (f & TerrainMover.FLOOR) != 0) {
                w.sfx.play(2, 4);
                step = Step.ROLL;
                speedY = 0;
            }
            anim.tick();
        }
        checkOffScreen(w);
    }

    private void startTurn(World w) {
        w.sfx.play(2, 4);
        x = Math.round(x);
        speedX = 0;
        step = Step.TURN;
        timer = 0;
    }

    private boolean wheelAhead(World w) {
        Rect mine = body.box();
        for (Entity e : w.entities) {
            if (e != this && !e.dead && e instanceof MetaWheelF other && other.body.box().overlaps(mine)) {
                boolean otherIsAhead = rollRight ? other.x > x : other.x < x;
                if (otherIsAhead) {
                    return true;
                }
            }
        }
        return false;
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 23, y - 23, 46, 46);
    }

    @Override
    public Rect contactBox() {
        return new Rect(x - 20, y - 20, 40, 40);
    }

    @Override
    public int drawPriority() {
        return 6;
    }

    @Override
    protected void onDeath(World w) {
        Explosion.around(w, x, y, 5, 16);
        Debris.spawn(w, "meta-wheel-f", new int[] {8, 9, 10}, x, y);
    }
}
