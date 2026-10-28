# Proposed doc changes (Nightmare Effect and infection, stage 7)

Written by the enemy-sheet task. I did not edit these files. Facts are from the stage 7 save states
and the disassembly (overlay 10 is the stage 7 overlay: 99.9 % word match at `0x800E9860`).
Probe scripts are in this folder.

## `docs/engine.md`

### Nightmare Effect (replace the bullet "Nightmare Effect.")

- **Nightmare Effect.** `nightmare_effect` (`0x800CD30A`, `game+0x43A`) selects the stage's effect.
  It is written once, at the stage start, by `0x8001E914` (store at `0x8001E9D0`): when
  `game.stage` < 19 and `game.area` = 0, `nightmare_effect` = `game+0x168`[stage] (`0x800CD038`, one
  byte per stage) [certain for the code]. The same function clears the stage time, the kill count
  and the damage total, and copies the 16 Reploid status words (`game+0xD8`) to `game+0x118`.
  What writes `game+0x168`[stage] was not found in the main program or overlay 10 [open].
- Values seen, bytes for stages 0–8 [certain for the values]:

  | save states | stages 0–8 |
  |---|---|
  | `yammark-1`, `-2`, `-3` | `0 0 0 1 0 0 0 1 0` |
  | `report-1`, `turtloid-*` | `0 6 0 1 0 6 0 1 0` |
  | `sheldon-*` | `0 6 7 1 0 6 7 1 0` |

  So the value in the Yammark and Turtloid states is 0, not 6 and 7 as the current text says. Each
  Maverick clear seems to write one value to two stages (6 to stages 1 and 5 after Yammark; 7 to
  stages 2 and 6 after Turtloid) [likely]. Value 1 is in stages 3 and 7 from the first save state.
- Objects that test the value [certain for the code]: 6 rain (`rain-effect`), 7 Nightmare shadows,
  8 the overlay 03 object at `0x800EE154`; stage 7: **1** the Nightmare Bug (main type 22,
  `docs/enemies/nightmare-bug.md`), **5** another object (state table `0x800FC6B8`, class-table entry
  `0x800FB7D4`, sprite bank 147, not read).
- The open question 7 ("What writes `nightmare_effect`") can say: written by `0x8001E914` from
  `game+0x168`[stage]; open: what writes that array.

### Weapon ids table

- Row "1, 10, 54": add "also the only ids that hurt the Nightmare Bug (4 damage)".

### Damage tables

Add after `dmg_tbl_nightmare_insect`:

`dmg_tbl_nightmare_bug` (`0x800746FC`): ids 1, 10, 54 = 4; ids 55, 59, 60, 63 and 64–79 no effect
(–); every other id deflected (D).

### ApplyWeaponHit

Add [certain, `0x800315CC`]: class 2 returns 0 (no hit; the shot is not stopped); class `0xFF`
calls `WeaponHitReaction(shot, −1)` and returns 127; class 7 and 8 also return 127.

### Game state table

- Add `+0x168`[stage] (`0x800CD038`): Nightmare Effect value per stage (see above).
- Add `+0x118` (`0x800CCFE8`): a copy of the 16 Reploid status words, made at the stage start
  [certain for the copy; the use is not known].

## `catalog/helpers.json`

See `helpers-proposed.json` in this folder.

## `docs/stages/sheldon.md` (owned by the stage-rip task)

### Hidden tiles: main type 32 (`0x800F3E2C`, overlay 10)

Found while reading the stage 7 main types. Kit: [`~/mmx-data/x6/re/sheldon-main-32/kit/`](../sheldon-main-32/kit/).

- Records (area 0): 9 of type 32. `id` 0 at (1648, 448), (1728, 448), (1824, 448), (2080, 448),
  (2176, 448), (2256, 448); `id` 1 at (3672, 992), (4408, 896), (4616, 896).
- Set-up [certain]: HP 50 (then 127 each update, so it never dies), no contact damage, priority 7,
  `display` 0 (hidden), sprite = `var & 1`, damage table `0x80074A1C` (class 2 for all ids except
  id 44: class 0, 1 damage). Terrain box by `var`: 0 = `0x800FC6D4` (0, 8, 34, 10): 68 × 20 px,
  centre 8 px below the origin; 1 = `0x800FC6D8` (0, 0, 26, 18): 52 × 36 px. Hurt box: 0 =
  (−40, 0, 80, 20), 1 = (−32, −24, 64, 48). No attack box.
- It does not call `SolidObject` and has no contact test: it only **draws** the piece. The solid
  part must be in the tile map [likely: the tile collision; not checked].
- Update [certain]:
  - When the timer `+0x7C` is 0: a hit by weapon id **44**, or the player's terrain box touching its
    terrain box (`TerrainBoxOverlap`; the player, or `actor2` when `mega+0xD5` ≠ 0) → shown, timer
    **150**. By a touch it also sets `+0x7E` = 1.
  - Each update with the timer ≠ 0: timer − 1. While the new value is 31 or more and `+0x7E` = 1:
    when the player no longer touches it, timer = **60** and `+0x7E` = 0 (60 more updates after
    the player steps off). Below 31 it blinks (shown on odd values). At 0 it is hidden.
  - So: touch → shown up to 150 updates; step off → 60 more updates; the last 30 updates blink.
    Touch again after it hides → shown again.
  - Off screen 80 × 160 (`OffScreenCheck2(80, 160)`) → state 2, `DeleteObject2`.
- Save states: `sheldon-5` slots 1, 3 hidden (`display` 0); `sheldon-6` slot 1 shown (`sub` = 1 is
  the `display` byte: the head `41 20 00 01`), the player at x 1661 on the piece at 1648.
- Open: which weapon is id 44.

### Other stage 7 objects seen

- Main type 27 (`0x800F1FD8`, records x 1656–2256, y 432, `id` 1): a hard-hat enemy (helmet and
  shield sprites, export `enemy-46d9e2249f`, 14 animations). HP 3, contact 1, `dmg_tbl_normal`, uses
  `dmg_tbl_deflect` in one step (`0x800F1C10`; probably under the helmet [guess]), makes visual
  type 32 (probably its shot [likely]), drop table 18 (no drop for a kill by id 55); `var` 2 and 3
  start in state 3 with no boxes, snapped to the 16 px grid (x + 8, y + 20). Not read further; the official name is to
  be checked (the Fandom X6 enemy list).
- Stage-logic type 7 (`id` 0): its x follows the player (1603 → 1661). Not read.

## `docs/enemies/nightmare-virus.md` (done in this task)

Added the stage 7 records. Observed: the snatcher record at (1768, 352) has kind `0x10` in
`sheldon-5`/`-6` and `0x20` in `sheldon-7` (the high nibble changed); the code that makes `0x20` was
not read.
