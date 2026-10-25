package mmx.x6.game;

import java.util.HashSet;
import java.util.List;
import java.util.Set;
import mmx.x6.level.RecordDef;

public final class Spawner {

    public static final int BAND = 48;

    private final List<RecordDef> records;
    private final int difficulty;
    private final EntityFactory factory;
    private final Set<RecordDef> spawned = new HashSet<>();
    private final Set<RecordDef> kept = new HashSet<>();
    private final Set<RecordDef> inBand = new HashSet<>();

    private final List<RecordDef> fixed;
    private boolean fixedMade;

    public Spawner(List<RecordDef> records, int difficulty, EntityFactory factory) {
        this(records, List.of(), difficulty, factory);
    }

    public Spawner(List<RecordDef> records, List<RecordDef> fixed, int difficulty, EntityFactory factory) {
        this.records = records;
        this.fixed = fixed;
        this.difficulty = difficulty;
        this.factory = factory;
    }

    public Entity createExtra(World w, String name, int b, int id, int x, int y) {
        Entity e = factory.create(w, new RecordDef(-1, name, "main", 0, b, id, 0, x, y));
        if (e != null) {
            e.record = null;
        }
        return e;
    }

    public void update(World w) {
        if (!fixedMade) {
            fixedMade = true;
            for (RecordDef r : fixed) {
                Entity e = factory.create(w, r);
                if (e != null) {
                    e.record = r;
                    w.add(e);
                }
            }
        }
        for (RecordDef r : records) {
            boolean now = inBand(w, r);
            if ("CheckpointMarker".equals(r.name())) {
                if (now && !kept.contains(r)) {
                    updateMarker(w, r);
                }
                continue;
            }
            boolean entered = now && !inBand.contains(r);
            if (now) {
                inBand.add(r);
            } else {
                inBand.remove(r);
            }
            if (!entered || spawned.contains(r) || kept.contains(r) || !passesFilter(r)) {
                continue;
            }
            Entity e = factory.create(w, r);
            if (e != null) {
                e.record = r;
                spawned.add(r);
                w.add(e);
            }
        }
    }

    private void updateMarker(World w, RecordDef r) {
        int cp = r.id() & 0x7F;
        boolean reached = (r.id() & 0x80) != 0 ? w.player.y >= r.y() : w.player.x >= r.x();
        if (reached) {
            w.checkpoint = cp;
            kept.add(r);
        }
    }

    private boolean passesFilter(RecordDef r) {
        return r.kind() > 2 || r.kind() <= difficulty;
    }

    private static boolean inBand(World w, RecordDef r) {
        float cx = w.camera.x();
        float cy = w.camera.y();
        return r.x() >= cx - BAND && r.x() < cx + CameraController.VIEW_W + BAND
                && r.y() >= cy - BAND && r.y() < cy + CameraController.VIEW_H + BAND;
    }

    public void released(RecordDef r) {
        if (r != null) {
            spawned.remove(r);
        }
    }

    public void kept(RecordDef r) {
        if (r != null) {
            spawned.remove(r);
            kept.add(r);
        }
    }

    public void reset() {
        fixedMade = false;
        spawned.clear();
        kept.clear();
        inBand.clear();
    }
}
