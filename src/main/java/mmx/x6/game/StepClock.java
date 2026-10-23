package mmx.x6.game;

public final class StepClock {

    public static final int MAX_UPDATES = 4;
    private static final double STEP = 1.0 / 60.0;

    private double acc;

    public int advance(float deltaSeconds) {
        acc += deltaSeconds;
        int n = 0;
        while (acc >= STEP - 1e-6) {
            acc -= STEP;
            n++;
        }
        if (n > MAX_UPDATES) {
            n = MAX_UPDATES;
            acc = 0;
        }
        return n;
    }
}
