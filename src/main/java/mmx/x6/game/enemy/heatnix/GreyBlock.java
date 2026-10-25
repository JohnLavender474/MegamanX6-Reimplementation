package mmx.x6.game.enemy.heatnix;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class GreyBlock extends Entity implements Solid {

    public enum Step { FALL, REST, RISE, DYING }

    private static final int NIGHTMARE_IRON = 4;

    private final float startY;
    private final int id;
    private Step step = Step.FALL;
    private int timer;

    public GreyBlock(World w, RecordDef r) {
        this.record = r;
        this.id = r.id();
        this.x = r.x();
        this.y = r.y();
        this.startY = y;
        this.hp = 16;
        this.contactDamage = 8;
        this.damageTable = DamageTables.Table.GREY_BLOCK;
        this.anim = new Animator(w.animSet("grey-block"));
        anim.play(0);
    }

    public Step step() {
        return step;
    }

    @Override
    public void update(World w) {
        if (w.stage.nightmareEffect() != NIGHTMARE_IRON || w.checkpoint >= id) {
            dead = true;
            w.spawner.kept(record);
            return;
        }
        if (w.offScreen(x, y, 64, 240)) {
            return;
        }
        anim.tick();
        switch (step) {
            case FALL -> {
                if (anim.flag() == 2) {
                    y += 4;
                    if (onFloor(w)) {
                        y = (float) Math.floor((y + 24) / 16) * 16 - 24;
                        w.sfx.play(2, 82);
                        // TODO: the landing's Rumble(5) and Quake2(90, 2, 2) screen shake are not done (docs/gaps.md, "Screen shake")
                        step = Step.REST;
                        timer = 181;
                    }
                }
            }
            case REST -> {
                if (--timer <= 0) {
                    step = Step.RISE;
                }
            }
            case RISE -> {
                y -= 3;
                if (y <= startY) {
                    y = startY;
                    step = Step.FALL;
                }
            }
            case DYING -> {
                if (++timer >= 60) {
                    for (int i = 0; i < 4; i++) {
                        w.add(new Explosion(w, x + w.rng.nextInt(33) - 16, y + w.rng.nextInt(33) - 16));
                    }
                    dead = true;
                    w.spawner.released(record);
                }
            }
        }
    }

    private boolean onFloor(World w) {
        float bottom = y + 24;
        for (float px = x - 28; px <= x + 28; px += 14) {
            if (TileRules.isObjectSolid(w.grid.at(px, bottom))) {
                return true;
            }
        }
        return false;
    }

    @Override
    public void onKilled(World w, int weaponId) {
        w.add(new Explosion(w, x, y));
        step = Step.DYING;
        timer = 0;
    }

    @Override
    public Rect solidRect() {
        if (anim.flag() == 0 || step == Step.DYING) {
            return null;
        }
        return new Rect(x - 32, y - 24, 64, 48);
    }

    @Override
    public boolean carries() {
        return true;
    }

    @Override
    public Rect hurtBox() {
        return step == Step.DYING ? null : new Rect(x - 32, y - 24, 64, 48);
    }

    @Override
    public boolean visible() {
        return step != Step.DYING || (timer & 1) == 0;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public String spriteSet() {
        return "grey-block";
    }

    @Override
    public int drawPriority() {
        return 7;
    }
}
