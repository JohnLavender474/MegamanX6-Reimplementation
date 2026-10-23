package mmx.x6.game.fx;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;

public final class Debris extends Entity {

    private static final float[] SPEED_X = {-3.0f, -2.0f, 1.5f, 2.5f, -3.5f, -2.5f, 2.0f, 3.0f};
    private static final float[] SPEED_Y = {3.5f, 4.5f, 6.0f, 3.0f, 4.0f, 5.0f, 5.5f, 2.5f};
    private static final float GRAVITY = 0.3125f;
    private static final float FALL_MAX = -6.5f;

    private final String set;
    private float speedX;
    private float speedY;
    private int age;

    public Debris(World w, String set, int animIndex, float x, float y, float parentSpeedX) {
        this.set = set;
        this.x = x + w.rng.nextInt(4);
        this.y = y + w.rng.nextInt(4);
        this.faceRight = w.rng.nextBoolean();
        this.speedX = SPEED_X[w.rng.nextInt(8)] + parentSpeedX / 2;
        this.speedY = SPEED_Y[w.rng.nextInt(8)];
        this.anim = new Animator(w.animSet(set));
        anim.play(animIndex);
    }

    public static void spawn(World w, String set, int[] anims, float x, float y) {
        for (int a : anims) {
            w.add(new Debris(w, set, a, x, y, 0));
        }
    }

    @Override
    public void update(World w) {
        age++;
        x += speedX;
        y -= speedY;
        speedY = Math.max(FALL_MAX, speedY - GRAVITY);
        anim.tick();
        if (w.offScreen(x, y, 64, 64)) {
            dead = true;
        }
    }

    @Override
    public boolean visible() {
        return (age & 1) == 0;
    }

    @Override
    public String spriteSet() {
        return set;
    }

    @Override
    public int drawPriority() {
        return -5;
    }
}
