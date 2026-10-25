package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.Progress;
import mmx.x6.game.World;
import mmx.x6.game.fx.AnimFx;
import mmx.x6.game.fx.Explosion;
import mmx.x6.game.player.PlayerConst;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class RescueReploid extends Entity {

    public enum State { WAIT, HEAL, TELEPORT, BEAM, CHANGE, GLOW, ARMED, ATTACK, GONE }

    private static final String SET = "rescue-reploid";
    private static final int[] HEAL = {2, 4, 8, 16};
    private static final int[][] MUZZLE = {
            {-14, -6, 14, -6}, {-8, 0, 8, 0}, {-8, -14, 16, -14}, {-12, -16, 12, -16},
            {-14, -6, 14, -6}, {-10, -17, 8, -17}, {-10, -2, 8, -2}, {-14, -6, 14, -6}};
    private static final float BULLET_SPEED = 4;

    private final int id;
    private final boolean hover;
    private final float startY;
    private State state = State.WAIT;
    private boolean right;
    private int timer;
    private int amount;
    private boolean caught;

    public RescueReploid(World w, RecordDef r) {
        this.record = r;
        this.id = r.id();
        this.x = r.x();
        this.y = r.y();
        this.startY = y;
        this.anim = new Animator(w.animSet(SET));
        this.hover = !TileRules.isObjectSolid(w.grid.at(x, y + 17));
        int status = w.progress.reploid(id);
        if (status == Progress.REPLOID_SAVED || status == Progress.REPLOID_DEAD || status == Progress.REPLOID_LOST) {
            state = State.GONE;
            return;
        }
        right = w.player.x >= x;
        anim.play(idleAnim());
    }

    public State state() {
        return state;
    }

    public boolean catchable() {
        return state == State.WAIT && !caught;
    }

    public void infect(World w) {
        caught = true;
    }

    private int idleAnim() {
        return hover ? (right ? 14 : 5) : (right ? 11 : 0);
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (state) {
            case WAIT -> updateWait(w);
            case HEAL -> updateHeal(w);
            case TELEPORT -> {
                if (--timer <= 0) {
                    anim.play(8);
                    state = State.BEAM;
                }
            }
            case BEAM -> {
                if (anim.flag() != 0) {
                    y -= 8;
                }
                if (w.offScreen(x, y, 64, 64)) {
                    dead = true;
                    w.spawner.kept(record);
                }
            }
            case CHANGE -> {
                if (--timer <= 0) {
                    w.sfx.play(2, 65);
                    state = State.GLOW;
                    timer = 60;
                }
            }
            case GLOW -> {
                flash = 1;
                if (--timer <= 0) {
                    hp = 13;
                    contactDamage = 6;
                    damageTable = DamageTables.Table.NORMAL;
                    anim.play(infectedIdle());
                    state = State.ARMED;
                    timer = 60;
                }
            }
            case ARMED -> {
                if (--timer <= 0) {
                    anim.play(hover ? (right ? 16 : 7) : (right ? 13 : 2));
                    state = State.ATTACK;
                }
            }
            case ATTACK -> {
                if (anim.entered(1) && w.player.step() != PlayerStep.GATE_WALK) {
                    fire(w);
                }
                if (anim.entered(2)) {
                    right = w.player.x >= x;
                    anim.play(infectedIdle());
                    state = State.ARMED;
                    timer = 60;
                }
            }
            case GONE -> {
                dead = true;
                w.spawner.kept(record);
            }
        }
        if (state == State.ARMED || state == State.ATTACK || state == State.CHANGE || state == State.GLOW) {
            if (w.offScreen(x, y, 64, 64)) {
                w.progress.setReploid(id, Progress.REPLOID_LOST);
                dead = true;
                w.spawner.kept(record);
            }
        }
    }

    private int infectedIdle() {
        return hover ? (right ? 22 : 21) : (right ? 20 : 19);
    }

    private void updateWait(World w) {
        boolean nowRight = w.player.x >= x;
        if (nowRight != right) {
            right = nowRight;
            anim.play(idleAnim());
        }
        Rect p = w.player.hurtBox();
        if (!caught && p != null && w.player.hp > 0 && p.overlaps(body())) {
            rescue(w);
            return;
        }
        if (caught) {
            w.progress.setReploid(id, Progress.REPLOID_LOST);
            anim.play(hover ? (right ? 15 : 6) : (right ? 12 : 1));
            state = State.CHANGE;
            timer = 130;
            return;
        }
        if (hover) {
            y = startY + (float) Math.sin((w.frame & 127) * Math.PI * 2 / 128) * 4;
        }
        if (w.offScreen(x, y, 48, 48)) {
            dead = true;
            w.spawner.released(record);
        }
    }

    private void rescue(World w) {
        w.progress.setReploid(id, Progress.REPLOID_SAVED);
        if (w.progress.lives < Progress.MAX_LIVES) {
            w.progress.lives++;
            w.sfx.play(0, 21);
        }
        amount = HEAL[w.rng.nextInt(4)];
        // TODO: the 0-3 sub tank units of the rescue are not given; there are no sub tanks yet (docs/gaps.md, "Rescue Reploid")
        if (w.player.hp < PlayerConst.MAX_HP) {
            w.pauseOwner = this;
            w.lockCount++;
            state = State.HEAL;
            timer = 0;
        } else {
            startTeleport(w);
        }
    }

    private void updateHeal(World w) {
        if (++timer % 4 != 0) {
            return;
        }
        w.player.hp++;
        w.sfx.play(0, 13);
        amount--;
        if (amount <= 0 || w.player.hp >= PlayerConst.MAX_HP) {
            w.pauseOwner = null;
            w.lockCount = Math.max(0, w.lockCount - 1);
            startTeleport(w);
        }
    }

    private void startTeleport(World w) {
        right = w.player.x >= x;
        anim.play(hover ? (right ? 18 : 10) : (right ? 17 : 9));
        state = State.TELEPORT;
        timer = 80;
    }

    private void fire(World w) {
        int[] m = MUZZLE[(id >> 4) & 7];
        float mx = x + (right ? m[2] : m[0]);
        float my = y + (right ? m[3] : m[1]);
        w.sfx.play(2, 46);
        w.add(new AnimFx(w, SET, 3, mx, my, false));
        float dir = right ? 1 : -1;
        w.add(new Projectile(w, SET, 4, mx + dir * 20, my, dir * BULLET_SPEED, 0, 4, 2, 2)
                .hidden(6).priority(1).facing(false));
    }

    private Rect body() {
        return new Rect(x - 8, y - 16, 16, 32);
    }

    @Override
    public Rect hurtBox() {
        return state == State.ARMED || state == State.ATTACK ? new Rect(x - 8, y - 10, 16, 20) : null;
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public void onKilled(World w, int weaponId) {
        w.progress.setReploid(id, Progress.REPLOID_DEAD);
        w.add(new Explosion(w, x, y));
        w.add(NightmareVirus.make(w, NightmareVirus.REPLOID_SOUL, x, y));
        dead = true;
        w.spawner.kept(record);
    }

    @Override
    public boolean isEnemy() {
        return state == State.ARMED || state == State.ATTACK;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public boolean visible() {
        return state != State.GONE;
    }

    @Override
    public String spriteSet() {
        return SET;
    }

    @Override
    public int drawPriority() {
        return 2;
    }
}
