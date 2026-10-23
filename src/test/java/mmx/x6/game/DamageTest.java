package mmx.x6.game;

import static mmx.x6.game.DamageTables.DEFLECT;
import static mmx.x6.game.DamageTables.NO_EFFECT;
import static mmx.x6.game.DamageTables.Table.D1000;
import static mmx.x6.game.DamageTables.Table.MINOR;
import static mmx.x6.game.DamageTables.Table.NORMAL;
import static mmx.x6.game.DamageTables.Table.SABRE_ONLY;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class DamageTest {

    @Test
    void playerDamageHalvedForArmor() {
        assertEquals(2, Damage.toPlayer(4));
        assertEquals(1, Damage.toPlayer(1));
        assertEquals(3, Damage.toPlayer(6));
        assertEquals(4, Damage.toPlayer(8));
        assertEquals(1, Damage.toPlayer(3));
        assertEquals(0, Damage.toPlayer(0));
        assertEquals(127, Damage.toPlayer(127));
    }

    @Test
    void tables() {
        assertEquals(3, DamageTables.value(NORMAL, 0));
        assertEquals(15, DamageTables.value(NORMAL, 19));
        assertEquals(4, DamageTables.value(NORMAL, 50));
        assertEquals(127, DamageTables.value(NORMAL, 55));
        assertEquals(DEFLECT, DamageTables.value(SABRE_ONLY, 0));
        assertEquals(4, DamageTables.value(SABRE_ONLY, 50));
        assertEquals(6, DamageTables.value(SABRE_ONLY, 51));
        assertEquals(NO_EFFECT, DamageTables.value(SABRE_ONLY, 7));
        assertEquals(DEFLECT, DamageTables.value(D1000, 0));
        assertEquals(DEFLECT, DamageTables.value(D1000, 9));
        assertEquals(1, DamageTables.value(D1000, 19));
        assertEquals(5, DamageTables.value(D1000, 18));
        assertEquals(1, DamageTables.value(D1000, 50));
        assertEquals(NO_EFFECT, DamageTables.value(DamageTables.Table.DEFLECT, 7));
        assertEquals(DEFLECT, DamageTables.value(DamageTables.Table.DEFLECT, 0));
        assertEquals(3, DamageTables.value(MINOR, 0));
        assertEquals(DEFLECT, DamageTables.value(NORMAL, 64));
    }

    @Test
    void hitObjectResults() {
        TestTarget t = new TestTarget(0, 0, 10, 10, 5, NORMAL);
        assertEquals(1, Damage.hitObject(t, 0));
        assertEquals(2, t.hp);
        assertEquals(-1, Damage.hitObject(t, 0));
        TestTarget wheel = new TestTarget(0, 0, 10, 10, 32, SABRE_ONLY);
        assertEquals(127, Damage.hitObject(wheel, 0));
        assertEquals(32, wheel.hp);
        assertEquals(0, Damage.hitObject(wheel, 7));
        assertEquals(51, Damage.hitObject(wheel, 50));
        assertEquals(28, wheel.hp);
    }
}
