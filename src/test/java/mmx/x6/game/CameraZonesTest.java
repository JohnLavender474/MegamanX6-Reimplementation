package mmx.x6.game;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import mmx.x6.level.CameraZoneDef;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.IntroCheckpoints;
import org.junit.jupiter.api.Test;

class CameraZonesTest {

    private static final CameraZoneDef ZONE0 =
            new CameraZoneDef(2, 0, 2272, 2303, 1040, 1135, -1, -1, 944, -1, false);
    private static final CameraZoneDef ZONE1 =
            new CameraZoneDef(3, 1, 2128, 2159, 1040, 1135, 1968, -1, -1, -1, true);
    private static final CameraZoneDef ZONE3 =
            new CameraZoneDef(5, 3, 4000, 4031, 752, 815, -1, -1, -1, 704, false);

    private static CameraController cam(int x) {
        CameraController c = new CameraController();
        c.reset(new Checkpoint(0, 0, x, 944, 0, 5120, 0, 944, true, 1));
        return c;
    }

    @Test
    void zone0SetsMinY() {
        CameraController c = cam(2000);
        CameraZones z = new CameraZones(List.of(ZONE0));
        z.update(2280, 1100, c);
        assertEquals(944, c.targetMinY());
        assertEquals(0, c.targetMinX());
        assertEquals(5120, c.targetMaxX());
    }

    @Test
    void outsideTheBoxDoesNothing() {
        CameraController c = cam(2000);
        CameraZones z = new CameraZones(List.of(ZONE0));
        z.update(2304, 1100, c);
        assertEquals(0, c.targetMinY());
    }

    @Test
    void zone1IsOneUse() {
        CameraController c = cam(2000);
        CameraZones z = new CameraZones(List.of(ZONE1));
        z.update(2130, 1100, c);
        assertEquals(1968, c.targetMinX());
        c.setTargets(0, -1, -1, -1);
        z.update(2130, 1100, c);
        assertEquals(0, c.targetMinX());
    }

    @Test
    void zonesResetBringZone1Back() {
        CameraController c = cam(2000);
        CameraZones z = new CameraZones(List.of(ZONE1));
        z.update(2130, 1100, c);
        c.setTargets(0, -1, -1, -1);
        z.reset();
        z.update(2130, 1100, c);
        assertEquals(1968, c.targetMinX());
    }

    @Test
    void zone3OnlyWhileCameraAtMost3840() {
        CameraController far = cam(3900);
        CameraZones z = new CameraZones(List.of(ZONE3));
        z.update(4010, 800, far);
        assertEquals(944, far.targetMaxY());
        CameraController near = cam(3800);
        z.update(4010, 800, near);
        assertEquals(704, near.targetMaxY());
    }

    @Test
    void realZonesLoadIntoTheController() {
        CameraController c = new CameraController();
        c.reset(IntroCheckpoints.ALL.get(0));
        CameraZones z = new CameraZones(List.of(ZONE0, ZONE1));
        z.update(2140, 1100, c);
        assertEquals(1968, c.targetMinX());
    }
}
