package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.DamageTables;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.game.fx.Explosion;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public class Projectile extends Entity {

    public float vx;
    public float vy;
    private final String set;
    private final float halfW;
    private final float halfH;
    private float gravity;
    private float fallMax = Float.MAX_VALUE;
    private boolean terrain = true;
    private boolean endsOnPlayer;
    private boolean explodes;
    private int margin = 32;
    private int hidden;
    private int life = -1;
    private int priority = 4;
    protected int age;

    public Projectile(World w, String set, int anim, float x, float y, float vx, float vy, int damage,
                      float halfW, float halfH) {
        this.set = set;
        this.x = x;
        this.y = y;
        this.prevX = x;
        this.prevY = y;
        this.vx = vx;
        this.vy = vy;
        this.contactDamage = damage;
        this.halfW = halfW;
        this.halfH = halfH;
        this.faceRight = vx > 0;
        this.anim = new Animator(w.animSet(set));
        this.anim.play(anim);
    }

    public static float[] aim(float fromX, float fromY, float toX, float toY, float speed) {
        double a = Math.atan2(toY - fromY, toX - fromX);
        return new float[] {(float) Math.cos(a) * speed, (float) Math.sin(a) * speed};
    }

    public Projectile throughTerrain() {
        terrain = false;
        return this;
    }

    public Projectile endsOnPlayer() {
        endsOnPlayer = true;
        return this;
    }

    public Projectile hittable(int hp, DamageTables.Table table) {
        this.hp = hp;
        this.damageTable = table;
        return this;
    }

    public Projectile explodes() {
        explodes = true;
        return this;
    }

    public Projectile gravity(float g, float max) {
        gravity = g;
        fallMax = max;
        return this;
    }

    public Projectile margin(int m) {
        margin = m;
        return this;
    }

    public Projectile hidden(int updates) {
        hidden = updates;
        return this;
    }

    public Projectile life(int updates) {
        life = updates;
        return this;
    }

    public Projectile priority(int p) {
        priority = p;
        return this;
    }

    public Projectile facing(boolean right) {
        faceRight = right;
        return this;
    }

    @Override
    public void update(World w) {
        age++;
        anim.tick();
        if (hidden > 0) {
            hidden--;
            return;
        }
        vy = Math.min(fallMax, vy + gravity);
        x += vx;
        y += vy;
        if (terrain && TileRules.isObjectSolid(w.grid.at(x, y))) {
            end(w);
            return;
        }
        if (life > 0 && --life == 0 || w.offScreen(x, y, margin, margin)) {
            dead = true;
        }
    }

    protected void end(World w) {
        dead = true;
        if (explodes) {
            w.add(new Explosion(w, x, y));
        } else {
            w.add(Explosion.spark(x, y));
        }
    }

    @Override
    public Rect contactBox() {
        return hidden > 0 || contactDamage <= 0 ? null : box();
    }

    private Rect box() {
        return new Rect(x - halfW, y - halfH, halfW * 2, halfH * 2);
    }

    @Override
    public Rect hurtBox() {
        return damageTable == null || hidden > 0 ? null : box();
    }

    @Override
    public void onKilled(World w, int weaponId) {
        end(w);
    }

    @Override
    public void onTouchPlayer(World w) {
        if (endsOnPlayer) {
            end(w);
        }
    }

    @Override
    public boolean visible() {
        return hidden == 0;
    }

    @Override
    public boolean clearedByGate() {
        return true;
    }

    @Override
    public String spriteSet() {
        return set;
    }

    @Override
    public int drawPriority() {
        return priority;
    }
}
