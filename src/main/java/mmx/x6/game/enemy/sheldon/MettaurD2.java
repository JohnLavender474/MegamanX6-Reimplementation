package mmx.x6.game.enemy.sheldon;

import java.util.List;
import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.game.player.BusterShot;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;
import mmx.x6.physics.TileRules;

public final class MettaurD2 extends Entity {

    public enum Step { FALL, LAND, WALK, JUMP, SHOOT, GUARD }

    static final String SET = "mettaur-d2";
    private static final float WALK = 0.375f;
    private static final float GRAVITY = 0.2578f;
    private static final int COOLDOWN = 120;
    private static final int[] DEBRIS = {9, 10, 11, 12};

    private final int id;
    private final Body body;
    private Step step = Step.FALL;
    private int timer;
    private float vx;
    private float vy;
    private int zoneCount;
    private int shotCooldown;
    private int guardCooldown;
    private boolean helmet;
    private boolean shootLocked;
    private boolean onFloor;

    public MettaurD2(World w, RecordDef r) {
        this.record = r;
        this.id = r.id();
        this.x = r.x();
        this.y = r.y();
        this.hp = 3;
        this.contactDamage = 1;
        this.damageTable = DamageTables.Table.NORMAL;
        this.faceRight = w.player.x >= x;
        this.body = new Body(x, y, 0, 5, 11, 8);
        this.anim = new Animator(w.animSet(SET));
        anim.play(3);
    }

    public Step step() {
        return step;
    }

    public boolean helmet() {
        return helmet;
    }

    private void go(Step s, int animIndex) {
        step = s;
        timer = 0;
        anim.play(animIndex);
    }

    @Override
    public void update(World w) {
        anim.tick();
        timer++;
        if (shotCooldown > 0) {
            shotCooldown--;
        }
        if (guardCooldown > 0) {
            guardCooldown--;
        }
        if (onFloor && step == Step.WALK) {
            ledgeCheck(w);
        }
        if (onFloor && step != Step.GUARD) {
            shotTrigger(w);
            guardTrigger(w);
        }
        switch (step) {
            case WALK -> {
                vx = faceRight ? WALK : -WALK;
                int f = move(w, vx, 1, true);
                if ((f & (faceRight ? TerrainMover.RIGHT : TerrainMover.LEFT)) != 0) {
                    faceRight = !faceRight;
                }
                if (!onFloor) {
                    go(Step.FALL, 3);
                }
            }
            case FALL -> {
                vy = Math.min(6.5f, vy + GRAVITY);
                move(w, vx, vy, false);
                if (onFloor) {
                    vx = vy = 0;
                    go(Step.LAND, 4);
                }
            }
            case LAND -> {
                if (timer >= 20) {
                    go(Step.WALK, 1);
                }
            }
            case JUMP -> {
                if (vy == 0 && anim.enteredFlag(1)) {
                    vy = -3.5f;
                    vx = faceRight ? 1 : -1;
                }
                if (vy != 0) {
                    vy += GRAVITY;
                    move(w, vx, vy, false);
                    if (vy > 0) {
                        go(Step.FALL, 3);
                    }
                }
            }
            case SHOOT -> {
                if (anim.enteredFlag(1)) {
                    w.sfx.play(2, 46);
                    float sx = x + (faceRight ? 16 : -16);
                    // TODO: the shot can be destroyed by weapons with +0x9C set (charged shots); it is not hittable here (docs/gaps.md, "Mettaur D2")
                    w.add(new Projectile(w, SET, 8, sx, y + 3, faceRight ? 2.5f : -2.5f, 0, 2, 4.5f, 3.5f)
                            .endsOnPlayer().priority(5).facing(faceRight));
                }
                if (timer >= 36) {
                    shotCooldown = COOLDOWN;
                    go(Step.WALK, 1);
                }
            }
            case GUARD -> {
                if (timer == 9) {
                    helmet = true;
                    damageTable = DamageTables.Table.DEFLECT;
                }
                if (timer == 60) {
                    anim.play(6);
                }
                if (timer >= 80) {
                    helmet = false;
                    damageTable = DamageTables.Table.NORMAL;
                    guardCooldown = COOLDOWN;
                    go(Step.WALK, 1);
                }
            }
        }
        if (w.offScreen(x, y, 64, 64)) {
            dead = true;
            w.spawner.released(record);
        }
    }

    private int move(World w, float dx, float dy, boolean grounded) {
        body.x = x;
        body.y = y;
        int f = w.mover.move(body, dx, dy, grounded, List.of());
        x = body.x;
        y = body.y;
        onFloor = (f & TerrainMover.FLOOR) != 0;
        return f;
    }

    private void ledgeCheck(World w) {
        float px = x + (faceRight ? 11 : -11);
        if (!TileRules.isObjectSolid(w.grid.at(px, y + 21))) {
            if (id == 1) {
                faceRight = !faceRight;
            } else {
                go(Step.JUMP, 2);
                vy = 0;
            }
        }
    }

    private void shotTrigger(World w) {
        if (step == Step.SHOOT || shootLocked || shotCooldown > 0) {
            return;
        }
        float dx = w.player.x - x;
        boolean inFront = faceRight ? dx < 80 : dx > -80;
        if (Math.abs(w.player.y - y) < 40 && inFront) {
            if (++zoneCount >= 61) {
                zoneCount = 0;
                go(Step.SHOOT, 7);
            }
        } else {
            zoneCount = 0;
        }
    }

    private void guardTrigger(World w) {
        if (guardCooldown > 0) {
            return;
        }
        boolean playerFront = faceRight ? w.player.x > x : w.player.x < x;
        boolean facesIt = w.player.faceRight == (w.player.x < x);
        if (!playerFront || !facesIt) {
            return;
        }
        for (Entity e : w.entities) {
            if (e instanceof BusterShot s && !s.dead && s.weaponId() == 0 && Math.abs(s.y - y) < 24
                    && Math.abs(s.x - x) < 128) {
                if (step == Step.SHOOT && timer < 20) {
                    shootLocked = true;
                }
                go(Step.GUARD, 5);
                return;
            }
        }
    }

    @Override
    public void onKilled(World w, int weaponId) {
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, SET, DEBRIS, x, y);
        // TODO: the drop roll (table 18) is not made; there are no item drops (docs/gaps.md, "Skipped on purpose")
        dead = true;
        w.spawner.released(record);
    }

    private Rect box(int bx, int by, int bw, int bh) {
        float left = faceRight ? x - bx - bw : x + bx;
        return new Rect(left, y + by, bw, bh);
    }

    @Override
    public Rect hurtBox() {
        return helmet ? box(-12, -4, 21, 18) : box(-12, -6, 21, 20);
    }

    @Override
    public Rect contactBox() {
        return helmet ? box(-12, -4, 21, 18) : box(-11, -16, 21, 29);
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
        return 6;
    }
}
