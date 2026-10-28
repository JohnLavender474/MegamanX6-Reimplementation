# Proposed changes from the Nightmare Snake work

The new sheet is `docs/enemies/nightmare-snake.md` (the text is `sheet.md` in this folder). These
changes are for files that other owners keep.

## Shared sprites (`~/mmx-data/x6/sprites/`)

Run `spriteexport --state heatnix-2_mini-boss.state.gz` on a copy of `sprites/` first, as
`CLAUDE.md` says. The check in `sprites-root/` gives:

- Keep `enemies/enemy-6e15315f49` (main 25, 13 animations): all strips checked, correct.
- Keep `enemies/enemy-7f9bbc9a47` (main 36, 5 animations, fire jets of the stage; strips look
  complete; not named yet).
- `enemies/enemy-c1aea0bc4c` (the stage 3 Rescue Reploid, stage object 4 live): animations 26, 27
  and 33 are wrong (set-B animations drawn with the sprites of this live source; see
  `docs/enemies/rescue-reploid.md`, set B). Keep it with a note, or delete those three strips and
  list them in `stages/heatnix.json` as skipped ("set-B sprites not in this source").
- The other five IDs already exist; the run only adds a stage 3 source to them.
- `sprites-root/ring-layer1.png` (158 × 144): the ring of the body, cut from the layer 1 art of
  `stages/m097-blaze-heatnix-mini-boss/stage.ora`. It is not a `spriteexport` sheet. Copy it, if
  wanted, to `enemies/nightmare-snake/ring-layer1.png` with a line in the sheet; its origin (the
  body point) is layer px (512, 512), so pixel (79, 80) of the crop.

## `catalog/enemies.json`

- `stages`: add `"3": "heatnix"`.
- `names`: add `"enemy-6e15315f49": "nightmare-snake"`.
- Optional: `"enemy-c1aea0bc4c": "rescue-reploid-heatnix"` (after the decision above).

## `catalog/helpers.json`

Add the 24 functions and 13 globals of `helpers-proposed.json` (all `overlay` 5 except
`dmg_tbl_nightmare_snake`, which is in the main program). No name or address clashes with the
current file (checked).

## `docs/engine.md`

- **Stage overlays** (the paragraph that starts "Stage 4 has two overlays"): change
  "stage 4 = 06 and 07, stage 5 = 08, stage 6 = 09, stage 7 = 10 [certain for 02, 03, 06, 07, 09,
  10]" to "stage 3 = 05, stage 4 = 06 and 07, stage 5 = 08, stage 6 = 09, stage 7 = 10 [certain for
  02, 03, 05, 06, 07, 09, 10; 05: `rock_bin/05.bin` matches RAM in `heatnix-2_mini-boss`, 4 of 44084
  bytes differ]".
- **A boss on a BG layer** (the paragraph after "Damage passed to a parent"): add: "Nightmare
  Snake (overlay 05, main 25) is a ring on BG layer 1 (mode 5): each update the body writes layer 1
  = (512 − x, 512 − y) and the redraw request; the layer is drawn relative to the layer 0 camera, so
  layer px (512, 512) stays on the body point. The layer is shown (`cam1+0x03`) only while the body
  is within 96 px of the view [certain]."
- **Damage passed to a parent**: add: "Nightmare Snake does it differently: the body sets its HP to
  0 each update and each gem adds its own HP (a byte add; −128 is changed to 127). So the bar shows
  the sum of the gem HP. The body dies when its four gems are dead (a counter in `+0x07`), not by HP
  [certain for the code]."
- **The damage tables**: add after `dmg_tbl_nightmare_shadow`:

  ```
  `dmg_tbl_nightmare_snake` (`0x800748DC`, the gems of Nightmare Snake,
  `docs/enemies/nightmare-snake.md`):

  | ids | damage |
  |---|---|
  | 0–15 | 1 2 5 D 5 5 4 – 2 1 4 8 D 8 8 8 |
  | 16–31 | 2 8 4 4 4 D 8 D 3 3 5 2 2 2 2 2 |
  | 32–47 | 2 5 D 5 6 3 2 2 2 2 4 2 8 1 4 8 |
  | 48–63 | 4 4 3 3 1 5 1 127 3 8 4 4 15 5 15 3 |
  | 64–79 | D × 15, then 127 (id 79) |
  ```

- **Weapon ids**, row "3, 12, 34": add "also deflected by `dmg_tbl_nightmare_snake` (the fire ids of
  the stage)".
- **Game state**, row `+0x26`: add "Stage 3 area 0: Nightmare Snake writes 0 at its set-up and 1 at
  its end (overlay 05, `0x800EB28C`, `0x800EC5C0`); boss gate `id` 7 at (944, 608) then opens."
- **Death, debris and drops**: Nightmare Snake has no drop and no Nightmare Soul.
- **The player**, events: add Nightmare Snake to the users of `PlayerEvent(20, …)` (`var` 5: facing
  64; `var` 4: facing 0); it does not call `PlayerEvent(29, …)` and does not make engine object 24.
- **Camera**, the open item "Code at `0x800503D8`–`0x80050470` … lowers layer 1 `+0x42` for an object
  whose `+0x02` is 25 or 39": no change, but note that main type 25 of stage 3 is Nightmare Snake
  (the value tested there is `+0x02`, the gate `id`, not a type) [guess; not read].

## `docs/enemies/boss-gate.md`

- Special behaviour table, row `id` 7: "at the trigger: `ClearEnemies`, then a main object type 25
  with `var` 4 (no null test): the next Nightmare Snake (stage 3 area 0,
  `docs/enemies/nightmare-snake.md`)".
- Open question 1: "`game+0x26` in stage 3 area 0: set to 1 by the end of Nightmare Snake `var` 5
  (the gate `id` 7 at (944, 608) of the fixed record list)".
- Open question 6: remove "main type 25 (id 7)" (answered: Nightmare Snake).

## `docs/stages/heatnix.md` (new, stage agent)

- Area 0 (member 97, overlay 05, 18 × 13 screens, 138 records). The fixed record list
  (`fixed_set_tbl`) has three entries: main 25 `id` 5 at (448, 512) (Nightmare Snake, first fight),
  main 25 `id` 4 at (1248, 2624) (a later Nightmare Snake; the gate makes it again), stage 8 `id` 7 at
  (944, 608) (the boss gate after the first fight).
- Records 22–26: main 25 `id` 6, 7, 9, 8, 10 (more Nightmare Snake fights).
- The first snake room: floor y 640 with three 32 px deep dips, walls x 320–352 (from y 432) and
  928–960; camera limits x 336–624, y 480; Rescue Reploid `id` 32 (record 45) at (624, 656). The
  fight starts when the player passes x 363 or y 464; the player is kept at y ≥ 432 until it passes
  x 944.

## `docs/TODO.md`

- Save states of the other Nightmare Snake fights (`var` 4, 6, 7, 8 or 9, 10) to check the code-only
  rows of the sheet.
- Play check: the roll cycle (about 576 updates); whether the ring hurts the Rescue Reploid.
- `spriteexport`: a live stage object 4 with other sprite data than the set-B rule expects gives
  wrong animations 26, 27, 33 (`enemy-c1aea0bc4c`).

## `docs/process/process-log.md`

One line: "Nightmare Snake (stage 3 area 0 mid-boss, overlay 05 main 25 + visual 29): sheet,
pseudo code, helper names; overlay 05 base confirmed; `dmg_tbl_nightmare_snake`."
