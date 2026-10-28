# Notes: method, evidence, open questions

## Method

1. `python3 -m mmxdemake.rekit --name meta-wheel-f` (first pilot of the RE kit). It found the entry
   (`0x800EBC24`, main table entry 2), 18 functions, the 3 state tables, the stud overlay (misc
   entry 2, a neighbour), 17 engine helpers by name, the animation table and the fields used.
2. Read `summary.md`, `calls.txt`, then `code.txt` from the entry, function by function.
3. Read the boxes and the stud table in `data.txt`, and the damage table `0x8007447C` from the
   executable.
4. Looked at the export (`~/mmx-data/x6/sprites/enemies/meta-wheel-f/`) to name the animations.

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | all 18 functions, annotated |
| `kit/data.txt` | boxes `0x800FAB7C`–`0x800FAB87`, animation frames, the dispatch tables, the stud table `0x800FAD30` |
| `kit/summary.md` | helpers, tables, fields, animations |

## Key facts and where they come from

- HP 32, contact damage 4, the three boxes: `wheel_init` constants and `data.txt`.
- 0.75 px/frame cap and 0.047 px/frame² acceleration: `wheel_main` (`0xC000`) and the `accel_x`
  writes (`-0xC00`).
- TURN timing 1 + 90 + 20 updates: `turn_start`, `turn_wait`, `turn_flip`.
- Squeeze death: `wheel_main` tests `collide_flags & 0x0C == 0x0C`.
- Wheel against wheel: `wheel_pair_test` walks the main slots after `self` to `0x80090C88`, the end
  of the 48 main slots.
- Sabre only: damage table `0x8007447C` deflects ids 0–23.

## Verified against the save state

- The export of the intro start state shows the wheel (anims 0, 2), the stud lights (1, 3–7) and the
  debris (8–10), as the code uses them.

## What the kit saved, and what stayed manual

- Saved: finding the functions and their order, the dispatch tables, every helper name, the field
  names on almost every line, the animation table, the boxes in hex.
- Manual: the meaning of each function, the switch tables in `turn_flip` (the kit lists them as data,
  not as code), the pair test's arguments (the kit marks the first argument as the object, but here
  `a1` is the wheel and `a0` the other one).
- Found a kit gap: a constant whose `lui` is more than 5 instructions before its use is not annotated
  (`mega.layer` at `0x800EB47C`).

## Open questions

1. A wheel landing on another one: the squeeze rule says the lower one dies; check in play.
2. Animation 3 has no user.
3. Weapon ids 32–39.
