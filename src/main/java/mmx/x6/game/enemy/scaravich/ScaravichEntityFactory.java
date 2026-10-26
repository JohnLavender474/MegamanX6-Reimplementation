package mmx.x6.game.enemy.scaravich;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.CommonEntityFactory;
import mmx.x6.game.enemy.heatnix.GreyBlock;
import mmx.x6.level.RecordDef;

public final class ScaravichEntityFactory extends CommonEntityFactory {

    @Override
    public Entity create(World w, RecordDef r) {
        return switch (r.name()) {
            case "TeleporterPole" -> new TeleporterPole(w, r);
            case "SectionGoal" -> new SectionGoal(w, r);
            case "FallingRock" -> FallingRock.spawn(w, r);
            case "RockBlock" -> RockBlock.spawn(w, r);
            case "GreyBlock" -> new GreyBlock(w, r);
            case "GroundScaravich" -> new GroundScaravich(w, r);
            default -> super.create(w, r);
        };
    }
}
