package mmx.x6.level;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.stream.Stream;
import mmx.x6.DataPaths;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

class TmxLoaderGoldenTest {

    static Stream<Path> maps() throws IOException {
        if (!DataPaths.assetsExist()) {
            return Stream.of();
        }
        try (Stream<Path> s = Files.list(DataPaths.assets().tmx().getParent())) {
            return s.filter(p -> p.toString().endsWith(".tmx")).sorted().toList().stream();
        }
    }

    private static List<List<Object>> nine(List<RecordDef> records) {
        return records.stream().map(r -> List.<Object>of(r.index(), r.name(), r.type(), r.kind(), r.b(), r.id(),
                r.a(), r.x(), r.y())).toList();
    }

    private static void sameLayer(TileLayer a, TileLayer b) {
        assertEquals(a.name(), b.name());
        assertEquals(a.width(), b.width());
        assertEquals(a.height(), b.height());
        assertEquals(a.x6Mode(), b.x6Mode());
        assertEquals(a.x6OffsetX(), b.x6OffsetX());
        assertEquals(a.x6OffsetY(), b.x6OffsetY());
        assertArrayEquals(a.gids(), b.gids());
    }

    @ParameterizedTest
    @MethodSource("maps")
    void newLoaderMatchesLegacy(Path tmx) throws Exception {
        assumeTrue(DataPaths.assetsExist());
        Level a = LegacyTmxLoader.load(tmx);
        Level b = TmxLoader.load(tmx);
        assertEquals(a.width(), b.width());
        assertEquals(a.height(), b.height());
        sameLayer(a.art2(), b.art2());
        sameLayer(a.art1(), b.art1());
        sameLayer(a.art0Main(), b.art0Main());
        sameLayer(a.art0Front(), b.art0Front());
        for (int y = 0; y < a.height(); y++) {
            for (int x = 0; x < a.width(); x++) {
                assertEquals(a.collision().raw(x, y), b.collision().raw(x, y));
            }
        }
        assertEquals(a.cameraZones(), b.cameraZones());
        assertEquals(nine(a.records()), nine(b.records()));
        assertEquals(nine(a.fixedRecords()), nine(b.fixedRecords()));
        for (RecordDef r : b.records()) {
            assertTrue(r.tmxId() > 0);
        }
        for (RecordDef r : b.fixedRecords()) {
            assertTrue(r.tmxId() > 0);
        }
    }
}
