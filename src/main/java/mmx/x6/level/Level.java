package mmx.x6.level;

import java.util.List;

public record Level(int width, int height, TileLayer art2, TileLayer art1, TileLayer art0Main, TileLayer art0Front,
                    CollisionGrid collision, List<RecordDef> records, List<CameraZoneDef> cameraZones,
                    List<RecordDef> fixedRecords) {

    public Level(int width, int height, TileLayer art2, TileLayer art1, TileLayer art0Main, TileLayer art0Front,
                 CollisionGrid collision, List<RecordDef> records, List<CameraZoneDef> cameraZones) {
        this(width, height, art2, art1, art0Main, art0Front, collision, records, cameraZones, List.of());
    }
}
