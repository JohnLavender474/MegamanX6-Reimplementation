package mmx.x6.game;

import static mmx.x6.input.Buttons.RIGHT;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import mmx.x6.game.enemy.BossGate;
import mmx.x6.game.enemy.IntroEntityFactory;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class BeamInTest {

    private static final String SKY = "................................................................";
    private static final String FLOOR = "################################################################";

    @Test
    void beamInFallsThroughTerrain() {
        String[] rows = new String[16];
        java.util.Arrays.fill(rows, SKY);
        rows[3] = FLOOR;
        rows[12] = FLOOR;
        Sim s = Sim.on(grid(rows)).playerAt(100, 192);
        Checkpoint cp = new Checkpoint(100, 0, 0, 0, 0, 0, 0, 16, true, 6);
        s.world.camera.reset(cp);
        s.sfx.played.clear();
        s.world.player.respawn(cp);
        assertEquals(PlayerStep.BEAM_IN, s.world.player.step());
        assertFalse(s.world.camera.follow());
        for (int i = 1; i <= 6; i++) {
            s.step(RIGHT);
            assertEquals(8f * i, s.world.player.y, 0.001);
            assertEquals(100, s.world.player.x, 0.001);
        }
        int guard = 0;
        while (s.world.player.body().bottom() < 192 && guard++ < 40) {
            s.step(0);
        }
        assertEquals(192, s.world.player.body().bottom(), 0.01);
        s.step(0);
        assertTrue(s.world.camera.follow());
        assertEquals(PlayerStep.BEAM_IN, s.world.player.step());
        assertEquals(1, s.sfx.count(1, 13));
        int landing = 0;
        while (s.world.player.step() == PlayerStep.BEAM_IN && landing < 200) {
            s.step(RIGHT);
            landing++;
        }
        assertTrue(Math.abs(landing - 87) <= 2, "landing " + landing);
        assertEquals(PlayerStep.STAND, s.world.player.step());
    }

    @Test
    void restartMidGatePass() {
        String[] rows = new String[10];
        java.util.Arrays.fill(rows, SKY);
        rows[8] = FLOOR;
        rows[9] = FLOOR;
        Sim s = Sim.on(grid(rows)).playerAt(300, 128);
        s.world.camera.reset(new Checkpoint(0, 0, 100, 0, 100, 700, 0, 0, true, 1));
        s.world.spawner = new Spawner(List.of(new RecordDef(18, "BossGate", "stage", 4, 8, 2, 0, 400, 96)), 1,
                new IntroEntityFactory());
        s.step(0);
        int guard = 0;
        while (s.world.player.step() != PlayerStep.GATE_WALK && guard++ < 120) {
            s.step(RIGHT);
        }
        s.steps(80, 0);
        assertTrue(s.world.locked());
        s.world.flow.restart(0);
        assertFalse(s.world.locked());
        assertEquals(0, s.world.checkpoint);
        assertEquals(9, s.music.track());
        assertEquals(PlayerStep.BEAM_IN, s.world.player.step());
        assertTrue(s.world.entities.stream().noneMatch(e -> e instanceof BossGate));
    }
}
