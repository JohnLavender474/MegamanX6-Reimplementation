package mmx.x6.game;

import mmx.x6.physics.Rect;

public interface Solid {

    Rect solidRect();

    default boolean carries() {
        return false;
    }

    default boolean skipsWhileSqueezed() {
        return true;
    }
}
