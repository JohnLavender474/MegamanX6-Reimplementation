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

    public Spawner(List<RecordDef> records, int difficulty, EntityFactory factory) {
        this.records = records;
        this.difficulty = difficulty;
        this.factory = factory;
    }

    public void update(World w) {
        for (RecordDef r : records) {
            if ("CheckpointMarker".equals(r.name())) {
                updateMarker(w, r);
                continue;
            }
            boolean now = inBand(w, r);
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
        if (reached && cp > w.checkpoint) {
            w.checkpoint = cp;
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
        spawned.clear();
        kept.clear();
        inBand.clear();
    }
}
