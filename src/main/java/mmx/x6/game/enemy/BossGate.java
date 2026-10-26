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
    private static final float PUSH_083 = 0.828125f;
    private static final float PUSH_177 = 1.765625f;
    private static final float PUSH_384 = 3.84375f;
    private static final int KEEP = -1;
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
                if (canOpen(id, w) && triggered(p)) {
                    w.lockCount++;
                    w.clearEnemies();
                    p.startGate();
                    w.camera.easeSpeed = 2;
                    w.sfx.play(0, 16);
                    step = GateStep.OPEN;
                    onTrigger(w);
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
                p.pushGate(push(id));
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

    private void onTrigger(World w) {
        if (id == 7) {
            Entity next = w.spawner.createExtra(w, "NightmareSnake", 25, 4, (int) x, (int) y);
            if (next != null) {
                w.add(next);
            }
        }
    }

    public static boolean canOpen(int id, World w) {
        return switch (id) {
            case 7, 8, 10, 12, 14, 15 -> w.flag26;
            case 34 -> w.flag28;
            case 44 -> !w.flag26;
            default -> true;
        };
    }

    public static float push(int id) {
        return switch (id) {
            case 3, 5, 37, 40, 46 -> PUSH_083;
            case 10, 12 -> PUSH_384;
            case 44 -> PUSH_177;
            default -> PUSH;
        };
    }

    public static int[] limits(int id, int gx, int gy) {
        int[] l = {gx, gx, KEEP, KEEP};
        switch (id) {
            case 1 -> l[1] = gx + 80;
            case 2 -> set(l, gx, gx, 224, 224);
            case 3, 40 -> set(l, gx - 48, gx - 48, gy - 64, gy - 64);
            case 4, 22, 33 -> l[1] = gx + 32;
            case 5 -> set(l, 6016, 6016, 304, 304);
            case 6 -> set(l, gx, 6384, 304, 320);
            case 7 -> set(l, 944, 976, gy - 160, 2624);
            case 8 -> set(l, gx - 16, gx - 16, 1616, KEEP);
            case 9 -> set(l, gx, 2416, 2528, 2816);
            case 10 -> set(l, 2544, 2544, 944, KEEP);
            case 11 -> set(l, gx, 2416, 2080, 2304);
            case 12 -> set(l, 2544, 2544, 2160, KEEP);
            case 13 -> set(l, gx - 16, gx + 16, KEEP, KEEP);
            case 14 -> set(l, gx, 3504, KEEP, 1184);
            case 15 -> set(l, gx - 16, gx - 16, KEEP, 944);
            case 16 -> set(l, gx, gx + 32, 944, 944);
            case 17, 21, 32, 42 -> set(l, gx - 16, gx - 16, KEEP, KEEP);
            case 19 -> set(l, 1264, 1264, 1104, 1184);
            case 20 -> set(l, gx, 1680, 1136, 1184);
            case 23 -> set(l, gx, 9040, 640, 688);
            case 24 -> set(l, gx, gx, KEEP, 480);
            case 25 -> l[1] = 7360;
            case 26 -> set(l, gx, 8096, KEEP, 160);
            case 27 -> set(l, gx, gx + 16, 832, 928);
            case 28 -> set(l, 2880, 2880, KEEP, KEEP);
            case 29 -> set(l, gx, 3216, 224, 224);
            case 30 -> set(l, gx - 48, gy - 48, KEEP, KEEP);
            case 31 -> set(l, gx, gx + 32, gy - 144, KEEP);
            case 35, 38, 45, 47, 49, 51 -> set(l, gx, gx + 32, gy - 160, gy - 160);
            case 37 -> set(l, 2848, 2848, KEEP, KEEP);
            case 39 -> set(l, gx, gx, 0, 0);
            case 41, 43 -> set(l, gx, gx + 32, gy - 112, gy - 112);
            case 44 -> set(l, gx - 112, gx - 112, KEEP, KEEP);
            case 46 -> set(l, gx - 48, gx - 48, KEEP, KEEP);
            case 50 -> set(l, gx, gx, gy - 160, gy - 160);
            default -> {
            }
        }
        return l;
    }

    private static void set(int[] l, int minX, int maxX, int minY, int maxY) {
        l[0] = minX;
        l[1] = maxX;
        l[2] = minY;
        l[3] = maxY;
    }

    private void setLimits(World w) {
        int[] l = limits(id, (int) x, (int) y);
        targetMinX = l[0];
        w.camera.setTargets(l[0], l[1], l[2], l[3]);
    }

    @Override
    public Rect solidRect() {
        boolean closed = step == GateStep.WAIT || step == GateStep.CLOSED && id != 20;
        return closed ? new Rect(x - HALF_W, y - HALF_H, 32, 64) : null;
    }

    @Override
    public boolean skipsWhileSqueezed() {
        return false;
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
