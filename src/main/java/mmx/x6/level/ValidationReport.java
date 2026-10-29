package mmx.x6.level;

import com.badlogic.gdx.utils.XmlReader;
import java.util.List;

public record ValidationReport(List<Finding> findings, XmlReader.Element map) {

    public enum Status { CERTIFIED, PLAYABLE, REJECTED }

    public Status status() {
        boolean playable = false;
        for (Finding f : findings) {
            if (f.result() == Finding.Result.REJECT) {
                return Status.REJECTED;
            }
            if (f.result() == Finding.Result.WARN || f.result() == Finding.Result.LOCK) {
                playable = true;
            }
        }
        return playable ? Status.PLAYABLE : Status.CERTIFIED;
    }
}
