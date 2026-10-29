package mmx.x6.level;

public record RecordDef(int index, String name, String type, int kind, int b, int id, int a, int x, int y, int tmxId,
                        int goal, int openedBy) {

    public RecordDef(int index, String name, String type, int kind, int b, int id, int a, int x, int y) {
        this(index, name, type, kind, b, id, a, x, y, 0, 0, 0);
    }
}
