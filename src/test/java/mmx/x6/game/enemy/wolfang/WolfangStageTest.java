package mmx.x6.game.enemy.wolfang;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.StageFlow;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.input.Buttons;
import mmx.x6.stage.StageRun;
import mmx.x6.stage.Stages;
import org.junit.jupiter.api.Test;

class WolfangStageTest {

    private static long pitBlocks(StageRun r) {
        return r.world.entities.stream()
                .filter(e -> e instanceof IceStackBlock b && !e.dead && b.kind() == IceStackBlock.PIT_BLOCK).count();
    }

    private static boolean dashRight(StageRun r, int max, java.util.function.Predicate<mmx.x6.game.World> done) {
        for (int i = 0; i < max; i++) {
            if (done.test(r.world)) {
                return true;
            }
            int t = i % 32;
            r.step(Buttons.RIGHT | (t < 2 ? 0 : Buttons.DASH) | (t >= 6 && t < 26 ? Buttons.JUMP : 0));
        }
        return done.test(r.world);
    }

    private static StageRun onTheIceStep() {
        StageRun r = StageRun.at(Stages.WOLFANG, 0);
        assertTrue(r.until(300, 0, w -> w.player.step() == PlayerStep.STAND));
        assertTrue(r.until(300, Buttons.RIGHT, w -> w.player.x >= 276));
        assertTrue(r.until(60, Buttons.UP | Buttons.JUMP, w -> w.player.step() == PlayerStep.ROPE));
        r.steps(60, Buttons.UP);
        r.step(Buttons.RIGHT | Buttons.JUMP);
        assertTrue(r.until(120, Buttons.RIGHT | Buttons.JUMP, w -> w.player.grounded() && w.player.x > 320));
        return r;
    }

    @Test
    void ropeClimbsUpToTheIceStep() {
        StageRun r = onTheIceStep();
        assertTrue(r.world.player.y < 1280, "y " + r.world.player.y);
    }

    @Test
    void walkCannotGoUpTheIceSlopeButADashCan() {
        StageRun walk = onTheIceStep();
        walk.steps(120, Buttons.RIGHT);
        assertTrue(walk.world.player.x < 400, "walk x " + walk.world.player.x);
        StageRun dash = onTheIceStep();
        dash.steps(25, Buttons.RIGHT | Buttons.DASH);
        assertTrue(dash.world.player.x > 400, "dash x " + dash.world.player.x);
    }

    @Test
    void avalancheStartsOnTheFirstSlope() {
        StageRun r = onTheIceStep();
        assertTrue(dashRight(r, 1500, w -> w.entities.stream()
                .anyMatch(e -> e instanceof IceBlock b && b.mode() == IceBlock.Mode.BLOCK_LEFT)));
    }

    @Test
    void icePitLocksTheCameraAndDropsBlocks() {
        StageRun r = StageRun.at(Stages.WOLFANG, 3);
        assertTrue(r.runRight(1500, w -> w.camera.targetMinX() == 4976 && w.camera.targetMaxX() == 4976));
        assertTrue(r.until(400, 0, w -> pitBlocks(r) > 0));
        assertTrue(r.until(400, 0, w -> w.entities.stream()
                .anyMatch(e -> e instanceof IceStackBlock b && b.state() != IceStackBlock.State.FALL
                        && b.kind() == IceStackBlock.PIT_BLOCK)), "a block lands");
    }

    @Test
    void bossIntroDarkensFillsAndFights() {
        StageRun r = StageRun.at(Stages.WOLFANG, 5);
        assertTrue(r.until(3000, Buttons.RIGHT, w -> w.bgDark > 0.5f));
        BlizzardWolfang boss = r.find(BlizzardWolfang.class);
        assertNotNull(boss);
        assertTrue(r.until(3000, 0, w -> boss.step() != BlizzardWolfang.Step.ENTRY));
        assertEquals(48, r.world.bossHp);
        assertEquals(0f, r.world.bgDark);
        assertTrue(r.music.requests.contains(22), "boss music " + r.music.requests);
        float x0 = boss.x;
        assertTrue(r.until(2500, 0, w -> {
            w.player.hp = 32;
            return boss.x != x0;
        }), "the boss moves");
    }

    @Test
    void bossDeathClearsTheStage() {
        StageRun r = StageRun.at(Stages.WOLFANG, 5);
        BlizzardWolfang[] boss = new BlizzardWolfang[1];
        assertTrue(r.until(4000, Buttons.RIGHT, w -> (boss[0] = r.find(BlizzardWolfang.class)) != null
                && boss[0].step() != BlizzardWolfang.Step.ENTRY));
        boss[0].onKilled(r.world, 0);
        assertTrue(r.until(2500, 0, w -> w.flow.state() == StageFlow.State.CLEAR));
    }
}
