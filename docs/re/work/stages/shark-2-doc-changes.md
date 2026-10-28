# Proposed changes from the Metal Shark Player section 2 rip (`shark-2` to `shark-7`)

Files that this agent may not edit. The main session decides. The facts are in
`docs/stages/shark-2.md`.

## `docs/engine.md`

### "Object classes", the "Stage overlays" bullet

Replace "the area 1 record lists are past the end of overlay 06 and are valid in overlay 07
[likely]" with:

> the area 1 record lists are past the end of overlay 06 and are valid in overlay 07; the six states
> of area 1 (`shark-2` to `shark-7`) have overlay 07 at `0x800E9860` [certain].

and change "[certain for 02, 03, 06, 09, 10]" to "[certain for 02, 03, 06, 07, 09, 10]".

### "Layout", the texture block and texture swap bullets

Add after the member 98 bullet:

> - Member 99 (Metal Shark Player, stage 4 area 1) also has entry `0x16`. Entry `0x10000` is in VRAM
>   on the upper path (`shark-2`, `shark-3`); entry `0x16` after the warp to the mini-boss room
>   (`shark-4` to `shark-7`). It draws the mini-boss room and the boss part [certain].

In "The texture swap", add a writer: "stage 4 area 1: `LowerPathWarpJump` (`0x800EDBEC`, stage logic
10) writes 1 at the warp to the mini-boss room".

### "Layout", a new bullet on animated tiles

> - **Animated tiles.** The belt of stage 4 area 1 (`BeltUpdate`, stage logic 9) changes byte 2 (the
>   texture cell) of 32 tile info words in RAM every 5 updates. So the RAM copy of the tile info can
>   differ from the disc in byte 2 of some words; `learn` accepts that [certain].

### "Game state", the `+0x26` row

Add: "Stage 4 area 1: Nightmare Pressure's death writes 1 (`0x800EB60C`); `AutoScroll` ends when it is
not 0."

### "Game state", the Nightmare Effect bullet "The value is the stage that caused it"

Replace "2 an object of stage 4 area 1 (overlay 07, set-up `0x800F01BC`)" with "2 main type 35 of
stage 4 area 1 (44 records, set-up `0x800F01BC`; blue ice-like blocks)", and "an object of stage 4
area 1" (value 5) with "stage object 15 of stage 4 area 1 (1 record, set-up `0x800EEC50`)".

### "Shots and attached objects", the `GetSysObject` bullet

Add:

> Stage logic 26 (`BossExplosion`, `0x80053CDC`, state table `0x8007AD08`) is the boss death
> explosion: rays, explosions, `Rumble(10)`, then `WhiteFadeOut(25)`. When the screen is white it
> sets its `+0x07` = 1; the dead boss can wait for that signal (Nightmare Pressure moves the camera
> and the player then). After 40 updates (200 with flag bit 7) `WhiteFadeIn(25)`; at the end
> `game.player_lock` = 1 and it deletes itself [certain for the code].

### "Screen effects"

Add:

> - `WhiteFadeOut(speed)` (`0x80013AA4`) and `WhiteFadeIn(speed)` (`0x80013A48`): the white fade,
>   as `FadeOut`/`FadeIn` with white. `white_fade_dir` (`0x80097425`): −1 to white, +1 back, 0 done.
>   The task `WhiteFadeTask` (`0x80013788`) draws a GPU rectangle `0x62` (monochrome,
>   semi-transparent) over the view, draw mode `0xE1000025` (additive), r = g = b =
>   `white_fade_level` (`0x80090C8A`) >> 4 [certain for the command words].

### "Camera"

Add:

> - Warps inside an area: overlay code can write the three layer cameras, the limits and the player
>   position at once, with `SpawnInRect` for the records of the new place. Stage 4 area 1 has two
>   (`LowerPathWarpJump`, Nightmare Pressure's death); see `docs/stages/shark-2.md` [certain].

## `docs/TODO.md`, the collision legend

Add a row or change the 56, 58 row:

> | 58 | 732 (stage 4 area 1) | also the belt surface of stage 4 area 1: `BeltUpdate` moves the player 1 px left per update on it | code (`docs/enemies/conveyor-belt.md`) |

## `catalog/helpers.json`

- Add the names in `~/mmx-data/x6/stages/shark-2-helpers-proposed.json`. The three
  `PressureDeath*` names are a proposal; use the names of the mini-boss sheet if they differ.
- `stage_texture_swap` (`0x8008EAB0`): add "stage 4 area 1 `LowerPathWarpJump` (`0x800EDBEC`)" to
  the writers.
- `0x80090C88` is named `main_objects_end`; the white fade work starts at the same address. Keep
  `main_objects_end` and add `white_fade_level` and `white_fade_speed` at `+2` and `+5`.

## `catalog/enemies.json` (names, not set)

- `enemy-74e4e39589` → `nightmare-pressure` (official name of the mini boss).
- `enemy-3f9602d9d5` → `metal-shark-player` (official).
- `enemy-4185141b5a` → `junk-block` (descriptive, as in `docs/stages/shark.md`).
- `enemy-4c32312ebb` (main 35, the effect 2 block): no name proposed; the official name is not known.
- `enemy-65ec66c9da` and `enemy-c5fe34fa82` (from `shark-7_boss`) are bad exports: the boss texture
  covers their cells. Do not name them. Delete the two folders in `~/mmx-data/x6/sprites/enemies/`
  and their IDs from `~/mmx-data/x6/sprites/stages/shark.json` if wanted.

## `docs/process/process-log.md` (a draft for section 83)

> **Metal Shark Player section 2.** Six states of area 1 (overlay 07, member 99, 26 × 5 screens). The
> `learn` fit test failed twice for new reasons: the belt animates 32 tile info words in RAM (so the
> exact search for the tile info found no member), and the press shakes the screen (two draw buffers
> with cameras 2 px apart tied the vote). Both fixes are small and change only results that failed
> before. The two jumps of X in the states are warps inside the area, not area changes: at camera x
> 4672 the code moves the camera and X by −4672 and to y 944 (the lower path has the same art at its
> start), and Nightmare Pressure's death moves them to the boss part while the screen is white. The
> white is an additive full-screen rectangle that the boss explosion fades in and out.
>
> Lessons:
>
> - When an exact RAM search fails, diff the closest copy byte by byte. Here only one byte of each
>   word changed, and that byte was the texture cell: an animation.
> - A tie in a vote can be two copies of one answer. Look at how far apart the tied candidates are.
> - A jump in the player position between two states is not always a level change. Check
>   `game.stage`, `game.area` and the exit request first, then search the code for the new
>   coordinates as constants.
