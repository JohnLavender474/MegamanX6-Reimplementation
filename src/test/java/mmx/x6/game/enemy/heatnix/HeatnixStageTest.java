package mmx.x6.game.enemy.heatnix;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.StageFlow;
import mmx.x6.game.enemy.BossGate;
import mmx.x6.input.Buttons;
import mmx.x6.stage.StageRun;
import mmx.x6.stage.Stages;
import org.junit.jupiter.api.Test;

class HeatnixStageTest {

    private static NightmareSnake snake(StageRun r, int kind) {
        return r.world.entities.stream().filter(e -> e instanceof NightmareSnake s && s.kind == kind && !e.dead)
                .map(e -> (NightmareSnake) e).findFirst().orElse(null);
    }

    private static void killGems(StageRun r, NightmareSnake s) {
        for (NightmareSnake.Gem g : s.gems) {
            if (!g.dead) {
                g.onKilled(r.world, 0);
            }
        }
    }

    @Test
    void firstSnakeFightsAndOpensGateSeven() {
        StageRun r = StageRun.at(Stages.HEATNIX, 0);
        assertTrue(r.runRight(2000, w -> {
            NightmareSnake s = snake(r, 5);
            return s != null && s.step() == NightmareSnake.Step.MOVE;
        }));
        NightmareSnake s = snake(r, 5);
        assertEquals(127, r.world.bossHp);
        assertFalse(r.world.flag26);
        float x0 = s.x;
        r.steps(120, 0);
        assertTrue(Math.abs(s.x - x0) > 50, "the ring rolls");
        assertTrue(s.x >= 400 && s.x <= 880, "x " + s.x);
        killGems(r, s);
        assertTrue(r.until(400, 0, w -> w.flag26));
        BossGate gate = r.world.entities.stream().filter(e -> e instanceof BossGate g && g.record != null
                && g.record.id() == 7).map(e -> (BossGate) e).findFirst().orElse(null);
        assertNotNull(gate);
        assertTrue(BossGate.canOpen(7, r.world));
    }

    @Test
    void lavaSnakeStartsInTheTopCorridor() {
        StageRun r = StageRun.at(Stages.HEATNIX, 6);
        assertTrue(r.until(600, Buttons.RIGHT, w -> {
            NightmareSnake s = snake(r, 10);
            return s != null && s.step() == NightmareSnake.Step.MOVE;
        }));
        NightmareSnake s = snake(r, 10);
        r.steps(300, 0);
        r.world.player.hp = 32;
        killGems(r, s);
        assertTrue(r.until(400, 0, w -> w.flag26));
    }

    @Test
    void risingRoomScrollsUp() {
        StageRun r = StageRun.at(Stages.HEATNIX, 5);
        r.steps(150, 0);
        r.world.player.placeStanding(2832, 2256 - 21);
        assertTrue(r.until(3000, Buttons.RIGHT, w -> {
            NightmareSnake s = snake(r, 9);
            return s != null && s.step() == NightmareSnake.Step.MOVE;
        }));
        int y0 = r.world.camera.targetMinY();
        r.steps(150, 0);
        assertTrue(r.world.camera.targetMinY() < y0 - 25, "the view goes up");
    }

    @Test
    void standInBossEndsTheStage() {
        StageRun r = StageRun.at(Stages.HEATNIX, 7);
        assertTrue(r.until(3000, Buttons.RIGHT, w -> w.bossBarShown && w.bossHp == 32));
        BlazeHeatnix boss = r.find(BlazeHeatnix.class);
        boss.onKilled(r.world, 0);
        assertTrue(r.until(2500, 0, w -> w.flow.state() == StageFlow.State.CLEAR));
    }
}
