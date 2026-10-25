package mmx.x6.game.enemy.yammark;

import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.enemy.CommonEntityFactory;
import mmx.x6.level.RecordDef;

public final class YammarkEntityFactory extends CommonEntityFactory {

    @Override
    public Entity create(World w, RecordDef r) {
        return switch (r.name()) {
            case "WormSpider" -> new WormSpider(w, r);
            case "KillerMoth" -> new KillerMoth(w, r);
            case "NightmareInsect" -> new NightmareInsect(w, r);
            case "Caterpirade" -> new Caterpirade(w, r);
            case "CommanderYammark" -> new CommanderYammark(w, r);
            default -> super.create(w, r);
        };
    }
}
