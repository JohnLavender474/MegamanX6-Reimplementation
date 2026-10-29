package mmx.x6;

import java.util.Locale;
import mmx.x6.frontend.Phase;
import mmx.x6.stage.StageDef;
import mmx.x6.stage.Stages;

public final class GameFlow {

    public enum Screen { FRONT_END, STAGE, SELECT }

    private static final String SELECT = "select";

    private final StageDef stage;
    private final String front;
    private final String levelPath;
    private Phase frontStart;

    public GameFlow(String stageKey, String front) {
        this(stageKey, front, null);
    }

    public GameFlow(String stageKey, String front, String levelPath) {
        this.levelPath = levelPath;
        this.stage = stageKey == null ? null : Stages.byKey(stageKey);
        this.front = front == null ? "ready" : front.toLowerCase(Locale.ROOT);
        this.frontStart = SELECT.equals(this.front) ? Phase.LOGO : Phase.valueOf(this.front.toUpperCase(Locale.ROOT));
    }

    public StageDef stage() {
        return stage;
    }

    public String levelPath() {
        return levelPath;
    }

    public Screen first() {
        if (stage != null || levelPath != null) {
            return Screen.STAGE;
        }
        return SELECT.equals(front) ? Screen.SELECT : Screen.FRONT_END;
    }

    public Phase frontStart() {
        return frontStart;
    }

    public Screen afterStageClear(StageDef s) {
        return Screen.SELECT;
    }

    public Screen afterStageEscape(StageDef s) {
        if (s == Stages.INTRO) {
            frontStart = Phase.TITLE;
            return Screen.FRONT_END;
        }
        return Screen.SELECT;
    }

    public boolean fallback(boolean frontEndExists) {
        return !frontEndExists;
    }
}
