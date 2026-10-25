package mmx.x6.game.enemy.turtloid;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.boss.Maverick;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.AnimFx;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class RainyTurtloid extends Maverick {

    public enum Step { INTRO, THINK, MISSILES, SPIN_ROLL, BUBBLE_ROLL }

    static final String SET = "rainy-turtloid";
    private static final int MISSILE_COUNT = 10;
    private static final float ROLL = 3;
    private static final int[][] SPIN_CIRCLE = new int[16][];

    static {
        for (int i = 0; i < 16; i++) {
            double a = Math.PI * 2 * i / 16;
            SPIN_CIRCLE[i] = new int[] {(int) Math.round(Math.cos(a) * 24), (int) Math.round(Math.sin(a) * 24)};
        }
    }

    final float roomX;
    final float roomY;
    final Crystal[] crystals = new Crystal[2];
    int lostBits;
    private Step step = Step.INTRO;
    private Step[] pattern;
    private int patternIndex;
    private int sub;
    private int timer;
    private int count;
    private int fireCrystal;
    private float vx;
    private float vy;
    private float gravity;
    private boolean leftHit;
    private boolean rightHit;

    public RainyTurtloid(World w, RecordDef r) {
        super(w, r, SET, 56, DamageTables.Table.TURTLOID);
        roomX = w.camera.targetMinX();
        roomY = w.camera.targetMinY();
        x = roomX + 240;
        y = roomY - 48;
        contactDamage = 6;
        anim.play(31);
        for (int i = 0; i < 2; i++) {
            crystals[i] = new Crystal(w, this, i);
            w.add(crystals[i]);
        }
        w.add(new Shell(this));
    }

    public Step step() {
        return step;
    }

    float floorY() {
        return roomY + 240 - 64;
    }

    float ceilingY() {
        return roomY + 16 + 64;
    }

    float leftX() {
        return roomX + 16 + 64;
    }

    float rightX() {
        return roomX + 400 - 64;
    }

    float middleX() {
        return roomX + 208;
    }

    boolean shellOpen() {
        int a = anim.anim();
        return step != Step.INTRO && (a == 10 || a == 19);
    }

    private boolean shellAnim() {
        int a = anim.anim();
        return a == 13 || a == 16 || a == 28 || a == 29 || a == 30 || a == 31;
    }

    boolean bothLost() {
        return lostBits == 3;
    }

    private void facePlayer(World w) {
        faceRight = x < w.player.x;
    }

    @Override
    protected void fight(World w) {
        anim.tick();
        switch (step) {
            case INTRO -> updateIntro(w);
            case THINK -> think(w);
            case MISSILES -> updateMissiles(w);
            default -> updateRoll(w);
        }
    }

    private void updateIntro(World w) {
        timer++;
        switch (sub) {
            case 0 -> {
                if (startWarning(w)) {
                    sub = 1;
                }
            }
            case 1 -> {
                if (warningOver(w)) {
                    shown = true;
                    facePlayer(w);
                    anim.play(31);
                    sub = 2;
                    timer = 0;
                }
            }
            case 2 -> {
                if (y < floorY()) {
                    y = Math.min(floorY(), y + 1);
                }
                if (timer % 20 == 0) {
                    w.sfx.play(2, 56);
                }
                if (y >= floorY() && anim.sprite() == 17) {
                    anim.play(4);
                    sub = 3;
                    timer = 0;
                }
            }
            case 3 -> {
                if (timer == 28) {
                    w.sfx.play(2, 57);
                }
                if (timer >= 100) {
                    // TODO: Rainy Turtloid's text box before the fight is not shown (docs/gaps.md, "Maverick text")
                    anim.play(7);
                    sub = 4;
                    timer = 0;
                }
            }
            case 4 -> {
                if (timer == 13) {
                    w.sfx.play(2, 57);
                }
                if (anim.finished()) {
                    anim.play(10);
                    startMusic(w);
                    sub = 5;
                    timer = 0;
                }
            }
            case 5 -> {
                if (timer >= 60) {
                    sub = 6;
                }
            }
            default -> {
                if (fillHp(w)) {
                    step = Step.THINK;
                }
            }
        }
    }

    private void think(World w) {
        int r = w.rng.nextInt(16);
        boolean high = hp > maxHp / 2;
        // TODO: the Hard choices (jump attacks, RAIN ROLL) are not made; difficulty is Normal (docs/gaps.md, "Rainy Turtloid")
        Step attack = high || r < 8 ? Step.SPIN_ROLL : Step.BUBBLE_ROLL;
        pattern = new Step[] {Step.MISSILES, attack};
        patternIndex = 0;
        start(pattern[0], w);
    }

    private void start(Step s, World w) {
        step = s;
        sub = 0;
        timer = 0;
        count = 0;
        if (s == Step.MISSILES) {
            facePlayer(w);
            sub = 2;
        }
    }

    private void next(World w) {
        patternIndex++;
        if (patternIndex >= pattern.length) {
            step = Step.THINK;
            return;
        }
        start(pattern[patternIndex], w);
    }

    private void updateMissiles(World w) {
        timer++;
        if (sub == 2) {
            if (timer % 8 != 0) {
                return;
            }
            if (count == MISSILE_COUNT) {
                anim.play(10);
                sub = 3;
                timer = 0;
                return;
            }
            if (count == 0) {
                w.sfx.play(2, 34);
                anim.play(19);
            } else {
                crystals[w.rng.nextInt(2)].fireMissile(w);
            }
            count++;
            return;
        }
        if (timer >= 300) {
            next(w);
        }
    }

    private void updateRoll(World w) {
        timer++;
        switch (sub) {
            case 0 -> {
                facePlayer(w);
                w.sfx.play(2, step == Step.SPIN_ROLL ? 35 : 36);
                anim.play(13);
                sub = 1;
            }
            case 1 -> {
                facePlayer(w);
                anim.play(28);
                w.sfx.play(2, 61);
                sub = 2;
                timer = 0;
            }
            case 2 -> {
                if (anim.anim() == 28 && anim.finished()) {
                    w.sfx.play(2, 62);
                    anim.play(30);
                    timer = 0;
                } else if (anim.anim() == 30) {
                    facePlayer(w);
                    if (timer >= 60) {
                        count = 0;
                        sub = 3;
                    }
                }
            }
            case 3 -> {
                if (count >= 4) {
                    sub = 9;
                    break;
                }
                if (step == Step.BUBBLE_ROLL) {
                    faceRight = x < middleX();
                    vx = 0;
                    sub = 4;
                } else {
                    facePlayer(w);
                    if (Math.abs(w.player.x - x) < 64) {
                        vy = -3;
                        sub = 6;
                    } else {
                        vx = faceRight ? ROLL : -ROLL;
                        sub = 4;
                    }
                }
            }
            case 4 -> {
                if (step == Step.BUBBLE_ROLL) {
                    vx = Math.max(-5, Math.min(5, vx + (faceRight ? 1 : -1)));
                    if (timer % 14 == 0) {
                        fireCrystal = w.rng.nextInt(2);
                        crystals[fireCrystal].fireBubble(w);
                    }
                }
                x += vx;
                if (x <= leftX() || x >= rightX()) {
                    boolean right = x >= rightX();
                    x = clampX(x);
                    vx = 0;
                    impact(w);
                    if (step == Step.BUBBLE_ROLL) {
                        hitWall(w, right);
                    }
                    count++;
                    sub = 5;
                    timer = 0;
                }
            }
            case 5 -> {
                if (timer >= (step == Step.BUBBLE_ROLL ? 50 : 60)) {
                    restoreWalls(w);
                    sub = 3;
                }
            }
            case 6 -> {
                y += vy;
                if (y <= ceilingY()) {
                    y = ceilingY();
                    vy = 0;
                    impact(w);
                    count++;
                    sub = 7;
                    timer = 0;
                }
            }
            case 7 -> {
                if (timer >= 60) {
                    if (Math.abs(w.player.x - x) >= 64) {
                        vx = x < middleX() ? 4 : -4;
                    } else {
                        vx = 0;
                        facePlayer(w);
                    }
                    gravity = 0.0625f;
                    vy = 0;
                    sub = 8;
                }
            }
            case 8 -> {
                vy += gravity;
                x = clampX(x + vx);
                y += vy;
                if (y >= floorY()) {
                    y = floorY();
                    vx = vy = gravity = 0;
                    impact(w);
                    count++;
                    sub = 5;
                    timer = 0;
                }
            }
            case 9 -> {
                if (anim.anim() != 30 || anim.sprite() == 0) {
                    facePlayer(w);
                    w.sfx.play(2, 20);
                    w.sfx.play(2, 64);
                    anim.play(29);
                    sub = 10;
                }
            }
            case 10 -> {
                if (anim.finished()) {
                    anim.play(16);
                    sub = 11;
                }
            }
            case 11 -> {
                if (anim.finished()) {
                    anim.play(10);
                    sub = 12;
                    timer = 0;
                }
            }
            default -> {
                if (timer >= 270) {
                    next(w);
                }
            }
        }
    }

    private void impact(World w) {
        // TODO: the impacts' Rumble(3) and Quake(90, 2, 3) screen shake are not done (docs/gaps.md, "Screen shake")
        w.sfx.play(2, 63);
    }

    private void hitWall(World w, boolean right) {
        int rows = 14;
        int top = (int) roomY + 16;
        int src = right ? (int) roomX + 400 : (int) roomX;
        w.copyTiles(src, 976, 1, rows, src, top);
        if (right) {
            rightHit = true;
        } else {
            leftHit = true;
        }
    }

    private void restoreWalls(World w) {
        int top = (int) roomY + 16;
        if (leftHit) {
            w.copyTiles((int) roomX + 16, 976, 1, 14, (int) roomX, top);
            leftHit = false;
        }
        if (rightHit) {
            w.copyTiles((int) roomX + 416, 976, 1, 14, (int) roomX + 400, top);
            rightHit = false;
        }
    }

    private float clampX(float v) {
        return Math.max(leftX(), Math.min(rightX(), v));
    }

    int[] spinPoint(boolean opposite) {
        int i = (anim.frameIndex() + (opposite ? 8 : 0)) % 16;
        return SPIN_CIRCLE[i];
    }

    @Override
    protected void onDeathStart(World w) {
        anim.play(32);
        w.sfx.play(2, 20);
        w.sfx.play(2, 32);
        vx = vy = gravity = 0;
        restoreWalls(w);
    }

    boolean over() {
        return dying() || dead;
    }

    @Override
    public Rect hurtBox() {
        if (step == Step.INTRO || dying()) {
            return null;
        }
        if (bothLost() || shellAnim()) {
            return new Rect(x - 48, y - 48, 96, 96);
        }
        return faceRight ? new Rect(x - 72, y - 48, 24, 96) : new Rect(x + 48, y - 48, 24, 96);
    }

    Rect shellBox() {
        if (step == Step.INTRO || dying() || bothLost() || shellAnim()) {
            return null;
        }
        return faceRight ? new Rect(x - 48, y - 48, 32, 96) : new Rect(x + 16, y - 48, 32, 96);
    }

    @Override
    public Rect contactBox() {
        if (step == Step.INTRO || dying()) {
            return null;
        }
        if (anim.anim() == 30) {
            return new Rect(x - 60, y - 60, 120, 120);
        }
        return faceRight ? new Rect(x - 64, y - 40, 104, 104) : new Rect(x - 40, y - 40, 104, 104);
    }

    private static final class Shell extends Entity {

        private final RainyTurtloid boss;

        Shell(RainyTurtloid boss) {
            this.boss = boss;
            this.damageTable = DamageTables.Table.DEFLECT;
        }

        @Override
        public void update(World w) {
            if (boss.dead || boss.over()) {
                dead = true;
            }
        }

        @Override
        public Rect hurtBox() {
            return boss.shellBox();
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

    static final class Crystal extends Entity {

        private static final int[] FOLLOW = {
                -1, -1, -1, -1, 5, -1, -1, 8, -1, -1, 11, -1, -1, 14, -1, -1, 17, -1, -1, 20, -1, -1, -1, -1, -1, -1,
                -1, -1, 2, 2, 0, 0, -1, -1};

        private final RainyTurtloid boss;
        private final int index;
        private int lostTimer;

        Crystal(World w, RainyTurtloid boss, int index) {
            this.boss = boss;
            this.index = index;
            this.hp = 16;
            this.damageTable = DamageTables.Table.TURTLOID;
            this.anim = new Animator(w.animSet(SET));
        }

        boolean lost() {
            return (boss.lostBits & (1 << index)) != 0;
        }

        @Override
        public void update(World w) {
            if (boss.dead || boss.over()) {
                dead = true;
                return;
            }
            x = boss.x;
            y = boss.y;
            faceRight = boss.faceRight;
            int a = boss.anim().anim();
            int mine = a >= 0 && a < FOLLOW.length && FOLLOW[a] >= 0 ? FOLLOW[a] + index : -1;
            if (mine >= 0 && anim.anim() != mine) {
                anim.play(mine, Math.min(boss.anim().frameIndex(), 15));
            }
            anim.tick();
            if (lost() && boss.bothLost()) {
                if (++lostTimer >= 660) {
                    hp = 16;
                    boss.lostBits &= ~(1 << index);
                    w.sfx.play(2, 65);
                    lostTimer = 0;
                }
            }
        }

        @Override
        public void onKilled(World w, int weaponId) {
            hp = 0;
            w.add(new Explosion(w, x, y + (index == 0 ? -22 : 18)));
            boss.lostBits |= 1 << index;
            lostTimer = 0;
        }

        void fireMissile(World w) {
            float dir = boss.faceRight ? 1 : -1;
            w.add(new Missile(w, boss, boss.x + dir * 8, boss.y + (index == 0 ? -24 : 16), index == 0));
        }

        void fireBubble(World w) {
            int[] p = boss.spinPoint(index == 1);
            float bx = boss.x + p[0];
            float by = boss.y + p[1];
            w.sfx.play(2, 58);
            w.add(new Bubble(w, boss, bx, by, index == 0 ? 2 : -2, -(5 + w.rng.nextInt(4))));
            w.add(new AnimFx(w, SET, 24, bx, by, false));
        }

        @Override
        public Rect hurtBox() {
            if (lost() || !boss.shellOpen()) {
                return null;
            }
            return index == 0 ? new Rect(x - 16, y - 30, 32, 16) : new Rect(x - 16, y + 10, 32, 16);
        }

        @Override
        public boolean visible() {
            if (anim.anim() < 0 || !boss.visible()) {
                return false;
            }
            if (lost()) {
                return boss.bothLost() && lostTimer >= 600 && (lostTimer & 2) == 0;
            }
            return true;
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

    private static final class Missile extends Projectile {

        private final RainyTurtloid boss;
        private int phase;
        private int timer;

        Missile(World w, RainyTurtloid boss, float x, float y, boolean up) {
            super(w, SET, 22, x, y, 0, 0, 4, 14, 5);
            this.boss = boss;
            hittable(4, DamageTables.Table.TURTLOID);
            explodes();
            endsOnPlayer();
            priority(3);
            double a = Math.toRadians(new int[] {0, 40, 80}[w.rng.nextInt(3)]);
            float dir = boss.faceRight ? 1 : -1;
            vx = (float) Math.cos(a) * dir;
            vy = (float) Math.sin(a) * (up ? -1 : 1);
            facing(boss.faceRight);
        }

        @Override
        public void update(World w) {
            if (boss.over()) {
                dead = true;
                return;
            }
            timer++;
            if (phase == 0 && timer >= 24) {
                vx = vy = 0;
                phase = 1;
                timer = 0;
            } else if (phase == 1 && timer >= 40) {
                w.sfx.play(2, 60);
                facing(w.player.x > x);
                vx = faceRight ? 4 : -4;
                phase = 2;
            } else if (phase == 2) {
                float dy = w.player.y - y;
                vy = Math.abs(dy) < 0.5f ? 0 : Math.signum(dy) * 0.5f;
            }
            super.update(w);
        }
    }

    private static final class Bubble extends Projectile {

        private final RainyTurtloid boss;

        Bubble(World w, RainyTurtloid boss, float x, float y, float vx, float vy) {
            super(w, SET, 23, x, y, vx, vy, 4, 14, 14);
            this.boss = boss;
            hittable(4, DamageTables.Table.TURTLOID);
            throughTerrain();
            endsOnPlayer();
            gravity(0.1875f, 8);
            margin(400);
            priority(3);
        }

        @Override
        public void update(World w) {
            if (boss.over() || y > boss.floorY() + 64 + 64) {
                dead = true;
                return;
            }
            super.update(w);
        }

        @Override
        protected void end(World w) {
            dead = true;
        }
    }
}
