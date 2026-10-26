package mmx.x6.game.enemy.scaravich;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.function.Predicate;
import mmx.x6.game.StageFlow;
import mmx.x6.game.World;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.input.Buttons;
import mmx.x6.stage.StageRun;
import mmx.x6.stage.Stages;
import org.junit.jupiter.api.Test;

class ScaravichStageTest {

    static boolean advance(StageRun r, int dir, int max, Predicate<World> done) {
        float lastX = r.world.player.x;
        int still = 0;
        int jump = 0;
        for (int i = 0; i < max; i++) {
            if (done.test(r.world)) {
                return true;
            }
            if (Math.abs(r.world.player.x - lastX) < 0.5f) {
                still++;
            } else {
                still = 0;
            }
            lastX = r.world.player.x;
            if (still > 6 && jump == 0) {
                jump = 24;
            }
            r.step(dir | (jump > 0 ? Buttons.JUMP : 0));
            if (jump > 0) {
                jump--;
                if (jump == 0) {
                    still = 0;
                }
            }
        }
        return done.test(r.world);
    }

    private static StageRun inFirstSection() {
        StageRun r = StageRun.at(Stages.SCARAVICH, 0);
        assertTrue(advance(r, Buttons.RIGHT, 2500, w -> w.player.step() == PlayerStep.TELEPORT));
        assertTrue(r.until(400, 0, w -> w.player.step() != PlayerStep.TELEPORT));
        return r;
    }

    @Test
    void firstPoleSendsXToARolledSectionOfTheFirstGroup() {
        StageRun r = inFirstSection();
        assertEquals(1, r.world.rollPhase);
        int section = Sections.ROUTE[r.world.routeRow][0];
        assertTrue(section >= 0 && section <= 3);
        assertEquals(Sections.entry(section).playerX(), r.world.player.x, 1);
        assertEquals(Sections.entry(section).camX(), r.world.camera.x(), 1);
        assertEquals(0f, r.world.screenFade);
        assertEquals(0, r.world.polesCleared);
    }

    @Test
    void goalKillReturnsXAndTheTotemFightClearsThePole() {
        StageRun r = inFirstSection();
        SectionGoal goal = null;
        for (int i = 0; i < 4000 && goal == null; i++) {
            r.world.player.hp = 32;
            r.step(0);
            SectionGoal g = r.find(SectionGoal.class);
            if (g != null) {
                goal = g;
            }
            if (i == 10) {
                int section = Sections.ROUTE[r.world.routeRow][0];
                int[][] goals = {{1840, 1632}, {1904, 2512}, {1648, 64}, {2288, 848}};
                r.world.player.placeStanding(goals[section][0] - 60, goals[section][1]);
            }
        }
        assertNotNull(goal, "the goal of the section spawns when X is near it");
        goal.onKilled(r.world, 0);
        assertTrue(r.until(300, 0, w -> w.player.step() == PlayerStep.TELEPORT));
        assertTrue(r.until(400, 0, w -> w.player.step() != PlayerStep.TELEPORT));
        assertEquals(800, r.world.player.x, 1);
        TotemPole base = r.find(TotemPole.class);
        assertNotNull(base);
        assertTrue(r.until(10, 0, w -> base.heads().size() == 5));
        assertTrue(base.solidRect() != null);
        for (TotemPole.Head h : base.heads()) {
            h.onKilled(r.world, 0);
        }
        assertTrue(r.until(200, 0, w -> w.polesCleared == 1));
        assertTrue(base.dead);
    }

    @Test
    void routeTableNeverRepeatsASection() {
        for (int[] row : Sections.ROUTE) {
            assertTrue(row[0] != row[1] && row[0] < 4 && row[1] < 4);
            assertTrue(row[2] != row[3] && row[2] >= 4 && row[3] >= 4);
        }
    }

    @Test
    void bossEntryFillsFortyAndDeathClearsTheStage() {
        StageRun r = StageRun.at(Stages.SCARAVICH, 2);
        r.world.polesCleared = 4;
        assertTrue(advance(r, Buttons.RIGHT, 3000, w -> w.bossBarShown && w.bossHp == 40));
        GroundScaravich boss = r.find(GroundScaravich.class);
        assertNotNull(boss);
        assertTrue(r.until(60, 0, w -> boss.step() == GroundScaravich.Step.WALK_OUT));
        assertNotNull(boss.ball());
        float x0 = boss.x;
        r.world.player.hp = 32;
        r.steps(30, 0);
        assertTrue(boss.x != x0, "the boss walks with the ball");
        boss.onKilled(r.world, 0);
        assertTrue(r.until(2500, 0, w -> w.flow.state() == StageFlow.State.CLEAR));
    }
}
