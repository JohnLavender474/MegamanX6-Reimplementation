package mmx.x6.game.player;

import java.util.List;
import mmx.x6.game.CrescentFx;
import mmx.x6.game.Damage;
import mmx.x6.game.Entity;
import mmx.x6.game.Weapon;
import mmx.x6.game.World;
import mmx.x6.physics.Rect;

public final class SabreSlash extends Entity implements Weapon {

    private static final int MAX_HITS = 4;
    private static final int NO_BOX_AFTER_HIT = 4;
    private static final int HEAD_TIME = 5;
    private static final int SWING_END = 14;
    private static final float ARC_FROM = -72;
    private static final float ARC_TO = 60;
    private static final float ARC_RADIUS = 50;
    private static final float THICKNESS = 20;
    private static final float CENTRE_FORWARD = 14;
    private static final float CENTRE_UP = 14;
    private static final int BLADE_LAYER = 1;
    private static final int OUTLINE_SEGMENTS = 18;

    private final Player player;
    private final int weaponId;
    private int hitsLeft = MAX_HITS;
    private int noBox;
    private int age;

    public SabreSlash(Player player, int weaponId) {
        this.player = player;
        this.weaponId = weaponId;
        this.x = player.x;
        this.y = player.y;
    }

    @Override
    public int weaponId() {
        return weaponId;
    }

    @Override
    public void update(World w) {
        x = player.x;
        y = player.y;
        faceRight = player.faceRight;
        age++;
        if (noBox > 0) {
            noBox--;
        }
    }

    @Override
    public Rect attackBox() {
        if (noBox > 0 || hitsLeft <= 0) {
            return null;
        }
        List<CrescentFx> layers = crescents();
        if (layers.isEmpty()) {
            return null;
        }
        // TODO: the slash box comes "from the frame flag" with no numbers in the docs; it follows the drawn blade (docs/gaps.md, "Sabre hit box")
        float minX = Float.POSITIVE_INFINITY;
        float maxX = Float.NEGATIVE_INFINITY;
        float minY = Float.POSITIVE_INFINITY;
        float maxY = Float.NEGATIVE_INFINITY;
        for (float[] p : layers.get(BLADE_LAYER).outline(OUTLINE_SEGMENTS)) {
            minX = Math.min(minX, Math.min(p[0], p[2]));
            maxX = Math.max(maxX, Math.max(p[0], p[2]));
            minY = Math.min(minY, Math.min(p[1], p[3]));
            maxY = Math.max(maxY, Math.max(p[1], p[3]));
        }
        return new Rect(minX, minY, maxX - minX, maxY - minY);
    }

    @Override
    public boolean canHit(Entity target) {
        return noBox == 0 && hitsLeft > 0;
    }

    @Override
    public void onResult(World w, Entity target, int result) {
        if (result == Damage.NO_HIT) {
            return;
        }
        hitsLeft--;
        noBox = NO_BOX_AFTER_HIT;
    }

    @Override
    public boolean visible() {
        return false;
    }

    @Override
    public List<CrescentFx> crescents() {
        if (age >= SWING_END) {
            return List.of();
        }
        // TODO: X's sabre swing sprites are not exported; a ShapeRenderer crescent stands in (docs/gaps.md, "Sabre swing")
        float head = Math.min(1f, (age + 1) / (float) HEAD_TIME);
        float tail = age < HEAD_TIME ? 0f : (age - HEAD_TIME + 1) / (float) (SWING_END - HEAD_TIME);
        float span = ARC_TO - ARC_FROM;
        float from = ARC_FROM + span * Math.min(tail, head);
        float to = ARC_FROM + span * head;
        float shape = Math.max(0.25f, (to - from) / span);
        float alpha = age < HEAD_TIME + 3 ? 1f : 1f - (age - HEAD_TIME - 3) / (float) (SWING_END - HEAD_TIME - 3);
        float face = faceRight ? 1 : -1;
        float cx = x + face * CENTRE_FORWARD;
        float cy = y - CENTRE_UP;
        return List.of(
                new CrescentFx(cx, cy, ARC_RADIUS + 3, (THICKNESS + 6) * shape, from, to, faceRight,
                        0.15f, 0.95f, 0.35f, 0.3f * alpha),
                new CrescentFx(cx, cy, ARC_RADIUS, THICKNESS * shape, from, to, faceRight,
                        0.45f, 1f, 0.55f, 0.85f * alpha),
                new CrescentFx(cx, cy, ARC_RADIUS - 1, THICKNESS * 0.4f * shape, from, to, faceRight,
                        0.9f, 1f, 0.92f, alpha));
    }
}
