package mmx.x6.game.enemy.sheldon;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class Guardian extends Entity {

    public enum Step { WAIT, TURN, ATTACK, DYING }

    private static final int[] SHOTS = {68, 89, 110, 131};
    private static final int LOWER = 141;
    private static final int END = 35;
    private static final int[] DEBRIS = {8, 9, 10, 11};

    private final boolean death;
    private final String set;
    private Body body;
    private Step step = Step.WAIT;
    private int timer;
    private boolean open;
    private boolean stopped;
    private int lowerAt;

    public Guardian(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        this.death = r.id() >= 16;
        this.set = death ? "death-guardian" : "guardian";
        this.faceRight = (r.id() & 15) != 0;
        this.hp = 6;
        this.contactDamage = death ? 7 : 6;
        this.damageTable = DamageTables.Table.DEFLECT;
        this.anim = new Animator(w.animSet(set));
        anim.play(0);
    }

    public Step step() {
        return step;
    }

    public boolean shieldOpen() {
        return open;
    }

    private boolean playerBehind(World w) {
        return faceRight ? w.player.x <= x : w.player.x > x;
    }

    private void go(Step s) {
        step = s;
        timer = 0;
    }

    @Override
    public void update(World w) {
        if (body == null && step != Step.DYING) {
            body = new Body(this);
            w.add(body);
        }
        anim.tick();
        timer++;
        switch (step) {
            case WAIT -> {
                if (timer == 1) {
                    anim.play(0);
                }
                if ((timer == 25 || timer == 57 || timer == 89) && playerBehind(w)) {
                    startTurn();
                } else if (timer > 120 && w.player.x - x < 188) {
                    if (playerBehind(w)) {
                        startTurn();
                    } else {
                        startAttack();
                    }
                }
            }
            case TURN -> {
                if (anim.enteredFlag(1)) {
                    faceRight = !faceRight;
                    anim.play(1);
                }
                if (timer >= 31) {
                    startAttack();
                }
            }
            case ATTACK -> attack(w);
            case DYING -> {
                if (timer % 6 == 0) {
                    w.add(new Explosion(w, x + w.rng.nextInt(33) - 16, y + w.rng.nextInt(33) - 16));
                }
                if (timer >= 32) {
                    dead = true;
                    w.spawner.released(record);
                }
                return;
            }
        }
        if (w.offScreen(x, y, 96, 96)) {
            dead = true;
            if (body != null) {
                body.dead = true;
            }
            w.spawner.released(record);
        }
    }

    private void startTurn() {
        go(Step.TURN);
        anim.play(2);
    }

    private void startAttack() {
        go(Step.ATTACK);
        anim.play(3);
        stopped = false;
        lowerAt = LOWER;
    }

    private void attack(World w) {
        if (anim.anim() == 3 && anim.enteredFlag(2)) {
            open = true;
        }
        if (anim.anim() == 12 && anim.enteredFlag(2)) {
            open = false;
        }
        if (!stopped) {
            for (int t : SHOTS) {
                if (timer == t) {
                    shoot(w);
                }
            }
        }
        if (timer == lowerAt) {
            anim.play(12);
        }
        if (timer >= lowerAt + END) {
            open = false;
            go(Step.WAIT);
        }
    }

    private void shoot(World w) {
        float ox = x + (faceRight ? 6 : -6);
        double a = Math.toDegrees(Math.atan2(-(w.player.y - y), w.player.x - ox));
        int d32 = Math.floorMod((int) Math.round(a / 11.25), 32);
        boolean front = faceRight ? d32 >= 24 || d32 <= 8 : d32 >= 8 && d32 <= 24;
        if (!front) {
            stopped = true;
            lowerAt = Math.min(lowerAt, timer + 10);
            return;
        }
        double rad = Math.toRadians(d32 * 11.25);
        float vx = (float) (Math.cos(rad) * 3);
        float vy = (float) (-Math.sin(rad) * 3);
        w.sfx.play(2, 79);
        anim.play(4);
        // TODO: the shot can be destroyed by weapons with +0x9C set (charged shots); it is not hittable here (docs/gaps.md, "Guardian")
        w.add(new Projectile(w, set, 5, ox, y, vx, vy, 3, 3, 2).endsOnPlayer().priority(0).facing(faceRight));
    }

    private Rect box(int bx, int by, int bw, int bh) {
        float left = faceRight ? x - bx - bw : x + bx;
        return new Rect(left, y + by, bw, bh);
    }

    @Override
    public Rect hurtBox() {
        if (step == Step.DYING) {
            return null;
        }
        return open ? box(-31, -32, 62, 20) : box(-24, -27, 17, 54);
    }

    @Override
    public Rect contactBox() {
        if (step == Step.DYING) {
            return null;
        }
        if (death) {
            return open ? box(-42, -33, 80, 17) : box(-24, -27, 17, 54);
        }
        return open ? box(-21, -26, 42, 16) : box(-17, -16, 10, 34);
    }

    void bodyKilled(World w) {
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, set, DEBRIS, x, y);
        // TODO: the drop roll (table 20) is not made; there are no item drops (docs/gaps.md, "Skipped on purpose")
        go(Step.DYING);
        damageTable = null;
        contactDamage = 0;
    }

    @Override
    public boolean visible() {
        return step != Step.DYING;
    }

    @Override
    public boolean isEnemy() {
        return step != Step.DYING;
    }

    @Override
    public String spriteSet() {
        return set;
    }

    @Override
    public int drawPriority() {
        return 6;
    }

    static final class Body extends Entity {

        private final Guardian g;

        Body(Guardian g) {
            this.g = g;
            this.hp = 6;
            this.contactDamage = g.contactDamage;
            this.damageTable = DamageTables.Table.NORMAL;
        }

        @Override
        public void update(World w) {
            x = g.x;
            y = g.y;
            if (g.dead) {
                dead = true;
            }
        }

        @Override
        public Rect hurtBox() {
            if (g.step == Step.DYING) {
                return null;
            }
            return g.death ? g.box(-15, -7, 40, 16) : g.box(-8, -7, 34, 16);
        }

        @Override
        public Rect contactBox() {
            if (g.step == Step.DYING) {
                return null;
            }
            return g.death ? g.box(-15, -2, 39, 6) : g.box(-8, -2, 33, 6);
        }

        @Override
        public void onKilled(World w, int weaponId) {
            dead = true;
            g.bodyKilled(w);
        }

        @Override
        public boolean visible() {
            return false;
        }

        @Override
        public boolean isEnemy() {
            return true;
        }
    }
}
