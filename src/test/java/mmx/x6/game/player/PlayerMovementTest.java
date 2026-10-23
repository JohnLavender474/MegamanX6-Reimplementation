package mmx.x6.game.player;

import static mmx.x6.input.Buttons.DASH;
import static mmx.x6.input.Buttons.DOWN;
import static mmx.x6.input.Buttons.JUMP;
import static mmx.x6.input.Buttons.LEFT;
import static mmx.x6.input.Buttons.RIGHT;
import static mmx.x6.input.Buttons.UP;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.game.Sim;
import org.junit.jupiter.api.Test;

class PlayerMovementTest {

    private static final String SKY = "........................................";
    private static final String FLOOR = "########################################";

    private static Sim flat() {
        return Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, FLOOR, FLOOR)).playerAt(100, 160);
    }

    private static Player p(Sim s) {
        return s.world.player;
    }

    @Test
    void walkStartThenWalk() {
        Sim s = flat();
        float x0 = p(s).x;
        s.step(RIGHT);
        assertEquals(PlayerStep.WALK_START, p(s).step());
        assertEquals(x0 + 0.5f, p(s).x, 0.001);
        s.steps(5, RIGHT);
        assertEquals(PlayerStep.WALK_START, p(s).step());
        assertEquals(x0 + 3.0f, p(s).x, 0.001);
        s.steps(2, RIGHT);
        assertEquals(PlayerStep.WALK, p(s).step());
        float before = p(s).x;
        s.step(RIGHT);
        assertEquals(before + 2.0f, p(s).x, 0.001);
        assertTrue(p(s).faceRight);
        s.step(0);
        assertEquals(PlayerStep.STAND, p(s).step());
    }

    @Test
    void fullJumpHeight() {
        Sim s = flat();
        float y0 = p(s).y;
        float minY = y0;
        int top = 0;
        for (int i = 1; i <= 60; i++) {
            s.step(JUMP);
            if (p(s).y < minY) {
                minY = p(s).y;
                top = i;
            }
        }
        float rise = y0 - minY;
        assertTrue(rise > 67.5f && rise < 69.5f, "rise " + rise);
        assertTrue(Math.abs(top - 23) <= 1, "top " + top);
        assertTrue(p(s).grounded());
    }

    @Test
    void tapJumpIsShort() {
        Sim s = flat();
        float y0 = p(s).y;
        float minY = y0;
        s.step(JUMP);
        for (int i = 0; i < 30; i++) {
            s.step(0);
            minY = Math.min(minY, p(s).y);
        }
        assertTrue(y0 - minY < 7, "rise " + (y0 - minY));
    }

    @Test
    void releaseCutsTheRise() {
        Sim s = flat();
        s.steps(5, JUMP);
        s.step(0);
        assertTrue(p(s).speedY <= 0);
    }

    @Test
    void fallSpeedCaps() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY,
                SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY,
                SKY, SKY, SKY, FLOOR)).playerAt(100, 40);
        s.world.player.placeStanding(100, 40);
        s.step(0);
        for (int i = 0; i < 60; i++) {
            s.step(0);
            assertTrue(p(s).speedY >= -6.5f - 1e-4);
        }
    }

    @Test
    void walkOffLedgeFalls() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY,
                "########................................", FLOOR)).playerAt(110, 160);
        int i = 0;
        while (p(s).step() != PlayerStep.FALL && i++ < 40) {
            s.step(RIGHT);
        }
        assertEquals(PlayerStep.FALL, p(s).step());
        assertTrue(p(s).body().left() + 1 >= 128, "left " + p(s).body().left());
        assertTrue(p(s).body().left() + 1 < 128 + 2.01f, "no grace time");
    }

    @Test
    void dashDistance() {
        Sim s = flat();
        float x0 = p(s).x;
        s.step(DASH | RIGHT);
        assertEquals(PlayerStep.DASH, p(s).step());
        for (int i = 0; i < 4; i++) {
            s.step(DASH | RIGHT);
        }
        assertEquals(x0, p(s).x, 0.001);
        s.step(DASH | RIGHT);
        float a = p(s).x;
        s.step(DASH | RIGHT);
        float b = p(s).x;
        assertEquals(6.5f, a - x0, 0.001);
        assertEquals(6.3125f, b - a, 0.001);
        int moving = 2;
        float last = b;
        while (p(s).step() == PlayerStep.DASH && moving < 40) {
            s.step(DASH | RIGHT);
            float d = p(s).x - last;
            last = p(s).x;
            if (p(s).step() == PlayerStep.DASH) {
                assertTrue(d >= 4.125f - 0.001f);
                moving++;
            }
        }
        assertTrue(moving <= 30, "moving " + moving);
        assertEquals(PlayerStep.DASH_END, p(s).step());
        float prev = p(s).x;
        float prevSpeed = 99;
        for (int i = 0; i < 10 && p(s).step() == PlayerStep.DASH_END; i++) {
            s.step(0);
            float d = p(s).x - prev;
            prev = p(s).x;
            assertTrue(d <= prevSpeed + 0.001f);
            prevSpeed = d;
        }
        s.steps(40, 0);
        assertEquals(PlayerStep.STAND, p(s).step());
    }

    @Test
    void dashJumpKeepsDashSpeed() {
        Sim s = flat();
        s.steps(8, DASH | RIGHT);
        s.step(DASH | RIGHT | JUMP);
        assertEquals(PlayerStep.JUMP, p(s).step());
        float a = p(s).x;
        s.step(RIGHT | JUMP);
        assertEquals(4.125f, p(s).x - a, 0.001);
        while (!p(s).grounded()) {
            float before = p(s).x;
            s.step(RIGHT | JUMP);
            if (!p(s).grounded()) {
                assertEquals(4.125f, p(s).x - before, 0.001);
            }
        }
    }

    private static Sim wallRoom(char wall) {
        String w = ".........................." + wall + ".............";
        return Sim.on(grid(w, w, w, w, w, w, w, w, w, w, FLOOR, FLOOR)).playerAt(380, 160);
    }

    @Test
    void wallGrabThenSlide() {
        Sim s = wallRoom('#');
        s.steps(12, JUMP);
        int guard = 0;
        while (p(s).step() != PlayerStep.WALL_GRAB && guard++ < 60) {
            s.step(RIGHT);
        }
        assertEquals(PlayerStep.WALL_GRAB, p(s).step());
        for (int i = 0; i < 8; i++) {
            s.step(RIGHT);
        }
        assertEquals(PlayerStep.WALL_SLIDE, p(s).step());
        float y = p(s).y;
        s.step(RIGHT);
        assertEquals(1.4375f, p(s).y - y, 0.001);
    }

    @Test
    void noWallGrabOn57() {
        Sim s = wallRoom('n');
        s.steps(12, JUMP);
        for (int i = 0; i < 30; i++) {
            s.step(RIGHT);
            assertFalse(p(s).step() == PlayerStep.WALL_GRAB || p(s).step() == PlayerStep.WALL_SLIDE);
        }
    }

    @Test
    void noWallGrabForTenUpdatesAfterJump() {
        Sim s = wallRoom('#');
        s.world.player.placeStanding(405, 160 - 21);
        s.step(RIGHT | JUMP);
        for (int i = 0; i < 9; i++) {
            s.step(RIGHT | JUMP);
            assertEquals(PlayerStep.JUMP, p(s).step(), "update " + i);
        }
        s.steps(3, RIGHT | JUMP);
        assertEquals(PlayerStep.WALL_GRAB, p(s).step());
    }

    @Test
    void wallKick() {
        Sim s = wallRoom('#');
        s.steps(12, JUMP);
        int guard = 0;
        while (p(s).step() != PlayerStep.WALL_GRAB && guard++ < 60) {
            s.step(RIGHT);
        }
        s.step(RIGHT | JUMP);
        assertEquals(PlayerStep.WALL_KICK, p(s).step());
        float x = p(s).x;
        for (int i = 0; i < 9; i++) {
            s.step(RIGHT | JUMP);
            assertEquals(PlayerStep.WALL_KICK, p(s).step());
        }
        assertEquals(-18f, p(s).x - x, 0.01);
    }

    @Test
    void falconDash() {
        Sim s = flat();
        s.steps(10, JUMP);
        s.step(DASH | RIGHT);
        assertEquals(PlayerStep.FALCON_DASH, p(s).step());
        float x0 = p(s).x;
        float y0 = p(s).y;
        int still = 0;
        while (p(s).x == x0 && still < 20) {
            s.step(RIGHT);
            still++;
        }
        assertTrue(still >= 7 && still <= 10, "still " + still);
        int moves = 1;
        float last = p(s).x;
        while (p(s).step() == PlayerStep.FALCON_DASH && moves < 40) {
            s.step(0);
            if (p(s).step() == PlayerStep.FALCON_DASH) {
                assertTrue(p(s).x - last >= 4.125f - 0.001f);
                moves++;
            }
            last = p(s).x;
        }
        assertTrue(moves >= 17 && moves <= 19, "moves " + moves);
        assertEquals(y0, p(s).y, 0.5);
    }

    @Test
    void oneAirDashPerAir() {
        Sim s = flat();
        s.steps(10, JUMP);
        s.step(DASH | RIGHT);
        s.steps(40, RIGHT);
        if (!p(s).grounded()) {
            s.step(DASH | RIGHT);
            assertFalse(p(s).step() == PlayerStep.FALCON_DASH);
        }
    }

    private static Sim ladder() {
        return Sim.on(grid(
                SKY, SKY, SKY,
                "##########tTt###########################",
                "...........L............................",
                "...........L............................",
                "...........L............................",
                "...........L............................",
                "...........L............................",
                "...........L............................",
                FLOOR, FLOOR)).playerAt(180, 160);
    }

    @Test
    void ladderClimb() {
        Sim s = ladder();
        s.step(UP);
        assertEquals(PlayerStep.LADDER, p(s).step());
        assertEquals(184, p(s).x, 0.001);
        float y = p(s).y;
        s.steps(8, UP);
        assertEquals(y - 9, p(s).y, 1.01);
        int guard = 0;
        while (p(s).step() == PlayerStep.LADDER && guard++ < 200) {
            s.step(UP);
        }
        assertEquals(PlayerStep.STAND, p(s).step());
        assertEquals(48, p(s).body().bottom(), 0.01);
    }

    @Test
    void ladderDownFromTop() {
        Sim s = ladder();
        s.world.player.placeStanding(184, 48 - 21);
        s.step(0);
        assertEquals(PlayerStep.STAND, p(s).step());
        s.step(DOWN);
        assertEquals(PlayerStep.LADDER, p(s).step());
        s.steps(40, DOWN);
        assertTrue(p(s).y > 48);
    }

    @Test
    void barHang() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, "....=====...............................", SKY, SKY, SKY, SKY, SKY, SKY,
                FLOOR, FLOOR)).playerAt(100, 160);
        s.steps(30, JUMP | UP);
        int guard = 0;
        while (p(s).step() != PlayerStep.BAR && guard++ < 40) {
            s.step(UP);
        }
        assertEquals(PlayerStep.BAR, p(s).step());
        float y = p(s).y;
        float x = p(s).x;
        s.step(UP | RIGHT);
        assertEquals(x + 2, p(s).x, 0.001);
        assertEquals(y, p(s).y, 0.001);
        s.step(UP | DOWN);
        s.step(DOWN | JUMP);
        assertEquals(PlayerStep.FALL, p(s).step());
    }

    @Test
    void slopeWalk() {
        Sim s = Sim.on(grid(SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY, SKY,
                ".......////#############################",
                "#######uuuu#############################")).playerAt(60, 176);
        for (int i = 0; i < 80; i++) {
            s.step(RIGHT);
            assertTrue(p(s).grounded(), "update " + i);
        }
        assertEquals(160, p(s).body().bottom(), 0.01);
    }

    @Test
    void landSoundPlayed() {
        Sim s = flat();
        s.steps(60, JUMP);
        assertEquals(1, s.sfx.count(1, 2));
        assertEquals(1, s.sfx.count(1, 1));
    }
}
