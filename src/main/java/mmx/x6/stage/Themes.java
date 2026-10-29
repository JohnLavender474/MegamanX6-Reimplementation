package mmx.x6.stage;

import java.util.function.Supplier;
import mmx.x6.game.EntityFactory;
import mmx.x6.game.enemy.IntroEntityFactory;
import mmx.x6.level.Catalog;

public final class Themes {

    private Themes() {
    }

    public static Supplier<EntityFactory> factory(String theme) {
        if ("intro".equals(theme)) {
            return IntroEntityFactory::new;
        }
        return null;
    }

    public static String tilesPng(Catalog catalog, String theme) {
        Catalog.Theme t = catalog.theme(theme);
        return t == null ? null : t.tilesPng();
    }
}
