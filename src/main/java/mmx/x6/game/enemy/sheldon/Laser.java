package mmx.x6.game.enemy.sheldon;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.physics.Rect;

public final class Laser extends Entity {

    public enum State { LIVE, END }

    static final String SET = "laser-reflector-puzzle-laser";
    static final int[] DX = {1, 1, 0, -1, -1, -1, 0, 1};
    static final int[] DY = {0, -1, -1, -1, 0, 1, 1, 1};
    private static final int LIFE = 182;
    private static final int END_UPDATES = 14;
    private static final int STEP = 16;
    private static final int FULL_CELLS = 14;
    private static final int[] HEAD = {210, 213, 222, 213, 210, 216, 219, 216};
    private static final int[] START = {225, 230, 245, 230, 225, 235, 240, 235};
    private static final int[] BODY_FULL = {39, 81, 207, 81, 39, 123, 165, 123};
    private static final int[] BODY_LAST = {0, 42, 168, 42, 0, 84, 126, 84};

    final LaserPuzzle parent;
    final int dir;
    final int shotId;
    final float startX;
    final float startY;
    private State state = State.LIVE;
    private int life = LIFE;
    private int moveCounter;
    private int startPhase;
    private int endTimer;
    int catchMark;

    Laser(World w, LaserPuzzle parent, int dir, int shotId, float x, float y) {
        this.parent = parent;
        this.dir = dir;
        this.shotId = shotId;
        this.x = x;
        this.y = y;
        this.startX = x;
        this.startY = y;
        this.contactDamage = 1;
        this.faceRight = dir == 0 || dir == 1 || dir == 7;
        this.anim = new Animator(w.animSet(SET));
        anim.play(HEAD[dir]);
    }

    public State state() {
        return state;
    }

    public int dir() {
        return dir;
    }

    boolean live() {
        return state == State.LIVE && !dead;
    }

    Rect headBox() {
        return new Rect(x - 8, y - 8, 16, 16);
    }

    void pin(float px, float py) {
        x = px;
        y = py;
        catchMark = 2;
    }

    @Override
    public void update(World w) {
        if (state == State.END) {
            if (++endTimer >= END_UPDATES) {
                dead = true;
            }
            drawPieces(w);
            return;
        }
        boolean pinned = catchMark == 2;
        catchMark = 0;
        if (parent.dead || parent.link() == 0 || parent.removed()) {
            end();
            return;
        }
        if (--life <= 0) {
            parent.timedOut();
            end();
            return;
        }
        int v = w.grid.at(x, y);
        boolean open = v == 0 || v >= 37 && v <= 47;
        if (open && moveCounter == 0 && !pinned) {
            x = Math.max(0, x + DX[dir] * STEP);
            y = Math.max(0, y + DY[dir] * STEP);
        }
        moveCounter = (moveCounter + 1) % 3;
        startPhase = (startPhase + 1) % 5;
        drawPieces(w);
    }

    private void end() {
        state = State.END;
        endTimer = 0;
        contactDamage = 0;
    }

    private void drawPieces(World w) {
        if (state == State.END && (w.frame & 1) == 0) {
            return;
        }
        int phase = moveCounter;
        anim.play(HEAD[dir] + phase);
        w.add(new Piece(w, START[dir] + startPhase, startX, startY, faceRight, 5));
        float len = Math.max(Math.abs(x - startX), Math.abs(y - startY));
        int cells = (int) Math.ceil(len / STEP);
        float px = startX;
        float py = startY;
        while (cells > 0) {
            int n = Math.min(cells, FULL_CELLS);
            int sprite = n == FULL_CELLS ? BODY_FULL[dir] + phase : BODY_LAST[dir] + phase + 3 * (n - 1);
            w.add(new Piece(w, sprite, px + DX[dir] * 112, py + DY[dir] * 112, faceRight, 6));
            px += DX[dir] * STEP * n;
            py += DY[dir] * STEP * n;
            cells -= n;
        }
    }

    @Override
    public Rect contactBox() {
        return state == State.LIVE ? headBox() : null;
    }

    @Override
    public boolean visible() {
        return state == State.LIVE || (endTimer & 1) == 1;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public String spriteSet() {
        return SET;
    }

    @Override
    public int drawPriority() {
        return 4;
    }

    static final class Piece extends Entity {

        private final int priority;

        Piece(World w, int sprite, float x, float y, boolean faceRight, int priority) {
            this.x = x;
            this.y = y;
            this.faceRight = faceRight;
            this.priority = priority;
            this.anim = new Animator(w.animSet(SET));
            anim.play(sprite);
        }

        @Override
        public void update(World w) {
            dead = true;
        }

        @Override
        public boolean clearedByGate() {
            return false;
        }

        @Override
        public String spriteSet() {
            return SET;
        }

        @Override
        public int drawPriority() {
            return priority;
        }
    }
}
