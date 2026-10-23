package mmx.x6.game.enemy;

import static mmx.x6.input.Buttons.JUMP;
import static mmx.x6.input.Buttons.RIGHT;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import mmx.x6.game.Sim;
import mmx.x6.game.Spawner;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class BossGateTest {

    private static final String SKY = "................................................................";
    private static final String FLOOR = "################################################################";

    private static RecordDef gateRec(int id, int x) {
        return new RecordDef(18, "BossGate", "stage", 4, 8, id, 0, x, 96);
    }

    private static Sim room(int id) {
        String[] rows = new String[10];
        java.util.Arrays.fill(rows, SKY);
        rows[8] = FLOOR;
        rows[9] = FLOOR;
        Sim s = Sim.on(grid(rows)).playerAt(300, 128);
        s.world.camera.reset(new Checkpoint(0, 0, 100, 0, 100, 700, 0, 0, true, 1));
        s.world.camera.setFollow(true);
        s.world.spawner = new Spawner(List.of(gateRec(id, 400)), 1, new IntroEntityFactory());
        s.step(0);
        return s;
    }

    private static BossGate gate(Sim s) {
        return s.world.entities.stream().filter(e -> e instanceof BossGate).map(e -> (BossGate) e)
                .findFirst().orElseThrow();
    }

    private static void walkIn(Sim s) {
        int guard = 0;
        while (gate(s).step() == BossGate.GateStep.WAIT && guard++ < 120) {
            s.step(RIGHT);
        }
    }

    @Test
    void gateIsAWallFromTheLeft() {
        Sim s = room(2);
        assertTrue(s.world.solids().stream().anyMatch(r -> r.x() == 384 && r.y() == 64 && r.w() == 32 && r.h() == 64));
        s.steps(40, 0);
        assertEquals(BossGate.GateStep.WAIT, gate(s).step());
    }

    @Test
    void opensOnlyFromTheLeftOnTheFloor() {
        Sim s = room(2);
        s.world.player.placeStanding(372, 107);
        s.step(JUMP | RIGHT);
        for (int i = 0; i < 10; i++) {
            s.step(JUMP | RIGHT);
            assertEquals(BossGate.GateStep.WAIT, gate(s).step());
        }
        Sim t = room(2);
        walkIn(t);
        assertEquals(BossGate.GateStep.OPEN, gate(t).step());
    }

    @Test
    void triggerClearsEnemiesAndLocks() {
        Sim s = room(2);
        Junkroid j = new Junkroid(s.world, new RecordDef(25, "Junkroid", "main", 0, 1, 0, 0, 200, 104));
        s.world.entities.add(j);
        walkIn(s);
        assertTrue(j.dead || !s.world.entities.contains(j));
        assertEquals(PlayerStep.GATE_WALK, s.world.player.step());
        assertTrue(s.world.player.faceRight);
        assertTrue(s.world.locked());
        assertEquals(28, s.music.track());
        assertEquals(1, s.sfx.count(0, 16));
    }

    @Test
    void openTakes75Updates() {
        Sim s = room(2);
        walkIn(s);
        int updates = 0;
        while (gate(s).step() == BossGate.GateStep.OPEN && updates < 200) {
            s.step(0);
            updates++;
        }
        assertTrue(Math.abs(updates - 75) <= 1, "updates " + updates);
        assertEquals(1, s.sfx.count(0, 17));
        assertEquals(400, s.world.camera.targetMinX());
        assertEquals(400, s.world.camera.targetMaxX());
        assertEquals(224, s.world.camera.targetMinY());
        assertEquals(224, s.world.camera.targetMaxY());
        assertNull(gate(s).solidRect());
    }

    @Test
    void passPushesThePlayerAndTheCameraEases() {
        Sim s = room(2);
        walkIn(s);
        while (gate(s).step() == BossGate.GateStep.OPEN) {
            s.step(0);
        }
        float x = s.world.player.x;
        float camX = s.world.camera.x();
        s.step(0);
        assertEquals(0.640625f, s.world.player.x - x, 0.001);
        assertFalse(s.world.player.body().hasBox);
        int guard = 0;
        while (gate(s).step() == BossGate.GateStep.PASS && guard++ < 400) {
            assertTrue(s.world.camera.x() - camX <= 2.001f);
            camX = s.world.camera.x();
            s.step(0);
        }
        assertEquals(BossGate.GateStep.CLOSE, gate(s).step());
        assertEquals(400, s.world.camera.x(), 0.001);
        assertTrue(s.world.player.body().hasBox);
        assertEquals(2, s.sfx.count(0, 17));
    }

    @Test
    void closeTakes76Updates() {
        Sim s = room(2);
        walkIn(s);
        while (gate(s).step() != BossGate.GateStep.CLOSE) {
            s.step(0);
        }
        int updates = 0;
        while (gate(s).step() == BossGate.GateStep.CLOSE && updates < 200) {
            s.step(0);
            updates++;
        }
        assertTrue(Math.abs(updates - 76) <= 1, "updates " + updates);
        assertEquals(BossGate.GateStep.CLOSED, gate(s).step());
        assertNotNull(gate(s).solidRect());
        assertFalse(s.world.locked());
        assertEquals(2, s.sfx.count(0, 16));
    }

    @Test
    void gate19Limits() {
        Sim s = room(1);
        walkIn(s);
        while (gate(s).step() == BossGate.GateStep.OPEN) {
            s.step(0);
        }
        assertEquals(400, s.world.camera.targetMinX());
        assertEquals(480, s.world.camera.targetMaxX());
    }
}
