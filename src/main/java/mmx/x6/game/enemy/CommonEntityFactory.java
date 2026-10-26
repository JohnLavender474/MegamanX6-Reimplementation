package mmx.x6.game.enemy;

import mmx.x6.game.Entity;
import mmx.x6.game.EntityFactory;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;

public class CommonEntityFactory implements EntityFactory {

    @Override
    public Entity create(World w, RecordDef r) {
        return switch (r.name()) {
            case "BossGate" -> new BossGate(w, r);
            case "RescueReploid" -> new RescueReploid(w, r);
            case "NightmareVirus" -> new NightmareVirus(w, r);
            case "LightCapsule" -> new LightCapsule(w, r);
            case "TeleporterCrystal" -> new TeleporterCrystal(w, r);
            case "NightmareShadow", "main_26" -> new NightmareShadow(w, r);
            case "IceSlide", "logic_7" -> new IceSlide(w, r);
            case "NightmareBug" -> new NightmareBug(w, r);
            default -> null;
        };
    }
}
