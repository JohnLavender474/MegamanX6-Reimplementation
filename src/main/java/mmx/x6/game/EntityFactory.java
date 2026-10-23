package mmx.x6.game;

import mmx.x6.level.RecordDef;

public interface EntityFactory {

    Entity create(World w, RecordDef r);
}
