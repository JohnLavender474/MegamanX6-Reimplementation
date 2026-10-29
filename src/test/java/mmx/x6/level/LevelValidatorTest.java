package mmx.x6.level;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import com.badlogic.gdx.utils.XmlReader;
import java.util.Collections;
import org.junit.jupiter.api.Test;

class LevelValidatorTest {

    private static final Catalog CATALOG = Catalog.parse(TmxFixture.CATALOG);

    private static ValidationReport check(TmxFixture f) {
        return LevelValidator.validate(f.build(), CATALOG, name -> null);
    }

    private static void assertFinding(ValidationReport r, String code, Finding.Result result) {
        assertTrue(r.findings().stream().anyMatch(f -> f.code().equals(code) && f.result() == result),
                "expected " + result + " " + code + " in " + r.findings());
    }

    private static String cells(int count, String value) {
        return String.join(",", Collections.nCopies(count, value));
    }

    @Test
    void validFixtureIsCertified() {
        ValidationReport r = check(TmxFixture.valid());
        assertEquals(ValidationReport.Status.CERTIFIED, r.status());
        assertTrue(r.findings().isEmpty(), r.findings().toString());
    }

    @Test
    void notXmlIsRejectedWithoutThrowing() {
        ValidationReport r = LevelValidator.validate("not xml", CATALOG, name -> null);
        assertEquals(ValidationReport.Status.REJECTED, r.status());
        assertNull(r.map());
        assertFinding(r, "parse", Finding.Result.REJECT);
    }

    @Test
    void wrongRootIsRejected() {
        ValidationReport r = LevelValidator.validate("<tileset/>", CATALOG, name -> null);
        assertFinding(r, "parse", Finding.Result.REJECT);
        assertNull(r.map());
    }

    @Test
    void infiniteMapIsRejected() {
        assertFinding(check(TmxFixture.valid().mapAttr("infinite", "1")), "map", Finding.Result.REJECT);
    }

    @Test
    void sizeNotAMultipleOf16IsRejected() {
        assertFinding(check(TmxFixture.valid().mapAttr("width", "20")), "map", Finding.Result.REJECT);
    }

    @Test
    void wrongTileSizeIsRejected() {
        assertFinding(check(TmxFixture.valid().mapAttr("tilewidth", "8")), "map", Finding.Result.REJECT);
    }

    @Test
    void groupLayerIsRejected() {
        assertFinding(check(TmxFixture.valid().raw("<group id=\"99\" name=\"g\"/>")), "group-layer",
                Finding.Result.REJECT);
    }

    @Test
    void embeddedTilesetIsRejected() {
        TmxFixture f = TmxFixture.valid().tilesets("<tileset firstgid=\"1\" name=\"t\" tilewidth=\"16\" "
                + "tileheight=\"16\" tilecount=\"4\" columns=\"2\"><image source=\"t.png\" width=\"32\" "
                + "height=\"32\"/></tileset>\n<tileset firstgid=\"3265\" source=\"../tsx/collision.tsx\"/>\n");
        assertFinding(check(f), "tileset", Finding.Result.REJECT);
    }

    @Test
    void oneTilesetIsRejected() {
        assertFinding(check(TmxFixture.valid().tilesets("<tileset firstgid=\"1\" source=\"../tsx/intro-tiles.tsx\"/>")),
                "tileset", Finding.Result.REJECT);
    }

    @Test
    void wrongArtTilesetIsRejected() {
        TmxFixture f = TmxFixture.valid().tilesets("<tileset firstgid=\"1\" source=\"../tsx/other.tsx\"/>\n"
                + "<tileset firstgid=\"3265\" source=\"../tsx/collision.tsx\"/>\n");
        assertFinding(check(f), "tileset", Finding.Result.REJECT);
    }

    @Test
    void wrongCollisionFirstGidIsRejected() {
        TmxFixture f = TmxFixture.valid().tilesets("<tileset firstgid=\"1\" source=\"../tsx/intro-tiles.tsx\"/>\n"
                + "<tileset firstgid=\"3000\" source=\"../tsx/collision.tsx\"/>\n");
        assertFinding(check(f), "tileset", Finding.Result.REJECT);
    }

    @Test
    void missingLayerIsRejected() {
        assertFinding(check(TmxFixture.valid().removeLayer("art 0 front")), "layer-missing", Finding.Result.REJECT);
    }

    @Test
    void layerSizeDifferingFromTheMapIsRejected() {
        assertFinding(check(TmxFixture.valid().layerAttr("collision", "width", "32")), "layer",
                Finding.Result.REJECT);
    }

    @Test
    void base64DataIsRejected() {
        String text = TmxFixture.valid().build().replaceFirst("<data encoding=\"csv\">", "<data encoding=\"base64\">");
        assertFinding(LevelValidator.validate(text, CATALOG, name -> null), "layer", Finding.Result.REJECT);
    }

    @Test
    void compressedDataIsRejected() {
        String text = TmxFixture.valid().build().replaceFirst("<data encoding=\"csv\">",
                "<data encoding=\"csv\" compression=\"zlib\">");
        assertFinding(LevelValidator.validate(text, CATALOG, name -> null), "layer", Finding.Result.REJECT);
    }

    @Test
    void emptyDataDoesNotThrow() {
        String text = TmxFixture.valid().build().replaceFirst("(?s)<data encoding=\"csv\">.*?</data>",
                "<data encoding=\"csv\"/>");
        assertFinding(LevelValidator.validate(text, CATALOG, name -> null), "layer", Finding.Result.REJECT);
    }

    @Test
    void artGidAboveTilecountIsRejected() {
        TmxFixture f = TmxFixture.valid().layerData("art 0 main", "3265," + cells(255, "0"));
        assertFinding(check(f), "gid-range", Finding.Result.REJECT);
    }

    @Test
    void collisionGidOutsideItsRangeIsRejected() {
        assertFinding(check(TmxFixture.valid().layerData("collision", "5," + cells(255, "0"))), "gid-range",
                Finding.Result.REJECT);
        assertFinding(check(TmxFixture.valid().layerData("collision", "3329," + cells(255, "0"))), "gid-range",
                Finding.Result.REJECT);
    }

    @Test
    void collisionGidInRangeIsCertified() {
        ValidationReport r = check(TmxFixture.valid().layerData("collision", "3265,3328," + cells(254, "0")));
        assertEquals(ValidationReport.Status.CERTIFIED, r.status());
    }

    @Test
    void flipBitsAreRepaired() {
        TmxFixture f = TmxFixture.valid().layerData("art 0 main", "2147483649," + cells(255, "0"));
        ValidationReport r = check(f);
        assertFinding(r, "flip-bits", Finding.Result.REPAIR);
        assertEquals(ValidationReport.Status.CERTIFIED, r.status());
        XmlReader.Element layer = null;
        for (XmlReader.Element l : r.map().getChildrenByName("layer")) {
            if ("art 0 main".equals(l.getAttribute("name"))) {
                layer = l;
            }
        }
        assertTrue(layer.getChildByName("data").getText().startsWith("1,0,"));
    }

    @Test
    void unknownThemeSkipsTilesetAndGidRules() {
        TmxFixture f = TmxFixture.valid().mapProperty("x6_theme", "string", "nope")
                .layerData("art 0 main", "99999," + cells(255, "0"));
        assertTrue(check(f).findings().isEmpty());
    }
}
