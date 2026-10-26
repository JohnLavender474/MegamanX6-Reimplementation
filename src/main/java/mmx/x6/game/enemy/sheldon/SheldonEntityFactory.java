package mmx.x6.game.enemy.sheldon;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.CommonEntityFactory;
import mmx.x6.level.RecordDef;

public final class SheldonEntityFactory extends CommonEntityFactory {

    @Override
    public Entity create(World w, RecordDef r) {
        return switch (r.name()) {
            case "LaserReflectorPuzzle" -> new LaserPuzzle(w, r);
            case "Guardian" -> new Guardian(w, r);
            case "MettaurD2" -> new MettaurD2(w, r);
            case "HiddenGlassFloor" -> new HiddenGlassFloor(w, r);
            case "ShieldSheldon" -> new ShieldSheldon(w, r);
            case "kind7_1", "SheldonBgSwitch" -> new SheldonBgSwitch(w, r);
            default -> super.create(w, r);
        };
    }
}
