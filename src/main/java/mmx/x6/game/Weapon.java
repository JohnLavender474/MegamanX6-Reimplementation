package mmx.x6.game;

import mmx.x6.physics.Rect;

public interface Weapon {

    int weaponId();

    Rect attackBox();

    boolean canHit(Entity target);

    void onResult(World w, Entity target, int result);
}
