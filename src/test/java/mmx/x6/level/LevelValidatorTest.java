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
    void unknownThemeRejectsAndSkipsTilesetAndGidRules() {
        TmxFixture f = TmxFixture.valid().mapProperty("x6_theme", "string", "nope")
                .layerData("art 0 main", "99999," + cells(255, "0"));
        ValidationReport r = check(f);
        assertFinding(r, "theme", Finding.Result.REJECT);
        assertTrue(r.findings().stream().noneMatch(x -> x.code().equals("tileset") || x.code().equals("gid-range")),
                r.findings().toString());
    }

    private static String record(String name, String idx, String kind) {
        return "<object id=\"" + (idx.hashCode() & 0xFFF) + "\" name=\"" + name + "\" x=\"16\" y=\"16\">"
                + "<properties><property name=\"index\" type=\"int\" value=\"" + idx + "\"/>"
                + "<property name=\"kind\" type=\"int\" value=\"" + kind + "\"/></properties></object>";
    }

    private static String checkpoint(int id, String number, String faceRight) {
        return "<object id=\"" + id + "\" name=\"c" + id + "\" x=\"32\" y=\"32\"><properties>"
                + "<property name=\"checkpoint\" type=\"int\" value=\"" + number + "\"/>"
                + "<property name=\"cam_x\" type=\"int\" value=\"0\"/>"
                + "<property name=\"cam_y\" type=\"int\" value=\"0\"/>"
                + "<property name=\"min_x\" type=\"int\" value=\"0\"/>"
                + "<property name=\"max_x\" type=\"int\" value=\"256\"/>"
                + "<property name=\"min_y\" type=\"int\" value=\"0\"/>"
                + "<property name=\"max_y\" type=\"int\" value=\"256\"/>"
                + "<property name=\"face_right\" type=\"bool\" value=\"" + faceRight + "\"/>"
                + "<property name=\"beam_thru\" type=\"int\" value=\"0\"/>"
                + "</properties><point/></object>";
    }

    @Test
    void missingProfileIsRejected() {
        assertFinding(check(TmxFixture.valid().removeMapProperty("x6_profile")), "profile", Finding.Result.REJECT);
    }

    @Test
    void profileOtherThanOneIsRejected() {
        assertFinding(check(TmxFixture.valid().mapProperty("x6_profile", "int", "2")), "profile",
                Finding.Result.REJECT);
    }

    @Test
    void missingThemeIsRejected() {
        assertFinding(check(TmxFixture.valid().removeMapProperty("x6_theme")), "theme", Finding.Result.REJECT);
    }

    @Test
    void unknownThemeIsRejected() {
        assertFinding(check(TmxFixture.valid().mapProperty("x6_theme", "string", "nope")), "theme",
                Finding.Result.REJECT);
    }

    @Test
    void musicOtherThanThemeIsRejected() {
        assertFinding(check(TmxFixture.valid().mapProperty("x6_music", "int", "10")), "property",
                Finding.Result.REJECT);
    }

    @Test
    void missingThemePropertyIsRejected() {
        assertFinding(check(TmxFixture.valid().removeMapProperty("x6_bank5")), "property", Finding.Result.REJECT);
    }

    @Test
    void intPropertyWithLettersIsRejected() {
        ValidationReport r = check(TmxFixture.valid().objectLayer("records", record("MetaWheelF", "0", "abc")));
        assertFinding(r, "property-type", Finding.Result.REJECT);
    }

    @Test
    void boolPropertyWithYesIsRejected() {
        ValidationReport r = check(TmxFixture.valid().objectLayer("x6 checkpoints", checkpoint(1, "0", "yes")));
        assertFinding(r, "property-type", Finding.Result.REJECT);
    }

    @Test
    void missingCheckpointLayerIsRejected() {
        assertFinding(check(TmxFixture.valid().removeLayer("x6 checkpoints")), "checkpoint",
                Finding.Result.REJECT);
    }

    @Test
    void checkpointGapIsRejected() {
        ValidationReport r = check(TmxFixture.valid().objectLayer("x6 checkpoints",
                checkpoint(1, "0", "true") + checkpoint(2, "2", "true")));
        assertFinding(r, "checkpoint", Finding.Result.REJECT);
    }

    @Test
    void duplicateCheckpointNumberIsRejected() {
        ValidationReport r = check(TmxFixture.valid().objectLayer("x6 checkpoints",
                checkpoint(1, "0", "true") + checkpoint(2, "0", "true")));
        assertFinding(r, "checkpoint", Finding.Result.REJECT);
    }

    @Test
    void checkpointThatIsNotAPointIsRejected() {
        String box = checkpoint(1, "0", "true").replace("<point/>", "");
        assertFinding(check(TmxFixture.valid().objectLayer("x6 checkpoints", box)), "checkpoint",
                Finding.Result.REJECT);
    }

    @Test
    void checkpointWithoutRequiredPropertyIsRejected() {
        String cut = checkpoint(1, "0", "true").replace("<property name=\"cam_x\" type=\"int\" value=\"0\"/>", "");
        assertFinding(check(TmxFixture.valid().objectLayer("x6 checkpoints", cut)), "checkpoint",
                Finding.Result.REJECT);
    }

    @Test
    void duplicateIndexIsRenumberedAndZoneFollows() {
        String zone = "<object id=\"9\" name=\"CameraZone\" x=\"0\" y=\"0\" width=\"16\" height=\"16\">"
                + "<properties><property name=\"record\" type=\"int\" value=\"5\"/></properties></object>";
        ValidationReport r = check(TmxFixture.valid()
                .objectLayer("records", record("MetaWheelF", "5", "0") + record("MetaWheelF", "5", "0")
                        + record("MetaWheelF", "7", "0"))
                .objectLayer("camera zones", zone));
        assertFinding(r, "record-index", Finding.Result.REPAIR);
        XmlReader.Element records = null;
        XmlReader.Element zones = null;
        for (XmlReader.Element g : r.map().getChildrenByName("objectgroup")) {
            if ("records".equals(g.getAttribute("name"))) {
                records = g;
            } else if ("camera zones".equals(g.getAttribute("name"))) {
                zones = g;
            }
        }
        StringBuilder order = new StringBuilder();
        for (XmlReader.Element o : records.getChildrenByName("object")) {
            for (XmlReader.Element p : o.getChildByName("properties").getChildrenByName("property")) {
                if ("index".equals(p.getAttribute("name"))) {
                    order.append(p.getAttribute("value"));
                }
            }
        }
        assertEquals("012", order.toString());
        XmlReader.Element zoneProp = zones.getChildByName("object").getChildByName("properties")
                .getChildByName("property");
        assertEquals("0", zoneProp.getAttribute("value"));
        assertEquals(ValidationReport.Status.CERTIFIED, r.status());
    }

    @Test
    void permutedIndexIsLeftAlone() {
        ValidationReport r = check(TmxFixture.valid()
                .objectLayer("records", record("MetaWheelF", "1", "0") + record("MetaWheelF", "0", "0")));
        assertTrue(r.findings().isEmpty(), r.findings().toString());
    }

    @Test
    void unknownEntityWarns() {
        ValidationReport r = check(TmxFixture.valid().objectLayer("records", record("Nope", "0", "0")));
        assertFinding(r, "entity-unknown", Finding.Result.WARN);
        assertEquals(ValidationReport.Status.PLAYABLE, r.status());
    }

    @Test
    void stageBoundRecordIsLocked() {
        ValidationReport r = check(TmxFixture.valid().objectLayer("fixed records", record("BossGate", "0", "0")));
        assertFinding(r, "stage-bound", Finding.Result.LOCK);
        assertEquals(ValidationReport.Status.PLAYABLE, r.status());
    }

    private static final Catalog REGION_CATALOG = Catalog.parse(TmxFixture.CATALOG.replace(
            "\"tiles_png\": \"intro-tiles.png\",",
            "\"tiles_png\": \"intro-tiles.png\", \"fixed_region\": {\"x0\": 32, \"x1\": 63, "
                    + "\"template\": \"t.tmx\"},"));

    private static ValidationReport checkRegion(TmxFixture f, String template) {
        return LevelValidator.validate(f.build(), REGION_CATALOG, name -> "t.tmx".equals(name) ? template : null);
    }

    private static String cellsWith(int index, String value) {
        String[] a = new String[256];
        java.util.Arrays.fill(a, "0");
        a[index] = value;
        return String.join(",", a);
    }

    @Test
    void changedTileInRegionIsRejected() {
        String template = TmxFixture.valid().build();
        ValidationReport r = checkRegion(TmxFixture.valid().layerData("art 0 main", cellsWith(3, "5")), template);
        assertFinding(r, "fixed-region", Finding.Result.REJECT);
    }

    @Test
    void changedTileOutsideRegionIsAccepted() {
        String template = TmxFixture.valid().build();
        ValidationReport r = checkRegion(TmxFixture.valid().layerData("art 0 main", cellsWith(0, "5")), template);
        assertTrue(r.findings().isEmpty(), r.findings().toString());
    }

    @Test
    void changedObjectInRegionIsRejected() {
        String template = TmxFixture.valid().build();
        ValidationReport r = checkRegion(TmxFixture.valid().objectLayer("fixed records",
                "<object id=\"3\" name=\"MetaWheelF\" x=\"40\" y=\"16\"/>"), template);
        assertFinding(r, "fixed-region", Finding.Result.REJECT);
    }

    @Test
    void objectIdIsIgnoredInRegionCompare() {
        String obj = "<object id=\"%d\" name=\"MetaWheelF\" x=\"40\" y=\"16\"><properties>"
                + "<property name=\"index\" type=\"int\" value=\"0\"/></properties></object>";
        String template = TmxFixture.valid().objectLayer("records", String.format(obj, 3)).build();
        ValidationReport r = checkRegion(TmxFixture.valid().objectLayer("records", String.format(obj, 8)), template);
        assertTrue(r.findings().stream().noneMatch(f -> f.code().equals("fixed-region")), r.findings().toString());
    }

    @Test
    void missingTemplateIsRejected() {
        ValidationReport r = checkRegion(TmxFixture.valid(), null);
        assertFinding(r, "fixed-region", Finding.Result.REJECT);
    }
}
