package mmx.x6.game.boss;

import static mmx.x6.game.boss.D1000.BossStep.BOMB;
import static mmx.x6.game.boss.D1000.BossStep.RAM;
import static mmx.x6.game.boss.D1000.BossStep.SUMMON;
import static mmx.x6.game.boss.D1000.BossStep.WALK;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import mmx.x6.game.CustomChanges;
import mmx.x6.game.Damage;
import mmx.x6.game.Entity;
import mmx.x6.game.Sim;
import mmx.x6.game.StageFlow;
import mmx.x6.game.fx.Explosion;
import mmx.x6.game.player.BusterShot;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.Checkpoint;
import mmx.x6.level.CollisionGrid;
import mmx.x6.level.RecordDef;
import org.junit.jupiter.api.Test;

class D1000Test {

    private static final RecordDef REC = new RecordDef(56, "D1000", "main", 0, 48, 0, 0, 6162, 346);

    static Sim bossRoom() {
        int w = 412;
        int h = 30;
        int[] v = new int[w * h];
        for (int x = 0; x < w; x++) {
            v[26 * w + x] = 56;
            v[27 * w + x] = 56;
        }
        for (int y = 13; y < 26; y++) {
            for (int col : new int[] {379, 403, 407, 408, 410, 411}) {
                v[y * w + col] = 56;
            }
        }
        Sim s = Sim.on(new CollisionGrid(w, h, v)).playerAt(6120, 416);
        s.world.camera.reset(new Checkpoint(6120, 395, 6064, 224, 6064, 6144, 224, 224, true, 1));
        s.world.camera.setFollow(true);
        return s;
    }

    static D1000 spawn(Sim s) {
        D1000 d = new D1000(s.world, REC);
        s.world.entities.add(d);
        return d;
    }

    static void runEntry(Sim s, D1000 d) {
        int guard = 0;
        while (d.step() == D1000.BossStep.ENTRY && guard++ < 3000) {
            s.step(0);
        }
        assertEquals(D1000.BossStep.THINK, d.step());
    }

    static D1000Core core(Sim s) {
        return s.world.entities.stream().filter(e -> e instanceof D1000Core).map(e -> (D1000Core) e)
                .findFirst().orElseThrow();
    }

    static void shoot(Sim s, Entity target, int weaponId) {
        s.world.entities.add(new BusterShot(s.world, weaponId, target.x - 30, target.y, true));
    }

    static void shootCore(Sim s, D1000 body, D1000Core core) {
        boolean fromRight = core.x > body.x;
        float x = fromRight ? core.x + 30 : core.x - 30;
        s.world.entities.add(new BusterShot(s.world, 19, x, core.y, !fromRight));
    }

    @Test
    void thinkTable() {
        assertEquals(WALK, D1000.pick(7, 32, true));
        assertEquals(SUMMON, D1000.pick(8, 32, true));
        assertEquals(RAM, D1000.pick(7, 32, false));
        assertEquals(SUMMON, D1000.pick(8, 32, false));
        assertEquals(SUMMON, D1000.pick(5, 16, true));
        assertEquals(BOMB, D1000.pick(6, 16, true));
        assertEquals(RAM, D1000.pick(5, 16, false));
        assertEquals(BOMB, D1000.pick(6, 16, false));
    }

    @Test
    void ramWindUpMovesTwoPixelsTowardThePlayersY() {
        assertEquals(306, D1000.windUpY(304, 395));
        assertEquals(395, D1000.windUpY(394, 395));
        assertEquals(395, D1000.windUpY(395, 395));
        assertEquals(302, D1000.windUpY(304, 230));
        assertEquals(230, D1000.windUpY(231, 230));
    }

    @Test
    void coreSideAwayFromPlayer() {
        assertEquals(6368, D1000.coreXFor(6100));
        assertEquals(6160, D1000.coreXFor(6300));
    }

    @Test
    void startsAboveTheRoom() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        s.step(0);
        assertEquals(6288, d.x, 0.001);
        assertEquals(160, d.y, 0.001);
        assertEquals(0, d.hp);
        assertFalse(d.visible());
        assertEquals(PlayerStep.EVENT_HOLD, s.world.player.step());
    }

    @Test
    void entryFillsHp() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        runEntry(s, d);
        assertEquals(32, d.hp);
        assertEquals(32, s.world.bossHp);
        assertTrue(s.world.bossBarShown);
        assertTrue(s.music.requests.contains(21));
        assertFalse(s.world.player.locked());
        assertTrue(s.sfx.count(0, 14) >= 10);
        assertEquals(304, d.y, 0.001);
    }

    @Test
    void entryLandsOnTheFloor() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        float lowest = 0;
        int guard = 0;
        while (d.step() == D1000.BossStep.ENTRY && guard++ < 3000) {
            s.step(0);
            lowest = Math.max(lowest, d.y);
        }
        assertEquals(D1000.FLOOR_Y - 44 + CustomChanges.D1000_LANDING_DROP, lowest, 0.001);
        assertEquals(384, lowest, 0.001);
    }

    @Test
    void lemonAndHalfAreDeflected() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        runEntry(s, d);
        assertEquals(Damage.BLOCKED, Damage.hitObject(d, 0));
        assertEquals(Damage.BLOCKED, Damage.hitObject(d, 9));
        assertEquals(32, d.hp);
    }

    @Test
    void fullChargeDoesOneThenInvincible() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        runEntry(s, d);
        shoot(s, d, 19);
        s.steps(6, 0);
        assertEquals(31, d.hp);
        shoot(s, d, 19);
        s.steps(6, 0);
        assertEquals(31, d.hp);
    }

    @Test
    void coreHitStuns() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        runEntry(s, d);
        s.step(0);
        D1000Core c = core(s);
        assertTrue(c.active());
        shootCore(s, d, c);
        int guard = 0;
        while (d.step() != D1000.BossStep.STUN && guard++ < 20) {
            s.step(0);
        }
        assertEquals(D1000.BossStep.STUN, d.step());
        assertEquals(null, d.contactBox());
        s.steps(120, 0);
        assertEquals(24, d.hp);
    }

    @Test
    void fourCoreHitsWin() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        runEntry(s, d);
        for (int i = 0; i < 4 && d.step() != D1000.BossStep.DEATH; i++) {
            int guard = 0;
            while (!core(s).active() && guard++ < 2000) {
                s.step(0);
                s.world.player.hp = 32;
            }
            shootCore(s, d, core(s));
            s.steps(10, 0);
            guard = 0;
            while (d.step() == D1000.BossStep.STUN && guard++ < 1000) {
                s.step(0);
                s.world.player.hp = 32;
            }
        }
        assertEquals(D1000.BossStep.DEATH, d.step());
    }

    @Test
    void deathBlinksThenExplodes() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        runEntry(s, d);
        d.hp = 1;
        shoot(s, d, 19);
        s.steps(6, 0);
        assertEquals(D1000.BossStep.DEATH, d.step());
        assertTrue(s.world.player.locked());
        assertTrue(s.world.entities.stream().noneMatch(e -> e instanceof D1000Shot || e instanceof D1000Core));
        s.steps(120, 0);
        assertTrue(s.world.entities.stream().noneMatch(e -> e instanceof Explosion));
        s.steps(20, 0);
        assertTrue(s.world.entities.stream().anyMatch(e -> e instanceof Explosion));
    }

    @Test
    void clearAfterTheExplosion() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        runEntry(s, d);
        d.hp = 1;
        shoot(s, d, 19);
        int guard = 0;
        while (s.world.flow.state() != StageFlow.State.CLEAR && guard++ < 3000) {
            s.step(0);
        }
        assertEquals(StageFlow.State.CLEAR, s.world.flow.state());
        assertFalse(s.music.playing());
        assertTrue(s.world.player.locked());
        s.world.flow.anyKey();
        assertEquals(StageFlow.State.PLAYING, s.world.flow.state());
        assertEquals(0, s.world.checkpoint);
        assertFalse(s.world.bossBarShown);
    }

    @Test
    void deathInTheFightRespawnsAtCheckpoint2() {
        Sim s = bossRoom();
        D1000 d = spawn(s);
        runEntry(s, d);
        s.world.checkpoint = 2;
        s.world.player.kill();
        int guard = 0;
        while (!s.world.flow.restarted() && guard++ < 400) {
            s.step(0);
        }
        assertEquals(5904, s.world.player.x, 0.001);
        assertTrue(s.world.entities.stream().noneMatch(e -> e instanceof D1000));
        assertFalse(s.world.bossBarShown);
        List<Integer> r = s.music.requests;
        assertEquals(9, (int) r.get(r.size() - 1));
    }
}
