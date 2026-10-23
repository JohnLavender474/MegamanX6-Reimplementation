package mmx.x6.game.fx;

import java.util.List;
import mmx.x6.game.Entity;
import mmx.x6.game.ShapeFx;
import mmx.x6.game.World;

public final class Explosion extends Entity {

    private static final int LIFE = 16;
    private static boolean toggle;

    private int age;
    private final float scale;

    public Explosion(World w, float x, float y) {
        this(x, y, 1f);
        toggle = !toggle;
        // TODO: no explosion sprites are exported; drawn as circles (docs/gaps.md, "Explosions")
        w.sfx.play(0, toggle ? 0 : 1);
    }

    private Explosion(float x, float y, float scale) {
        this.x = x;
        this.y = y;
        this.scale = scale;
    }

    public static Explosion spark(float x, float y) {
        return new Explosion(x, y, 0.35f);
    }

    public static void around(World w, float x, float y, int count, float spread) {
        for (int i = 0; i < count; i++) {
            float dx = (w.rng.nextFloat() * 2 - 1) * spread;
            float dy = (w.rng.nextFloat() * 2 - 1) * spread;
            w.add(new Explosion(w, x + dx, y + dy));
        }
    }

    @Override
    public void update(World w) {
        if (++age >= LIFE) {
            dead = true;
        }
    }

    @Override
    public int drawPriority() {
        return -10;
    }

    @Override
    public List<ShapeFx> shapes() {
        float t = age / (float) LIFE;
        float radius = (4 + 14 * t) * scale;
        float alpha = 1 - t;
        return List.of(
                new ShapeFx(x, y, radius, 1f, 0.55f, 0.1f, alpha),
                new ShapeFx(x, y, radius * 0.6f, 1f, 0.95f, 0.6f, alpha));
    }
}
