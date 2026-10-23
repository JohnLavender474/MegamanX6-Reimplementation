package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Explosion;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class JunkroidShot extends Entity {

    private final float speedX;

    public JunkroidShot(World w, float x, float y, boolean faceRight) {
        this.x = x;
        this.y = y;
        this.faceRight = faceRight;
        this.speedX = faceRight ? 2 : -2;
        hp = 1;
        contactDamage = 1;
        damageTable = DamageTables.Table.MINOR;
        anim = new Animator(w.animSet("junkroid"));
        anim.play(6);
    }

    @Override
    public void update(World w) {
        anim.tick();
        x += speedX;
        if (TileRules.isObjectSolid(w.grid.at(x, y)) || hitsSolid(w)) {
            dead = true;
            w.add(Explosion.spark(x, y));
            return;
        }
        if (w.offScreen(x, y, 32, 32)) {
            dead = true;
        }
    }

    private boolean hitsSolid(World w) {
        Rect box = hurtBox();
        return w.solids().stream().anyMatch(box::overlaps);
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - 4, y - 4, 8, 8);
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
        w.add(Explosion.spark(x, y));
    }

    @Override
    public boolean clearedByGate() {
        return true;
    }

    @Override
    public String spriteSet() {
        return "junkroid";
    }

    @Override
    public int drawPriority() {
        return 4;
    }
}
