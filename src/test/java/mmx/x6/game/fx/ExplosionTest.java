package mmx.x6.game.fx;

import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.Sim;
import org.junit.jupiter.api.Test;

class ExplosionTest {

    private static Sim sim() {
        return Sim.on(grid("....", "....", "####")).playerAt(8, 32);
    }

    @Test
    void drawsTheExportedAnimationAndEndsAtItsHoldFrame() {
        Sim s = sim();
        Explosion e = new Explosion(s.world, 30, 10);
        assertEquals("explosion", e.spriteSet());
        assertEquals(0, e.anim().anim());
        int life = 0;
        while (!e.dead && life < 200) {
            e.update(s.world);
            life++;
        }
        assertEquals(46, life);
        assertTrue(e.shapes().isEmpty());
    }

    @Test
    void firstFrameIsNotDrawn() {
        Explosion e = new Explosion(sim().world, 30, 10);
        assertFalse(e.visible());
    }

    @Test
    void sparkStaysAShape() {
        Explosion spark = Explosion.spark(30, 10);
        assertEquals(null, spark.spriteSet());
        assertFalse(spark.shapes().isEmpty());
    }
}
