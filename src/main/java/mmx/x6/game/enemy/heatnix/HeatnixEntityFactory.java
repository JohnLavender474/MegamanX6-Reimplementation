package mmx.x6.game.enemy.heatnix;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.CommonEntityFactory;
import mmx.x6.level.RecordDef;

public final class HeatnixEntityFactory extends CommonEntityFactory {

    @Override
    public Entity create(World w, RecordDef r) {
        return switch (r.name()) {
            case "FlameJet" -> new FlameJet(w, r);
            case "GreyBlock" -> new GreyBlock(w, r);
            case "NightmareSnake" -> new NightmareSnake(w, r);
            case "BlazeHeatnix" -> new BlazeHeatnix(w, r);
            default -> super.create(w, r);
        };
    }
}
