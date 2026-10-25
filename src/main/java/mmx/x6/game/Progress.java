package mmx.x6.game;

import java.util.HashMap;
import java.util.Map;

public final class Progress {

    public static final int REPLOID_NEW = 0;
    public static final int REPLOID_SAVED = 2;
    public static final int REPLOID_DEAD = 3;
    public static final int REPLOID_LOST = 4;
    public static final int MAX_LIVES = 9;

    public int lives = 2;
    public int souls;
    public int armorParts;
    private final Map<Integer, Integer> reploids = new HashMap<>();

    public int reploid(int id) {
        return reploids.getOrDefault(id, REPLOID_NEW);
    }

    public void setReploid(int id, int status) {
        reploids.put(id, status);
    }
}
