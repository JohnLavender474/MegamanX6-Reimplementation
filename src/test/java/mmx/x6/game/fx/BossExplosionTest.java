package mmx.x6.game.fx;

import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.Sim;
import org.junit.jupiter.api.Test;

class BossExplosionTest {

    private static Sim sim() {
        Sim s = Sim.on(grid("....", "....", "####")).playerAt(8, 32);
        s.world.bossBarShown = true;
        s.music.play(21);
        return s;
    }

    private static long explosions(Sim s) {
        return s.world.entities.stream().filter(e -> e instanceof Explosion).count();
    }

    @Test
    void firstNineUpdatesMakeEightExplosionsAndStopTheMusic() {
        Sim s = sim();
        BossExplosion b = new BossExplosion(30, 10, BossExplosion.LONG_HOLD | 1);
        s.world.entities.add(b);
        s.steps(8, 0);
        assertTrue(s.music.playing());
        s.step(0);
        assertEquals(8, explosions(s));
        assertFalse(s.music.playing());
    }

    @Test
    void flagsOneKeepsTheMusic() {
        Sim s = sim();
        s.world.entities.add(new BossExplosion(30, 10, 1));
        s.steps(9, 0);
        assertTrue(s.music.playing());
    }

    @Test
    void whiteFadeStartsAfter120UpdatesAndSignalsAtFullWhite() {
        Sim s = sim();
        BossExplosion b = new BossExplosion(30, 10, BossExplosion.LONG_HOLD | 1);
        s.world.entities.add(b);
        s.steps(119, 0);
        assertEquals(0, s.world.whiteLevel, 0.0001);
        s.step(0);
        assertEquals(25 / 4095f, s.world.whiteLevel, 0.0001);
        s.steps(162, 0);
        assertFalse(b.white());
        assertTrue(s.world.bossBarShown);
        s.step(0);
        assertTrue(b.white());
        assertEquals(1, s.world.whiteLevel, 0.0001);
        assertFalse(s.world.bossBarShown);
        assertEquals(1, s.sfx.count(0, 5));
    }

    @Test
    void longHoldThenFadeBackThenGone() {
        Sim s = sim();
        BossExplosion b = new BossExplosion(30, 10, BossExplosion.LONG_HOLD | 1);
        s.world.entities.add(b);
        s.steps(283 + 201, 0);
        assertEquals(1, s.world.whiteLevel, 0.0001);
        s.step(0);
        assertTrue(s.world.whiteLevel < 1);
        int guard = 0;
        while (!b.dead && guard++ < 400) {
            s.step(0);
        }
        assertTrue(b.dead);
        assertEquals(0, s.world.whiteLevel, 0.0001);
    }

    @Test
    void shortHoldIs41Updates() {
        Sim s = sim();
        s.world.entities.add(new BossExplosion(30, 10, 1));
        s.steps(283 + 41, 0);
        assertEquals(1, s.world.whiteLevel, 0.0001);
        s.step(0);
        assertTrue(s.world.whiteLevel < 1);
    }
}
