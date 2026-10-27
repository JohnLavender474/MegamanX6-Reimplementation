package mmx.x6.game;

import java.util.List;
import mmx.x6.anim.Animator;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public abstract class Entity {

    public float x;
    public float y;
    public float prevX;
    public float prevY;
    public boolean faceRight;
    public int hp;
    public int contactDamage;
    public int hitClass;
    public RecordDef record;
    public boolean dead;
    public DamageTables.Table damageTable;
    public int flash;
    protected Animator anim;

    public abstract void update(World w);

    public Rect hurtBox() {
        return null;
    }

    public Rect contactBox() {
        return null;
    }

    public Animator anim() {
        return anim;
    }

    public String spriteSet() {
        return null;
    }

    public int drawPriority() {
        return 0;
    }

    public boolean visible() {
        return true;
    }

    public float damageTakenScale(int weaponId) {
        return 1;
    }

    public boolean hitBeforeOthers() {
        return false;
    }

    public void onHurt(World w, int weaponId) {
    }

    public void onKilled(World w, int weaponId) {
        dead = true;
    }

    public void onDeflect(World w, int weaponId) {
    }

    public void onTouchPlayer(World w) {
    }

    public boolean isEnemy() {
        return false;
    }

    public boolean clearedByGate() {
        return isEnemy();
    }

    public boolean runsInPause() {
        return false;
    }

    public List<ShapeFx> shapes() {
        return List.of();
    }
}
