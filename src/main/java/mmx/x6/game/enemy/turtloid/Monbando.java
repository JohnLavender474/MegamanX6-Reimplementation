package mmx.x6.game.enemy.turtloid;

import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Enemy;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;
import mmx.x6.physics.TileRules;

public final class Monbando extends Enemy {

    public enum Step { FALL, WAIT, FIRE, RECOVER }

    static final String SET = "monbando";
    private static final float GRAVITY = 0.2578125f;
    private static final int WAIT_UPDATES = 62;

    private final Body body;
    private Step step = Step.FALL;
    private int timer;
    private int kind;
    private float speedY;
    private boolean onFloor;

    public Monbando(World w, RecordDef r) {
        super(w, r, SET);
        hp = 32;
        contactDamage = 6;
        damageTable = DamageTables.Table.DEFLECT;
        body = new Body(x, y, 0, 0, 24, 24);
        facePlayer(w);
        anim.play(0);
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        damageTable = anim.sprite() == 0 ? DamageTables.Table.DEFLECT : DamageTables.Table.NORMAL;
        anim.tick();
        switch (step) {
            case FALL -> {
                fall(w);
                if (onFloor && anim.flag() == 2) {
                    anim.play(1);
                    step = Step.WAIT;
                    timer = 0;
                }
            }
            case WAIT -> {
                if (timer++ >= WAIT_UPDATES - 1) {
                    // TODO: the punch (kind 2) is Zero's; X is the only player, so the roll picks the slam or the missile (docs/gaps.md, "Monbando")
                    kind = w.rng.nextInt(2);
                    anim.play(kind == 0 ? 4 : 3);
                    step = Step.FIRE;
                }
            }
            case FIRE -> {
                if (anim.entered(1)) {
                    fire(w);
                    step = Step.RECOVER;
                }
            }
            case RECOVER -> {
                if (anim.finished() || anim.atLastFrame() && anim.sprite() == 0) {
                    anim.play(1);
                    timer = 0;
                    step = Step.WAIT;
                }
            }
        }
        if (step == Step.WAIT || step == Step.FALL) {
            facePlayer(w);
        }
        checkOffScreen(w);
    }

    private void fall(World w) {
        body.x = x;
        body.y = y;
        speedY = Math.min(6.5f, speedY + GRAVITY);
        int f = w.mover.move(body, 0, speedY, onFloor, w.solids());
        onFloor = (f & TerrainMover.FLOOR) != 0;
        if (onFloor) {
            speedY = 0;
        }
        y = body.y;
    }

    private void fire(World w) {
        float dir = faceRight ? 1 : -1;
        if (kind == 0) {
            // TODO: the slam's Rumble(5) and Quake2(8, 2, 2) screen shake are not done (docs/gaps.md, "Screen shake")
            for (int i = 0; i < 3; i++) {
                w.sfx.play(2, 29);
                w.add(new Wave(w, x + dir * 36, y + 8, faceRight, i * 6));
            }
        } else {
            w.sfx.play(2, 28);
            w.add(new Missile(w, x + dir * 32, y - 16, faceRight));
        }
    }

    @Override
    public void onDeflect(World w, int weaponId) {
        if (step == Step.WAIT) {
            timer = 0;
        }
    }

    @Override
    protected void onDeath(World w) {
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, SET, new int[] {7, 8, 9, 10}, x, y);
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 24, y - 24, 48, 48);
    }

    @Override
    public Rect contactBox() {
        return new Rect(x - 16, y - 24, 32, 48);
    }

    @Override
    public int drawPriority() {
        return 4;
    }

    private static final class Wave extends Entity {

        private static final float SPEED = 2.5f;

        private final Body body;
        private int delay;
        private boolean grounded;
        private int floorUpdates;

        Wave(World w, float x, float y, boolean faceRight, int delay) {
            this.x = x;
            this.y = y;
            this.faceRight = faceRight;
            this.delay = delay;
            this.contactDamage = 4;
            this.body = new Body(x, y, 0, 0, 10, 14);
            this.anim = new mmx.x6.anim.Animator(w.animSet(SET));
            anim.play(5);
        }

        @Override
        public void update(World w) {
            if (delay > 0) {
                delay--;
                return;
            }
            anim.tick();
            body.x = x;
            body.y = y;
            float vx = faceRight ? SPEED : -SPEED;
            int f = w.mover.move(body, vx, floorUpdates >= 2 ? 1 : SPEED, floorUpdates >= 2, w.solids());
            boolean floor = (f & TerrainMover.FLOOR) != 0;
            if ((f & (TerrainMover.LEFT | TerrainMover.RIGHT)) != 0 || grounded && !floor) {
                dead = true;
                return;
            }
            if (floor) {
                floorUpdates++;
                grounded = true;
            }
            x = body.x;
            y = body.y;
            if (w.offScreen(x, y, 32, 32)) {
                dead = true;
            }
        }

        @Override
        public Rect contactBox() {
            return delay > 0 ? null : new Rect(x - 8, y - 4, 16, 16);
        }

        @Override
        public boolean visible() {
            return delay == 0;
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
            return 4;
        }
    }

    private static final class Missile extends Projectile {

        private int timer = 50;

        Missile(World w, float x, float y, boolean right) {
            super(w, SET, 6, x, y, right ? 3 : -3, 0, 4, 16, 4);
            hittable(6, DamageTables.Table.MINOR);
            explodes();
            endsOnPlayer();
            priority(6);
        }

        @Override
        public void update(World w) {
            timer--;
            if (timer >= 0 && timer <= 38) {
                float dy = w.player.y - y;
                vy = Math.abs(dy) > 8 ? Math.signum(dy) * 2 : 0;
            } else {
                vy = 0;
            }
            super.update(w);
            if (!dead && TileRules.isWall(w.grid.at(x + (vx > 0 ? 16 : -16), y))) {
                end(w);
            }
        }
    }
}
