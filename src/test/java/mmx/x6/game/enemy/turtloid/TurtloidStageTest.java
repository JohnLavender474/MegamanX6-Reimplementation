package mmx.x6.game.enemy.turtloid;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.StageFlow;
import mmx.x6.input.Buttons;
import mmx.x6.stage.StageRun;
import mmx.x6.stage.Stages;
import org.junit.jupiter.api.Test;

class TurtloidStageTest {

    private static StageRun toFight() {
        StageRun r = StageRun.at(Stages.TURTLOID, 5);
        assertTrue(r.until(3000, Buttons.RIGHT, w -> w.bossBarShown && w.bossHp == 56));
        return r;
    }

    @Test
    void bossStandsOnTheRoomFloor() {
        StageRun r = toFight();
        RainyTurtloid boss = r.find(RainyTurtloid.class);
        assertNotNull(boss);
        assertEquals(816, boss.y, 0.01);
        assertEquals(22, r.music.track());
    }

    @Test
    void fightStaysInTheRoomAndFiresMissiles() {
        StageRun r = toFight();
        RainyTurtloid boss = r.find(RainyTurtloid.class);
        boolean missiles = false;
        for (int i = 0; i < 4000; i++) {
            r.step(0);
            r.world.player.hp = 32;
            assertTrue(boss.x >= 9024 && boss.x <= 9280, "x " + boss.x);
            assertTrue(boss.y >= 720 && boss.y <= 816, "y " + boss.y);
            missiles |= r.world.entities.stream().anyMatch(e -> e.getClass().getSimpleName().equals("Missile"));
        }
        assertTrue(missiles);
    }

    @Test
    void deathEndsTheStage() {
        StageRun r = toFight();
        RainyTurtloid boss = r.find(RainyTurtloid.class);
        boss.onKilled(r.world, 0);
        assertTrue(r.until(2500, 0, w -> w.flow.state() == StageFlow.State.CLEAR));
    }

    @Test
    void firstMachineRainsUntilItsSensorsAndItBreak() {
        StageRun r = StageRun.at(Stages.TURTLOID, 1);
        assertTrue(r.runRight(1500, w -> r.find(WeatherAnalyze.class) != null && w.player.x > 1320));
        AcidRain rain = r.find(AcidRain.class);
        assertNotNull(rain);
        assertTrue(r.until(60, 0, w -> rain.state() == AcidRain.State.RAIN));
        WeatherAnalyze machine = r.find(WeatherAnalyze.class);
        r.steps(10, 0);
        assertEquals(4, machine.sensors);
        assertTrue(machine.hurtBox() == null);
        r.world.entities.stream().filter(e -> e.getClass().getSimpleName().equals("Sensor")).toList()
                .forEach(e -> e.onKilled(r.world, 0));
        r.step(0);
        assertNotNull(machine.hurtBox());
        machine.onKilled(r.world, 0);
        r.steps(40, 0);
        assertFalse(r.world.flag26);
        assertEquals(2272, r.world.camera.targetMaxX());
    }

    @Test
    void noCeilingOverTheRocketPlatforms() {
        StageRun r = StageRun.at(Stages.TURTLOID, 3);
        for (int x = 4128; x < 4992; x += 16) {
            assertEquals(0, r.world.grid.at(x, 208), "x " + x);
        }
        assertEquals(57, r.world.grid.at(4112, 208));
        assertEquals(57, r.world.grid.at(4992, 208));
        r.world.flow.restart(3);
        assertEquals(0, r.world.grid.at(4500, 208), "the change stays after a restart");
    }

    @Test
    void rainIsDrawnInFrontOfTheTiles() {
        StageRun r = StageRun.at(Stages.TURTLOID, 1);
        r.steps(5, 0);
        assertTrue(r.world.layer1Front);
    }
}
