package mmx.x6.game.boss;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.physics.Rect;

public final class FirePillar extends Entity {

    private static final int LIFE = 50;

    private int age;

    public FirePillar(World w, float x, float landingY) {
        this.x = x;
        this.y = landingY - 78;
        contactDamage = 6;
        anim = new Animator(w.animSet("d1000"));
        anim.play(20);
    }

    @Override
    public void update(World w) {
        age++;
        anim.tick();
        if (anim.anim() == 20 && anim.finished()) {
            anim.play(21);
        }
        if (age == LIFE) {
            anim.play(22);
        }
        if (age > LIFE && anim.finished()) {
            dead = true;
        }
    }

    @Override
    public Rect contactBox() {
        return age <= LIFE ? new Rect(x - 16, y - 128, 32, 255) : null;
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
