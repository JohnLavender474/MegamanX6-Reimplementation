package mmx.x6.game.boss;

import java.util.Set;
import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.physics.Rect;

public final class D1000Core extends Entity {

    private enum State { APPEAR, COUNT, IDLE, HIT }

    private static final Set<Integer> STUN_IDS = Set.of(18, 19, 20, 21, 50, 51);
    private static final int COUNT_TIME = 100;
    private static final int HIT_WAIT = 60;
    private static final int[] SUMMON_SHOTS = {0, 8, 16, 24};
    private static final int SUMMON_END = 124;
    public static final float Y = 256;

    private final D1000 body;
    private State state = State.APPEAR;
    private int timer;
    private boolean signal;
    private boolean active;
    private int summonTimer = -1;
    private boolean summonDone;

    public D1000Core(World w, D1000 body, float x) {
        this.body = body;
        this.x = x;
        this.y = Y;
        this.hp = 127;
        this.damageTable = DamageTables.Table.NORMAL;
        this.anim = new Animator(w.animSet("d1000"));
        anim.play(16);
    }

    public boolean active() {
        return active;
    }

    public void activate() {
        if (state == State.IDLE) {
            active = true;
        }
    }

    public boolean takeSignal() {
        boolean s = signal;
        signal = false;
        return s;
    }

    public void summon() {
        summonTimer = 0;
        summonDone = false;
    }

    public boolean takeSummonDone() {
        boolean s = summonDone;
        summonDone = false;
        return s;
    }

    @Override
    public void update(World w) {
        hp = 127;
        anim.tick();
        switch (state) {
            case APPEAR -> {
                if (anim.finished()) {
                    anim.play(13);
                    state = State.COUNT;
                    timer = 0;
                }
            }
            case COUNT -> {
                if (++timer >= COUNT_TIME) {
                    signal = true;
                    state = State.IDLE;
                }
            }
            case IDLE -> {
            }
            case HIT -> {
                if (++timer >= HIT_WAIT) {
                    x = D1000.coreXFor(w.player.x);
                    anim.play(16);
                    state = State.APPEAR;
                }
            }
        }
        if (summonTimer >= 0) {
            for (int t : SUMMON_SHOTS) {
                if (summonTimer == t) {
                    w.add(new D1000Shot(w, x, y, w.player.x, w.player.y));
                }
            }
            if (summonTimer >= SUMMON_END) {
                summonTimer = -1;
                summonDone = true;
            } else {
                summonTimer++;
            }
        }
    }

    @Override
    public void onHurt(World w, int weaponId) {
        hp = 127;
        if (active && STUN_IDS.contains(weaponId)) {
            active = false;
            state = State.HIT;
            timer = 0;
            anim.play(15);
            body.stun(w);
        }
    }

    @Override
    public void onKilled(World w, int weaponId) {
        hp = 127;
        onHurt(w, weaponId);
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 10, y - 10, 20, 20);
    }

    @Override
    public String spriteSet() {
        return "d1000";
    }

    @Override
    public int drawPriority() {
        return 5;
    }
}
