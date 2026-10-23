package mmx.x6.anim;

import java.util.List;

public record AnimDef(int index, List<Frame> frames, Integer loopTo) {

    public record Frame(int sprite, int ticks, int flag) {
    }
}
