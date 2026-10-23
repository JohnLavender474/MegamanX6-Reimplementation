package mmx.x6.game.player;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;

public final class ChargeFlash extends Entity {

    private final Player player;
    private final int weaponId;
    private final boolean startFacing;

    public ChargeFlash(World w, Player player, int weaponId) {
        this.player = player;
        this.weaponId = weaponId;
        this.startFacing = player.faceRight;
        this.faceRight = player.faceRight;
        this.x = player.busterX();
        this.y = player.busterY();
        this.anim = new Animator(w.animSet("shots"));
        anim.play(weaponId == 9 ? 22 : 25);
        w.sfx.play(1, weaponId == 9 ? 9 : 10);
    }

    public int weaponId() {
        return weaponId;
    }

    @Override
    public void update(World w) {
        if (player.step() == PlayerStep.EVENT_HOLD) {
            dead = true;
            return;
        }
        if (player.faceRight == startFacing && player.shooting()) {
            x = player.busterX();
            y = player.busterY();
        }
        anim.tick();
        if (anim.flagEntered()) {
            float forward = weaponId == 9 ? 17 : 14;
            w.add(new BusterShot(w, weaponId, x + (faceRight ? forward : -forward), y, faceRight));
            dead = true;
        }
    }

    @Override
    public String spriteSet() {
        return "shots";
    }

    @Override
    public int drawPriority() {
        return -3;
    }
}
