package mmx.x6.level;

import java.util.List;
import java.util.Map;

public record Level(int width, int height, TileLayer art2, TileLayer art1, TileLayer art0Main, TileLayer art0Front,
                    CollisionGrid collision, List<RecordDef> records, List<CameraZoneDef> cameraZones,
                    List<RecordDef> fixedRecords, Map<String, String> properties, List<Checkpoint> checkpoints,
                    List<GoalDef> goals) {

    public Level(int width, int height, TileLayer art2, TileLayer art1, TileLayer art0Main, TileLayer art0Front,
                 CollisionGrid collision, List<RecordDef> records, List<CameraZoneDef> cameraZones,
                 List<RecordDef> fixedRecords) {
        this(width, height, art2, art1, art0Main, art0Front, collision, records, cameraZones, fixedRecords,
                Map.of(), List.of(), List.of());
    }

    public Level(int width, int height, TileLayer art2, TileLayer art1, TileLayer art0Main, TileLayer art0Front,
                 CollisionGrid collision, List<RecordDef> records, List<CameraZoneDef> cameraZones) {
        this(width, height, art2, art1, art0Main, art0Front, collision, records, cameraZones, List.of());
    }
}
