package mmx.x6.game.enemy.turtloid;

import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Arrays;
import mmx.x6.game.Sim;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class RocketPlatformTest {

    @Test
    void platformMovingIntoXPushesHimAlongTheFloor() {
        String[] rows = new String[16];
        Arrays.fill(rows, "................................................................");
        rows[10] = "################################################################";
        rows[11] = rows[10];
        Sim s = Sim.on(grid(rows)).playerAt(100, 160);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        s.steps(5, 0);
        s.world.entities.add(new RocketPlatform(s.world, new RecordDef(1, "RocketPlatform", "stage", 4, 5, 0, 0, 150, 140)));
        float x0 = s.world.player.x;
        for (int i = 0; i < 60; i++) {
            s.step(0);
            assertEquals(160, s.world.player.hurtBox().bottom(), 0.01f, "update " + i);
        }
        assertTrue(s.world.player.x < x0 - 10, "x " + s.world.player.x);
    }
}
