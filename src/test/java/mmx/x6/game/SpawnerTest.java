package mmx.x6.game;

import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.ArrayList;
import java.util.List;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class SpawnerTest {

    private static final String ROW = "............................................................................";

    private static final class Factory implements EntityFactory {
        final List<Entity> made = new ArrayList<>();

        @Override
        public Entity create(World w, RecordDef r) {
            TestTarget t = new TestTarget(r.x(), r.y(), 8, 8, 1, DamageTables.Table.NORMAL);
            t.record = r;
            made.add(t);
            return t;
        }
    }

    private static RecordDef rec(int index, int kind, int x, int y) {
        return new RecordDef(index, "Junkroid", "main", kind, 1, 1, 0, x, y);
    }

    private static Sim sim(Factory f, RecordDef... records) {
        Sim s = Sim.on(grid(ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW, ROW,
                ROW, ROW, ROW)).playerAt(160, 200);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        s.world.spawner = new Spawner(List.of(records), 1, f);
        return s;
    }

    @Test
    void spawnBandIs48px() {
        Factory f = new Factory();
        Sim s = sim(f, rec(1, 0, 320 + 47, 100), rec(2, 0, 320 + 49, 100), rec(3, 0, 100, -47), rec(4, 0, 100, -49));
        s.step(0);
        assertEquals(List.of(1, 3), f.made.stream().map(e -> e.record.index()).toList());
    }

    @Test
    void kindFilterNormal() {
        Factory f = new Factory();
        Sim s = sim(f, rec(1, 0, 100, 100), rec(2, 1, 120, 100), rec(3, 2, 140, 100), rec(4, 4, 160, 100));
        s.step(0);
        assertEquals(List.of(1, 2, 4), f.made.stream().map(e -> e.record.index()).toList());
    }

    @Test
    void oneObjectPerRecord() {
        Factory f = new Factory();
        Sim s = sim(f, rec(1, 0, 100, 100));
        s.steps(5, 0);
        assertEquals(1, f.made.size());
    }

    @Test
    void releasedRecordSpawnsAgainOnlyAfterLeavingTheBand() {
        Factory f = new Factory();
        RecordDef r = rec(1, 0, 100, 100);
        Sim s = sim(f, r);
        s.step(0);
        f.made.get(0).dead = true;
        s.world.spawner.released(r);
        s.steps(3, 0);
        assertEquals(1, f.made.size());
        s.world.camera.reset(new Checkpoint(0, 0, 600, 0, 600, 600, 0, 0, true, 1));
        s.step(0);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        s.step(0);
        assertEquals(2, f.made.size());
    }

    @Test
    void keptRecordNeverSpawns() {
        Factory f = new Factory();
        RecordDef r = rec(1, 4, 100, 100);
        Sim s = sim(f, r);
        s.step(0);
        f.made.get(0).dead = true;
        s.world.spawner.kept(r);
        s.world.camera.reset(new Checkpoint(0, 0, 600, 0, 600, 600, 0, 0, true, 1));
        s.step(0);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        s.step(0);
        assertEquals(1, f.made.size());
        s.world.spawner.reset();
        s.step(0);
        assertEquals(2, f.made.size());
    }

    @Test
    void checkpointMarkers() {
        Factory f = new Factory();
        Sim s = sim(f, new RecordDef(0, "CheckpointMarker", "logic", 3, 6, 1, 0, 300, 100),
                new RecordDef(1, "CheckpointMarker", "logic", 3, 6, 2, 0, 600, 100));
        s.step(0);
        assertEquals(0, s.world.checkpoint);
        s.world.player.placeStanding(310, 200);
        s.step(0);
        assertEquals(1, s.world.checkpoint);
        assertEquals(0, f.made.size());
    }
}
