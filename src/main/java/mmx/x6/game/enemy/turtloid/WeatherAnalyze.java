package mmx.x6.game.enemy.turtloid;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;

public final class WeatherAnalyze extends Entity {

    static final String SET = "weather-analyze";
    private static final int[][] POS = {
            {1574, 592}, {2848, 384}, {7648, 384}, {8576, 672}, {854, 864}, {3168, 1488}, {4144, 1584}};
    private static final int[][] ZONE = {
            {1248, 1631}, {1872, 2879}, {3584, 7695}, {7792, 8655}, {0, 1119}, {1920, 3199}, {3328, 4207}};
    private static final int[][] SENSORS = {
            {1296, 448, 1584, 448, 1392, 576, 1472, 640},
            {2536, 528, 1928, 384, 2528, 64, 2816, 256},
            {4144, 384, 4992, 384, 6064, 384, 7520, 400},
            {7888, 704, 7888, 848, 8560, 528, 8560, 832},
            {1037, 299, 1037, 595, 47, 413, 47, 816},
            {2143, 1567, 2688, 1584, 3040, 1544, 3095, 1583},
            {3584, 1488, 3632, 1648, 3792, 1616, 3984, 1472}};

    private final int index;
    private final Body body;
    private boolean landed;
    private boolean wrecked;
    private float speedY;
    int sensors;

    public WeatherAnalyze(World w, RecordDef r) {
        this.record = r;
        this.index = Math.min(6, r.id() & 0x0F);
        this.x = POS[index][0];
        this.y = POS[index][1];
        this.hp = 32;
        this.damageTable = DamageTables.Table.NORMAL;
        this.body = new Body(x, y, 0, 0, 20, 32);
        this.anim = new Animator(w.animSet(SET));
        anim.play(0);
        w.flag26 = true;
    }

    public boolean wrecked() {
        return wrecked;
    }

    @Override
    public void update(World w) {
        anim.tick();
        if (wrecked) {
            return;
        }
        float px = w.player.x;
        w.flag26 = px >= ZONE[index][0] && px <= ZONE[index][1];
        if (!landed) {
            body.x = x;
            body.y = y;
            speedY = Math.min(6.5f, speedY + 4);
            int f = w.mover.move(body, 0, speedY, false, w.solids());
            y = body.y;
            if ((f & TerrainMover.FLOOR) != 0) {
                landed = true;
                sensors = 4;
                for (int i = 0; i < 4; i++) {
                    w.add(new Sensor(w, this, SENSORS[index][i * 2], SENSORS[index][i * 2 + 1]));
                }
                w.add(new Overlay(w, this, false));
                w.add(new Overlay(w, this, true));
            }
        }
    }

    @Override
    public void onKilled(World w, int weaponId) {
        hp = 0;
        wrecked = true;
        flash = 0;
        switch (index) {
            case 0 -> w.camera.setTargets(-1, 2272, -1, -1);
            case 1 -> {
                w.camera.setTargets(-1, 7360, 224, -1);
                w.copyTiles(2832, 512, 1, 20, 2592, 96);
                w.copyTiles(2944, 512, 1, 22, 2832, 0);
            }
            case 2 -> w.camera.setTargets(-1, 8288, -1, -1);
            case 3 -> w.flag28 = true;
            case 5 -> w.camera.setTargets(w.camera.targetMaxX(), 3840, -1, -1);
            default -> {
            }
        }
        w.flag26 = false;
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, SET, new int[] {14, 15, 16, 17}, x, y);
        anim.play(13);
    }

    @Override
    public Rect hurtBox() {
        return landed && sensors == 0 && !wrecked ? new Rect(x - 16, y - 32, 32, 64) : null;
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

    private static final class Sensor extends Entity {

        private final WeatherAnalyze machine;

        Sensor(World w, WeatherAnalyze machine, float x, float y) {
            this.machine = machine;
            this.x = x;
            this.y = y;
            this.hp = 16;
            this.contactDamage = 4;
            this.damageTable = DamageTables.Table.NORMAL;
            this.anim = new Animator(w.animSet(SET));
            anim.play(9);
        }

        @Override
        public void update(World w) {
            if (machine.wrecked) {
                dead = true;
            }
        }

        @Override
        public void onKilled(World w, int weaponId) {
            dead = true;
            machine.sensors = Math.max(0, machine.sensors - 1);
            w.add(new Explosion(w, x, y));
            Debris.spawn(w, SET, new int[] {10, 11, 12}, x, y);
        }

        @Override
        public Rect hurtBox() {
            return new Rect(x - 6, y - 6, 12, 12);
        }

        @Override
        public Rect contactBox() {
            return hurtBox();
        }

        @Override
        public boolean isEnemy() {
            return true;
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
    }

    private static final class Overlay extends Entity {

        private final WeatherAnalyze machine;
        private final boolean barrier;
        private int shown = -1;

        Overlay(World w, WeatherAnalyze machine, boolean barrier) {
            this.machine = machine;
            this.barrier = barrier;
            this.anim = new Animator(w.animSet(SET));
        }

        @Override
        public void update(World w) {
            x = machine.x;
            y = machine.y;
            int n = machine.sensors;
            if (n == 0 || machine.wrecked) {
                dead = true;
                return;
            }
            if (n != shown) {
                shown = n;
                anim.play((barrier ? 9 : 5) - n);
            }
            anim.tick();
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
            return barrier ? 5 : 4;
        }
    }
}
