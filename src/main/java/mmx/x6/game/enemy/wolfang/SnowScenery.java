package mmx.x6.game.enemy.wolfang;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;

public final class SnowScenery extends Entity {

    static final String SET = "enemy-cd98e86270";

    public SnowScenery(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        this.anim = new Animator(w.animSet(SET));
        anim.play(r.id());
    }

    @Override
    public void update(World w) {
        // TODO: the draw order of misc 23 against the tiles and X is not read; it is drawn behind X like other objects (docs/gaps.md, "Snow scenery")
        if (w.offScreen(x, y, 160, 160)) {
            dead = true;
            w.spawner.released(record);
        }
    }

    @Override
    public String spriteSet() {
        return SET;
    }
}
