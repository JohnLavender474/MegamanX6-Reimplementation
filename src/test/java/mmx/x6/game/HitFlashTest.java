package mmx.x6.game;

import static mmx.x6.input.Buttons.BUSTER;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class HitFlashTest {

    private static final String SKY = "........................................";
    private static final String FLOOR = "########################################";

    @Test
    void hitFlashLastsOneUpdate() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
        TestTarget t = new TestTarget(160, 140, 16, 32, 32, DamageTables.Table.NORMAL);
        s.world.entities.add(t);
        s.step(BUSTER);
        int guard = 0;
        while (t.hp == 32 && guard++ < 60) {
            s.step(0);
        }
        assertEquals(1, t.flash);
        s.step(0);
        assertEquals(0, t.flash);
    }
}
