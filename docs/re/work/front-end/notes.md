# Front end: five save states (Capcom logo to main menu)

States (`~/mmx-data/x6/dumps/`), saved 2026-10-25 22:11:05–22:11:40 (PCSXR slots 1–5, file
`.000`–`.004`; the SHA-1 of each slot file matches its dump):

| state | user's words | what it holds |
|---|---|---|
| `title-1_capcom-logo` | Capcom intro logo screen | `CAPLOGO.STR` frame 41/42 of 93 in the two frame buffers (24 bit). `game` all 0 (stage 0). `fe` all 0. Task 0 running (state 127): `BootTask` → `0x8001CBE0` → movie player `0x8001A9B0` |
| `title-2_loading` | Anime intro screen | **Not the opening.** The frame buffers are black. `game+0x0C` = 14, area 0, `fe` all 0, task 0 waiting (state 1). `arc_load_status` = 1 (a load runs), `cd_upload_pending` = 1. Stack: `BootTask` → `0x80013DE8` (the stage 14 data load) → wait `0x80015EC0`. So it is the black load between the logo and the opening |
| `title-3_anime-intro` | Loading screen | **The opening movie**, `X6OP.STR` frame 19/20 of 1527 (the "MEGAMAN X / EPISODE 6" card, 1.3 s in). `fe` state 0 sub-step 6, task 0 running, stack `0x8001D3F0` (`SetDisplayMode`) / `0x8001D3F8` (`PlayMovie`) |
| `title-4_press-start` | PRESS START screen | `fe` state 1 (title) sub-step 3, timer 465 (counts down from 600: 135 updates after the fade-in). One misc object type 19 `var` 4 (PRESS START BUTTON), display 0 at this frame (it blinks) |
| `title-5_main-menu` | Select screen | `fe` state 6 (menu) sub-step 1, row 0, timer 77, `+0x0D` = 1, `+0x0E` = 1 (the menu was reached by a skip on the title). Six misc type 19 objects |

The user's words for states 2 and 3 do not fit the contents [certain for the contents]: state 2
is the black load screen, state 3 is the opening. The likely cause: the user described the slots
after the fact. The file names are kept; the catalog note should say what each holds.

The emulator does not keep rendered frames in VRAM: the frame buffers are black in states 2, 4, 5
(as in all stage states; `re/boss-intro/notes.md`), and the preview at `0x25` is a black picture
with a "0" in all five. Only the movie frames (uploaded with `LoadImage` by the MDEC player) are in
VRAM, as 24-bit 320 × 240 at y 0 and y 240 [certain]. So the screens below are rebuilt from the
layout, the texture and the objects.

## Scripts (`scripts/`)

- `dump_fe.py`: `game`, `fe`, tasks, display mode, fade, load status, movie rows, live objects.
- `task_ctx.py`: (no use: PCSXR HLE BIOS keeps the thread contexts outside RAM; TCB pointers are
  `0xC0`, `0x180`.) The return addresses were found by a stack scan of task 0 (`0x801FE800`–`0x801FEC00`).
- `vram_png.py`, `vram24_png.py`, `vram4.py`: VRAM as 15 bit, 24 bit, 4 bpp with a CLUT.
- `match_movie.py`: matches a frame buffer against `~/mmx-data/x6/movies/*/frame-*.png`.
- `render_title.py`: draws stage 14 area 0 (member 107) from the RAM layout.
- `draw_misc19.py`: every sprite of the misc type 19 source, CLUT `0x7800` and `0x7801`.
- `compose_screen.py`: BG layer 0 + live misc 19 objects = the 512 × 240 screen.

Images (`images/`): `stage14-area0-layer0.png` (the 4 screens), `title-screen.png`,
`main-menu-screen.png`, `misc19-sprites.png`, `vram-512-256-clut7800.png`. All checked by eye.

## Stage 14 area 0 (title and menu picture) [certain]

- Layout in RAM: 4 × 2, layer 0 row 0 = screens 1, 2, 3, 4; layers 1 and 2 empty. Camera of all
  three layers at (0, 0); `cam+0x03` (shown) = 1 for layer 0, 0 for layers 1, 2 (title sub-step 0
  writes these three bytes: `0x8001DE84`–`0x8001DE98`).
- Texture: member 107 entry `0x10014` at VRAM (512, 0) (exact match). Entry `0x10016` is not in
  VRAM in these states. CLUTs: member 26 entry 5 rows 0–7 at VRAM rows 480–487; the tiles use
  palette 0 (4 bpp) and 16 (8 bpp) at the BG CLUT base (0, 486).
- Screens 1, 2 (x 0–511) = the title picture: the MEGAMAN X6 logo, a green lens, the two copyright
  lines. This is the full 512 × 240 view. Screens 3, 4 (x 512–1023) = a blue hexagon pattern, not
  in view at camera x 0 [guess: the background of the CONTINUE or OPTION screen].
- So the title picture is drawn with BG layer 0 (`DrawStage`), as the boss intro. This closes the
  [likely] in `docs/engine.md`.
- `learn` fails on this stage ("screens do not join ... 0 joins"): it has no solid cells. A tool
  run needs a way to learn a stage without collision (see open items).

## Front-end code read

Title (state 1, `0x8001E024`): `title_steps[fe+0x01]` (`0x80071118`, 5 entries), then
`StageLogicLoop`, `MiscLoop`, `RayLoop`, `PaletteFade`, `DrawStage`.

| sub-step | code | what it does |
|---|---|---|
| 0 | `0x8001DE10` | `fe+0x0D` = 2, `0x8001D218`, `0x8002C1DC`, stage 14 area 0, the stage data load `0x80013DE8`, `SetDisplayMode(1)`, `0x80017E24`, `0x8002B838`, `0x8002C2B8`, `CheckpointStart`, `LayerInit`, `0x80025F34`, layer 0 shown, layers 1 and 2 hidden |
| 1 | `0x8001DFB0` | waits for `fade_dir` = 0 |
| 2 | `0x8001DEB8` | one misc object type 19, `var` = `0x80071114[0]` = 4; timer = 600, `fe+0x0D` = 0 (skip allowed), `FadeIn(8)` |
| 3 | `0x8001DF5C` | timer − 1; at 0 `FadeOut(8)` |
| 4 | `0x8001DFDC` | when `fade_dir` = 0: `fe+0x0D` = 1, `0x8001D218`, state 2 (demo), sub-step 0 |

The title loads stage 14 again each time it starts [certain for the call; not read whether the
load skips data that is already in RAM].

Main menu (state 6, `0x8001D9CC`): `menu_steps[fe+0x01]` (`0x800710D0`), then `MiscLoop`,
`PaletteFade`, `DrawStage`.

| sub-step | code | what it does |
|---|---|---|
| 0 | `0x8001DA20` | `0x80097426` = 0, `StreamStopNow`, `PaletteFxClear(0)`, `0x8002B838`; six misc 19 objects (`var` 0, 1, 2, 5, 6, 7); `FadeIn(8)`; timer 0; clears the two cheat counters `0x800E3130`, `0x800E3134` |
| 1 | `0x8001CF78` | when no fade: Up (`0x1000`) row − 1 (0 → 2), Down (`0x4000`) row + 1 (2 → 0), both sound 12; the two cheat counters; Start or Cross (`0x0840`): sound 34, `FadeOut(8)`, `StreamStopNow`, sub-step 2. Timer + 1 in each pass with no new press, 0 after a press; at 18 001: `StreamStopNow`, `FadeOut(8)`, sub-step 3 |
| 2 | `0x8001DAF8` | when no fade: row 0: `0x8002C1DC`, `game+0x00..+0x03` = 0, `TaskStart(1, GameModeTask)`, `TaskExit`. Row 1: state 7. Row 2: state 8 |
| 3 | `0x8001DBAC` | when no fade: state 3 |

Pad bits are those of `pad1+4` (pressed): `0x0001` L2, `0x0002` R2, `0x0004` L1, `0x0008` R1,
`0x0010` Triangle, `0x0020` Circle, `0x0040` Cross, `0x0080` Square, `0x0100` Select, `0x0800`
Start, `0x1000` Up, `0x2000` Right, `0x4000` Down, `0x8000` Left [likely: the standard pad word
byte-swapped; Start = `0x0800`, Start + Select = `0x0900` and Cross = `0x0040` agree with the
code].

### The two new-game cheats [certain for the code; the names are likely]

On row 0 only (any other row, or Up or Down, clears both):

- `cheat_ultimate` (`0x800E3130`, word): Left (`0x8000`) adds 1 while < 3; Right (`0x2000`) before
  3 clears it. At 3, Right: sound 34, the counter = 4 (and the other one = 0); Left clears it.
  Input: **Left, Left, Left, Right**.
- `cheat_black_zero` (`0x800E3134`): L1 (`0x0004`) adds 1 while < 3; R1 (`0x0008`) before 3
  clears it. At 3, R2 (`0x0002`): sound 34, the counter = 4 (and the other one = 0). Input: **L1,
  L1, L1, R2**.
- Game mode 0 (new game, `0x8001E088`) reads them: `game+0x5F` = 1, `+0x5E` = 1 (Falcon); when
  `cheat_ultimate` = 4: `+0x5F` = 9, `+0x5E` = 4 (Ultimate Armor); when `cheat_black_zero` = 4:
  `+0x5F |= 0x20` (Black Zero). Then game mode 19. Only one can be 4 at a time (each clears the
  other).
- `docs/players/steps-extra.md` says the cheat is "an input counter that reaches 11" in overlay 19.
  That is a second, different code: overlay 19 (stage 14 area 1) at `0x800E9A60`–`0x800E9BA0` has
  a Left / Right choice of `game+0x38` (X or Zero) and a counter per character at `0x800EA760`
  that must be 11 at Start or Cross. So overlay 19 is likely a character select [likely; not
  seen live]. Which of the two the US game reaches in a new game is open: the menu path (game
  mode 0, then mode 19) does not load stage 14 area 1 in the code read here.

Other game mode 0 values [certain for the code]: lives `+0x39` = 2, max HP `+0x5B`, `+0x5C` = 32,
`+0x61`, `+0x62` = 48, `+0x8D`, `+0x8E` = 7, `+0xC9` = byte `0x8006D5F7`, difficulty `+0x68` =
byte `0x8006D5F8`, `+0xD0`/`+0xD1` = bytes `0x8006D5FC`/`0x8006D5FE`, `+0xD8` = 16 words from
`0x8006D634`, the 16 stage-effect bytes `+0x168` = 0, `+0x438` = 0, `0x80097428` = 0, 8 words at
`+0x440` = 36 000, checkpoint 0.

### Misc type 19 (overlay 18, `FrontEndText`) [certain]

- Misc type table of stage 14 area 0: scratchpad `+0x90` = `0x800F15F8` (overlay 18). Type 19 =
  `0x800EDB6C`; type 1 = `0x800EA02C` (overlay 18, not read); the rest are main-program entries.
- State table `0x800F0BF8`: 0 init `0x800ED96C`, 1 update `0x800EDA38`, 2 `DeleteObject3`.
- Init: layer −1 (screen space), priority 3, texture `0x1800` (4 bpp page 24, VRAM (512, 256)),
  sprite data = `sprite_bank_base` + its first word (`0x80103008` + `0x14` = `0x8010301C`), and
  from `fe_text_tbl` (`0x800F0B44`, 6 bytes per `var`: s16 x, s16 y, u8 sprite, u8 CLUT with high
  nibble = row − 480, low nibble = x / 16).
- Update: `var` 6 (the highlight): priority 2, y = 142 + (row & 3) × 16, sprite = the sprite of
  `var` row (rows 0–2; any other row → `var` 3). `var` 4, 5, 7 blink: shown 16 of each 32 frames
  (`frame_count & 0x10`). The others are always shown.

| `var` | x, y | sprite | CLUT | text | used by |
|---|---|---|---|---|---|
| 0 | 258, 142 | 0 | `0x7800` | GAME START | menu |
| 1 | 258, 158 | 1 | `0x7800` | CONTINUE | menu |
| 2 | 258, 174 | 2 | `0x7800` | OPTION | menu |
| 3 | 258, 182 | 3 | `0x7800` | TRAINING | nothing (no list makes it) |
| 4 | 258, 160 | 4 | `0x7800` | PRESS START BUTTON | title |
| 5 | 220, 192 | 5 | `0x7800` | PRESS START BUTTON or ⊗ (left part) | menu |
| 6 | 258, 134 | 0 | `0x7801` | the highlight: the row text in orange | menu |
| 7 | 476, 192 | 6 | `0x7800` | BUTTON (right part of `var` 5) | menu |

CLUT `0x7800` = purple text, `0x7801` = orange text. The menu has **three** rows (GAME START,
CONTINUE, OPTION), not two. Sprite 3 TRAINING and `var` 3 are left over: its y (182) is not on the
16-px row grid (190 would be), and the menu moves only over rows 0–2 [likely: a leftover of
another version].

Live check (`title-5_main-menu`): the six objects have exactly these values; the highlight is on
row 0 (y 142, sprite 0, CLUT `0x7801`). `title-4_press-start`: `var` 4, display 0 (blink off).

### States 7 and 8 (overlay 18) [certain for the calls]

- `front_end_states[7]` = `0x800ED760`, sub-step table `0x800F08FC`; sub-step 3 (`0x800EA8C0`)
  calls `LoadGame` (`0x8001BA1C`, at `0x800EA900`) and `TaskStart` (`0x800EA954`). So row 1
  (CONTINUE) = state 7 = the memory card load. The memory card calls `0x8001C1AC`, `0x8001C384`,
  `0x8001C4C0`, `0x8001C5EC`, `0x8001C8C4` are in this area (`0x800EAB0C`–`0x800EB028`).
- `front_end_states[8]` = `0x800EA5C8`, sub-step table `0x800F08E4` (6 entries: `0x800E98B0`,
  `0x800E9A64`, `0x800EA500`, `0x800E9E44`, `0x800EA0E8`, `0x800EA570`). The half-speed option code
  `0x800E9C74` is in this area, so row 2 (OPTION) = state 8 [likely; the function bounds were not
  walked]. The two tables are next to each other: state 8's table has 6 entries, state 7's starts
  at `0x800F08FC`.
- Overlay 18 in RAM equals `rock_bin/18.bin` (32 488 bytes, 0 differ) in states 4 and 5.

## VRAM of the title and menu [certain for what is there]

- (512, 0)–(1023, 255): member 107 entry `0x10014` (the BG texture, 8 and 4 bpp).
- (512, 256), page 24: the misc 19 text (GAME START … TRAINING, PRESS START BUTTON, … or ⊗
  BUTTON); next to it (576–703, 256–511) a font (upper and lower case, digits, button icons) and
  small icons.
- (832, 256)–(959, 511): a resident system page, already in VRAM in the logo state: "Now
  Loading...", Japanese text, NO DATA, READY, PRESS START BUTTON, GAME OVER, THANK YOU, an X
  portrait [not known which code draws "Now Loading..."; no state shows it].
- (960, 256): a second font.
- CLUTs at rows 480–487 (member 26).

## Live values that check the docs

- `fe` (`0x800CD3F8`): state 3: `00 06 … +0x0D = 1`; state 4: `01 03`, timer 465, `+0x0D` = 0;
  state 5: `06 01`, row 0, timer 77, `+0x0D` = 1, `+0x0E` = 1. All agree with the code. [certain]
- `display_mode` = 0 in states 1–3 (320), 1 in states 4, 5 (512, `screen_width` 512). [certain]
- `movie_table` rows 8 and 9 are 0 in RAM in all five states too (rows 0, 1 as in the doc). So
  they stay 0 up to the menu; nothing fills them before a game starts [certain for these states].
- Tasks: in the logo and the opening, slot 0 runs (state 127) at the save (the movie player
  runs inside task 0); in states 2, 4, 5 slot 0 waits (state 1, wait 1). Slot 1 is free in all
  five. This differs from the 44 stage states (only slot 1 live). [certain]
- `reset_enable` (`0x8008EC0C`) = 0 in all five: no soft reset on the front end; `BootTask`
  writes 0 to it (`0x8001CB9C`). [certain]
- Options block `0x8006D5F0`: `CAPCOM\0`, `+7` = `0x18`, difficulty `0x8006D5F8` = 1, half speed
  `0x8006D5F9` = 0. [certain]

## Open

- What the user saw in state 2 (black in VRAM; the emulator keeps no rendered frame).
- Who draws "Now Loading..." and when (a state during a stage load would show it).
- `learn` / `stages` cannot draw stage 14 (no collision, no type list for `spriteexport`). A tool
  change is needed: a stage entry without the join check, and a sprite source from a live misc
  object without a type list.
- Misc type 1 of overlay 18 (`0x800EA02C`); misc type 21 (state 3).
- Screens 3, 4 of member 107 (the hexagon pattern): which screen shows them (states 7, 8).
- Overlay 19 (stage 14 area 1): when it is loaded; its counter-11 cheat against the menu cheats.
- The movie rows 8, 9, member 162, member 0, the demo, the CONTINUE and OPTION screens: need more
  states (TODO list).
