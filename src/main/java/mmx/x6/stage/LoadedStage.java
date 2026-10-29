package mmx.x6.stage;

import mmx.x6.level.Level;
import mmx.x6.level.ValidationReport;

public record LoadedStage(StageDef stage, Level level, ValidationReport report) {
}
