package mmx.x6.game;

import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import mmx.x6.game.enemy.IntroEntityFactory;
import mmx.x6.game.enemy.SabreBlock;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.IntroCheckpoints;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class StageFlowTest {

    private static final String SKY = "........................................";
    private static final String FLOOR = "########################################";

    @Test
    void deathRestartsAtTheCheckpoint() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
        s.world.checkpoint = 1;
        s.world.player.hp = 1;
        s.world.player.hurt(s.world, 8, 150);
        int guard = 0;
        while (!s.world.flow.restarted() && guard++ < 400) {
            s.step(0);
        }
        assertTrue(s.world.flow.restarted(), "restart");
        assertEquals(IntroCheckpoints.ALL.get(1).playerX(), s.world.player.x, 0.001);
        assertEquals(32, s.world.player.hp);
    }

    @Test
    void respawnResetsTheWorld() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
        s.world.entities.add(new TestTarget(10, 10, 4, 4, 3, DamageTables.Table.NORMAL));
        s.world.grid.set(3, 3, 56);
        s.world.lockCount = 2;
        s.world.flow.restart(1);
        assertTrue(s.world.entities.isEmpty());
        assertEquals(0, s.world.grid.raw(3, 3));
        assertEquals(0, s.world.lockCount);
        assertEquals(IntroCheckpoints.ALL.get(1).camX(), s.world.camera.x(), 0.001);
        assertEquals(9, s.music.track());
        assertEquals(2112, s.world.player.x, 0.001);
        assertTrue(s.world.player.step() != PlayerStep.DEAD);
    }

    @Test
    void respawnBringsBrokenBlocksBack() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
        RecordDef block = new RecordDef(40, "SabreBlock", "stage", 4, 2, 0, 0, 200, 136);
        s.world.spawner = new Spawner(List.of(block), 1, new IntroEntityFactory());
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        s.step(0);
        assertEquals(1, s.world.entities.stream().filter(e -> e instanceof SabreBlock).count());
        s.world.spawner.kept(block);
        s.world.entities.clear();
        s.world.flow.restart(0);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        s.step(0);
        assertEquals(1, s.world.entities.stream().filter(e -> e instanceof SabreBlock).count());
    }
}
