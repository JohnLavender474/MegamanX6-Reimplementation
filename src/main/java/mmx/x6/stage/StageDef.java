package mmx.x6.stage;

import java.util.List;
import java.util.function.Supplier;
import mmx.x6.game.EntityFactory;
import mmx.x6.level.Checkpoint;

public record StageDef(String key, String title, int number, String tmxFile,
                       List<Checkpoint> checkpoints, int music, int bossMusic, String bank2, String bank5,
                       String bossBank2, int nightmareEffect, Supplier<EntityFactory> factory) {
}
