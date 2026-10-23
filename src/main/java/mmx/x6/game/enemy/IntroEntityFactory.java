package mmx.x6.game.enemy;

import mmx.x6.game.Entity;
import mmx.x6.game.EntityFactory;
import mmx.x6.game.World;
import mmx.x6.game.boss.D1000;
import mmx.x6.level.RecordDef;

public final class IntroEntityFactory implements EntityFactory {

    @Override
    public Entity create(World w, RecordDef r) {
        return switch (r.name()) {
            case "Junkroid" -> new Junkroid(w, r);
            case "HoverGunner" -> new HoverGunner(w, r);
            case "MetaWheelF" -> new MetaWheelF(w, r);
            case "MetaDridler" -> new MetaDridler(w, r);
            case "SabreBlock" -> new SabreBlock(w, r);
            case "BossGate" -> new BossGate(w, r);
            case "D1000" -> new D1000(w, r);
            default -> null;
        };
    }
}
