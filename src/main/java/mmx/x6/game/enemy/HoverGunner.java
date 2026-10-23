package mmx.x6.game.enemy;

import mmx.x6.game.DamageTables;
import mmx.x6.game.World;
import mmx.x6.game.fx.AnimFx;
import mmx.x6.game.fx.Debris;
import mmx.x6.game.fx.Explosion;
import mmx.x6.level.RecordDef;
import mmx.x6.physics.Rect;

public final class HoverGunner extends Enemy {

    public enum Step { WAIT, FLY, BURST, FAN, LEAVE }

    private static final float FLY_SPEED = 4;
    private static final float LEAVE_SPEED = 2;
    private static final int FLY_LIMIT = 60;
    private static final int WAKE_DISTANCE = 176;
    private static final int[] BURST_SHOTS = {22, 31, 40};
    private static final int BURST_END = 62;
    private static final int FAN_FIRST = 21;
    private static final int FAN_EVERY = 10;
    private static final int FAN_COUNT = 9;
    private static final int FAN_END = 104;

    private final int range;
    private Step step;
    private int timer;
    private int bursts;
    private int aim;
    private int sideA;
    private int sideB;
    private int fanShots;

    public HoverGunner(World w, RecordDef r) {
        super(w, r, "hover-gunner");
        hp = 5;
        contactDamage = 3;
        damageTable = DamageTables.Table.NORMAL;
        range = 64 + 16 * w.rng.nextInt(4);
        anim.play(0);
        if (r.id() == 1) {
            step = Step.WAIT;
        } else {
            faceRight = false;
            startFly();
        }
    }

    public Step step() {
        return step;
    }

    private void startFly() {
        step = Step.FLY;
        timer = 0;
        anim.play(0);
    }

    @Override
    public void update(World w) {
        switch (step) {
            case WAIT -> {
                if (w.player.x >= x + WAKE_DISTANCE) {
                    faceRight = true;
                    startFly();
                }
                return;
            }
            case FLY -> {
                if (Math.abs(w.player.x - x) <= range) {
                    step = Step.BURST;
                    timer = 0;
                } else {
                    x += faceRight ? FLY_SPEED : -FLY_SPEED;
                    anim.tick();
                    if (++timer >= FLY_LIMIT) {
                        step = Step.LEAVE;
                    }
                }
            }
            case BURST -> updateBurst(w);
            case FAN -> updateFan(w);
            case LEAVE -> {
                y -= LEAVE_SPEED;
                anim.playIfNot(0);
                anim.tick();
            }
        }
        checkOffScreen(w);
    }

    private void updateBurst(World w) {
        timer++;
        if (timer == 20) {
            aimAtPlayer(w);
        }
        for (int t : BURST_SHOTS) {
            if (timer == t) {
                fire(w, aim);
            }
        }
        if (timer >= BURST_END) {
            bursts++;
            if (bursts == 1) {
                startFly();
            } else {
                step = Step.FAN;
                timer = 0;
                fanShots = 0;
            }
        }
    }

    private void updateFan(World w) {
        if (timer == 0) {
            aimAtPlayer(w);
            boolean axis = aim % 4 != 2;
            int d = axis ? 1 : 2;
            sideA = (aim + 16 - d) % 16;
            sideB = (aim + d) % 16;
        }
        timer++;
        if (fanShots < FAN_COUNT && timer == FAN_FIRST + fanShots * FAN_EVERY) {
            int dir = switch (fanShots % 4) {
                case 1 -> sideA;
                case 3 -> sideB;
                default -> aim;
            };
            fire(w, dir);
            fanShots++;
        }
        if (timer >= FAN_END) {
            step = Step.LEAVE;
        }
    }

    private void aimAtPlayer(World w) {
        double a = Math.atan2(w.player.y - y, w.player.x - x);
        aim = Math.floorMod((int) Math.round(a / (Math.PI * 2 / 16)), 16);
        faceRight = w.player.x >= x;
        anim.play(poseFor(aim));
    }

    private static int poseFor(int dir) {
        int d = Math.floorMod(dir, 16);
        // TODO: which of animations 1-5 shows which aim is not documented; this mapping is a guess (docs/gaps.md, "Hover Gunner poses")
        if (d == 0 || d == 8) {
            return 1;
        }
        if (d == 4) {
            return 5;
        }
        if (d == 12) {
            return 3;
        }
        return d < 8 ? 4 : 2;
    }

    private void fire(World w, int dir) {
        float ox;
        float oy;
        int quadrant = Math.floorMod(dir, 16);
        if (quadrant == 0 || quadrant == 8) {
            ox = quadrant == 0 ? 13 : -13;
            oy = 0;
        } else if (quadrant == 4) {
            ox = 0;
            oy = 16;
        } else if (quadrant == 12) {
            ox = 0;
            oy = -16;
        } else {
            boolean right = quadrant < 4 || quadrant > 12;
            boolean down = quadrant < 8;
            ox = right ? 10 : -10;
            oy = down ? 12 : -12;
        }
        w.add(new HoverGunnerShot(w, x + ox, y + oy, dir));
        w.add(new AnimFx(w, "hover-gunner", 7, x + ox, y + oy, faceRight));
        w.sfx.play(2, 0);
    }

    @Override
    public boolean visible() {
        return step != Step.WAIT;
    }

    @Override
    public Rect hurtBox() {
        return step == Step.WAIT ? null : Rect.ofDoc(x, y, -9, -9, 23, 18, faceRight);
    }

    @Override
    public Rect contactBox() {
        return step == Step.WAIT ? null : Rect.ofDoc(x, y, 0, 0, 11, 11, faceRight);
    }

    @Override
    public int drawPriority() {
        return 4;
    }

    @Override
    protected void onDeath(World w) {
        w.add(new Explosion(w, x, y));
        Debris.spawn(w, "hover-gunner", new int[] {8, 9, 10, 11, 12}, x, y);
    }
}
