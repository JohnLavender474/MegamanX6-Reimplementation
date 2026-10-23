package mmx.x6.game.player;

import mmx.x6.game.Damage;
import mmx.x6.game.Entity;
import mmx.x6.game.Weapon;
import mmx.x6.game.World;
import mmx.x6.physics.Rect;

public final class SabreSlash extends Entity implements Weapon {

    private static final int MAX_HITS = 4;
    private static final int NO_BOX_AFTER_HIT = 4;

    private final Player player;
    private final int weaponId;
    private int hitsLeft = MAX_HITS;
    private int noBox;

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
}
