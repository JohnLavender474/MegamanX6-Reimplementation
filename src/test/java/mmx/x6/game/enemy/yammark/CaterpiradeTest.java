package mmx.x6.game.enemy.yammark;

import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Arrays;
import mmx.x6.game.Sim;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class CaterpiradeTest {

    private static final String SKY = "................................................................";
    private static final String FLOOR = "################################################################";
    private static final float FLOOR_Y = 160;

    private static Sim flat(float playerX) {
        String[] rows = new String[16];
        Arrays.fill(rows, SKY);
        rows[10] = FLOOR;
        rows[11] = FLOOR;
        Sim s = Sim.on(grid(rows)).playerAt(playerX, FLOOR_Y);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        return s;
    }

    private static Caterpirade crawler(Sim s, float x) {
        Caterpirade c = new Caterpirade(s.world, new RecordDef(7, "Caterpirade", "main", 0, 7, 0, 0, (int) x, 140));
        s.world.entities.add(c);
        s.steps(30, 0);
        return c;
    }

    private static float feet(Sim s) {
        return s.world.player.hurtBox().bottom();
    }

    @Test
    void crawlingIntoXDoesNotPushHimIntoTheFloor() {
        Sim s = flat(100);
        crawler(s, 160);
        int hp = s.world.player.hp;
        for (int i = 0; i < 240; i++) {
            s.step(0);
            assertTrue(feet(s) <= FLOOR_Y + 0.01f, "feet " + feet(s) + " at update " + i);
        }
        assertTrue(s.world.player.hp < hp);
    }

    @Test
    void xRidesOnItsBack() {
        Sim s = flat(40);
        Caterpirade c = crawler(s, 160);
        float top = c.y - 14;
        s.world.player.placeStanding(c.x, top - 21);
        float x0 = c.x;
        s.steps(60, 0);
        assertEquals(top, feet(s), 0.5f);
        assertTrue(Math.abs(c.x - x0) > 10);
        assertEquals(c.x, s.world.player.x, 1.5f);
    }
}
