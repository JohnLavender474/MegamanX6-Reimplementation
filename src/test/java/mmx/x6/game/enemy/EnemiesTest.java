package mmx.x6.game.enemy;

import static mmx.x6.input.Buttons.BUSTER;
import static mmx.x6.input.Buttons.RIGHT;
import static mmx.x6.input.Buttons.WEAPON;
import static mmx.x6.physics.Grids.grid;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.ArrayList;
import java.util.List;
import mmx.x6.game.Damage;
import mmx.x6.game.Entity;
import mmx.x6.game.Sim;
import mmx.x6.game.Spawner;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class EnemiesTest {

    private static final String SKY = "................................................................";
    private static final String FLOOR = "################################################################";

    private static Sim flat(float playerX) {
        String[] rows = new String[16];
        java.util.Arrays.fill(rows, SKY);
        rows[10] = FLOOR;
        rows[11] = FLOOR;
        Sim s = Sim.on(grid(rows)).playerAt(playerX, 160);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        return s;
    }

    private static RecordDef rec(String name, int id, int x, int y) {
        return new RecordDef(1, name, "main", 0, 0, id, 0, x, y);
    }

    private static <T> List<T> all(Sim s, Class<T> type) {
        List<T> out = new ArrayList<>();
        for (Entity e : s.world.entities) {
            if (type.isInstance(e) && !e.dead) {
                out.add(type.cast(e));
            }
        }
        return out;
    }

    private static <T extends Entity> T spawn(Sim s, T e) {
        s.world.entities.add(e);
        return e;
    }

    @Test
    void junkroidShotTiming() {
        Sim s = flat(100);
        spawn(s, new Junkroid(s.world, rec("Junkroid", 1, 200, 136)));
        List<Integer> shotTimes = new ArrayList<>();
        int before = 0;
        for (int i = 1; i <= 330; i++) {
            s.step(0);
            s.world.player.placeStanding(100, 139);
            int now = all(s, JunkroidShot.class).size();
            if (now > before) {
                shotTimes.add(i);
            }
            before = now;
        }
        assertTrue(shotTimes.size() >= 2, "shots " + shotTimes);
        assertTrue(shotTimes.get(0) >= 10 && shotTimes.get(0) <= 14, "first " + shotTimes);
        assertTrue(Math.abs(shotTimes.get(1) - shotTimes.get(0) - 153) <= 2, "interval " + shotTimes);
        assertEquals(shotTimes.size(), s.sfx.count(2, 26));
    }

    @Test
    void junkroidDormant() {
        Sim s = flat(100);
        Junkroid j = spawn(s, new Junkroid(s.world, rec("Junkroid", 0, 200, 136)));
        boolean faceRight = j.faceRight;
        for (int i = 0; i < 300; i++) {
            s.step(0);
            s.world.player.placeStanding(i % 2 == 0 ? 100 : 300, 139);
        }
        assertTrue(all(s, JunkroidShot.class).isEmpty());
        assertEquals(faceRight, j.faceRight);
    }

    @Test
    void junkroidHp() {
        Sim s = flat(100);
        Junkroid j = spawn(s, new Junkroid(s.world, rec("Junkroid", 0, 170, 136)));
        for (int i = 0; i < 4; i++) {
            s.step(BUSTER);
            s.steps(7, 0);
        }
        s.steps(5, 0);
        assertTrue(j.dead);
        assertEquals(4, all(s, Debris.class).size());
    }

    @Test
    void hoverGunnerFlyAndBurst() {
        Sim s = flat(100);
        HoverGunner g = spawn(s, new HoverGunner(s.world, rec("HoverGunner", 0, 300, 80)));
        float x0 = g.x;
        s.step(0);
        assertEquals(x0 - 4, g.x, 0.001);
        int guard = 0;
        while (g.step() == HoverGunner.Step.FLY && guard++ < 100) {
            s.step(0);
        }
        assertEquals(HoverGunner.Step.BURST, g.step());
        float dx = Math.abs(g.x - s.world.player.x);
        assertTrue(dx >= 60 && dx <= 116, "dx " + dx);
        List<Integer> times = new ArrayList<>();
        int before = 0;
        for (int i = 1; i <= 60; i++) {
            s.step(0);
            int now = all(s, HoverGunnerShot.class).size();
            if (now > before) {
                times.add(i);
            }
            before = now;
        }
        assertEquals(3, times.size(), "times " + times);
        assertEquals(9, times.get(1) - times.get(0));
        assertEquals(9, times.get(2) - times.get(1));
        HoverGunnerShot shot = all(s, HoverGunnerShot.class).get(0);
        assertEquals(2.0f, (float) Math.hypot(shot.vx(), shot.vy()), 0.01);
    }

    @Test
    void hoverGunnerId1Waits() {
        Sim s = flat(100);
        HoverGunner g = spawn(s, new HoverGunner(s.world, rec("HoverGunner", 1, 100, 80)));
        s.steps(10, 0);
        assertFalse(g.visible());
        assertEquals(null, g.contactBox());
        s.world.player.placeStanding(290, 139);
        s.step(0);
        s.step(0);
        assertTrue(g.visible());
        assertTrue(g.faceRight);
    }

    @Test
    void hoverGunnerLeaves() {
        Sim s = flat(100);
        HoverGunner g = spawn(s, new HoverGunner(s.world, rec("HoverGunner", 0, 300, 80)));
        int guard = 0;
        while (g.step() != HoverGunner.Step.LEAVE && guard++ < 600) {
            s.step(0);
            s.world.player.placeStanding(100, 139);
        }
        assertEquals(HoverGunner.Step.LEAVE, g.step());
        float y = g.y;
        s.step(0);
        assertEquals(y - 2, g.y, 0.001);
        s.steps(200, 0);
        assertTrue(g.dead);
    }

    @Test
    void metaWheelRollsAndTurns() {
        String[] rows = new String[16];
        java.util.Arrays.fill(rows, "##..............................................................");
        rows[10] = FLOOR;
        rows[11] = FLOOR;
        Sim s = Sim.on(grid(rows)).playerAt(600, 160);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        MetaWheelF w = spawn(s, new MetaWheelF(s.world, rec("MetaWheelF", 1, 120, 130)));
        s.steps(30, 0);
        float x = w.x;
        s.step(0);
        assertEquals(-0.75f, w.x - x, 0.001);
        int guard = 0;
        while (w.step() != MetaWheelF.Step.TURN && guard++ < 200) {
            s.step(0);
        }
        assertEquals(MetaWheelF.Step.TURN, w.step());
        assertTrue(s.sfx.count(2, 4) >= 2);
        int still = 0;
        while (w.step() == MetaWheelF.Step.TURN && still < 200) {
            s.step(0);
            still++;
        }
        assertTrue(Math.abs(still - 111) <= 2, "still " + still);
        float turnedAt = w.x;
        s.steps(40, 0);
        assertTrue(w.x > turnedAt + 10, "x " + w.x + " from " + turnedAt);
    }

    @Test
    void metaWheelSabreOnly() {
        Sim s = flat(100);
        MetaWheelF w = new MetaWheelF(s.world, rec("MetaWheelF", 1, 200, 130));
        assertEquals(Damage.BLOCKED, Damage.hitObject(w, 0));
        assertEquals(51, Damage.hitObject(w, 50));
        assertEquals(28, w.hp);
    }

    @Test
    void metaDridlerCycle() {
        String[] rows = new String[30];
        java.util.Arrays.fill(rows, SKY);
        rows[28] = FLOOR;
        rows[29] = FLOOR;
        Sim s = Sim.on(grid(rows)).playerAt(300, 448);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 240, true, 1));
        MetaDridler d = spawn(s, new MetaDridler(s.world, rec("MetaDridler", 0, 200, 60)));
        s.steps(89, 0);
        assertEquals(MetaDridler.Step.WAIT, d.step());
        s.steps(2, 0);
        assertEquals(MetaDridler.Step.DROP, d.step());
        assertEquals(1, s.sfx.count(2, 1));
        int guard = 0;
        while (d.step() == MetaDridler.Step.DROP && guard++ < 100) {
            s.step(0);
        }
        assertEquals(MetaDridler.Step.DRILL, d.step());
        assertEquals(60 + 128, d.y, 0.5);
        s.steps(60, 0);
        assertEquals(MetaDridler.Step.RISE, d.step());
        float y = d.y;
        s.step(0);
        assertEquals(y - 0.5f, d.y, 0.001);
        s.steps(260, 0);
        assertEquals(MetaDridler.Step.WAIT, d.step());
        assertEquals(60, d.y, 0.001);
        assertEquals(8, d.contactDamage);
    }

    @Test
    void metaDridlerCannotDie() {
        Sim s = flat(100);
        MetaDridler d = new MetaDridler(s.world, rec("MetaDridler", 0, 200, 60));
        assertEquals(Damage.BLOCKED, Damage.hitObject(d, 19));
        assertEquals(Damage.BLOCKED, Damage.hitObject(d, 50));
        assertEquals(1, d.hp);
    }

    private static RecordDef block(int x, int y) {
        return new RecordDef(40, "SabreBlock", "stage", 4, 2, 0, 0, x, y);
    }

    @Test
    void sabreBlockIsSolid() {
        Sim s = flat(100);
        SabreBlock b = spawn(s, new SabreBlock(s.world, block(200, 136)));
        s.step(0);
        assertEquals(128, b.y, 0.001);
        assertTrue(s.world.solids().stream().anyMatch(r -> r.x() == 168 && r.y() == 96 && r.w() == 64));
        s.steps(60, RIGHT);
        assertEquals(168 - 11, s.world.player.x, 0.01);
    }

    @Test
    void sabreBlockBreaks() {
        Sim s = flat(150);
        Spawner sp = new Spawner(List.of(block(200, 136)), 1, new IntroEntityFactory());
        s.world.spawner = sp;
        s.step(0);
        SabreBlock b = all(s, SabreBlock.class).get(0);
        s.world.player.placeStanding(150, 139);
        s.world.player.faceRight = true;
        int guard = 0;
        while (!b.dead && guard++ < 40) {
            s.step(WEAPON);
            for (int i = 0; i < 44 && !b.dead; i++) {
                s.step(0);
            }
        }
        assertTrue(b.dead, "hp " + b.hp);
        assertEquals(3, all(s, Debris.class).size());
        assertTrue(all(s, Explosion.class).size() >= 6);
        s.world.camera.reset(new Checkpoint(0, 0, 700, 0, 700, 700, 0, 0, true, 1));
        s.step(0);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        s.step(0);
        assertTrue(all(s, SabreBlock.class).isEmpty());
    }

    @Test
    void sabreBlockRespawnsFullHpAfterLeaving() {
        Sim s = flat(60);
        Spawner sp = new Spawner(List.of(block(200, 136)), 1, new IntroEntityFactory());
        s.world.spawner = sp;
        s.step(0);
        SabreBlock b = all(s, SabreBlock.class).get(0);
        b.hp = 10;
        s.world.camera.reset(new Checkpoint(0, 0, 700, 0, 700, 700, 0, 0, true, 1));
        s.step(0);
        s.step(0);
        assertTrue(b.dead);
        s.world.camera.reset(new Checkpoint(0, 0, 0, 0, 0, 0, 0, 0, true, 1));
        s.step(0);
        s.step(0);
        assertEquals(32, all(s, SabreBlock.class).get(0).hp);
    }
}
