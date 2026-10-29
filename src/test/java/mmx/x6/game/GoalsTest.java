package mmx.x6.game;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import mmx.x6.level.CollisionGrid;
import mmx.x6.level.GoalDef;
import mmx.x6.level.GoalMode;
import mmx.x6.level.Level;
import mmx.x6.level.RecordDef;
import mmx.x6.level.TileLayer;
import org.junit.jupiter.api.Test;

class GoalsTest {

    private static RecordDef rec(int tmxId, int goal) {
        return new RecordDef(tmxId, "MetaWheelF", "main", 0, 0, 0, 0, 100 + tmxId * 10, 100, tmxId, goal, 0);
    }

    private static GoalDef goal(int id, GoalMode mode, boolean ends, int parent) {
        return new GoalDef(id, 0, 0, 100, 100, mode, ends, parent);
    }

    private static World world(List<GoalDef> goals, List<RecordDef> records) {
        CollisionGrid g = new CollisionGrid(32, 32, new int[32 * 32]);
        TileLayer empty = new TileLayer("empty", 32, 32, new int[32 * 32], 0, 0, 0);
        Level level = new Level(32, 32, empty, empty, empty, empty, g, records, List.of(), List.of(),
                java.util.Map.of(), List.of(), goals);
        Sim.RecordingMusic music = new Sim.RecordingMusic();
        return new World(level, Sim::set, new Sim.RecordingSfx(), music);
    }

    @Test
    void allNeedsEveryMember() {
        RecordDef a = rec(10, 1);
        RecordDef b = rec(11, 1);
        World w = world(List.of(goal(1, GoalMode.ALL, false, 0)), List.of(a, b));
        w.goals.reportKill(w, a);
        assertFalse(w.goals.done(1));
        w.goals.reportKill(w, b);
        assertTrue(w.goals.done(1));
    }

    @Test
    void anyNeedsOneMember() {
        RecordDef a = rec(10, 1);
        RecordDef b = rec(11, 1);
        World w = world(List.of(goal(1, GoalMode.ANY, false, 0)), List.of(a, b));
        assertFalse(w.goals.done(1));
        w.goals.reportKill(w, b);
        assertTrue(w.goals.done(1));
    }

    @Test
    void chainEndsStageThroughParent() {
        RecordDef r = rec(10, 1);
        World w = world(List.of(goal(1, GoalMode.ALL, false, 2), goal(2, GoalMode.ALL, true, 0)), List.of(r));
        w.goals.reportKill(w, r);
        assertTrue(w.goals.done(1));
        assertTrue(w.goals.done(2));
        assertEquals(StageFlow.State.CLEAR, w.flow.state());
    }

    @Test
    void despawnDoesNotCount() {
        RecordDef r = new RecordDef(10, "MetaWheelF", "main", 0, 0, 0, 0, 500, 100, 10, 1, 0);
        World w = world(List.of(goal(1, GoalMode.ALL, false, 0)), List.of(r));
        mmx.x6.game.enemy.MetaWheelF e = new mmx.x6.game.enemy.MetaWheelF(w, r);
        w.entities.add(e);
        e.update(w);
        assertTrue(e.dead);
        assertFalse(w.goals.done(1));
        mmx.x6.game.enemy.MetaWheelF again = new mmx.x6.game.enemy.MetaWheelF(w, r);
        w.entities.add(again);
        again.onKilled(w, 19);
        assertTrue(w.goals.done(1));
    }

    @Test
    void bossEndWithoutGoalsClearsStage() {
        World w = world(List.of(), List.of());
        w.goals.reportBossEnd(w, rec(10, 0));
        assertEquals(StageFlow.State.CLEAR, w.flow.state());
    }

    @Test
    void bossEndThatIsNotAMemberDoesNotClear() {
        World w = world(List.of(goal(1, GoalMode.ALL, true, 0)), List.of(rec(10, 1)));
        w.goals.reportBossEnd(w, rec(11, 0));
        assertEquals(StageFlow.State.PLAYING, w.flow.state());
        assertFalse(w.goals.done(1));
    }

    @Test
    void resetClearsState() {
        RecordDef r = rec(10, 1);
        World w = world(List.of(goal(1, GoalMode.ALL, false, 0)), List.of(r));
        w.goals.reportKill(w, r);
        assertTrue(w.goals.done(1));
        w.goals.reset();
        assertFalse(w.goals.done(1));
    }

    @Test
    void parentCycleStops() {
        RecordDef r = rec(10, 1);
        World w = world(List.of(goal(1, GoalMode.ANY, false, 2), goal(2, GoalMode.ANY, false, 1)), List.of(r));
        w.goals.reportKill(w, r);
        assertTrue(w.goals.done(1));
    }

    @Test
    void bossGateWithOpenedByWaitsForItsGoal() {
        String sky = "................................................................";
        String floor = "################################################################";
        String[] rows = new String[10];
        java.util.Arrays.fill(rows, sky);
        rows[8] = floor;
        rows[9] = floor;
        CollisionGrid g = mmx.x6.physics.Grids.grid(rows);
        TileLayer empty = new TileLayer("empty", 64, 10, new int[64 * 10], 0, 0, 0);
        RecordDef member = new RecordDef(10, "MetaWheelF", "main", 0, 0, 0, 0, 900, 100, 10, 1, 0);
        RecordDef gate = new RecordDef(18, "BossGate", "stage", 4, 8, 2, 0, 400, 96, 11, 0, 1);
        Level level = new Level(64, 10, empty, empty, empty, empty, g, List.of(member), List.of(), List.of(),
                java.util.Map.of(), List.of(), List.of(goal(1, GoalMode.ALL, false, 0)));
        World w = new World(level, Sim::set, new Sim.RecordingSfx(), new Sim.RecordingMusic());
        w.spawner = new Spawner(List.of(gate), 1, new mmx.x6.game.enemy.IntroEntityFactory());
        w.camera.reset(new mmx.x6.level.Checkpoint(0, 0, 100, 0, 100, 700, 0, 0, true, 1));
        w.player.placeStanding(300, 128 - 21);
        for (int i = 0; i < 200; i++) {
            w.buttons.set(mmx.x6.input.Buttons.RIGHT);
            w.update();
        }
        mmx.x6.game.enemy.BossGate bossGate = w.entities.stream()
                .filter(e -> e instanceof mmx.x6.game.enemy.BossGate)
                .map(e -> (mmx.x6.game.enemy.BossGate) e).findFirst().orElseThrow();
        assertEquals(mmx.x6.game.enemy.BossGate.GateStep.WAIT, bossGate.step());
        assertFalse(w.locked());
        w.goals.reportKill(w, member);
        assertTrue(w.goals.done(1));
        int guard = 0;
        while (bossGate.step() == mmx.x6.game.enemy.BossGate.GateStep.WAIT && guard++ < 120) {
            w.buttons.set(mmx.x6.input.Buttons.RIGHT);
            w.update();
        }
        assertTrue(bossGate.step() != mmx.x6.game.enemy.BossGate.GateStep.WAIT, "gate step " + bossGate.step());
    }
}
