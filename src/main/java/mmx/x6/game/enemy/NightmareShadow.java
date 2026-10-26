package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class NightmareShadow extends Entity {

    public enum Step { WAIT_ZONE, SPAWN, APPEAR, DASH, DYING }

    static final String SET = "nightmare-shadow";
    private static final int EFFECT = 7;
    private static final float DASH_SPEED = 4;

    private final int kind;
    private Step step;
    private int sub;
    private int timer;
    private float vx;
    private float vy;

    public NightmareShadow(World w, RecordDef r) {
        this.record = r;
        this.kind = r.id();
        this.x = r.x();
        this.y = r.y();
        this.anim = new Animator(w.animSet(SET));
        this.hp = 4;
        this.contactDamage = 6;
        this.damageTable = DamageTables.Table.SHADOW;
        if (kind == 10) {
            step = Step.APPEAR;
            timer = 40;
            anim.play(0);
            faceRight = w.player.x >= x;
        } else {
            step = Step.WAIT_ZONE;
        }
    }

    public Step step() {
        return step;
    }

    private static boolean inZone(int kind, World w) {
        float px = w.player.x;
        return switch (kind) {
            case 0 -> px >= 321 && px <= 2559;
            case 1 -> px >= 3201 && px <= 4159;
            case 2 -> px >= 1249 && px <= 1631;
            case 3 -> px >= 4961 && px <= 7311;
            case 4 -> px >= 1 && px <= 1055;
            case 5 -> px >= 321 && px <= 2143 && w.player.y < 1072;
            default -> false;
        };
    }

    @Override
    public void update(World w) {
        if (w.stage.nightmareEffect() != EFFECT) {
            dead = true;
            return;
        }
        anim.tick();
        switch (step) {
            case WAIT_ZONE -> {
                if (inZone(kind, w)) {
                    step = Step.SPAWN;
                    timer = randomWait(w);
                }
            }
            case SPAWN -> {
                if (inZone(kind, w) && --timer <= 0) {
                    float sx = w.player.x + (w.player.faceRight ? 32 : -32);
                    w.add(new NightmareShadow(w, new RecordDef(-1, "NightmareShadow", "main", 0, 26, 10, 0, (int) sx,
                            (int) w.player.y)));
                    timer = randomWait(w);
                }
            }
            case APPEAR -> {
                timer--;
                if (sub == 0 && timer <= 0) {
                    sub = 1;
                    timer = 50;
                } else if (sub == 1 && timer <= 0) {
                    anim.play(1);
                    float[] v = Projectile.aim(x, y, w.player.x, w.player.y, DASH_SPEED);
                    vx = v[0];
                    vy = v[1];
                    faceRight = vx > 0;
                    step = Step.DASH;
                }
            }
            case DASH -> {
                x += vx;
                y += vy;
                if (w.offScreen(x, y, 128, 128)) {
                    dead = true;
                }
            }
            case DYING -> {
                // TODO: the death draws two copies that move apart at 1 px/update; one flickering copy stands in (docs/gaps.md, "Nightmare Shadow")
                if (++timer >= 60) {
                    dead = true;
                }
            }
        }
    }

    private static int randomWait(World w) {
        return (w.rng.nextInt(3) + 1) * 100 + 200;
    }

    private boolean boxes() {
        return step == Step.APPEAR && sub == 1 || step == Step.DASH;
    }

    @Override
    public Rect hurtBox() {
        if (!boxes()) {
            return null;
        }
        return step == Step.DASH ? new Rect(x - 11, y - 14, 18, 34) : new Rect(x - 10, y - 17, 19, 38);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public void onKilled(World w, int weaponId) {
        step = Step.DYING;
        timer = 0;
    }

    @Override
    public boolean visible() {
        if (step == Step.WAIT_ZONE || step == Step.SPAWN) {
            return false;
        }
        if (step == Step.APPEAR && sub == 0 || step == Step.DYING) {
            return (timer & 1) == 0;
        }
        return true;
    }

    @Override
    public boolean isEnemy() {
        return kind == 10;
    }

    @Override
    public boolean clearedByGate() {
        return kind == 10;
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
