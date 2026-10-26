package mmx.x6.game.enemy.sheldon;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.Damage;
import mmx.x6.game.StageFlow;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.input.Buttons;
import mmx.x6.level.RecordDef;
import mmx.x6.stage.StageRun;
import mmx.x6.stage.Stages;
import org.junit.jupiter.api.Test;

class SheldonStageTest {

    private static StageRun placed(int checkpoint, float x, float y) {
        StageRun r = StageRun.at(Stages.SHELDON, checkpoint);
        assertTrue(r.until(300, 0, w -> w.player.step() == PlayerStep.STAND));
        r.world.player.placeStanding(x, y);
        r.steps(5, 0);
        return r;
    }

    private static LaserPuzzle part(StageRun r, int x, int y) {
        return r.world.entities.stream().filter(e -> e instanceof LaserPuzzle p && (int) p.x == x && (int) p.y == y)
                .map(e -> (LaserPuzzle) e).findFirst().orElse(null);
    }

    @Test
    void hiddenFloorShowsWhileXStandsOnItAndHolds() {
        StageRun r = placed(0, 1560, 427);
        HiddenGlassFloor[] floor = new HiddenGlassFloor[1];
        assertTrue(r.until(200, Buttons.RIGHT, w -> {
            floor[0] = w.entities.stream().filter(e -> e instanceof HiddenGlassFloor f && (int) f.x == 1648)
                    .map(e -> (HiddenGlassFloor) e).findFirst().orElse(null);
            return floor[0] != null && floor[0].shown();
        }));
        assertTrue(r.world.player.y < 440, "X stands on the glass, y " + r.world.player.y);
    }

    @Test
    void beamerTurnsToXAndTheReflectedLaserOpensDoorZero() {
        StageRun r = placed(0, 3080, 427);
        LaserPuzzle door = null;
        for (int i = 0; i < 900 && (door == null || !door.open()); i++) {
            r.world.player.hp = 32;
            r.step(0);
            door = part(r, 3152, 360);
        }
        assertNotNull(door);
        LaserPuzzle beamer = part(r, 3080, 304);
        assertNotNull(beamer);
        assertEquals(6, beamer.direction(), "the beamer turned from down-left to down");
        assertTrue(door.open(), "door 0 opens");
        assertTrue(r.until(60, 0, w -> w.camera.targetMaxX() == 3488));
        assertEquals(0, r.world.grid.at(3140, 360), "the gate cells are open");
    }

    @Test
    void reflectorTurnsOncePerHitAndThenWaitsForTheSabre() {
        StageRun r = StageRun.at(Stages.SHELDON, 0);
        LaserPuzzle ref = new LaserPuzzle(r.world, new RecordDef(-1, "LaserReflectorPuzzle", "main", 0, 13, 0x40, 0, 300, 300));
        r.world.add(ref);
        r.step(0);
        assertEquals(9, ref.direction());
        assertTrue(Damage.hitObject(ref, 50) != Damage.NO_HIT);
        ref.onHurt(r.world, 50);
        assertEquals(5, ref.direction());
        assertEquals(Damage.NO_HIT, Damage.hitObject(ref, 50), "the sabre does not count for 33 updates");
        assertTrue(Damage.hitObject(ref, 0) != Damage.NO_HIT, "a lemon still counts");
        r.steps(34, 0);
        assertTrue(Damage.hitObject(ref, 50) != Damage.NO_HIT);
    }

    @Test
    void backgroundSwitchesAtCheckpointOne() {
        StageRun r = StageRun.at(Stages.SHELDON, 1);
        assertTrue(r.until(30, 0, w -> w.camera.layer2Mode() == 5));
        SheldonBgSwitch s = r.find(SheldonBgSwitch.class);
        assertTrue(s.switched());
        StageRun first = StageRun.at(Stages.SHELDON, 0);
        first.steps(30, 0);
        assertFalse(first.find(SheldonBgSwitch.class).switched());
    }

    @Test
    void bossIntroFillsThirtyTwoAndDeathClearsTheStage() {
        StageRun r = StageRun.at(Stages.SHELDON, 3);
        assertTrue(r.until(3000, Buttons.RIGHT, w -> w.bossBarShown && w.bossHp == 32));
        ShieldSheldon boss = r.find(ShieldSheldon.class);
        assertNotNull(boss);
        assertTrue(r.until(30, 0, w -> boss.step() != ShieldSheldon.Step.INTRO));
        assertEquals(ShieldSheldon.Step.SHIELD_THROW, boss.step(), "every high-HP pattern starts with the throw");
        assertTrue(r.until(400, 0, w -> {
            w.player.hp = 32;
            return boss.open();
        }), "he opens after the second shield");
        boss.onKilled(r.world, 0);
        assertTrue(r.until(2500, 0, w -> w.flow.state() == StageFlow.State.CLEAR));
    }
}
