package mmx.x6.game;

import mmx.x6.physics.Rect;

public interface Weapon {

    int weaponId();

    default float damageScale() {
        return 1;
    }

    Rect attackBox();

    boolean canHit(Entity target);

    void onResult(World w, Entity target, int result);
}
