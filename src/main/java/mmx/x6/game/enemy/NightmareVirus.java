package mmx.x6.game.enemy;

import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class NightmareVirus extends Enemy {

    public enum Step { IDLE, DASH, SHOOT, SNATCH, SOUL, BOSS_SOUL, GONE }

    public static final int BOSS_SOUL_FAST = 0x10;
    public static final int BOSS_SOUL_SLOW = 0x11;
    public static final int REPLOID_SOUL = 0x31;

    private static final Step[][] ROWS = {
            {Step.IDLE, Step.SHOOT},
            {Step.IDLE, Step.DASH},
            {Step.IDLE, Step.DASH, Step.SHOOT, Step.DASH},
            {Step.IDLE, Step.IDLE}};
    private static final int HP = 10;
    private static final int SHOTS = 4;
    private static final int SHOT_DELAY = 60;
    private static final float ORB_SPEED = 5;
    private static final float SNATCH_SPEED = 0.5f;
    private static final float[] DASH_FACTOR = {1, 1.5f, 2};

    private final int kind;
    private int row;
    private int rowIndex;
    private boolean snatcher;
    private Step step;
    private int sub;
    private int timer;
    private int count;
    private float vx;
    private float vy;
    private float baseY;
    private int worth;
    private boolean boxes = true;
    private boolean shown = true;
    private RescueReploid target;
    private Runnable onBossSoul;

    public NightmareVirus(World w, RecordDef r) {
        super(w, r, "nightmare-virus");
        int id = r.id();
        kind = id & 0xF0;
        row = Math.min(3, id & 0x0F);
        hp = HP;
        contactDamage = 4;
        damageTable = DamageTables.Table.NORMAL;
        facePlayer(w);
        anim.play(0);
        switch (kind) {
            case 0x10 -> startBossSoul(w, id);
            case 0x30 -> startSoul(w, id == REPLOID_SOUL ? 4 : 200, false);
            default -> {
                snatcher = kind == 0x20;
                startStep(w, snatcher ? Step.SNATCH : ROWS[row][0]);
            }
        }
    }

    public static NightmareVirus make(World w, int id, float x, float y) {
        return new NightmareVirus(w, new RecordDef(-1, "NightmareVirus", "main", 0, 15, id, 0, (int) x, (int) y));
    }

    public NightmareVirus onBossSoul(Runnable r) {
        onBossSoul = r;
        return this;
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (step) {
            case IDLE -> updateIdle(w);
            case DASH -> updateDash(w);
            case SHOOT -> updateShoot(w);
            case SNATCH -> updateSnatch(w);
            case SOUL -> updateSoul(w);
            case BOSS_SOUL -> updateBossSoul(w);
            case GONE -> {
            }
        }
        if (!dead && !keep() && w.offScreen(x, y, 64, 64)) {
            despawn(w);
        }
    }

    private boolean keep() {
        return step == Step.SNATCH && sub >= 2 || step == Step.SOUL || step == Step.BOSS_SOUL;
    }

    private void startStep(World w, Step s) {
        step = s;
        sub = 0;
        timer = 0;
        switch (s) {
            case IDLE -> {
                facePlayer(w);
                anim.play(0);
            }
            case DASH -> {
                facePlayer(w);
                anim.play(1);
                count = 6 + w.rng.nextInt(7);
                nextDash(w);
            }
            case SHOOT -> {
                if (!insideView(w)) {
                    startStep(w, Step.IDLE);
                    return;
                }
                facePlayer(w);
                anim.play(4);
                count = SHOTS;
                timer = SHOT_DELAY;
            }
            case SNATCH -> {
                facePlayer(w);
                anim.play(0);
            }
            default -> {
            }
        }
    }

    private void nextStep(World w) {
        if (snatcher) {
            RescueReploid r = findReploid(w);
            if (r != null) {
                target = r;
                step = Step.SNATCH;
                sub = 1;
                anim.play(1);
                return;
            }
        }
        rowIndex = (rowIndex + 1) % ROWS[row].length;
        startStep(w, ROWS[row][rowIndex]);
    }

    private boolean insideView(World w) {
        float cx = w.camera.x();
        float cy = w.camera.y();
        return x >= cx + 16 && x <= cx + 304 && y >= cy + 16 && y <= cy + 224;
    }

    private void updateIdle(World w) {
        facePlayer(w);
        timer++;
        if (timer > 60 && anim.entered(1)) {
            nextStep(w);
        }
    }

    private void nextDash(World w) {
        float[] v = Projectile.aim(x, y, w.player.x, w.player.y, DASH_FACTOR[w.rng.nextInt(3)]);
        vx = v[0];
        vy = v[1];
        timer = 4 + w.rng.nextInt(13);
        faceRight = vx > 0;
    }

    private void updateDash(World w) {
        x += vx;
        y += vy;
        if (--timer > 0) {
            return;
        }
        if (--count > 0) {
            nextDash(w);
        } else {
            nextStep(w);
        }
    }

    private void updateShoot(World w) {
        if (count > 0) {
            if (--timer <= 0) {
                facePlayer(w);
                w.sfx.play(2, 66);
                float ox = x + (faceRight ? 4 : -4);
                float oy = y + 11;
                float[] v = Projectile.aim(ox, oy, w.player.x, w.player.y, ORB_SPEED);
                w.add(new Projectile(w, "nightmare-virus", 5, ox, oy, v[0], v[1], 4, 3, 3)
                        .throughTerrain().priority(3));
                count--;
                timer = count > 0 ? SHOT_DELAY : 40;
            }
            return;
        }
        if (--timer <= 0) {
            nextStep(w);
        }
    }

    private RescueReploid findReploid(World w) {
        RescueReploid best = null;
        double bestD = 1000;
        for (Entity e : w.entities) {
            if (e instanceof RescueReploid r && r.catchable() && !w.offScreen(r.x, r.y, 0, 0)) {
                double d = Math.hypot(r.x - x, r.y - y);
                if (d < bestD) {
                    bestD = d;
                    best = r;
                }
            }
        }
        return best;
    }

    private void updateSnatch(World w) {
        switch (sub) {
            case 0 -> {
                facePlayer(w);
                timer++;
                if (timer > 60 && anim.entered(1)) {
                    RescueReploid r = findReploid(w);
                    if (r == null) {
                        startStep(w, ROWS[row][rowIndex]);
                        return;
                    }
                    target = r;
                    sub = 1;
                    anim.play(1);
                }
            }
            case 1 -> {
                if (target.dead || !target.catchable()) {
                    target = null;
                    nextStep(w);
                    return;
                }
                float tx = target.x;
                float ty = target.y - 28;
                faceRight = tx > x;
                float dx = tx - x;
                float dy = ty - y;
                double d = Math.hypot(dx, dy);
                if (d <= 2) {
                    x = tx;
                    y = ty;
                    anim.play(2);
                    boxes = false;
                    if (record != null) {
                        w.soulsTaken.add(record);
                    }
                    target.infect(w);
                    sub = 2;
                    timer = 0;
                    return;
                }
                x += (float) (dx / d * SNATCH_SPEED);
                y += (float) (dy / d * SNATCH_SPEED);
            }
            case 2 -> {
                timer++;
                if (anim.entered(1) && timer < 20) {
                    w.sfx.play(2, 63);
                }
                if (timer == 76) {
                    w.sfx.play(2, 64);
                }
                if (timer > 76) {
                    shown = (timer & 2) == 0;
                }
                if (timer >= 166) {
                    despawn(w);
                }
            }
            default -> {
            }
        }
    }

    private void startSoul(World w, int firstWorth, boolean bob1) {
        step = Step.SOUL;
        boxes = false;
        baseY = y;
        vy = -8;
        sub = 0;
        timer = 0;
        worth = firstWorth;
        count = bob1 ? 1 : 0;
        anim.play(bob1 ? 6 : 7);
    }

    private void updateSoul(World w) {
        timer++;
        switch (sub) {
            case 0 -> {
                y += vy;
                vy += 0.5f;
                if (vy > 0 && y >= baseY) {
                    y = baseY;
                    sub = count == 1 ? 1 : 2;
                    timer = 0;
                }
            }
            case 1 -> {
                bob();
                if (timer >= 60) {
                    sub = 2;
                    timer = 0;
                    worth = 4;
                    anim.play(7);
                }
            }
            case 2 -> {
                bob();
                if (timer >= 180) {
                    sub = 3;
                    timer = 0;
                    y = baseY;
                }
            }
            default -> {
                if (kind == 0x30) {
                    shown = (timer & 2) == 0;
                    if (timer >= 30) {
                        despawn(w);
                    }
                    return;
                }
                // TODO: the two after-images of the revival (misc type 11) are not drawn; the virus is hidden for their 8 updates (docs/gaps.md, "Nightmare Virus")
                shown = timer > 8 && (timer & 2) == 0;
                if (timer == 9) {
                    anim.play(0);
                }
                if (timer >= 38) {
                    shown = true;
                    boxes = true;
                    hp = HP;
                    row = 2;
                    rowIndex = 0;
                    snatcher = false;
                    startStep(w, Step.IDLE);
                }
                return;
            }
        }
        if (sub <= 2 && pickedUp(w)) {
            w.progress.souls += worth;
            w.sfx.play(0, 41);
            despawn(w);
        }
    }

    private void bob() {
        y = baseY + (float) Math.sin(timer * Math.PI * 2 / 128) * 8;
    }

    private boolean pickedUp(World w) {
        Rect p = w.player.hurtBox();
        return p != null && new Rect(x - 5, y - 5, 10, 10).overlaps(p);
    }

    private void startBossSoul(World w, int id) {
        step = Step.BOSS_SOUL;
        boxes = false;
        worth = id == BOSS_SOUL_FAST ? 200 : 100;
        anim.play(id == BOSS_SOUL_FAST ? 8 : 9);
        x = w.player.x;
        y = w.camera.y() - 32;
        sub = 0;
    }

    private void updateBossSoul(World w) {
        if (sub == 1) {
            return;
        }
        y += 1;
        if (count == 0 && y >= w.camera.y()) {
            count = 1;
            w.sfx.play(0, 43);
        }
        // TODO: the 3 light rays every 40 updates (ray type 13) are not drawn (docs/gaps.md, "Nightmare Virus")
        if (y >= w.player.y) {
            w.progress.souls += worth;
            w.sfx.play(0, 41);
            shown = false;
            sub = 1;
            if (onBossSoul != null) {
                onBossSoul.run();
            }
        }
    }

    @Override
    public void onKilled(World w, int weaponId) {
        w.add(new Explosion(w, x, y));
        if (record == null || w.soulsTaken.contains(record)) {
            despawn(w);
            return;
        }
        w.soulsTaken.add(record);
        startSoul(w, 8, true);
    }

    @Override
    protected void onDeath(World w) {
    }

    @Override
    public boolean isEnemy() {
        return step != Step.BOSS_SOUL;
    }

    @Override
    public Rect hurtBox() {
        return boxes && shown ? new Rect(x - 10, y - 10, 20, 20) : null;
    }

    @Override
    public Rect contactBox() {
        return boxes && shown ? new Rect(x - 12, y - 12, 24, 24) : null;
    }

    @Override
    public boolean visible() {
        return shown;
    }

    @Override
    public int drawPriority() {
        return 3;
    }
}
