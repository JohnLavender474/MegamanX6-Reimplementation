package mmx.x6.game.enemy.heatnix;

import java.util.ArrayList;
import java.util.List;
import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Explosion;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;

public final class NightmareSnake extends Entity {

    public enum Step { WAIT, INTRO, MAGMA_INTRO, MOVE, DEATH, END }

    static final String SET = "nightmare-snake";
    static final int[] LAVA_FRAMES = {960, 1568, 2176, 2784};
    private static final int BAR_MAX = 127;
    private static final int GEM_HP = 32;
    private static final float ACCEL = 1 / 16f;
    private static final int[][] GEM_OFS = {{-44, -57}, {58, -54}, {-28, 50}, {66, 36}};
    private static final int[][] GEM_BOX = {{-12, -14}, {-12, -14}, {-14, -13}, {-12, -13}};
    private static final int[][] JET_OFS = {{-84, -11}, {82, -18}, {26, 70}, {-4, -83}};
    private static final int[] HOP_RIGHT = {1953, 2241};
    private static final int[] HOP_LEFT = {2400, 2112};

    final int kind;
    final List<Gem> gems = new ArrayList<>();
    private final Body body;
    Step step = Step.WAIT;
    int sub;
    int dir;
    boolean waiting;
    boolean canShoot;
    private int timer;
    private float vx;
    private float vy;
    private float ax;
    private float ay;
    private int deadGems;
    private boolean ringShown;
    private boolean ringFlicker;
    private boolean setUp;
    private int pickRight;
    private int pickLeft;
    private float flameY;
    private final List<KillBox> killBoxes = new ArrayList<>();

    public NightmareSnake(World w, RecordDef r) {
        this.record = r;
        this.kind = r.id();
        this.x = r.x();
        this.y = r.y();
        this.body = new Body(x, y, 0, -8, 64, 64);
        this.contactDamage = 8;
        this.damageTable = DamageTables.Table.DEFLECT;
    }

    public Step step() {
        return step;
    }

    public int gemsLeft() {
        return 4 - deadGems;
    }

    @Override
    public void update(World w) {
        switch (step) {
            case WAIT -> waitStart(w);
            case INTRO -> updateIntro(w);
            case MAGMA_INTRO -> updateMagmaIntro(w);
            case MOVE -> move(w);
            case DEATH -> updateDeath(w);
            case END -> updateEnd(w);
        }
        if (step != Step.WAIT && step != Step.INTRO && step != Step.MAGMA_INTRO && step != Step.END && setUp) {
            int sum = 0;
            for (Gem g : gems) {
                if (!g.dead) {
                    sum += g.hp;
                }
            }
            w.bossHp = Math.min(BAR_MAX, sum);
            if (deadGems >= 4 && step != Step.DEATH) {
                startDeath(w);
            }
        }
        if (setUp && step != Step.END) {
            w.layer1Relative = true;
            w.camera.setLayer1(512 - x, 512 - y);
            boolean near = !w.offScreen(x, y, 96, 96);
            w.layer1Alpha = ringShown && near && !(ringFlicker && (w.frame & 1) == 0) ? 1 : 0;
        }
    }

    private void waitStart(World w) {
        if (w.player.step() == PlayerStep.GATE_WALK || w.locked()) {
            return;
        }
        switch (kind) {
            case 4 -> {
                if (w.checkpoint >= 3) {
                    remove(w);
                    return;
                }
                if (w.player.y < 2384) {
                    return;
                }
                x = 1072;
                y = 2752;
                w.player.startEvent(false, 0);
            }
            case 5 -> {
                if (w.checkpoint != 0 || w.player.x < 363 && w.player.y < 464) {
                    return;
                }
                x = 576;
                y = 584;
                w.player.startEvent(true, 0);
            }
            case 6, 7 -> {
                x = 2640;
                y = kind == 6 ? 2304 : 2944;
                w.player.startEvent(true, 0);
            }
            case 8 -> {
                if (w.checkpoint == 5) {
                    remove(w);
                    return;
                }
                w.camera.setTargets(-1, -1, 2816, 2816);
                float px = w.player.x;
                float py = w.player.y;
                boolean near = px < 2944 && py < 2912 || px >= 3009 && px <= 3071 && py < 2880;
                if (py < 2800 || !near) {
                    return;
                }
                x = 3024;
                y = w.camera.targetMinY() + 352;
                w.player.startEvent(true, 0);
            }
            case 9 -> {
                if (w.checkpoint == 4) {
                    remove(w);
                    return;
                }
                w.camera.setTargets(-1, -1, 2160, 2160);
                x = 3024;
                y = 2160 + 352;
                w.player.startEvent(true, 0);
            }
            default -> {
                if (killBoxes.isEmpty()) {
                    for (int i = 0; i < 4; i++) {
                        KillBox k = new KillBox(this, 7, 3296 + 160 * i, 1304);
                        killBoxes.add(k);
                        w.add(k);
                    }
                    w.camera.setLayer2Mode(2, LAVA_FRAMES[0] - 3232, 608);
                    w.layer2Front = true;
                    w.layer2Alpha = 0.75f;
                    x = 3616;
                    y = w.camera.targetMinY() + 352;
                }
                if (w.player.x < 3344) {
                    return;
                }
                w.player.startEvent(true, 0);
            }
        }
        setUp = true;
        w.flag26 = false;
        body.x = x;
        body.y = y;
        for (int i = 0; i < 4; i++) {
            Gem g = new Gem(w, this, i);
            gems.add(g);
            w.add(g);
            w.add(new Jet(w, this, i));
        }
        canShoot = kind < 8;
        ringShown = kind != 4;
        step = kind == 8 || kind == 9 ? Step.MAGMA_INTRO : Step.INTRO;
        sub = 0;
        timer = 0;
    }

    private void remove(World w) {
        dead = true;
        w.spawner.kept(record);
    }

    private boolean playerHeld(World w) {
        return w.player.step() == PlayerStep.EVENT_HOLD;
    }

    private void updateIntro(World w) {
        if (sub == 0) {
            if (!playerHeld(w)) {
                return;
            }
            ringShown = true;
            w.bossHpMax = BAR_MAX;
            w.bossHp = GEM_HP;
            w.bossBarShown = true;
            sub = 1;
            timer = 0;
            return;
        }
        if (fill(w)) {
            canShoot = kind < 10;
            startMove(w);
        }
    }

    private boolean fill(World w) {
        timer++;
        if ((w.frame & 1) == 0) {
            w.bossHp = Math.min(BAR_MAX, w.bossHp + 1);
        }
        if (timer % 3 == 1) {
            w.sfx.play(0, 14);
        }
        if (w.bossHp >= BAR_MAX) {
            w.player.endEvent();
            return true;
        }
        return false;
    }

    private void updateMagmaIntro(World w) {
        timer++;
        switch (sub) {
            case 0 -> {
                if (!playerHeld(w)) {
                    return;
                }
                for (int bx : new int[] {2944, 3104}) {
                    KillBox k = new KillBox(this, 6, bx, w.camera.targetMinY() + 192);
                    killBoxes.add(k);
                    w.add(k);
                }
                w.camera.setLayer2Mode(9, LAVA_FRAMES[0] - 2848, 0);
                flameY = 1600;
                w.camera.setLayer2(w.camera.x() + LAVA_FRAMES[0] - 2848, flameY);
                w.layer2Front = true;
                w.layer2Alpha = 0.6f;
                w.sfx.play(5, 1);
                sub = 1;
                timer = 0;
            }
            case 1 -> {
                flameY = Math.min(1712, flameY + 0.5f);
                w.camera.setLayer2(w.camera.layer2X(), flameY);
                if (flameY >= 1712) {
                    w.sfx.play(5, 3);
                    sub = 2;
                    timer = 0;
                }
            }
            case 2 -> {
                float top = w.camera.targetMinY() + 192;
                y = Math.max(top, y - 1);
                if (timer % 32 == 0) {
                    w.sfx.play(2, 3);
                }
                if (y <= top) {
                    w.bossHpMax = BAR_MAX;
                    w.bossHp = GEM_HP;
                    w.bossBarShown = true;
                    sub = 3;
                    timer = 0;
                }
            }
            default -> {
                if (fill(w)) {
                    w.sfx.play(5, 1);
                    step = Step.MOVE;
                    sub = 4;
                    timer = 0;
                    vy = 1;
                    dir = 4;
                }
            }
        }
    }

    private void startMove(World w) {
        step = Step.MOVE;
        sub = 0;
        timer = 0;
        switch (kind) {
            case 4 -> {
                dir = 1;
                vx = 1;
            }
            case 5, 7 -> {
                dir = 1;
                vx = 1;
            }
            case 6 -> {
                dir = 3;
                vx = -1;
            }
            default -> {
                dir = w.player.x > x ? 1 : 3;
                vx = dir == 1 ? 1 : -1;
            }
        }
    }

    private void move(World w) {
        if (kind == 5 && w.player.y < 432) {
            w.player.carry(0, 432 - w.player.y);
        }
        if (timer > 0) {
            waiting = true;
            timer--;
            if (kind == 8 || kind == 9) {
                magmaScroll(w);
            }
            return;
        }
        waiting = false;
        switch (kind) {
            case 4 -> moveWalls(w);
            case 5 -> moveRoll(w, false);
            case 6 -> moveHops(w);
            case 7 -> moveRoll(w, true);
            case 8, 9 -> moveMagma(w);
            default -> moveChase(w);
        }
    }

    private int step(World w, float dx, float dy) {
        body.x = x;
        body.y = y;
        int f = w.mover.move(body, dx, dy, false, List.of());
        x = body.x;
        y = body.y;
        return f;
    }

    private void wallHit(World w) {
        // TODO: the wall hits' Rumble(3) and Quake2(60, 2, 1) screen shake are not done (docs/gaps.md, "Screen shake")
        w.sfx.play(2, 24);
        timer = 60;
    }

    private void moveRoll(World w, boolean gravity) {
        vx = Math.max(-2, Math.min(2, vx + (vx > 0 ? ACCEL : -ACCEL)));
        if (gravity) {
            vy = Math.min(4, vy + 0.25f);
        }
        int f = step(w, vx, gravity ? vy : 0);
        if ((f & TerrainMover.FLOOR) != 0) {
            if (gravity && vy > 2) {
                w.sfx.play(2, 24);
            }
            vy = 0;
        }
        if (vx > 0 && (f & TerrainMover.RIGHT) != 0) {
            wallHit(w);
            vx = -1;
            dir = 3;
        } else if (vx < 0 && (f & TerrainMover.LEFT) != 0) {
            wallHit(w);
            vx = 1;
            dir = 1;
        }
    }

    private void moveWalls(World w) {
        // TODO: the wall run (step 2, var 4) is read only in short form: right, up, left, down along the walls, 1 to 2 px/frame (docs/gaps.md, "Nightmare Snake")
        float speed;
        switch (dir) {
            case 1 -> {
                vx = Math.min(2, vx + ACCEL);
                if ((step(w, vx, 0) & TerrainMover.RIGHT) != 0) {
                    wallHit(w);
                    dir = 2;
                    vy = 1;
                }
            }
            case 2 -> {
                vy = Math.min(2, vy + ACCEL);
                if ((step(w, 0, -vy) & TerrainMover.CEILING) != 0) {
                    wallHit(w);
                    dir = 3;
                    vx = 1;
                }
            }
            case 3 -> {
                vx = Math.min(2, Math.abs(vx) + ACCEL);
                if ((step(w, -vx, 0) & TerrainMover.LEFT) != 0) {
                    wallHit(w);
                    dir = 4;
                    vy = 1;
                }
            }
            default -> {
                speed = Math.min(2, vy + ACCEL);
                vy = speed;
                if ((step(w, 0, vy) & TerrainMover.FLOOR) != 0) {
                    wallHit(w);
                    dir = 1;
                    vx = 1;
                }
            }
        }
    }

    private void moveHops(World w) {
        // TODO: the hop roll (step 4, var 6) is read only in short form: hops at fixed x, turns at x 2657 and 1664 (docs/gaps.md, "Nightmare Snake")
        float oldX = x;
        vx = Math.max(-2, Math.min(2, vx + (vx > 0 ? ACCEL : -ACCEL)));
        vy += 0.05078125f;
        int f = step(w, vx, vy);
        if ((f & TerrainMover.FLOOR) != 0) {
            if (vy > 1) {
                w.sfx.play(2, 24);
            }
            vy = 0;
        }
        int[] hops = vx > 0 ? HOP_RIGHT : HOP_LEFT;
        for (int hx : hops) {
            if (oldX < hx && x >= hx || oldX > hx && x <= hx) {
                vy = -2.8125f;
            }
        }
        if (vx > 0 && (x >= 2657 || (f & TerrainMover.RIGHT) != 0)) {
            vx = -1;
            dir = 3;
        } else if (vx < 0 && (x < 1664 || (f & TerrainMover.LEFT) != 0)) {
            vx = 1;
            dir = 1;
        }
    }

    private void magmaScroll(World w) {
        float minY = w.camera.targetMinY();
        if (minY >= 945 && (w.frame & 3) == 0) {
            w.camera.setTargets(-1, -1, (int) minY - 1, w.camera.targetMaxY() - 1);
            y -= 1;
        }
        minY = w.camera.targetMinY();
        if (w.player.y < minY - 16) {
            w.player.carry(0, minY - 16 - w.player.y);
        }
        for (KillBox k : killBoxes) {
            k.y = minY + 192;
        }
    }

    private void moveMagma(World w) {
        magmaScroll(w);
        float camY = w.camera.targetMinY();
        switch (sub) {
            case 0 -> {
                boolean right = w.rng.nextBoolean();
                if (right && pickRight >= 2) {
                    right = false;
                } else if (!right && pickLeft >= 2) {
                    right = true;
                }
                pickRight = right ? pickRight + 1 : 0;
                pickLeft = right ? 0 : pickLeft + 1;
                x = right ? 3104 : 2944;
                vy = 1;
                dir = 2;
                sub = 2;
                timer = 90;
            }
            case 2 -> {
                float target = camY + 144;
                vy = y > target + 40 ? Math.min(3, vy + ACCEL) : Math.max(0.25f, vy - ACCEL);
                y -= vy;
                if (y <= target) {
                    y = target;
                    canShoot = true;
                    sub = 4;
                    timer = 90;
                    vy = 1;
                }
            }
            default -> {
                canShoot = false;
                dir = 4;
                vy = Math.min(2, vy + ACCEL);
                y += vy;
                if (y >= camY + 416) {
                    sub = 0;
                    timer = 90;
                }
            }
        }
    }

    private void moveChase(World w) {
        switch (sub) {
            case 0 -> {
                dir = w.player.x > x ? 1 : 3;
                vx = dir == 1 ? 1 : -1;
                sub = 1;
            }
            case 1 -> {
                vx = Math.max(-2, Math.min(2, vx + (vx > 0 ? ACCEL : -ACCEL)));
                x += vx;
                boolean edge = vx > 0 ? x >= 3729 : x < 3296;
                if (Math.abs(x - w.player.x) < 48 || edge) {
                    vx = 0;
                    vy = 1;
                    dir = 2;
                    sub = 2;
                    timer = 50;
                }
            }
            case 2 -> {
                float top = w.camera.targetMinY() + 192;
                vy = y > top ? Math.min(3, vy + ACCEL) : vy - ACCEL;
                y -= vy;
                if (vy <= 0) {
                    vy = 1;
                    dir = 4;
                    canShoot = true;
                    sub = 3;
                    timer = 90;
                }
            }
            default -> {
                canShoot = false;
                vy = Math.min(2, vy + ACCEL);
                y += vy;
                if (y >= w.camera.targetMinY() + 352) {
                    y = w.camera.targetMinY() + 352;
                    sub = 0;
                }
            }
        }
    }

    private void startDeath(World w) {
        step = Step.DEATH;
        sub = 0;
        timer = 0;
        canShoot = false;
        ringFlicker = true;
    }

    private void updateDeath(World w) {
        timer++;
        switch (sub) {
            case 0 -> {
                if (timer % 8 == 0) {
                    w.add(new Explosion(w, x + w.rng.nextInt(64) - 32, y + w.rng.nextInt(64) - 32));
                }
                if (timer >= 120) {
                    ringFlicker = false;
                    w.whiteLevel = 1;
                    sub = 1;
                }
            }
            case 1 -> {
                w.whiteLevel = 0;
                w.bossBarShown = false;
                ringShown = false;
                for (int i = 0; i < 12; i++) {
                    w.add(new Explosion(w, x + w.rng.nextInt(129) - 64, y + w.rng.nextInt(129) - 64));
                }
                sub = 2;
                timer = 0;
            }
            default -> {
                if (kind == 8 || kind == 9) {
                    if (timer <= 112) {
                        w.camera.setLayer2(w.camera.layer2X(), w.camera.layer2Y() - 1);
                        for (KillBox k : killBoxes) {
                            k.y += 1;
                        }
                        return;
                    }
                    w.sfx.play(5, 3);
                    w.layer2Alpha = 0;
                    w.layer2Front = false;
                    w.camera.setTargets(-1, -1, 944, 2816);
                    for (KillBox k : killBoxes) {
                        k.dead = true;
                    }
                }
                w.flag26 = true;
                step = Step.END;
            }
        }
    }

    private void updateEnd(World w) {
        w.layer1Alpha = 0;
        if (kind == 5 && w.player.x < 944) {
            if (w.player.y < 432) {
                w.player.carry(0, 432 - w.player.y);
            }
            return;
        }
        dead = true;
        w.spawner.kept(record);
    }

    void gemDied() {
        deadGems++;
    }

    boolean fighting() {
        return setUp && step == Step.MOVE;
    }

    @Override
    public Rect contactBox() {
        if (!setUp || step == Step.DEATH || step == Step.END) {
            return null;
        }
        return new Rect(x - 51, y - 67, 106, 118);
    }

    @Override
    public Rect hurtBox() {
        if (!setUp || step == Step.DEATH || step == Step.END) {
            return null;
        }
        return new Rect(x - 42, y - 62, 100, 100);
    }

    @Override
    public boolean isEnemy() {
        return true;
    }

    @Override
    public boolean visible() {
        return false;
    }

    @Override
    public int drawPriority() {
        return 5;
    }

    static final class Gem extends Entity {

        private final NightmareSnake parent;
        private final int index;

        Gem(World w, NightmareSnake parent, int index) {
            this.parent = parent;
            this.index = index;
            this.hp = GEM_HP;
            this.damageTable = DamageTables.Table.SNAKE;
            this.anim = new Animator(w.animSet(SET));
            anim.play(index);
            place();
        }

        private void place() {
            x = parent.x + GEM_OFS[index][0];
            y = parent.y + GEM_OFS[index][1];
        }

        @Override
        public void update(World w) {
            if (parent.dead || parent.step == Step.END) {
                dead = true;
                return;
            }
            anim.tick();
            place();
            if (parent.fighting() && parent.canShoot && w.frame % 120 == 0) {
                w.sfx.play(2, 31);
                float[] v = Projectile.aim(x, y, w.player.x, w.player.y, 4);
                w.add(new Projectile(w, SET, 9, x, y, v[0], v[1], 4, 6, 6)
                        .hittable(2, DamageTables.Table.NORMAL).explodes().endsOnPlayer().margin(96));
            }
        }

        @Override
        public void onKilled(World w, int weaponId) {
            dead = true;
            w.add(new Explosion(w, x, y));
            parent.gemDied();
        }

        @Override
        public Rect hurtBox() {
            if (!parent.setUp || parent.step == Step.INTRO || parent.step == Step.MAGMA_INTRO) {
                return null;
            }
            return new Rect(x + GEM_BOX[index][0], y + GEM_BOX[index][1], 16, 16);
        }

        @Override
        public boolean isEnemy() {
            return true;
        }

        @Override
        public boolean visible() {
            return !parent.ringFlicker;
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

    static final class Jet extends Entity {

        private final NightmareSnake parent;
        private final int index;

        Jet(World w, NightmareSnake parent, int index) {
            this.parent = parent;
            this.index = index;
            this.faceRight = index == 0;
            this.anim = new Animator(w.animSet(SET));
            anim.play(index < 2 ? 10 : index == 2 ? 11 : 12);
        }

        private boolean on() {
            int want = switch (index) {
                case 0 -> 1;
                case 1 -> 3;
                case 2 -> 2;
                default -> 4;
            };
            return parent.fighting() && !parent.waiting && parent.dir == want;
        }

        @Override
        public void update(World w) {
            if (parent.dead || parent.step == Step.END) {
                dead = true;
                return;
            }
            x = parent.x + JET_OFS[index][0];
            y = parent.y + JET_OFS[index][1];
            anim.tick();
            if (on() && (w.frame & 31) == 0) {
                w.sfx.play(2, 3);
            }
        }

        @Override
        public boolean visible() {
            return on();
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
            return 4;
        }
    }

    static final class KillBox extends Entity {

        private final NightmareSnake parent;
        private final int kind;
        private int frame;
        private int timer;

        KillBox(NightmareSnake parent, int kind, float x, float y) {
            this.parent = parent;
            this.kind = kind;
            this.x = x;
            this.y = y;
            this.contactDamage = 127;
        }

        @Override
        public void update(World w) {
            boolean animator = kind == 6 ? x == 2944 : x == 3296;
            if (animator && ++timer % 10 == 0) {
                frame = (frame + 1) % 4;
                if (kind == 6) {
                    w.camera.setLayer2Offset(LAVA_FRAMES[frame] - 2848, 0);
                } else {
                    w.camera.setLayer2Offset(LAVA_FRAMES[frame] - 3232, 608);
                }
            }
            if (kind == 7 && w.player.x >= 3825) {
                dead = true;
            }
        }

        @Override
        public Rect contactBox() {
            return kind == 6 ? new Rect(x - 80, y - 56, 160, 112) : new Rect(x - 80, y - 64, 160, 128);
        }

        @Override
        public boolean visible() {
            return false;
        }

        @Override
        public boolean clearedByGate() {
            return kind == 6;
        }
    }
}
