package mmx.x6.game.enemy;

import mmx.x6.game.DamageTables;
import mmx.x6.game.World;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;

public final class Junkroid extends Enemy {

    private enum Step { IDLE, SHOOT, FALL }

    private static final float GRAVITY = 0.2578125f;
    private static final float FALL_MAX = -6.5f;
    private static final int RANGE_ENTER = 112;
    private static final int RANGE_LEAVE = 113;
    private static final int SHOT_INTERVAL = 120;

    private final Body body;
    private final boolean active;
    private Step step = Step.IDLE;
    private int shotTimer = 1;
    private float speedY;

    public Junkroid(World w, RecordDef r) {
        super(w, r, "junkroid");
        hp = 12;
        contactDamage = 4;
        damageTable = DamageTables.Table.NORMAL;
        active = r.id() != 0;
        body = new Body(x, y, 0, 16, 16, 8);
        facePlayer(w);
        anim.play(0);
    }

    @Override
    public void update(World w) {
        switch (step) {
            case IDLE -> {
                if (active && Math.abs(w.player.x - x) < RANGE_ENTER) {
                    step = Step.SHOOT;
                }
            }
            case SHOOT -> updateShoot(w);
            case FALL -> {
                body.x = x;
                body.y = y;
                int f = w.mover.move(body, 0, -speedY, false, w.solids());
                speedY = Math.max(FALL_MAX, speedY - GRAVITY);
                y = body.y;
                if ((f & TerrainMover.FLOOR) != 0) {
                    step = Step.IDLE;
                    speedY = 0;
                }
            }
        }
        if (active && anim.sprite() == 0) {
            facePlayer(w);
        }
        body.x = x;
        body.y = y;
        if (step != Step.FALL && !w.mover.onFloor(body, w.solids())) {
            step = Step.FALL;
            speedY = 0;
        }
        checkOffScreen(w);
    }

    private void updateShoot(World w) {
        if (Math.abs(w.player.x - x) >= RANGE_LEAVE) {
            anim.play(0);
            shotTimer = SHOT_INTERVAL;
            step = Step.IDLE;
            return;
        }
        if (shotTimer != 0 && --shotTimer == 0) {
            anim.play(1);
        }
        if (anim.anim() == 1 && anim.entered(1)) {
            w.sfx.play(2, 26);
            w.add(new JunkroidShot(w, x + (faceRight ? 16 : -16), y - 12, faceRight));
        }
        if (anim.anim() == 1 && anim.entered(2)) {
            anim.play(0);
            shotTimer = SHOT_INTERVAL;
            step = Step.IDLE;
            return;
        }
        anim.tick();
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 12, y - 20, 24, 40);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public int drawPriority() {
        return 6;
    }

    @Override
    protected void onDeath(World w) {
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, "junkroid", new int[] {2, 3, 4, 5}, x, y);
    }
}
