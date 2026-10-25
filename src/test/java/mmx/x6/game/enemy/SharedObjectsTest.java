package mmx.x6.game.enemy;

import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Arrays;
import mmx.x6.game.Damage;
import mmx.x6.game.Entity;
import mmx.x6.game.Progress;
import mmx.x6.game.Sim;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class SharedObjectsTest {

    private static final String SKY = "................................................................";
    private static final String FLOOR = "################################################################";

    private static Sim flat(float playerX) {
        String[] rows = new String[16];
        Arrays.fill(rows, SKY);
        rows[10] = FLOOR;
        rows[11] = FLOOR;
        Sim s = Sim.on(grid(rows)).playerAt(playerX, 160);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        s.world.camera.setFollow(false);
        return s;
    }

    private static RecordDef rec(String name, int id, int x, int y) {
        return new RecordDef(7, name, "main", 0, 0, id, 0, x, y);
    }

    private static <T extends Entity> T spawn(Sim s, T e) {
        s.world.entities.add(e);
        return e;
    }

    private static void kill(Sim s, Entity e) {
        while (Damage.hitObject(e, 18) > 0) {
            e.flash = 0;
        }
        e.onKilled(s.world, 18);
    }

    @Test
    void virusLeavesASoulOnce() {
        Sim s = flat(40);
        NightmareVirus v = spawn(s, new NightmareVirus(s.world, rec("NightmareVirus", 1, 200, 100)));
        assertEquals(10, v.hp);
        kill(s, v);
        assertEquals(NightmareVirus.Step.SOUL, v.step());
        assertNull(v.hurtBox());
        s.steps(32 + 60 + 180 + 40, 0);
        assertEquals(NightmareVirus.Step.IDLE, v.step());
        assertNotNull(v.hurtBox());
        kill(s, v);
        assertTrue(v.dead);
    }

    @Test
    void soulPickupAddsEight() {
        Sim s = flat(200);
        NightmareVirus v = spawn(s, new NightmareVirus(s.world, rec("NightmareVirus", 1, 200, 136)));
        kill(s, v);
        s.steps(40, 0);
        assertTrue(v.dead);
        assertEquals(8, s.world.progress.souls);
        assertEquals(1, s.sfx.count(0, 41));
    }

    @Test
    void reploidRescueGivesALifeAndHeals() {
        Sim s = flat(100);
        s.world.player.hp = 20;
        RescueReploid r = spawn(s, new RescueReploid(s.world, rec("RescueReploid", 3, 120, 144)));
        int lives = s.world.progress.lives;
        s.steps(10, mmx.x6.input.Buttons.RIGHT);
        assertEquals(Progress.REPLOID_SAVED, s.world.progress.reploid(3));
        assertEquals(lives + 1, s.world.progress.lives);
        assertEquals(RescueReploid.State.HEAL, r.state());
        s.steps(80, 0);
        assertTrue(s.world.player.hp > 20);
        assertFalse(r.state() == RescueReploid.State.HEAL);
    }

    @Test
    void savedReploidDoesNotComeBack() {
        Sim s = flat(40);
        s.world.progress.setReploid(3, Progress.REPLOID_SAVED);
        RescueReploid r = spawn(s, new RescueReploid(s.world, rec("RescueReploid", 3, 120, 144)));
        s.step(0);
        assertTrue(r.dead);
    }

    @Test
    void snatcherInfectsAReploid() {
        Sim s = flat(20);
        RescueReploid r = spawn(s, new RescueReploid(s.world, rec("RescueReploid", 5, 200, 144)));
        spawn(s, new NightmareVirus(s.world, rec("NightmareVirus", 0x21, 240, 80)));
        s.steps(300, 0);
        assertEquals(Progress.REPLOID_LOST, s.world.progress.reploid(5));
        s.steps(200, 0);
        assertTrue(r.state() == RescueReploid.State.ARMED || r.state() == RescueReploid.State.ATTACK);
        assertEquals(13, r.hp);
    }
}
