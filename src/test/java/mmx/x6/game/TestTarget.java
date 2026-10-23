package mmx.x6.game;

import mmx.x6.physics.Rect;

public final class TestTarget extends Entity {

    public int hurtCount;
    public int killCount;
    public final float w;
    public final float h;

    public TestTarget(float x, float y, float w, float h, int hp, DamageTables.Table table) {
        this.x = x;
        this.y = y;
        this.w = w;
        this.h = h;
        this.hp = hp;
        this.damageTable = table;
    }

    @Override
    public void update(World world) {
    }

    @Override
    public Rect hurtBox() {
        return new Rect(x - w / 2, y - h / 2, w, h);
    }

    @Override
    public void onHurt(World world, int weaponId) {
        hurtCount++;
    }

    @Override
    public void onKilled(World world, int weaponId) {
        killCount++;
        dead = true;
    }
}
