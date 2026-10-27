package mmx.x6.game.enemy.heatnix;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.ArrayList;
import java.util.List;
import mmx.x6.game.CustomChanges;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
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
        assertTrue(r.world.layer1OverMain, "the ring is drawn in front of the main tiles");
        float x0 = s.x;
        r.steps(120, 0);
        assertTrue(Math.abs(s.x - x0) > 50, "the ring moves");
        assertTrue(s.x >= 400 && s.x <= 880, "x " + s.x);
        killGems(r, s);
        assertTrue(r.until(400, 0, w -> w.flag26));
        r.step(0);
        assertFalse(r.world.layer1OverMain, "the normal draw order comes back");
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

    private static List<NightmareSnake.HeldBall> balls(StageRun r) {
        return r.world.entities.stream().filter(e -> e instanceof NightmareSnake.HeldBall && !e.dead)
                .map(e -> (NightmareSnake.HeldBall) e).toList();
    }

    private static int waitForVolley(StageRun r, NightmareSnake s, int limit) {
        int landings = 0;
        float lastY = s.y;
        for (int i = 0; i < limit; i++) {
            r.world.player.hp = 32;
            long before = shots(r);
            r.step(0);
            if (s.y >= 584 && lastY < 584) {
                landings++;
            }
            lastY = s.y;
            if (shots(r) > before) {
                return landings;
            }
        }
        return -1;
    }

    private static void volleyWithTheGemInView(StageRun r, NightmareSnake s, Entity g) {
        for (int volley = 0; volley < 6; volley++) {
            assertTrue(waitForVolley(r, s, 800) >= 0);
            if (g.x - 40 >= r.world.camera.x() && g.x + 40 <= r.world.camera.x() + 320) {
                r.steps(CustomChanges.SNAKE_SHOT_PAUSE - 15, 0);
                return;
            }
        }
        throw new AssertionError("no volley with the gem in view");
    }

    @Test
    void firstSnakeHopsBackAndForth() {
        StageRun r = StageRun.at(Stages.HEATNIX, 0);
        NightmareSnake s = firstSnakeMoving(r);
        List<Integer> landings = new ArrayList<>();
        float lastY = s.y;
        float top = s.y;
        for (int i = 0; i < 3000; i++) {
            r.world.player.hp = 32;
            r.step(0);
            assertTrue(s.y <= 584.001f, "y " + s.y);
            assertTrue(s.x >= 415.999f && s.x <= 864.001f, "x " + s.x);
            top = Math.min(top, s.y);
            if (s.y >= 584 && lastY < 584) {
                landings.add(java.util.Arrays.binarySearch(CustomChanges.SNAKE_BLOCKS, Math.round(s.x)));
            }
            lastY = s.y;
        }
        assertTrue(landings.size() >= 8, "landings " + landings);
        for (int i = 1; i < landings.size(); i++) {
            int a = landings.get(i - 1);
            int b = landings.get(i);
            assertTrue(a >= 0 && b >= 0 && Math.abs(b - a) == 1, "landings " + landings);
            if (i > 1) {
                int before = landings.get(i - 2);
                assertEquals(a == 0 || a == 3, b == before, "turns only at the ends: " + landings);
            }
        }
        assertEquals(584 - CustomChanges.SNAKE_HOP_HEIGHT, top, 1.0);
        assertTrue(top - 71 >= r.world.camera.y(), "the top gems stay on screen");
    }

    @Test
    void firstSnakeStopsAtThePeakEveryTwoHopsToFire() {
        StageRun r = StageRun.at(Stages.HEATNIX, 0);
        NightmareSnake s = firstSnakeMoving(r);
        for (int volley = 0; volley < 4; volley++) {
            int landings = waitForVolley(r, s, 800);
            if (volley > 0) {
                assertEquals(CustomChanges.SNAKE_SHOT_EVERY, landings, "hops before volley " + volley);
            }
            float x0 = s.x;
            float y0 = s.y;
            assertEquals(584 - CustomChanges.SNAKE_HOP_HEIGHT, y0, 0.001, "at the peak");
            for (int i = 1; i < CustomChanges.SNAKE_SHOT_PAUSE; i++) {
                r.world.player.hp = 32;
                r.step(0);
                assertEquals(x0, s.x, 0.001, "update " + i + " of volley " + volley);
                assertEquals(y0, s.y, 0.001, "update " + i + " of volley " + volley);
            }
            assertTrue(r.until(2, 0, w -> s.x != x0 || s.y != y0), "it moves again");
        }
    }

    @Test
    void firstSnakeGemsTakeMoreBusterAndSabreDamage() {
        StageRun r = StageRun.at(Stages.HEATNIX, 0);
        NightmareSnake s = firstSnakeMoving(r);
        NightmareSnake.Gem g = s.gems.get(2);
        volleyWithTheGemInView(r, s, g);
        r.world.add(new BusterShot(r.world, 0, g.x - 24, g.y + 2, true));
        r.steps(6, 0);
        assertEquals(32 - 5, g.hp, "a lemon does 1 x 4.5");
        int charge = Math.round(DamageTables.value(DamageTables.Table.SNAKE, 19) * g.damageTakenScale(19));
        assertEquals(18, charge);
        for (int sabre : new int[] {50, 51}) {
            float scale = CustomChanges.SABRE_DAMAGE_SCALE * g.damageTakenScale(sabre);
            assertEquals(charge, Math.round(DamageTables.value(DamageTables.Table.SNAKE, sabre) * scale), "sabre " + sabre);
        }
    }

    @Test
    void aShotOnAGemAndTheRingHitsTheGem() {
        StageRun r = StageRun.at(Stages.HEATNIX, 0);
        NightmareSnake s = firstSnakeMoving(r);
        NightmareSnake.Gem g = s.gems.get(2);
        volleyWithTheGemInView(r, s, g);
        BusterShot b = new BusterShot(r.world, 0, g.x - 4, s.y + 34, true);
        assertTrue(b.attackBox().overlaps(s.hurtBox()), "the shot touches the ring");
        r.world.add(b);
        r.steps(2, 0);
        assertEquals(32 - 5, g.hp);
    }

    @Test
    void firstSnakeBallsWaitThenFlyOneByOne() {
        StageRun r = StageRun.at(Stages.HEATNIX, 0);
        firstSnakeMoving(r);
        assertTrue(r.until(800, 0, w -> {
            w.player.hp = 32;
            return balls(r).size() == 4;
        }));
        List<NightmareSnake.HeldBall> balls = balls(r);
        float[][] start = new float[4][];
        for (int i = 0; i < 4; i++) {
            start[i] = new float[] {balls.get(i).x, balls.get(i).y};
        }
        int hold = CustomChanges.SNAKE_BALL_HOLD;
        int gap = CustomChanges.SNAKE_BALL_GAP;
        for (int t = 1; t <= hold + 3 * gap + 2; t++) {
            r.world.player.hp = 32;
            r.step(0);
            for (int i = 0; i < 4; i++) {
                NightmareSnake.HeldBall b = balls.get(i);
                boolean moved = b.dead || b.x != start[i][0] || b.y != start[i][1];
                boolean released = t > hold + i * gap;
                assertEquals(released, moved, "ball " + i + " at update " + t);
            }
        }
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
