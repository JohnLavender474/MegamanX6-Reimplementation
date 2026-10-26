package mmx.x6.game.player;

public final class PlayerConst {

    public static final float WALK_START = 0.5f;
    public static final float WALK = 2.0f;
    public static final float JUMP = 5.8125f;
    public static final float FALL_MAX = -6.5f;
    public static final float FALL_GRAVITY = 0.2578125f;
    public static final float JUMP_GRAVITY = 0.2578125f;
    public static final float DASH_START = 6.5f;
    public static final float DASH_MIN = 4.125f;
    public static final float DASH_DECEL = -0.1875f;
    public static final int DASH_TIME = 30;
    public static final float WALL_SLIDE = -1.4375f;
    public static final float WALL_KICK_UP = 5.625f;
    public static final float LADDER_FIRST = 1.0f;
    public static final float LADDER = 1.5f;
    public static final float BAR_MOVE = 2.0f;
    public static final float ROPE_MOVE = 1.5f;
    public static final float HANG_DASH = 4.125f;
    public static final int FALCON_DASH_TIME = 18;
    public static final int MAX_HP = 32;
    public static final float BOX_OFF_X = 0;
    public static final float BOX_OFF_Y = 2;
    public static final float BOX_HALF_W = 11;
    public static final float BOX_HALF_H = 19;
    public static final float FEET = BOX_OFF_Y + BOX_HALF_H;

    private PlayerConst() {
    }
}
