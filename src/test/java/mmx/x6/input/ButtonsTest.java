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
        down.add(Keys.Z);
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
        Set<Integer> down = Set.of(Keys.LEFT, Keys.RIGHT, Keys.UP);
        b.sample(down::contains);
        assertEquals(Buttons.UP, b.held());
    }

    @Test
    void keyMap() {
        Buttons b = new Buttons();
        b.sample(Set.of(Keys.X, Keys.C, Keys.A, Keys.DOWN)::contains);
        assertEquals(Buttons.BUSTER | Buttons.DASH | Buttons.WEAPON | Buttons.DOWN, b.held());
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
