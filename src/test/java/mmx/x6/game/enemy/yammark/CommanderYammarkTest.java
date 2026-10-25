package mmx.x6.game.enemy.yammark;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.StageFlow;
import mmx.x6.game.enemy.NightmareVirus;
import mmx.x6.input.Buttons;
import mmx.x6.stage.StageRun;
import mmx.x6.stage.Stages;
import org.junit.jupiter.api.Test;

class CommanderYammarkTest {

    private static StageRun toFight() {
        StageRun r = StageRun.at(Stages.YAMMARK, 2);
        assertTrue(r.until(3000, Buttons.RIGHT, w -> w.bossBarShown && w.bossHp == 32));
        return r;
    }

    @Test
    void introFillsTheBarAndMakesThreeDrones() {
        StageRun r = toFight();
        CommanderYammark boss = r.find(CommanderYammark.class);
        assertNotNull(boss);
        assertEquals(22, r.music.track());
        assertEquals(3, r.count(YammarkDrone.class));
        assertEquals(r.world.stage.checkpoints().get(2).camX(), 4800);
        assertTrue(boss.x > 5120 && boss.x < 5440);
    }

    @Test
    void fightRunsWithoutLeavingTheRoom() {
        StageRun r = toFight();
        CommanderYammark boss = r.find(CommanderYammark.class);
        for (int i = 0; i < 3000; i++) {
            r.step(0);
            assertTrue(boss.x >= 5120 && boss.x <= 5440, "x " + boss.x);
            assertTrue(boss.y >= 1904 && boss.y <= 2096, "y " + boss.y);
            r.world.player.hp = 32;
        }
    }

    @Test
    void deathLeavesABossSoulThenClear() {
        StageRun r = toFight();
        CommanderYammark boss = r.find(CommanderYammark.class);
        boss.hp = 1;
        boss.onKilled(r.world, 0);
        assertTrue(r.until(1500, 0, w -> w.entities.stream()
                .anyMatch(e -> e instanceof NightmareVirus v && v.step() == NightmareVirus.Step.BOSS_SOUL)));
        assertEquals(0, r.count(YammarkDrone.class));
        assertTrue(r.until(1500, 0, w -> w.flow.state() == StageFlow.State.CLEAR));
        assertTrue(r.world.progress.souls >= 100);
    }
}
