package mmx.x6.game.enemy.wolfang;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.boss.Maverick;
import mmx.x6.game.enemy.Projectile;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class BlizzardWolfang extends Maverick {

    public enum Step { ENTRY, DASH, JUMP, WALL_DIVE, SLIDE, WALL_BOUNCE, BALLS, CEILING_JUMP, CEILING_BALLS, CEILING_SLIDE }

    static final String SET = "blizzard-wolfang";
    private static final Step[][] TABLE_1 = {
            {Step.DASH, Step.DASH, Step.JUMP}, {Step.DASH, Step.DASH, Step.WALL_DIVE}, {Step.BALLS}};
    private static final Step[][] TABLE_2 = {
            {Step.BALLS}, {Step.SLIDE, Step.SLIDE, Step.BALLS}, {Step.CEILING_JUMP, Step.CEILING_SLIDE},
            {Step.CEILING_JUMP, Step.CEILING_BALLS, Step.CEILING_SLIDE}, {Step.SLIDE, Step.SLIDE, Step.SLIDE}};
    private static final float SIN20 = 0.342f;
    private static final float COS20 = 0.940f;
    private static final float FAST = 6;
    private static final int FADE = 124;

    private final float roomX;
    private final float roomY;
    private Step step = Step.ENTRY;
    private Step[] row;
    private int rowIndex;
    private boolean secondTable;
    private int sub;
    private int timer;
    private int count;
    private float vx;
    private float vy;
    private float gravity;
    private float targetX;
    private float targetY;
    private int trail;

    public BlizzardWolfang(World w, RecordDef r) {
        super(w, r, SET, 48, DamageTables.Table.WOLFANG);
        roomX = w.camera.targetMinX();
        roomY = w.camera.targetMinY();
        x = r.x();
        y = r.y();
        contactDamage = 4;
        anim.play(0);
    }

    public Step step() {
        return step;
    }

    private float floorY() {
        return roomY + 208 - 32;
    }

    private float ceilingY() {
        return roomY + 16 + 32;
    }

    private float leftX() {
        return roomX + 16 + 20;
    }

    private float rightX() {
        return roomX + 368 - 20;
    }

    private void facePlayer(World w) {
        faceRight = w.player.x > x;
    }

    private void voice(World w) {
        w.sfx.play(2, 34 + w.rng.nextInt(3));
    }

    @Override
    protected void fight(World w) {
        anim.tick();
        timer++;
        switch (step) {
            case ENTRY -> entry(w);
            case DASH -> dash(w);
            case JUMP -> jump(w);
            case WALL_DIVE -> wallDive(w);
            case SLIDE -> slide(w);
            case WALL_BOUNCE -> wallBounce(w);
            case BALLS -> balls(w);
            case CEILING_JUMP -> ceilingJump(w);
            case CEILING_BALLS -> ceilingBalls(w);
            case CEILING_SLIDE -> ceilingSlide(w);
        }
    }

    private void entry(World w) {
        switch (sub) {
            case 0 -> {
                if (startWarning(w)) {
                    sub = 1;
                }
            }
            case 1 -> {
                if (warningOver(w)) {
                    sub = 2;
                    timer = 0;
                }
            }
            case 2 -> {
                w.bgDark = Math.min(1, timer / (float) FADE);
                if (timer >= FADE) {
                    x = roomX + 240;
                    y = roomY + 144;
                    vy = 0;
                    sub = 3;
                }
            }
            case 3 -> {
                vy += 0.0458f;
                y += vy;
                if (y >= floorY()) {
                    x += 6;
                    y = floorY() - 24;
                    anim.play(7);
                    shown = true;
                    sub = 4;
                    timer = 0;
                }
            }
            case 4 -> {
                if (timer >= 180) {
                    // TODO: Wolfang's text box before the fight is not shown (docs/gaps.md, "Maverick text")
                    startMusic(w);
                    x -= 6;
                    y += 24;
                    anim.play(0);
                    sub = 5;
                    timer = 0;
                }
            }
            case 5 -> {
                w.bgDark = Math.max(0, 1 - timer / (float) FADE);
                if (timer >= FADE) {
                    w.sfx.play(2, 16);
                    anim.play(8);
                    sub = 6;
                }
            }
            case 6 -> {
                if (anim.entered(1)) {
                    sub = 7;
                }
            }
            default -> {
                if (fillHp(w)) {
                    nextMove(w);
                }
            }
        }
    }

    private void nextMove(World w) {
        contactDamage = 4;
        sub = 0;
        timer = 0;
        vx = vy = gravity = 0;
        if (!secondTable && hp <= maxHp / 2 && row != null) {
            secondTable = true;
            newRow(w);
        } else if (row == null || rowIndex >= row.length) {
            newRow(w);
        }
        step = row[rowIndex++];
    }

    private void newRow(World w) {
        int r = w.rng.nextInt(16);
        if (!secondTable) {
            row = r < 5 ? TABLE_1[0] : r < 10 ? TABLE_1[1] : TABLE_1[2];
        } else {
            row = r < 2 ? TABLE_2[0] : r < 6 ? TABLE_2[1] : r < 9 ? TABLE_2[2] : r < 13 ? TABLE_2[3] : TABLE_2[4];
        }
        rowIndex = 0;
    }

    private boolean atWallAhead() {
        return faceRight ? x >= rightX() : x <= leftX();
    }

    private void clampRoom() {
        x = Math.max(leftX(), Math.min(rightX(), x));
        y = Math.max(ceilingY(), Math.min(floorY(), y));
    }

    private boolean playerAhead(World w) {
        return faceRight ? w.player.x > x : w.player.x < x;
    }

    private void dash(World w) {
        switch (sub) {
            case 0 -> {
                facePlayer(w);
                anim.play(2);
                sub = 1;
            }
            case 1 -> {
                if (anim.entered(1)) {
                    w.sfx.play(2, 18);
                    vx = faceRight ? 4 : -4;
                    sub = 2;
                }
            }
            case 2 -> {
                x += vx;
                if (anim.entered(2)) {
                    w.sfx.play(2, 18);
                }
                if (!playerAhead(w) || atWallAhead()) {
                    anim.play(23);
                    vx = faceRight ? 2 : -2;
                    sub = 3;
                    timer = 0;
                }
                clampRoom();
            }
            default -> {
                x += vx;
                clampRoom();
                if (timer >= 16) {
                    nextMove(w);
                }
            }
        }
    }

    private void jump(World w) {
        switch (sub) {
            case 0 -> {
                facePlayer(w);
                anim.play(4);
                sub = 1;
                timer = 0;
            }
            case 1 -> {
                if (timer >= 60) {
                    w.sfx.play(2, 17);
                    voice(w);
                    anim.play(5);
                    vy = -8.889f;
                    gravity = 0.494f;
                    vx = faceRight ? 4.444f : -4.444f;
                    sub = 2;
                    timer = 0;
                }
            }
            case 2 -> {
                if (air(w)) {
                    w.sfx.play(2, 18);
                    // TODO: the landing's Rumble(8) is not done (docs/gaps.md, "Screen shake")
                    anim.play(4);
                    sub = 3;
                    timer = 0;
                } else if (timer == 12) {
                    anim.play(6);
                } else if (timer == 23) {
                    anim.play(5);
                }
            }
            default -> {
                if (timer >= 13) {
                    nextMove(w);
                }
            }
        }
    }

    private boolean air(World w) {
        vy += gravity;
        x += vx;
        y += vy;
        if (x <= leftX() || x >= rightX()) {
            vx = 0;
        }
        if (y >= floorY() && vy > 0) {
            clampRoom();
            vx = vy = gravity = 0;
            return true;
        }
        clampRoom();
        return false;
    }

    private boolean flyTo(float tx, float ty) {
        float dx = tx - x;
        float dy = ty - y;
        double d = Math.hypot(dx, dy);
        if (d <= FAST) {
            x = tx;
            y = ty;
            return true;
        }
        x += (float) (dx / d * FAST);
        y += (float) (dy / d * FAST);
        return false;
    }

    private void wallDive(World w) {
        switch (sub) {
            case 0 -> {
                anim.play(9);
                sub = 1;
                timer = 0;
            }
            case 1 -> {
                if (timer >= 60) {
                    w.sfx.play(2, 17);
                    voice(w);
                    anim.play(10);
                    boolean toRight = x < roomX + 192;
                    faceRight = toRight;
                    targetX = toRight ? rightX() : leftX();
                    targetY = roomY + 128;
                    sub = 2;
                }
            }
            case 2 -> {
                if (flyTo(targetX, targetY) || atWallAhead()) {
                    clampRoom();
                    w.sfx.play(2, 18);
                    anim.play(11);
                    faceRight = !faceRight;
                    targetX = w.player.x;
                    targetY = w.player.y;
                    sub = 3;
                    timer = 0;
                }
            }
            case 3 -> {
                if (timer >= 90) {
                    w.sfx.play(2, 19);
                    anim.play(10);
                    float dx = targetX - x;
                    float dy = targetY - y;
                    double d = Math.max(1, Math.hypot(dx, dy));
                    vx = (float) (dx / d * FAST);
                    vy = (float) (dy / d * FAST);
                    faceRight = vx > 0;
                    sub = 4;
                }
            }
            case 4 -> {
                x += vx;
                y += vy;
                if (y >= floorY() || y <= ceilingY() || atWallAhead()) {
                    clampRoom();
                    vx = vy = 0;
                    gravity = 0.0916f;
                    sub = 5;
                }
            }
            case 5 -> {
                vy += gravity;
                y += vy;
                if (y >= floorY()) {
                    clampRoom();
                    w.sfx.play(2, 18);
                    anim.play(9);
                    sub = 6;
                    timer = 0;
                }
            }
            default -> {
                if (timer >= 20) {
                    nextMove(w);
                }
            }
        }
    }

    private void slide(World w) {
        switch (sub) {
            case 0 -> {
                facePlayer(w);
                anim.play(16);
                sub = 1;
            }
            case 1 -> {
                if (anim.entered(1)) {
                    w.sfx.play(2, 37 + w.rng.nextInt(2));
                    vx = faceRight ? FAST : -FAST;
                    contactDamage = 6;
                    // TODO: the floor ice spikes (var 16) exist only at boss level 2 and 3; the level is 0 here, so they are not made (docs/gaps.md, "Blizzard Wolfang")
                    sub = 2;
                    trail = 0;
                } else {
                    w.sfx.play(2, 19);
                }
            }
            case 2 -> {
                x += vx;
                trail(w);
                PlayerStep ps = w.player.step();
                if (ps == PlayerStep.WALL_GRAB || ps == PlayerStep.WALL_SLIDE) {
                    clampRoom();
                    step = Step.WALL_BOUNCE;
                    sub = 0;
                    return;
                }
                if (!playerAhead(w) || atWallAhead()) {
                    anim.play(23);
                    sub = 3;
                    timer = 0;
                }
                clampRoom();
            }
            default -> {
                x += vx;
                clampRoom();
                if (timer >= 8) {
                    nextMove(w);
                }
            }
        }
    }

    private void trail(World w) {
        if (++trail % 8 == 0) {
            w.add(new Trail(w, this));
        }
    }

    private void bounceAway() {
        faceRight = !faceRight;
        vx = (faceRight ? 1 : -1) * FAST * COS20;
        vy = FAST * SIN20;
    }

    private void wallBounce(World w) {
        switch (sub) {
            case 0 -> {
                facePlayer(w);
                anim.play(18);
                sub = 1;
            }
            case 1 -> {
                if (anim.entered(1)) {
                    w.sfx.play(2, 17);
                    voice(w);
                    vx = (faceRight ? 1 : -1) * FAST * COS20;
                    vy = -FAST * SIN20;
                    sub = 2;
                }
            }
            case 2 -> {
                x += vx;
                y += vy;
                clampRoom();
                if (atWallAhead()) {
                    hitWall(w);
                }
            }
            case 3 -> {
                if (timer >= 30) {
                    w.sfx.play(2, 19);
                    anim.play(10);
                    bounceAway();
                    sub = 4;
                }
            }
            case 4 -> zigzag(w);
            default -> {
                if (timer >= 30) {
                    nextMove(w);
                }
            }
        }
    }

    private void hitWall(World w) {
        w.sfx.play(2, 18);
        anim.play(11);
        // TODO: the wall runners (visual 20 var 32, 33) make wall spikes only at boss level 2 and 3; not made at level 0 (docs/gaps.md, "Blizzard Wolfang")
        sub = 3;
        timer = 0;
    }

    private void zigzag(World w) {
        x += vx;
        y += vy;
        if (y >= floorY()) {
            clampRoom();
            w.sfx.play(2, 18);
            anim.play(9);
            sub = 5;
            timer = 0;
            return;
        }
        if (atWallAhead()) {
            clampRoom();
            hitWall(w);
        }
        clampRoom();
    }

    private void balls(World w) {
        switch (sub) {
            case 0 -> {
                count = 3;
                sub = 1;
            }
            case 1 -> {
                facePlayer(w);
                anim.play(13);
                sub = 2;
            }
            case 2 -> {
                if (anim.enteredFlag(1)) {
                    w.sfx.play(2, 21);
                    w.add(new MouthIce(w, this, faceRight ? 24 : -24, -8, false));
                }
                if (anim.enteredFlag(2)) {
                    w.sfx.play(2, 24);
                    w.add(new IceBall(w, this, x + (faceRight ? 24 : -24), y - 8, false));
                }
                if (anim.enteredFlag(3)) {
                    if (--count > 0) {
                        sub = 3;
                        timer = 0;
                    } else {
                        anim.play(0);
                        sub = 4;
                        timer = 0;
                    }
                }
            }
            case 3 -> {
                if (timer >= 40) {
                    sub = 1;
                }
            }
            default -> {
                if (timer >= 60) {
                    nextMove(w);
                }
            }
        }
    }

    private void ceilingJump(World w) {
        switch (sub) {
            case 0 -> {
                facePlayer(w);
                anim.play(19);
                sub = 1;
            }
            case 1 -> {
                if (anim.entered(1)) {
                    w.sfx.play(2, 17);
                    voice(w);
                    vy = -FAST;
                    sub = 2;
                }
            }
            case 2 -> {
                y += vy;
                if (y <= ceilingY()) {
                    clampRoom();
                    w.sfx.play(2, 18);
                    facePlayer(w);
                    anim.play(20);
                    sub = 3;
                }
            }
            default -> {
                if (anim.entered(1)) {
                    nextMove(w);
                }
            }
        }
    }

    private void ceilingBalls(World w) {
        switch (sub) {
            case 0 -> {
                count = 1;
                sub = 1;
            }
            case 1 -> {
                w.sfx.play(2, 39);
                facePlayer(w);
                anim.play(15);
                sub = 2;
            }
            case 2 -> {
                if (anim.enteredFlag(1)) {
                    w.sfx.play(2, 21);
                    w.add(new MouthIce(w, this, faceRight ? 24 : -24, 8, true));
                }
                if (anim.enteredFlag(2)) {
                    w.add(new IceBall(w, this, x + (faceRight ? 24 : -24), y + 8, true));
                }
                if (anim.enteredFlag(4) || anim.finished()) {
                    count--;
                    sub = count > 0 ? 3 : 4;
                    timer = 0;
                }
            }
            case 3 -> {
                if (timer >= 40) {
                    sub = 1;
                }
            }
            default -> {
                if (timer >= 40) {
                    nextMove(w);
                }
            }
        }
    }

    private void ceilingSlide(World w) {
        switch (sub) {
            case 0 -> {
                faceRight = x < roomX + 192;
                anim.play(17);
                sub = 1;
            }
            case 1 -> {
                if (anim.entered(1)) {
                    w.sfx.play(2, 19);
                    vx = faceRight ? FAST : -FAST;
                    sub = 2;
                    trail = 0;
                }
            }
            case 2 -> {
                x += vx;
                trail(w);
                if (atWallAhead()) {
                    clampRoom();
                    w.sfx.play(2, 18);
                    anim.play(11);
                    timer = 8 - trail % 8;
                    sub = 3;
                }
            }
            case 3 -> {
                if (--timer <= 0) {
                    w.sfx.play(2, 19);
                    anim.play(10);
                    bounceAway();
                    sub = 4;
                }
            }
            case 4 -> {
                x += vx;
                y += vy;
                if (y >= floorY()) {
                    clampRoom();
                    w.sfx.play(2, 18);
                    anim.play(9);
                    sub = 6;
                    timer = 0;
                } else if (atWallAhead()) {
                    clampRoom();
                    w.sfx.play(2, 18);
                    anim.play(11);
                    sub = 5;
                    timer = 0;
                }
                clampRoom();
            }
            case 5 -> {
                if (timer >= 30) {
                    w.sfx.play(2, 19);
                    anim.play(10);
                    bounceAway();
                    sub = 4;
                }
            }
            default -> {
                if (timer >= 30) {
                    nextMove(w);
                }
            }
        }
    }

    @Override
    protected void onDeathStart(World w) {
        anim.play(25);
        w.sfx.play(2, 32);
        w.bgDark = 0;
    }

    boolean over() {
        return dying() || dead;
    }

    private boolean wallPose() {
        int s = anim.sprite();
        return s >= 7 && s <= 12 || s >= 26 && s <= 29 || s == 91 || s == 96 || s == 97;
    }

    @Override
    public Rect hurtBox() {
        if (step == Step.ENTRY || dying()) {
            return null;
        }
        return wallPose() ? new Rect(x - 16, y - 26, 32, 42) : new Rect(x - 16, y - 16, 32, 42);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    private static final class Trail extends Entity {

        private int age;

        Trail(World w, BlizzardWolfang boss) {
            this.x = boss.x;
            this.y = boss.y;
            this.faceRight = boss.faceRight;
            this.anim = new Animator(w.animSet(SET));
            anim.play(boss.anim().anim(), boss.anim().frameIndex());
        }

        @Override
        public void update(World w) {
            // TODO: the trail image uses flag 0x10 and the next bank's palette (semi-transparent [guess]); it is drawn as a plain copy (docs/gaps.md, "Blizzard Wolfang")
            if (++age >= 24) {
                dead = true;
            }
        }

        @Override
        public boolean visible() {
            return (age & 1) == 0;
        }

        @Override
        public String spriteSet() {
            return SET;
        }

        @Override
        public int drawPriority() {
            return 6;
        }
    }

    private static final class MouthIce extends Entity {

        private final BlizzardWolfang boss;
        private final boolean ceiling;
        private final float dx;
        private final float dy;

        MouthIce(World w, BlizzardWolfang boss, float dx, float dy, boolean ceiling) {
            this.boss = boss;
            this.dx = dx;
            this.dy = dy;
            this.ceiling = ceiling;
            this.anim = new Animator(w.animSet(SET));
            anim.play(26);
            follow();
        }

        private void follow() {
            x = boss.x + dx;
            y = boss.y + dy;
            faceRight = boss.faceRight;
        }

        @Override
        public void update(World w) {
            if (boss.over()) {
                dead = true;
                return;
            }
            follow();
            anim.tick();
            boolean end = ceiling ? boss.anim().enteredFlag(4) || boss.step != Step.CEILING_BALLS : anim.entered(1);
            if (end) {
                dead = true;
            }
        }

        @Override
        public String spriteSet() {
            return SET;
        }

        @Override
        public int drawPriority() {
            return 4;
        }
    }

    private static final class IceBall extends Projectile {

        private final BlizzardWolfang boss;
        private final boolean aimed;
        private boolean thrown;

        IceBall(World w, BlizzardWolfang boss, float x, float y, boolean aimed) {
            super(w, SET, 27, x, y, 0, 0, 4, 10, 10);
            this.boss = boss;
            this.aimed = aimed;
            hittable(2, DamageTables.Table.MINOR);
            explodes();
            endsOnPlayer();
            throughTerrain();
            hidden(0);
            margin(80);
            priority(4);
            facing(boss.faceRight);
        }

        @Override
        public void update(World w) {
            if (boss.over()) {
                dead = true;
                return;
            }
            if (!thrown) {
                anim.tick();
                if (anim.entered(1)) {
                    w.sfx.play(2, 22);
                    if (aimed) {
                        float[] v = Projectile.aim(x, y, w.player.x, w.player.y, 7);
                        vx = v[0];
                        vy = v[1];
                    } else {
                        vx = (w.player.x - x) / 46;
                        vy = -8.364f;
                        gravity(0.3636f, 12);
                    }
                    thrown = true;
                }
                return;
            }
            super.update(w);
            if (!dead && TileRules.isObjectSolid(w.grid.at(x, y))) {
                w.sfx.play(2, 23);
                Debris.spawn(w, SET, new int[] {28, 29, 30}, x, y);
                w.add(new Explosion(w, x, y));
                dead = true;
            }
        }

        @Override
        public Rect contactBox() {
            return thrown ? super.contactBox() : null;
        }

        @Override
        public Rect hurtBox() {
            return thrown ? new Rect(x - 6, y - 6, 12, 12) : null;
        }
    }
}
