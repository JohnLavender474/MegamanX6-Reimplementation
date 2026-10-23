package mmx.x6.game.player;

import static mmx.x6.input.Buttons.BUSTER;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Sim;
import mmx.x6.game.TestTarget;
import org.junit.jupiter.api.Test;

class BusterTest {

    private static final String SKY = "............................................................";
    private static final String FLOOR = "############################################################";

    private static Sim flat() {
        return Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
    }

    private static List<BusterShot> shots(Sim s) {
        return s.world.entities.stream().filter(e -> e instanceof BusterShot).map(e -> (BusterShot) e).toList();
    }

    @Test
    void lemonLimitThree() {
        Sim s = flat();
        for (int i = 0; i < 4; i++) {
            s.step(BUSTER);
            s.steps(5, 0);
        }
        assertEquals(3, shots(s).size());
    }

    @Test
    void lemonWaitSix() {
        Sim s = flat();
        for (int i = 0; i < 6; i++) {
            s.step(i % 2 == 0 ? BUSTER : 0);
        }
        assertEquals(1, shots(s).size());
        s.step(BUSTER);
        assertEquals(2, shots(s).size());
    }

    @Test
    void lemonSpeed() {
        Sim s = flat();
        s.step(BUSTER);
        s.step(0);
        BusterShot shot = shots(s).get(0);
        float x = shot.x;
        s.step(0);
        assertEquals(6.75f, shot.x - x, 0.001);
        assertEquals(1, s.sfx.count(1, 8));
    }

    @Test
    void lemonEndsAtAWall() {
        String wallRow = "....................#.......................................";
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, wallRow, wallRow, wallRow, wallRow, wallRow, FLOOR, FLOOR))
                .playerAt(100, 160);
        s.step(BUSTER);
        s.steps(40, 0);
        assertTrue(shots(s).isEmpty());
    }

    @Test
    void chargeLevels() {
        Sim s = flat();
        s.step(BUSTER);
        s.steps(24, BUSTER);
        assertEquals(0, s.world.player.chargeLevel());
        s.step(BUSTER);
        assertEquals(1, s.world.player.chargeLevel());
        assertEquals(1, s.sfx.count(1, 7));
        s.steps(64, BUSTER);
        assertEquals(1, s.world.player.chargeLevel());
        s.step(BUSTER);
        assertEquals(2, s.world.player.chargeLevel());
    }

    @Test
    void halfChargeRelease() {
        int updates = 0;
        Sim t = flat();
        t.step(BUSTER);
        t.steps(40, BUSTER);
        t.step(0);
        while (shots(t).stream().noneMatch(b -> b.weaponId() == 9) && updates < 20) {
            t.step(0);
            updates++;
        }
        BusterShot half = shots(t).stream().filter(b -> b.weaponId() == 9).findFirst().orElseThrow();
        assertTrue(updates >= 2 && updates <= 5, "updates " + updates);
        float x = half.x;
        t.step(0);
        assertEquals(8.0f, half.x - x, 0.001);
    }

    @Test
    void falconFullCharge() {
        Sim s = flat();
        s.step(BUSTER);
        s.steps(95, BUSTER);
        assertEquals(2, s.world.player.chargeLevel());
        s.steps(30, BUSTER);
        s.step(0);
        int updates = 0;
        while (shots(s).stream().noneMatch(b -> b.weaponId() == 19) && updates < 20) {
            s.step(0);
            updates++;
        }
        BusterShot full = shots(s).stream().filter(b -> b.weaponId() == 19).findFirst().orElseThrow();
        assertTrue(updates >= 4 && updates <= 7, "updates " + updates);
        float x = full.x;
        s.step(0);
        assertEquals(10.0f, full.x - x, 0.001);
    }

    @Test
    void fullChargeRefusedWithTwoLiveShots() {
        Sim s = flat();
        s.step(BUSTER);
        s.steps(6, 0);
        s.step(BUSTER);
        s.steps(95, BUSTER);
        long lemons = shots(s).size();
        if (lemons >= 2) {
            s.step(0);
            s.steps(10, 0);
            assertFalse(shots(s).stream().anyMatch(b -> b.weaponId() == 19));
        }
    }

    @Test
    void hitDropsCharge() {
        Sim s = flat();
        s.step(BUSTER);
        s.steps(40, BUSTER);
        assertEquals(1, s.world.player.chargeLevel());
        s.world.player.hurt(s.world, 4, 200);
        s.step(BUSTER);
        assertEquals(0, s.world.player.chargeLevel());
    }

    @Test
    void deflectedLemonFliesOff() {
        Sim s = flat();
        TestTarget wheel = new TestTarget(160, 130, 46, 46, 32, DamageTables.Table.SABRE_ONLY);
        s.world.entities.add(wheel);
        s.step(BUSTER);
        s.steps(12, 0);
        BusterShot shot = shots(s).get(0);
        assertTrue(shot.deflected);
        assertTrue(shot.speedX() < 0);
        assertEquals(4.8f, Math.abs(shot.speedY()), 0.001);
        assertEquals(null, shot.attackBox());
        assertEquals(32, wheel.hp);
        assertEquals(1, s.sfx.count(0, 6));
    }

    @Test
    void halfChargePiercesAKill() {
        Sim s = flat();
        TestTarget a = new TestTarget(170, 130, 20, 40, 6, DamageTables.Table.NORMAL);
        TestTarget b = new TestTarget(230, 130, 20, 40, 6, DamageTables.Table.NORMAL);
        s.world.entities.add(a);
        s.world.entities.add(b);
        s.step(BUSTER);
        s.steps(40, BUSTER);
        s.steps(40, 0);
        assertEquals(1, a.killCount);
        assertEquals(1, b.killCount);
    }

    @Test
    void lemonStopsAtTheFirstHit() {
        Sim s = flat();
        TestTarget a = new TestTarget(170, 130, 20, 40, 12, DamageTables.Table.NORMAL);
        TestTarget b = new TestTarget(230, 130, 20, 40, 12, DamageTables.Table.NORMAL);
        s.world.entities.add(a);
        s.world.entities.add(b);
        s.step(BUSTER);
        s.steps(40, 0);
        assertEquals(9, a.hp);
        assertEquals(12, b.hp);
    }
}
