package mmx.x6.input;

import com.badlogic.gdx.controllers.Controller;
import com.badlogic.gdx.controllers.ControllerMapping;
import com.badlogic.gdx.controllers.Controllers;
import com.badlogic.gdx.utils.Array;
import java.util.function.Supplier;

public final class GdxPads implements Supplier<Pad> {

    private GdxPad last;

    @Override
    public Pad get() {
        Array<Controller> all = Controllers.getControllers();
        if (all.isEmpty()) {
            last = null;
            return null;
        }
        Controller c = all.first();
        if (last == null || last.controller != c) {
            last = new GdxPad(c);
        }
        return last;
    }

    private static final class GdxPad implements Pad {

        private final Controller controller;

        GdxPad(Controller controller) {
            this.controller = controller;
        }

        @Override
        public String name() {
            return controller.getName();
        }

        @Override
        public boolean button(int code) {
            return controller.getButton(code);
        }

        @Override
        public int buttonCount() {
            return controller.getMaxButtonIndex() + 1;
        }

        @Override
        public int defaultCode(PadButton b) {
            ControllerMapping m = controller.getMapping();
            int code = switch (b) {
                case UP -> m.buttonDpadUp;
                case DOWN -> m.buttonDpadDown;
                case LEFT -> m.buttonDpadLeft;
                case RIGHT -> m.buttonDpadRight;
                case CROSS -> m.buttonA;
                case CIRCLE -> m.buttonB;
                case SQUARE -> m.buttonX;
                case TRIANGLE -> m.buttonY;
                case L1 -> m.buttonL1;
                case R1 -> m.buttonR1;
                case START -> m.buttonStart;
            };
            return code == ControllerMapping.UNDEFINED ? Bindings.NONE : code;
        }
    }
}
