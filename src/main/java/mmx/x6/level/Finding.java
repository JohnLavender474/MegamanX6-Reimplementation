package mmx.x6.level;

public record Finding(Result result, String code, String message) {

    public enum Result { REJECT, REPAIR, WARN, LOCK }
}
