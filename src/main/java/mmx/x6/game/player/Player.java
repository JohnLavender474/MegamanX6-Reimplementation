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

import java.util.Set;
import mmx.x6.anim.Animator;
import mmx.x6.game.CustomChanges;
import mmx.x6.game.Damage;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Orb;
import mmx.x6.input.Buttons;
import mmx.x6.level.Checkpoint;
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

    private static final int SABRE_FLOOR = 0;
    private static final int SABRE_AIR = 1;
    private static final int SABRE_WALL = 2;

    private static final Set<Integer> SHOOTING_VERSIONS = Set.of(7, 8, 10, 11, 12, 13, 14, 15, 16, 17);
    private static final Set<Integer> STAND_SHOTS = Set.of(88, 90, 91);

    private static final int HURT_IFRAMES = 75;
    private static final int HURT_REACTION_END = 60;
    private static final float HURT_SPEED = 2.5f;
    private static final float HURT_DECEL = 0.1875f;
    private static final int SQUEEZE_UPDATES = 75;
    private static final float BEAM_SPEED = 8;

    private final Body body;
    private PlayerStep step = PlayerStep.STAND;
    private int sub;
    private int timer;
    public float speedX;
    public int squeezeTimer;
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
    private float split;
    private int ropeDash;
    private int ropeDashDir;
    private int ropeSound;
    private boolean blockedR;
    private boolean blockedL;
    private boolean clingR;
    private boolean clingL;
    private int lastFlags;
    private int held;
    private int pressed;
    private int released;
    private World world;

    private int shotWait;
    private int shotPose;
    private int chargeTimer;
    private int chargeLevel;
    private int iframes;
    private boolean pendingHit;
    private boolean pendingDeath;
    private boolean hitFromRight;
    private int deathTimer;
    private boolean hidden;
    private boolean spikeTouched;
    private int beamThru;
    private float eventWalk;
    private int sabreKind;
    private boolean sabreCancel;
    private SabreSlash slash;

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
        shotPose = shotWait = 0;
        chargeTimer = chargeLevel = 0;
        toStand();
    }

    public void respawn(Checkpoint cp) {
        hp = PlayerConst.MAX_HP;
        iframes = 0;
        pendingHit = pendingDeath = false;
        hidden = false;
        deathTimer = 0;
        endSlash();
        faceRight = cp.faceRight();
        placeStanding(cp.playerX(), cp.playerY());
        step = PlayerStep.BEAM_IN;
        sub = 0;
        beamThru = cp.beamThru();
        body.hasBox = false;
        latch = -1;
        anim.play(1);
        world.sfx.play(1, 13);
        world.camera.setFollow(false);
    }

    public void startGate() {
        endSlash();
        dropCharge();
        shotPose = 0;
        step = PlayerStep.GATE_WALK;
        faceRight = true;
        body.hasBox = false;
        speedX = speedY = 0;
        anim.play(8);
    }

    public void startEvent(boolean faceRight, float walk) {
        endSlash();
        dropCharge();
        shotPose = 0;
        this.faceRight = faceRight;
        eventWalk = walk;
        speedX = 0;
        if (step != PlayerStep.DEAD) {
            step = PlayerStep.EVENT_HOLD;
            anim.play(walk > 0 ? 8 : standAnim());
        }
    }

    public void setEventWalk(float walk) {
        eventWalk = walk;
        anim.play(walk > 0 ? 8 : standAnim());
    }

    public void startTeleport() {
        endSlash();
        dropCharge();
        shotPose = 0;
        split = 0;
        speedX = speedY = gravity = 0;
        body.hasBox = false;
        step = PlayerStep.TELEPORT;
    }

    public void setSplit(float split) {
        this.split = split;
    }

    public void endTeleport() {
        split = 0;
        x = body.x;
        body.hasBox = true;
        if (step == PlayerStep.TELEPORT) {
            toStand();
        }
    }

    public void endEvent() {
        eventWalk = 0;
        if (step == PlayerStep.EVENT_HOLD) {
            toStand();
        }
    }

    public void carry(float dx, float dy) {
        if (dx != 0) {
            world.mover.move(body, dx, 0, true, world.solids());
        }
        body.y += dy;
        x = body.x;
        y = body.y;
    }

    public int slide(float dx) {
        int f = world.mover.move(body, dx, 1, true, world.solids());
        x = body.x;
        y = body.y;
        return f;
    }

    public void shove(float dx, float dy) {
        body.x += dx;
        body.y += dy;
        x = body.x;
        y = body.y;
    }

    public void squeeze(World w, int damage) {
        if (dying()) {
            return;
        }
        squeezeTimer = SQUEEZE_UPDATES;
        hp -= damage;
        if (hp <= 0) {
            kill();
        }
    }

    public void pushGate(float dx) {
        body.x += dx;
        x = body.x;
    }

    public void endGate() {
        body.hasBox = true;
        latch = 0;
        toStand();
    }

    private void updateBeamIn() {
        anim.tick();
        if (sub == 0) {
            if (beamThru > 0) {
                beamThru--;
                body.y += BEAM_SPEED;
                if (beamThru == 0) {
                    body.hasBox = true;
                }
                return;
            }
            body.hasBox = true;
            int f = world.mover.move(body, 0, BEAM_SPEED, false, world.solids());
            if ((f & TerrainMover.FLOOR) != 0) {
                anim.play(2);
                latch = 0;
                sub = 1;
                world.camera.setFollow(true);
            }
            return;
        }
        if (anim.atLastFrame()) {
            toStand();
        }
    }

    public PlayerStep step() {
        return step;
    }

    public Body body() {
        return body;
    }

    public boolean grounded() {
        return latch == 0 && step != PlayerStep.LADDER && step != PlayerStep.BAR && step != PlayerStep.ROPE;
    }

    public boolean locked() {
        return world.locked() || step == PlayerStep.GATE_WALK || step == PlayerStep.EVENT_HOLD
                || step == PlayerStep.BEAM_IN || step == PlayerStep.DEAD;
    }

    public int chargeLevel() {
        return chargeLevel;
    }

    public int chargeTimer() {
        return chargeTimer;
    }

    public boolean shooting() {
        return shotPose > 0;
    }

    public boolean invulnerable() {
        return iframes > 0 || pendingHit || pendingDeath;
    }

    public boolean dying() {
        return step == PlayerStep.DEAD || pendingDeath;
    }

    public float busterX() {
        int forward = BusterPoints.forward(anim.anim(), anim.sprite());
        return x + (faceRight ? forward : -forward);
    }

    public float busterY() {
        return y + BusterPoints.down(anim.anim(), anim.sprite());
    }

    @Override
    public String spriteSet() {
        return "x-falcon";
    }

    @Override
    public boolean visible() {
        if (hidden) {
            return false;
        }
        return iframes == 0 || step == PlayerStep.HURT || (iframes & 2) == 0;
    }

    @Override
    public Rect hurtBox() {
        return body.hasBox && step != PlayerStep.DEAD ? body.box() : null;
    }

    public boolean hurt(World w, int damage, float attackerX) {
        if (dying() || invulnerable() || locked()) {
            return false;
        }
        int d = Damage.toPlayer(damage);
        hp -= d;
        hitFromRight = attackerX > x;
        iframes = HURT_IFRAMES;
        dropCharge();
        if (hp <= 0) {
            hp = 0;
            pendingDeath = true;
        } else {
            pendingHit = true;
        }
        return true;
    }

    public void kill() {
        if (!dying()) {
            hp = 0;
            pendingDeath = true;
        }
    }

    @Override
    public void update(World w) {
        world = w;
        prevX = x;
        prevY = y;
        if (squeezeTimer > 0) {
            squeezeTimer--;
        }
        spikeTouched = false;
        boolean lock = locked();
        held = lock ? 0 : w.buttons.held();
        pressed = lock ? 0 : w.buttons.pressed();
        released = lock ? 0 : w.buttons.released();
        if (hangCooldown > 0) {
            hangCooldown--;
        }
        if (shotWait > 0) {
            shotWait--;
        }
        if (shotPose > 0 && --shotPose == 0) {
            endShotPose();
        }
        if (iframes > 0 && step != PlayerStep.DEAD) {
            iframes--;
        }
        if (pendingDeath) {
            pendingDeath = false;
            startDeath();
        } else if (pendingHit) {
            pendingHit = false;
            startHurt();
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
            case ROPE -> updateRope();
            case SABRE -> updateSabre();
            case HURT -> updateHurt();
            case DEAD -> updateDeath();
            case BEAM_IN -> updateBeamIn();
            case EVENT_HOLD -> {
                anim.tick();
                if (latch == 0) {
                    groundMove(face() * eventWalk);
                    if (step == PlayerStep.FALL) {
                        step = PlayerStep.EVENT_HOLD;
                    }
                } else {
                    int f = airMove();
                    if ((f & TerrainMover.FLOOR) != 0) {
                        latch = 0;
                        speedY = 0;
                    }
                }
            }
            case TELEPORT -> {
            }
            default -> anim.tick();
        }
        if (step != PlayerStep.SABRE) {
            endSlash();
        }
        if (step != PlayerStep.DEAD) {
            updateCharge();
            clampToCamera();
            if (spikeTouched && iframes == 0) {
                kill();
            }
            if (w.camera.y() >= w.camera.maxY() && body.y - 8 >= w.camera.maxY() + 256) {
                kill();
            }
        }
        x = body.x + ((world.frame & 1) == 0 ? split : -split);
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

    private boolean isReleased(int bit) {
        return (released & bit) != 0;
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
        anim.play(shotPose > 0 && SHOOTING_VERSIONS.contains(a) ? a + 112 : a);
    }

    private void endShotPose() {
        int a = anim.anim();
        if (STAND_SHOTS.contains(a)) {
            anim.play(standAnim());
        } else if (SHOOTING_VERSIONS.contains(a - 112)) {
            anim.play(a - 112, anim.frameIndex());
        }
    }

    private void startShotPose(int poseUpdates, int standShot) {
        shotPose = poseUpdates;
        int a = anim.anim();
        if (step == PlayerStep.STAND) {
            anim.play(standShot);
        } else if (step == PlayerStep.CROUCH && sub == 1) {
            anim.play(standShot == 91 ? 139 : 108);
        } else if (SHOOTING_VERSIONS.contains(a)) {
            anim.play(a + 112, anim.frameIndex());
        }
    }

    private int countShots() {
        int n = 0;
        for (Entity e : world.entities) {
            if (!e.dead && (e instanceof BusterShot || e instanceof ChargeFlash)) {
                n++;
            }
        }
        return n;
    }

    private boolean tryShoot(boolean allowSabre) {
        if (shotWait > 0) {
            return false;
        }
        if (allowSabre && isPressed(Buttons.WEAPON)) {
            startSabre();
            return true;
        }
        int id = -1;
        if (isPressed(Buttons.BUSTER)) {
            id = 0;
        } else if (isReleased(Buttons.BUSTER) && chargeLevel > 0) {
            id = chargeLevel == 1 ? 9 : 19;
        }
        if (id < 0) {
            return false;
        }
        int limit = id == 19 ? 2 : 3;
        if (countShots() >= limit) {
            return false;
        }
        if (id == 0) {
            startShotPose(20, 88);
            world.add(new BusterShot(world, 0, busterX(), busterY(), faceRight));
            world.sfx.play(1, 8);
            shotWait = 6;
        } else {
            world.add(new ChargeFlash(world, this, id));
            shotWait = 15;
            startShotPose(id == 9 ? 25 : 35, id == 9 ? 90 : 91);
        }
        return false;
    }

    private void updateCharge() {
        int raw = locked() ? 0 : world.buttons.held();
        int rawPressed = locked() ? 0 : world.buttons.pressed();
        int rawReleased = locked() ? 0 : world.buttons.released();
        int bits = Buttons.BUSTER | Buttons.WEAPON;
        if ((rawPressed & bits) != 0 || (rawReleased & bits) != 0 || step == PlayerStep.HURT) {
            dropCharge();
            return;
        }
        if ((raw & Buttons.BUSTER) == 0) {
            return;
        }
        chargeTimer++;
        if (chargeTimer == 25) {
            chargeLevel = 1;
            world.sfx.play(1, 7);
        } else if (chargeTimer == 90) {
            chargeLevel = 2;
        }
    }

    private void dropCharge() {
        if (chargeLevel > 0) {
            world.sfx.stop(1, 7);
        }
        chargeTimer = 0;
        chargeLevel = 0;
    }

    private boolean groundMove(float dx) {
        int f = world.mover.move(body, dx, 1, true, world.solids());
        lastFlags = f;
        spikeTouched |= world.mover.lastSpike;
        if ((f & TerrainMover.FLOOR) == 0) {
            toFall();
            return false;
        }
        return true;
    }

    private int airMove() {
        int f = world.mover.move(body, speedX, -speedY, false, world.solids());
        lastFlags = f;
        spikeTouched |= world.mover.lastSpike;
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
        if (tryShoot(true)) {
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
        anim.play(standAnim());
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
        if (!(shotPose > 0 && STAND_SHOTS.contains(anim.anim()))) {
            anim.playIfNot(standAnim());
        }
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
        tryShoot(false);
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
        play(shotPose > 0 ? 109 : 81);
    }

    private void updateCrouch() {
        int d = dir();
        if (d != 0) {
            faceRight = d > 0;
        }
        tryShoot(false);
        if (isPressed(Buttons.DASH)) {
            startDash();
            return;
        }
        if (isPressed(Buttons.JUMP)) {
            startJump(false);
            return;
        }
        anim.tick();
        if (sub != 2 && !isHeld(Buttons.DOWN)) {
            play(shotPose > 0 ? 110 : 82);
            sub = 2;
        } else if (sub == 0 && anim.atLastFrame()) {
            sub = 1;
        } else if (sub == 1 && shotPose == 0 && anim.anim() != 81) {
            anim.play(81, 2);
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
        if (tryShoot(true)) {
            return;
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
        if (tryShoot(true)) {
            return;
        }
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
        tryShoot(false);
        anim.tick();
        if (anim.entered(0x80)) {
            faceRight = wallSide < 0;
        }
        if (--wallTimer <= 0) {
            toWallSlide();
        }
    }

    private void toWallSlide() {
        step = PlayerStep.WALL_SLIDE;
        sub = 0;
        play(15);
        speedY = PlayerConst.WALL_SLIDE;
        gravity = 0;
        speedX = 0;
        latch = -1;
        faceRight = wallSide < 0;
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
            if (tryShoot(true)) {
                return;
            }
            anim.tick();
            if (dir() == wallSide) {
                int f = world.mover.move(body, 0, -PlayerConst.WALL_SLIDE, false, world.solids());
                spikeTouched |= world.mover.lastSpike;
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
            tryShoot(false);
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
        if (tryShoot(true)) {
            return;
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
        tryShoot(false);
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
        spikeTouched |= world.mover.lastSpike;
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
        if (shotWait == 0 && isHeld(Buttons.UP) && TileRules.isLadder(tileAt(body.x, body.top()))) {
            body.x = (float) Math.floor(body.x / 16) * 16 + 8;
            play(27);
            latch = 1;
            step = PlayerStep.LADDER;
            sub = LADDER_FROM_GROUND;
            speedX = speedY = 0;
            return true;
        }
        if (shotWait == 0 && isHeld(Buttons.DOWN) && TileRules.isLadderTop(tileAt(body.x, body.bottom()))) {
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
        tryShoot(false);
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
        if (!isHeld(Buttons.UP) || hangCooldown > 0) {
            return false;
        }
        float top = body.top();
        if (TileRules.isRope(tileAt(body.x, top))) {
            // TODO: the hang test point is not read; the rope is tested at the head, as the bars are, and the shooting pose 144 is not used (docs/gaps.md, "Rope hang")
            body.x = ((int) body.x & ~15) + 8;
            step = PlayerStep.ROPE;
            speedX = speedY = gravity = 0;
            airDashSpeed = airDashUsed = dashFlag = false;
            ropeDash = ropeSound = 0;
            play(160);
            world.sfx.play(0, 38);
            return true;
        }
        if (speedY > 0) {
            return false;
        }
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

    private void updateRope() {
        anim.tick();
        if (isPressed(Buttons.JUMP)) {
            hangCooldown = 4;
            ropeDash = 0;
            if (isHeld(Buttons.DOWN)) {
                toFall();
            } else {
                startJump(false);
            }
            return;
        }
        tryShoot(false);
        int d = dir();
        if (d != 0) {
            faceRight = d > 0;
        }
        if (isPressed(Buttons.DASH) && ropeDash == 0) {
            ropeDash = PlayerConst.DASH_TIME;
            ropeDashDir = isHeld(Buttons.UP) ? -1 : 1;
        }
        float dy = 0;
        if (ropeDash > 0) {
            int held = ropeDashDir < 0 ? Buttons.UP : Buttons.DOWN;
            if (--ropeDash == 0 || !isHeld(held)) {
                ropeDash = 0;
                world.sfx.play(1, 5);
            } else {
                dy = ropeDashDir * PlayerConst.HANG_DASH;
            }
        } else if (isHeld(Buttons.UP)) {
            dy = -PlayerConst.ROPE_MOVE;
        } else if (isHeld(Buttons.DOWN)) {
            dy = PlayerConst.ROPE_MOVE;
        }
        if (dy != 0) {
            float oldY = body.y;
            int f = world.mover.move(body, 0, dy, false, world.solids());
            if (!TileRules.isRope(tileAt(body.x, body.top()))) {
                body.y = oldY;
                ropeDash = 0;
            } else if (++ropeSound % 21 == 0) {
                world.sfx.play(0, 39);
            }
            if ((f & TerrainMover.FLOOR) != 0) {
                ropeDash = 0;
            }
        }
        if (!TileRules.isRope(tileAt(body.x, body.top()))) {
            hangCooldown = 4;
            toFall();
        }
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
        tryShoot(false);
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

    private void startSabre() {
        if (slash != null) {
            return;
        }
        if (step == PlayerStep.WALL_SLIDE) {
            sabreKind = SABRE_WALL;
            anim.play(187);
        } else if (latch == 0) {
            sabreKind = SABRE_FLOOR;
            anim.play(183);
            speedX = 0;
        } else {
            sabreKind = SABRE_AIR;
            anim.play(184);
        }
        step = PlayerStep.SABRE;
        sabreCancel = false;
        shotWait = 1;
    }

    private void updateSabre() {
        anim.tick();
        for (int i = 1; i < CustomChanges.SABRE_RECOVERY_SPEED; i++) {
            if ((anim.flag() & 0x40) == 0 || anim.flagEntered() || anim.atLastFrame()) {
                break;
            }
            anim.tick();
        }
        if (slash == null && anim.entered(0x80)) {
            slash = new SabreSlash(world, this, sabreKind == SABRE_FLOOR ? 50 : 51, switch (sabreKind) {
                case SABRE_FLOOR -> SabreSlash.FLOOR_ANIM;
                case SABRE_AIR -> SabreSlash.AIR_ANIM;
                default -> SabreSlash.WALL_ANIM;
            });
            world.add(slash);
            // TODO: X's sabre sound is not named in the docs; the Magma Blade slash sound (1, 23) is used (docs/gaps.md, "Sabre sound")
            world.sfx.play(1, 23);
        }
        if (anim.entered(0x40)) {
            sabreCancel = true;
        }
        switch (sabreKind) {
            case SABRE_FLOOR -> {
                if (sabreCancel && isPressed(Buttons.JUMP)) {
                    endSlash();
                    startJump(false);
                    return;
                }
                if (sabreCancel && isHeld(Buttons.DOWN)) {
                    endSlash();
                    toCrouch();
                    return;
                }
                if (!groundMove(0)) {
                    return;
                }
            }
            case SABRE_AIR -> {
                if (wallTest()) {
                    return;
                }
                int f = airControlMove();
                if ((f & TerrainMover.FLOOR) != 0) {
                    endSlash();
                    toLand();
                    return;
                }
            }
            default -> {
                if (isPressed(Buttons.JUMP)) {
                    endSlash();
                    wallKick(wallSide);
                    return;
                }
                if (!wallStillThere()) {
                    endSlash();
                    toFall();
                    return;
                }
            }
        }
        if (anim.atLastFrame()) {
            endSlash();
            switch (sabreKind) {
                case SABRE_FLOOR -> toStand();
                case SABRE_AIR -> {
                    step = speedY > 0 ? PlayerStep.JUMP : PlayerStep.FALL;
                    latch = speedY > 0 ? 1 : -1;
                    play(speedY > 0 ? 10 : 11);
                }
                default -> toWallSlide();
            }
        }
    }

    private void endSlash() {
        if (slash != null) {
            slash.dead = true;
            slash = null;
        }
    }

    private void startHurt() {
        endSlash();
        dropCharge();
        shotPose = 0;
        step = PlayerStep.HURT;
        anim.play(35);
        faceRight = hitFromRight;
        speedX = HURT_SPEED;
        airDashSpeed = dashFlag = false;
        world.sfx.play(3, world.rng.nextInt(2));
        world.sfx.play(3, 17);
    }

    private void updateHurt() {
        anim.tick();
        int away = -face();
        float dx = blocked(away) ? 0 : away * speedX;
        int f = world.mover.move(body, dx, latch == 0 ? 1 : 0, latch == 0, world.solids());
        spikeTouched |= world.mover.lastSpike;
        if (latch == 0 && (f & TerrainMover.FLOOR) == 0) {
            latch = -1;
        }
        speedX = Math.max(0, speedX - HURT_DECEL);
        if (iframes <= HURT_REACTION_END) {
            if (world.mover.onFloor(body, world.solids())) {
                latch = 0;
                toStand();
            } else {
                play(11);
                gravity = FALL_GRAVITY;
                speedX = speedY = 0;
                latch = -1;
                step = PlayerStep.FALL;
            }
        }
    }

    private void startDeath() {
        endSlash();
        dropCharge();
        world.sfx.stop(1, 5);
        step = PlayerStep.DEAD;
        anim.play(33);
        deathTimer = 0;
        hidden = false;
        world.freeze(9);
    }

    private void updateDeath() {
        deathTimer++;
        if (deathTimer < 9) {
            return;
        }
        if (deathTimer == 9) {
            hidden = true;
            world.sfx.play(3, 16);
            world.sfx.play(3, 14);
            Orb.wave(world, x, y, 0);
        } else if (deathTimer == 28) {
            Orb.wave(world, x, y, 2);
        } else if (deathTimer == 47) {
            Orb.wave(world, x, y, 1);
            Orb.wave(world, x, y, 3);
        } else if (deathTimer == 69) {
            world.flow.playerDeathDone();
        }
    }
}
