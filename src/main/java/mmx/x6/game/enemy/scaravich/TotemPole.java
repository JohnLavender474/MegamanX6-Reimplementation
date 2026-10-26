package mmx.x6.game.enemy.scaravich;

import java.util.ArrayList;
import java.util.List;
import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.physics.Rect;

public final class TotemPole extends Entity implements Solid {

    static final String SET = TeleporterPole.SET;
    private static final int[] HEAD_DY = {64, 32, 0, -32, -64};
    private static final int[] POLE_Y = {336, 576, 1056, 1296};
    private static final int[] WRECK_DEBRIS = {22, 23, 24};
    private static final int[] BASE_DEBRIS = {22, 23, 24, 25};
    private static final int DEATH_UPDATES = 120;

    private final List<Head> heads = new ArrayList<>();
    private boolean madeHeads;
    private int timer;

    TotemPole(World w, float x, float y) {
        this.x = x;
        this.y = y;
        this.hp = 5;
        this.damageTable = DamageTables.Table.DEFLECT;
        this.anim = new Animator(w.animSet(SET));
        anim.play(19);
    }

    public List<Head> heads() {
        return heads;
    }

    boolean over() {
        return dead || hp <= 0 && timer >= DEATH_UPDATES;
    }

    @Override
    public void update(World w) {
        if (!madeHeads) {
            madeHeads = true;
            for (int n = 0; n < HEAD_DY.length; n++) {
                Head h = new Head(w, this, 14 - n, x, y + HEAD_DY[n]);
                heads.add(h);
                w.add(h);
            }
        }
        anim.tick();
        if (hp > 0) {
            return;
        }
        timer++;
        if (timer % 8 == 0) {
            w.add(new Explosion(w, x + w.rng.nextInt(16) - 16, y + w.rng.nextInt(128) - 64));
        }
        if (timer % 32 == 0) {
            Debris.spawn(w, SET, BASE_DEBRIS, x, y);
        }
        if (timer >= DEATH_UPDATES) {
            dead = true;
            // TODO: the item roll DropItem(table 21) is not made; there are no item drops (docs/gaps.md, "Skipped on purpose")
            for (int py : POLE_Y) {
                if ((int) y == py) {
                    w.polesCleared++;
                    break;
                }
            }
        }
    }

    void headLost() {
        hp--;
    }

    @Override
    public Rect solidRect() {
        return dead ? null : new Rect(x - 14, y - 80, 28, 160);
    }

    @Override
    public Rect hurtBox() {
        return hp > 0 ? new Rect(x - 14, y - 16, 28, 32) : null;
    }

    @Override
    public boolean visible() {
        return hp > 0 || (timer & 2) == 0;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public String spriteSet() {
        return SET;
    }

    @Override
    public int drawPriority() {
        return 6;
    }

    public static final class Head extends Entity {

        public enum State { FIGHT, WRECK }

        private final TotemPole base;
        private final int var;
        private State state = State.FIGHT;
        private int act;
        private int timer;
        private float vx;
        private float vy;
        private float ax;
        private float ay;
        private float aimX;
        private float aimY;
        private int flyTimer;

        Head(World w, TotemPole base, int var, float x, float y) {
            this.base = base;
            this.var = var;
            this.x = x;
            this.y = y;
            this.hp = 16;
            this.contactDamage = 4;
            this.damageTable = DamageTables.Table.NORMAL;
            this.anim = new Animator(w.animSet(SET));
            this.timer = var == 10 || var == 11 ? 180 : 90;
            anim.play(var == 12 ? 6 : var == 14 ? 3 : 0);
        }

        public int var() {
            return var;
        }

        public State state() {
            return state;
        }

        public boolean flying() {
            return var == 12 && act > 0;
        }

        @Override
        public void update(World w) {
            if (state == State.WRECK) {
                if (base.over()) {
                    for (int i = 0; i < 5; i++) {
                        w.add(new Explosion(w, x + w.rng.nextInt(33) - 16, y + w.rng.nextInt(33) - 16));
                    }
                    Debris.spawn(w, SET, BASE_DEBRIS, x, y);
                    dead = true;
                }
                return;
            }
            faceRight = x < w.player.x;
            anim.tick();
            damageTable = (anim.flag() & 0xF0) != 0 ? DamageTables.Table.DEFLECT : DamageTables.Table.NORMAL;
            if (var == 12) {
                bird(w);
            } else {
                face(w);
            }
        }

        private int period(World w) {
            // TODO: the period is 40 on Hard too; the difficulty is fixed at Normal here (docs/gaps.md, "Totem pole")
            return base.hp < 3 ? 40 : 90;
        }

        private void sound(World w, int id) {
            if (!w.offScreen(x, y, 0, 0)) {
                w.sfx.play(2, id);
            }
        }

        private void face(World w) {
            timer--;
            switch (act) {
                case 0 -> {
                    if (timer <= 0) {
                        anim.play(var == 14 ? 4 : 1);
                        timer = period(w);
                        sound(w, 11);
                        act = 1;
                    }
                }
                case 1 -> {
                    if (timer <= 0) {
                        anim.play(var == 14 ? 28 : 27);
                        timer = period(w);
                        sound(w, 11);
                        act = 2;
                    }
                }
                case 2 -> {
                    if (anim.enteredFlag(1)) {
                        anim.play(var == 14 ? 15 : 14);
                        act = 3;
                    }
                }
                default -> {
                    if (anim.enteredFlag(1)) {
                        fire(w);
                        act = 0;
                    }
                }
            }
        }

        private void fire(World w) {
            float ox = x + (faceRight ? 18 : -18);
            float oy = y + 8;
            float[] v = Projectile.aim(ox, oy, w.player.x, w.player.y, 1.5f);
            sound(w, 10);
            w.add(new Projectile(w, SET, 16, ox, oy, v[0], v[1], 4, 4, 4).endsOnPlayer().priority(3)
                    .margin(32).facing(faceRight));
        }

        private void aim(World w) {
            aimX = w.player.x;
            aimY = w.player.y;
            float[] v = Projectile.aim(x, y, aimX, aimY, 2);
            vx = v[0];
            vy = v[1];
        }

        private void bird(World w) {
            if (act >= 2 && ++flyTimer % 32 == 0) {
                sound(w, 13);
            }
            switch (act) {
                case 0 -> {
                    if (base.hp < 4) {
                        timer = 10;
                        vx = faceRight ? 2 : -2;
                        sound(w, 12);
                        act = 1;
                    }
                }
                case 1 -> {
                    x += vx;
                    if (--timer <= 0) {
                        anim.play(17);
                        aim(w);
                        act = 2;
                    }
                }
                case 2 -> {
                    x += vx;
                    y += vy;
                    if (Math.abs(x - aimX) <= 8 && Math.abs(y - aimY) <= 8) {
                        ax = vx / 16;
                        ay = vy / 16;
                        timer = 16;
                        act = 3;
                    }
                }
                case 3 -> {
                    vx -= ax;
                    vy -= ay;
                    x += vx;
                    y += vy;
                    if (--timer < 0) {
                        vx = vy = 0;
                        timer = 40;
                        act = 4;
                    }
                }
                default -> {
                    if (--timer <= 0) {
                        aim(w);
                        act = 2;
                    }
                }
            }
        }

        @Override
        public void onKilled(World w, int weaponId) {
            base.headLost();
            w.add(new Explosion(w, x, y));
            if (var == 12) {
                Debris.spawn(w, SET, BASE_DEBRIS, x, y);
                Debris.spawn(w, SET, new int[] {22}, x, y);
                w.sfx.play(2, 20);
                dead = true;
                return;
            }
            Debris.spawn(w, SET, WRECK_DEBRIS, x, y);
            hp = 0;
            contactDamage = 0;
            damageTable = DamageTables.Table.DEFLECT;
            anim.play(var == 14 ? 21 : 20);
            state = State.WRECK;
        }

        @Override
        public Rect hurtBox() {
            return new Rect(x - 14, y - 16, 28, 32);
        }

        @Override
        public Rect contactBox() {
            return state == State.FIGHT ? new Rect(x - 16, y - 16, 32, 32) : null;
        }

        @Override
        public boolean isEnemy() {
            return state == State.FIGHT;
        }

        @Override
        public boolean clearedByGate() {
            return false;
        }

        @Override
        public String spriteSet() {
            return SET;
        }

        @Override
        public int drawPriority() {
            return var == 12 ? 4 : 5;
        }
    }
}
