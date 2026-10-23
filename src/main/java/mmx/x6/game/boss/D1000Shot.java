package mmx.x6.game.boss;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Explosion;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class D1000Shot extends Entity {

    private static final float SPEED = 2;

    private final float vx;
    private final float vy;

    public D1000Shot(World w, float x, float y, float targetX, float targetY) {
        this.x = x;
        this.y = y;
        double a = Math.atan2(targetY - y, targetX - x);
        this.vx = (float) Math.cos(a) * SPEED;
        this.vy = (float) Math.sin(a) * SPEED;
        hp = 1;
        contactDamage = 4;
        damageTable = DamageTables.Table.MINOR;
        anim = new Animator(w.animSet("d1000"));
        anim.play(19);
    }

    @Override
    public void update(World w) {
        anim.tick();
        x += vx;
        y += vy;
        if (TileRules.isObjectSolid(w.grid.at(x, y))) {
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
        return new Rect(x - 8, y - 8, 16, 16);
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
    public boolean clearedByGate() {
        return true;
    }

    @Override
    public String spriteSet() {
        return "d1000";
    }

    @Override
    public int drawPriority() {
        return 2;
    }
}
