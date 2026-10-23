package mmx.x6.game.player;

import java.util.ArrayList;
import java.util.List;
import mmx.x6.game.Damage;
import mmx.x6.game.ShapeFx;
import mmx.x6.game.Entity;
import mmx.x6.game.Weapon;
import mmx.x6.game.World;
import mmx.x6.physics.Rect;

public final class SabreSlash extends Entity implements Weapon {

    private static final int MAX_HITS = 4;
    private static final int NO_BOX_AFTER_HIT = 4;

    private final Player player;
    private final int weaponId;
    private static final int SWING_TIME = 6;
    private static final int SWING_END = 14;
    private static final int ARC_POINTS = 9;
    private static final float ARC_FROM = -80;
    private static final float ARC_TO = 50;
    private static final float ARC_RADIUS = 30;

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
        // TODO: the slash box comes "from the frame flag" with no numbers in the docs (docs/gaps.md, "Sabre hit box")
        return faceRight ? new Rect(x - 8, y - 32, 56, 48) : new Rect(x + 8 - 56, y - 32, 56, 48);
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
    public List<ShapeFx> shapes() {
        if (age >= SWING_END) {
            return List.of();
        }
        // TODO: X's sabre swing sprites are not exported; a green crescent of circles stands in (docs/gaps.md, "Sabre swing")
        float sweep = Math.min(1f, (age + 1) / (float) SWING_TIME);
        float alpha = age < SWING_TIME + 2 ? 1f : 1f - (age - SWING_TIME - 2) / (float) (SWING_END - SWING_TIME - 2);
        float face = faceRight ? 1 : -1;
        float cx = x + face * 4;
        float cy = y - 6;
        List<ShapeFx> out = new ArrayList<>();
        for (int i = 0; i < ARC_POINTS; i++) {
            float t = i / (float) (ARC_POINTS - 1);
            if (t > sweep) {
                break;
            }
            double a = Math.toRadians(ARC_FROM + (ARC_TO - ARC_FROM) * t);
            float px = cx + face * (float) Math.cos(a) * ARC_RADIUS;
            float py = cy + (float) Math.sin(a) * ARC_RADIUS;
            float lead = 1f - (sweep - t);
            out.add(new ShapeFx(px, py, 5, 0.2f, 0.9f, 0.4f, 0.35f * alpha * lead));
            out.add(new ShapeFx(px, py, 2.5f, 0.75f, 1f, 0.8f, alpha * lead));
        }
        return out;
    }
}
