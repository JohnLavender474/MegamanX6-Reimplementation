package mmx.x6.game;

public final class Damage {

    public static final int NO_HIT = 0;
    public static final int BLOCKED = 127;

    private Damage() {
    }

    public static int toPlayer(int contactDamage) {
        if (contactDamage <= 0 || contactDamage == 127) {
            return Math.max(0, contactDamage);
        }
        return Math.max(1, contactDamage / 2);
    }

    public static int hitObject(Entity e, int weaponId) {
        if (e.damageTable == null) {
            return NO_HIT;
        }
        int v = DamageTables.value(e.damageTable, weaponId);
        if (v == DamageTables.NO_EFFECT) {
            return NO_HIT;
        }
        if (v == DamageTables.DEFLECT) {
            return BLOCKED;
        }
        e.hp -= v;
        e.flash = 1;
        return e.hp <= 0 ? -(weaponId + 1) : weaponId + 1;
    }
}
