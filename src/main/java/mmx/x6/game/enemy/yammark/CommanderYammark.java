package mmx.x6.game.enemy.yammark;

import java.util.ArrayList;
import java.util.List;
import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.boss.Maverick;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class CommanderYammark extends Maverick {

    public enum Step { INTRO, THINK, BOMBARD, DASH, WALL_DASH, CHASE }

    public static final int MODE_KEEP = 0;
    public static final int MODE_BACK = 1;
    public static final int MODE_CIRCLE = 2;
    public static final int MODE_BOMBARD = 3;
    public static final int MODE_WALL = 5;
    public static final int MODE_TRAIL = 6;

    static final String SET = "commander-yammark";
    private static final int[][] PAIRS = {{3, 1}, {4, 2}, {6, 3}};
    private static final Step[][] PATTERNS = {
            {Step.BOMBARD, Step.DASH},
            {Step.BOMBARD, Step.DASH, Step.DASH},
            {Step.CHASE, Step.WALL_DASH},
            {Step.CHASE, Step.DASH, Step.DASH}};
    static final int HISTORY = 210;

    final float roomX;
    final float roomY;
    final List<YammarkDrone> drones = new ArrayList<>();
    final float[] histX = new float[HISTORY];
    final float[] histY = new float[HISTORY];
    int histHead;
    int chaseTime;
    int mode = MODE_KEEP;
    int pair;
    double circleAngle;
    boolean dashing;
    int dashCount;
    private Step step = Step.INTRO;
    private Step[] pattern;
    private int patternIndex;
    private int sub;
    private int timer;
    private float vx;
    private float vy;
    private int walls;
    private Wings wings;

    public CommanderYammark(World w, RecordDef r) {
        super(w, r, SET, 32, DamageTables.Table.YAMMARK);
        roomX = w.camera.targetMinX();
        roomY = w.camera.targetMinY();
        x = roomX + 240;
        y = roomY - 32;
        contactDamage = 4;
        anim.play(0);
    }

    public Step step() {
        return step;
    }

    float floorY() {
        return roomY + 192 - 40;
    }

    float ceilingY() {
        return roomY + 24;
    }

    float leftX() {
        return roomX + 32;
    }

    float rightX() {
        return roomX + 320;
    }

    int droneCount() {
        return PAIRS[pair][0];
    }

    int stayBack() {
        return PAIRS[pair][1];
    }

    @Override
    protected void fight(World w) {
        anim.tick();
        circleAngle += (faceRight ? 1 : -1) * Math.toRadians(3);
        drones.removeIf(d -> d.dead);
        switch (step) {
            case INTRO -> updateIntro(w);
            case THINK -> think(w);
            default -> updateAttack(w);
        }
        histX[histHead] = x;
        histY[histHead] = y;
        histHead = (histHead + 1) % HISTORY;
    }

    private void updateIntro(World w) {
        switch (sub) {
            case 0 -> {
                if (startWarning(w)) {
                    sub = 1;
                }
            }
            case 1 -> {
                if (warningOver(w)) {
                    shown = true;
                    wings = new Wings(w, this);
                    w.add(wings);
                    anim.play(0);
                    sub = 2;
                }
            }
            case 2 -> {
                y += 1;
                if (y >= floorY()) {
                    y = floorY();
                    for (int i = 0; i < droneCount(); i++) {
                        summon(w, i, true);
                    }
                    sub = 3;
                }
            }
            case 3 -> {
                if (!drones.isEmpty() && drones.stream().allMatch(YammarkDrone::inPlace)) {
                    mode = MODE_CIRCLE;
                    // TODO: Yammark's text box before the fight is not shown (docs/gaps.md, "Maverick text")
                    anim.play(1);
                    sub = 4;
                }
            }
            case 4 -> {
                if (anim.finished()) {
                    anim.play(0);
                    startMusic(w);
                    sub = 5;
                }
            }
            default -> {
                if (fillHp(w)) {
                    step = Step.THINK;
                }
            }
        }
    }

    private void updatePair() {
        if (hp < maxHp / 3) {
            pair = 2;
        } else if (hp < maxHp * 2 / 3) {
            pair = Math.max(pair, 1);
        }
    }

    private void think(World w) {
        updatePair();
        int r = w.rng.nextInt(16);
        // TODO: the Zero patterns (1 and 3, the drone lance) are not made; X is the only player (docs/gaps.md, "Commander Yammark")
        if (hp > maxHp / 2) {
            pattern = r < 8 ? PATTERNS[0] : PATTERNS[1];
        } else if (r < 4) {
            pattern = PATTERNS[0];
        } else if (r < 12) {
            pattern = PATTERNS[2];
        } else {
            pattern = PATTERNS[3];
        }
        patternIndex = 0;
        startAttack(pattern[0]);
    }

    private void startAttack(Step s) {
        step = s;
        sub = 0;
        timer = 0;
    }

    private void nextAttack() {
        patternIndex++;
        if (patternIndex >= pattern.length) {
            step = Step.THINK;
            return;
        }
        startAttack(pattern[patternIndex]);
    }

    private void updateAttack(World w) {
        switch (sub) {
            case 0 -> {
                updatePair();
                anim.play(0);
                if (mode != MODE_CIRCLE) {
                    mode = MODE_BACK;
                    sub = 1;
                } else {
                    sub = 2;
                    pickSide(w);
                }
            }
            case 1 -> {
                if (drones.stream().allMatch(YammarkDrone::inPlace)) {
                    sub = 2;
                    pickSide(w);
                }
            }
            case 2 -> {
                if (moveTo(vx, vy, 2)) {
                    mode = MODE_KEEP;
                    int made = 0;
                    for (int i = 0; i < droneCount(); i++) {
                        if (freeSlot(i)) {
                            summon(w, i, false);
                            made++;
                        }
                    }
                    if (made > 0) {
                        anim.play(5);
                        w.sfx.play(2, 34);
                        w.add(new YammarkFx(w, this, 15, 0, 0));
                    }
                    sub = 4;
                }
            }
            case 4 -> {
                if (drones.stream().allMatch(YammarkDrone::inPlace)) {
                    anim.play(0);
                    beginAttack(w);
                    sub = 5;
                    timer = 0;
                }
            }
            default -> runAttack(w);
        }
    }

    private void pickSide(World w) {
        boolean left = x < roomX + 176;
        faceRight = left;
        vx = left ? roomX + 48 : roomX + 304;
        float height = switch (step) {
            case BOMBARD -> w.rng.nextBoolean() ? 96 : 150;
            case CHASE -> 150;
            default -> 96;
        };
        vy = roomY + height;
        anim.play(Math.abs(vy - y) < 2 ? 2 : vy < y ? 3 : 4);
    }

    private boolean moveTo(float tx, float ty, float speed) {
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

    private boolean freeSlot(int slot) {
        return drones.stream().noneMatch(d -> d.slot == slot);
    }

    private void summon(World w, int slot, boolean intro) {
        YammarkDrone d = new YammarkDrone(w, this, slot, intro);
        drones.add(d);
        w.add(d);
    }

    private void beginAttack(World w) {
        switch (step) {
            case BOMBARD -> mode = MODE_BOMBARD;
            case DASH -> {
                w.sfx.play(2, 39);
                mode = MODE_CIRCLE;
            }
            case WALL_DASH -> {
                w.sfx.play(2, 39);
                mode = MODE_WALL;
                walls = 0;
                dashCount = 0;
            }
            case CHASE -> {
                anim.play(2);
                w.sfx.play(2, 39);
                mode = MODE_TRAIL;
                chaseTime = 0;
            }
            default -> {
            }
        }
    }

    private void runAttack(World w) {
        timer++;
        switch (step) {
            case BOMBARD -> {
                if (timer == 120) {
                    w.sfx.play(2, 38);
                }
                if (timer >= 500) {
                    nextAttack();
                }
            }
            case DASH -> runDash(w);
            case WALL_DASH -> runWallDash(w);
            case CHASE -> runChase(w);
            default -> {
            }
        }
    }

    private void runDash(World w) {
        if (sub == 5) {
            faceRight = w.player.x > x;
            float dy = w.player.y - y;
            y += Math.max(-2, Math.min(2, dy));
            y = clampY(y);
            if (timer == 90) {
                w.sfx.play(2, 40);
            }
            if (timer >= 120) {
                sub = 6;
                dashing = true;
                anim.play(2);
            }
            return;
        }
        x += faceRight ? 2 : -2;
        if (x <= leftX() || x >= rightX()) {
            x = clampX(x);
            dashing = false;
            mode = MODE_KEEP;
            anim.play(0);
            nextAttack();
        }
    }

    private void runWallDash(World w) {
        switch (sub) {
            case 5 -> {
                if (timer == 90) {
                    w.sfx.play(2, 41);
                }
                if (timer >= 260) {
                    sub = 6;
                    dashCount++;
                    anim.play(2);
                }
            }
            case 6 -> {
                x += faceRight ? 2.5f : -2.5f;
                if (x <= leftX() || x >= rightX()) {
                    x = clampX(x);
                    walls++;
                    anim.play(0);
                    if (walls >= 3) {
                        mode = MODE_KEEP;
                        nextAttack();
                        return;
                    }
                    sub = 7;
                    timer = 0;
                }
            }
            default -> {
                if (timer >= 50) {
                    faceRight = !faceRight;
                    sub = 6;
                    anim.play(2);
                }
            }
        }
    }

    private void runChase(World w) {
        chaseTime++;
        if (timer == 90) {
            w.sfx.play(2, 42);
        }
        if (timer % 8 == 1) {
            double a = Math.atan2(w.player.y - y, w.player.x - x);
            vx = (float) Math.cos(a) * 2;
            vy = (float) Math.sin(a) * 2;
            faceRight = vx > 0;
        }
        x = clampX(x + vx);
        y = clampY(y + vy);
        if (timer >= 500 || drones.isEmpty()) {
            mode = MODE_KEEP;
            anim.play(0);
            nextAttack();
        }
    }

    float clampX(float v) {
        return Math.max(leftX(), Math.min(rightX(), v));
    }

    float clampY(float v) {
        return Math.max(ceilingY(), Math.min(floorY(), v));
    }

    float histX(int updatesBack) {
        return histX[Math.floorMod(histHead - 1 - updatesBack, HISTORY)];
    }

    float histY(int updatesBack) {
        return histY[Math.floorMod(histHead - 1 - updatesBack, HISTORY)];
    }

    @Override
    protected void onDeathStart(World w) {
        anim.play(7);
        w.sfx.play(2, 32);
        dashing = false;
        for (Entity e : w.entities) {
            if (e instanceof YammarkDrone || e instanceof YammarkFx.Shot) {
                e.dead = true;
            }
        }
    }

    boolean fightOver() {
        return dying() || dead;
    }

    private float boxLeft() {
        return faceRight ? x - 16 : x - 8;
    }

    @Override
    public Rect hurtBox() {
        if (step == Step.INTRO || dying()) {
            return null;
        }
        // TODO: the box (-8, -16, 24, 48) is not centred; it is mirrored with the facing (docs/gaps.md, "Commander Yammark")
        return new Rect(boxLeft(), y - 16, 24, 48);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    private static final class Wings extends Entity {

        private final CommanderYammark boss;
        private int lastAnim = -1;

        Wings(World w, CommanderYammark boss) {
            this.boss = boss;
            this.anim = new Animator(w.animSet(SET));
            anim.play(14);
        }

        @Override
        public void update(World w) {
            if (boss.dead || boss.fightOver()) {
                dead = true;
                return;
            }
            faceRight = boss.faceRight;
            x = boss.x + (faceRight ? -3 : 3);
            y = boss.y - 6;
            int a = boss.anim().anim();
            boolean flying = a == 0 || a == 2 || a == 3 || a == 4;
            if (flying && a != lastAnim && !(lastAnim == 0 || lastAnim == 2 || lastAnim == 3 || lastAnim == 4)) {
                w.sfx.play(2, 48);
            }
            lastAnim = a;
            anim.tick();
        }

        @Override
        public boolean visible() {
            return boss.anim().anim() == 0 && boss.visible();
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
    }
}
