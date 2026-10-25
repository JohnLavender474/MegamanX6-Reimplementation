package mmx.x6.game.enemy.yammark;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.physics.Rect;

final class YammarkDrone extends Entity {

    enum State { INTRO, APPEAR, BACK, CIRCLE, BOMBARD, WALL, TRAIL }

    private static final float[] INTRO_X = {32, 176, 320, 104, 248, 176};
    private static final int[] DASH_SHOT = {30, 25, 45, 35, 25, 20, 35, 40};
    private static final float[] WALL_Y = {-40, 40, -60, 60, -20, 20, -80, 80};
    private static final float RADIUS = 32;

    final int slot;
    private final CommanderYammark boss;
    private State state;
    private int phase;
    private int timer;
    private int shotTimer;
    private int lastMode = -1;
    private boolean inPlace;
    private boolean boxes;
    private boolean arrived;
    private float radius;
    private int arrivals;
    private double lastDiff = Double.NaN;
    private float bob;
    private float savedX;
    private float savedY;
    private int age;

    YammarkDrone(World w, CommanderYammark boss, int slot, boolean intro) {
        this.boss = boss;
        this.slot = slot;
        this.hp = 8;
        this.contactDamage = 2;
        this.damageTable = DamageTables.Table.YAMMARK;
        this.anim = new Animator(w.animSet(CommanderYammark.SET));
        anim.play(11);
        if (intro) {
            state = State.INTRO;
            x = boss.roomX + INTRO_X[slot % INTRO_X.length];
            y = boss.roomY - 32;
        } else {
            state = State.APPEAR;
            radius = RADIUS;
            placeOnCircle();
        }
        lastMode = boss.mode;
    }

    boolean inPlace() {
        return inPlace;
    }

    private double slotAngle() {
        return Math.PI * 2 * slot / boss.droneCount();
    }

    private double angle() {
        return boss.circleAngle + slotAngle();
    }

    private void placeOnCircle() {
        x = boss.x + (float) Math.cos(angle()) * radius;
        y = boss.y + (float) Math.sin(angle()) * radius;
    }

    @Override
    public void update(World w) {
        if (boss.dead || boss.fightOver()) {
            dead = true;
            return;
        }
        age++;
        anim.tick();
        faceRight = boss.faceRight;
        if (boss.mode != lastMode) {
            lastMode = boss.mode;
            onMode(w, boss.mode);
        }
        switch (state) {
            case INTRO -> updateIntro();
            case APPEAR -> updateAppear();
            case BACK -> {
                float tx = boss.x + (float) Math.cos(angle()) * RADIUS;
                float ty = boss.y + (float) Math.sin(angle()) * RADIUS;
                if (step(tx, ty, 4)) {
                    inPlace = true;
                    state = State.CIRCLE;
                }
            }
            case CIRCLE -> {
                placeOnCircle();
                if (boss.dashing && ++shotTimer >= DASH_SHOT[slot % DASH_SHOT.length]) {
                    shotTimer = 0;
                    shoot(w);
                }
            }
            case BOMBARD -> updateBombard(w);
            case WALL -> updateWall(w);
            case TRAIL -> updateTrail(w);
        }
    }

    private void onMode(World w, int mode) {
        if (state == State.INTRO || state == State.APPEAR && !inPlace) {
            return;
        }
        switch (mode) {
            case CommanderYammark.MODE_BACK -> {
                state = State.BACK;
                inPlace = false;
                boxes = false;
                anim.play(11);
            }
            case CommanderYammark.MODE_CIRCLE -> {
                state = State.BACK;
                boxes = false;
            }
            case CommanderYammark.MODE_BOMBARD -> {
                if (slot < boss.droneCount() - boss.stayBack()) {
                    startTurn(State.BOMBARD);
                }
            }
            case CommanderYammark.MODE_WALL -> startTurn(State.WALL);
            case CommanderYammark.MODE_TRAIL -> {
                state = State.TRAIL;
                phase = 0;
                savedX = boss.x;
                savedY = boss.y;
            }
            default -> {
            }
        }
    }

    private void startTurn(State s) {
        state = s;
        phase = 0;
        arrivals = 0;
        lastDiff = Double.NaN;
        timer = 0;
    }

    private void updateIntro() {
        if (!arrived) {
            float speed = slot == 0 ? 2.8125f : 2;
            if (step(boss.x, boss.y, speed) || Math.hypot(boss.x - x, boss.y - y) <= 2) {
                arrived = true;
            }
            return;
        }
        boolean all = boss.drones.stream().allMatch(d -> d.arrived || d.state != State.INTRO);
        if (!all) {
            x = boss.x;
            y = boss.y;
            return;
        }
        radius = Math.min(RADIUS, radius + 1);
        placeOnCircle();
        if (radius >= RADIUS) {
            inPlace = true;
            state = State.CIRCLE;
        }
    }

    private void updateAppear() {
        placeOnCircle();
        timer++;
        if (phase == 0 && timer >= 38) {
            anim.play(9);
            phase = 1;
        } else if (phase == 1 && anim.finished()) {
            phase = 2;
            timer = 0;
            anim.play(11);
        } else if (phase == 2 && timer >= 60) {
            inPlace = true;
            state = State.CIRCLE;
        }
    }

    private boolean turned(boolean top) {
        double target = top ? -Math.PI / 2 : Math.PI / 2;
        double d = Math.atan2(Math.sin(angle() - target), Math.cos(angle() - target));
        if (!Double.isNaN(lastDiff) && Math.signum(d) != Math.signum(lastDiff) && Math.abs(d) < 1) {
            arrivals++;
        }
        lastDiff = d;
        return arrivals >= 2;
    }

    private void updateBombard(World w) {
        if (phase == 0) {
            placeOnCircle();
            if (turned(slot % 2 == 0)) {
                phase = 1;
                boxes = true;
                anim.play(10);
                timer = (slot % 2 == 0) ? -1 : 1;
                shotTimer = 0;
            }
            return;
        }
        y += timer * 3.75f;
        if (y <= boss.roomY + 8) {
            y = boss.roomY + 8;
            timer = 1;
        } else if (y >= boss.roomY + 184) {
            y = boss.roomY + 184;
            timer = -1;
        }
        if (++shotTimer >= 30) {
            shotTimer = 0;
            shoot(w);
        }
    }

    private void updateWall(World w) {
        if (phase == 0) {
            placeOnCircle();
            if (turned(slot % 2 == 0)) {
                phase = 1;
                boxes = true;
                anim.play(10);
            }
            return;
        }
        x = boss.x;
        if (boss.dashCount > 0) {
            boolean up = ((w.frame >> 5) & 1) == 0 ^ slot % 2 == 1;
            bob += up ? -1 : 1;
        }
        float ty = boss.y + WALL_Y[slot % WALL_Y.length] + bob;
        y += Math.max(-3, Math.min(3, ty - y));
    }

    private void updateTrail(World w) {
        int delay = (slot + 1) * 30;
        if (phase == 0) {
            x = savedX + (float) Math.cos(angle()) * RADIUS;
            y = savedY + (float) Math.sin(angle()) * RADIUS;
            if (boss.chaseTime >= delay) {
                phase = 1;
                boxes = true;
                anim.play(10);
            }
            return;
        }
        int back = Math.min(CommanderYammark.HISTORY - 1, delay);
        step(boss.histX(back), boss.histY(back), 3);
        if (shotTimer > 0) {
            shotTimer--;
        } else if (Math.abs(w.player.x - x) <= 12 && w.player.y - y > 48) {
            anim.play(12);
            w.add(new YammarkFx.Bomb(w, x, y + 6));
            shotTimer = 30;
        }
    }

    private boolean step(float tx, float ty, float speed) {
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

    private void shoot(World w) {
        w.add(new YammarkFx.Shot(w, x, y, w.player.x > x));
    }

    @Override
    public void onKilled(World w, int weaponId) {
        dead = true;
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, CommanderYammark.SET, new int[] {21, 22}, x, y);
    }

    @Override
    public Rect hurtBox() {
        return boxes ? new Rect(x - 16, y - 6, 32, 12) : null;
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public boolean visible() {
        boolean solid = state == State.INTRO || state == State.APPEAR && phase == 1 || boxes;
        if (state == State.APPEAR && phase == 0) {
            return false;
        }
        return solid || (age & 1) == 0;
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
        return 4;
    }
}
