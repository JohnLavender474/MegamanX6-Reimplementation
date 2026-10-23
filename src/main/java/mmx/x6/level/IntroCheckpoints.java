package mmx.x6.level;

import java.util.List;

public final class IntroCheckpoints {

    public static final int L1_OX = 0;
    public static final int L1_OY = 560;
    public static final int L2_OX = 0;
    public static final int L2_OY = 640;

    public static final List<Checkpoint> ALL = List.of(
            new Checkpoint(144, 976, 0, 944, 0, 5120, 896, 944, true, 1),
            new Checkpoint(2112, 1072, 1968, 944, 1968, 5120, 944, 944, true, 6),
            new Checkpoint(5904, 364, 5744, 224, 5744, 5744, 224, 224, true, 1));

    private IntroCheckpoints() {
    }
}
