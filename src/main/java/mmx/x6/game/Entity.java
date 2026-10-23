package mmx.x6.game;

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
}
