package mmx.x6.game.enemy.yammark;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Enemy;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;
import mmx.x6.physics.TileRules;

public final class NightmareInsect extends Enemy {

    public enum Step { IDLE, BLADE, LAY, REVIVE }

    static final String SET = "nightmare-insect";
    private static final int HP = 32;
    private static final int IDLE_UPDATES = 62;

    private Step step = Step.IDLE;
    private int sub;
    private int timer;

    public NightmareInsect(World w, RecordDef r) {
        super(w, r, SET);
        hp = HP;
        contactDamage = 8;
        damageTable = DamageTables.Table.NORMAL;
        facePlayer(w);
        anim.play(0);
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (step) {
            case IDLE -> {
                facePlayer(w);
                anim.playIfNot(0);
                timer++;
                if (timer >= IDLE_UPDATES && !w.offScreen(x, y, 48, 48)) {
                    boolean near = Math.abs(w.player.x - x) < 64;
                    if (near || hatchlings(w) >= 4) {
                        step = Step.BLADE;
                        anim.play(1);
                    } else {
                        step = Step.LAY;
                        anim.play(3);
                        sub = 1;
                        timer = 0;
                    }
                }
            }
            case BLADE -> {
                if (anim.entered(1)) {
                    w.sfx.play(2, 62);
                    w.add(new Blade(w, this, x + (faceRight ? 16 : -16), y, faceRight));
                }
                if (anim.entered(2)) {
                    toIdle();
                }
            }
            case LAY -> {
                if (sub == 1) {
                    timer++;
                    if (timer % 20 == 0 && timer <= 160) {
                        w.add(new Hatchling(w, this, x + (faceRight ? 32 : -32), y - 16, faceRight));
                    }
                    if (timer >= 161 || hatchlings(w) >= 8) {
                        anim.play(4);
                        sub = 2;
                    }
                } else if (anim.entered(2)) {
                    toIdle();
                }
            }
            case REVIVE -> {
                if (anim.entered(3)) {
                    toIdle();
                }
            }
        }
    }

    private void toIdle() {
        step = Step.IDLE;
        anim.play(0);
        timer = 0;
        sub = 0;
    }

    private static int hatchlings(World w) {
        int n = 0;
        for (Entity e : w.entities) {
            if (e instanceof Hatchling && !e.dead) {
                n++;
            }
        }
        return n;
    }

    boolean husk() {
        return step == Step.REVIVE;
    }

    @Override
    public void onKilled(World w, int weaponId) {
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, SET, new int[] {8, 9, 10, 11}, x, y);
        if (weaponId == 1 || weaponId == 10 || weaponId == 54) {
            dead = true;
            w.spawner.released(record);
            return;
        }
        hp = HP;
        step = Step.REVIVE;
        anim.play(5);
    }

    @Override
    protected void onDeath(World w) {
    }

    private float boxX(float left, float width) {
        return faceRight ? x - left - width : x + left;
    }

    @Override
    public Rect hurtBox() {
        return husk() ? null : new Rect(boxX(-34, 60), y - 16, 60, 32);
    }

    @Override
    public Rect contactBox() {
        // TODO: the boxes are not centred (x -28..+20, -34..+26); whether the engine mirrors them with the facing is not read, so they are mirrored (docs/gaps.md, "Nightmare Insect")
        return husk() ? null : new Rect(boxX(-28, 48), y - 16, 48, 32);
    }

    @Override
    public int drawPriority() {
        return 5;
    }

    private static final class Blade extends Entity {

        private final NightmareInsect parent;
        private int part;
        private int timer;
        private float vx;
        private float vy;

        Blade(World w, NightmareInsect parent, float x, float y, boolean faceRight) {
            this.parent = parent;
            this.x = x;
            this.y = y;
            this.faceRight = faceRight;
            this.vx = faceRight ? 3 : -3;
            this.contactDamage = 6;
            this.damageTable = DamageTables.Table.DEFLECT;
            this.anim = new Animator(w.animSet(SET));
            anim.play(7);
        }

        @Override
        public void update(World w) {
            anim.tick();
            if (parent.dead || parent.husk()) {
                dead = true;
                return;
            }
            timer++;
            switch (part) {
                case 0 -> {
                    x += vx;
                    if (timer >= 92 || Math.abs(w.player.x - x) <= 8 && Math.abs(w.player.y - y) <= 32) {
                        part = 1;
                        timer = 0;
                    }
                }
                case 1 -> {
                    if (timer >= 92) {
                        double a = Math.atan2(w.player.y - y, w.player.x - x);
                        vx = (float) Math.cos(a) * 5;
                        vy = (float) Math.sin(a) * 5;
                        part = 2;
                    }
                }
                default -> {
                    x += vx;
                    y += vy;
                    if (w.offScreen(x, y, 64, 64)) {
                        dead = true;
                    }
                }
            }
        }

        @Override
        public Rect contactBox() {
            return new Rect(x - 32, y - 8, 64, 16);
        }

        @Override
        public Rect hurtBox() {
            return new Rect(x - 16, y - 8, 32, 16);
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
            return 5;
        }
    }

    private static final class Hatchling extends Entity {

        private static final float SPEED = 1.5f;

        private final NightmareInsect parent;
        private final Body body;
        private int step;
        private int timer;

        Hatchling(World w, NightmareInsect parent, float x, float y, boolean faceRight) {
            this.parent = parent;
            this.x = x;
            this.y = y;
            this.faceRight = faceRight;
            this.hp = 3;
            this.contactDamage = 3;
            this.damageTable = DamageTables.Table.MINOR;
            this.body = new Body(x, y, 0, 0, 8, 8);
            this.anim = new Animator(w.animSet(SET));
            anim.play(6);
        }

        @Override
        public void update(World w) {
            anim.tick();
            if (parent.dead || parent.husk()) {
                burst(w);
                return;
            }
            body.x = x;
            body.y = y;
            switch (step) {
                case 0 -> {
                    int f = w.mover.move(body, 0, SPEED, false, w.solids());
                    if ((f & TerrainMover.FLOOR) != 0) {
                        step = 1;
                    }
                }
                case 1 -> {
                    float ahead = x + (faceRight ? 8 : -8);
                    boolean ledge = !TileRules.isObjectSolid(w.grid.at(ahead, y + 16));
                    boolean wall = TileRules.isWall(w.grid.at(ahead, y));
                    if (ledge || wall) {
                        faceRight = !faceRight;
                    }
                    w.mover.move(body, faceRight ? SPEED : -SPEED, 1, true, w.solids());
                    if (++timer >= 242) {
                        step = 2;
                        timer = 0;
                    }
                }
                default -> {
                    flash = (timer & 2) == 0 ? 1 : 0;
                    if (++timer >= 52) {
                        burst(w);
                        return;
                    }
                }
            }
            x = body.x;
            y = body.y;
            if (w.offScreen(x, y, 64, 64) && y >= w.camera.y() + 301) {
                dead = true;
            }
        }

        private void burst(World w) {
            dead = true;
            Debris.spawn(w, SET, new int[] {12, 13}, x, y);
            w.add(new Explosion(w, x, y));
        }

        @Override
        public void onKilled(World w, int weaponId) {
            burst(w);
        }

        @Override
        public Rect hurtBox() {
            return new Rect(x - 4, y - 4, 8, 8);
        }

        @Override
        public Rect contactBox() {
            return hurtBox();
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
}
