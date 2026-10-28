# Notes: method, evidence, open questions

## Method

1. `python3 -m mmxdemake.rekit --name killer-moth`. It found the entry (`0x800EB2C8`, main table
   entry 4), 32 functions, 8 state tables, 2 neighbours (visual type 2 = powder shot, misc type 4 =
   exhaust flame), 21 engine helpers by name (no unnamed call outside the walk), the animation table
   and the fields used.
2. Read `summary.md`, `calls.txt`, then `code.txt` from the entry, function by function.
3. Read the boxes, the delay table and the start-point table in `data.txt`, and the damage table
   `0x800743DC` from the executable (file offset `0x64BDC`; 64 weapon ids, the same bytes as in RAM).
4. Looked at the export (`~/mmx-data/x6/sprites/enemies/killer-moth/`, `anim-0.png`–`anim-7.png`) to
   name the animations.
5. Read the stage object list from the save state (`mmxdemake.objects.read_list`, table entry 1 as in
   `catalog/stages.json`) to see which record ids the stage uses.
6. Looked in `~/IdeaProjects/mmx-reference/` for facts on this enemy: nothing found.

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | all 32 functions, annotated |
| `kit/data.txt` | boxes `0x800F7180`, `0x800F7184`, `0x800F72F8`, `0x800F72FC`; animation frames; the dispatch tables; delays `0x800F729C`; start points `0x800F72AC`; the damage table |
| `kit/summary.md` | helpers, tables, fields, animations |

## Key facts and where they come from

- HP 8, contact damage 4, the boxes, priority 5, damage table: `moth_init` (`0x800EB37C`) and
  `data.txt`.
- id 0 → HOVER with boxes and flame mode 1; id ≠ 0 → ENTER without boxes: the `var` test at
  `0x800EB3A8`.
- HOVER timer 180, player distance 48 px (x only): `hover_start`, `hover_wait` (`0x800EB644`).
- The double increment in `hover_wait`: both paths reach `0x800EB674`, so the distance test runs
  after the timer has added 1.
- SWOOP speeds: `swoop_move` (`0x800EAE94`): `cos × 80 × 2 / 4` and `sin × 80 × 2 / 8`, where 4096 =
  1.0, so 163840 (`0x28000`, 2.5 px/frame) and 81920 (`0x14000`, 1.25 px/frame) at full. Target
  offset 48 (X) / 64 (Zero) by `mega+0x02`; snap test `< 3` px.
- CIRCLE: centre −16 px, angle 3072, radius 16 (`<< 4 >> 12`), step ±32 (`+0x84`), 20-update shot
  timer, 8 shots: `circle_start` (`0x800EB7E4`), `circle_move` (`0x800EB870`), `swoop_move`'s end.
- ENTER: view test ±160 / ±120 round the camera (the spawn point inside the 320 × 240 view), delay
  table, start-point table, 1.25 px/frame homing: `0x800EB0A0`, `0x800EB9C8`, `0x800EB170`.
- Powder: `speed_y` = `0xFFFFC001` (−16383, −0.25 px/frame), contact 4, boxes, removal rules:
  `0x800EBC50`, `0x800EBCEC`.
- Flame: mode → animation map 1 → 1, 2 → 3, 3 → 5, 0 → hidden: `flame_follow` (`0x800EBE14`).
- Death: `SpawnExplosion` once, `DropItem(self, 18)`: `moth_main`.
- The flame mode `+0x88` and the circle work `+0x80`–`+0x8C` are written through `current_main`
  (`0x1F80004C`), the pointer to the main object being updated, so they are the moth's own fields.

## Verified against the save state

- The stage object list (table entry 1, 108 records, overlay 03 offset 51976) has 8 records of kind 0,
  `b` 4 (Killer Moth), all with id 0, at (1440, 576), (1744, 1712), (2032, 1696), (2192, 1680),
  (2480, 1680), (2592, 1808), (2656, 1760), (2688, 1760).
- No Killer Moth is live in `yammark-1_start.state.gz` (no main slot of type 4), so no live field
  values were compared. The player is X (`mega.var` = 0).
- The export shows the moth poses (0, 2, 4, 6), the three flames (1, 3, 5) and the powder (7), as the
  code uses them.

## What the kit saved, and what stayed manual

- Saved: all functions and their order, the 8 dispatch tables, every helper name, most field names,
  the animation table, the boxes and tables in hex.
- Manual: the meaning of each function; the X/Zero meaning of `mega+0x02`; the stage record ids.

## Kit problems

1. Fields written through `current_main` (`+0x80`, `+0x84`, `+0x88`, `+0x8A`, `+0x8C`) are not in the
   "Object fields used" table and not named in `code.txt` (the lines show raw offsets on the loaded
   pointer, for example `sb v0,136(v1)`). Worked around by reading each `lw ...,76(...)` +
   `current_main` pair by hand.
2. Accesses through a copy of `self` in another register are not annotated: in `circle_move`
   (`0x800EB870`) `self` is in `s2`, so `sh v1,10(s2)` (x.px) and `+0x7C`/`+0x7E` have no names.
   Player fields read through a base register (`lh v1,10(a2)` with `a2` = `mega`) also have no names.
3. The animation note says "cut to the export's 8 animations (the table runs on for 4 more)". The 4
   extra words at `0x800F7254` are the state table (code addresses `0x800EB37C`, ...), not
   animations. The pointer list has exactly 8 entries.

## Open questions

1. Which object or record uses id ≠ 0 (the ENTER variant)? All 8 records in this stage have id 0.
2. Whether the engine ever sets step 0 (hit reaction) for this enemy.
3. The stuck-hover quirk (timer end and player within 48 px in the same update) is not seen in play.
4. `latch` = 1 in the powder shot: meaning not read.
5. Whether `ContactMega` on a visual object damages the player (assumed).
6. Weapon ids other than 0, 9, 18–20, 24–31.
7. Which draw priority is in front (moth 5, flame 6, powder 4).
8. The flame offset relative to the body: the flame takes the moth's position; any offset is in the
   sprite data (not read).
