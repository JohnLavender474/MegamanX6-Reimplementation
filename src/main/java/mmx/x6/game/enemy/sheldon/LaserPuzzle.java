package mmx.x6.game.enemy.sheldon;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class LaserPuzzle extends Entity {

    public enum Part { FIXED_BEAMER, AIMING_BEAMER, RELAY_BEAMER, FIXED_REFLECTOR, TURNING_REFLECTOR, DOOR }

    static final String SET = "laser-reflector-puzzle";
    private static final int NONE = 64;
    private static final int LIFE_WAIT = 302;
    private static final int FIXED_OFF = 120;
    private static final int WAIT_TABLE = 33;
    private static final int AIM_RANGE = 144;

    private static final int[][] REFLECT_INDEX = {
            {-1, -1, -1, -1, -1, 0, 48, 1}, {2, -1, -1, -1, -1, 3, 4, 5}, {6, -1, -1, -1, -1, -1, 7, 49},
            {8, 9, -1, -1, -1, -1, 10, 11}, {50, 12, -1, -1, -1, -1, -1, 13}, {14, 15, 16, -1, -1, -1, -1, 17},
            {18, 51, 19, -1, -1, -1, -1, -1}, {20, 21, 22, 23, -1, -1, -1, -1}, {-1, 24, 52, 25, -1, -1, -1, -1},
            {-1, 26, 27, 28, 29, -1, -1, -1}, {-1, -1, 30, 53, 31, -1, -1, -1}, {-1, -1, 32, 33, 34, 35, -1, -1},
            {-1, -1, -1, 36, 54, 37, -1, -1}, {-1, -1, -1, 38, 39, 40, 41, -1}, {-1, -1, -1, -1, 42, 55, 43, -1},
            {-1, -1, -1, -1, 44, 45, 46, 47}};
    private static final int[][] REFLECT = {
            {3, 15, -15}, {1, -15, -15}, {1, -24, 0}, {4, 14, -18}, {3, 0, -16}, {2, -13, -13}, {2, -20, 0},
            {4, 0, -20}, {3, -16, 0}, {2, -16, 16}, {5, 0, -24}, {4, -13, -13}, {3, -16, 16}, {5, -16, -16},
            {5, -16, 0}, {4, -13, 13}, {3, 0, 24}, {6, -16, -16}, {6, -24, 0}, {4, 0, 24}, {7, -24, 0},
            {6, -13, 13}, {5, 0, 16}, {4, 13, 16}, {7, -13, 13}, {5, 13, 13}, {0, -16, 16}, {7, 0, 14},
            {6, 10, 10}, {5, 24, 0}, {0, 0, 16}, {6, 16, 0}, {1, 0, 24}, {0, 10, 10}, {7, 13, 0}, {6, 16, -16},
            {1, 13, 13}, {7, 13, -13}, {2, 16, 16}, {1, 13, 0}, {0, 10, -10}, {7, 0, -13}, {2, 16, 0},
            {0, 0, -16}, {3, 16, 0}, {2, 10, -10}, {1, 0, -14}, {0, -16, -16}, {-1, 0, -15}, {-1, -10, -10},
            {-1, -16, 0}, {-1, -10, 10}, {-1, 0, 13}, {-1, 9, 9}, {-1, 13, 0}, {-1, 8, -8}};
    private static final int[][] REFLECTOR_BOX = {
            {-20, -16, 40, 16}, {-24, -18, 40, 20}, {-24, -24, 32, 32}, {-24, -24, 24, 40}, {-16, -22, 20, 42},
            {-20, -16, 20, 40}, {-24, -8, 32, 32}, {-28, -6, 44, 24}, {-22, -6, 42, 20}, {-16, -6, 40, 28},
            {-10, -8, 34, 32}, {-6, -16, 24, 40}, {-6, -22, 20, 42}, {-6, -24, 22, 40}, {-8, -24, 28, 32},
            {-18, -18, 38, 20}};
    private static final int[][] TURN_LIST = {
            {9, 5, 1, 13, 9, 5, 1, 13}, {15, 13, 11, 9, 7, 5, 3, 1}, {12, 8, 4, 0, 12, 8, 4, 0},
            {11, 10, 9, 8, 7, 8, 9, 10}, {0, 12, 8, 4, 0, 12, 8, 4}, {9, 7, 5, 3, 1, 15, 13, 11},
            {0, 12, 8, 4, 0, 12, 8, 4}, {8, 6, 4, 2, 0, 14, 12, 10}, {0, 14, 12, 10, 8, 6, 4, 2},
            {3, 15, 11, 7, 3, 15, 11, 7}, {0, 14, 10, 9, 8, 6, 4, 2}, {12, 7, 3, 15, 10, 7, 3, 15},
            {0, 14, 12, 10, 8, 6, 4, 2}, {8, 6, 4, 2, 0, 14, 12, 10}, {8, 6, 4, 2, 0, 14, 12, 10},
            {8, 6, 4, 2, 0, 14, 12, 10}};
    private static final int[][] DOOR = {
            {3136, 336, 3136, 144, 2, 4}, {3792, 384, 3792, 144, 2, 4}, {4128, 336, 4128, 144, 2, 4},
            {4784, 352, 4784, 144, 2, 4}, {4688, 448, 4688, 176, 4, 2}, {3888, 704, 3584, 704, 4, 2}};
    private static final int[] DOOR_LIMIT = {3488, 3824, 4480, 4832, 512, 768};
    private static final int[][] DOOR_ROOM = {
            {3168, 3791, 256, 447}, {3824, 4127, 256, 447}, {4160, 4783, 256, 447}, null,
            {3856, 4783, 480, 703}, {3856, 4767, 736, 991}};
    private static final int[][] DOOR_DELETE = {
            {2896, 3167, 256, 447}, {3168, 3823, 256, 447}, {3824, 4159, 256, 447}, {4160, 4815, 256, 447},
            {4160, 4783, 256, 479}, {3856, 4783, 480, 735}};

    private static int nextShotId;

    private final Part part;
    private final int n;
    private final int shotId;
    private int dir;
    private int r;
    private int listIndex;
    private int link;
    private int lastIn = NONE;
    private boolean fireNext;
    private boolean refire;
    private int waitTimer;
    private int sub;
    private int timer;
    private boolean relayAwake;
    private int doorStep = 2;
    private int removeTimer = -1;

    public LaserPuzzle(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        int var = r.id();
        this.part = Part.values()[Math.min(5, var >> 4)];
        this.n = var & 15;
        this.shotId = nextShotId++;
        this.hp = 127;
        this.anim = new Animator(w.animSet(SET));
        switch (part) {
            case FIXED_BEAMER, AIMING_BEAMER -> {
                dir = n & 7;
                anim.play(8 + dir);
            }
            case RELAY_BEAMER -> {
                dir = n & 7;
                anim.play(dir);
            }
            case FIXED_REFLECTOR -> {
                this.r = n;
                damageTable = DamageTables.Table.REFLECTOR;
                anim.play(16 + this.r);
            }
            case TURNING_REFLECTOR -> {
                this.r = TURN_LIST[n][0];
                damageTable = DamageTables.Table.REFLECTOR;
                anim.play(16 + this.r);
            }
            case DOOR -> {
                anim.play(n <= 3 ? 33 : 32);
                int[] room = DOOR_ROOM[Math.min(n, 5)];
                if (room != null) {
                    w.spawner.spawnInRect(w, room[0], room[1], room[2], room[3]);
                }
            }
        }
    }

    public Part part() {
        return part;
    }

    public int direction() {
        return part == Part.FIXED_REFLECTOR || part == Part.TURNING_REFLECTOR ? r : dir;
    }

    public boolean open() {
        return part == Part.DOOR && doorStep >= 3;
    }

    int link() {
        return link;
    }

    void timedOut() {
        link = 2;
    }

    boolean removed() {
        return removeTimer >= 0;
    }

    @Override
    public void update(World w) {
        if (removeTimer >= 0) {
            if (--removeTimer < 0) {
                dead = true;
                w.spawner.kept(record);
            }
            return;
        }
        switch (part) {
            case FIXED_BEAMER -> fixedBeamer(w);
            case AIMING_BEAMER -> aimingBeamer(w);
            case RELAY_BEAMER -> {
                if (relayAwake) {
                    aimingBeamer(w);
                }
                if (catchLaser(w) != null) {
                    relayAwake = true;
                }
            }
            case FIXED_REFLECTOR, TURNING_REFLECTOR -> reflector(w);
            case DOOR -> door(w);
        }
    }

    private void fire(World w, int d, float sx, float sy, int sound) {
        w.sfx.play(2, sound);
        link = 1;
        w.add(new Laser(w, this, d, shotId, sx, sy));
    }

    private void fireBeam(World w) {
        float step = Laser.DX[dir] != 0 && Laser.DY[dir] != 0 ? 12 : 16;
        fire(w, dir, x + Laser.DX[dir] * step, y + Laser.DY[dir] * step, 19);
    }

    private void fixedBeamer(World w) {
        switch (sub) {
            case 0 -> {
                timer = 3;
                sub = 1;
            }
            case 1 -> {
                if (--timer <= 0) {
                    fireBeam(w);
                    sub = 2;
                }
            }
            case 2 -> {
                if (link == 2) {
                    link = 0;
                    timer = FIXED_OFF;
                    sub = 3;
                }
            }
            default -> {
                if (--timer <= 0) {
                    sub = 0;
                }
            }
        }
    }

    private void aimingBeamer(World w) {
        switch (sub) {
            case 0 -> {
                if (link == 2) {
                    link = 0;
                }
                if (link == 1) {
                    return;
                }
                if (Math.abs(w.player.x - x) <= AIM_RANGE) {
                    turnToward(w);
                    anim.play((part == Part.RELAY_BEAMER ? 0 : 8) + dir);
                }
                sub = 1;
            }
            case 1 -> {
                boolean back = part == Part.RELAY_BEAMER && lastIn != NONE && dir == (lastIn + 4) % 8;
                if (!back) {
                    fireBeam(w);
                }
                timer = LIFE_WAIT;
                sub = 2;
            }
            default -> {
                if (link == 2) {
                    link = 0;
                }
                if (--timer <= 0) {
                    sub = 0;
                }
            }
        }
    }

    private void turnToward(World w) {
        double a = Math.toDegrees(Math.atan2(-(w.player.y - y), w.player.x - x));
        int i32 = Math.floorMod((int) Math.round(a / 11.25), 32);
        int target = -1;
        for (int d = 0; d < 8; d++) {
            int half = d % 2 == 0 ? 2 : 1;
            int diff = Math.floorMod(i32 - d * 4 + 16, 32) - 16;
            if (Math.abs(diff) <= half) {
                target = d;
                break;
            }
        }
        if (target < 0 || target == dir) {
            return;
        }
        int diff = Math.floorMod(target - dir, 8);
        dir = Math.floorMod(dir + (diff > 0 && diff < 4 ? 1 : -1), 8);
    }

    private void reflector(World w) {
        anim.tick();
        if (link == 0 && fireNext) {
            int idx = REFLECT_INDEX[r][lastIn & 7];
            if (lastIn != NONE && idx >= 0 && REFLECT[idx][0] >= 0) {
                fire(w, REFLECT[idx][0], x + REFLECT[idx][1], y + REFLECT[idx][2], 18);
            }
            fireNext = false;
        } else if (link == 2) {
            link = 0;
            refire = true;
        }
        hp = 127;
        Laser caught = catchLaser(w);
        if (caught != null) {
            if (link == 0) {
                fireNext = true;
            }
        } else {
            link = 0;
        }
        if (waitTimer > 0 && --waitTimer == 0) {
            damageTable = DamageTables.Table.REFLECTOR;
        }
    }

    private void turn(World w) {
        hp = 127;
        damageTable = DamageTables.Table.REFLECTOR_WAIT;
        waitTimer = WAIT_TABLE;
        if (part != Part.TURNING_REFLECTOR) {
            return;
        }
        if (link == 1) {
            fireNext = true;
            link = 0;
        }
        w.sfx.play(2, 78);
        listIndex = (listIndex + 1) % 8;
        r = TURN_LIST[n][listIndex];
        anim.play(16 + r);
    }

    @Override
    public void onHurt(World w, int weaponId) {
        turn(w);
    }

    @Override
    public void onDeflect(World w, int weaponId) {
        turn(w);
    }

    @Override
    public void onKilled(World w, int weaponId) {
        turn(w);
    }

    private Rect sensor() {
        return switch (part) {
            case RELAY_BEAMER -> new Rect(x - 8, y - 8, 16, 16);
            case FIXED_REFLECTOR, TURNING_REFLECTOR -> reflectorBox();
            case DOOR -> n <= 3 ? new Rect(x - 16, y - 40, 16, 16) : new Rect(x + 24, y - 16, 16, 16);
            default -> null;
        };
    }

    private Rect reflectorBox() {
        int[] b = REFLECTOR_BOX[r];
        return new Rect(x + b[0], y + b[1], b[2], b[3]);
    }

    private Laser catchLaser(World w) {
        Rect s = sensor();
        if (s == null) {
            return null;
        }
        for (Entity e : w.entities) {
            if (!(e instanceof Laser l) || !l.live() || l.shotId == shotId || !l.headBox().overlaps(s)) {
                continue;
            }
            if (l.catchMark != 0) {
                return null;
            }
            if (refire) {
                l.catchMark = 1;
                refire = false;
            } else {
                float[] p = catchPoint(l.dir());
                l.pin(p[0], p[1]);
            }
            lastIn = l.dir();
            return l;
        }
        return null;
    }

    private float[] catchPoint(int in) {
        return switch (part) {
            case FIXED_REFLECTOR, TURNING_REFLECTOR -> {
                int idx = REFLECT_INDEX[r][in];
                yield idx >= 0 ? new float[] {x + REFLECT[idx][1], y + REFLECT[idx][2]} : new float[] {x, y};
            }
            case DOOR -> n <= 3 ? new float[] {x - 8, y - 30} : new float[] {x + 30, y - 8};
            default -> new float[] {x, y};
        };
    }

    private void door(World w) {
        switch (doorStep) {
            case 2 -> {
                if (catchLaser(w) != null) {
                    w.sfx.play(2, 32);
                    doorStep = 3;
                }
            }
            case 3 -> {
                catchLaser(w);
                anim.tick();
                if (anim.enteredFlag(1)) {
                    int[] d = DOOR[n];
                    w.copyTiles(d[2], d[3], d[4], d[5], d[0], d[1]);
                    if (n <= 3) {
                        w.camera.setTargets(-1, DOOR_LIMIT[n], -1, -1);
                    } else {
                        w.camera.setTargets(-1, -1, -1, DOOR_LIMIT[n]);
                    }
                    doorStep = 4;
                }
            }
            case 4 -> {
                catchLaser(w);
                boolean passed = n <= 3 ? w.camera.x() >= x + 16 : w.camera.y() >= y + 16;
                if (passed) {
                    afterPass(w);
                    doorStep = 5;
                }
            }
            default -> catchLaser(w);
        }
    }

    private void afterPass(World w) {
        int[] rect = DOOR_DELETE[n];
        for (Entity e : w.entities) {
            if (e instanceof LaserPuzzle p && p != this && !p.dead && p.x >= rect[0] && p.x <= rect[1]
                    && p.y >= rect[2] && p.y <= rect[3]) {
                p.removeTimer = 2;
            }
        }
        if (x >= rect[0] && x <= rect[1] && y >= rect[2] && y <= rect[3]) {
            removeTimer = 2;
        }
        switch (n) {
            case 4 -> {
                w.camera.setTargets(3840, -1, (int) y + 32, (int) y + 32);
                w.copyTiles(4592, 448, 5, 2, 4688, 448);
            }
            case 5 -> {
                w.camera.setTargets(-1, -1, (int) y + 32, (int) y + 32);
                w.copyTiles(4592, 448, 5, 2, 3888, 704);
            }
            default -> w.camera.setTargets((int) x + 16, -1, -1, -1);
        }
    }

    @Override
    public Rect hurtBox() {
        return part == Part.FIXED_REFLECTOR || part == Part.TURNING_REFLECTOR ? reflectorBox() : null;
    }

    @Override
    public boolean visible() {
        return !removed();
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
        return 7;
    }
}
