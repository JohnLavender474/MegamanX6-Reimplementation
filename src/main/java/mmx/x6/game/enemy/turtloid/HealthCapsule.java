package mmx.x6.game.enemy.turtloid;

import mmx.x6.anim.Animator;
import mmx.x6.game.Entity;
import mmx.x6.game.Solid;
import mmx.x6.game.World;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.player.PlayerConst;
import mmx.x6.game.player.PlayerStep;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class HealthCapsule extends Entity implements Solid {

    public enum State { WAIT, START, FULL, DELAY, REFILL, HOLD, AFTER }

    static final String SET = "health-capsule";

    private final boolean once;
    private State state = State.WAIT;
    private int timer;
    private Column column;

    public HealthCapsule(World w, RecordDef r) {
        this.record = r;
        this.x = r.x();
        this.y = r.y();
        this.once = r.id() == 1;
        this.anim = new Animator(w.animSet(SET));
        anim.play(0);
    }

    public State state() {
        return state;
    }

    @Override
    public void update(World w) {
        anim.tick();
        switch (state) {
            case WAIT -> {
                if (standing(w) && !w.player.locked()) {
                    w.healHold = true;
                    w.player.carry(x - w.player.x, 0);
                    w.player.startEvent(w.player.faceRight, 0);
                    w.pauseOwner = this;
                    state = State.START;
                }
            }
            case START -> {
                if (w.player.hp >= PlayerConst.MAX_HP) {
                    anim.play(2);
                    state = State.FULL;
                } else {
                    w.sfx.play(2, 85);
                    anim.play(1);
                    timer = 31;
                    column = new Column(w, this);
                    w.add(column);
                    w.add(new Column(w, this, 4));
                    state = State.DELAY;
                }
            }
            case FULL -> {
                if (anim.finished()) {
                    end(w);
                }
            }
            case DELAY -> {
                if (--timer <= 0) {
                    state = State.REFILL;
                    timer = 0;
                }
            }
            case REFILL -> {
                if (++timer % 2 == 0) {
                    w.player.hp++;
                    w.sfx.play(0, 13);
                    if (w.player.hp >= PlayerConst.MAX_HP) {
                        w.player.hp = PlayerConst.MAX_HP;
                        state = State.HOLD;
                        timer = 27;
                    }
                }
            }
            case HOLD -> {
                if (--timer <= 0) {
                    end(w);
                }
            }
            case AFTER -> {
                if (!standing(w)) {
                    state = State.WAIT;
                }
            }
        }
    }

    private void end(World w) {
        w.player.endEvent();
        w.healHold = false;
        w.pauseOwner = null;
        anim.play(0);
        if (once) {
            Debris.spawn(w, SET, new int[] {7, 8, 9}, x, y);
            dead = true;
            w.spawner.kept(record);
            return;
        }
        state = State.AFTER;
    }

    boolean holding() {
        return state == State.HOLD;
    }

    private boolean standing(World w) {
        PlayerStep s = w.player.step();
        if (s != PlayerStep.STAND && s != PlayerStep.WALK_START && s != PlayerStep.WALK
                && s != PlayerStep.EVENT_HOLD) {
            return false;
        }
        Rect p = w.player.hurtBox();
        if (p == null) {
            return false;
        }
        Rect pad = solidRect();
        return p.x() >= pad.x() + 8 && p.right() <= pad.right() - 8 && Math.abs(p.bottom() - pad.y()) <= 1.5f;
    }

    @Override
    public Rect solidRect() {
        return new Rect(x - 24, y - 8, 48, 16);
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
        return 7;
    }

    private static final class Column extends Entity {

        private final HealthCapsule capsule;
        private final boolean front;
        private int phase;

        Column(World w, HealthCapsule capsule) {
            this.capsule = capsule;
            this.front = false;
            this.x = capsule.x;
            this.y = capsule.y;
            this.anim = new Animator(w.animSet(SET));
            anim.play(3);
        }

        Column(World w, HealthCapsule capsule, int animIndex) {
            this.capsule = capsule;
            this.front = true;
            this.x = capsule.x;
            this.y = capsule.y;
            this.anim = new Animator(w.animSet(SET));
            anim.play(animIndex);
        }

        @Override
        public void update(World w) {
            anim.tick();
            if (front) {
                if (anim.finished()) {
                    dead = true;
                }
                return;
            }
            if (phase == 0 && anim.finished()) {
                anim.play(5);
                phase = 1;
            } else if (phase == 1 && (capsule.holding() || capsule.dead || capsule.state == State.AFTER)) {
                anim.play(6);
                phase = 2;
            } else if (phase == 2 && anim.finished()) {
                dead = true;
            }
        }

        @Override
        public boolean clearedByGate() {
            return false;
        }

        @Override
        public boolean runsInPause() {
            return true;
        }

        @Override
        public String spriteSet() {
            return SET;
        }

        @Override
        public int drawPriority() {
            return front ? 1 : 6;
        }
    }
}
