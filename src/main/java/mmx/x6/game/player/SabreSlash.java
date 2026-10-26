package mmx.x6.game.player;

import mmx.x6.anim.Animator;
import mmx.x6.game.CustomChanges;
import mmx.x6.game.Damage;
import mmx.x6.game.Entity;
import mmx.x6.game.Weapon;
import mmx.x6.game.World;
import mmx.x6.physics.Rect;

public final class SabreSlash extends Entity implements Weapon {

    public static final int FLOOR_ANIM = 0;
    public static final int AIR_ANIM = 1;
    public static final int WALL_ANIM = 4;

    private static final int MAX_HITS = 4;
    private static final int NO_BOX_AFTER_HIT = 4;

    private final Player player;
    private final int weaponId;
    private int hitsLeft = MAX_HITS;
    private int noBox;

    public SabreSlash(World w, Player player, int weaponId, int slashAnim) {
        this.player = player;
        this.weaponId = weaponId;
        this.x = player.x;
        this.y = player.y;
        this.faceRight = player.faceRight;
        this.anim = new Animator(w.animSet("x-sabre"));
        anim.play(slashAnim);
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
        anim.tick();
        if (anim.atLastFrame()) {
            dead = true;
        }
        if (noBox > 0) {
            noBox--;
        }
    }

    @Override
    public float damageScale() {
        return CustomChanges.SABRE_DAMAGE_SCALE;
    }

    @Override
    public int drawPriority() {
        return -1;
    }

    @Override
    public Rect attackBox() {
        if (noBox > 0 || hitsLeft <= 0 || anim.flag() == 0) {
            return null;
        }
        // TODO: the slash box comes "from the frame flag" with no numbers in the docs; it is the drawn blade of the frame (docs/gaps.md, "Sabre hit box")
        Rect b = anim.set().bounds(anim.anim(), anim.frameIndex());
        if (b == null) {
            return null;
        }
        float left = faceRight ? x - b.right() : x + b.x();
        return new Rect(left, y + b.y(), b.w(), b.h());
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
    public String spriteSet() {
        return "x-sabre";
    }
}
