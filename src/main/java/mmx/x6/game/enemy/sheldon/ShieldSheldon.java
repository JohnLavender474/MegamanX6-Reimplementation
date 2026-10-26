package mmx.x6.game.enemy.sheldon;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.boss.Maverick;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.AnimFx;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class ShieldSheldon extends Maverick {

    public enum Step { INTRO, BOUNCE, SHIELD_THROW, PLATES, MIRROR, TURRETS }

    static final String SET = "shield-sheldon";
    static final int ROOM_X = 5152;
    static final int ROOM_Y = 256;
    private static final int MAX_HP = 32;
    private static final int[][] HIGH = {{4, 3}, {4, 6}};
    private static final int[][] LOW = {{5, 7}, {5, 4}, {6, 6, 7}};
    private static final int[][][] BOUNCE_ROUTE = {
            {{80, 176}, {16, 80}, {96, 0}, {240, 176}, {304, 80}, {224, 0}, {160, 176}, {160, 32}},
            {{240, 176}, {304, 80}, {224, 0}, {80, 176}, {16, 80}, {96, 0}, {160, 176}, {160, 32}}};
    private static final int[][] CORNERS = {{288, 32}, {32, 32}, {32, 144}, {288, 144}};

    private Step step = Step.INTRO;
    private int sub;
    private int timer;
    private int mode = -1;
    private int[] pattern;
    private int patternIndex;
    private boolean guarded = true;
    private boolean boxes;
    private boolean lungeBoxes;
    int act4;
    int freeze;
    private int warp;
    private int warpX;
    private int warpY;
    private boolean warpFace;
    private int warpIn;
    private int[][] route;
    private int leg;
    private int dashes;
    private int dashTarget;
    private boolean firstWarp;
    private Mirror mirror;
    private boolean flashDone;

    public ShieldSheldon(World w, RecordDef r) {
        super(w, r, SET, MAX_HP, DamageTables.Table.SHELDON);
        x = r.x();
        y = r.y();
        contactDamage = 10;
    }

    public Step step() {
        return step;
    }

    public boolean open() {
        return !guarded;
    }

    boolean active() {
        return !dead && !dying();
    }

    float rx(float roomX) {
        return ROOM_X + roomX;
    }

    float ry(float roomY) {
        return ROOM_Y + roomY;
    }

    private void open(boolean on) {
        guarded = !on;
        if (invincible == 0) {
            damageTable = on ? table : DamageTables.Table.DEFLECT;
        }
    }

    private void facePlayer(World w) {
        faceRight = w.player.x > x;
    }

    @Override
    protected void fight(World w) {
        anim.tick();
        timer++;
        if (step != Step.INTRO && invincible == 0 && guarded) {
            damageTable = DamageTables.Table.DEFLECT;
        }
        if (warp != 0) {
            updateWarp(w);
            return;
        }
        switch (step) {
            case INTRO -> intro(w);
            case BOUNCE -> bounce(w);
            case SHIELD_THROW -> shieldThrow(w);
            case PLATES -> plates(w);
            case MIRROR -> mirrorStep(w);
            case TURRETS -> turrets(w);
        }
    }

    private void startWarp(int outAnim, int inAnim, int px, int py, boolean face) {
        anim.play(outAnim);
        boxes = false;
        warp = 1;
        warpIn = inAnim;
        warpX = px;
        warpY = py;
        warpFace = face;
    }

    private void updateWarp(World w) {
        if (anim.enteredFlag(3)) {
            w.sfx.play(2, 81);
        }
        if (anim.enteredFlag(2)) {
            w.sfx.play(2, 89);
        }
        if (anim.enteredFlag(1)) {
            if (warp == 1) {
                w.sfx.play(2, 89);
                x = rx(warpX);
                y = ry(warpY);
                faceRight = warpFace;
                anim.play(warpIn);
                warp = 2;
            } else {
                warp = 0;
                timer = 0;
                sub++;
            }
        }
    }

    private void think(World w) {
        boolean high = hp > maxHp / 2;
        if (mode == -1) {
            mode = 0;
            pattern = HIGH[w.rng.nextInt(16) <= 8 ? 0 : 1];
            patternIndex = 0;
        } else if (mode == 0 && !high) {
            mode = 1;
            int r = w.rng.nextInt(8);
            pattern = LOW[r <= 4 ? 0 : 1];
            patternIndex = 0;
        } else if (patternIndex >= pattern.length) {
            int r = w.rng.nextInt(16);
            pattern = mode == 0 ? HIGH[r <= 8 ? 0 : 1] : LOW[r <= 4 ? 0 : r <= 10 ? 1 : 2];
            patternIndex = 0;
        }
        int next = pattern[patternIndex++];
        step = switch (next) {
            case 3 -> Step.BOUNCE;
            case 4 -> Step.SHIELD_THROW;
            case 5 -> Step.PLATES;
            case 6 -> Step.MIRROR;
            default -> Step.TURRETS;
        };
        sub = 0;
        timer = 0;
        open(false);
        lungeBoxes = false;
    }

    private void intro(World w) {
        switch (sub) {
            case 0 -> {
                if (startWarning(w)) {
                    sub = 1;
                }
            }
            case 1 -> {
                if (warningOver(w)) {
                    x = rx(240);
                    y = ry(144);
                    w.add(new Flash(w, this, 4, x, y));
                    sub = 2;
                }
            }
            case 2 -> {
                if (flashDone) {
                    w.sfx.play(2, 80);
                    anim.play(0);
                    shown = true;
                    sub = 3;
                }
            }
            case 3 -> {
                if (anim.enteredFlag(1)) {
                    anim.play(1);
                    sub = 4;
                }
            }
            case 4 -> {
                if (anim.enteredFlag(1)) {
                    anim.play(5);
                    sub = 5;
                }
            }
            case 5 -> {
                if (anim.enteredFlag(1)) {
                    // TODO: the text box before the fight is not shown (docs/gaps.md, "Maverick text")
                    startMusic(w);
                    sub = 6;
                }
            }
            default -> {
                if (fillHp(w)) {
                    boxes = true;
                    anim.play(10);
                    open(false);
                    think(w);
                }
            }
        }
    }

    private void bounce(World w) {
        switch (sub) {
            case 0 -> {
                facePlayer(w);
                startWarp(13, 12, 160, 32, w.player.x > rx(160));
            }
            case 1 -> {
                anim.play(19);
                boxes = true;
                sub = 2;
            }
            case 2 -> {
                if (anim.enteredFlag(2)) {
                    w.sfx.play(2, 81);
                }
                if (anim.enteredFlag(1)) {
                    anim.play(1);
                    int row = w.player.x > rx(160) ? 0 : 1;
                    route = new int[8][];
                    for (int i = 0; i < 8; i++) {
                        route[i] = BOUNCE_ROUTE[row][i].clone();
                    }
                    followPlayer(w, route, 48, 144);
                    leg = 0;
                    sub = 3;
                }
            }
            case 3 -> {
                if (fly(route[leg], 6 + leg / 2f, 5)) {
                    w.sfx.play(2, 85);
                    // TODO: Rumble(6) at each point is not done (docs/gaps.md, "Screen shake")
                    if (leg == 2) {
                        anim.play(0);
                    }
                    if (++leg >= route.length) {
                        facePlayer(w);
                        anim.play(20);
                        sub = 4;
                    }
                }
            }
            default -> {
                if (anim.enteredFlag(1)) {
                    think(w);
                }
            }
        }
    }

    private void followPlayer(World w, int[][] points, int lo1, int hi1) {
        int px = (int) Math.floor((w.player.x - ROOM_X) / 16) * 16;
        for (int[] p : points) {
            if (p[1] == 176 && p[0] == 80 && w.rng.nextBoolean()) {
                p[0] = Math.max(lo1, Math.min(hi1, px));
            } else if (p[1] == 176 && p[0] == 240 && w.rng.nextBoolean()) {
                p[0] = Math.max(176, Math.min(272, px));
            }
        }
    }

    private boolean fly(int[] p, float speed, float near) {
        float tx = rx(p[0]);
        float ty = ry(p[1]);
        float dx = tx - x;
        float dy = ty - y;
        double d = Math.hypot(dx, dy);
        if (d <= speed) {
            x = tx;
            y = ty;
        } else {
            x += (float) (dx / d * speed);
            y += (float) (dy / d * speed);
        }
        return Math.abs(tx - x) <= near && Math.abs(ty - y) <= near;
    }

    private void throwSound(World w) {
        w.sfx.play(2, w.rng.nextBoolean() ? 34 : 35);
    }

    private void shieldThrow(World w) {
        switch (sub) {
            case 0 -> {
                boolean right = w.player.x >= rx(160);
                startWarp(13, 12, right ? 48 : 272, 144, right);
            }
            case 1 -> {
                anim.play(21);
                boxes = true;
                sub = 2;
            }
            case 2 -> {
                if (anim.enteredFlag(2)) {
                    throwSound(w);
                    w.sfx.play(2, 83);
                    w.add(new Shield(w, this, 0));
                }
                if (anim.enteredFlag(1)) {
                    anim.play(22);
                    sub = 3;
                }
            }
            case 3 -> {
                if (anim.enteredFlag(2)) {
                    throwSound(w);
                    w.sfx.play(2, 83);
                    w.add(new Shield(w, this, 1));
                    open(true);
                }
                if (anim.enteredFlag(1)) {
                    anim.play(10);
                    sub = 4;
                }
            }
            case 4 -> {
                if (act4 == 1) {
                    startWarp(17, 16, faceRight ? 256 : 64, 64, !faceRight);
                } else if (act4 == 3 || act4 == 4) {
                    catchShield(w);
                    sub = 6;
                }
            }
            case 5 -> {
                w.sfx.play(2, 82);
                anim.play(8);
                open(false);
                sub = 6;
            }
            case 6 -> {
                if (act4 == 4) {
                    w.sfx.play(2, 82);
                    anim.play(6);
                    open(false);
                    act4 = 0;
                    timer = 0;
                    sub = 7;
                }
            }
            default -> {
                if (timer >= 10) {
                    think(w);
                }
            }
        }
    }

    private void catchShield(World w) {
        w.sfx.play(2, 82);
        anim.play(8);
        open(false);
    }

    boolean throwDone() {
        return step == Step.SHIELD_THROW && sub >= 6;
    }

    private void plates(World w) {
        switch (sub) {
            case 0 -> startWarp(13, 12, 160, 64, true);
            case 1 -> {
                w.sfx.play(2, 86);
                anim.play(23);
                boxes = true;
                sub = 2;
            }
            case 2 -> {
                if (anim.enteredFlag(2)) {
                    w.sfx.play(2, 87);
                }
                if (anim.enteredFlag(1)) {
                    w.sfx.play(2, w.rng.nextBoolean() ? 36 : 37);
                    anim.play(10);
                    open(true);
                    freeze = 0;
                    act4 = 1;
                    for (int v = 16; v <= 19; v++) {
                        w.add(new Plate(w, this, v));
                    }
                    timer = 0;
                    sub = 3;
                }
            }
            case 3 -> {
                if (freeze > 0) {
                    freeze--;
                } else if (Math.abs(w.player.x - x) > 6 || Math.abs(w.player.y - y) > 6) {
                    double a = Math.atan2(w.player.y - y, w.player.x - x);
                    float dx = (float) Math.cos(a);
                    x += dx;
                    y += (float) Math.sin(a);
                    faceRight = dx > 0;
                }
                if (timer >= 600) {
                    anim.play(6);
                    open(false);
                    act4 = 0;
                    think(w);
                }
            }
            default -> {
            }
        }
    }

    private void mirrorStep(World w) {
        switch (sub) {
            case 0 -> startWarp(13, 12, 160, 64, true);
            case 1 -> {
                anim.play(6);
                facePlayer(w);
                boxes = true;
                timer = 0;
                sub = 2;
            }
            case 2 -> {
                if (timer >= 20) {
                    anim.play(8);
                    mirror = new Mirror(w, this);
                    w.add(mirror);
                    act4 = 1;
                    timer = 0;
                    sub = 3;
                }
            }
            case 3 -> {
                if (timer >= 20) {
                    act4 = 2;
                    int p = (int) (w.player.x - ROOM_X);
                    int imageX;
                    boolean imageRight;
                    if (p <= 160) {
                        imageX = Math.max(p - 48, 40);
                        imageRight = true;
                    } else {
                        imageX = Math.min(p + 48, 264);
                        imageRight = false;
                    }
                    int yy = w.rng.nextBoolean() ? 48 : 160;
                    startWarp(15, 14, 320 - imageX, yy, !imageRight);
                }
            }
            case 4 -> {
                act4 = 3;
                if (y == ry(48)) {
                    anim.play(22);
                    sub = 5;
                } else {
                    throwSound(w);
                    w.sfx.play(2, 84);
                    anim.play(29);
                    open(true);
                    lungeBoxes = true;
                    sub = 7;
                }
            }
            case 5 -> {
                if (anim.enteredFlag(2)) {
                    throwSound(w);
                    w.sfx.play(2, 83);
                    open(true);
                    MirrorShield s = new MirrorShield(w, this);
                    w.add(s);
                    w.add(new MirrorShield.Copy(w, s));
                }
                if (anim.enteredFlag(1)) {
                    anim.play(10);
                    sub = 6;
                }
            }
            case 6 -> {
                if (act4 == 4) {
                    w.sfx.play(2, 82);
                    anim.play(8);
                    open(false);
                    timer = 0;
                    sub = 8;
                }
            }
            case 7 -> {
                if (anim.enteredFlag(1)) {
                    open(false);
                    lungeBoxes = false;
                    timer = 0;
                    sub = 8;
                }
            }
            case 8 -> {
                if (timer >= 30) {
                    w.sfx.play(2, 82);
                    anim.play(6);
                    act4 = 0;
                    timer = 0;
                    sub = 9;
                }
            }
            default -> {
                if (timer >= 20) {
                    think(w);
                }
            }
        }
    }

    private void turrets(World w) {
        switch (sub) {
            case 0 -> startWarp(13, 12, 160, 80, true);
            case 1 -> {
                anim.play(30);
                facePlayer(w);
                boxes = true;
                sub = 2;
            }
            case 2 -> {
                if (anim.enteredFlag(2)) {
                    w.add(new Flash(w, null, 31, x, y));
                }
                if (anim.enteredFlag(3)) {
                    w.sfx.play(2, w.rng.nextBoolean() ? 38 : 39);
                    act4 = 1;
                    for (int v = 48; v <= 51; v++) {
                        w.add(new Turret(w, this, v));
                    }
                }
                if (anim.enteredFlag(1)) {
                    open(true);
                    dashes = 0;
                    firstWarp = true;
                    anim.play(17);
                    boxes = false;
                    sub = 3;
                }
            }
            case 3 -> {
                if (anim.enteredFlag(2)) {
                    w.sfx.play(2, firstWarp ? 89 : 90);
                }
                if (anim.enteredFlag(1)) {
                    firstWarp = false;
                    int c = w.rng.nextInt(4);
                    x = rx(CORNERS[c][0]);
                    y = ry(CORNERS[c][1]);
                    faceRight = c == 1 || c == 2;
                    dashTarget = (c + 1 + w.rng.nextInt(3)) & 3;
                    timer = 0;
                    sub = 4;
                }
            }
            case 4 -> {
                if (timer >= 50) {
                    anim.play(16);
                    sub = 5;
                }
            }
            case 5 -> {
                if (anim.enteredFlag(2)) {
                    w.sfx.play(2, 90);
                }
                if (anim.enteredFlag(1)) {
                    boxes = true;
                    int[] t = CORNERS[dashTarget];
                    float dx = rx(t[0]) - x;
                    float dy = ry(t[1]) - y;
                    if (Math.abs(dx) < 1) {
                        anim.play(dy < 0 ? 39 : 40);
                        faceRight = x < rx(160);
                    } else {
                        anim.play(dy > 1 ? 38 : 37);
                        faceRight = dx > 0;
                    }
                    sub = 6;
                }
            }
            default -> {
                // TODO: the dash is 5 px/update on Hard; the difficulty is Normal here (docs/gaps.md, "Shield Sheldon")
                if (fly(CORNERS[dashTarget], 4, 3)) {
                    dashes++;
                    if (dashes < 4) {
                        anim.play(17);
                        w.sfx.play(2, 90);
                        boxes = false;
                        sub = 3;
                    } else {
                        anim.play(6);
                        open(false);
                        act4 = 0;
                        think(w);
                    }
                }
            }
        }
    }

    @Override
    protected void onDeathStart(World w) {
        anim.play(18);
        w.sfx.play(2, 32);
        act4 = 0;
    }

    Rect normalHurt() {
        return new Rect(x - 16, y - 16, 32, 32);
    }

    @Override
    public Rect hurtBox() {
        if (!boxes || dying()) {
            return null;
        }
        return lungeBoxes ? lunge(-20, -20, 40, 40) : normalHurt();
    }

    @Override
    public Rect contactBox() {
        if (!boxes || dying()) {
            return null;
        }
        return lungeBoxes ? lunge(-32, -24, 48, 40) : new Rect(x - 14, y - 14, 28, 28);
    }

    private Rect lunge(int bx, int by, int bw, int bh) {
        float left = faceRight ? x - bx - bw : x + bx;
        return new Rect(left, y + by, bw, bh);
    }

    @Override
    public int drawPriority() {
        return step == Step.TURRETS ? 6 : 5;
    }

    void flashDone() {
        flashDone = true;
    }

    static final class Flash extends Entity {

        private final ShieldSheldon boss;

        Flash(World w, ShieldSheldon boss, int animIndex, float x, float y) {
            this.boss = boss;
            this.x = x;
            this.y = y;
            this.anim = new Animator(w.animSet(SET));
            anim.play(animIndex);
        }

        @Override
        public void update(World w) {
            anim.tick();
            if (anim.enteredFlag(2)) {
                w.sfx.play(2, 79);
                if (boss != null) {
                    boss.flashDone();
                }
            }
            if (anim.finished() || anim.enteredFlag(1) && anim.atLastFrame()) {
                dead = true;
            }
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
            return 4;
        }
    }

    abstract static class Helper extends Entity {

        final ShieldSheldon boss;

        Helper(World w, ShieldSheldon boss) {
            this.boss = boss;
            this.anim = new Animator(w.animSet(SET));
        }

        boolean ended() {
            return !boss.active() || boss.act4 == 0;
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
            return 5;
        }
    }

    static final class Shield extends Helper {

        private final int var;
        private int phase;
        private final boolean right;

        Shield(World w, ShieldSheldon boss, int var) {
            super(w, boss);
            this.var = var;
            this.right = boss.faceRight;
            this.faceRight = right;
            this.x = boss.x + (right ? 16 : -16);
            this.y = boss.y;
            this.contactDamage = 8;
            this.damageTable = DamageTables.Table.DEFLECT;
            anim.play(3);
        }

        private boolean flyTo(float tx, float ty, float speed) {
            float dx = tx - x;
            float dy = ty - y;
            double d = Math.hypot(dx, dy);
            if (d <= speed) {
                x = tx;
                y = ty;
                return true;
            }
            x += (float) (dx / d * speed);
            y += (float) (dy / d * speed);
            return false;
        }

        @Override
        public void update(World w) {
            if (!boss.active()) {
                dead = true;
                return;
            }
            anim.tick();
            if (var == 0) {
                first(w);
            } else {
                second(w);
            }
            if (w.offScreen(x, y, 32, 32)) {
                dead = true;
            }
        }

        private void first(World w) {
            switch (phase) {
                case 0 -> {
                    if (flyTo(boss.rx(right ? 288 : 32), y, 2)) {
                        phase = 1;
                    }
                }
                case 1 -> {
                    if (flyTo(boss.rx(right ? 112 : 208), y, 2)) {
                        boss.act4 = w.rng.nextBoolean() ? 1 : 2;
                        phase = boss.act4 == 1 ? 2 : 3;
                    }
                }
                case 2 -> {
                    if (flyTo(boss.rx(right ? 64 : 272), boss.ry(64), 4)) {
                        boss.act4 = 4;
                        dead = true;
                    }
                }
                default -> {
                    if (flyTo(boss.rx(right ? 48 : 272), y, 2)) {
                        boss.act4 = 3;
                        dead = true;
                    }
                }
            }
        }

        private void second(World w) {
            switch (phase) {
                case 0 -> {
                    flyTo(boss.rx(right ? 272 : 64), boss.ry(64), 4);
                    if (boss.act4 == 1 && boss.throwDone()) {
                        dead = true;
                    } else if (boss.act4 == 2) {
                        phase = 1;
                    }
                }
                default -> {
                    if (flyTo(boss.rx(right ? 48 : 272), boss.ry(144), 4)) {
                        boss.act4 = 4;
                        dead = true;
                    }
                }
            }
        }

        @Override
        public Rect hurtBox() {
            return new Rect(x - 16, y - 12, 32, 28);
        }

        @Override
        public Rect contactBox() {
            return hurtBox();
        }

        @Override
        public int drawPriority() {
            return 4;
        }
    }

    static final class Plate extends Helper {

        private static final int[][] OFFSET = {{0, -38}, {-38, 0}, {0, 38}, {38, 0}};
        private final int var;
        private int state;
        private int timer;

        Plate(World w, ShieldSheldon boss, int var) {
            super(w, boss);
            this.var = var;
            this.hp = 1000;
            this.damageTable = DamageTables.Table.NORMAL;
            anim.play(vertical() ? 24 : 25);
            follow();
        }

        private boolean vertical() {
            return var == 16 || var == 18;
        }

        private void follow() {
            x = boss.x + OFFSET[var - 16][0];
            y = boss.y + OFFSET[var - 16][1];
        }

        @Override
        public void update(World w) {
            if (ended()) {
                dead = true;
                return;
            }
            hp = 1000;
            anim.tick();
            follow();
            switch (state) {
                case 1 -> {
                    if (++timer >= 30) {
                        boss.freeze = 8;
                        anim.play(27);
                        state = 2;
                    }
                }
                case 2 -> {
                    if (anim.enteredFlag(1)) {
                        int[] o = OFFSET[var - 16];
                        float vx = Math.signum(o[0]) * 2;
                        float vy = Math.signum(o[1]) * 2;
                        w.sfx.play(2, 88);
                        // TODO: the plate shot is 3 px/update on Hard and can be destroyed by weapons with +0x9C set; neither is done (docs/gaps.md, "Shield Sheldon")
                        w.add(new Projectile(w, SET, 28, x, y, vx, vy, 4, 4, 4).throughTerrain().endsOnPlayer()
                                .priority(4));
                        timer = 0;
                        state = 3;
                    }
                }
                case 3 -> {
                    if (++timer >= 30) {
                        anim.play(vertical() ? 24 : 25);
                        state = 0;
                    }
                }
                default -> {
                }
            }
        }

        @Override
        public void onHurt(World w, int weaponId) {
            hp = 1000;
            if (state == 0) {
                anim.play(26);
                timer = 0;
                state = 1;
            }
        }

        @Override
        public Rect hurtBox() {
            if (state != 0) {
                return new Rect(x - 8, y - 8, 16, 16);
            }
            return vertical() ? new Rect(x - 16, y - 8, 32, 16) : new Rect(x - 8, y - 16, 16, 32);
        }
    }

    static final class Mirror extends Helper {

        Mirror(World w, ShieldSheldon boss) {
            super(w, boss);
            this.hp = 1000;
            this.damageTable = DamageTables.Table.NORMAL;
            w.sfx.play(2, 101);
            this.faceRight = !boss.faceRight;
            this.x = boss.x + (boss.faceRight ? 32 : -32);
            this.y = boss.y;
            anim.play(8);
        }

        @Override
        public void update(World w) {
            if (ended()) {
                dead = true;
                return;
            }
            hp = 1000;
            anim.play(boss.anim().anim(), boss.anim().frameIndex());
            faceRight = !boss.faceRight;
            if (boss.act4 >= 3) {
                x = 2 * ROOM_X + 320 - boss.x;
                y = boss.y;
            }
        }

        @Override
        public Rect hurtBox() {
            return boss.hurtBox() == null ? null : new Rect(x - 16, y - 16, 32, 32);
        }

        @Override
        public boolean visible() {
            return boss.visible() && (boss.fightTimer & 1) == 0;
        }
    }

    static final class MirrorShield extends Helper {

        private static final int[][][] ROUTE = {
                {{80, 176}, {16, 64}, {64, 0}, {240, 176}, {304, 64}, {256, 0}},
                {{240, 176}, {304, 64}, {256, 0}, {80, 176}, {16, 64}, {64, 0}}};

        private final int[][] points;
        private int leg;

        MirrorShield(World w, ShieldSheldon boss) {
            super(w, boss);
            this.contactDamage = 8;
            this.damageTable = DamageTables.Table.DEFLECT;
            this.x = boss.x + (boss.faceRight ? 16 : -16);
            this.y = boss.y;
            int row = boss.faceRight ? 1 : 0;
            points = new int[7][];
            for (int i = 0; i < 6; i++) {
                points[i] = ROUTE[row][i].clone();
            }
            points[6] = new int[] {(int) (320 - (x - ROOM_X)), (int) (y - ROOM_Y)};
            boss.followPlayer(w, points, 32, 144);
            anim.play(3);
        }

        @Override
        public void update(World w) {
            if (!boss.active()) {
                dead = true;
                return;
            }
            anim.tick();
            float speed = 6 + leg / 2f;
            float tx = boss.rx(points[leg][0]);
            float ty = boss.ry(points[leg][1]);
            float dx = tx - x;
            float dy = ty - y;
            double d = Math.hypot(dx, dy);
            if (d <= speed) {
                x = tx;
                y = ty;
                if (leg < points.length - 1) {
                    w.sfx.play(2, 85);
                }
                if (++leg >= points.length) {
                    boss.act4 = 4;
                    dead = true;
                }
            } else {
                x += (float) (dx / d * speed);
                y += (float) (dy / d * speed);
            }
        }

        @Override
        public Rect hurtBox() {
            return new Rect(x - 16, y - 12, 32, 28);
        }

        @Override
        public Rect contactBox() {
            return hurtBox();
        }

        @Override
        public int drawPriority() {
            return 4;
        }

        static final class Copy extends Entity {

            private final MirrorShield source;

            Copy(World w, MirrorShield source) {
                this.source = source;
                this.contactDamage = 8;
                this.damageTable = DamageTables.Table.DEFLECT;
                this.anim = new Animator(w.animSet(SET));
                anim.play(3);
                follow();
            }

            private void follow() {
                x = 2 * ROOM_X + 320 - source.x;
                y = source.y;
                faceRight = !source.faceRight;
            }

            @Override
            public void update(World w) {
                if (source.dead) {
                    dead = true;
                    return;
                }
                anim.tick();
                follow();
            }

            @Override
            public Rect hurtBox() {
                return new Rect(x - 16, y - 12, 32, 28);
            }

            @Override
            public Rect contactBox() {
                return hurtBox();
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
                return 4;
            }
        }
    }

    static final class Turret extends Helper {

        private final int var;
        private boolean placed;
        private int flashTimer;

        Turret(World w, ShieldSheldon boss, int var) {
            super(w, boss);
            this.var = var;
            this.x = boss.x;
            this.y = boss.y;
            this.hp = 1000;
            this.damageTable = DamageTables.Table.NORMAL;
            anim.play(2);
        }

        @Override
        public void update(World w) {
            if (ended()) {
                dead = true;
                return;
            }
            hp = 1000;
            anim.tick();
            if (!placed) {
                int[] c = CORNERS[var - 48];
                float tx = boss.rx(c[0]);
                float ty = boss.ry(c[1]);
                float dx = tx - x;
                float dy = ty - y;
                double d = Math.hypot(dx, dy);
                if (d <= 6) {
                    x = tx;
                    y = ty;
                    placed = true;
                    anim.play(c[1] < 100 ? 35 : 36);
                    faceRight = c[0] < 160;
                } else {
                    x += (float) (dx / d * 6);
                    y += (float) (dy / d * 6);
                }
                return;
            }
            if (flashTimer > 0) {
                flash = 1;
                if (--flashTimer == 0) {
                    w.sfx.play(2, 88);
                    float[] v = Projectile.aim(x, y, w.player.x, w.player.y, 2);
                    // TODO: the turret shot is 4 px/update on Hard; the difficulty is Normal here (docs/gaps.md, "Shield Sheldon")
                    w.add(new Projectile(w, SET, 34, x, y, v[0], v[1], 8, 4, 4).endsOnPlayer().priority(4));
                }
            }
        }

        @Override
        public void onHurt(World w, int weaponId) {
            hp = 1000;
            if (placed && flashTimer == 0) {
                w.add(new Flash(w, null, 33, x, y));
                flashTimer = 7;
            }
        }

        @Override
        public Rect hurtBox() {
            return new Rect(x - 16, y - 16, 32, 32);
        }
    }
}
