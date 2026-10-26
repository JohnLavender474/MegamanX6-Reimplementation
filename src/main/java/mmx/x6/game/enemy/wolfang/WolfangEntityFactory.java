package mmx.x6.game.enemy.wolfang;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.CommonEntityFactory;
import mmx.x6.level.RecordDef;

public final class WolfangEntityFactory extends CommonEntityFactory {

    @Override
    public Entity create(World w, RecordDef r) {
        return switch (r.name()) {
            case "Wolfloid" -> new Wolfloid(w, r);
            case "IceStackBlock" -> new IceStackBlock(w, r);
            case "IceShaftController" -> new IceShaftController(w, r);
            case "IceFallController" -> new IceBlock.Controller(w, r);
            case "SnowScenery" -> new SnowScenery(w, r);
            case "BlizzardWolfang" -> new BlizzardWolfang(w, r);
            default -> super.create(w, r);
        };
    }
}
