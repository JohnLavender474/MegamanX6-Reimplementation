package mmx.x6.level;

import java.util.List;

public record Level(int width, int height, TileLayer art2, TileLayer art1, TileLayer art0Main, TileLayer art0Front,
                    CollisionGrid collision, List<RecordDef> records, List<CameraZoneDef> cameraZones) {
}
