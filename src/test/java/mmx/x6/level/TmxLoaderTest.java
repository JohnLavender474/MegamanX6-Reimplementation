package mmx.x6.level;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import mmx.x6.DataPaths;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

class TmxLoaderTest {

    private static Level level;

    @BeforeAll
    static void load() throws Exception {
        if (DataPaths.defaultRootExists()) {
            level = TmxLoader.load(DataPaths.resolve(new String[0]).tmx());
        }
    }

    private static Level level() {
        assumeTrue(level != null);
        return level;
    }

    private static CameraZoneDef zone(int id) {
        return level().cameraZones().stream().filter(z -> z.zoneId() == id).findFirst().orElseThrow();
    }

    private static RecordDef record(int index) {
        return level().records().stream().filter(r -> r.index() == index).findFirst().orElseThrow();
    }

    @Test
    void loadsTheIntroMap() {
        Level l = level();
        assertEquals(416, l.width());
        assertEquals(96, l.height());
        assertEquals(57, l.records().size());
        assertEquals(13, l.cameraZones().size());
        assertEquals(1, l.art1().x6Mode());
        assertEquals(560, l.art1().x6OffsetY());
        assertEquals(3, l.art2().x6Mode());
        assertEquals(640, l.art2().x6OffsetY());
        assertEquals(416 * 96, l.art0Main().gids().length);
    }

    @Test
    void collisionValuesAreGidMinusFirstGid() {
        Level l = level();
        Map<Integer, Integer> count = new TreeMap<>();
        for (int y = 0; y < 96; y++) {
            for (int x = 0; x < 416; x++) {
                count.merge(l.collision().raw(x, y), 1, Integer::sum);
            }
        }
        assertEquals(33603, count.get(0));
        assertEquals(6, count.get(1));
        assertEquals(6, count.get(4));
        assertEquals(7, count.get(5));
        assertEquals(7, count.get(8));
        assertEquals(52, count.get(16));
        assertEquals(7, count.get(32));
        assertEquals(1, count.get(33));
        assertEquals(2, count.get(34));
        assertEquals(5200, count.get(56));
        assertEquals(961, count.get(57));
        assertEquals(20, count.get(59));
        assertEquals(21, count.get(63));
        assertEquals(17, count.get(45));
    }

    @Test
    void cameraZone0() {
        CameraZoneDef z = zone(0);
        assertEquals(List.of(2272, 2303, 1040, 1135, -1, -1, 944, -1),
                List.of(z.x0(), z.x1(), z.y0(), z.y1(), z.minX(), z.maxX(), z.minY(), z.maxY()));
        assertFalse(z.textureSwap());
        assertEquals(2, z.record());
    }

    @Test
    void cameraZone1IsTheTextureSwapZone() {
        CameraZoneDef z = zone(1);
        assertTrue(z.textureSwap());
        assertEquals(1968, z.minX());
        assertEquals(2128, z.x0());
        assertEquals(2159, z.x1());
    }

    @Test
    void cameraZone9() {
        CameraZoneDef z = zone(9);
        assertEquals(List.of(5152, 5168, 3136, 704, 704),
                List.of(z.x0(), z.x1(), z.minX(), z.minY(), z.maxY()));
    }

    @Test
    void sabreBlockRecord40() {
        RecordDef r = record(40);
        assertEquals("SabreBlock", r.name());
        assertEquals(4, r.kind());
        assertEquals(2, r.b());
        assertEquals(2016, r.x());
        assertEquals(984, r.y());
    }

    @Test
    void d1000Record() {
        RecordDef r = record(56);
        assertEquals("D1000", r.name());
        assertEquals(0, r.kind());
        assertEquals(48, r.b());
        assertEquals(6162, r.x());
        assertEquals(346, r.y());
    }

    @Test
    void checkpoints() {
        assertEquals(new Checkpoint(144, 976, 0, 944, 0, 5120, 896, 944, true, 1), IntroCheckpoints.ALL.get(0));
        assertEquals(new Checkpoint(2112, 1072, 1968, 944, 1968, 5120, 944, 944, true, 6), IntroCheckpoints.ALL.get(1));
        assertEquals(new Checkpoint(5904, 364, 5744, 224, 5744, 5744, 224, 224, true, 1), IntroCheckpoints.ALL.get(2));
    }
}
