package mmx.x6.anim;

public final class Animator {

    private final AnimSet set;
    private AnimDef def;
    private int anim = -1;
    private int frame;
    private int remaining;
    private boolean finished;
    private boolean flagEntered;

    public Animator(AnimSet set) {
        this.set = set;
    }

    public AnimSet set() {
        return set;
    }

    public void play(int anim) {
        play(anim, 0);
    }

    public void play(int anim, int frame) {
        AnimDef d = set.get(anim);
        if (d == null || d.frames().isEmpty()) {
            return;
        }
        this.def = d;
        this.anim = anim;
        this.frame = Math.max(0, Math.min(frame, d.frames().size() - 1));
        this.remaining = d.frames().get(this.frame).ticks();
        this.finished = false;
        this.flagEntered = d.frames().get(this.frame).flag() != 0;
    }

    public void playIfNot(int anim) {
        if (this.anim != anim) {
            play(anim);
        }
    }

    public void tick() {
        flagEntered = false;
        if (def == null || finished) {
            return;
        }
        if (--remaining > 0) {
            return;
        }
        int next = frame + 1;
        if (next >= def.frames().size()) {
            if (def.loopTo() == null) {
                finished = true;
                return;
            }
            next = def.loopTo();
        }
        frame = next;
        remaining = def.frames().get(frame).ticks();
        flagEntered = def.frames().get(frame).flag() != 0;
    }

    public int anim() {
        return anim;
    }

    public int frameIndex() {
        return frame;
    }

    public int sprite() {
        return def == null ? -1 : def.frames().get(frame).sprite();
    }

    public int flag() {
        return def == null ? 0 : def.frames().get(frame).flag();
    }

    public boolean flagEntered() {
        return flagEntered;
    }

    public boolean atLastFrame() {
        return def != null && frame == def.frames().size() - 1;
    }

    public boolean finished() {
        return finished;
    }

    public int remaining() {
        return remaining;
    }

    public boolean entered(int mask) {
        return flagEntered && (flag() & mask) != 0;
    }

    public boolean enteredFlag(int value) {
        return flagEntered && flag() == value;
    }
}
