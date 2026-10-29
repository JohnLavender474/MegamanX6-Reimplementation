package mmx.x6.stage;

import java.util.List;
import java.util.function.Supplier;
import mmx.x6.game.EntityFactory;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.Level;

public record StageDef(String key, String title, int number, String tmxFile,
                       List<Checkpoint> checkpoints, int music, int bossMusic, String bank2, String bank5,
                       String bossBank2, int nightmareEffect, Supplier<EntityFactory> factory) {

    public StageDef withMapData(Level level) {
        java.util.Map<String, String> p = level.properties();
        return new StageDef(key, title, number, tmxFile, level.checkpoints(), intOr(p, "x6_music"),
                intOr(p, "x6_boss_music"), text(p, "x6_bank2"), text(p, "x6_bank5"), text(p, "x6_boss_bank2"),
                intOr(p, "x6_nightmare_effect"), factory);
    }

    private static String text(java.util.Map<String, String> p, String key) {
        String v = p.get(key);
        return v == null ? null : v.trim();
    }

    private static int intOr(java.util.Map<String, String> p, String key) {
        String v = p.get(key);
        return v == null ? 0 : Integer.parseInt(v.trim());
    }
}
