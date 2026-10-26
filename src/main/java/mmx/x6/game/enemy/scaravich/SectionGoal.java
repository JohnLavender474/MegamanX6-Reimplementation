package mmx.x6.game.enemy.scaravich;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class SectionGoal extends Entity {

    public enum State { WAIT, BREAK, TELEPORT }

    static final String SET = "section-goal";
    private static final int BREAK_UPDATES = 60;
    private static final int[] DEBRIS = {2, 3, 4};

    private State state = State.WAIT;
    private int timer;

    public SectionGoal(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        this.hp = 32;
        this.contactDamage = 4;
        this.damageTable = DamageTables.Table.NORMAL;
        this.anim = new Animator(w.animSet(SET));
        anim.play(0);
    }

    public State state() {
        return state;
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (state) {
            case WAIT -> {
                if (w.offScreen(x, y, 64, 64)) {
                    dead = true;
                    w.spawner.released(record);
                }
            }
            case BREAK -> {
                timer++;
                // TODO: the goal's 60 updates of effects and debris are not read in detail; an explosion every 8 updates and debris every 20 stand in (docs/gaps.md, "Scaravich routing")
                if (timer % 8 == 1) {
                    w.add(new Explosion(w, x + w.rng.nextInt(33) - 16, y + w.rng.nextInt(41) - 20));
                }
                if (timer % 20 == 1) {
                    Debris.spawn(w, SET, DEBRIS, x, y);
                }
                if (timer >= BREAK_UPDATES) {
                    state = State.TELEPORT;
                    int cleared = w.polesCleared;
                    w.add(new SectionTeleport(w, () -> Sections.returnPoint(cleared), () -> {
                        int[] p = Sections.POLE[cleared];
                        w.add(new TotemPole(w, p[0], p[1]));
                    }));
                    dead = true;
                    w.spawner.kept(record);
                }
            }
            default -> {
            }
        }
    }

    @Override
    public void onKilled(World w, int weaponId) {
        hp = 0;
        damageTable = null;
        contactDamage = 0;
        state = State.BREAK;
        timer = 0;
    }

    @Override
    public Rect hurtBox() {
        // TODO: the goal's boxes are not read; the drawn statue (28 x 40 px) stands in (docs/gaps.md, "Scaravich routing")
        return state == State.WAIT ? new Rect(x - 14, y - 20, 28, 40) : null;
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public boolean visible() {
        return state == State.WAIT || (timer & 2) == 0;
    }

    @Override
    public boolean isEnemy() {
        return state == State.WAIT;
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
        return 5;
    }
}
