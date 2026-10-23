package mmx.x6.input;

import static org.junit.jupiter.api.Assertions.assertEquals;

import com.badlogic.gdx.Input.Keys;
import java.util.HashSet;
import java.util.Set;
import org.junit.jupiter.api.Test;

class ButtonsTest {

    @Test
    void buttonsPressedAndReleased() {
        Buttons b = new Buttons();
        Set<Integer> down = new HashSet<>();
        down.add(Keys.K);
        b.sample(down::contains);
        assertEquals(Buttons.JUMP, b.pressed() & Buttons.JUMP);
        assertEquals(Buttons.JUMP, b.held() & Buttons.JUMP);
        b.sample(down::contains);
        assertEquals(0, b.pressed());
        down.clear();
        b.sample(down::contains);
        assertEquals(Buttons.JUMP, b.released());
        b.sample(down::contains);
        assertEquals(0, b.released());
    }

    @Test
    void oppositeDirectionsCancel() {
        Buttons b = new Buttons();
        Set<Integer> down = Set.of(Keys.A, Keys.D, Keys.W);
        b.sample(down::contains);
        assertEquals(Buttons.UP, b.held());
    }

    @Test
    void keyMap() {
        Buttons b = new Buttons();
        b.sample(Set.of(Keys.J, Keys.L, Keys.I, Keys.S, Keys.D)::contains);
        assertEquals(Buttons.BUSTER | Buttons.DASH | Buttons.WEAPON | Buttons.DOWN | Buttons.RIGHT, b.held());
        b.sample(Set.of(Keys.K, Keys.Q, Keys.O, Keys.W, Keys.A)::contains);
        assertEquals(Buttons.JUMP | Buttons.WEAPON_PREV | Buttons.WEAPON_NEXT | Buttons.UP | Buttons.LEFT, b.held());
    }

    @Test
    void scriptedHeld() {
        Buttons b = new Buttons();
        b.set(Buttons.RIGHT | Buttons.JUMP);
        assertEquals(Buttons.RIGHT | Buttons.JUMP, b.pressed());
        b.set(Buttons.RIGHT);
        assertEquals(Buttons.JUMP, b.released());
    }
}
