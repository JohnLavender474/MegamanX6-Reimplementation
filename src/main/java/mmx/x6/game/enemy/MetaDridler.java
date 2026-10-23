package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.level.TileLayer;
import mmx.x6.physics.Body;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TerrainMover;

public final class MetaDridler extends Enemy {

    public enum Step { WAIT, DROP, DRILL, RISE }

    private static final int WAIT_TIME = 90;
    private static final int WARNING = 30;
    private static final float DROP_START = 1f / 16f;
    private static final float DROP_GRAVITY = 1f / 8f;
    private static final float MAX_DROP = 128;
    private static final int DRILL_TIME = 60;
    private static final float RISE_SPEED = 0.5f;
    private static final int RISE_SOUND_EVERY = 60;

    private final Body body;
    private final float homeY;
    private final Effect glow;
    private final Effect warning;
    private Step step = Step.WAIT;
    private int timer;
    private float speedY;

    public MetaDridler(World w, RecordDef r) {
        super(w, r, "meta-dridler");
        hp = 1;
        contactDamage = 8;
        damageTable = DamageTables.Table.DEFLECT;
        faceRight = false;
        homeY = y;
        body = new Body(x, y, 0, 6, 11, 32);
        offMarginX = 80;
        offMarginY = 160;
        anim.play(0);
        glow = new Effect(w, this);
        warning = new Effect(w, this);
        w.add(glow);
        w.add(warning);
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        timer++;
        switch (step) {
            case WAIT -> {
                anim.playIfNot(0);
                glow.show(1);
                warning.show(timer >= WAIT_TIME - WARNING ? 2 : -1);
                if (timer >= WAIT_TIME) {
                    step = Step.DROP;
                    timer = 0;
                    speedY = DROP_START;
                    w.sfx.play(2, 1);
                }
            }
            case DROP -> {
                anim.playIfNot(3);
                glow.show(4);
                warning.show(2);
                body.x = x;
                body.y = y;
                int f = w.mover.move(body, 0, -speedY, false, w.solids());
                y = body.y;
                speedY -= DROP_GRAVITY;
                boolean onFloor = (f & TerrainMover.FLOOR) != 0;
                if (onFloor || y - homeY >= MAX_DROP) {
                    if (!onFloor) {
                        y = homeY + MAX_DROP;
                    } else if (record.id() >= 1 && record.id() <= 3) {
                        dropRock(w);
                    }
                    step = Step.DRILL;
                    timer = 0;
                    w.sfx.play(2, 2);
                }
            }
            case DRILL -> {
                glow.show(-1);
                warning.show(timer >= DRILL_TIME - WARNING ? 5 : -1);
                if (timer >= DRILL_TIME) {
                    step = Step.RISE;
                    timer = 0;
                    w.sfx.play(2, 3);
                }
            }
            case RISE -> {
                anim.playIfNot(6);
                glow.show(1);
                warning.show(5);
                y -= RISE_SPEED;
                if (timer % RISE_SOUND_EVERY == 0) {
                    w.sfx.play(2, 3);
                }
                if (y <= homeY) {
                    y = homeY;
                    step = Step.WAIT;
                    timer = 0;
                }
            }
        }
        anim.tick();
        if (checkOffScreen(w)) {
            glow.dead = true;
            warning.dead = true;
        }
    }

    private void dropRock(World w) {
        int[][] places = {{4640, 880}, {4864, 880}, {4928, 880}};
        int[] p = places[record.id() - 1];
        int dstX = p[0] / 16;
        int dstY = p[1] / 16;
        int srcY = (p[1] + 256) / 16;
        TileLayer art = w.level.art0Main();
        for (int cy = 0; cy < 3; cy++) {
            for (int cx = 0; cx < 4; cx++) {
                w.grid.set(dstX + cx, dstY + cy, w.grid.raw(dstX + cx, srcY + cy));
                art.setGid(dstX + cx, dstY + cy, art.gid(dstX + cx, srcY + cy));
            }
        }
        Explosion.around(w, x, y + 32, 4, 32);
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 16, y - 40, 32, 80);
    }

    @Override
    public Rect contactBox() {
        return new Rect(x - 14, y - 40, 28, 80);
    }

    @Override
    public int drawPriority() {
        return 33;
    }

    @Override
    protected void onDeath(World w) {
    }

    @Override
    public void onKilled(World w, int weaponId) {
        hp = 1;
    }

    static final class Effect extends Entity {
        private final MetaDridler parent;
        private int shown = -1;

        Effect(World w, MetaDridler parent) {
            this.parent = parent;
            this.anim = new Animator(w.animSet("meta-dridler"));
            this.x = parent.x;
            this.y = parent.y;
        }

        void show(int animIndex) {
            if (animIndex != shown) {
                shown = animIndex;
                if (animIndex >= 0) {
                    anim.play(animIndex);
                }
            }
        }

        @Override
        public void update(World w) {
            x = parent.x;
            y = parent.y;
            faceRight = parent.faceRight;
            if (parent.dead) {
                dead = true;
            }
            anim.tick();
        }

        @Override
        public boolean visible() {
            return shown >= 0;
        }

        @Override
        public String spriteSet() {
            return "meta-dridler";
        }

        @Override
        public int drawPriority() {
            return 32;
        }
    }
}
