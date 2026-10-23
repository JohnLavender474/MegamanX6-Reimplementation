package mmx.x6.game.boss;

import mmx.x6.game.Entity;
import mmx.x6.game.World;

public final class BossWarning extends Entity {

    public static final int DURATION = 470;

    private int age;

    @Override
    public void update(World w) {
        age++;
        if (age == 6 || (age > 6 && (age - 6) % 61 == 0)) {
            w.sfx.play(0, 19);
        }
        if (age >= DURATION) {
            dead = true;
        }
    }

    public boolean textVisible() {
        return (age / 16) % 2 == 0;
    }

    @Override
    public boolean visible() {
        return false;
    }
}
