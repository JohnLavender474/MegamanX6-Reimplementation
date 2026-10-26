package mmx.x6.frontend;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class FrontEndTextTest {

    @Test
    void sheetRectOfOrangeGameStart() {
        assertArrayEquals(new int[] {264, 4, 134, 12}, FrontEndText.sheetRect(0, true));
        assertArrayEquals(new int[] {4, 142, 60, 9}, FrontEndText.sheetRect(6, false));
    }

    @Test
    void buttonTextSitsRightOfPressStart() {
        FrontEndText.Text t = FrontEndText.text(7, 0);
        FrontEndText.Sprite s = FrontEndText.sprite(t.sprite());
        assertEquals(348, t.x() - s.ox());
        assertEquals(190, t.y() - s.oy());
    }

    @Test
    void highlightFollowsTheRow() {
        FrontEndText.Text t = FrontEndText.text(6, 2);
        assertEquals(258, t.x());
        assertEquals(174, t.y());
        assertEquals(2, t.sprite());
        assertTrue(t.orange());
        assertEquals(new FrontEndText.Text(0, 258, 142, 0, false), FrontEndText.text(0, 2));
    }

    @Test
    void highlightIsDrawnLastOnTheMenu() {
        assertArrayEquals(new int[] {0, 1, 2, 5, 7, 6}, FrontEndText.MENU_VARS);
        assertArrayEquals(new int[] {4}, FrontEndText.TITLE_VARS);
    }
}
