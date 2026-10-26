package mmx.x6.game.enemy.scaravich;

import mmx.x6.anim.Animator;
import mmx.x6.anim.BankSheetAnims;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class RockBlock extends Entity implements Solid {

    static final String SET = BankSheetAnims.SCARAVICH_ROCKS;
    private static final int[] DEBRIS = {13, 14, 15, 16, 17, 18};

    private final float halfW;
    private final float halfH;

    public RockBlock(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        int size = r.id() == 4 ? 4 : r.id() == 3 || r.id() == 6 ? 3 : 5;
        this.halfW = size == 4 ? 16 : 32;
        this.halfH = size == 5 ? 16 : 40;
        this.hp = 8;
        this.contactDamage = 127;
        this.damageTable = DamageTables.Table.NORMAL;
        this.anim = new Animator(w.animSet(SET));
        anim.play(size == 3 ? 10 : size == 4 ? 11 : 12);
    }

    public static RockBlock spawn(World w, RecordDef r) {
        if (!w.rng.nextBoolean()) {
            w.spawner.kept(r);
            return null;
        }
        return new RockBlock(w, r);
    }

    @Override
    public void update(World w) {
        anim.tick();
    }

    @Override
    public void onKilled(World w, int weaponId) {
        for (int i = 0; i < 3; i++) {
            w.add(new Explosion(w, x + w.rng.nextInt((int) halfW * 2 + 1) - halfW,
                    y + w.rng.nextInt((int) halfH * 2 + 1) - halfH));
        }
        Debris.spawn(w, SET, DEBRIS, x, y);
        dead = true;
        w.spawner.kept(record);
    }

    @Override
    public Rect solidRect() {
        return dead ? null : new Rect(x - halfW, y - halfH, halfW * 2, halfH * 2);
    }

    @Override
    public boolean carries() {
        return true;
    }

    @Override
    public Rect hurtBox() {
        return solidRect();
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
        return record != null && record.id() == 6 ? 6 : 7;
    }
}
