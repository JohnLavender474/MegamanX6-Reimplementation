package mmx.x6.stage;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.StageFlow;
import mmx.x6.game.boss.D1000;
import mmx.x6.game.player.BusterShot;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class IntroGoalTest {

    private static D1000 bossFight(StageRun r) {
        RecordDef rec = r.world.level.records().stream().filter(x -> x.name().equals("D1000")).findFirst()
                .orElseThrow();
        r.world.camera.reset(new Checkpoint(6120, 395, 6064, 224, 6064, 6144, 224, 224, true, 1));
        r.world.camera.setFollow(true);
        r.world.player.placeStanding(6120, 395);
        D1000 d = new D1000(r.world, rec);
        r.world.entities.add(d);
        int guard = 0;
        while (d.step() == D1000.BossStep.ENTRY && guard++ < 3000) {
            r.step(0);
        }
        return d;
    }

    private static void finish(StageRun r, D1000 d) {
        d.hp = 1;
        for (int i = 0; i < 4000 && r.world.flow.state() != StageFlow.State.CLEAR; i++) {
            if (d.hp > 0 && i % 4 == 0) {
                r.world.entities.add(new BusterShot(r.world, 19, d.x - 30, d.y, true));
            }
            r.step(0);
        }
    }

    @Test
    void d1000DeathClearsThroughGoal() {
        StageRun r = StageRun.at(Stages.INTRO, 2);
        assertNotNull(r.world.level.goals());
        assertFalse(r.world.goals.isEmpty());
        D1000 d = bossFight(r);
        assertFalse(r.world.goals.done(80));
        finish(r, d);
        assertEquals(StageFlow.State.CLEAR, r.world.flow.state());
        assertTrue(r.world.goals.done(80));
    }

    @Test
    void deathAfterGoalResetsAndClearsOnce() {
        StageRun r = StageRun.at(Stages.INTRO, 2);
        finish(r, bossFight(r));
        assertTrue(r.world.goals.done(80));
        r.world.flow.restart(2);
        assertFalse(r.world.goals.done(80));
        assertEquals(StageFlow.State.PLAYING, r.world.flow.state());
        finish(r, bossFight(r));
        assertEquals(StageFlow.State.CLEAR, r.world.flow.state());
        assertEquals(1, r.world.lockCount);
        assertTrue(r.world.goals.done(80));
    }
}
