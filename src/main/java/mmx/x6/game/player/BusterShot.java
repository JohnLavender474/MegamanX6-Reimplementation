package mmx.x6.game.player;

import java.util.HashSet;
import java.util.Set;
import mmx.x6.anim.Animator;
import mmx.x6.game.Damage;
import mmx.x6.game.Entity;
import mmx.x6.game.Weapon;
import mmx.x6.game.World;
import mmx.x6.game.fx.Explosion;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class BusterShot extends Entity implements Weapon {

    private static final float DEFLECT_SPEED_Y = 4.8f;

    private final int weaponId;
    private final Set<Entity> hit = new HashSet<>();
    private float speedX;
    private float speedY;
    public boolean deflected;

    public BusterShot(World w, int weaponId, float x, float y, boolean faceRight) {
        this.weaponId = weaponId;
        this.x = x;
        this.y = y;
        this.faceRight = faceRight;
        float speed = switch (weaponId) {
            case 9 -> 8.0f;
            case 19 -> 10.0f;
            default -> 6.75f;
        };
        this.speedX = faceRight ? speed : -speed;
        this.anim = new Animator(w.animSet("shots"));
        anim.play(switch (weaponId) {
            case 9 -> 26;
            case 19 -> 27;
            default -> 21;
        });
    }

    @Override
    public int weaponId() {
        return weaponId;
    }

    public float speedX() {
        return speedX;
    }

    public float speedY() {
        return speedY;
    }

    @Override
    public void update(World w) {
        anim.tick();
        x += speedX;
        y -= speedY;
        if (!deflected && TileRules.isObjectSolid(w.grid.at(x, y))) {
            dead = true;
            w.add(Explosion.spark(x, y));
            if (weaponId == 0) {
                w.sfx.play(1, 12);
            }
            return;
        }
        float mx = weaponId == 0 ? 12 : 32;
        float my = weaponId == 0 ? 8 : weaponId == 9 ? 20 : 32;
        if (w.offScreen(x, y, mx, my)) {
            dead = true;
        }
    }

    @Override
    public Rect attackBox() {
        if (deflected) {
            return null;
        }
        return switch (weaponId) {
            case 9 -> Rect.ofDoc(x, y, -13, -7, 29, 13, faceRight);
            // TODO: shot 19 has no exported art or box; it uses the shot 18 box (docs/gaps.md, "Shot sprites")
            case 19 -> Rect.ofDoc(x, y, -34, -19, 64, 36, faceRight);
            default -> Rect.ofDoc(x, y, -10, -6, 18, 11, faceRight);
        };
    }

    @Override
    public boolean canHit(Entity target) {
        return !deflected && !hit.contains(target);
    }

    @Override
    public void onResult(World w, Entity target, int result) {
        hit.add(target);
        if (result == Damage.BLOCKED) {
            deflected = true;
            speedX = -speedX;
            speedY = w.rng.nextBoolean() ? DEFLECT_SPEED_Y : -DEFLECT_SPEED_Y;
            return;
        }
        boolean kill = result < 0;
        if (weaponId == 9 && kill) {
            return;
        }
        dead = true;
    }

    @Override
    public String spriteSet() {
        return "shots";
    }

    @Override
    public int drawPriority() {
        return -2;
    }
}
