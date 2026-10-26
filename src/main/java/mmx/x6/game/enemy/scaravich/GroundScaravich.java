package mmx.x6.game.enemy.scaravich;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.boss.Maverick;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class GroundScaravich extends Maverick {

    public enum Step { ENTRY, WALK_OUT, NEW_BALL, WALK_IN, PELLET, WAIT, RUN_OFF, HIT_RUN, KICK }

    static final String SET = "ground-scaravich";
    static final String BALL_SET = "scaravich-ball";
    static final float FLOOR_Y = 1360;
    private static final int MAX_HP = 40;
    private static final float[] SPEED = {3.0f, 2.5f, 2.0f, 1.5f};
    private static final int RIGHT_TURN = 2176;
    private static final int LEFT_TURN = 1424;

    private Step step = Step.ENTRY;
    private int sub;
    private int timer;
    private int cycles;
    private boolean halfDone;
    private boolean hitRun;
    private int lastHp;
    private float vx;
    private Ball ball;
    private int ballSize = 1;

    public GroundScaravich(World w, RecordDef r) {
        super(w, r, SET, MAX_HP, DamageTables.Table.SCARAVICH);
        x = 2048;
        y = FLOOR_Y;
        contactDamage = 4;
        w.copyTiles(1600, 1136, 2, 15, 1568, 1136);
        w.copyTiles(1600, 1136, 2, 15, 1984, 1136);
        anim.play(0);
        ball = new Ball(w, this, 1);
        w.add(ball);
    }

    public Step step() {
        return step;
    }

    Ball ball() {
        return ball;
    }

    private int sizeFor(int count) {
        return count <= 3 ? 1 : count <= 6 ? 2 : count <= 8 ? 3 : 4;
    }

    private float speed() {
        return SPEED[ballSize - 1];
    }

    @Override
    protected void fight(World w) {
        anim.tick();
        timer++;
        boolean hit = hp < lastHp;
        lastHp = hp;
        switch (step) {
            case ENTRY -> entry(w);
            case WALK_OUT -> walkOut(w, hit);
            case NEW_BALL -> newBall(w);
            case WALK_IN -> walkIn(w, hit);
            case PELLET -> pellet(w);
            case WAIT -> waitStep(w);
            case RUN_OFF -> runOff(w);
            case HIT_RUN -> hitRun(w);
            case KICK -> kick(w);
        }
    }

    private void go(Step s) {
        step = s;
        sub = 0;
        timer = 0;
    }

    private void entry(World w) {
        switch (sub) {
            case 0 -> {
                if (startWarning(w)) {
                    w.camera.setTargets(1632, 1632, -1, -1);
                    sub = 1;
                }
            }
            case 1 -> {
                if (warningOver(w)) {
                    vx = -3;
                    shown = true;
                    anim.play(1);
                    sub = 2;
                }
            }
            case 2 -> {
                x += vx;
                // TODO: the walk-in sounds (2, 52) and (2, 34) and the rolling sound timer are not played (docs/gaps.md, "Ground Scaravich")
                if (x < 1888) {
                    // TODO: the text box before the fight is not shown (docs/gaps.md, "Maverick text")
                    anim.play(0);
                    startMusic(w);
                    w.camera.setTargets(1600, 1664, -1, -1);
                    sub = 3;
                }
            }
            default -> {
                if (fillHp(w)) {
                    lastHp = hp;
                    faceRight = false;
                    go(Step.WALK_OUT);
                }
            }
        }
    }

    private void startWalk() {
        anim.play(1);
        vx = faceRight ? speed() : -speed();
    }

    private boolean ballGone(World w) {
        if (ball != null && ball.destroyed()) {
            go(Step.RUN_OFF);
            return true;
        }
        return false;
    }

    private boolean hitReaction(boolean hit) {
        if (hit && !hitRun) {
            hitRun = true;
            go(Step.HIT_RUN);
            return true;
        }
        return false;
    }

    private void walkOut(World w, boolean hit) {
        if (ballGone(w) || hitReaction(hit)) {
            return;
        }
        if (sub == 0) {
            startWalk();
            sub = 1;
        }
        x += vx;
        if (vx > 0 && x > RIGHT_TURN) {
            faceRight = false;
            go(Step.NEW_BALL);
        } else if (vx < 0 && x < LEFT_TURN) {
            faceRight = true;
            go(Step.NEW_BALL);
        }
    }

    private void newBall(World w) {
        if (sub == 0) {
            if (ball != null && !ball.destroyed()) {
                ball.dead = true;
            }
            ball = null;
            damageTable = table;
            cycles = Math.min(10, cycles + 1);
            hitRun = false;
            if (!halfDone && hp < maxHp / 2) {
                halfDone = true;
                int size = sizeFor(cycles);
                if (size == 3) {
                    cycles = 0;
                } else if (size == 4) {
                    cycles = 4;
                }
            }
            ballSize = sizeFor(cycles);
            timer = 0;
            sub = 1;
            return;
        }
        if (timer >= ballSize * 10) {
            ball = new Ball(w, this, ballSize);
            w.add(ball);
            // TODO: the hard ball (gold) and the purple balls exist only on Hard; the difficulty is Normal here (docs/gaps.md, "Ground Scaravich")
            if (hp < maxHp / 2 && w.rng.nextInt(16) >= 9) {
                go(Step.KICK);
            } else {
                go(Step.WALK_IN);
            }
        }
    }

    private void walkIn(World w, boolean hit) {
        if (ballGone(w) || hitReaction(hit)) {
            return;
        }
        switch (sub) {
            case 0 -> {
                startWalk();
                sub = 1;
            }
            case 1 -> {
                x += vx;
                int s = anim.sprite();
                boolean there = vx > 0 ? x > 1728 : x < 1856;
                if (there && s != 4 && s != 5) {
                    int r = w.rng.nextInt(16);
                    if (r <= 4) {
                        anim.play(0);
                        go(Step.PELLET);
                    } else if (r <= 9) {
                        go(Step.WAIT);
                        timer = -50;
                    } else {
                        step = Step.WALK_OUT;
                        sub = 1;
                    }
                }
            }
            default -> {
            }
        }
    }

    private void pellet(World w) {
        if (ballGone(w)) {
            return;
        }
        switch (sub) {
            case 0 -> {
                anim.play(0);
                timer = 0;
                sub = 1;
            }
            case 1 -> {
                if (timer >= 40) {
                    anim.play(2);
                    sub = 2;
                }
            }
            case 2 -> {
                if (anim.enteredFlag(5)) {
                    w.sfx.play(2, 38);
                    w.sfx.play(2, 55);
                    float px = x + (faceRight ? 10 : -10);
                    w.add(new Projectile(w, SET, 6, px, y + 6, faceRight ? 3 : -3, 0, 4, 6, 6)
                            .endsOnPlayer().priority(4).facing(faceRight));
                    timer = 0;
                    sub = 3;
                }
            }
            default -> {
                if (timer >= 50) {
                    go(Step.WALK_OUT);
                    sub = 0;
                }
            }
        }
    }

    private void waitStep(World w) {
        if (ballGone(w)) {
            return;
        }
        if (sub == 0) {
            anim.play(0);
            sub = 1;
        }
        if (timer >= 40) {
            go(Step.WALK_OUT);
        }
    }

    private void runOff(World w) {
        if (sub == 0) {
            anim.play(4);
            vx = x < 1792 ? -5 : 5;
            faceRight = vx > 0;
            sub = 1;
        }
        x += vx;
        if (x > RIGHT_TURN || x < LEFT_TURN) {
            faceRight = x < LEFT_TURN;
            cycles = Math.min(10, cycles + 1);
            go(Step.NEW_BALL);
        }
    }

    private void hitRun(World w) {
        if (ballGone(w)) {
            return;
        }
        if (sub == 0) {
            anim.play(4);
            float v = Math.abs(vx) > 0 ? Math.abs(vx) : speed();
            int[] base = {32, 64, 96, 128};
            int[] extra = {6, 6, 0, -6};
            timer = -(int) (base[ballSize - 1] / v + extra[ballSize - 1]);
            sub = 1;
        }
        x += vx;
        if (timer >= 0) {
            faceRight = vx < 0;
            go(Step.WAIT);
        }
    }

    private void kick(World w) {
        if (ball == null || ball.destroyed() || ball.gone()) {
            go(Step.RUN_OFF);
            return;
        }
        switch (sub) {
            case 0 -> {
                startWalk();
                sub = 1;
            }
            case 1 -> {
                x += vx;
                if (vx > 0 && x > 1664 || vx < 0 && x < 1920) {
                    w.sfx.play(2, w.rng.nextBoolean() ? 39 : 40);
                    anim.play(3);
                    sub = 2;
                }
            }
            case 2 -> {
                if (anim.enteredFlag(2) && ball != null) {
                    ball.launch(w);
                    sub = 3;
                }
            }
            default -> {
                // TODO: at boss level 0 there is one kick; the extra falling balls (var 5) of the higher levels are not made (docs/gaps.md, "Ground Scaravich")
            }
        }
    }

    boolean rolling() {
        return (anim.flag() & 1) != 0;
    }

    @Override
    protected void onDeathStart(World w) {
        anim.play(5);
        w.sfx.play(2, 32);
        if (ball != null && !ball.dead) {
            ball.breakUp(w);
        }
    }

    @Override
    public Rect hurtBox() {
        // TODO: the boss boxes are not read; a 40 x 52 px box from the drawn sprite stands in (docs/gaps.md, "Ground Scaravich")
        return step == Step.ENTRY && sub < 3 || dying() ? null : new Rect(x - 20, y - 36, 40, 52);
    }

    @Override
    public Rect contactBox() {
        return invincible > 0 ? null : hurtBox();
    }

    static final class Ball extends Entity {

        private static final int[] DX = {28, 42, 48, 46};
        private static final int[] DY = {4, 16, 36, 50};
        private static final int[] HP = {8, 16, 48, 127};
        private static final int[] CONTACT = {4, 6, 8, 16};
        private static final int[] HURT = {32, 48, 80, 112};
        private static final int[] ATTACK = {30, 46, 72, 100};
        private static final int[] DEBRIS = {6, 7, 8, 9};

        enum Mode { CARRIED, ROLL, LOB, BOUNCE, BREAK }

        private final GroundScaravich boss;
        private final int size;
        private Mode mode = Mode.CARRIED;
        private float vx;
        private float vy;
        private float gravity;
        private int timer;
        private boolean gone;

        Ball(World w, GroundScaravich boss, int size) {
            this.boss = boss;
            this.size = size;
            this.hp = HP[size - 1];
            this.contactDamage = CONTACT[size - 1];
            this.damageTable = DamageTables.Table.SCARAVICH_BALL;
            this.anim = new Animator(w.animSet(BALL_SET));
            anim.play(size - 1);
            follow();
        }

        boolean destroyed() {
            return mode == Mode.BREAK;
        }

        boolean gone() {
            return gone;
        }

        private float floorY() {
            return FLOOR_Y - DY[size - 1];
        }

        private void follow() {
            faceRight = boss.faceRight;
            if (boss.step != Step.HIT_RUN) {
                x = boss.x + (boss.faceRight ? DX[size - 1] : -DX[size - 1]);
            }
            y = floorY();
        }

        void launch(World w) {
            w.sfx.play(2, 53);
            switch (w.rng.nextInt(3)) {
                case 0 -> {
                    mode = Mode.ROLL;
                    vx = w.player.x >= x ? 3 : -3;
                }
                case 1 -> {
                    mode = Mode.LOB;
                    vx = (w.player.x - x - 16) / 40;
                    vy = -2.5f;
                    gravity = 0.078f;
                }
                default -> {
                    mode = Mode.BOUNCE;
                    vx = w.player.x >= x ? 1.33f : -1.33f;
                    vy = -3.5f;
                    gravity = 0.109f;
                }
            }
        }

        @Override
        public void update(World w) {
            switch (mode) {
                case CARRIED -> {
                    if (boss.dead) {
                        dead = true;
                        return;
                    }
                    if (boss.rolling()) {
                        anim.tick();
                    }
                    follow();
                }
                case ROLL -> {
                    anim.tick();
                    x += vx;
                    leaveCheck(w);
                }
                case LOB, BOUNCE -> {
                    anim.tick();
                    vy += gravity;
                    x += vx;
                    y += vy;
                    if (y >= floorY() && vy > 0) {
                        y = floorY();
                        // TODO: the landing's Rumble(5) is not done (docs/gaps.md, "Screen shake")
                        if (mode == Mode.LOB) {
                            w.sfx.play(2, 54);
                            mode = Mode.ROLL;
                            vx = vx >= 0 ? 3 : -3;
                        } else {
                            w.sfx.play(2, 54);
                            vy = -3.5f;
                        }
                    }
                    leaveCheck(w);
                }
                case BREAK -> {
                    timer++;
                    if (timer % 10 == 1) {
                        int r = HURT[size - 1] / 2;
                        w.add(new Explosion(w, x + w.rng.nextInt(2 * r + 1) - r, y + w.rng.nextInt(2 * r + 1) - r));
                    }
                    if (timer >= 120) {
                        for (int i = 0; i < size; i++) {
                            Debris.spawn(w, BALL_SET, DEBRIS, x, y);
                            for (int k = 0; k < 4; k++) {
                                w.add(new Explosion(w, x + w.rng.nextInt(33) - 16, y + w.rng.nextInt(33) - 16));
                            }
                        }
                        dead = true;
                    }
                }
            }
        }

        private void leaveCheck(World w) {
            int m = 16 * size;
            if (x < 1568 - m || x > 2016 + m || y < 1104 - m || y > 1408 + m) {
                gone = true;
                dead = true;
            }
        }

        void breakUp(World w) {
            mode = Mode.BREAK;
            timer = 0;
            contactDamage = 0;
            damageTable = null;
        }

        @Override
        public void onKilled(World w, int weaponId) {
            boolean wasCarried = mode == Mode.CARRIED;
            breakUp(w);
            if (!wasCarried) {
                gone = true;
            }
        }

        @Override
        public Rect hurtBox() {
            if (mode == Mode.BREAK) {
                return null;
            }
            float h = HURT[size - 1] / 2f;
            return new Rect(x - h, y - h, h * 2, h * 2);
        }

        @Override
        public Rect contactBox() {
            if (mode == Mode.BREAK) {
                return null;
            }
            float h = ATTACK[size - 1] / 2f;
            return new Rect(x - h, y - h, h * 2, h * 2);
        }

        @Override
        public boolean visible() {
            return mode != Mode.BREAK || (timer & 2) == 0;
        }

        @Override
        public boolean isEnemy() {
            return mode != Mode.BREAK;
        }

        @Override
        public boolean clearedByGate() {
            return false;
        }

        @Override
        public String spriteSet() {
            return BALL_SET;
        }

        @Override
        public int drawPriority() {
            return 4;
        }
    }
}
