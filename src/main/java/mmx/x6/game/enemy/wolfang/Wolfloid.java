package mmx.x6.game.enemy.wolfang;

import mmx.x6.anim.Animator;
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

public final class Wolfloid extends Enemy {

    public enum Kind { BREATH_FLAT, CANNON_FLAT, CANNON_SLOPE_R, CANNON_SLOPE_L }

    static final String SET = "wolfloid";
    private static final int HOWL_EVERY = 200;

    private final Body body;
    private Kind kind;
    private IceStackBlock carrier;
    private int sub;
    private int timer;

    public Wolfloid(World w, RecordDef r) {
        super(w, r, SET);
        hp = 12;
        contactDamage = 4;
        damageTable = DamageTables.Table.NORMAL;
        // TODO: X is the only player, so the slope wolves (ids 0, 1) always use the cannon; Zero's breath steps are not made (docs/gaps.md, "Wolfloid")
        kind = switch (r.id()) {
            case 0 -> Kind.CANNON_SLOPE_R;
            case 1 -> Kind.CANNON_SLOPE_L;
            case 2, 4 -> Kind.BREATH_FLAT;
            default -> Kind.CANNON_FLAT;
        };
        body = new Body(x, y, 0, 0, 16, 16);
        facePlayer(w);
        anim.play(animFor(STAND));
    }

    static Wolfloid carried(World w, IceStackBlock block, boolean cannon) {
        Wolfloid wolf = new Wolfloid(w, new RecordDef(-1, "Wolfloid", "main", 0, 6, cannon ? 5 : 4, 0,
                (int) block.x, (int) block.y - 39));
        wolf.carrier = block;
        return wolf;
    }

    public Kind kind() {
        return kind;
    }

    private boolean slope() {
        return kind == Kind.CANNON_SLOPE_R || kind == Kind.CANNON_SLOPE_L;
    }

    private boolean cannon() {
        return kind != Kind.BREATH_FLAT;
    }

    private static final int STAND = 0;
    private static final int HOWL = 1;
    private static final int ATTACK = 2;

    private int animFor(int pose) {
        if (slope()) {
            boolean first = kind == Kind.CANNON_SLOPE_R ? !faceRight : faceRight;
            return (first ? 9 : 27) + pose;
        }
        return (cannon() ? 3 : 0) + pose;
    }

    @Override
    public void update(World w) {
        if (carrier != null) {
            if (carrier.broken()) {
                onKilled(w, -1);
                return;
            }
            if (carrier.frozen()) {
                carrier = null;
            } else {
                x = carrier.x;
                y = carrier.y - 39;
            }
        } else {
            fall(w);
        }
        anim.tick();
        timer++;
        if (!slope()) {
            facePlayer(w);
        }
        int range = cannon() ? 180 : 128;
        boolean inRange = Math.abs(w.player.x - x) <= range;
        switch (sub) {
            case 0 -> {
                startAnim(w, STAND);
                timer = 0;
                sub = 1;
            }
            case 1, 2 -> {
                if (inRange) {
                    startAnim(w, ATTACK);
                    timer = 0;
                    sub = 3;
                    if (!cannon()) {
                        w.sfx.play(2, 16);
                        w.add(new Breath(w, this));
                    }
                } else if (sub == 1 && timer % HOWL_EVERY == 0) {
                    startAnim(w, HOWL);
                    sub = 2;
                } else if (sub == 2 && anim.finished()) {
                    sub = 0;
                }
            }
            case 3 -> {
                if (cannon() && timer == 12) {
                    fireBall(w);
                }
                if (timer >= 90) {
                    if (cannon()) {
                        sub = 0;
                    } else {
                        startAnim(w, STAND);
                        timer = 0;
                        sub = 4;
                    }
                }
            }
            default -> {
                if (timer >= 40) {
                    sub = 0;
                }
            }
        }
        if (carrier == null) {
            checkOffScreen(w);
        }
    }

    private void startAnim(World w, int pose) {
        if (slope()) {
            facePlayer(w);
        }
        anim.play(animFor(pose));
    }

    private void fall(World w) {
        body.x = x;
        body.y = y;
        w.mover.move(body, 0, 6.5f, false, w.solids());
        y = body.y;
    }

    private void fireBall(World w) {
        float bx;
        float by;
        switch (kind) {
            case CANNON_SLOPE_R -> {
                bx = faceRight ? x : x - 16;
                by = faceRight ? y - 14 : y - 12;
            }
            case CANNON_SLOPE_L -> {
                bx = faceRight ? x + 16 : x;
                by = faceRight ? y - 12 : y - 14;
            }
            default -> {
                bx = x + (faceRight ? 12 : -12);
                by = y - 12;
            }
        }
        w.sfx.play(2, 15);
        w.add(new Projectile(w, SET, 18, bx, by, (w.player.x - bx) / 70, -3, 3, 4, 4)
                .hittable(2, DamageTables.Table.NORMAL).gravity(0.09375f, 6.5f).explodes().endsOnPlayer()
                .margin(16).facing(faceRight));
    }

    @Override
    protected void onDeath(World w) {
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, SET, new int[] {19, 20, 21, 22, 23}, x, y);
    }

    @Override
    public void onKilled(World w, int weaponId) {
        dead = true;
        if (record != null && record.index() >= 0) {
            w.spawner.released(record);
        }
        onDeath(w);
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 16, y - 16, 32, 32);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public int drawPriority() {
        return 5;
    }

    private static final class Breath extends Entity {

        private final Wolfloid wolf;
        private int age;

        Breath(World w, Wolfloid wolf) {
            this.wolf = wolf;
            this.contactDamage = 4;
            this.anim = new Animator(w.animSet(SET));
            anim.play(12);
            follow();
        }

        private void follow() {
            faceRight = wolf.faceRight;
            x = wolf.x + (faceRight ? 45 : -45);
            y = wolf.y + 3;
        }

        @Override
        public void update(World w) {
            if (wolf.dead) {
                dead = true;
                return;
            }
            follow();
            anim.tick();
            age++;
            if (anim.anim() == 12 && anim.finished()) {
                anim.play(13);
            }
            if (age == 90) {
                anim.play(14);
            }
            if (anim.anim() == 14 && anim.finished()) {
                dead = true;
            }
        }

        @Override
        public Rect contactBox() {
            if (anim.anim() != 13) {
                return null;
            }
            return faceRight ? new Rect(x - 14, y - 10, 32, 20) : new Rect(x - 18, y - 10, 32, 20);
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
}
