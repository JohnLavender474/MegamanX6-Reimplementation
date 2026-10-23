package mmx.x6.game;

import java.util.HashSet;
import java.util.List;
import java.util.Set;
import mmx.x6.level.CameraZoneDef;

public final class CameraZones {

    private final List<CameraZoneDef> zones;
    private final Set<CameraZoneDef> used = new HashSet<>();

    public CameraZones(List<CameraZoneDef> zones) {
        this.zones = zones;
    }

    public void reset() {
        used.clear();
    }

    public void update(float px, float py, CameraController cam) {
        for (CameraZoneDef z : zones) {
            if (used.contains(z) || !z.contains(px, py)) {
                continue;
            }
            // TODO: X6 deletes zone 3 once the camera x passes 3840; with the centred follow that is always true in the trigger, so the check is not made (docs/gaps.md, "Camera zone 3")
            cam.setTargets(z.minX(), z.maxX(), z.minY(), z.maxY());
            cam.easeSpeed = 2;
            if (z.textureSwap()) {
                used.add(z);
            }
        }
    }
}
