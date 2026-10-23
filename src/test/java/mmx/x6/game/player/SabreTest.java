package mmx.x6.game.player;

import static mmx.x6.input.Buttons.JUMP;
import static mmx.x6.input.Buttons.WEAPON;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import mmx.x6.game.CrescentFx;
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
        List<CrescentFx> early = slash.crescents();
        assertEquals(3, early.size(), "glow, blade and core layers");
        assertTrue(early.stream().allMatch(c -> c.faceRight() == s.world.player.faceRight));
        assertTrue(early.stream().allMatch(c -> c.g() >= c.r()), "green or white blade");
        float firstSpan = early.get(0).toDeg() - early.get(0).fromDeg();
        s.steps(3, 0);
        float laterSpan = slash.crescents().get(0).toDeg() - slash.crescents().get(0).fromDeg();
        assertTrue(laterSpan > firstSpan, "the leading edge sweeps " + firstSpan + " -> " + laterSpan);
        assertTrue(slash.shapes().isEmpty(), "no circle chain any more");
        s.steps(20, 0);
        assertTrue(slash.dead || slash.crescents().isEmpty());
    }

    @Test
    void crescentPointsAreInFrontAndTaperAtTheEnds() {
        CrescentFx c = new CrescentFx(100, 50, 30, 10, -80, 50, true, 0.5f, 1f, 0.6f, 1f);
        List<float[]> pts = c.outline(16);
        assertEquals(17, pts.size());
        assertTrue(pts.stream().allMatch(p -> p[0] >= 100 - 1e-3), "outer edge in front");
        float[] mid = pts.get(8);
        float[] end = pts.get(0);
        assertEquals(0, Math.hypot(end[0] - end[2], end[1] - end[3]), 1e-3);
        assertTrue(Math.hypot(mid[0] - mid[2], mid[1] - mid[3]) > 9, "thick in the middle");
        CrescentFx left = new CrescentFx(100, 50, 30, 10, -80, 50, false, 0.5f, 1f, 0.6f, 1f);
        assertTrue(left.outline(16).stream().allMatch(p -> p[0] <= 100 + 1e-3), "mirrored when facing left");
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
