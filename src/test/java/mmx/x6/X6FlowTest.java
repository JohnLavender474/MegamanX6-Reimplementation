package mmx.x6;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.frontend.Phase;
import mmx.x6.stage.Stages;
import org.junit.jupiter.api.Test;

class X6FlowTest {

    @Test
    void bootStartsAtTheReadyScreen() {
        GameFlow f = new GameFlow(null, null);
        assertEquals(GameFlow.Screen.FRONT_END, f.first());
        assertEquals(Phase.READY, f.frontStart());
    }

    @Test
    void frontPropertyCanStillStartAtTheLogo() {
        assertEquals(Phase.LOGO, new GameFlow(null, "logo").frontStart());
    }

    @Test
    void frontPropertyPicksTheStartPhase() {
        assertEquals(Phase.MENU, new GameFlow(null, "menu").frontStart());
        assertEquals(GameFlow.Screen.SELECT, new GameFlow(null, "select").first());
    }

    @Test
    void stageKeyStillSkips() {
        assertEquals(GameFlow.Screen.STAGE, new GameFlow("yammark", null).first());
    }

    @Test
    void introClearOpensSelect() {
        assertEquals(GameFlow.Screen.SELECT, new GameFlow(null, null).afterStageClear(Stages.INTRO));
    }

    @Test
    void introEscapeReturnsToTheTitle() {
        GameFlow f = new GameFlow(null, null);
        assertEquals(GameFlow.Screen.FRONT_END, f.afterStageEscape(Stages.INTRO));
        assertEquals(Phase.TITLE, f.frontStart());
    }

    @Test
    void bossStageEscapeReturnsToSelect() {
        assertEquals(GameFlow.Screen.SELECT, new GameFlow(null, null).afterStageEscape(Stages.YAMMARK));
    }

    @Test
    void selectListHasNoIntro() {
        assertFalse(Stages.BOSSES.contains(Stages.INTRO));
        assertEquals(Stages.ALL.size() - 1, Stages.BOSSES.size());
    }

    @Test
    void missingDataFallsBackToSelect() {
        assertTrue(new GameFlow(null, null).fallback(false));
        assertFalse(new GameFlow(null, null).fallback(true));
    }
}
