package mmx.x6.game.enemy.turtloid;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class RocketPlatform extends Entity implements Solid {

    private static final int STOP = 40;

    private final boolean vertical;
    private final float startX;
    private final float startY;
    private final float range;
    private float speed;
    private int stop;

    public RocketPlatform(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        this.startX = x;
        this.startY = y;
        this.vertical = r.id() != 0;
        this.range = vertical ? 80 : 96;
        this.anim = new Animator(w.animSet("rocket-platform"));
        if (vertical) {
            speed = -1;
            anim.play(0);
        } else {
            speed = w.player.x < x ? -1 : 1;
            anim.play(1);
        }
    }

    @Override
    public void update(World w) {
        anim.tick();
        if (stop > 0) {
            stop--;
            return;
        }
        if (vertical) {
            y += speed;
            if (Math.abs(y - startY) >= range) {
                y = startY + Math.signum(y - startY) * range;
                reverse();
                anim.play(speed < 0 ? 0 : 1);
            }
        } else {
            x += speed;
            if (Math.abs(x - startX) >= range) {
                x = startX + Math.signum(x - startX) * range;
                reverse();
            }
        }
    }

    private void reverse() {
        speed = -speed;
        stop = STOP;
    }

    @Override
    public Rect solidRect() {
        // TODO: the platform is also off for 75 updates after the player is hurt (mega+0xE5); not done (docs/gaps.md, "Caterpirade")
        return new Rect(x - 14, y - 28, 28, 56);
    }

    @Override
    public boolean carries() {
        return true;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public String spriteSet() {
        return "rocket-platform";
    }

    @Override
    public int drawPriority() {
        return 7;
    }
}
