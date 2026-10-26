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
    void followMovesAtMostEightPerUpdate() {
        CameraController cam = new CameraController();
        cam.reset(at(0, 944));
        cam.setFollow(true);
        cam.update(1000, 944 + 150);
        assertEquals(8, cam.x());
        cam.update(1000, 944 + 150);
        assertEquals(16, cam.x());
        cam.update(16 + 163, 944 + 150);
        assertEquals(19, cam.x());
    }

    @Test
    void followKeepsTheYDeadZone() {
        CameraController cam = new CameraController();
        cam.reset(at(0, 944));
        cam.setFollow(true);
        cam.update(160, 944 + 150);
        assertEquals(944, cam.y());
        cam.update(160, 944 + 175);
        assertEquals(948, cam.y());
        cam.update(160, 948 + 90);
        assertEquals(942, cam.y());
        cam.update(160, 942 + 300);
        assertEquals(950, cam.y());
    }

    @Test
    void followStopsAtTheLimits() {
        CameraController cam = new CameraController();
        cam.reset(IntroCheckpoints.ALL.get(0));
        cam.setFollow(true);
        for (int i = 0; i < 40; i++) {
            cam.update(1000, 1300);
        }
        assertEquals(944, cam.y());
    }

    @Test
    void openingLimitDoesNotSnapTheCamera() {
        CameraController cam = new CameraController();
        cam.reset(new Checkpoint(0, 0, 0, 944, 0, 0, 944, 944, true, 1));
        cam.setFollow(true);
        cam.update(400, 944 + 150);
        cam.setTargets(-1, 2000, -1, -1);
        cam.update(400, 944 + 150);
        assertEquals(2000, cam.maxX());
        assertEquals(0, cam.x());
        cam.update(400, 944 + 150);
        assertEquals(8, cam.x());
        cam.update(400, 944 + 150);
        assertEquals(16, cam.x());
    }

    @Test
    void noFollowKeepsPosition() {
        CameraController cam = new CameraController();
        cam.reset(IntroCheckpoints.ALL.get(0));
        cam.update(1000, 1100);
        assertEquals(0, cam.x());
    }

    @Test
    void closingLimitMovesAtTheEaseSpeed() {
        CameraController cam = new CameraController();
        cam.reset(IntroCheckpoints.ALL.get(0));
        cam.setFollow(true);
        cam.setTargets(1968, -1, -1, -1);
        cam.update(160, 944 + 150);
        assertEquals(4, cam.minX());
        cam.update(164, 944 + 150);
        assertEquals(4, cam.x());
        assertEquals(8, cam.minX());
        cam.easeSpeed = 2;
        cam.update(168, 944 + 150);
        assertEquals(10, cam.minX());
        assertEquals(1968, cam.targetMinX());
        assertEquals(5120, cam.targetMaxX());
    }

    @Test
    void openingLimitsGoToTheTargetAsX6Does() {
        assertEquals(3000, CameraController.easeMaxX(320, 3000, 0, 160, 0, 2));
        assertEquals(402, CameraController.easeMaxX(320, 398, 380, 158, 20, 2));
        assertEquals(0, CameraController.easeMinX(500, 0, 600, 150, 0, 2));
        assertEquals(0, CameraController.easeMinX(500, 0, 600, 170, 0, 2));
        assertEquals(2000, CameraController.easeMaxY(944, 2000, 944, 175, 0, 2));
        assertEquals(946, CameraController.easeMaxY(944, 2000, 944, 171, 0, 2));
        assertEquals(54, CameraController.easeMinY(944, 0, 900, 150, 0, 2));
        assertEquals(890, CameraController.easeMinY(944, 0, 900, 90, -8, 2));
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
        assertEquals(16, CameraController.cap(0, 200));
        assertEquals(16, CameraController.cap(0, 100));
    }

    @Test
    void layerSmallChangeGoesAtOnce() {
        assertEquals(15, CameraController.cap(0, 15));
        CameraController cam = new CameraController();
        cam.reset(at(0, 944));
        cam.setFollow(true);
        cam.update(160 + 6, 944 + 128);
        assertEquals(3, cam.layer1X());
        assertEquals(1, cam.layer2X());
    }

    @Test
    void layerCapFarJump() {
        assertEquals(30, CameraController.cap(0, 350));
        assertEquals(380, CameraController.cap(0, 700));
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
        assertEquals(2792, cam.layer2X());
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
