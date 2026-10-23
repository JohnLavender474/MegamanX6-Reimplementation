package mmx.x6.game.enemy;

import mmx.x6.game.DamageTables;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class SabreBlock extends Enemy implements Solid {

    public SabreBlock(World w, RecordDef r) {
        super(w, r, "sabre-block");
        y = r.y() - 8;
        prevY = y;
        hp = 32;
        contactDamage = 0;
        damageTable = DamageTables.Table.SABRE_ONLY;
        anim.play(0);
    }

    @Override
    public void update(World w) {
        checkOffScreen(w);
    }

    @Override
    public Rect solidRect() {
        return new Rect(x - 32, y - 32, 64, 64);
    }

    @Override
    public Rect hurtBox() {
        return solidRect();
    }

    @Override
    public boolean isEnemy() {
        return false;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }

    @Override
    public int drawPriority() {
        return 7;
    }

    @Override
    public void onKilled(World w, int weaponId) {
        dead = true;
        w.spawner.kept(record);
        onDeath(w);
    }

    @Override
    protected void onDeath(World w) {
        for (int i = 0; i < 5; i++) {
            float dx = (w.rng.nextBoolean() ? 16 : 0) * (w.rng.nextBoolean() ? 1 : -1);
            float dy = (w.rng.nextBoolean() ? 16 : 0) * (w.rng.nextBoolean() ? 1 : -1);
            w.add(new Explosion(w, x + dx, y + dy));
        }
        Debris.spawn(w, "sabre-block", new int[] {1, 2, 3}, x, y);
        w.add(new Explosion(w, x, y));
    }
}
