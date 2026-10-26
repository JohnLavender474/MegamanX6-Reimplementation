package mmx.x6.game.enemy.sheldon;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class HiddenGlassFloor extends Entity {

    static final String SET = "hidden-glass-floor";
    private static final int SHOW = 150;
    private static final int AFTER = 60;
    private static final int BLINK = 31;

    private final int id;
    private int timer;
    private boolean touched;
    private boolean shown;

    public HiddenGlassFloor(World w, RecordDef r) {
        this.record = r;
        this.id = r.id() & 1;
        this.x = r.x();
        this.y = r.y();
        this.anim = new Animator(w.animSet(SET));
        anim.play(id);
    }

    public boolean shown() {
        return shown;
    }

    private Rect area() {
        return id == 0 ? new Rect(x - 34, y - 2, 68, 20) : new Rect(x - 26, y - 18, 52, 36);
    }

    @Override
    public void update(World w) {
        Rect p = w.player.hurtBox();
        boolean touching = p != null && p.overlaps(area());
        // TODO: weapon id 44 (Zero's step 59) also shows the floor; X has no such weapon (docs/gaps.md, "Hidden glass floor")
        if (timer == 0 && touching) {
            timer = SHOW;
            touched = true;
        }
        if (timer > 0) {
            timer--;
            if (timer >= BLINK && touched && !touching) {
                timer = AFTER;
                touched = false;
            }
        }
        shown = timer >= BLINK || timer > 0 && (timer & 1) == 1;
        if (w.offScreen(x, y, 80, 160)) {
            dead = true;
            w.spawner.released(record);
        }
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
        return SET;
    }

    @Override
    public int drawPriority() {
        return 7;
    }
}
