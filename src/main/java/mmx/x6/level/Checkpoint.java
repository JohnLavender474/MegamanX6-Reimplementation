package mmx.x6.level;

public record Checkpoint(int playerX, int playerY, int camX, int camY, int minX, int maxX, int minY, int maxY,
                         boolean faceRight, int beamThru) {
}
