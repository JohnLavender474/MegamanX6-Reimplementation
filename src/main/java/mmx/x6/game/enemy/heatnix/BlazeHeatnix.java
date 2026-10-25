package mmx.x6.game.enemy.heatnix;

import java.util.List;
import mmx.x6.game.DamageTables;
import mmx.x6.game.ShapeFx;
import mmx.x6.game.World;
import mmx.x6.game.boss.Maverick;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class BlazeHeatnix extends Maverick {

    private static final float SPEED = 1;

    private final float leftX;
    private final float rightX;
    private final float baseY;
    private int sub;
    private int timer;
    private float vx = -SPEED;

    public BlazeHeatnix(World w, RecordDef r) {
        // TODO: Blaze Heatnix (main type 52) has no behaviour sheet and no sprite export (no save state in his room); this stand-in only hovers, takes hits and ends the stage (docs/gaps.md, "Blaze Heatnix")
        super(w, r, null, 32, DamageTables.Table.NORMAL);
        x = r.x();
        y = r.y() - 48;
        baseY = y;
        leftX = w.camera.targetMinX() + 64;
        rightX = w.camera.targetMinX() + 288;
        contactDamage = 4;
    }

    @Override
    protected void fight(World w) {
        timer++;
        switch (sub) {
            case 0 -> {
                if (startWarning(w)) {
                    sub = 1;
                }
            }
            case 1 -> {
                if (warningOver(w)) {
                    shown = true;
                    startMusic(w);
                    w.showMessage("BLAZE HEATNIX: NO DATA YET", 240);
                    sub = 2;
                }
            }
            case 2 -> {
                if (fillHp(w)) {
                    sub = 3;
                }
            }
            default -> {
                x += vx;
                if (x <= leftX || x >= rightX) {
                    vx = -vx;
                }
                y = baseY + (float) Math.sin(timer * Math.PI * 2 / 120) * 16;
                faceRight = vx > 0;
            }
        }
    }

    @Override
    protected void onDeathStart(World w) {
    }

    @Override
    public Rect hurtBox() {
        return sub < 3 || dying() ? null : new Rect(x - 16, y - 24, 32, 48);
    }

    @Override
    public Rect contactBox() {
        return hurtBox();
    }

    @Override
    public List<ShapeFx> shapes() {
        if (!shown) {
            return List.of();
        }
        float a = flash > 0 ? 0.5f : 1f;
        return List.of(
                new ShapeFx(x, y, 22, 0.9f, 0.25f, 0.1f, a),
                new ShapeFx(x, y - 4, 14, 1f, 0.6f, 0.15f, a),
                new ShapeFx(x, y - 8, 7, 1f, 0.95f, 0.6f, a));
    }
}
