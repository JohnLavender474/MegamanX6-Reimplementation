package mmx.x6.game.fx;

import java.util.List;
import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.ShapeFx;
import mmx.x6.game.World;

public final class Explosion extends Entity {

    private static final int SPARK_LIFE = 16;
    private static final float SPARK_SCALE = 0.35f;
    private static boolean toggle;

    private int age;
    private final boolean spark;

    public Explosion(World w, float x, float y) {
        this(x, y, false);
        this.anim = new Animator(w.animSet("explosion"));
        anim.play(0);
        toggle = !toggle;
        w.sfx.play(0, toggle ? 0 : 1);
    }

    private Explosion(float x, float y, boolean spark) {
        this.x = x;
        this.y = y;
        this.spark = spark;
    }

    public static Explosion spark(float x, float y) {
        return new Explosion(x, y, true);
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
        if (spark) {
            if (++age >= SPARK_LIFE) {
                dead = true;
            }
            return;
        }
        anim.tick();
        if (anim.atLastFrame()) {
            dead = true;
        }
    }

    @Override
    public String spriteSet() {
        return spark ? null : "explosion";
    }

    @Override
    public boolean visible() {
        // TODO: frame 0 is a flash frame drawn with the fixed flash CLUT 0x7840, which the export does not draw; it is skipped (docs/gaps.md, "Explosions")
        return spark || anim.frameIndex() > 0;
    }

    @Override
    public int drawPriority() {
        return -10;
    }

    @Override
    public List<ShapeFx> shapes() {
        if (!spark) {
            return List.of();
        }
        // TODO: the shot spark (effect type 3) is not exported; drawn as circles (docs/gaps.md, "Explosions")
        float t = age / (float) SPARK_LIFE;
        float radius = (4 + 14 * t) * SPARK_SCALE;
        float alpha = 1 - t;
        return List.of(
                new ShapeFx(x, y, radius, 1f, 0.55f, 0.1f, alpha),
                new ShapeFx(x, y, radius * 0.6f, 1f, 0.95f, 0.6f, alpha));
    }
}
