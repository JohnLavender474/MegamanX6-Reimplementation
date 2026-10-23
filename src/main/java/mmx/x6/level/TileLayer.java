package mmx.x6.level;

public record TileLayer(String name, int width, int height, int[] gids, int x6Mode, int x6OffsetX, int x6OffsetY) {

    public int gid(int cx, int cy) {
        if (cx < 0 || cy < 0 || cx >= width || cy >= height) {
            return 0;
        }
        return gids[cy * width + cx] & 0x1FFFFFFF;
    }

    public void setGid(int cx, int cy, int gid) {
        if (cx >= 0 && cy >= 0 && cx < width && cy < height) {
            gids[cy * width + cx] = gid;
        }
    }
}
