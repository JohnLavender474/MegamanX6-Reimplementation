package mmx.x6.level;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import mmx.x6.DataPaths;
import mmx.x6.game.Entity;
import mmx.x6.game.GoalMember;
import mmx.x6.game.enemy.BossGate;
import mmx.x6.game.enemy.IntroEntityFactory;
import mmx.x6.stage.Stages;
import mmx.x6.stage.StageRun;
import mmx.x6.stage.Themes;
import org.junit.jupiter.api.Test;

class CatalogClassesTest {

    private static Catalog catalog() throws IOException {
        assumeTrue(DataPaths.assetsExist());
        return Catalog.load(DataPaths.assets().catalogJson());
    }

    @Test
    void catalogAgreesWithClasses() throws IOException {
        Catalog catalog = catalog();
        StageRun run = StageRun.at(Stages.INTRO, 0);
        IntroEntityFactory factory = new IntroEntityFactory();
        int checked = 0;
        for (String name : new String[] {"MetaWheelF", "MetaDridler", "Junkroid", "SabreBlock", "HoverGunner",
                "BossGate", "D1000"}) {
            Catalog.Entity e = catalog.entity(name);
            assertNotNull(e, name);
            RecordDef r = new RecordDef(0, name, "main", 0, 0, 0, 0, 6162, 346);
            Entity created = factory.create(run.world, r);
            if (created == null) {
                continue;
            }
            checked++;
            assertEquals(created instanceof GoalMember, e.defeatable(), name + " defeatable");
            assertEquals(created instanceof BossGate, e.openable(), name + " openable");
        }
        assertEquals(7, checked);
    }

    @Test
    void everyThemeHasAFactory() throws IOException {
        Catalog catalog = catalog();
        for (String key : catalog.themeKeys()) {
            assertNotNull(Themes.factory(key), key);
        }
    }
}
