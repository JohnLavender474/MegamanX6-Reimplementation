package mmx.x6.game.player;

import static mmx.x6.input.Buttons.JUMP;
import static mmx.x6.input.Buttons.WEAPON;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.HashSet;
import java.util.Set;
import mmx.x6.anim.AnimSet;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Sim;
import mmx.x6.game.TestTarget;
import mmx.x6.physics.Rect;
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

    private static SabreSlash slash(Sim s) {
        return (SabreSlash) s.world.entities.stream().filter(e -> e instanceof SabreSlash).findFirst().orElseThrow();
    }

    @Test
    void slashAnimationPerPose() {
        Sim floor = flat();
        floor.step(WEAPON);
        floor.steps(4, 0);
        assertEquals("x-sabre", slash(floor).spriteSet());
        assertEquals(0, slash(floor).anim().anim());

        Sim air = flat();
        air.steps(5, JUMP);
        air.step(JUMP | WEAPON);
        air.steps(4, JUMP);
        assertEquals(1, slash(air).anim().anim());
    }

    @Test
    void boxIsTheDrawnBladeOnFlaggedFrames() {
        Sim s = flat();
        s.step(WEAPON);
        s.steps(4, 0);
        SabreSlash slash = slash(s);
        AnimSet set = s.world.animSet("x-sabre");
        Set<Integer> boxed = new HashSet<>();
        for (int i = 0; i < 40 && !slash.dead; i++) {
            Rect box = slash.attackBox();
            int frame = slash.anim().frameIndex();
            Rect b = set.bounds(0, frame);
            if (slash.anim().flag() == 0 || b == null) {
                assertNull(box, "no box on frame " + frame);
            } else {
                boxed.add(frame);
                float left = s.world.player.faceRight ? slash.x - b.right() : slash.x + b.x();
                assertEquals(left, box.x(), 0.01);
                assertEquals(slash.y + b.y(), box.y(), 0.01);
                assertEquals(b.w(), box.w(), 0.01);
                assertEquals(b.h(), box.h(), 0.01);
            }
            s.step(0);
        }
        assertEquals(Set.of(1, 2, 3, 4, 5, 6), boxed);
    }

    @Test
    void floorBladeReachesInFront() {
        Sim s = flat();
        s.step(WEAPON);
        s.steps(4, 0);
        SabreSlash slash = slash(s);
        float reach = 0;
        for (int i = 0; i < 20; i++) {
            Rect box = slash.attackBox();
            if (box != null) {
                reach = Math.max(reach, box.right() - s.world.player.x);
            }
            s.step(0);
        }
        assertTrue(reach >= 40, "reach " + reach);
    }

    @Test
    void slashEndsAtItsHoldFrame() {
        Sim s = flat();
        s.step(WEAPON);
        s.steps(4, 0);
        SabreSlash slash = slash(s);
        s.steps(30, 0);
        assertTrue(slash.dead);
    }

    @Test
    void sabreHitsUpToFour() {
        Sim s = flat();
        TestTarget block = new TestTarget(130, 130, 64, 64, 32, DamageTables.Table.SABRE_ONLY);
        s.world.entities.add(block);
        s.step(WEAPON);
        s.steps(60, 0);
        assertTrue(block.hp < 32, "hp " + block.hp);
        assertTrue(block.hp >= 32 - 4 * 6, "hp " + block.hp);
        assertEquals(0, (32 - block.hp) % 6, "the table value 4 times the custom 1.5");
    }

    @Test
    void floorSabreRecoversInHalfTheHoldTime() {
        Sim s = flat();
        s.step(WEAPON);
        int n = 1;
        while (s.world.player.step() == PlayerStep.SABRE && n < 100) {
            s.step(0);
            n++;
        }
        assertEquals(PlayerStep.STAND, s.world.player.step());
        assertTrue(n <= 26, "updates " + n);
        assertTrue(n >= 20, "updates " + n);
    }

    @Test
    void slashIsDrawnInFrontOfX() {
        Sim s = flat();
        s.step(WEAPON);
        s.steps(4, 0);
        assertTrue(slash(s).drawPriority() < 0);
    }
}
