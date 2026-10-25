package mmx.x6.game.boss;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.BossExplosion;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.RecordDef;
import mmx.x6.level.TileLayer;
import mmx.x6.physics.Rect;

public final class D1000 extends Entity {

    public enum BossStep { ENTRY, THINK, WALK, CROUCH, BOMB, SUMMON, RAM, STUN, DEATH }

    public static final float START_X = 6288;
    public static final float START_Y = 160;
    public static final float STAND_Y = 304;
    public static final float CROUCH_Y = 372;
    public static final float FLOOR_Y = 416;
    public static final int MAX_HP = 32;

    private static final float RAM_LOW_Y = 352;
    private static final float ROOM_LEFT = 6136;
    private static final float ROOM_RIGHT = 6392;
    private static final float LEFT_WALL_INNER = 6080;
    private static final float RIGHT_WALL_INNER = 6448;
    private static final int WALL_TOP_ROW = 208 / 16;
    private static final int WALL_ROWS = 13;
    private static final int SINK_AFTER_FADE = 31;
    private static final float SINK_SPEED = 0.25f;

    private BossStep step = BossStep.ENTRY;
    private int sub;
    private int timer;
    private float speedX;
    private float speedY;
    private boolean visible;
    private boolean afterCrouch;
    private boolean touched;
    private int invincible;
    private int ramDir;
    private D1000Core core;
    private BossExplosion explosion;

    public D1000(World w, RecordDef r) {
        this.record = r;
        this.x = START_X;
        this.y = START_Y;
        this.hp = 0;
        this.contactDamage = 6;
        this.damageTable = null;
        this.anim = new Animator(w.animSet("d1000"));
        anim.play(7);
        w.bossHpMax = MAX_HP;
        w.bossHp = 0;
        w.sfx.useBossBank(true);
    }

    public BossStep step() {
        return step;
    }

    public static BossStep pick(int r, int hp, boolean afterCrouch) {
        if (hp > MAX_HP / 2) {
            if (afterCrouch) {
                return r < 8 ? BossStep.WALK : BossStep.SUMMON;
            }
            return r < 8 ? BossStep.RAM : BossStep.SUMMON;
        }
        if (afterCrouch) {
            return r < 6 ? BossStep.SUMMON : BossStep.BOMB;
        }
        return r < 6 ? BossStep.RAM : BossStep.BOMB;
    }

    public static int coreXFor(float playerX) {
        return playerX < 6264 ? 6368 : 6160;
    }

    @Override
    public void update(World w) {
        if (invincible > 0 && --invincible == 0 && step != BossStep.STUN && step != BossStep.DEATH) {
            damageTable = DamageTables.Table.D1000;
        }
        anim.tick();
        switch (step) {
            case ENTRY -> updateEntry(w);
            case THINK -> updateThink(w);
            case WALK -> updateWalk(w);
            case CROUCH -> updateCrouch(w);
            case BOMB -> updateBomb(w);
            case SUMMON -> updateSummon(w);
            case RAM -> updateRam(w);
            case STUN -> updateStun(w);
            case DEATH -> updateDeath(w);
        }
        if (step != BossStep.ENTRY && step != BossStep.DEATH) {
            w.bossHp = Math.max(0, hp);
        }
    }

    private void facePlayer(World w) {
        faceRight = w.player.x > x;
    }

    private void updateEntry(World w) {
        switch (sub) {
            case 0 -> {
                if (w.player.step() == PlayerStep.GATE_WALK || w.player.dying()) {
                    return;
                }
                w.player.startEvent(true, 1);
                timer = 40;
                w.add(new BossWarning());
                w.music.stop();
                sub = 1;
            }
            case 1 -> {
                if (timer > 0 && --timer == 0) {
                    w.player.setEventWalk(0);
                }
                if (timer == 0 && w.entities.stream().noneMatch(e -> e instanceof BossWarning)) {
                    visible = true;
                    anim.play(7);
                    speedY = 0;
                    sub = 2;
                }
            }
            case 2 -> {
                speedY += 0.375f;
                y += speedY;
                if (y >= CROUCH_Y) {
                    y = CROUCH_Y;
                    w.sfx.play(2, 5);
                    anim.play(8);
                    sub = 3;
                    timer = 0;
                }
            }
            case 3 -> {
                timer++;
                if (timer == 120) {
                    core = new D1000Core(w, this, coreXFor(w.player.x));
                    w.add(core);
                }
                if (core != null && core.takeSignal()) {
                    anim.play(9);
                    sub = 4;
                }
            }
            case 4 -> {
                if (anim.finished()) {
                    w.sfx.play(2, 4);
                    sub = 5;
                }
            }
            case 5 -> {
                y -= 1;
                if (y <= STAND_Y) {
                    y = STAND_Y;
                    anim.play(0);
                    w.music.play(21);
                    w.bossBarShown = true;
                    sub = 6;
                    timer = 0;
                }
            }
            case 6 -> {
                timer++;
                hp++;
                w.bossHp = hp;
                if (timer % 3 == 1) {
                    w.sfx.play(0, 14);
                }
                if (hp >= MAX_HP) {
                    hp = MAX_HP;
                    damageTable = DamageTables.Table.D1000;
                    w.player.endEvent();
                    startThink(false);
                }
            }
            default -> {
            }
        }
    }

    private void startThink(boolean crouched) {
        step = BossStep.THINK;
        timer = 0;
        afterCrouch = crouched;
        anim.playIfNot(0);
        if (core != null) {
            core.activate();
        }
    }

    private void updateThink(World w) {
        facePlayer(w);
        anim.playIfNot(0);
        if (++timer < 60) {
            return;
        }
        BossStep next = pick(w.rng.nextInt(16), hp, afterCrouch);
        timer = 0;
        sub = 0;
        speedX = 0;
        speedY = 0;
        touched = false;
        step = next;
        switch (next) {
            case WALK -> anim.play(1);
            case SUMMON -> {
                if (core != null) {
                    core.summon();
                }
            }
            case RAM -> ramDir = faceRight ? 1 : -1;
            default -> {
            }
        }
    }

    private void updateWalk(World w) {
        facePlayer(w);
        anim.playIfNot(1);
        x = clampX(x + (faceRight ? 1 : -1));
        if (Math.abs(w.player.x - x) <= 60) {
            step = BossStep.CROUCH;
            sub = 0;
            speedY = 0;
            timer = 0;
            return;
        }
        if (++timer >= 120) {
            startThink(false);
        }
    }

    private void updateCrouch(World w) {
        switch (sub) {
            case 0 -> {
                speedY += 0.5f;
                y += speedY;
                if (y >= CROUCH_Y) {
                    y = CROUCH_Y;
                    w.sfx.play(2, 5);
                    anim.play(3);
                    sub = 1;
                    timer = 0;
                }
            }
            case 1 -> {
                timer++;
                if (timer < 60 && timer % 2 == 0) {
                    x += timer % 4 == 0 ? 2 : -2;
                }
                if (timer >= 90 || touched) {
                    sub = 2;
                }
            }
            default -> {
                y -= 1;
                if (y <= STAND_Y) {
                    y = STAND_Y;
                    startThink(true);
                }
            }
        }
    }

    private void updateBomb(World w) {
        facePlayer(w);
        if (++timer == 24) {
            w.sfx.play(2, 6);
            float bx = x + (faceRight ? 56 : -56);
            w.add(new D1000Bomb(w, bx, y - 24, w.player.x, FLOOR_Y));
        }
        if (timer >= 160) {
            step = BossStep.WALK;
            timer = 0;
            anim.play(1);
        }
    }

    private void updateSummon(World w) {
        facePlayer(w);
        if (core == null || core.takeSummonDone()) {
            step = BossStep.WALK;
            timer = 0;
            anim.play(1);
        }
    }

    private void updateRam(World w) {
        switch (sub) {
            case 0 -> {
                facePlayer(w);
                ramDir = faceRight ? 1 : -1;
                float target = Math.max(STAND_Y - 48, Math.min(RAM_LOW_Y, w.player.y - 90));
                y += Math.signum(target - y) * Math.min(2, Math.abs(target - y));
                if (++timer >= 180) {
                    sub = 1;
                    speedX = 0;
                    anim.play(4);
                }
            }
            case 1 -> {
                speedX = Math.min(4, speedX + 1);
                x += ramDir * speedX;
                float edge = ramDir > 0 ? x + 56 : x - 56;
                if (ramDir > 0 && edge >= RIGHT_WALL_INNER || ramDir < 0 && edge <= LEFT_WALL_INNER) {
                    x = ramDir > 0 ? RIGHT_WALL_INNER - 56 : LEFT_WALL_INNER + 56;
                    w.sfx.play(2, 5);
                    dentWall(w, ramDir);
                    sub = 2;
                    timer = 0;
                    anim.play(0);
                }
            }
            case 2 -> {
                if (++timer >= 180) {
                    restoreWalls(w);
                    sub = 3;
                }
            }
            default -> {
                y -= 1;
                if (y <= STAND_Y) {
                    y = STAND_Y;
                    x = clampX(x);
                    step = BossStep.WALK;
                    timer = 0;
                    anim.play(1);
                }
            }
        }
    }

    private float clampX(float v) {
        return Math.max(ROOM_LEFT, Math.min(ROOM_RIGHT, v));
    }

    private void dentWall(World w, int dir) {
        if (dir > 0) {
            copyColumn(w, 6448, 6576);
        } else {
            copyColumn(w, 6064, 6528);
        }
    }

    private void restoreWalls(World w) {
        copyColumn(w, 6064, 6512);
        copyColumn(w, 6448, 6560);
    }

    private static void copyColumn(World w, int dstX, int srcX) {
        TileLayer art = w.level.art0Main();
        int dst = dstX / 16;
        int src = srcX / 16;
        for (int r = WALL_TOP_ROW; r < WALL_TOP_ROW + WALL_ROWS; r++) {
            w.grid.set(dst, r, w.grid.raw(src, r));
            art.setGid(dst, r, art.gid(src, r));
        }
    }

    public void stun(World w) {
        if (step == BossStep.DEATH || step == BossStep.STUN) {
            return;
        }
        restoreWalls(w);
        step = BossStep.STUN;
        sub = 0;
        speedY = 0;
        damageTable = DamageTables.Table.DEFLECT;
        anim.play(5);
    }

    private void updateStun(World w) {
        switch (sub) {
            case 0 -> {
                speedY += 0.375f;
                y += speedY;
                if (y >= CROUCH_Y) {
                    y = CROUCH_Y;
                    w.sfx.play(2, 5);
                    anim.play(8);
                    sub = 1;
                    timer = 0;
                }
            }
            case 1 -> {
                if (++timer == 65) {
                    hp -= 8;
                    w.bossHp = Math.max(0, hp);
                    if (hp <= 0) {
                        hp = 0;
                        startDeath(w);
                        return;
                    }
                    sub = 2;
                }
            }
            case 2 -> {
                if (core != null && core.takeSignal()) {
                    anim.play(9);
                    sub = 3;
                }
            }
            case 3 -> {
                if (anim.finished()) {
                    sub = 4;
                }
            }
            default -> {
                y -= 1;
                if (y <= STAND_Y) {
                    y = STAND_Y;
                    damageTable = DamageTables.Table.D1000;
                    startThink(false);
                }
            }
        }
    }

    @Override
    public void onHurt(World w, int weaponId) {
        invincible = weaponId == 24 || weaponId == 25 ? 2 : 30;
        damageTable = DamageTables.Table.DEFLECT;
        w.bossHp = Math.max(0, hp);
    }

    @Override
    public void onKilled(World w, int weaponId) {
        hp = 0;
        startDeath(w);
    }

    @Override
    public void onTouchPlayer(World w) {
        touched = true;
    }

    private void startDeath(World w) {
        step = BossStep.DEATH;
        sub = 0;
        timer = 0;
        damageTable = null;
        w.bossHp = 0;
        for (Entity e : w.entities) {
            if (e instanceof D1000Shot || e instanceof D1000Bomb || e instanceof FirePillar) {
                e.dead = true;
            }
            if (e instanceof D1000Core) {
                e.dead = true;
                w.sfx.play(2, 20);
            }
        }
        core = null;
        restoreWalls(w);
        anim.play(0);
        w.player.startEvent(w.player.x < x, 0);
    }

    private void updateDeath(World w) {
        timer++;
        switch (sub) {
            case 0 -> {
                flash = blinkOn(timer) ? 1 : 0;
                if (timer >= 127) {
                    explosion = new BossExplosion(x, y, BossExplosion.LONG_HOLD | 1);
                    w.add(explosion);
                    sub = 1;
                    timer = 0;
                }
            }
            case 1 -> {
                flash = (timer / 6) % 2;
                if (explosion.fadeUpdates() > SINK_AFTER_FADE) {
                    y += SINK_SPEED;
                }
                if (explosion.white()) {
                    // TODO: X is not moved at the white screen, and the second blink, the second explosion and Zero are cut (docs/gaps.md, "D-1000", "Death")
                    anim.play(11);
                    y = w.camera.y() + 160;
                    flash = 0;
                    sub = 2;
                }
            }
            case 2 -> {
                if (explosion.dead) {
                    w.flow.clear();
                    sub = 3;
                }
            }
            default -> {
            }
        }
    }

    private static boolean blinkOn(int t) {
        int[] toggles = {26, 47, 63, 74};
        int count = 0;
        for (int tg : toggles) {
            if (t >= tg) {
                count++;
            }
        }
        if (t >= 74) {
            count += (t - 74) / 6;
        }
        return count % 2 == 1;
    }

    @Override
    public boolean visible() {
        if (!visible) {
            return false;
        }
        if (step == BossStep.DEATH) {
            return true;
        }
        return invincible == 0 || (invincible / 4) % 2 == 0;
    }

    @Override
    public Rect hurtBox() {
        if (!visible || step == BossStep.DEATH || step == BossStep.ENTRY && sub < 6) {
            return null;
        }
        return new Rect(x - 56, y - 64, 112, 92);
    }

    @Override
    public Rect contactBox() {
        if (!visible || step == BossStep.STUN || step == BossStep.DEATH || step == BossStep.ENTRY) {
            return null;
        }
        return new Rect(x - 56, y - 64, 112, 92);
    }

    @Override
    public String spriteSet() {
        return "d1000";
    }

    @Override
    public int drawPriority() {
        return 8;
    }
}
