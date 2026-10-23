package mmx.x6.game.player;

import static mmx.x6.input.Buttons.BUSTER;
import static mmx.x6.input.Buttons.DOWN;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;

import mmx.x6.game.Sim;
import org.junit.jupiter.api.Test;

class CrouchShotTest {

    private static final String SKY = "............................................................";
    private static final String FLOOR = "############################################################";

    private static Sim crouched() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
        s.steps(10, DOWN);
        assertEquals(PlayerStep.CROUCH, s.world.player.step());
        assertEquals(81, s.world.player.anim().anim());
        return s;
    }

    @Test
    void lemonShowsTheCrouchShot() {
        Sim s = crouched();
        s.step(DOWN | BUSTER);
        assertEquals(108, s.world.player.anim().anim());
    }

    @Test
    void crouchedFrameComesBackAfterThePose() {
        Sim s = crouched();
        s.step(DOWN | BUSTER);
        s.steps(25, DOWN);
        assertEquals(81, s.world.player.anim().anim());
        assertEquals(2, s.world.player.anim().frameIndex());
    }

    @Test
    void standUpInThePoseUses110() {
        Sim s = crouched();
        s.step(DOWN | BUSTER);
        s.step(0);
        assertEquals(110, s.world.player.anim().anim());
    }

    @Test
    void standUpWithoutThePoseUses82() {
        Sim s = crouched();
        s.step(0);
        assertEquals(82, s.world.player.anim().anim());
    }

    @Test
    void crouchInThePoseUses109() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
        s.steps(5, 0);
        s.step(BUSTER);
        s.step(DOWN);
        assertEquals(PlayerStep.CROUCH, s.world.player.step());
        assertEquals(109, s.world.player.anim().anim());
    }

    @Test
    void fullChargeShowsTheLongCrouchShot() {
        Sim s = crouched();
        s.step(DOWN | BUSTER);
        s.steps(25, DOWN);
        s.steps(95, DOWN | BUSTER);
        assertEquals(2, s.world.player.chargeLevel());
        s.step(DOWN);
        assertEquals(139, s.world.player.anim().anim());
    }
}
