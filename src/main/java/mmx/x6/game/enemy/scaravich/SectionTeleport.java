package mmx.x6.game.enemy.scaravich;

import java.util.function.Supplier;
import mmx.x6.game.Entity;
import mmx.x6.game.World;
import mmx.x6.level.Checkpoint;

public final class SectionTeleport extends Entity {

    public enum Phase { SPLIT, FADE_OUT, FADE_IN, JOIN }

    private static final int SPLIT_UPDATES = 29;
    // TODO: FadeOut(8) and FadeIn(8) are taken as 256 / 8 = 32 updates; the fade task's speed unit is not read (docs/gaps.md, "Scaravich routing")
    private static final int FADE_UPDATES = 32;
    private static final int JOIN_START = 64;

    private final Supplier<Checkpoint> destination;
    private final Runnable atEnd;
    private Phase phase = Phase.SPLIT;
    private int timer;
    private Checkpoint dest;

    SectionTeleport(World w, Supplier<Checkpoint> destination, Runnable atEnd) {
        this.destination = destination;
        this.atEnd = atEnd;
        w.sfx.play(1, 1);
        w.camera.setFollow(false);
        w.pauseOwner = this;
        w.player.startTeleport();
        // TODO: the player's shots are not cleared at the teleport start (docs/gaps.md, "Scaravich routing")
    }

    public Phase phase() {
        return phase;
    }

    @Override
    public void update(World w) {
        timer++;
        switch (phase) {
            case SPLIT -> {
                w.player.setSplit(timer);
                if (timer >= SPLIT_UPDATES) {
                    dest = destination.get();
                    phase = Phase.FADE_OUT;
                    timer = 0;
                }
            }
            case FADE_OUT -> {
                w.player.setSplit(SPLIT_UPDATES + timer);
                w.screenFade = Math.min(1, timer / (float) FADE_UPDATES);
                if (timer >= FADE_UPDATES) {
                    arrive(w);
                    phase = Phase.FADE_IN;
                    timer = 0;
                }
            }
            case FADE_IN -> {
                w.player.setSplit(JOIN_START - timer);
                w.screenFade = Math.max(0, 1 - timer / (float) FADE_UPDATES);
                if (timer >= FADE_UPDATES) {
                    phase = Phase.JOIN;
                    timer = 0;
                }
            }
            case JOIN -> {
                w.player.setSplit(Math.max(0, JOIN_START - FADE_UPDATES - timer));
                if (timer >= SPLIT_UPDATES) {
                    w.player.endTeleport();
                    w.camera.setFollow(true);
                    if (w.pauseOwner == this) {
                        w.pauseOwner = null;
                    }
                    dead = true;
                    atEnd.run();
                }
            }
        }
    }

    private void arrive(World w) {
        w.player.placeStanding(dest.playerX(), dest.playerY());
        w.player.faceRight = dest.faceRight();
        w.player.startTeleport();
        w.camera.reset(dest);
        w.sfx.play(1, 1);
    }

    @Override
    public boolean visible() {
        return false;
    }

    @Override
    public boolean runsInPause() {
        return true;
    }

    @Override
    public boolean clearedByGate() {
        return false;
    }
}
