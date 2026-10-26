package mmx.x6.game.enemy.scaravich;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class TeleporterPole extends Entity {

    static final String SET = "totem-pole";
    private static final int POLE_ANIM = 26;

    private final int pole;
    private SectionTeleport teleport;
    private int section = -1;

    public TeleporterPole(World w, RecordDef r) {
        this.record = r;
        this.pole = r.id();
        this.x = r.x();
        this.y = r.y();
        this.anim = new Animator(w.animSet(SET));
        anim.play(POLE_ANIM);
    }

    public int section() {
        return section;
    }

    @Override
    public void update(World w) {
        if (w.polesCleared >= pole) {
            dead = true;
            w.spawner.kept(record);
            return;
        }
        if (teleport != null) {
            return;
        }
        Rect p = w.player.hurtBox();
        if (p != null && !w.player.invulnerable() && !w.player.dying() && p.overlaps(trigger())) {
            teleport = new SectionTeleport(w, () -> {
                section = Sections.roll(w, pole);
                Sections.placeBlocks(w, section);
                return Sections.entry(section);
            }, () -> {
                dead = true;
                w.spawner.kept(record);
            });
            w.add(teleport);
            return;
        }
        if (w.offScreen(x, y, 64, 128)) {
            dead = true;
            w.spawner.released(record);
        }
    }

    private Rect trigger() {
        return new Rect(x - 16, y - 80, 32, 160);
    }

    @Override
    public boolean runsInPause() {
        return teleport != null;
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
        return 5;
    }
}
