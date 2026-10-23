package mmx.x6.game.player;

import static mmx.x6.game.player.PlayerConst.DASH_DECEL;
import static mmx.x6.game.player.PlayerConst.DASH_MIN;
import static mmx.x6.game.player.PlayerConst.DASH_START;
import static mmx.x6.game.player.PlayerConst.DASH_TIME;
import static mmx.x6.game.player.PlayerConst.FALCON_DASH_TIME;
import static mmx.x6.game.player.PlayerConst.FALL_GRAVITY;
import static mmx.x6.game.player.PlayerConst.FALL_MAX;
import static mmx.x6.game.player.PlayerConst.FEET;
import static mmx.x6.game.player.PlayerConst.JUMP_GRAVITY;
import static mmx.x6.game.player.PlayerConst.WALK;
import static mmx.x6.game.player.PlayerConst.WALK_START;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.input.Buttons;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;
import mmx.x6.physics.TileRules;

public final class Player extends Entity {

    private static final int LADDER_FROM_GROUND = 0;
    private static final int LADDER_TOP = 1;
    private static final int LADDER_DOWN_FROM_TOP = 2;
    private static final int LADDER_BOTTOM = 3;
    private static final int LADDER_LET_GO = 4;
    private static final int LADDER_UP = 10;
    private static final int LADDER_DOWN = 11;

    private final Body body;
    private PlayerStep step = PlayerStep.STAND;
    private int sub;
    private int timer;
    public float speedX;
    public float speedY;
    public float gravity;
    private int latch;
    private boolean airDashSpeed;
    private boolean airDashUsed;
    private boolean dashFlag;
    private int wallDelay;
    private int wallTimer;
    private int wallSide;
    private int ladderTopRow;
    private int barRow;
    private int hangCooldown;
    private boolean blockedR;
    private boolean blockedL;
    private boolean clingR;
    private boolean clingL;
    private int lastFlags;
    private int held;
    private int pressed;
    private World world;

    public Player(World w) {
        this.world = w;
        body = new Body(0, 0, PlayerConst.BOX_OFF_X, PlayerConst.BOX_OFF_Y, PlayerConst.BOX_HALF_W,
                PlayerConst.BOX_HALF_H);
        anim = new Animator(w.animSet("x-falcon"));
        hp = PlayerConst.MAX_HP;
        faceRight = true;
    }

    public void placeStanding(float px, float py) {
        body.x = px;
        body.y = py;
        x = px;
        y = py;
        prevX = px;
        prevY = py;
        speedX = speedY = gravity = 0;
        latch = 0;
        airDashSpeed = airDashUsed = dashFlag = false;
        hangCooldown = 0;
        body.hasBox = true;
        toStand();
    }

    public PlayerStep step() {
        return step;
    }

    public Body body() {
        return body;
    }

    public boolean grounded() {
        return latch == 0 && step != PlayerStep.LADDER && step != PlayerStep.BAR;
    }

    public boolean locked() {
        return world.locked() || step == PlayerStep.GATE_WALK || step == PlayerStep.EVENT_HOLD
                || step == PlayerStep.BEAM_IN;
    }

    @Override
    public String spriteSet() {
        return "x-falcon";
    }

    @Override
    public Rect hurtBox() {
        return body.box();
    }

    @Override
    public void update(World w) {
        world = w;
        prevX = x;
        prevY = y;
        boolean lock = locked();
        held = lock ? 0 : w.buttons.held();
        pressed = lock ? 0 : w.buttons.pressed();
        if (hangCooldown > 0) {
            hangCooldown--;
        }
        probeSides();
        switch (step) {
            case STAND -> updateStand();
            case WALK_START -> updateWalk(true);
            case WALK -> updateWalk(false);
            case DASH -> updateDash();
            case DASH_END -> updateDashEnd();
            case CROUCH -> updateCrouch();
            case LAND -> updateLand();
            case JUMP -> updateJump();
            case FALL -> updateFall();
            case WALL_GRAB -> updateWallGrab();
            case WALL_SLIDE -> updateWallSlide();
            case WALL_KICK -> updateWallKick();
            case FALCON_DASH -> updateFalconDash();
            case LADDER -> updateLadder();
            case BAR -> updateBar();
            case EVENT_HOLD -> {
                anim.tick();
                groundMove(0);
            }
            default -> anim.tick();
        }
        clampToCamera();
        x = body.x;
        y = body.y;
    }

    private void probeSides() {
        if (!body.hasBox) {
            blockedR = blockedL = clingR = clingL = false;
            return;
        }
        blockedR = world.mover.blocked(body, TerrainMover.RIGHT, world.solids());
        clingR = world.mover.lastProbeCling;
        blockedL = world.mover.blocked(body, TerrainMover.LEFT, world.solids());
        clingL = world.mover.lastProbeCling;
    }

    private void clampToCamera() {
        float lo = world.camera.minX() + 8;
        float hi = world.camera.maxX() + 312;
        if (body.x < lo) {
            body.x = lo;
        } else if (body.x > hi) {
            body.x = hi;
        }
    }

    private boolean isHeld(int bit) {
        return (held & bit) != 0;
    }

    private boolean isPressed(int bit) {
        return (pressed & bit) != 0;
    }

    private int dir() {
        if (isHeld(Buttons.RIGHT)) {
            return 1;
        }
        if (isHeld(Buttons.LEFT)) {
            return -1;
        }
        return 0;
    }

    private int face() {
        return faceRight ? 1 : -1;
    }

    private boolean blocked(int d) {
        return d > 0 ? blockedR : d < 0 && blockedL;
    }

    private int tileAt(float px, float py) {
        return world.grid.at(px, py);
    }

    private int standAnim() {
        return hp < PlayerConst.MAX_HP / 3 ? 6 : 5;
    }

    private void play(int a) {
        anim.play(a);
    }

    private boolean groundMove(float dx) {
        int f = world.mover.move(body, dx, 1, true, world.solids());
        lastFlags = f;
        if ((f & TerrainMover.FLOOR) == 0) {
            toFall();
            return false;
        }
        return true;
    }

    private int airMove() {
        int f = world.mover.move(body, speedX, -speedY, false, world.solids());
        lastFlags = f;
        speedY -= gravity;
        if (speedY < FALL_MAX) {
            speedY = FALL_MAX;
        }
        if ((f & TerrainMover.CEILING) != 0 && speedY > 0) {
            speedY = 0;
        }
        return f;
    }

    private int airControlMove() {
        int d = dir();
        if (d != 0) {
            faceRight = d > 0;
            speedX = blocked(d) ? 0 : d * (airDashSpeed ? DASH_MIN : WALK);
        } else {
            speedX = 0;
        }
        return airMove();
    }

    private boolean groundTries(boolean allowCrouch) {
        if (tryLadderFromGround()) {
            return true;
        }
        if (isPressed(Buttons.DASH)) {
            startDash();
            return true;
        }
        if (isPressed(Buttons.JUMP)) {
            startJump(false);
            return true;
        }
        if (allowCrouch && isHeld(Buttons.DOWN)) {
            toCrouch();
            return true;
        }
        return false;
    }

    private void toStand() {
        step = PlayerStep.STAND;
        sub = 0;
        speedX = speedY = 0;
        latch = 0;
        play(standAnim());
    }

    private void updateStand() {
        if (groundTries(true)) {
            return;
        }
        int d = dir();
        if (d != 0) {
            faceRight = d > 0;
            if (!blocked(d)) {
                step = PlayerStep.WALK_START;
                play(7);
                speedX = d * WALK_START;
                groundMove(speedX);
                return;
            }
        }
        anim.playIfNot(standAnim());
        anim.tick();
        groundMove(0);
    }

    private void updateWalk(boolean start) {
        if (groundTries(!start)) {
            return;
        }
        int d = dir();
        if (d == 0 || blocked(d)) {
            if (d != 0) {
                faceRight = d > 0;
            }
            toStand();
            groundMove(0);
            return;
        }
        faceRight = d > 0;
        speedX = d * (start ? WALK_START : WALK);
        anim.tick();
        if (!groundMove(speedX)) {
            return;
        }
        if (start && anim.atLastFrame()) {
            step = PlayerStep.WALK;
            play(8);
        }
    }

    private void startDash() {
        int d = dir();
        if (d != 0) {
            faceRight = d > 0;
        }
        airDashUsed = true;
        if (isPressed(Buttons.JUMP)) {
            startJump(true);
            return;
        }
        speedX = blocked(face()) ? 0 : DASH_START;
        step = PlayerStep.DASH;
        sub = 0;
        timer = DASH_TIME;
        play(16);
        groundMove(0);
    }

    private void updateDash() {
        anim.tick();
        if (sub == 0) {
            if (isPressed(Buttons.JUMP)) {
                startJump(false);
                return;
            }
            if (!anim.entered(0x80)) {
                groundMove(0);
                return;
            }
            world.sfx.play(1, 5);
            dashFlag = true;
            sub = 1;
        }
        if (isPressed(Buttons.JUMP)) {
            startJump(true);
            return;
        }
        if (blocked(face())) {
            toDashEnd();
            return;
        }
        if (!groundMove(face() * speedX)) {
            return;
        }
        speedX = Math.max(DASH_MIN, speedX + DASH_DECEL);
        timer--;
        int d = dir();
        if (timer <= 0 || (held & (Buttons.DASH | Buttons.LEFT | Buttons.RIGHT)) == 0
                || (d != 0 && (d > 0) != faceRight)) {
            toDashEnd();
        }
    }

    private void toDashEnd() {
        step = PlayerStep.DASH_END;
        play(17);
        world.sfx.stop(1, 5);
        speedX = DASH_MIN;
        dashFlag = false;
        airDashSpeed = false;
        airDashUsed = false;
    }

    private void updateDashEnd() {
        if (groundTries(true)) {
            return;
        }
        anim.tick();
        if (anim.entered(0x80)) {
            world.sfx.play(1, 6);
        }
        if (blocked(face())) {
            speedX = 0;
        }
        if (!groundMove(face() * speedX)) {
            return;
        }
        speedX = Math.max(0, speedX + DASH_DECEL);
        if (anim.atLastFrame()) {
            toStand();
        }
    }

    private void toCrouch() {
        step = PlayerStep.CROUCH;
        sub = 0;
        speedX = 0;
        play(81);
    }

    private void updateCrouch() {
        int d = dir();
        if (d != 0) {
            faceRight = d > 0;
        }
        if (isPressed(Buttons.DASH)) {
            startDash();
            return;
        }
        if (isPressed(Buttons.JUMP)) {
            startJump(false);
            return;
        }
        anim.tick();
        if (sub == 0 && !isHeld(Buttons.DOWN)) {
            play(82);
            sub = 2;
        } else if (sub == 2 && anim.atLastFrame()) {
            toStand();
        }
        groundMove(0);
    }

    private void toLand() {
        world.sfx.play(1, 2);
        airDashSpeed = false;
        airDashUsed = false;
        dashFlag = false;
        latch = 0;
        speedY = 0;
        speedX = 0;
        if (groundTries(true)) {
            return;
        }
        step = PlayerStep.LAND;
        play(12);
    }

    private void updateLand() {
        if (groundTries(true)) {
            return;
        }
        int d = dir();
        if (d != 0) {
            faceRight = d > 0;
            if (!blocked(d)) {
                step = PlayerStep.WALK_START;
                play(7);
                speedX = d * WALK_START;
                groundMove(speedX);
                return;
            }
        }
        anim.tick();
        if (anim.atLastFrame()) {
            toStand();
        }
        groundMove(0);
    }

    private void startJump(boolean dash) {
        speedY = PlayerConst.JUMP;
        gravity = JUMP_GRAVITY;
        speedX = 0;
        latch = 1;
        wallDelay = 10;
        step = PlayerStep.JUMP;
        sub = 0;
        play(10);
        if (dash || isHeld(Buttons.DASH)) {
            airDashSpeed = true;
            airDashUsed = true;
            dashFlag = true;
        }
        int f = airControlMove();
        if ((f & TerrainMover.FLOOR) != 0 && speedY <= 0) {
            toLand();
        }
    }

    private void updateJump() {
        if (!isHeld(Buttons.JUMP) || locked() || (lastFlags & TerrainMover.CEILING) != 0) {
            speedY = Math.min(speedY, 0);
        }
        anim.tick();
        if (anim.entered(0x80)) {
            world.sfx.play(1, 1);
        }
        if (tryLadderAir() || tryHang()) {
            return;
        }
        if (wallDelay == 0) {
            if (wallTest()) {
                return;
            }
        } else {
            wallDelay--;
        }
        if (tryAirDash()) {
            return;
        }
        int f = airControlMove();
        if ((f & TerrainMover.FLOOR) != 0) {
            toLand();
            return;
        }
        if (speedY <= 0) {
            play(11);
            latch = -1;
            step = PlayerStep.FALL;
        }
    }

    public void toFall() {
        step = PlayerStep.FALL;
        sub = 0;
        play(11);
        speedX = speedY = 0;
        gravity = FALL_GRAVITY;
        latch = -1;
        if (airDashSpeed) {
            airDashSpeed = false;
            dashFlag = false;
        }
    }

    private void updateFall() {
        anim.tick();
        if (tryLadderAir() || tryHang() || wallTest() || tryAirDash()) {
            return;
        }
        int f = airControlMove();
        if ((f & TerrainMover.FLOOR) != 0) {
            toLand();
        }
    }

    private boolean wallTest() {
        int d = dir();
        int side;
        if (d > 0 && blockedR) {
            side = 1;
        } else if (d < 0 && blockedL) {
            side = -1;
        } else if (blockedR) {
            side = 1;
        } else if (blockedL) {
            side = -1;
        } else {
            return false;
        }
        boolean cling = side > 0 ? clingR : clingL;
        if (!cling) {
            return false;
        }
        if (isPressed(Buttons.JUMP)) {
            wallKick(side);
            return true;
        }
        if (d == side) {
            wallGrab(side);
            return true;
        }
        return false;
    }

    private boolean wallStillThere() {
        return wallSide > 0 ? blockedR && clingR : blockedL && clingL;
    }

    private void wallGrab(int side) {
        step = PlayerStep.WALL_GRAB;
        play(13);
        world.sfx.play(1, 4);
        speedX = speedY = gravity = 0;
        airDashSpeed = airDashUsed = dashFlag = false;
        wallSide = side;
        faceRight = side > 0;
        wallTimer = 8;
        latch = -1;
    }

    private void updateWallGrab() {
        if (world.mover.onFloor(body, world.solids())) {
            toLand();
            return;
        }
        if (locked() || !wallStillThere()) {
            toFall();
            return;
        }
        if (isPressed(Buttons.JUMP)) {
            wallKick(wallSide);
            return;
        }
        anim.tick();
        if (anim.entered(0x80)) {
            faceRight = wallSide < 0;
        }
        if (--wallTimer <= 0) {
            step = PlayerStep.WALL_SLIDE;
            sub = 0;
            play(15);
            speedY = PlayerConst.WALL_SLIDE;
            gravity = 0;
            speedX = 0;
        }
    }

    private void updateWallSlide() {
        if (sub == 0) {
            if (world.mover.onFloor(body, world.solids())) {
                toLand();
                return;
            }
            if (locked() || !wallStillThere()) {
                toFall();
                return;
            }
            if (isPressed(Buttons.JUMP)) {
                wallKick(wallSide);
                return;
            }
            anim.tick();
            if (dir() == wallSide) {
                int f = world.mover.move(body, 0, -PlayerConst.WALL_SLIDE, false, world.solids());
                if ((f & TerrainMover.FLOOR) != 0) {
                    toLand();
                }
            } else {
                play(11);
                speedX = -wallSide * WALK;
                speedY = 0;
                gravity = FALL_GRAVITY;
                sub = 1;
                timer = 4;
            }
            return;
        }
        anim.tick();
        int f = airMove();
        if ((f & TerrainMover.FLOOR) != 0) {
            toLand();
            return;
        }
        if (--timer <= 0) {
            speedX = 0;
            step = PlayerStep.FALL;
            latch = -1;
        }
    }

    private void wallKick(int side) {
        step = PlayerStep.WALL_KICK;
        sub = 0;
        play(14);
        world.sfx.play(1, 3);
        latch = 1;
        speedY = PlayerConst.WALL_KICK_UP;
        gravity = JUMP_GRAVITY;
        boolean dash = isHeld(Buttons.DASH);
        speedX = -side * (dash ? DASH_MIN : WALK);
        faceRight = side > 0;
        airDashSpeed = dash;
        airDashUsed = dash;
        dashFlag = dash;
        wallTimer = 10;
        wallSide = side;
    }

    private void updateWallKick() {
        if (sub == 0) {
            anim.tick();
            int f = airMove();
            int side = speedX > 0 ? TerrainMover.RIGHT : TerrainMover.LEFT;
            if ((f & TerrainMover.CEILING) != 0 || (f & side) != 0) {
                toFall();
                return;
            }
            if (--wallTimer <= 0) {
                speedX = 0;
                sub = 1;
            }
            return;
        }
        if (!isHeld(Buttons.JUMP) || locked() || (lastFlags & TerrainMover.CEILING) != 0) {
            speedY = Math.min(speedY, 0);
        }
        anim.tick();
        if (tryLadderAir() || tryHang() || wallTest() || tryAirDash()) {
            return;
        }
        int f = airControlMove();
        if ((f & TerrainMover.FLOOR) != 0) {
            toLand();
            return;
        }
        if (speedY <= 0) {
            play(11);
            latch = -1;
            step = PlayerStep.FALL;
        }
    }

    private boolean tryAirDash() {
        if (locked() || airDashUsed) {
            return false;
        }
        boolean byJump = isPressed(Buttons.JUMP);
        boolean byDash = isPressed(Buttons.DASH);
        if (!byJump && !byDash) {
            return false;
        }
        int d = dir();
        if (d != 0) {
            faceRight = d > 0;
        }
        if (blocked(face())) {
            return false;
        }
        step = PlayerStep.FALCON_DASH;
        sub = 0;
        timer = FALCON_DASH_TIME;
        airDashUsed = true;
        airDashSpeed = true;
        speedY = 0;
        gravity = 0;
        speedX = byDash ? DASH_START : DASH_MIN;
        world.sfx.play(1, 5);
        play(169);
        return true;
    }

    private void updateFalconDash() {
        anim.tick();
        if (sub == 0) {
            if (!anim.atLastFrame()) {
                return;
            }
            sub = 1;
            play(171);
        }
        if (blocked(face())) {
            endFalconDash();
            return;
        }
        int f = world.mover.move(body, face() * speedX, 0, false, world.solids());
        lastFlags = f;
        if ((f & (faceRight ? TerrainMover.RIGHT : TerrainMover.LEFT)) != 0) {
            endFalconDash();
            return;
        }
        speedX = Math.max(DASH_MIN, speedX + DASH_DECEL);
        int d = dir();
        if (--timer <= 0 || (d != 0 && (d > 0) != faceRight)) {
            endFalconDash();
        }
    }

    private void endFalconDash() {
        world.sfx.stop(1, 5);
        airDashSpeed = false;
        toFall();
    }

    private boolean tryLadderFromGround() {
        if (isHeld(Buttons.UP) && TileRules.isLadder(tileAt(body.x, body.top()))) {
            body.x = (float) Math.floor(body.x / 16) * 16 + 8;
            play(27);
            latch = 1;
            step = PlayerStep.LADDER;
            sub = LADDER_FROM_GROUND;
            speedX = speedY = 0;
            return true;
        }
        if (isHeld(Buttons.DOWN) && TileRules.isLadderTop(tileAt(body.x, body.bottom()))) {
            body.x = (float) Math.floor(body.x / 16) * 16 + 8;
            play(29);
            latch = 1;
            step = PlayerStep.LADDER;
            sub = LADDER_DOWN_FROM_TOP;
            speedX = speedY = 0;
            return true;
        }
        return false;
    }

    private boolean tryLadderAir() {
        if (isHeld(Buttons.UP) && TileRules.isLadder(tileAt(body.x, body.y))) {
            body.x = (float) Math.floor(body.x / 16) * 16 + 8;
            play(31);
            latch = 1;
            step = PlayerStep.LADDER;
            sub = LADDER_UP;
            speedX = speedY = gravity = 0;
            airDashSpeed = airDashUsed = dashFlag = false;
            return true;
        }
        return false;
    }

    private void updateLadder() {
        switch (sub) {
            case LADDER_FROM_GROUND -> {
                anim.tick();
                body.y -= PlayerConst.LADDER_FIRST;
                if (anim.atLastFrame()) {
                    play(31);
                    sub = LADDER_UP;
                }
            }
            case LADDER_TOP -> {
                anim.tick();
                if (anim.entered(0x80)) {
                    body.y -= 16;
                }
                if (anim.atLastFrame()) {
                    body.y = ladderTopRow * 16 - FEET;
                    latch = 0;
                    airDashUsed = false;
                    toStand();
                }
            }
            case LADDER_DOWN_FROM_TOP -> {
                anim.tick();
                if (anim.entered(0x80)) {
                    body.y = (float) Math.floor(body.y / 16) * 16 + 32;
                }
                if (anim.atLastFrame()) {
                    body.y += 16;
                    anim.play(32, 3);
                    sub = LADDER_DOWN;
                }
            }
            case LADDER_BOTTOM -> {
                anim.tick();
                if (anim.atLastFrame()) {
                    latch = 0;
                    toStand();
                    groundMove(0);
                }
            }
            case LADDER_LET_GO -> {
                anim.tick();
                if (anim.atLastFrame()) {
                    toFall();
                }
            }
            default -> updateClimb();
        }
    }

    private void updateClimb() {
        int t = tileAt(body.x, body.y);
        if (isHeld(Buttons.DOWN) && world.mover.onFloor(body, world.solids())) {
            play(30);
            sub = LADDER_BOTTOM;
            return;
        }
        if (isHeld(Buttons.UP) && TileRules.isLadderTop(t)) {
            play(28);
            ladderTopRow = (int) Math.floor(body.y / 16);
            body.y = ladderTopRow * 16;
            sub = LADDER_TOP;
            return;
        }
        if ((!TileRules.isLadder(t) && !TileRules.isLadderTop(t)) || isPressed(Buttons.JUMP)) {
            play(30);
            sub = LADDER_LET_GO;
            return;
        }
        int d = dir();
        if (d != 0) {
            faceRight = d > 0;
        }
        if (isHeld(Buttons.UP)) {
            if (sub == LADDER_DOWN) {
                anim.play(31, anim.flag() & 0x3F);
                sub = LADDER_UP;
            }
            anim.tick();
            body.y -= PlayerConst.LADDER;
        } else if (isHeld(Buttons.DOWN)) {
            if (sub == LADDER_UP) {
                anim.play(32, anim.flag() & 0x3F);
                sub = LADDER_DOWN;
            }
            anim.tick();
            body.y += PlayerConst.LADDER;
        }
    }

    private boolean tryHang() {
        if (!isHeld(Buttons.UP) || hangCooldown > 0 || speedY > 0) {
            return false;
        }
        float top = body.top();
        if (!TileRules.isBar(tileAt(body.x, top))) {
            return false;
        }
        barRow = (int) Math.floor(top / 16);
        // TODO: the bar snap (0x800397F8) is not read; the head is put 4 px into the bar cell (docs/gaps.md, "Bar hang")
        body.y = barRow * 16 + 4 + PlayerConst.BOX_HALF_H - PlayerConst.BOX_OFF_Y;
        step = PlayerStep.BAR;
        speedX = speedY = gravity = 0;
        airDashSpeed = airDashUsed = dashFlag = false;
        play(163);
        world.sfx.play(0, 38);
        return true;
    }

    private void updateBar() {
        anim.tick();
        if (isPressed(Buttons.JUMP)) {
            hangCooldown = 4;
            if (isHeld(Buttons.DOWN)) {
                toFall();
            } else {
                startJump(false);
            }
            return;
        }
        int d = dir();
        float barY = barRow * 16 + 8;
        if (d != 0) {
            faceRight = d > 0;
            float nx = body.x + d * PlayerConst.BAR_MOVE;
            if (TileRules.isBar(tileAt(nx, barY)) && !blocked(d)) {
                body.x = nx;
            }
        }
        if (!TileRules.isBar(tileAt(body.x, barY))) {
            hangCooldown = 4;
            toFall();
        }
    }
}
