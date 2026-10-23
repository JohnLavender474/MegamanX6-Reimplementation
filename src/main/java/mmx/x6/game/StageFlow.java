package mmx.x6.game;

import mmx.x6.level.IntroCheckpoints;

public final class StageFlow {

    public enum State { PLAYING, FADE_OUT, CLEAR }

    private static final int FADE_UPDATES = 30;

    private final World world;
    private State state = State.PLAYING;
    private int timer;
    private int restarts;

    public StageFlow(World world) {
        this.world = world;
    }

    public State state() {
        return state;
    }

    public void playerDeathDone() {
        if (state == State.PLAYING) {
            state = State.FADE_OUT;
            // TODO: the restart path after the death orbs is not read; a 30-update fade is a guess (docs/gaps.md, "Respawn")
            timer = FADE_UPDATES;
        }
    }

    public void clear() {
        if (state == State.CLEAR) {
            return;
        }
        state = State.CLEAR;
        world.music.stop();
        world.lockCount++;
    }

    public void anyKey() {
        if (state == State.CLEAR) {
            restart(0);
        }
    }

    public void update() {
        if (state == State.FADE_OUT && --timer <= 0) {
            restart(world.checkpoint);
        }
    }

    public void restart(int checkpoint) {
        restarts++;
        state = State.PLAYING;
        timer = 0;
        world.resetTo(checkpoint, IntroCheckpoints.ALL.get(checkpoint));
    }

    public boolean restarted() {
        return restarts > 0;
    }

    public float fade() {
        if (state == State.FADE_OUT) {
            return 1f - timer / (float) FADE_UPDATES;
        }
        return 0f;
    }
}
