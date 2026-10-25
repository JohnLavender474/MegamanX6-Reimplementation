package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;

public final class TeleporterCrystal extends Entity {

    public TeleporterCrystal(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        this.anim = new Animator(w.animSet("teleport-crystal"));
        anim.play(0);
    }

    @Override
    public void update(World w) {
        anim.tick();
        // TODO: the crystal's area change (game.exit_req 0x41) is not done: the hidden sections are out of scope, and only Turtloid's destination is known (docs/gaps.md, "Teleporter crystal")
        if (w.offScreen(x, y, 64, 64)) {
            dead = true;
            w.spawner.released(record);
        }
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public String spriteSet() {
        return "teleport-crystal";
    }

    @Override
    public int drawPriority() {
        return 5;
    }
}
