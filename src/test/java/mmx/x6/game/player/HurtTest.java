package mmx.x6.game.player;

import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.Sim;
import org.junit.jupiter.api.Test;

class HurtTest {

    private static final String SKY = "........................................";
    private static final String FLOOR = "########################################";

    private static Sim flat() {
        return Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
    }

    @Test
    void hurtClassFour() {
        Sim s = flat();
        Player p = s.world.player;
        p.hurt(s.world, 4, 150);
        assertEquals(30, p.hp);
        s.step(0);
        assertEquals(PlayerStep.HURT, p.step());
        assertEquals(35, p.anim().anim());
        assertTrue(p.faceRight);
        assertTrue(p.x < 100);
        int reaction = 1;
        while (p.step() == PlayerStep.HURT && reaction < 40) {
            s.step(0);
            reaction++;
        }
        assertTrue(Math.abs(reaction - 16) <= 1, "reaction " + reaction);
        assertTrue(p.invulnerable());
        s.steps(75 - reaction - 1, 0);
        assertTrue(p.invulnerable());
        s.steps(2, 0);
        assertTrue(!p.invulnerable());
        assertEquals(1, s.sfx.count(3, 17));
    }

    @Test
    void noDamageWhileInvulnerable() {
        Sim s = flat();
        Player p = s.world.player;
        p.hurt(s.world, 4, 150);
        s.step(0);
        p.hurt(s.world, 4, 150);
        assertEquals(30, p.hp);
    }

    @Test
    void zeroHpKills() {
        Sim s = flat();
        Player p = s.world.player;
        p.hp = 2;
        p.hurt(s.world, 8, 150);
        s.step(0);
        assertEquals(PlayerStep.DEAD, p.step());
    }

    @Test
    void spikesKill() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY,
                "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^", FLOOR)).playerAt(100, 120);
        s.steps(30, 0);
        assertEquals(PlayerStep.DEAD, s.world.player.step());
    }

    @Test
    void pitKills() {
        String[] rows = new String[30];
        java.util.Arrays.fill(rows, SKY);
        Sim s = Sim.on(grid(rows)).playerAt(100, 160);
        s.world.camera.reset(new mmx.x6.level.Checkpoint(0, 0, 0, 0, 0, 320, 0, 0, true, 1));
        s.steps(80, 0);
        assertEquals(PlayerStep.DEAD, s.world.player.step());
    }
}
