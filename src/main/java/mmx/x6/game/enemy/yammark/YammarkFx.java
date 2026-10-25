package mmx.x6.game.enemy.yammark;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Explosion;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

final class YammarkFx extends Entity {

    private final CommanderYammark boss;
    private int age;

    YammarkFx(World w, CommanderYammark boss, int animIndex, float dx, float dy) {
        this.boss = boss;
        this.anim = new Animator(w.animSet(CommanderYammark.SET));
        anim.play(animIndex);
        follow();
    }

    private void follow() {
        x = boss.x;
        y = boss.y;
        faceRight = boss.faceRight;
    }

    @Override
    public void update(World w) {
        if (boss.fightOver()) {
            dead = true;
            return;
        }
        follow();
        anim.tick();
        if (++age == 22) {
            w.sfx.play(2, 49);
        }
        if (anim.finished()) {
            dead = true;
        }
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public String spriteSet() {
        return CommanderYammark.SET;
    }

    @Override
    public int drawPriority() {
        return 3;
    }

    static final class Shot extends Projectile {

        Shot(World w, float x, float y, boolean right) {
            super(w, CommanderYammark.SET, 17, x, y, right ? 3.75f : -3.75f, 0, 2, 7, 7);
            hittable(1, DamageTables.Table.YAMMARK);
            explodes();
            endsOnPlayer();
            w.sfx.play(2, 50);
        }
    }

    static final class Bomb extends Entity {

        private static final int[][] BURSTS = {{10, -8, 1}, {-16, -4, 8}, {4, -12, 16}};

        private float vy = 1;
        private boolean landed;

        Bomb(World w, float x, float y) {
            this.x = x;
            this.y = y;
            this.contactDamage = 2;
            this.anim = new Animator(w.animSet(CommanderYammark.SET));
            anim.play(18);
        }

        @Override
        public void update(World w) {
            vy = Math.min(3, vy + 0.0625f);
            y += vy;
            if (TileRules.isObjectSolid(w.grid.at(x, y + 6))) {
                for (int[] b : BURSTS) {
                    w.add(new Burst(w, x + b[0], y + b[1], b[2]));
                }
                dead = true;
                return;
            }
            if (w.offScreen(x, y, 32, 32)) {
                dead = true;
            }
        }

        @Override
        public Rect contactBox() {
            return new Rect(x - 2, y - 6, 4, 12);
        }

        @Override
        public boolean clearedByGate() {
            return true;
        }

        @Override
        public String spriteSet() {
            return CommanderYammark.SET;
        }

        @Override
        public int drawPriority() {
            return 4;
        }
    }

    static final class Burst extends Entity {

        private int delay;
        private int life = 29;

        Burst(World w, float x, float y, int delay) {
            this.x = x;
            this.y = y;
            this.delay = delay;
            this.contactDamage = 4;
        }

        @Override
        public void update(World w) {
            if (delay > 0) {
                if (--delay == 0) {
                    w.add(new Explosion(w, x, y));
                }
                return;
            }
            if (--life <= 0) {
                dead = true;
            }
        }

        @Override
        public Rect contactBox() {
            return delay > 0 ? null : new Rect(x - 8, y - 8, 16, 16);
        }

        @Override
        public boolean visible() {
            return false;
        }

        @Override
        public boolean clearedByGate() {
            return true;
        }
    }
}
