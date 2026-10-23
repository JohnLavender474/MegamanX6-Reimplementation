package mmx.x6.game.fx;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;

public final class AnimFx extends Entity {

    private final String set;

    public AnimFx(World w, String set, int animIndex, float x, float y, boolean faceRight) {
        this.set = set;
        this.x = x;
        this.y = y;
        this.faceRight = faceRight;
        this.anim = new Animator(w.animSet(set));
        anim.play(animIndex);
    }

    @Override
    public void update(World w) {
        anim.tick();
        if (anim.finished()) {
            dead = true;
        }
    }

    @Override
    public String spriteSet() {
        return set;
    }

    @Override
    public int drawPriority() {
        return 3;
    }
}
