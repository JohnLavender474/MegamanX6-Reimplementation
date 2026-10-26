package mmx.x6.game.enemy.scaravich;

import java.util.List;
import mmx.x6.anim.Animator;
import mmx.x6.anim.BankSheetAnims;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;

public final class FallingRock extends Entity {

    public enum Step { APPEAR, DROP, BOULDER_FALL, BOULDER_ROLL, AIM, FLY }

    static final String SET = BankSheetAnims.SCARAVICH_ROCKS;
    private static final float GRAVITY = 0.125f;
    private static final float FALL_MAX = 2;
    private static final float ROLL = 1.5f;

    private final int kind;
    private final Body body;
    private Step step = Step.APPEAR;
    private int timer;
    private int landings;
    private int wallTimer;
    private float vx;
    private float vy;
    private float gravity;

    public FallingRock(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        this.kind = r.id() > 0 ? r.id() : w.rng.nextInt(3);
        this.hp = 8;
        this.contactDamage = 2;
        this.damageTable = DamageTables.Table.NORMAL;
        this.body = new Body(x, y, 0, 0, 13, 13);
        this.anim = new Animator(w.animSet(SET));
        anim.play(kind == 1 ? 1 : 0);
    }

    public static FallingRock spawn(World w, RecordDef r) {
        if (!w.rng.nextBoolean()) {
            w.spawner.kept(r);
            return null;
        }
        return new FallingRock(w, r);
    }

    public int kind() {
        return kind;
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        if (step != Step.APPEAR && step != Step.AIM) {
            anim.tick();
        }
        switch (step) {
            case APPEAR -> {
                if (++timer > 60) {
                    timer = 0;
                    gravity = GRAVITY;
                    step = kind == 0 ? Step.DROP : kind == 1 ? Step.BOULDER_FALL : Step.AIM;
                }
            }
            case DROP -> {
                if ((move(w) & TerrainMover.FLOOR) != 0) {
                    w.sfx.play(2, 82);
                    breakUp(w);
                }
            }
            case BOULDER_FALL -> {
                if ((move(w) & TerrainMover.FLOOR) != 0) {
                    land(w);
                }
            }
            case BOULDER_ROLL -> roll(w);
            case AIM -> {
                if (++timer >= 20) {
                    float[] v = Projectile.aim(x, y, w.player.x, w.player.y, 2);
                    vx = v[0];
                    vy = v[1];
                    gravity = 0;
                    step = Step.FLY;
                }
            }
            case FLY -> {
                if (move(w) != 0) {
                    breakUp(w);
                }
            }
        }
        if (!dead && w.offScreen(x, y, 48, 48)) {
            dead = true;
            w.spawner.released(record);
        }
    }

    private int move(World w) {
        if (gravity > 0) {
            vy = Math.min(FALL_MAX, vy + gravity);
        }
        body.x = x;
        body.y = y;
        int f = w.mover.move(body, vx, vy, false, List.of());
        x = body.x;
        y = body.y;
        return f;
    }

    private void land(World w) {
        w.sfx.play(2, 82);
        vy = 0;
        wallTimer = 0;
        if (++landings >= 3) {
            breakUp(w);
            return;
        }
        int v = w.grid.at(x, y + 24);
        if (v >= 1 && v <= 4 || v == 9 || v == 10 || v >= 17 && v <= 20 || v == 25 || v == 26) {
            vx = -ROLL;
        } else if (v >= 5 && v <= 8 || v == 11 || v == 12 || v >= 21 && v <= 24 || v == 27 || v == 28) {
            vx = ROLL;
        } else {
            vx = w.player.x >= x ? ROLL : -ROLL;
        }
        faceRight = vx > 0;
        step = Step.BOULDER_ROLL;
    }

    private void roll(World w) {
        if ((w.frame & 31) == 0) {
            w.sfx.play(2, 83);
        }
        int f = move(w);
        if ((f & TerrainMover.FLOOR) == 0) {
            step = Step.BOULDER_FALL;
            return;
        }
        vy = 0;
        if ((f & (TerrainMover.LEFT | TerrainMover.RIGHT)) != 0 && ++wallTimer >= 60) {
            breakUp(w);
        }
    }

    private void breakUp(World w) {
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, SET, debris(w), x, y);
        w.sfx.play(2, 84);
        dead = true;
        w.spawner.released(record);
    }

    private int[] debris(World w) {
        boolean first = w.rng.nextBoolean();
        if (kind == 1) {
            return first ? new int[] {6, 7} : new int[] {8, 9};
        }
        return first ? new int[] {2, 3} : new int[] {4, 5};
    }

    @Override
    public void onKilled(World w, int weaponId) {
        // TODO: the item roll DropItem(table 18) is not made; there are no item drops (docs/gaps.md, "Skipped on purpose")
        breakUp(w);
    }

    @Override
    public Rect hurtBox() {
        return step == Step.APPEAR ? null : new Rect(x - 13, y - 13, 26, 26);
    }

    @Override
    public Rect contactBox() {
        return step == Step.APPEAR ? null : new Rect(x - 11, y - 11, 22, 22);
    }

    @Override
    public boolean visible() {
        return step != Step.APPEAR || (timer & 1) == 0;
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
