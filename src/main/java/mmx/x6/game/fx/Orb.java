package mmx.x6.game.fx;

import java.util.List;
import mmx.x6.game.Entity;
import mmx.x6.game.ShapeFx;
import mmx.x6.game.World;

public final class Orb extends Entity {

    private static final float SPEED = 2.5f;

    private final float vx;
    private final float vy;
    private int age;

    public Orb(float x, float y, int direction32) {
        this.x = x;
        this.y = y;
        double a = direction32 * Math.PI * 2 / 32;
        // TODO: the death orb object (misc type 17) is not read; speed and look are guesses (docs/gaps.md, "Death orbs")
        this.vx = (float) Math.cos(a) * SPEED;
        this.vy = (float) Math.sin(a) * SPEED;
    }

    public static void wave(World w, float x, float y, int first) {
        for (int i = 0; i < 8; i++) {
            w.add(new Orb(x, y, first + i * 4));
        }
    }

    @Override
    public void update(World w) {
        age++;
        x += vx;
        y += vy;
        if (age > 120) {
            dead = true;
        }
    }

    @Override
    public List<ShapeFx> shapes() {
        float pulse = (age / 3) % 2 == 0 ? 1f : 0.7f;
        return List.of(new ShapeFx(x, y, 6, 0.3f, 0.6f, 1f, 0.8f * pulse), new ShapeFx(x, y, 3, 1f, 1f, 1f, pulse));
    }
}
