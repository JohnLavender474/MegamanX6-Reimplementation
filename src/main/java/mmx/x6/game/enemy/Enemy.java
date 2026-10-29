package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.GoalMember;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;

public abstract class Enemy extends Entity implements GoalMember {

    private final String set;
    protected float offMarginX = 64;
    protected float offMarginY = 64;

    protected Enemy(World w, RecordDef r, String set) {
        this.record = r;
        this.set = set;
        this.x = r.x();
        this.y = r.y();
        this.prevX = x;
        this.prevY = y;
        this.anim = new Animator(w.animSet(set));
    }

    @Override
    public String spriteSet() {
        return set;
    }

    @Override
    public boolean isEnemy() {
        return true;
    }

    protected boolean checkOffScreen(World w) {
        if (w.offScreen(x, y, offMarginX, offMarginY)) {
            despawn(w);
            return true;
        }
        return false;
    }

    protected void despawn(World w) {
        dead = true;
        w.spawner.released(record);
    }

    protected void facePlayer(World w) {
        faceRight = w.player.x >= x;
    }

    @Override
    public void onKilled(World w, int weaponId) {
        dead = true;
        w.spawner.released(record);
        onDeath(w);
        w.goals.reportKill(w, record);
    }

    protected abstract void onDeath(World w);
}
