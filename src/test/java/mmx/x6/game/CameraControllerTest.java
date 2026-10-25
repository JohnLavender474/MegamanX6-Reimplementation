package mmx.x6.game;

import static org.junit.jupiter.api.Assertions.assertEquals;

import mmx.x6.level.Checkpoint;
import mmx.x6.level.IntroCheckpoints;
import org.junit.jupiter.api.Test;

class CameraControllerTest {

    private static Checkpoint at(int x, int y) {
        return new Checkpoint(0, 0, x, y, 0, 8000, 0, 2000, true, 1);
    }

    @Test
    void resetUsesTheCheckpoint() {
        CameraController cam = new CameraController();
        cam.reset(IntroCheckpoints.ALL.get(0));
        assertEquals(0, cam.x());
        assertEquals(944, cam.y());
        assertEquals(896, cam.minY());
        assertEquals(5120, cam.maxX());
    }

    @Test
    void followCentresAndClamps() {
        CameraController cam = new CameraController();
        cam.reset(IntroCheckpoints.ALL.get(0));
        cam.setFollow(true);
        cam.update(1000, 1100);
        assertEquals(840, cam.x());
        assertEquals(929, cam.y());
        cam.update(1000, 1200);
        assertEquals(944, cam.y());
    }

    @Test
    void noFollowKeepsPosition() {
        CameraController cam = new CameraController();
        cam.reset(IntroCheckpoints.ALL.get(0));
        cam.update(1000, 1100);
        assertEquals(0, cam.x());
    }

    @Test
    void limitsEaseTwoPerUpdate() {
        CameraController cam = new CameraController();
        cam.reset(IntroCheckpoints.ALL.get(0));
        cam.setTargets(1968, -1, -1, -1);
        cam.update(0, 0);
        assertEquals(2, cam.minX());
        cam.update(0, 0);
        assertEquals(4, cam.minX());
        assertEquals(1968, cam.targetMinX());
        assertEquals(5120, cam.targetMaxX());
    }

    @Test
    void playerAtLeftOfMinX() {
        CameraController cam = new CameraController();
        cam.reset(IntroCheckpoints.ALL.get(1));
        cam.setFollow(true);
        cam.update(2000, 1100);
        assertEquals(1968, cam.x());
    }

    @Test
    void layerFormulas() {
        CameraController cam = new CameraController();
        cam.reset(at(6064, 224));
        assertEquals(3032, cam.layer1X());
        assertEquals(672, cam.layer1Y());
        assertEquals(1516, cam.layer2X());
        assertEquals(696, cam.layer2Y());
        cam.reset(at(0, 944));
        assertEquals(0, cam.layer1X());
        assertEquals(1032, cam.layer1Y());
        assertEquals(876, cam.layer2Y());
        cam.reset(at(183, 944));
        assertEquals(91, cam.layer1X());
        assertEquals(45, cam.layer2X());
    }

    @Test
    void layerCapSixteen() {
        CameraController cam = new CameraController();
        cam.reset(at(0, 944));
        cam.setFollow(true);
        cam.update(560, 944 + 128);
        assertEquals(400, cam.x());
        assertEquals(16, cam.layer2X());
        assertEquals(16, cam.layer1X());
    }

    @Test
    void layerSmallChangeGoesAtOnce() {
        CameraController cam = new CameraController();
        cam.reset(at(0, 944));
        cam.setFollow(true);
        cam.update(160 + 30, 944 + 128);
        assertEquals(15, cam.layer1X());
        assertEquals(7, cam.layer2X());
    }

    @Test
    void layerCapFarJump() {
        CameraController cam = new CameraController();
        cam.reset(at(0, 944));
        cam.setFollow(true);
        cam.update(160 + 1400, 944 + 128);
        assertEquals(30, cam.layer2X());
        assertEquals(380, cam.layer1X());
    }

    @Test
    void halfXModeKeepsY() {
        CameraController cam = new CameraController();
        cam.configure(5, 0, 0, 4, 0, 0);
        cam.reset(new Checkpoint(0, 0, 5576, 224, 0, 9000, 0, 2000, true, 1, new mmx.x6.level.Point(320, 0), null));
        assertEquals(2788, cam.layer2X());
        assertEquals(224, cam.layer2Y());
        assertEquals(320, cam.layer1X());
        cam.setFollow(true);
        cam.update(5576 + 160 + 10, 224 + 128);
        assertEquals(2793, cam.layer2X());
        assertEquals(320, cam.layer1X());
    }

    @Test
    void layerTwoOffsetFromTheCheckpoint() {
        CameraController cam = new CameraController();
        cam.configure(5, 0, 0, 1, 0, 0);
        cam.reset(new Checkpoint(0, 0, 3200, 1056, 3200, 3504, 944, 1184, true, 1, null,
                new mmx.x6.level.Point(-672, 1136)));
        assertEquals(928, cam.layer2X());
        assertEquals(1664, cam.layer2Y());
    }
}
