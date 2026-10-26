package mmx.x6.game.enemy.wolfang;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Debris;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;

public final class IceBlock extends Entity {

    public enum Mode { AVALANCHE, AVALANCHE_RIGHT, FALLING_ICE, BOUNCING, BLOCK_LEFT, BLOCK_RIGHT, PIECE, HOPPER }

    static final String SET = "ice-block";
    private static final int[] DEBRIS = {4, 5, 6};
    private static final float HOP_UP = 2.0625f;
    private static final float HOP_GRAVITY = 0.064453125f;

    private final Mode mode;
    private final boolean small;
    private final Body body;
    private int timer;
    private int count;
    private int made;
    private float vx;
    private float vy;
    private boolean boxes;
    private int age;

    IceBlock(World w, Mode mode, float x, float y) {
        this.mode = mode;
        this.x = x;
        this.y = y;
        this.small = w.rng.nextBoolean();
        this.hp = 8;
        this.contactDamage = 8;
        this.damageTable = DamageTables.Table.ICE_BLOCK;
        this.anim = new Animator(w.animSet(SET));
        float half = small ? 22 : 30;
        this.body = new Body(x, y, 0, 0, half, half);
        switch (mode) {
            case AVALANCHE -> {
                timer = 120;
                count = 360;
            }
            case AVALANCHE_RIGHT -> {
                timer = 12;
                count = 360;
            }
            case FALLING_ICE -> count = 180;
            case BLOCK_LEFT, BLOCK_RIGHT -> {
                vx = mode == Mode.BLOCK_LEFT ? -8 : 8;
                vy = 4;
                timer = 50;
                anim.play(small ? 1 : 0);
            }
            case PIECE -> {
                vy = 9;
                boxes = true;
                anim.play(small ? 3 : 2);
            }
            case HOPPER -> {
                faceRight = w.player.x >= x;
                vx = faceRight ? 1.125f : -1.125f;
                vy = -HOP_UP;
                timer = 20;
                anim.play(small ? 1 : 0);
            }
            default -> {
            }
        }
    }

    public Mode mode() {
        return mode;
    }

    private boolean spawner() {
        return mode.ordinal() <= Mode.BOUNCING.ordinal();
    }

    @Override
    public void update(World w) {
        if (w.player.x >= 4673 && w.player.y >= 1265) {
            dead = true;
            return;
        }
        switch (mode) {
            case AVALANCHE, AVALANCHE_RIGHT -> spawnAvalanche(w);
            case FALLING_ICE -> spawnFalling(w);
            case BOUNCING -> spawnHoppers(w);
            case BLOCK_LEFT, BLOCK_RIGHT -> {
                if (appear()) {
                    move();
                }
                boolean gone = mode == Mode.BLOCK_LEFT ? x < w.player.x - 240 || w.checkpoint > 0
                        : x > w.player.x + 240 || w.player.y >= 529 && w.player.x >= 5025;
                if (gone) {
                    dead = true;
                }
            }
            case PIECE -> {
                age++;
                anim.tick();
                y += vy;
                if (w.offScreen(x, y, 240, 64)) {
                    dead = true;
                }
            }
            case HOPPER -> hop(w);
        }
    }

    private boolean appear() {
        if (timer > 0) {
            timer--;
            return false;
        }
        boxes = true;
        return true;
    }

    private void move() {
        age++;
        anim.tick();
        x += vx;
        y += vy;
    }

    private void spawnAvalanche(World w) {
        if (timer > 0) {
            timer--;
            return;
        }
        if (mode == Mode.AVALANCHE && w.checkpoint > 0 || count <= 0) {
            dead = true;
            return;
        }
        if (count % 7 == 0) {
            Mode block = mode == Mode.AVALANCHE ? Mode.BLOCK_LEFT : Mode.BLOCK_RIGHT;
            w.add(new IceBlock(w, block, x, y + w.rng.nextInt(32) - 16));
        }
        count--;
    }

    private void spawnFalling(World w) {
        if (timer > 0) {
            timer--;
            return;
        }
        if (count == 0) {
            timer = 120;
            count = 180;
            return;
        }
        if (count % 10 == 0) {
            w.add(new IceBlock(w, Mode.PIECE, x + w.rng.nextInt(16) - 8, y));
        }
        count--;
    }

    private void spawnHoppers(World w) {
        if (timer > 0) {
            timer--;
            return;
        }
        if (Math.abs(w.player.x - x) < 48) {
            return;
        }
        if (count > 0) {
            count--;
            return;
        }
        w.add(new IceBlock(w, Mode.HOPPER, x + w.rng.nextInt(16) - 8, y));
        if (++made >= 4) {
            made = 0;
            timer = 140;
        }
        count = 100;
    }

    private void hop(World w) {
        if (!appear()) {
            return;
        }
        anim.tick();
        vy = Math.min(6.5f, vy + HOP_GRAVITY);
        body.x = x;
        body.y = y;
        int f = w.mover.move(body, vx, vy, false, java.util.List.of());
        x = body.x;
        y = body.y;
        if ((f & (TerrainMover.LEFT | TerrainMover.RIGHT)) != 0) {
            w.sfx.play(2, 17);
            Debris.spawn(w, SET, DEBRIS, x, y);
            dead = true;
            return;
        }
        if ((f & TerrainMover.FLOOR) != 0) {
            vy = -HOP_UP;
            // TODO: each landing's Quake2(8, 2, 2) screen shake is not done (docs/gaps.md, "Screen shake")
            w.sfx.play(2, 17);
        }
        if (w.offScreen(x, y, 240, 64)) {
            dead = true;
        }
    }

    @Override
    public void onKilled(World w, int weaponId) {
        dead = true;
        Debris.spawn(w, SET, DEBRIS, x, y);
    }

    @Override
    public Rect hurtBox() {
        if (spawner() || !boxes) {
            return null;
        }
        float h = small ? 24 : 32;
        return new Rect(x - h, y - h, h * 2, h * 2);
    }

    @Override
    public Rect contactBox() {
        if (spawner() || !boxes) {
            return null;
        }
        float h = small ? 22 : 28;
        return new Rect(x - h, y - h, h * 2, h * 2);
    }

    @Override
    public boolean visible() {
        if (spawner()) {
            return false;
        }
        return boxes || (timer & 1) == 0;
    }

    @Override
    public boolean isEnemy() {
        return !spawner();
    }

    @Override
    public String spriteSet() {
        return SET;
    }

    @Override
    public int drawPriority() {
        return 6;
    }

    static final class Controller extends Entity {

        private static final int[][] TRIGGERS = {
                {497, 864, 1024, 0, 0}, {769, 1104, 944, 0, 0}, {1281, 1600, 752, 0, 0},
                {1809, 2224, 496, 0, 0}, {2225, 2384, 480, 0, 0}, {2513, 2656, 400, 0, 0},
                {3185, 3320, 224, 2, 0}, {3425, 3568, 224, 2, 0}, {3713, 3856, 224, 2, 0},
                {3857, 4000, 224, 2, 0}, {4593, 4720, 224, 2, 0}, {4977, 5080, 224, 2, 0},
                {4753, 4720, 432, 3, 448}, {4833, 4720, 648, 3, 688}, {4753, 4720, 848, 3, 848}};

        private final boolean[] done = new boolean[TRIGGERS.length];

        Controller(World w, mmx.x6.level.RecordDef r) {
            this.record = r;
        }

        @Override
        public void update(World w) {
            float px = w.player.x;
            float py = w.player.y;
            if (px >= 4833 && py >= 1217) {
                w.copyTiles(4672, 912, 3, 1, 4672, 1056);
                w.sfx.play(5, 3);
                dead = true;
                return;
            }
            if (w.checkpoint >= 3) {
                return;
            }
            for (int i = 0; i < TRIGGERS.length; i++) {
                int[] t = TRIGGERS[i];
                if (done[i]) {
                    continue;
                }
                if (px < t[0] || px > t[0] + 94) {
                    continue;
                }
                if (t[4] > 0 && py < t[4]) {
                    break;
                }
                done[i] = true;
                Mode m = Mode.values()[t[3]];
                float sy = m == Mode.BOUNCING ? t[2] : t[2] + w.rng.nextInt(32) - 16;
                // TODO: the rumble loops (5/8, 5/9) play once at the start; their stop rules and the avalanche camera shake are not done (docs/gaps.md, "Ice fall controller")
                if (m == Mode.FALLING_ICE) {
                    w.sfx.play(5, 9);
                } else if (m == Mode.AVALANCHE) {
                    w.sfx.play(5, 8);
                }
                w.add(new IceBlock(w, m, t[1], sy));
            }
        }

        @Override
        public boolean visible() {
            return false;
        }

        @Override
        public boolean clearedByGate() {
            return false;
        }
    }
}
