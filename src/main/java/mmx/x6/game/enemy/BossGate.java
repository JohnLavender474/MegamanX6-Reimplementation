package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.game.player.Player;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class BossGate extends Entity implements Solid {

    public enum GateStep { WAIT, OPEN, PASS, CLOSE, CLOSED }

    private static final float PUSH = 0.640625f;
    private static final float HALF_W = 16;
    private static final float HALF_H = 32;

    private final int id;
    private GateStep step = GateStep.WAIT;
    private int targetMinX;

    public BossGate(World w, RecordDef r) {
        this.record = r;
        this.id = r.id() & 0x3F;
        this.x = r.x();
        this.y = r.y();
        this.anim = new Animator(w.animSet("boss-gate"));
        anim.play(0);
    }

    public GateStep step() {
        return step;
    }

    @Override
    public void update(World w) {
        Player p = w.player;
        switch (step) {
            case WAIT -> {
                if (triggered(p)) {
                    w.lockCount++;
                    w.clearEnemies();
                    p.startGate();
                    w.camera.easeSpeed = 2;
                    w.music.play(28);
                    w.sfx.play(0, 16);
                    step = GateStep.OPEN;
                }
            }
            case OPEN -> {
                anim.tick();
                if (anim.entered(1)) {
                    w.sfx.play(0, 17);
                }
                if (anim.entered(2)) {
                    setLimits(w);
                    step = GateStep.PASS;
                }
            }
            case PASS -> {
                p.pushGate(PUSH);
                if (w.camera.x() >= targetMinX) {
                    p.endGate();
                    w.sfx.play(0, 17);
                    anim.play(1);
                    step = GateStep.CLOSE;
                }
            }
            case CLOSE -> {
                anim.tick();
                if (anim.entered(1)) {
                    w.sfx.play(0, 16);
                }
                if (anim.entered(2)) {
                    step = GateStep.CLOSED;
                    w.lockCount = Math.max(0, w.lockCount - 1);
                }
            }
            case CLOSED -> {
            }
        }
    }

    private boolean triggered(Player p) {
        if (p.dying() || p.invulnerable() || p.locked() || !p.grounded()) {
            return false;
        }
        Rect box = p.body().box();
        boolean touching = p.x <= x && Math.abs(p.body().centerX() - x) <= HALF_W + p.body().boxHalfW + 0.5f;
        boolean inside = box.y() >= y - HALF_H - 0.01f && box.bottom() <= y + HALF_H + 0.01f;
        return touching && inside;
    }

    private void setLimits(World w) {
        int gx = (int) x;
        // TODO: only the two intro gate ids are mapped; the other 49 cases of the table are not (docs/gaps.md, "Boss gates")
        if (id == 1) {
            targetMinX = gx;
            w.camera.setTargets(gx, gx + 80, -1, -1);
        } else {
            targetMinX = gx;
            w.camera.setTargets(gx, gx, 224, 224);
        }
    }

    @Override
    public Rect solidRect() {
        return step == GateStep.WAIT || step == GateStep.CLOSED ? new Rect(x - HALF_W, y - HALF_H, 32, 64) : null;
    }

    @Override
    public String spriteSet() {
        return "boss-gate";
    }

    @Override
    public int drawPriority() {
        return 6;
    }
}
