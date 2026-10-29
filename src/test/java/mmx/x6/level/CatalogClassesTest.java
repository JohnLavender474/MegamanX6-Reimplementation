package mmx.x6.level;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.util.List;
import java.util.Map;
import mmx.x6.DataPaths;
import mmx.x6.game.Entity;
import mmx.x6.game.GoalMember;
import mmx.x6.game.Sim;
import mmx.x6.game.World;
import mmx.x6.game.enemy.BossGate;
import mmx.x6.game.enemy.Enemy;
import mmx.x6.game.enemy.IntroEntityFactory;
import mmx.x6.stage.Themes;
import org.junit.jupiter.api.Test;

class CatalogClassesTest {

    private static final int BUSTER_ID = 19;

    private static Catalog catalog() throws IOException {
        assumeTrue(DataPaths.assetsExist());
        return Catalog.load(DataPaths.assets().catalogJson());
    }

    private static World goalWorld(RecordDef member) {
        CollisionGrid g = new CollisionGrid(32, 32, new int[32 * 32]);
        TileLayer empty = new TileLayer("empty", 32, 32, new int[32 * 32], 0, 0, 0);
        GoalDef goal = new GoalDef(1, 0, 0, 100, 100, GoalMode.ALL, false, 0);
        Level level = new Level(32, 32, empty, empty, empty, empty, g, List.of(member), List.of(), List.of(),
                Map.of(), List.of(), List.of(goal));
        return new World(level, Sim::set, new Sim.RecordingSfx(), new Sim.RecordingMusic());
    }

    @Test
    void catalogAgreesWithClasses() throws IOException {
        Catalog catalog = catalog();
        IntroEntityFactory factory = new IntroEntityFactory();
        int created = 0;
        for (Catalog.Entity e : catalog.entities()) {
            if (!e.themes().contains("intro")) {
                continue;
            }
            RecordDef r = new RecordDef(0, e.name(), "main", 0, 0, 0, 0, 100, 100, 10, 1, 0);
            World world = goalWorld(r);
            Entity entity = factory.create(world, r);
            if (entity == null) {
                continue;
            }
            created++;
            String name = e.name();
            assertEquals(entity instanceof BossGate, e.openable(), name + " openable");
            if (e.defeatable()) {
                assertTrue(entity instanceof GoalMember, name + " is a GoalMember");
                if (entity instanceof Enemy) {
                    world.entities.add(entity);
                    entity.onKilled(world, BUSTER_ID);
                    assertTrue(world.goals.done(1), name + " reports its kill");
                }
            } else if (entity instanceof Enemy) {
                world.entities.add(entity);
                entity.onKilled(world, BUSTER_ID);
                assertFalse(world.goals.done(1), name + " does not complete a goal");
            }
        }
        assertTrue(created > 0);
    }

    @Test
    void everyThemeHasAFactory() throws IOException {
        Catalog catalog = catalog();
        for (String key : catalog.themeKeys()) {
            assertNotNull(Themes.factory(key), key);
        }
    }
}
