package mmx.x6.game;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.Checkpoint;
import mmx.x6.stage.StageRun;
import mmx.x6.stage.Stages;
import org.junit.jupiter.api.Test;

class CameraFallTest {

    @Test
    void dropFromTheYammarkRopeFollowsXDownAlive() {
        StageRun r = StageRun.at(Stages.YAMMARK, 1);
        r.steps(120, 0);
        World w = r.world;
        w.camera.reset(new Checkpoint(2960, 1440, 2800, 1200, 2672, 2880, 1200, 1200, true, 1));
        w.camera.setFollow(true);
        w.player.placeStanding(2960, 1460);
        w.player.toFall();
        w.camera.setTargets(-1, -1, -1, 1904);
        boolean snapped = false;
        for (int i = 0; i < 300; i++) {
            r.step(0);
            snapped |= w.camera.maxY() == 1904 && i < 40;
            assertTrue(w.player.step() != PlayerStep.DEAD, "dead at update " + i);
        }
        assertTrue(snapped);
        assertEquals(2096, w.player.hurtBox().bottom(), 0.5f);
    }
}
