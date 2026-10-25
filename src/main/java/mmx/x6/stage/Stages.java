package mmx.x6.stage;

import java.util.List;
import mmx.x6.game.enemy.IntroEntityFactory;
import mmx.x6.game.enemy.heatnix.HeatnixEntityFactory;
import mmx.x6.game.enemy.turtloid.TurtloidEntityFactory;
import mmx.x6.game.enemy.yammark.YammarkEntityFactory;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.IntroCheckpoints;
import mmx.x6.level.Point;

public final class Stages {

    public static final StageDef INTRO = new StageDef("intro", "INTRO STAGE", 0, "m094-intro", "intro.tmx",
            IntroCheckpoints.ALL, 9, 21, "m094-e05", "m094-e03", "m123-e01", 0, IntroEntityFactory::new);

    public static final StageDef YAMMARK = new StageDef("yammark", "COMMANDER YAMMARK", 1,
            "m095-commander-yammark-section-1", "commander-yammark-section-1.tmx",
            List.of(
                    new Checkpoint(160, 256, 0, 224, 0, 1200, 208, 720, true, 1),
                    new Checkpoint(512, 1504, 432, 1424, 432, 1040, 1424, 1424, true, 1),
                    new Checkpoint(4960, 1968, 4800, 1904, 4800, 4800, 1904, 1904, true, 1),
                    new Checkpoint(3136, 1920, 3024, 1904, 3024, 4528, 1648, 1904, true, 1)),
            11, 22, "m095-e03", null, "m075-e02", 0, YammarkEntityFactory::new);

    public static final StageDef HEATNIX = new StageDef("heatnix", "BLAZE HEATNIX", 3,
            "m097-blaze-heatnix-section-1", "blaze-heatnix-section-1.tmx",
            List.of(
                    new Checkpoint(80, 224, 0, 224, 0, 624, 224, 480, true, 1),
                    new Checkpoint(1088, 464, 944, 464, 944, 976, 464, 2624, true, 1),
                    new Checkpoint(1248, 2624, 976, 2624, 640, 976, 2400, 2624, false, 1),
                    new Checkpoint(1424, 2736, 1280, 2624, 1280, 1280, 1616, 2624, true, 1),
                    new Checkpoint(2784, 2816, 2544, 2816, 2544, 2544, 2816, 2816, true, 16,
                            new Point(0, 0), null),
                    new Checkpoint(2784, 2160, 2544, 2160, 2544, 2544, 2160, 2160, true, 16,
                            new Point(0, 0), null),
                    new Checkpoint(3360, 1024, 3200, 1056, 3200, 3504, 944, 1184, true, 1,
                            null, new Point(-672, 1136)),
                    new Checkpoint(3952, 992, 3808, 944, 3808, 3808, 944, 944, true, 1)),
            14, 22, "m097-e07", "m097-e05", "m077-e02", 4, HeatnixEntityFactory::new);

    public static final StageDef TURTLOID = new StageDef("turtloid", "RAINY TURTLOID", 6,
            "m101-rainy-turtloid-section-1", "rainy-turtloid-section-1.tmx",
            List.of(
                    turtloid(160, 464, 0, 464, 0, 1296, 464),
                    turtloid(1120, 464, 1056, 464, 1056, 1296, 464),
                    turtloid(1760, 464, 1600, 464, 1600, 2272, 464),
                    turtloid(3520, 224, 3488, 224, 3488, 7360, 224),
                    turtloid(7752, 224, 7680, 224, 7680, 8320, 224),
                    turtloid(8784, 624, 8624, 528, 8624, 8624, 528)),
            16, 22, "m101-e05", "m101-e03", "m080-e02", 0, TurtloidEntityFactory::new);

    public static final List<StageDef> ALL = List.of(INTRO, YAMMARK, TURTLOID, HEATNIX);

    private Stages() {
    }

    private static Checkpoint turtloid(int px, int py, int cx, int cy, int minX, int maxX, int y) {
        return new Checkpoint(px, py, cx, cy, minX, maxX, y, y, true, 1, new Point(320, 0), null);
    }

    public static StageDef byKey(String key) {
        for (StageDef s : ALL) {
            if (s.key().equalsIgnoreCase(key)) {
                return s;
            }
        }
        return null;
    }
}
