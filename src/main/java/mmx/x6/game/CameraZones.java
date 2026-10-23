package mmx.x6.game;

import java.util.HashSet;
import java.util.List;
import java.util.Set;
import mmx.x6.level.CameraZoneDef;

public final class CameraZones {

    private static final int ZONE3_MAX_CAMERA_X = 3840;

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
            if (z.zoneId() == 3 && cam.x() > ZONE3_MAX_CAMERA_X) {
                continue;
            }
            cam.setTargets(z.minX(), z.maxX(), z.minY(), z.maxY());
            cam.easeSpeed = 2;
            if (z.textureSwap()) {
                used.add(z);
            }
        }
    }
}
