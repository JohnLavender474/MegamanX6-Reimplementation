package mmx.x6.game.boss;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.physics.Rect;

public final class D1000Bomb extends Entity {

    private static final float GRAVITY = 0.25f;

    private final float floorY;
    private final float vx;
    private float speedY;

    public D1000Bomb(World w, float x, float y, float targetX, float floorY) {
        this.x = x;
        this.y = y;
        this.floorY = floorY;
        this.vx = (targetX - x) / 32f;
        contactDamage = 6;
        anim = new Animator(w.animSet("d1000"));
        anim.play(14);
    }

    @Override
    public void update(World w) {
        anim.tick();
        x += vx;
        y -= speedY;
        speedY -= GRAVITY;
        if (y >= floorY) {
            dead = true;
            w.sfx.play(2, 5);
            w.add(new FirePillar(w, x, floorY));
        }
    }

    @Override
    public Rect contactBox() {
        return new Rect(x - 8, y - 8, 16, 16);
    }

    @Override
    public boolean clearedByGate() {
        return true;
    }

    @Override
    public String spriteSet() {
        return "d1000";
    }

    @Override
    public int drawPriority() {
        return 2;
    }
}
