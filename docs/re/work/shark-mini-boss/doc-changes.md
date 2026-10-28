# Proposed changes from the Nightmare Pressure work

The new sheet is `docs/enemies/nightmare-pressure.md`. These changes are for files that other owners
keep.

## `catalog/enemies.json`

- `names`: `"enemy-74e4e39589": "nightmare-pressure"` (the export of main 16 and stage 10 from
  `shark-4_mini-boss`, table `0x800F8638`, 17 animations, cell 255 × 120).

## `catalog/helpers.json`

Add the entries of `helpers-proposed.json` (`GetMainObjectAfter`, `SolidWeaponHit`,
`SolidPushShots`, and five overlay 07 names).

## `docs/engine.md`

- **Stage overlays**: overlay 07 is stage 4 area 1, at `0x800E9860` [certain now: `rock_bin/07.bin`
  matches RAM in `shark-4_mini-boss` and `shark-5_after-mini-boss`, 150 / 152 bytes differ]. Move
  06 and 07 to the "certain" list.
- **Damage to an object**, new paragraph: "**Damage passed to a parent.** Nightmare Pressure's
  parts (overlay 07, main 16) give their parent `old hp − new hp` (a signed byte) after each
  `ContactWeapon`. Because `ApplyWeaponHit` lets the HP go below 0, the parent takes the full damage
  of the killing hit, also past the part's HP."
- **Camera**, new item: "**A boss on a BG layer.** Nightmare Pressure (overlay 07) is drawn on BG
  layer 1 (mode 5: no automatic position). Its code moves `cam1.y` (16.16 at `cam1+0x0C`) to move
  the machine; its objects are at `x = cam0.x + cam1.x + ox`, `y = cam0.y + 952 − cam1.y + oy`."
- **Game state**, `+0x26` (stage 4): "Nightmare Pressure's death also writes 1 (overlay 07,
  `0x800EB60C`); `shark-5_after-mini-boss` has 1."
- **Weapon ids**: no new id. The parts use `dmg_tbl_normal`: no stun and no special weakness.
- **Death, debris and drops**: Nightmare Pressure has no drop and no Nightmare Soul.
- **The player**, events: add Nightmare Pressure to the users of `PlayerEvent(20, …)` (intro with
  facing 64, death with the player's own facing). It does not call `PlayerEvent(29, …)` and does not
  make engine object 24.
- **Shots and attached objects**: engine object 26 is also used by Nightmare Pressure, made at the
  room middle (`M(160, −40)`); the death code waits for its `+0x07` ≠ 0, then moves the cameras and
  X, then waits for the object to end.

## `docs/stages/shark.md` (stage agent)

- Area 1, record 75 (first list): main 16 `id` 0, kind 0 at (160, 1064): **Nightmare Pressure**
  (`docs/enemies/nightmare-pressure.md`). The room: camera fixed at (10, 944) in
  `shark-4_mini-boss` (limits 10–10, 944–944).
- After the death the code itself (not a camera zone) sets: checkpoint 2; cameras (5456, 224) for
  layers 0 and 1 and (2728, 224) for layer 2; all limits 5456 / 224; X at (5604, 395); records in
  x 5408–5824, y 224–512 spawned (the boss gate `id` 21 at x 5776 is live in `shark-5`). The white
  screen is engine object 26 (its `+0x07` gives the moment of the move).
- Stage objects 10 of area 1 are made by Nightmare Pressure, not by records.

## `docs/TODO.md`

- Play check: the press gap (40 px; X 38 px tall) and the drill-ram safe spot.
- Open: who reaches Nightmare Pressure step 9 (END CRUSH, `cam0.x` ≥ 5440).
- A save state during the BEAM, the DRILL and the PRESS would confirm the boxes on screen.

## `docs/process/process-log.md`

One line: "Nightmare Pressure (stage 4 area 1 mini boss, overlay 07 main 16 + stage 10 + visual 13):
sheet, pseudo code, C; overlay 07 base confirmed."
