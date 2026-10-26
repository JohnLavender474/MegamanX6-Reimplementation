package mmx.x6.screen;

final class ShotClock {

    private final String base;
    private final int[] times;
    private int index;

    ShotClock(String base, String list) {
        this.base = base;
        this.times = Shots.times(list);
    }

    static ShotClock fromProperties() {
        return new ShotClock(System.getProperty("mmx.shot"), System.getProperty("mmx.shotAfter", "30"));
    }

    boolean active() {
        return base != null;
    }

    String due(int updates) {
        if (!active() || finished() || updates < times[index]) {
            return null;
        }
        return Shots.path(base, times, index++);
    }

    boolean finished() {
        return active() && index >= times.length;
    }
}
