# Doc changes from the front-end states

## `docs/engine.md`

Old:

```
The screens from power-on to the start of a game. No save state shows them yet.
```

New:

```
The screens from power-on to the start of a game. Save states: `title-1_capcom-logo` (the logo
movie), `title-2_loading` (the black load of stage 14 after the logo; not the opening),
`title-3_anime-intro` (the opening movie, frame 19 of 1527), `title-4_press-start` (the title) and
`title-5_main-menu` (the menu). The emulator keeps no rendered frame in VRAM, so only the movie
frames are visible in a state; `~/mmx-data/x6/re/front-end/` rebuilds the title and menu screens
from the layout and the objects. In the logo and opening states, task slot 0 runs (state 127,
the movie player runs inside the task); slot 1 is free in all five [certain].
```

Old:

```
  (`0x80069000`–`0x80069800`) at 320 × 240. Game mode 15 (`0x8001EC68`) calls `PlayMovie(8)` and
  `PlayMovie(9)`; rows 8 and 9 are 0 in the executable [not known what fills them].
```

New:

```
  (`0x80069000`–`0x80069800`) at 320 × 240, 24 bit, into both frame buffers. Game mode 15
  (`0x8001EC68`) calls `PlayMovie(8)` and `PlayMovie(9)`; rows 8 and 9 are 0 in the executable
  and in RAM in the five front-end states [not known what fills them].
```

Old:

```
| 1 | `0x8001E024` | **Title.** Stage 14 area 0 (member 107, overlay 18), `SetDisplayMode(1)` (512 wide), one misc object type 19 `var` 4 (`0x80071114`). 600 frames, then state 2 |
```

New:

```
| 1 | `0x8001E024` | **Title.** Sub-step table `0x80071118`; then `StageLogicLoop`, `MiscLoop`, `RayLoop`, `PaletteFade`, `DrawStage`. Sub-step 0 loads stage 14 area 0 again (member 107, overlay 18), `SetDisplayMode(1)` (512 wide), shows BG layer 0 only; sub-step 2 makes one misc object type 19 `var` 4 (`0x80071114`; PRESS START BUTTON, blinks), `FadeIn(8)`. 600 frames (`fe+0x04` counts down), then `FadeOut(8)` and state 2. Live: `title-4_press-start`, sub-step 3, timer 465 |
```

Old:

```
| 6 | `0x8001D9CC` | **Main menu.** Sub-step table `0x800710D0`. Six misc objects type 19, `var` 0, 1, 2, 5, 6, 7 (`0x800710E0`). Up and Down (sound 12) move `fe+0x02` over rows 0–2; Start or Cross (`0x0840`, sound 34) picks the row. Row 0: `game+0x00` = 0 (new game), `TaskStart(1, GameModeTask)`. Row 1: state 7. Row 2: state 8. 18 000 frames with no button go back to state 3 |
| 7, 8 | `0x800ED760`, `0x800EA5C8` | In overlay 18 [guess: Continue (memory card) and Options; the options code is `0x800E9C74`] |
```

New:

```
| 6 | `0x8001D9CC` | **Main menu.** Sub-step table `0x800710D0`. Six misc objects type 19, `var` 0, 1, 2, 5, 6, 7 (`0x800710E0`): the rows GAME START, CONTINUE, OPTION, the line "PRESS START BUTTON or ⊗ BUTTON" (`var` 5 and 7, blink) and the orange highlight (`var` 6). Up and Down (sound 12) move `fe+0x02` over rows 0–2; Start or Cross (`0x0840`, sound 34) picks the row. Row 0 (GAME START): `game+0x00` = 0 (new game), `TaskStart(1, GameModeTask)`. Row 1 (CONTINUE): state 7. Row 2 (OPTION): state 8. 18 000 frames with no button go back to state 3. On row 0, two input counters are the new-game cheats (see below). Live: `title-5_main-menu`, sub-step 1, row 0 |
| 7 | `0x800ED760` | **CONTINUE** (overlay 18): sub-step table `0x800F08FC`; sub-step 3 (`0x800EA8C0`) calls `LoadGame` (`0x8001BA1C`) [certain] |
| 8 | `0x800EA5C8` | **OPTION** (overlay 18): sub-step table `0x800F08E4` (6 entries); the half-speed option code `0x800E9C74` is in this part [likely] |
```

Old:

```
known what uses area 1]. Member 107 has the entry types of the boss intro members (`0x4`,
`0x20000`, `0x10014`), so the title picture is likely drawn with the BG layers too [likely].
```

New:

```
known what uses area 1]. The title picture is drawn with BG layer 0, as the boss intro [certain;
`title-4_press-start`, `title-5_main-menu`]: the layout is 4 × 2 with layer 0 row 0 = screens 1–4
and layers 1, 2 empty; all cameras at (0, 0); member 107 entry `0x10014` is at VRAM (512, 0);
the CLUTs are member 26 rows 0–7 at VRAM rows 480–487 (BG CLUT base (0, 486)). Screens 1, 2 are
the title picture (logo, lens, copyright lines; the full 512 × 240 view). Screens 3, 4 are a blue
hexagon pattern outside the view [guess: the CONTINUE or OPTION background]. Stage 14 has no
solid cells, so `learn` cannot learn it.

**Title and menu text** (misc type 19, overlay 18, `FrontEndText` `0x800EDB6C`; misc type table
`0x800F15F8`) [certain]. State table `0x800F0BF8`: init, update, delete. The init puts the object
in screen space (layer −1), texture `0x1800` (VRAM (512, 256)), sprite data = the first entry of
`sprite_bank_base`, and x, y, sprite and CLUT from `0x800F0B44` (6 bytes per `var`). `var` 0–3:
GAME START, CONTINUE, OPTION, TRAINING at x 258, y 142, 158, 174, 182; `var` 4: PRESS START BUTTON
(258, 160); `var` 5 and 7: "PRESS START BUTTON or ⊗" and "BUTTON" (220 and 476, y 192); `var` 6:
the highlight, CLUT `0x7801` (orange; the others `0x7800`, purple), priority 2, y = 142 +
row × 16, sprite of the row. `var` 4, 5 and 7 are shown 16 of each 32 frames. No code makes
`var` 3 (TRAINING) [likely a leftover of another version].

**New-game cheats** [certain for the code]. On menu row 0, two word counters (cleared by Up, Down,
another row and the menu set-up): `0x800E3130` counts Left (`0x8000`) to 3, then Right
(`0x2000`) sets it to 4; `0x800E3134` counts L1 (`0x0004`) to 3, then R2 (`0x0002`) sets it to 4
(sound 34 at 4; each clears the other). Game mode 0 (`0x8001E088`) gives X armour bits `0x01`
and form 1 (Falcon); with the first counter at 4, bits `0x09` and form 4 (Ultimate Armor); with
the second at 4, it adds bit `0x20` (Black Zero). Then game mode 19.
```

## `docs/TODO.md`

Old:

```
Status: the state flow is read from the code and the two movies are decoded (2026-10-25,
`docs/engine.md`, "Front end"). No save state shows a front-end screen.
```

New:

```
Status: the state flow is read from the code and the two movies are decoded (2026-10-25,
`docs/engine.md`, "Front end"). Five save states from the logo to the menu (`title-1` to
`title-5`); the title and menu screens are rebuilt in `~/mmx-data/x6/re/front-end/images/`.
```

Old:

```
- [ ] Save states: the title with PRESS START, the main menu (two rows), the screens of menu rows
  1 and 2, a demo in play, the picture after the demo (member 0).
- [ ] With the states: learn the stage 14 layout (member 107) and draw it; export the misc type 19
  (title and menu) and type 21 sprites; find where the misc type table of overlay 18 is.
- [ ] Menu rows 1 and 2 (states 7 and 8, overlay 18): which is Continue and which is Options; the
  options rows; the memory card load screen.
```

New:

```
- [x] Save states of the title with PRESS START and the main menu (three rows: GAME START,
  CONTINUE, OPTION): `title-4_press-start`, `title-5_main-menu`.
- [ ] Save states: the CONTINUE screen (state 7), the OPTION screen (state 8), a demo in play, the
  picture after the demo (member 0), a "Now Loading..." screen (the text is in the resident VRAM
  page at (832, 256); not known which code draws it).
- [x] The stage 14 layout (4 × 2, layer 0 screens 1–4) and the misc type 19 sprites are drawn by
  scripts in `~/mmx-data/x6/re/front-end/`; the misc type table of overlay 18 is `0x800F15F8`.
- [ ] Tools: `learn` refuses stage 14 (no solid cells, so no joins), and `spriteexport` refuses it
  (no type list). Allow a stage without collision in `learn`, and a sprite source from a live misc
  object without a type list in `spriteexport`. Misc type 21 (state 3) and misc type 1 of overlay
  18 (`0x800EA02C`) are not read.
- [ ] Menu rows 1 and 2: row 1 is CONTINUE (state 7, `LoadGame`), row 2 is OPTION (state 8). Still
  open: the options rows, the memory card load screen, what shows screens 3 and 4 of member 107.
- [ ] Overlay 19 (stage 14 area 1) has a Left / Right X-or-Zero choice and a cheat counter that
  must reach 11 (`docs/players/steps-extra.md`); the main menu has its own two cheats (game mode
  0). Find when overlay 19 is loaded.
```
