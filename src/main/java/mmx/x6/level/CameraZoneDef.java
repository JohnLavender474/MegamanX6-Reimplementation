package mmx.x6.level;

public record CameraZoneDef(int record, int zoneId, int x0, int x1, int y0, int y1,
                            int minX, int maxX, int minY, int maxY, boolean textureSwap) {

    public boolean contains(float px, float py) {
        return px >= x0 && px <= x1 && py >= y0 && py <= y1;
    }
}
