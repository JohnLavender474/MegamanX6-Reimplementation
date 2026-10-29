package mmx.x6.game.boss;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.GoalMember;
import mmx.x6.game.World;
import mmx.x6.game.enemy.NightmareVirus;
import mmx.x6.game.fx.BossExplosion;
import mmx.x6.level.RecordDef;

public abstract class Maverick extends Entity implements GoalMember {

    private static final int FAST_FIGHT = 5401;
    private static final int[] BLINK_TOGGLES = {26, 47, 63, 74};

    protected final int maxHp;
    protected final DamageTables.Table table;
    private final String set;
    protected boolean shown;
    protected int invincible;
    protected int fightTimer;
    private int warnTimer;
    private boolean warned;
    private int fillTimer;
    private boolean dying;
    private int deathSub;
    private int deathTimer;
    private BossExplosion explosion;

    protected Maverick(World w, RecordDef r, String set, int maxHp, DamageTables.Table table) {
        this.record = r;
        this.set = set;
        this.maxHp = maxHp;
        this.table = table;
        this.anim = set == null ? null : new Animator(w.animSet(set));
        this.hp = 0;
        w.bossHpMax = maxHp;
        w.bossHp = 0;
        w.sfx.useBossBank(true);
    }

    @Override
    public final void update(World w) {
        if (dying) {
            updateDeath(w);
            return;
        }
        if (invincible > 0) {
            flash = (invincible >> 1) & 1;
            if (--invincible == 0) {
                damageTable = table;
                flash = 0;
            }
        }
        if (w.bossBarShown) {
            fightTimer++;
        }
        fight(w);
        if (!dying && w.bossBarShown) {
            w.bossHp = Math.max(0, hp);
        }
    }

    protected abstract void fight(World w);

    protected boolean startWarning(World w) {
        if (warned) {
            return true;
        }
        if (w.player.step() == mmx.x6.game.player.PlayerStep.GATE_WALK || w.player.dying()) {
            return false;
        }
        warned = true;
        w.player.startEvent(w.player.x < x, 0);
        w.add(new BossWarning());
        w.music.stop();
        warnTimer = 40;
        return true;
    }

    protected boolean warningOver(World w) {
        if (warnTimer > 0) {
            warnTimer--;
        }
        return warnTimer == 0 && w.entities.stream().noneMatch(e -> e instanceof BossWarning);
    }

    protected void startMusic(World w) {
        w.music.play(w.stage.bossMusic());
        w.bossBarShown = true;
    }

    protected boolean fillHp(World w) {
        fillTimer++;
        hp++;
        w.bossHp = hp;
        if (fillTimer % 3 == 1) {
            w.sfx.play(0, 14);
        }
        if (hp >= maxHp) {
            hp = maxHp;
            damageTable = table;
            w.player.endEvent();
            return true;
        }
        return false;
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
        w.bossHp = 0;
        dying = true;
        damageTable = null;
        invincible = 0;
        deathSub = 0;
        deathTimer = 0;
        w.player.startEvent(w.player.x < x, 0);
        onDeathStart(w);
    }

    protected abstract void onDeathStart(World w);

    protected boolean dying() {
        return dying;
    }

    private void updateDeath(World w) {
        deathTimer++;
        if (anim != null) {
            anim.tick();
        }
        switch (deathSub) {
            case 0 -> {
                flash = blinkOn(deathTimer) ? 1 : 0;
                if (deathTimer >= 127) {
                    explosion = new BossExplosion(x, y, BossExplosion.LONG_HOLD | 1);
                    w.add(explosion);
                    deathSub = 1;
                    deathTimer = 0;
                }
            }
            case 1 -> {
                flash = (deathTimer / 6) % 2;
                if (explosion.white()) {
                    shown = false;
                }
                if (explosion.dead) {
                    flash = 0;
                    shown = false;
                    int id = fightTimer < FAST_FIGHT ? NightmareVirus.BOSS_SOUL_FAST : NightmareVirus.BOSS_SOUL_SLOW;
                    // TODO: after the boss soul the player exit (player step 25, game+0x0F = 16) is not read; STAGE CLEAR shows instead (docs/gaps.md, "Maverick end")
                    w.add(NightmareVirus.make(w, id, x, y).onBossSoul(() -> w.goals.reportBossEnd(w, record)));
                    deathSub = 2;
                }
            }
            default -> {
            }
        }
    }

    private static boolean blinkOn(int t) {
        int count = 0;
        for (int tg : BLINK_TOGGLES) {
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
        return shown;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public String spriteSet() {
        return set;
    }

    @Override
    public int drawPriority() {
        return 5;
    }
}
