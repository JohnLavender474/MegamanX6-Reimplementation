package mmx.x6.frontend;

import mmx.x6.audio.Sfx;
import mmx.x6.input.Buttons;

public final class FrontEnd {

    // TODO: BootTask plays the logo with the movie player directly; 90 frames (movie_table row 0) and the Start skip are taken from row 0 (docs/gaps.md, "Front end")
    public static final int LOGO_FRAMES = 90;
    public static final int OPENING_FRAMES = 1518;
    public static final int UPDATES_PER_FRAME = 4;
    // TODO: the load after the logo has no known length; 60 updates is a guess (docs/gaps.md, "Front end")
    public static final int LOAD_UPDATES = 60;
    public static final int TITLE_UPDATES = 600;
    public static final int MENU_IDLE = 18001;
    public static final int FADE_SPEED = 8;
    public static final int SOUND_MOVE = 12;
    public static final int SOUND_PICK = 34;
    private static final int ROWS = 3;
    private static final int OPENING_SKIP = Buttons.START | Buttons.JUMP | Buttons.BUSTER | Buttons.DASH
            | Buttons.WEAPON;
    private static final int MENU_PICK = Buttons.START | Buttons.JUMP;

    private final Sfx sfx;
    private final Fade fade = new Fade();
    private Phase phase;
    private Phase pending;
    private boolean starting;
    private boolean started;
    private int phaseUpdates;
    private int timer;
    private int row;
    private int before;

    public FrontEnd(Sfx sfx, Phase start) {
        this.sfx = sfx;
        enter(start);
    }

    public Phase phase() {
        return phase;
    }

    public int phaseUpdates() {
        return phaseUpdates;
    }

    public int movieFrame() {
        return phase == Phase.LOGO || phase == Phase.OPENING ? phaseUpdates / UPDATES_PER_FRAME : 0;
    }

    public int row() {
        return row;
    }

    public float darkness() {
        return fade.darkness();
    }

    public boolean startGame() {
        return started;
    }

    public boolean textShown(int textVar) {
        // TODO: the blink phase (shown while bit 0x10 of the update count is clear) is a guess (docs/gaps.md, "Front end")
        boolean blinkOn = (phaseUpdates & 0x10) == 0;
        if (phase == Phase.TITLE) {
            return textVar == 4 && blinkOn;
        }
        if (phase == Phase.MENU) {
            return switch (textVar) {
                case 0, 1, 2, 6 -> true;
                case 5, 7 -> blinkOn;
                default -> false;
            };
        }
        return false;
    }

    public void update(int held) {
        int pressed = held & ~before;
        before = held;
        fade.update();
        if (fade.busy()) {
            phaseUpdates++;
            tickWhileFading();
            return;
        }
        if (pending != null) {
            enter(pending);
            return;
        }
        if (starting) {
            started = true;
            return;
        }
        phaseUpdates++;
        switch (phase) {
            case READY -> {
                if ((pressed & Buttons.START) != 0) {
                    enter(Phase.LOGO);
                }
            }
            case LOGO -> {
                if ((pressed & Buttons.START) != 0 || phaseUpdates >= LOGO_FRAMES * UPDATES_PER_FRAME) {
                    enter(Phase.LOAD);
                }
            }
            case LOAD -> {
                if (phaseUpdates >= LOAD_UPDATES) {
                    enter(Phase.OPENING);
                }
            }
            case OPENING -> {
                if ((pressed & OPENING_SKIP) != 0) {
                    // TODO: the movie player stops the opening at the press (movie_table skip buttons); the sound 34 of the front-end skip rule is a guess here (docs/gaps.md, "Front end")
                    sfx.play(0, SOUND_PICK);
                    enter(Phase.TITLE);
                } else if (phaseUpdates >= OPENING_FRAMES * UPDATES_PER_FRAME) {
                    enter(Phase.TITLE);
                }
            }
            case TITLE -> {
                if ((pressed & Buttons.START) != 0) {
                    sfx.play(0, SOUND_PICK);
                    fadeTo(Phase.MENU);
                } else {
                    countTitle();
                }
            }
            case MENU -> menu(pressed);
        }
    }

    private void tickWhileFading() {
        if (phase == Phase.TITLE && pending == null) {
            countTitle();
        }
    }

    private void countTitle() {
        if (--timer <= 0) {
            // TODO: X6 runs a demo and the member 0 picture after the title; the title goes back to the opening (docs/gaps.md, "Front end")
            fadeTo(Phase.OPENING);
        }
    }

    private void menu(int pressed) {
        if (pressed == 0) {
            if (++timer >= MENU_IDLE) {
                fadeTo(Phase.OPENING);
            }
            return;
        }
        timer = 0;
        if ((pressed & Buttons.UP) != 0) {
            row = (row + ROWS - 1) % ROWS;
            sfx.play(0, SOUND_MOVE);
        } else if ((pressed & Buttons.DOWN) != 0) {
            row = (row + 1) % ROWS;
            sfx.play(0, SOUND_MOVE);
        } else if ((pressed & MENU_PICK) != 0 && row == 0) {
            // TODO: CONTINUE and OPTION (rows 1, 2) and the two new-game cheats on row 0 are not made (docs/gaps.md, "Front end")
            sfx.play(0, SOUND_PICK);
            starting = true;
            fade.out(FADE_SPEED);
        }
    }

    private void fadeTo(Phase next) {
        pending = next;
        fade.out(FADE_SPEED);
    }

    private void enter(Phase next) {
        phase = next;
        pending = null;
        phaseUpdates = 0;
        row = 0;
        timer = 0;
        if (next == Phase.TITLE) {
            timer = TITLE_UPDATES;
            fade.in(FADE_SPEED);
        } else if (next == Phase.MENU) {
            fade.in(FADE_SPEED);
        } else {
            fade.clear();
        }
    }
}
