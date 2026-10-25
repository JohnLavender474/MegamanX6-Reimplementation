package mmx.x6.game.enemy.turtloid;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.CommonEntityFactory;
import mmx.x6.level.RecordDef;

public final class TurtloidEntityFactory extends CommonEntityFactory {

    @Override
    public Entity create(World w, RecordDef r) {
        return switch (r.name()) {
            case "Monbando" -> new Monbando(w, r);
            case "SeaAttackerST" -> new SeaAttackerST(w, r);
            case "BattonBoneB81" -> new BattonBoneB81(w, r);
            case "WeatherAnalyze" -> new WeatherAnalyze(w, r);
            case "RocketPlatform" -> new RocketPlatform(w, r);
            case "HealthCapsule" -> new HealthCapsule(w, r);
            case "logic_11" -> new AcidRain(w, r);
            case "RainyTurtloid" -> new RainyTurtloid(w, r);
            default -> super.create(w, r);
        };
    }
}
