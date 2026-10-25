package mmx.x6.game.enemy;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;
import mmx.x6.physics.TileRules;

public final class LightCapsule extends Entity implements Solid {

    public enum State { WAIT_NEAR, OPEN, TALK, CLOSE, WAIT_STAND, INSIDE, GIVE, DONE, GONE }

    private static final String SET = "light-capsule";
    private static final int[] BITS = {0x01, 0x04, 0x02, 0x08, 0x10, 0x40, 0x20, 0x80};
    private static final String[] PARTS = {"BLADE ARMOR: HEAD", "BLADE ARMOR: BODY", "BLADE ARMOR: ARMS",
            "BLADE ARMOR: LEGS", "SHADOW ARMOR: HEAD", "SHADOW ARMOR: BODY", "SHADOW ARMOR: ARMS",
            "SHADOW ARMOR: LEGS"};
    private static final int TALK_UPDATES = 150;

    private final int id;
    private State state = State.WAIT_NEAR;
    private int timer;
    private Lid lid;

    public LightCapsule(World w, RecordDef r) {
        this.record = r;
        this.id = r.id() & 7;
        this.x = r.x();
        this.y = r.y();
        this.anim = new Animator(w.animSet(SET));
        anim.play(0);
        if ((w.progress.armorParts & BITS[id]) != 0) {
            state = State.GONE;
            return;
        }
        for (int i = 0; i < 64 && !TileRules.isObjectSolid(w.grid.at(x, y + 12)); i++) {
            y += 16;
        }
        y = (float) Math.floor((y + 12) / 16) * 16 - 12;
        lid = new Lid(this);
        w.add(lid);
    }

    public State state() {
        return state;
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (state) {
            case WAIT_NEAR -> {
                float dx = Math.abs(w.player.x - x);
                float dy = w.player.y - y;
                if (dx <= 64 && dy >= -104 && dy <= 16 && w.player.grounded() && !w.player.locked()) {
                    anim.play(10);
                    w.player.startEvent(w.player.x < x, 0);
                    w.pauseOwner = this;
                    state = State.OPEN;
                }
            }
            case OPEN -> {
                if (anim.entered(1)) {
                    anim.play(11);
                    // TODO: Dr. Light's voice (stream 28) and the text box are not played; a short message stands in (docs/gaps.md, "Light capsule")
                    w.showMessage("DR. LIGHT CAPSULE", TALK_UPDATES);
                    state = State.TALK;
                    timer = TALK_UPDATES;
                }
            }
            case TALK -> {
                if (--timer <= 0) {
                    anim.play(12);
                    state = State.CLOSE;
                }
            }
            case CLOSE -> {
                if (anim.entered(1)) {
                    w.player.endEvent();
                    w.pauseOwner = null;
                    state = State.WAIT_STAND;
                }
            }
            case WAIT_STAND -> {
                if (standing(w)) {
                    anim.play(1);
                    w.player.startEvent(w.player.faceRight, 0);
                    w.pauseOwner = this;
                    state = State.INSIDE;
                    timer = 0;
                }
            }
            case INSIDE -> {
                w.player.carry(x - w.player.x, 0);
                // TODO: the player's "get part" sequence (player event step 24, phases mega+0xE2 1-3) is not read; 60 updates of animation 1 stand in (docs/gaps.md, "Light capsule")
                if (++timer >= 60) {
                    anim.play(2);
                    state = State.GIVE;
                    timer = 40;
                }
            }
            case GIVE -> {
                w.player.carry(x - w.player.x, 0);
                if (--timer <= 0) {
                    anim.play(13);
                    w.progress.armorParts |= BITS[id];
                    w.showMessage(PARTS[id], 180);
                    w.player.endEvent();
                    w.pauseOwner = null;
                    state = State.DONE;
                }
            }
            case DONE -> {
                if (w.offScreen(x, y, 136, 136)) {
                    remove(w);
                }
            }
            case GONE -> remove(w);
        }
    }

    private void remove(World w) {
        dead = true;
        if (lid != null) {
            lid.dead = true;
        }
        w.spawner.kept(record);
    }

    private boolean standing(World w) {
        Rect p = w.player.hurtBox();
        return p != null && w.player.grounded() && Math.abs(p.bottom() - (y - 12)) <= 1.5f
                && Math.abs(w.player.x - x) <= 16;
    }

    @Override
    public Rect solidRect() {
        return state == State.GONE ? null : new Rect(x - 16, y - 12, 32, 24);
    }

    @Override
    public boolean visible() {
        return state != State.GONE;
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
        return 4;
    }

    private static final class Lid extends Entity implements Solid {

        private final LightCapsule capsule;

        Lid(LightCapsule capsule) {
            this.capsule = capsule;
        }

        @Override
        public void update(World w) {
            if (capsule.dead) {
                dead = true;
            }
        }

        @Override
        public Rect solidRect() {
            return new Rect(capsule.x - 32, capsule.y - 78 - 12, 64, 24);
        }

        @Override
        public boolean visible() {
            return false;
        }

        @Override
        public boolean clearedByGate() {
            return false;
        }
    }
}
