package mmx.x6.level;

public record Checkpoint(int playerX, int playerY, int camX, int camY, int minX, int maxX, int minY, int maxY,
                         boolean faceRight, int beamThru, Point layer1, Point layer2Offset) {

    public Checkpoint(int playerX, int playerY, int camX, int camY, int minX, int maxX, int minY, int maxY,
                      boolean faceRight, int beamThru) {
        this(playerX, playerY, camX, camY, minX, maxX, minY, maxY, faceRight, beamThru, null, null);
    }
}
