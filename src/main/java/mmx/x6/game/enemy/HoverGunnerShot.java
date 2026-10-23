package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Explosion;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class HoverGunnerShot extends Entity {

    private static final float SPEED = 2;

    private final float vx;
    private final float vy;

    public HoverGunnerShot(World w, float x, float y, int direction16) {
        this.x = x;
        this.y = y;
        double a = direction16 * Math.PI * 2 / 16;
        this.vx = (float) Math.cos(a) * SPEED;
        this.vy = (float) Math.sin(a) * SPEED;
        hp = 1;
        contactDamage = 3;
        damageTable = DamageTables.Table.MINOR;
        anim = new Animator(w.animSet("hover-gunner"));
        anim.play(6);
    }

    public float vx() {
        return vx;
    }

    public float vy() {
        return vy;
    }

    @Override
    public void update(World w) {
        anim.tick();
        x += vx;
        y += vy;
        if (TileRules.isObjectSolid(w.grid.at(x, y)) || w.solids().stream().anyMatch(hurtBox()::overlaps)) {
            dead = true;
            w.add(Explosion.spark(x, y));
            return;
        }
        if (w.offScreen(x, y, 32, 32)) {
            dead = true;
        }
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 3, y - 6, 8, 8);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public void onKilled(World w, int weaponId) {
        dead = true;
        w.add(Explosion.spark(x, y));
    }

    @Override
    public void onTouchPlayer(World w) {
        dead = true;
    }

    @Override
    public String spriteSet() {
        return "hover-gunner";
    }

    @Override
    public int drawPriority() {
        return 4;
    }
}
