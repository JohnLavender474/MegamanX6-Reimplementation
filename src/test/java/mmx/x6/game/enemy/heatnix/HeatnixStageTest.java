package mmx.x6.game.enemy.heatnix;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import mmx.x6.game.CustomChanges;
import mmx.x6.game.StageFlow;
import mmx.x6.game.enemy.BossGate;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.player.BusterShot;
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
        assertTrue(Math.abs(s.x - x0) > 50, "the ring moves");
        assertTrue(s.x >= 400 && s.x <= 880, "x " + s.x);
        killGems(r, s);
        assertTrue(r.until(400, 0, w -> w.flag26));
        BossGate gate = r.world.entities.stream().filter(e -> e instanceof BossGate g && g.record != null
                && g.record.id() == 7).map(e -> (BossGate) e).findFirst().orElse(null);
        assertNotNull(gate);
        assertTrue(BossGate.canOpen(7, r.world));
    }

    private static NightmareSnake firstSnakeMoving(StageRun r) {
        assertTrue(r.runRight(2000, w -> {
            NightmareSnake s = snake(r, 5);
            return s != null && s.step() == NightmareSnake.Step.MOVE;
        }));
        return snake(r, 5);
    }

    private static long shots(StageRun r) {
        return r.world.entities.stream().filter(e -> e instanceof Projectile && !e.dead).count();
    }

    @Test
    void nextBlockIsNeverTheCurrentOne() {
        Random rng = new Random(1);
        for (int i = 0; i < 200; i++) {
            int cur = i % 4;
            int next = NightmareSnake.nextBlock(cur, rng);
            assertTrue(next >= 0 && next < 4 && next != cur, "from " + cur + " to " + next);
        }
        boolean[] seen = new boolean[4];
        for (int i = 0; i < 200; i++) {
            seen[NightmareSnake.nextBlock(-1, rng)] = true;
        }
        assertTrue(seen[0] && seen[1] && seen[2] && seen[3]);
    }

    @Test
    void firstSnakeHopsBetweenTheRaisedParts() {
        StageRun r = StageRun.at(Stages.HEATNIX, 0);
        NightmareSnake s = firstSnakeMoving(r);
        List<Integer> landings = new ArrayList<>();
        float lastY = s.y;
        float top = s.y;
        for (int i = 0; i < 1500; i++) {
            r.world.player.hp = 32;
            r.step(0);
            assertTrue(s.y <= 584.001f, "y " + s.y);
            assertTrue(s.x >= 415.999f && s.x <= 864.001f, "x " + s.x);
            top = Math.min(top, s.y);
            if (s.y >= 584 && lastY < 584) {
                landings.add(Math.round(s.x));
            }
            lastY = s.y;
        }
        assertTrue(landings.size() >= 5, "landings " + landings);
        for (int i = 0; i < landings.size(); i++) {
            int x = landings.get(i);
            assertTrue(x == 416 || x == 560 || x == 720 || x == 864, "landings " + landings);
            if (i > 0) {
                assertTrue(x != landings.get(i - 1), "landings " + landings);
            }
        }
        assertEquals(584 - CustomChanges.SNAKE_HOP_HEIGHT, top, 1.0);
        assertTrue(top - 71 >= r.world.camera.y(), "the top gem stays on screen");
    }

    @Test
    void firstSnakeStopsWhereItIsWhenTheGemsFire() {
        StageRun r = StageRun.at(Stages.HEATNIX, 0);
        NightmareSnake s = firstSnakeMoving(r);
        for (int volley = 0; volley < 4; volley++) {
            long before = shots(r);
            int guard = 0;
            while (shots(r) <= before && guard++ < 200) {
                r.world.player.hp = 32;
                before = shots(r);
                r.step(0);
            }
            float x0 = s.x;
            float y0 = s.y;
            for (int i = 1; i < CustomChanges.SNAKE_SHOT_PAUSE; i++) {
                r.world.player.hp = 32;
                r.step(0);
                assertEquals(x0, s.x, 0.001, "update " + i + " of volley " + volley);
                assertEquals(y0, s.y, 0.001, "update " + i + " of volley " + volley);
            }
            assertTrue(r.until(CustomChanges.SNAKE_LAND_WAIT + 2, 0, w -> s.x != x0 || s.y != y0), "it moves again");
        }
    }

    @Test
    void firstSnakeGemsTakeTripleDamage() {
        StageRun r = StageRun.at(Stages.HEATNIX, 0);
        NightmareSnake s = firstSnakeMoving(r);
        long before = shots(r);
        assertTrue(r.until(200, 0, w -> {
            w.player.hp = 32;
            return shots(r) > before;
        }));
        r.steps(20, 0);
        NightmareSnake.Gem g = s.gems.get(0);
        r.world.add(new BusterShot(r.world, 0, g.x - 24, g.y - 12, true));
        r.steps(6, 0);
        assertEquals(32 - 3, g.hp);
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
