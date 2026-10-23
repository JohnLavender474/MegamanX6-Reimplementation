package mmx.x6.game.player;

import static mmx.x6.input.Buttons.JUMP;
import static mmx.x6.input.Buttons.WEAPON;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.DamageTables;
import mmx.x6.game.Sim;
import mmx.x6.game.TestTarget;
import org.junit.jupiter.api.Test;

class SabreTest {

    private static final String SKY = "........................................";
    private static final String FLOOR = "########################################";

    private static Sim flat() {
        return Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
    }

    @Test
    void sabreFloor() {
        Sim s = flat();
        s.step(WEAPON);
        assertEquals(PlayerStep.SABRE, s.world.player.step());
        assertEquals(183, s.world.player.anim().anim());
        s.steps(4, 0);
        assertTrue(s.world.entities.stream().anyMatch(e -> e instanceof SabreSlash sl && sl.weaponId() == 50));
        s.steps(60, 0);
        assertEquals(PlayerStep.STAND, s.world.player.step());
        assertTrue(s.world.entities.stream().noneMatch(e -> e instanceof SabreSlash));
    }

    @Test
    void sabreAir() {
        Sim s = flat();
        s.steps(5, JUMP);
        s.step(JUMP | WEAPON);
        assertEquals(184, s.world.player.anim().anim());
        s.steps(4, JUMP);
        assertTrue(s.world.entities.stream().anyMatch(e -> e instanceof SabreSlash sl && sl.weaponId() == 51));
    }

    @Test
    void swingIsDrawnInFrontThenFades() {
        Sim s = flat();
        s.step(WEAPON);
        s.steps(4, 0);
        SabreSlash slash = (SabreSlash) s.world.entities.stream().filter(e -> e instanceof SabreSlash)
                .findFirst().orElseThrow();
        s.step(0);
        assertTrue(!slash.shapes().isEmpty());
        float px = s.world.player.x;
        assertTrue(slash.shapes().stream().allMatch(f -> f.x() >= px - 12), "in front of X");
        assertTrue(slash.shapes().stream().allMatch(f -> f.g() > f.r()), "green blade");
        s.steps(20, 0);
        assertTrue(slash.dead || slash.shapes().isEmpty());
    }

    @Test
    void sabreHitsUpToFour() {
        Sim s = flat();
        TestTarget block = new TestTarget(130, 130, 64, 64, 32, DamageTables.Table.SABRE_ONLY);
        s.world.entities.add(block);
        s.step(WEAPON);
        s.steps(60, 0);
        assertTrue(block.hp < 32, "hp " + block.hp);
        assertTrue(block.hp >= 32 - 4 * 4, "hp " + block.hp);
        assertEquals(0, (32 - block.hp) % 4);
    }
}
