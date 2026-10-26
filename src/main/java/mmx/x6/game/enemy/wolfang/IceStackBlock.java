package mmx.x6.game.enemy.wolfang;

import java.util.ArrayList;
import java.util.List;
import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.game.fx.AnimFx;
import mmx.x6.game.fx.Debris;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class IceStackBlock extends Entity implements Solid {

    public enum State { WAIT, MAKE, FALL, LANDED, FLASH, BROKEN }

    static final String SET = "ice-stack-block";
    static final int GENERATOR = 0;
    static final int ROW_BLOCK = 10;
    static final int GAP_FILLER = 11;
    static final int PIT_BLOCK = 12;
    private static final int[] BLOCK_SRC = {6016, 1568, 3, 4};
    private static final int[] PLAIN_SRC = {6016, 1584, 3, 3};

    private final int kind;
    private final IceStackBlock generator;
    private final IceRow row;
    private final int column;
    private State state;
    private int timer;
    private int counter;
    private int count;
    private final int[] columns = new int[6];
    private boolean plain;

    public IceStackBlock(World w, RecordDef r) {
        this(w, r.id(), r.x(), r.y(), null, null, -1);
        this.record = r;
    }

    private IceStackBlock(World w, int kind, float x, float y, IceStackBlock generator, IceRow row, int column) {
        this.kind = kind;
        this.x = x;
        this.y = y;
        this.generator = generator;
        this.row = row;
        this.column = column;
        this.hp = 32;
        this.damageTable = DamageTables.Table.DEFLECT;
        this.anim = new Animator(w.animSet(SET));
        anim.play(0);
        switch (kind) {
            case GENERATOR -> state = State.WAIT;
            case ROW_BLOCK -> {
                contactDamage = 16;
                state = State.FALL;
            }
            case GAP_FILLER -> {
                contactDamage = 127;
                state = State.LANDED;
                plain = TileRules.isObjectSolid(w.grid.at(x, y - 26));
                anim.play(plain ? 1 : 0);
            }
            default -> {
                contactDamage = 6;
                state = State.FALL;
            }
        }
    }

    static IceStackBlock rowBlock(World w, IceRow row, float x, float y) {
        return new IceStackBlock(w, ROW_BLOCK, x, y, null, row, -1);
    }

    static IceStackBlock gapFiller(World w, float x, float y) {
        return new IceStackBlock(w, GAP_FILLER, x, y, null, null, -1);
    }

    public int kind() {
        return kind;
    }

    public State state() {
        return state;
    }

    boolean frozen() {
        return state == State.LANDED || state == State.FLASH;
    }

    boolean broken() {
        return state == State.BROKEN || dead && state != State.FLASH;
    }

    private boolean first() {
        return x < 5312;
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (state) {
            case WAIT -> waitForPlayer(w);
            case MAKE -> makeBlocks(w);
            case FALL -> {
                if (kind == ROW_BLOCK) {
                    y = row.y;
                    if (row.broken()) {
                        breakUp(w, 2);
                    } else if (row.landed()) {
                        land(w, false);
                    }
                } else {
                    fallPit(w);
                }
                crush(w);
            }
            case LANDED -> {
                timer++;
                if (kind == GAP_FILLER && timer == 1) {
                    if (TileRules.isObjectSolid(w.grid.at(x, y))) {
                        dead = true;
                        return;
                    }
                    if (overlapsPlayer(w)) {
                        w.player.squeeze(w, 127);
                    }
                }
                if (timer >= (kind == PIT_BLOCK ? 32 : 2)) {
                    anim.play(plain ? 3 : 2);
                    state = State.FLASH;
                }
            }
            case FLASH -> {
                if (anim.finished()) {
                    int[] src = plain ? PLAIN_SRC : BLOCK_SRC;
                    w.copyTiles(src[0], src[1], src[2], src[3], (int) x - 24, (int) y - (plain ? 24 : 32));
                    dead = true;
                }
            }
            case BROKEN -> dead = true;
        }
    }

    private void waitForPlayer(World w) {
        float px = w.player.x;
        float py = w.player.y;
        boolean in = first() ? px >= 5137 && py >= 1473 : px >= 5345 && py >= 1489;
        if (in) {
            int lock = first() ? 4976 : 5328;
            w.camera.setTargets(lock, lock, -1, -1);
            state = State.MAKE;
        }
    }

    private void makeBlocks(World w) {
        int total = first() ? 18 : 24;
        int depth = first() ? 3 : 4;
        int c = w.rng.nextInt(6);
        int period = 128;
        if (counter % period == 0 && count < total) {
            for (int i = 0; i < 5 && columns[c] >= depth; i++) {
                c = (c + 1) % 6;
            }
            float bx = (first() ? 5016 : 5368) + 48 * c;
            IceStackBlock b = new IceStackBlock(w, PIT_BLOCK, bx, 1248, this, null, c);
            w.add(b);
            if (w.rng.nextBoolean()) {
                w.add(Wolfloid.carried(w, b, w.rng.nextBoolean()));
            }
            columns[c]++;
            count++;
        }
        counter = (counter + 1) & 0xFF;
        float px = w.player.x;
        int lock = first() ? 4976 : 5328;
        if (first() ? px >= 5281 : px >= 5633) {
            if (w.camera.targetMinX() == lock) {
                int next = first() ? 5328 : 5712;
                w.camera.setTargets(next, next, -1, -1);
            }
            dead = true;
            w.spawner.kept(record);
        }
    }

    private void freed() {
        if (generator != null) {
            generator.count--;
            generator.columns[column]--;
        }
    }

    private void fallPit(World w) {
        float oldY = y;
        y += 2;
        if (y < 1296) {
            return;
        }
        if (blockUnder(w)) {
            y = oldY;
            return;
        }
        if (w.grid.at(x - 32, y) != 0 && w.grid.at(x + 32, y) != 0) {
            contactDamage = 127;
        }
        for (float px = x - 23; px <= x + 23; px += 23) {
            if (TileRules.isObjectSolid(w.grid.at(px, y + 24))) {
                land(w, true);
                return;
            }
        }
    }

    private boolean blockUnder(World w) {
        for (Entity e : w.entities) {
            if (e != this && e instanceof IceStackBlock b && !b.dead && b.kind != GENERATOR && b.state != State.BROKEN) {
                Rect r = new Rect(b.x - 24, b.y - 24, 48, 48);
                if (r.contains(x - 24, y + 23) || r.contains(x + 23, y + 23)) {
                    return true;
                }
            }
        }
        return false;
    }

    void land(World w, boolean sound) {
        y = (float) Math.floor((y + 24) / 16f) * 16 - 24;
        if (sound) {
            w.sfx.play(2, 17);
        }
        w.sfx.play(2, 21);
        w.add(new AnimFx(w, SET, 4, x, y, false));
        state = State.LANDED;
        timer = 0;
    }

    private boolean overlapsPlayer(World w) {
        Rect p = w.player.hurtBox();
        return p != null && p.overlaps(new Rect(x - 24, y - 24, 48, 48));
    }

    private void crush(World w) {
        Rect p = w.player.hurtBox();
        if (p == null || !w.player.grounded() || w.player.squeezeTimer > 0 || p.y() < y) {
            return;
        }
        if (overlapsPlayer(w)) {
            w.player.squeeze(w, contactDamage);
            if (kind == ROW_BLOCK) {
                row.breakUp();
            } else {
                breakUp(w, 3);
            }
        }
    }

    private void breakUp(World w, int pieces) {
        int[] anims = {5, 6, 7};
        int[] used = new int[pieces];
        System.arraycopy(anims, 0, used, 0, pieces);
        Debris.spawn(w, SET, used, x, y);
        freed();
        state = State.BROKEN;
    }

    @Override
    public Rect solidRect() {
        if (kind == GENERATOR || state == State.BROKEN || kind == GAP_FILLER && timer < 1) {
            return null;
        }
        return new Rect(x - 24, y - 24, 48, 48);
    }

    @Override
    public boolean carries() {
        return true;
    }

    @Override
    public Rect hurtBox() {
        return solidRect();
    }

    @Override
    public boolean visible() {
        return kind != GENERATOR && state != State.BROKEN;
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
        return 6;
    }

    static final class IceRow extends Entity {

        private static final int[][] OFFSETS = {
                {96, 48, 0, -48}, {96, 48, 0, -96}, {96, 48, -48, -96}, {96, 0, -48, -96}, {48, 0, -48, -96}};
        private static final int[] GAP = {96, 48, 0, -48, -96};

        private final IceShaftController controller;
        private final int kind;
        private final List<IceStackBlock> blocks = new ArrayList<>();
        private boolean landed;
        private boolean broken;

        IceRow(World w, IceShaftController controller, int kind, float x, float y) {
            this.controller = controller;
            this.kind = kind;
            this.x = x;
            this.y = y;
            for (int off : OFFSETS[kind - 1]) {
                IceStackBlock b = rowBlock(w, this, x - off, y);
                blocks.add(b);
                w.add(b);
            }
        }

        boolean landed() {
            return landed;
        }

        boolean broken() {
            return broken;
        }

        void breakUp() {
            if (!broken) {
                broken = true;
                controller.rowGone();
            }
        }

        @Override
        public void update(World w) {
            if (broken || landed) {
                dead = true;
                return;
            }
            y += 1;
            if (y < 369) {
                return;
            }
            for (float px = x - 119; px <= x + 119; px += 16) {
                if (TileRules.isObjectSolid(w.grid.at(px, y + 24))) {
                    y = (float) Math.floor((y + 24) / 16f) * 16 - 24;
                    w.sfx.play(2, 17);
                    // TODO: the row landing's Quake2(16, 3, 1) screen shake is not done (docs/gaps.md, "Screen shake")
                    landed = true;
                    w.add(gapFiller(w, x + GAP[kind - 1], y));
                    if (y >= 977 && y <= 1022 || y >= 737 && y <= 782) {
                        w.add(gapFiller(w, x - 144, y));
                    } else if (y >= 833 && y <= 878 || y >= 689 && y <= 734) {
                        w.add(gapFiller(w, x + 144, y));
                    }
                    return;
                }
            }
        }

        @Override
        public boolean visible() {
            return false;
        }

        @Override
        public boolean clearedByGate() {
            return false;
        }
    }
}
