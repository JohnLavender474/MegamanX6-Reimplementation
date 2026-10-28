# Notes: method, evidence, open questions

## Method

1. `python3 -m mmxdemake.rekit --name nightmare-insect`. It found the entry (`0x800ED704`, main
   table entry 23), 38 functions, 10 state tables, 2 neighbours (visual 27 at `0x800EDD2C`, misc 56
   at `0x800ECBAC`), 20 engine helpers by name, the animation table (16 animations) and the fields.
2. Read `summary.md`, `calls.txt`, then `code.txt` from the entry, function by function.
3. Read with `--at` the code outside the walk and the engine functions that the logic depends on:
   - `0x80030CCC` (called by the hatchling walk): a point test against the terrain;
   - `0x80031474` `ContactWeapon` and `0x800315CC` `ApplyWeaponHit`: to decode the return values
     −2, −11, −55 that the main state tests.
4. Read the boxes in `data.txt`, and the damage tables `0x8007479C`, `0x8007433C`, `0x800741FC` from
   `SLUS_013.95` (file offset = RAM − `0x80010000` + `0x800`).
5. Looked at the export (`~/mmx-data/x6/sprites/enemies/nightmare-insect/anim-*.png`) to name the
   animations, and computed the frame times from `animations.json`.

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | all 38 functions, annotated |
| `kit/data.txt` | boxes `0x800F7448`, `0x800F744C`, `0x800F76EC`–`0x800F7703`, the animation frames, the dispatch tables, the debris lists `0x800F7690`, `0x800F7694`, the start of the damage tables |
| `kit/summary.md` | helpers, tables, fields, animations |
| `kit/calls.txt` | call tree |

## Key facts and where they come from

- HP 32, contact damage 8, boxes, damage table `0x8007479C`, priority 5: `ni_init` `0x800ED760`.
- No movement: the main object has no `MoveObject` call, no terrain box, and its speeds are set to
  0 once.
- Attack choice: `ni_idle` `0x800ECA2C` (timer `slti 61`, `mega+0x02` test, `dx < 64`, hatchling
  count `< 4`). `mega.var` = 0 for X and non-zero for Zero (from the D-1000 notes).
- Hatchling count: the word mask `0x00FFFF00` = `0x00021B00` (type 27, var 2) over the 32 visual
  slots, inlined three times (`0x800ECAFC`, `0x800ECBB0`, `0x800ECC6C`).
- Laying: `lay_spawn` `0x800ECC38`: `slti 161`, count `< 8`, `old % 20 == 0` (magic number
  `0x66666667`, shift 3).
- Husk and revive: `ni_main` `0x800EC8F0` and `ni_revive` `0x800ED924` (animation 5 or 15 by
  `game.difficulty < 2`, HP 32, boxes 0, flag 3).
- Real kill ids: `ApplyWeaponHit` returns `nor v0,zero,s0` (= `~id`) on a kill; `s0` is the weapon
  object's type byte. −2, −11, −55 = ids 1, 10, 54.
- Shot speeds: `lui 0x0003` / `0xFFFD` = ±3 px/frame; `rcos * 80` = 5 px/frame; `rcos * 48` =
  3 px/frame; hatchling `0xFFFE8000` = −1.5 px/frame and `0x18000` = 1.5 px/frame, doubled on Hard.
- Timers: blade and boomerang `slti 91` (92 frames), hatchling walk `slti 241` (242 frames), burst
  `slti 51` (52 frames).
- Sound: `PlaySound(2, 62)` in `kid_init` for var 0 and 1.
- Drop: `DropItem(self, 21)` only on the real kill.

## Verified against the save state

- The export of `yammark-1_start.state.gz` shows the body (animations 0–5, 14, 15), the hatchling
  (6), the spinning blade (7) and the debris (8–13), as the code uses them.
- Not run in an emulator: the timings are from the code and the frame tables only.

## What the kit saved, and what stayed manual

- Saved: the functions, the 10 dispatch tables, the helper names, the field names in the main code,
  the animation table, the boxes in hex.
- Manual: the meaning of each function, the visual object's own work fields (`+0x7C` parent,
  `+0x8C` timer, `+0x8E`/`+0x90` target), the weapon id decode (needed two `--at` reads), the
  damage tables (from the executable), the timing sums.
- Kit problems:
  1. **False neighbour:** "misc type 56 at `0x800ECBAC`" is a branch target inside `ni_idle`
     (the `sb v0,5(s0)` after the Zero branch). The code makes no misc object (no `GetMiscObject`).
     The kit also printed that range a second time as a separate block. Work-round: ignored it.
  2. Accesses through the `mega` pointer are not named (`lb v0,2(a1)` with `a1` = `mega` is
     `mega.var`; `lh a1,10(a1)` is `mega.x.px`). Work-round: named by hand.
  3. The kit's `data.txt` dumps only 128 bytes per damage table, and the table length is not known
     (the engine accepts ids up to 58). Work-round: read 59 entries from the executable and tagged
     ids 40–58 [likely].

## Open questions

1. Which weapons are ids 1, 10 and 54?
2. Does the hatchling's 32 × 32 px burst box ever reach `ContactMega` (it is set in the last frame
   before the delete)?
3. Hang in BOOMERANG sub-step 2 when no visual slot is free at the throw [likely].
4. Does the engine mirror the off-centre boxes with `flip`?
5. Length of the damage tables.
6. `0x80030234` (the map lookup behind `0x80030CCC`) is not read.
