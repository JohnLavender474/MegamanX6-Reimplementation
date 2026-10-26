package mmx.x6.frontend;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.ArrayList;
import java.util.List;
import mmx.x6.audio.Sfx;
import mmx.x6.input.Buttons;
import org.junit.jupiter.api.Test;

class FrontEndTest {

    private final List<String> sounds = new ArrayList<>();
    private final Sfx sfx = new Sfx() {
        @Override
        public void play(int bank, int id) {
            sounds.add(bank + "/" + id);
        }

        @Override
        public void stop(int bank, int id) {
        }
    };

    private static void run(FrontEnd fe, int n, int held) {
        for (int i = 0; i < n; i++) {
            fe.update(held);
        }
    }

    private FrontEnd menuReady() {
        FrontEnd fe = new FrontEnd(sfx, Phase.MENU);
        run(fe, 32, 0);
        assertEquals(0f, fe.darkness());
        return fe;
    }

    @Test
    void logoPlaysNinetyFramesThenLoads() {
        FrontEnd fe = new FrontEnd(sfx, Phase.LOGO);
        run(fe, 359, 0);
        assertEquals(Phase.LOGO, fe.phase());
        assertEquals(89, fe.movieFrame());
        fe.update(0);
        assertEquals(Phase.LOAD, fe.phase());
    }

    @Test
    void startSkipsTheLogo() {
        FrontEnd fe = new FrontEnd(sfx, Phase.LOGO);
        fe.update(Buttons.START);
        assertEquals(Phase.LOAD, fe.phase());
    }

    @Test
    void loadCannotBeSkippedAndLastsSixtyUpdates() {
        FrontEnd fe = new FrontEnd(sfx, Phase.LOAD);
        run(fe, 59, Buttons.START);
        assertEquals(Phase.LOAD, fe.phase());
        fe.update(Buttons.START);
        assertEquals(Phase.OPENING, fe.phase());
        assertEquals(0, fe.movieFrame());
    }

    @Test
    void faceButtonSkipsTheOpeningToTheTitle() {
        FrontEnd fe = new FrontEnd(sfx, Phase.OPENING);
        run(fe, 100, 0);
        fe.update(Buttons.BUSTER);
        assertEquals(List.of("0/34"), sounds);
        assertEquals(Phase.TITLE, fe.phase(), "the movie stops at the press");
        assertEquals(1f, fe.darkness());
        fe.update(0);
        assertTrue(fe.darkness() < 1f);
    }

    @Test
    void openingEndsOnItsOwnAtFrame1518() {
        FrontEnd fe = new FrontEnd(sfx, Phase.OPENING);
        run(fe, 1518 * 4 - 1, 0);
        assertEquals(Phase.OPENING, fe.phase());
        fe.update(0);
        assertEquals(Phase.TITLE, fe.phase());
    }

    @Test
    void titleTimeoutReturnsToOpening() {
        FrontEnd fe = new FrontEnd(sfx, Phase.TITLE);
        run(fe, 631, 0);
        assertEquals(Phase.TITLE, fe.phase());
        fe.update(0);
        assertEquals(Phase.OPENING, fe.phase());
        assertEquals(0, fe.movieFrame());
    }

    @Test
    void startOnTheTitleOpensTheMenuAtRowZero() {
        FrontEnd fe = new FrontEnd(sfx, Phase.TITLE);
        run(fe, 40, 0);
        fe.update(Buttons.START);
        assertEquals(List.of("0/34"), sounds);
        run(fe, 32, 0);
        assertEquals(Phase.MENU, fe.phase());
        assertEquals(0, fe.row());
    }

    @Test
    void menuRowsWrap() {
        FrontEnd fe = menuReady();
        fe.update(Buttons.UP);
        assertEquals(2, fe.row());
        assertEquals(List.of("0/12"), sounds);
        fe.update(0);
        fe.update(Buttons.DOWN);
        assertEquals(0, fe.row());
    }

    @Test
    void gameStartFadesThenStartsTheGame() {
        FrontEnd fe = menuReady();
        fe.update(Buttons.JUMP);
        assertEquals(List.of("0/34"), sounds);
        run(fe, 31, 0);
        assertFalse(fe.startGame());
        fe.update(0);
        assertTrue(fe.startGame());
    }

    @Test
    void continueAndOptionDoNothing() {
        FrontEnd fe = menuReady();
        for (int row = 1; row <= 2; row++) {
            fe.update(Buttons.DOWN);
            fe.update(0);
            fe.update(Buttons.START);
            fe.update(0);
            assertEquals(row, fe.row());
        }
        run(fe, 40, 0);
        assertEquals(List.of("0/12", "0/12"), sounds);
        assertEquals(Phase.MENU, fe.phase());
        assertFalse(fe.startGame());
    }

    @Test
    void heldStartDoesNotPickOnTheMenu() {
        FrontEnd fe = new FrontEnd(sfx, Phase.TITLE);
        run(fe, 40, 0);
        run(fe, 200, Buttons.START);
        assertEquals(Phase.MENU, fe.phase());
        assertFalse(fe.startGame());
        assertEquals(0f, fe.darkness());
        assertEquals(List.of("0/34"), sounds);
    }

    @Test
    void pressDuringFadeIsIgnored() {
        FrontEnd fe = new FrontEnd(sfx, Phase.MENU);
        run(fe, 5, 0);
        fe.update(Buttons.DOWN);
        assertEquals(0, fe.row());
        assertTrue(sounds.isEmpty());
    }

    @Test
    void menuIdleReturnsToOpening() {
        FrontEnd fe = menuReady();
        run(fe, 17999, 0);
        assertEquals(0f, fe.darkness());
        fe.update(0);
        run(fe, 31, 0);
        assertEquals(Phase.MENU, fe.phase());
        fe.update(0);
        assertEquals(Phase.OPENING, fe.phase());
    }

    @Test
    void pressStartBlinks() {
        FrontEnd fe = new FrontEnd(sfx, Phase.TITLE);
        assertTrue(fe.textShown(4));
        run(fe, 15, 0);
        assertTrue(fe.textShown(4));
        fe.update(0);
        assertFalse(fe.textShown(4));
        run(fe, 16, 0);
        assertTrue(fe.textShown(4));
        assertFalse(fe.textShown(0), "the menu rows are not on the title");
    }

    @Test
    void menuRowsShowAndTheButtonLineBlinks() {
        FrontEnd fe = new FrontEnd(sfx, Phase.MENU);
        run(fe, 16, 0);
        assertTrue(fe.textShown(0));
        assertTrue(fe.textShown(6));
        assertFalse(fe.textShown(5));
        assertFalse(fe.textShown(7));
        assertFalse(fe.textShown(4));
    }
}
