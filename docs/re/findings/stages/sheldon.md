# Stage: Shield Sheldon (stage 7, overlay 10)

Overlay `10` at RAM `0x800E9860`. Stage member 102, a layout of 24 × 8 screens. Save states (all
stage 7, area 0, difficulty 1, in `~/mmx-data/x6/dumps/`; checkpoint 0 unless the table says
other):

| state | player | X at (x, y) | camera x | what |
|---|---|---|---|---|
| `sheldon-1_lemons` | X, form 1 (Falcon) | 160, 427 | 0 | start, lemon shots |
| `sheldon-2_half-charge` | X, form 1 | 160, 427 | 0 | start, half-charge shot |
| `sheldon-3_full-charge` | X, form 1 | 160, 427 | 0 | start, full-charge shot |
| `sheldon-4_no-armor` | X, form 0 (no armour) | 160, 427 | 0 | start |
| `sheldon-5_nightmare-hidden` | X, form 0 | 1574, 427 | 1414 | before the hidden glass floors; floors not shown |
| `sheldon-6_tile-shown` | X, form 0 | 1660, 427 | 1501 | X stands on the hidden floor at x 1648; it is shown |
| `sheldon-7_lasers` | X, form 0 | 3034, 427 | 2874 | in the laser puzzle; laser gate `0x50` open |
| `sheldon-8_lower-1` | X, form 0 | 3931, 843 | 3840 (y 711) | checkpoint 2; the lower rooms, after floor gate `0x55` |
| `sheldon-9_lower-2` | X, form 0 | 3520, 955 | 3408 (y 752) | checkpoint 2; the lower-left room, at the Light capsule |
| `sheldon-13_door-open` | X, form byte 1 | 3121, 427 | 2961 | laser door `0x50` has just opened |
| `sheldon-14_before-lower` | X, form byte 1 | 4655, 427 | 4495 | checkpoint 1; above floor door `0x54` (open), before the lower rooms |
| `sheldon-15_lower-wall` | X, form byte 1 | 4716, 683 | 4480 (y 496) | checkpoint 1; the first lower room, under the closed door `0x54` hole |
| `sheldon-16_death-guardian` | X, form byte 1 | 4032, 683 | 3872 (y 496) | checkpoint 1; the first lower room, the Death Guardian live |
| `sheldon-17_boss` | X, form byte 1 | 5222, 427 | 5152 | checkpoint 3; the boss room, Shield Sheldon live |

The hidden area behind the teleporter crystal is stage 22, a section of its own:
[`sheldon-alt.md`](sheldon-alt.md) (states `sheldon-10` to `sheldon-12`).

The player position is the halfwords `mega+0x1A` (x) and `mega+0x1E` (y). `mega+0x14` is not a
position. All seven states have X standing (step 2, air latch 0). `game+0x5E` (the form) is 0 in
states 4–7: form 0 is X with no armour [certain for the byte; the user says that X has no armour].
In states 13–17 the byte is 1 (the Falcon Armor value of states 1–3), and `game+0x69` is `0x0C`. The
user said "no armour" for these states; the byte does not agree [certain for the byte; which is
correct is open]. In states 13–17 X stands (step 2), except in `sheldon-16` (step 17).

The stage map is extracted with `learn` and `stages` (catalog entry "Shield Sheldon, section 1",
from `sheldon-1_lemons`): `~/mmx-data/x6/stages/m102-shield-sheldon-section-1/` has `stage.png`,
`collision.png`, `objects.json`, `summary.txt` and `stage.ora`. The `learn` fit test of states
13–17 (into a copy of the catalog) passes with no tool change: member 102, overlay 10 at
`0x11440`, edges 0.975 / 0.792, 0 differing cells, 114 records at `0x11680`; texture `0x10000` in
state 13, texture `0x16` in states 14–17. No new catalog entry was needed. States 1–7 learn with 0
differing cells, except `sheldon-6_tile-shown`, where the palette step fails ("no camera for any
layout layer"; the layout, the member and the objects are the same) [likely cause: the frame was
saved in the middle of a redraw].

The same map has a second texture block. A second catalog entry, "Shield Sheldon, section 1, lower
rooms" (from `sheldon-8_lower-1`, `"texture": 22`), draws the same layout with it:
`~/mmx-data/x6/stages/m102-shield-sheldon-section-1-lower-rooms/`. Use each drawing for the part
that it shows correctly (see "Two texture blocks" below). The layout, the collision and the 114
records are the same in both entries; only the art differs.

`learn` first refused this stage: the vertical edge score was 0.282. The layout is correct (X stands
on solid floor). Many floors and ceilings of this stage end exactly at a screen edge, so a full
solid row faces an empty row. The edge test now skips a vertical join when one of the two edge rows
is all solid or all empty; it still needs 3 scored joins (`mmxdemake/ramlayout.py`). Score after the
fix: horizontal 0.975 over 54 joins, vertical 0.792 over 6 joins.

The record format and the kinds are in `docs/objects-legend.md`; the shared rules in
`docs/engine.md`.

## The map

Layer 0 uses columns 0–21 and rows 0–4. From left to right [certain for the tiles; the names are
from the art]:

| x (px) | y (px) | part |
|---|---|---|
| 0–1599 | 256–511 | outside: a bridge with domes over water; the start (X at x 160) |
| 1600–2303 | 448–463 | a gap with six hidden glass floors (below) and one pillar (x 1904–1983) |
| 2304–4799 | 256–511 | the museum corridor; the laser puzzle (x 3072–4800) |
| 3400–4860 | 512–1023 | the lower rooms, under the corridor; reached through the floor gates |
| 4800–5470 | 256–511 | the corridor to the boss: boss gate `id` 36 (x 5152), Shield Sheldon |

Row 0 (y 0–255) is never in view (camera y is 256 or more). It holds tile templates: the empty cells
that the laser gates copy (see "Camera").

Collision values in layer 0 (cells): 0 (45 732), 56 (2 430), 57 (844), 59 (68), 60 (24), 63 (54).
Value 60 is new: only the hidden glass floors use it. Value 59 is only on the six laser gates.

## Objects

114 records (list entry 7, area 0), by class and type [certain for the counts]:

| class, type | records (kind 0 / 2) | object |
|---|---|---|
| main 13 | 33 / 3 | the laser puzzle: beamers, reflectors and the 6 laser gates (`docs/enemies/laser-reflector-puzzle.md`, below) |
| main 15 | 5 | Nightmare Virus (`id` 34 ×3, 33, 0) |
| main 22 | 11 | Nightmare Bug, the Nightmare Effect of this stage (`docs/enemies/nightmare-bug.md`) |
| main 27 | 4 | Mettaur D2 (`docs/enemies/mettaur-d2.md`); all `id` 1, at y 432, on the hidden floors 73, 75, 76 and 78: each one paces on one hidden floor and turns at its ends, so it shows where the floor is |
| main 28 | 1 | teleporter crystal `id` 1 at (4744, 928): to the hidden area, stage 22 ([`sheldon-alt.md`](sheldon-alt.md)) |
| main 30 | 12 / 6 | Guardian (`docs/enemies/guardian.md`); `id` 0 faces left (12), `id` 1 faces right (5), `id` 16 is the Death Guardian (sprite bank 8) at (3928, 664), live in `sheldon-16` |
| main 32 | 9 | hidden glass floor (`id` 0) and hidden brick block (`id` 1), below |
| main 47 | 1 | item, `id` 6, at (4512, 960) |
| main 51 | 1 | Shield Sheldon at (5280, 336) (`docs/enemies/shield-sheldon.md`); confirmed by the live object in `sheldon-17_boss` (the last record of the list) |
| stage object 4 | 8 | Rescue Reploids, `id` 96–103 |
| stage object 8 | 1 | boss gate `id` 36 at (5152, 384) |
| stage object 15 | 7 | Nightmare crate, the Nightmare Effect 5 object (`docs/enemies/nightmare-crate.md`, below); all `id` 3 (red, explodes), x 4024–4472; not live in any state (the effect is 1) |
| stage object 26 | 1 | Light capsule `id` 1 at (3456, 968), in the lower-left room |
| stage logic 0 | 7 | camera zones (below) |
| stage logic 6 | 3 | checkpoint markers (below) |
| misc 14 (kind 5) | 1 | at (40, 415), near the start; not read |

No main enemy is live in states 1–4. The Rescue Reploid `id` 97 is infected in `sheldon-5` and
`sheldon-6` (`docs/enemies/rescue-reploid.md`, "Stage 7").

The Nightmare Effect of this stage in all seven states is value 1 (the Nightmare Bugs). The pick of
Commander Yammark's stage wrote it; the later pick of Rainy Turtloid's stage did not change it but
replaced the stage select highlight, so this stage had the effect but no purple mark
(`docs/engine.md`, "Nightmare Effect").

**Nightmare Effect 5 in this stage: the crates** [certain for the code]. The value-5 object of
stage 7 (state table `0x800FC6B8`, class-table entry `0x800FB7D4`, sprite type 147) is stage object
15, the same object as the falling crates of stage 4: the code of overlay 10 (`0x800F2600`–
`0x800F3C9C`) is the same, instruction for instruction, as that of overlay 06 (stage 4, entry
`0x800EE050`) and overlay 31 (stage 22, entry `0x800F1DC4`); only the data addresses differ, and the
boxes and the debris tables have the same bytes. The set-up (`0x800F2734`) deletes the object unless
`nightmare_effect` is 5. Value 5 comes from the pick of stage 5 (Ground Scaravich's stage; stage 5 in
`scaravich-1_start`): its targets `4 7 7` give stages 4 and 7 the value 5. The 7 records of this stage
are all `id` 3: the red crate that explodes. Behaviour: `docs/enemies/nightmare-crate.md`.

### Laser puzzle records (main type 13)

Roles from `docs/enemies/laser-reflector-puzzle.md`. Kind 2 = Hard and above only.

| record | x, y | `id` | kind | part |
|---|---|---|---|---|
| 43 | 3080, 304 | `0x15` | 0 | aiming beamer, starts down-left |
| 44 | 3080, 408 | `0x3F` | 0 | fixed reflector, r 15 |
| 82 | 3152, 360 | `0x50` | 0 | door 0 |
| 45 | 3360, 288 | `0x16` | 0 | aiming beamer, starts down |
| 46 | 3600, 288 | `0x16` | 0 | aiming beamer, starts down |
| 47 | 3760, 448 | `0x30` | 0 | fixed reflector, r 0 |
| 83 | 3808, 408 | `0x51` | 0 | door 1 |
| 48 | 3904, 296 | `0x06` | 0 | fixed beamer, down |
| 49 | 3904, 440 | `0x3E` | 0 | fixed reflector, r 14 |
| 50 | 4064, 424 | `0x40` | 0 | turning reflector: r 9, 5, 1, 13 |
| 84 | 4144, 360 | `0x52` | 0 | door 2 |
| 51 | 4216, 280 | `0x00` | 0 | fixed beamer, right |
| 52 | 4440, 280 | `0x41` | 0 | turning reflector: r 15, 13, 11, 9, 7, 5, 3, 1 |
| 53 | 4528, 432 | `0x02` | 2 | fixed beamer, up (Hard) |
| 54 | 4576, 440 | `0x42` | 0 | turning reflector: r 12, 8, 4, 0 |
| 55 | 4656, 328 | `0x43` | 0 | turning reflector: r 11, 10, 9, 8, 7, 8, 9, 10 |
| 85 | 4800, 376 | `0x53` | 0 | door 3 |
| 86 | 4728, 464 | `0x54` | 0 | door 4 (opens downward) |
| 56 | 4776, 520 | `0x04` | 0 | fixed beamer, left |
| 57 | 4664, 520 | `0x3B` | 0 | fixed reflector, r 11 |
| 58 | 4776, 616 | `0x44` | 0 | turning reflector: r 0, 12, 8, 4 |
| 59 | 4664, 696 | `0x45` | 0 | turning reflector: r 9, 7, 5, 3, 1, 15, 13, 11 |
| 60 | 4584, 680 | `0x22` | 0 | relay beamer, starts up |
| 61 | 4488, 584 | `0x46` | 0 | turning reflector: r 0, 12, 8, 4 |
| 62 | 4400, 696 | `0x47` | 0 | turning reflector: r 8, 6, 4, 2, 0, 14, 12, 10 |
| 63 | 4344, 608 | `0x48` | 0 | turning reflector: r 0, 14, 12, 10, 8, 6, 4, 2 |
| 64 | 4280, 696 | `0x49` | 0 | turning reflector: r 3, 15, 11, 7 |
| 65 | 4288, 528 | `0x26` | 0 | relay beamer, starts down |
| 66 | 4200, 616 | `0x4A` | 0 | turning reflector: r 0, 14, 10, 9, 8, 6, 4, 2 |
| 67 | 4168, 552 | `0x4B` | 0 | turning reflector: r 12, 7, 3, 15, 10, 7, 3, 15 |
| 68 | 4096, 528 | `0x06` | 2 | fixed beamer, down (Hard) |
| 69 | 4008, 528 | `0x06` | 2 | fixed beamer, down (Hard) |
| 70 | 3944, 568 | `0x4C` | 0 | turning reflector: r 0, 14, 12, 10, 8, 6, 4, 2 |
| 71 | 4240, 700 | `0x02` | 0 | fixed beamer, up |
| 72 | 4160, 696 | `0x04` | 0 | fixed beamer, left |
| 87 | 3928, 720 | `0x55` | 0 | door 5 (opens downward) |

Rooms: door 0 spawns 3168–3791 × 256–447 (records 45, 46; 47 is just below and spawns normally),
door 1 spawns 3824–4127 × 256–447, door 2 spawns 4160–4783 × 256–447, door 4 spawns 3856–4783 ×
480–703, door 5 spawns 3856–4767 × 736–991. The route is: room 0 (x 2896–3167) → door 0 → room 1 →
door 1 → room 2 → door 2 → room 3 (x 4160–4815) → door 3 or door 4 (down) → the lower rooms (y
480–735) → door 5 (down) [likely for the route, from the rectangles].

### Checkpoints (stage logic 6)

| record | `id` | test | checkpoint after |
|---|---|---|---|
| 0 | 1 | x ≥ 4176 | 1 |
| 1 | 130 | y ≥ 832 (bit 7: y test) | 2 |
| 2 | 3 | x ≥ 4944 | 3 |

## The hidden glass floors (main type 32)

**What they are** [certain for the code and the tiles]. Six floors of 4 tiles (64 px) and three
blocks of 3 × 2 tiles. The collision is in the map all the time, also while the floor is not
shown. The object only draws the floor. It changes no tile.

| record | `id` | x, y | collision cells | sprite |
|---|---|---|---|---|
| 73 | 0 | 1648, 448 | x 1616–1679, y 448–463, value 60 | 0 (glass floor) |
| 74 | 0 | 1728, 448 | x 1696–1759, y 448–463, value 60 | 0 |
| 75 | 0 | 1824, 448 | x 1792–1855, y 448–463, value 60 | 0 |
| 76 | 0 | 2080, 448 | x 2048–2111, y 448–463, value 60 | 0 |
| 77 | 0 | 2176, 448 | x 2144–2207, y 448–463, value 60 | 0 |
| 78 | 0 | 2256, 448 | x 2224–2287, y 448–463, value 60 | 0 |
| 79 | 1 | 3672, 992 | x 3648–3695, y 976–1007, value 56 | 1 (brick block) |
| 80 | 1 | 4408, 896 | x 4384–4431, y 880–911, value 56 | 1 |
| 81 | 1 | 4616, 896 | x 4592–4639, y 880–911, value 56 | 1 |

- The map art in these cells is a black tile (tile word `0x4039`), so the map alone does not show
  the floors. The sprites are sprite 0 and 1 of bank 11 (two sprites:
  `~/mmx-data/x6/sprites/banks/stage-7/*/bank-11.png`).
- **Collision 60 is solid floor.** In `sheldon-6_tile-shown`, X stands on it at x 1660 by the
  tiles: step 2, air latch 0, and `mega+0x72` bit `0x08` ("on an object") is 0. X stands at the same
  height (y 427) as on value 56. In `sheldon-5_nightmare-hidden`, the floors are live but not
  shown, so they are solid while not shown [certain for the tiles; X did not stand on a hidden floor
  in a save state while it was not shown].
- The code: entry `0x800F3E2C`, state table `0x800FC6E4` (set-up `0x800F3E78`, update
  `0x800F3CA0`, delete `0x800F3F34`). It is not solid (no `SolidObject`). `display` (`+0x03`)
  is the "shown" flag. The set-up puts `var & 1` in the sprite byte `+0x47` and picks the boxes
  (table `0x800FC6D4` and `0x800FC6DC`):

  | `id` | terrain box (centre offset, half size) | hurt box |
  |---|---|---|
  | 0 | (0, 8), 34 × 10 | x −40…40, y 0…20 |
  | 1 | (0, 0), 26 × 18 | x −32…32, y −24…24 |

  The set-up also writes the state table to `+0x30` (the animation table field), so the exporter
  reads the state table as animations and skips type 32 ("animation 0 uses sprite 164").
- **Show and hide** (update `0x800F3CA0`). The timer is `+0x7C`; `+0x7E` = 1 means "shown by a
  touch":
  1. Timer 0: when a hit by weapon id 44 comes (`ContactWeapon`, damage table
     `0x80074A1C`, where only id 44 has an effect), or when the object's terrain box overlaps the
     player's terrain box, then `display` = 1 and the timer = 150 (and `+0x7E` = 1 for the touch).
  2. Each update the timer goes down by 1.
  3. Timer ≥ 31, `+0x7E` = 1, and the player no longer touches: timer = 60, `+0x7E` = 0. So the
     floor stays 60 updates (1 s) after the player steps off.
  4. Timer < 31: the floor blinks (`display` = timer & 1).
  5. Timer 0: hidden. When the player still stands on it, the next update starts again at 150.

  So the floor shows for 150 updates (2.5 s) while the player stands on it, blinks for the last
  30, and shows for 60 more updates after the player steps off (if more than 30 are left).
  In `sheldon-6_tile-shown` the floor at x 1648 has timer 130 and `+0x7E` = 1: X touched it 20
  updates before the save.
- When `mega+0xD5` ≠ 0, the touch test uses `actor2` (`0x800CD340`), or `actor2 + 0xB0` when
  `actor2+0x05` is 0, instead of the player [certain for the code; what `mega+0xD5` means is not
  known].
- Off screen by more than 80 × 160 px: the object deletes itself, and the record spawns it again.
- **Weapon id 44 is a Zero attack, not an X weapon** [certain for the code]. `weapon_types[44]`
  (`0x80071220` + 4 × 44) is `0x801EDFC8`. In X's overlay 0 that address is in the middle of a
  function; in Zero's overlay 1 (also at `0x801EA000`) it is a function start. The only code that
  makes weapon objects of type 44 is `0x801EAA24` in overlay 1: it makes one id-44 weapon object at
  Zero's x, or two at x ± 32 × n, and an effect object type 26 with each. Sub-step 4 of Zero's step
  59 (`0x801ED198`; step 59 at `0x801EC614`, sub-step table `0x801EE39C`, read in
  `turtloid-6_zero-player`) counts `mega+0xE1` up by 1 each update and calls it when the count
  mod 10 is 1, with n = count / 10; so a new pair comes every 10 updates, 32 px further out each
  time. The sub-step ends when `mega+0x07` is 11 or more (its writer was not read). Step 59 is Zero's special attack (skill bit `0x80`, button
  `0x40`, `docs/players/steps-extra.md`). This agrees with the stun ids of `docs/engine.md`: slot 8
  has ids 8, 17 and 44, and Zero's skill bit `0x80` is the technique of the stage 8 Maverick
  [likely, from bits `0x01` = stage 1 and `0x20` = stage 6]. So only Zero can show a hidden floor
  with a weapon; X shows it only by a touch. The game name of the technique is not in the code
  [open]. No other code writes type 44 into a weapon object [likely: a scan of the main program and
  overlays 0, 1 and 10 for the constant 44].
- **The floors are slippery**: see "The glass slide (stage logic 7)" below.
- The user says that the floors are invisible when the Nightmare Effect is on. In all seven states
  `nightmare_effect` (`0x800CD30A`) is 1, and no code of type 32 reads it. What the floors look like
  with the effect off is not known.

## The glass slide (stage logic 7)

Stage logic 7 (`0x800F0BB0`, overlay 10; one record in the second list) makes the hidden glass
floors slippery: when X stops, lands or dashes on collision 60, X slides on [certain for the code].
Collision 60 is only on the six glass floors (see "The map"), so this is the slide of the glass
floors. The object is never deleted.

The same code is stage logic 7 of overlay 04 (Blizzard Wolfang, ice) and 07 (stage 4 area 1)
[certain: the words differ only in address parts].

Each update (only while the player state `mega+0x04` is 1):

1. It reads the collision value under the middle of the player's feet (`TileCollisionAt` at x,
   y + terrain box bottom) into `+0x20` (the value of the update before goes to `+0x21`).
2. By that value (jump table `0x800E9C34`): 60 runs the state function; 0 runs it only when the
   player stands on an object (`mega+0x89` bit `0x08`), and always in state 2; 1–15 and 29–59 set
   state 1 (no slide); 16 reads the cell 16 px higher; 17–28 (slopes) push the player down the
   slope at 3.5 px/update (not used in this stage: the map has no slope values; see
   `docs/engine.md`, "Slippery floors and slopes (stage logic 7)", and Blizzard Wolfang's stage,
   which uses them).
3. `+0x1C` = the player step, `+0x1E` = `0x40` when the player moved right since the update before,
   0 when it moved left (`mega+0x08` against `mega+0x13C`).

States (table `0x800FC4E8`): 0 set-up `0x800F1728` (clears the fields), 1 arm `0x800F1014`, 2 slide
`0x800F1374`.

**State 1 (arm).** When the player is on the floor (`mega+0x70` bit `0x08`), `mega+0xE0` is not 2,
and none of the three cells under the feet (middle, left and right edge of the terrain box) is 56,
the player step (table `0x800E9D2C`) picks the start of the slide. `v` = slide speed (`+0x14`), `a`
= its change per update (`+0x18`); 16.16 px:

| player step now | `v` | `a` | mode `+0x1D` |
|---|---|---|---|
| 2 stand, 26 crouch | the last x move; ± 0.5 when the step before was 3 (walk start); ± 2.0 after step 4 (walk) or 74 (Mach Dash) | ∓ 0.09375 (`0x1800`), against `v` | 1 right, 0 left |
| 5 (Zero's stop pose), 8 land | `mega.speed_x`; ± 2.0 after step 35 (Falcon dash) | ∓ 0.09375 | 1 right, 0 left |
| 12 dash, sub-step 2 | 0; timer `+0x1F` = abs(`mega.speed_x`) / `0x6000` (17 for the 6.5 px dash) | ± 0.15625 (`0x2800`), to the side X faces | 3 right, 2 left |

Then `v` = `v` × 4/3, and state 2. (The sign of `v` follows `+0x1E`: right is +.)

**State 2 (slide).** Each update: x = player x + `v`, `v` += `a`; then the player x = x. So the
slide adds `v` to the player's own move. It ends (state 1) when:

- the player step is 6 (jump) or 9 (wall grab), `mega+0x89` bit 0 or 1 is set (a wall), or
  `mega+0xE0` is 2;
- the cell under the new x is not 60, 20, 24, 26 or 28 (or 0 while on an object);
- modes 0 and 1: `v` changes sign (the slide stopped);
- the player moves back against the slide.

In modes 2 and 3 (the dash) the timer counts down; when it passes 0, `a` becomes ∓ 0.0234 (`0x600`,
against the slide) and the mode goes to 1 or 0. So a dash on glass gains about 17 × 0.156 = 2.7 px
per update and then loses it slowly [likely for the numbers in play].

Examples [likely, from the numbers]: a walk at 2.0 px per update that stops on glass gives
`v` = 2.67, which falls to 0 in 28 updates: about 38 px of slide. In `sheldon-6_tile-shown` the
object is in state 2, mode 1: X stands (step 2) at x 1661 on the glass floor and slides right with
`v` = 0.604, `a` = −0.09375 [certain for the bytes]. The object x is the player x (1661) during a
slide; in the other states it keeps the x of the last slide (for example 2926 in `sheldon-7`).

For the conversion: make collision 60 an ice floor. When X stops, lands or dashes on it, keep a
sliding x speed with the values above.

## Camera

The camera of layer 0 starts with the limits x 0–2832, y 256–256 (states 1–6). So the first part
is one screen row high: the view is y 256–495. In `sheldon-7_lasers` the limits are x 0–3488.

**Camera zones (stage logic 0, code `0x80052258` in the main program)** [certain for the code].
The set-up takes a 16-byte entry from a table per stage and area (`0x80079D70`, word index
stage × 2 + area; stage 7 area 0: `0x800798C0`) at `(id & 0x7F) × 16`: the trigger rectangle
x0, x1, y0, y1 (the player's position), then the target limits min x, max x, min y, max y
(`0xFFFF` = keep). While the player is in the rectangle, the zone writes the target limits
(`cam+0x26`, `+0x24`, `+0x2A`, `+0x28`) and sets the ease speed `cam+0x47` = 2. With `id` bit 7 it
also sets `0x8008EAB0` = 1 and deletes itself without a respawn (one use).

| record | `id` | at (x, y) | trigger x | trigger y | sets |
|---|---|---|---|---|---|
| 3 | 128 | 4360, 296 | 4352–4383 | 256–335 | no limit; `0x8008EAB0` = 1 (the texture swap, below), one use |
| 4 | 1 | 4792, 368 | 4784–4799 | 336–415 | max x 4832 |
| 5 | 2 | 4808, 368 | 4800–4815 | 336–415 | max y 256 |
| 6 | 3 | 4728, 456 | 4688–4767 | 448–463 | max y 512 |
| 7 | 4 | 4728, 472 | 4688–4767 | 464–479 | max x 4480 |
| 8 | 5 | 3848, 928 | 3840–3855 | 896–959 | min x 3408 |
| 9 | 6 | 4008, 864 | 4000–4015 | 832–895 | min x 3840 |

**Laser gates (main 13, `id` `0x50`–`0x55`)** [certain for the code and the tiles]. When a gate
opens (state 3, `0x800EA650`), `id & 0x0F` picks one case. It sets one target limit and copies
empty cells (collision 0) over the gate cells (collision 59) with `CopyTiles` on layer 0. The
puzzle itself is in `docs/enemies/`.

| record | `id` | at (x, y) | gate cells (x, y, cells) | sets | copy source |
|---|---|---|---|---|---|
| 82 | `0x50` | 3152, 360 | 3136, 336, 2 × 4 (wall) | max x 3488 | 3136, 144 |
| 83 | `0x51` | 3808, 408 | 3792, 384, 2 × 4 (wall) | max x 3824 | 3792, 144 |
| 84 | `0x52` | 4144, 360 | 4128, 336, 2 × 4 (wall) | max x 4480 | 4128, 144 |
| 85 | `0x53` | 4800, 376 | 4784, 352, 2 × 4 (wall) | max x 4832 | 4784, 144 |
| 86 | `0x54` | 4728, 464 | 4688, 448, 4 × 2 (floor; the gate is 5 × 2) | max y 512 | 4688, 176 |
| 87 | `0x55` | 3928, 720 | 3888, 704, 4 × 2 (floor) | max y 768 | 3584, 704 |

Each wall gate opens the view to the next one: the view is 320 px wide, and 2832 + 320 = 3152 (gate
`0x50`), 3488 + 320 = 3808 (`0x51`), 3824 + 320 = 4144 (`0x52`), 4480 + 320 = 4800 (`0x53`),
4832 + 320 = 5152 (boss gate `id` 36). In `sheldon-7_lasers` gate `0x50` is live in state 3 and
max x is 3488, so it is open.

**The door opening in the live state** (`sheldon-13_door-open`) [certain for the state]:

- Door `0x50` (main slot 3) is in state 3, step 4, sub 0: the tiles are copied, the limit is raised,
  and the door waits until the camera x is 3168 (door x + 16) or more. The camera is at x 2961, so
  the room behind is not deleted yet and the camera x minimum is still 0.
- Current and target max x are both 3488 (ease speed 4). Min x 0, y 256–256.
- The laser that opened the door is still live: a laser visual and a probe (weapon 79) at
  (3144, 330), which is door + (−8, −30), the sensor centre. The two aiming beamers of room 1
  (x 3360 and 3600) are live, because the door spawned them.
- The live screen data differs from the disc in 8 cells only: screen 28, cells (4–5, 5–8), which
  are x 3136–3167, y 336–399. Each changed from collision 59 to 0 [certain]. `CopyCell`
  (`0x800262EC`) writes into the screen data (scratchpad `+0x08`, `0x200` bytes per screen), not
  into the layout. So every place of the layout that uses the same screen changes too. Screen 28
  is used only once in this layout.
- `0x8008EAB0` is 0, and VRAM holds texture entry `0x10000`.
- Timing: the state does not give the number of updates since the hit. The order is: hit, 19
  updates of animation 33, then the copy and the limit (`docs/enemies/laser-reflector-puzzle.md`).

**Doors 1–4 in `sheldon-14`**: all are open; each gate area is collision 0 (8 cells each for
`0x51`–`0x53`), and the door `0x54` gap is open in 4 × 2 cells (below). Door `0x54` is in state 3,
step 4, waiting for the camera.

**Door `0x54` is 5 cells wide on the disc, but opens only 4** [certain for the cells]. The disc has
collision 59 at x 4688–4767, y 448–479 (5 × 2 cells), with 57 at x 4672 and x 4768. The opening
copy (4 × 2) makes x 4688–4751 empty; x 4752–4767 stays 59 (solid; `TileAt` reads 59 as 56). So
the hole is 64 px wide. After the camera passes (y ≥ 480), the door copies 5 × 2 cells from
(4592, 448) to (4688, 448): in `sheldon-15` all 10 cells are 56. The hole is closed again (see
"The lower part"). Door `0x55` has the same shape on the disc (59 at x 3888–3967, y 704–735; 57 at
x 3872 and 3968).

**Rooms for the conversion** [certain for the numbers; the order of play is likely]:

| room | camera x (left edge) | camera y (top edge) | opened by |
|---|---|---|---|
| start to the first gate | 0–2832 | 256 | stage start |
| laser room 1 | up to 3488 | 256 | gate `0x50` |
| laser room 2 | up to 3824 | 256 | gate `0x51` |
| laser room 3 | up to 4480 | 256 | gate `0x52` |
| lower room 1 | 3840–4480 | 256–512, then 496 | floor gate `0x54`; zones 3 and 4 in the hole; the door lock (y 496–496, min x 3840) after the pass (`sheldon-15`) |
| lower room 2 | 3840 and up | up to 768 | floor gate `0x55` |
| lower-left room (Light capsule) | 3408 and up | 752 (`sheldon-9`) | zone 5; zone 6 closes it again |
| before the boss gate | up to 4832 | 256 | gate `0x53`; zones 1 and 2 |
| boss room | 5152 (min = max) | 256 | boss gate `id` 36 (`docs/enemies/boss-gate.md`); `sheldon-17`, checkpoint 3 |

The map in `stages` is drawn from the disc, so all six gates are closed in the drawing.

## The lower part (states 8, 9, 14–16)

**The layout is the same as in the first rip** [certain]. For both states, `learn` gives member
102, the same layout offset (`0x11440`), 0 differing cells and the same list of 114 records
(`0x11680`). No new map was made; there is only a second drawing with the other texture block.

### Two texture blocks

Member 102 has two texture blocks of 256 KB each [certain for the bytes]:

| entry type | storage | in VRAM (x 320–831, y 256–511) |
|---|---|---|
| `0x10000` | page by page: 8 pages of 64 × 256, one after the other | states 1–7 and 13 (all 8 pages match) |
| `0x16` | row by row: 256 rows of 1024 bytes, the 8 pages side by side | states 8, 9 and 14–17 (all 8 pages match) |

- In states 8 and 9, entry `0x10000` differs in 48 rows of page 1, in all rows of pages 2–6 and in
  191 rows of page 7. So the change is a full copy of entry `0x16` over the stage texture.
- The whole entry `0x16` is in RAM during the stage (at `0x8015F378` in `sheldon-1` and
  `sheldon-13` to `sheldon-17`, at `0x8015A548` in `sheldon-7` to `sheldon-9`). The stage load
  packs the kept entries one after the other (`0xC`, `0xA`, `0x16`, `0x7`, …); entry `0x10000` is
  not kept in RAM. The only pointer to entry `0x16` is in the scratchpad: word `+0x44` (in all
  states that were checked) [certain].

### The texture swap (entry `0x16` to VRAM) [certain for the code and the states]

`StageTextureSwap` (`0x80027A50`, main program; proposed name), called once per frame from the
main loop (`0x8001226C`):

```
if (stage_texture_swap (0x8008EAB0) == 1) {
    LoadImage(&rect_tbl[0x80090D6A], scratchpad+0x44);   /* 0x800663FC */
    stage_texture_swap = 2;
}
```

- The rectangle table at `0x80071E70` has (320, 256, 512, 256) at index 0 and (512, 0, 512, 256)
  at index 1. Halfword `0x80090D6A` is 0 in all Sheldon states, so the upload is x 320–831,
  y 256–511: the stage texture area. Entry `0x16` is stored row by row because one `LoadImage` of a
  512 × 256 rectangle needs that order. (Entry `0x10000` is stored in 64-wide pages, like the upload
  queue at `*0x80097790`, whose items are 64 × 16 halfwords.)
- `0x800663FC` is called from 14 places; its arguments `(RECT *, data)` agree with the Psy-Q
  `LoadImage` [likely for the name].
- **The trigger is camera zone record 3** (stage logic 0, `id` 128, at (4360, 296); trigger x
  4352–4383, y 256–335, in laser room 3 after door `0x52`). `CameraZone` (`0x8005222C`) writes
  `0x8008EAB0` = 1 for a zone with `id` bit 7 and deletes the zone. The next frame uploads the
  texture. So the swap happens when X walks past x 4352 in laser room 3. Checkpoint 1 (x ≥ 4176)
  and with it the background switch (below) come first, 176 px before. In laser room 3 the layer-0
  art covers the whole view, so the player does not see either change there [likely; seen in
  the drawing at the `sheldon-14` camera].
- The states agree: `0x8008EAB0` is 0 in states 1, 7, 13 (texture `0x10000`) and 2 in states 8, 9,
  14–17 (texture `0x16`). Zone record 3 is not live in state 14; zones 1, 2, 3 and 4 (records 4–7)
  are.
- Other writers of `0x8008EAB0`: `0x8002649C` (the layer set-up of the stage start, writes 0); the
  teleporter crystal `id` 2 (B2, writes 1; in overlay 10 at `0x800EF5F4`; stage 7 has no `id` 2
  crystal); the boss gate (stage object 8) `id` 18 (`0x80050F28`, writes 1 and sets the camera
  target min x 3440; not in stage 7).
- **It does not change back.** No code uploads entry `0x10000` during play; that entry is not in
  RAM. Texture `0x16` is in VRAM in the boss room (`sheldon-17`). A restart after a death runs the
  layer set-up, which writes 0 to the flag; whether a restart reloads entry `0x10000` from the
  disc was not read [open; a save state after a death at checkpoint 1, 2 or 3 is needed].
- The route through door 3 goes through laser room 3, so it also passes the zone: the texture is
  swapped on both routes [certain for the zone position].

### The background change (`sheldon-14` to `sheldon-15`)

The user said that the background changed in `sheldon-15`. VRAM is almost the same in states 14
and 15: only the player frame area (x 320–383, y 0–31) and the last 1–5 colours of 18 palettes in
rows 486–487 differ. Other pairs of states differ in the same way (16 to 28 palettes), so that is
a colour cycle, not a change of art [certain]. The change that the user sees has two parts:

1. **The BG layers move to a second setting at checkpoint 1** [certain]. A small object pool at
   `0x800C4778` (4 slots of `0x30`; type at `+0x01`; type table `0x8007136C`; update loop
   `0x800208F0`) holds one object of type 1 in stage 7: `SheldonBgSwitch` (`0x800E9E4C`, overlay
   10; proposed name). Its state 0 (`0x800E9E88`) waits until `game+0x1D` (the checkpoint) is not
   0. Then it sets:
   - layer 1 `+0x42` = 320 (a y offset), and the layer 1 redraw request;
   - layer 2 mode (`+0x04`) = 5 (the empty mode `0x800299C4`), layer 2 y = 448, layer 2 x = layer
     1 x / 4, and the layer 2 redraw request.

   Its state 1 (`0x800E9EF0`) then sets layer 2 x = layer 1 x / 4 each frame. The object is in
   state 0 in states 1, 7 and 13, and in state 1 in states 8 and 14–17. Stage 22 has no object in
   the pool.

   | setting | layer 1 (x, y) | layer 2 (x, y) | states |
   |---|---|---|---|
   | first (checkpoint 0) | camera / 2 | camera / 4 (mode 3, `0x80028900`) | 1, 7, 13 |
   | second (checkpoint ≥ 1) | camera x / 2, camera y / 2 + 320 | camera x / 8, 448 | 8, 9, 14–17 |

   Example: state 13 has layer 1 at (1480, 128) and layer 2 at (740, 64); state 14 has layer 1 at
   (2247, 448) and layer 2 at (561, 448).
2. **The texture swap changes the art of the BG layers too.** In the first setting, with texture
   `0x10000`, layer 1 shows a purple machine and layer 2 a sky with clouds. In the second setting,
   with texture `0x16`, layer 1 shows a dark sphere and dark blocks, and layer 2 a blue hall with a
   central machine [observed in the drawings; the layer 1 and 2 screens at the second-setting
   position are garbled with texture `0x10000`].

In `sheldon-14` the layer-0 wall art covers the whole view, so the new background is not seen.
In `sheldon-15` layer 0 is open (the room interior has no layer-0 art), so layers 1 and 2 show.
That is the first place where the player sees the new background.

For the conversion: use the first setting and the first drawing up to checkpoint 1, and the second
setting with the second drawing (catalog entry "lower rooms") after it.
- The two drawings [observed]: entry `0x10000` gives correct art from the start to laser room 3
  (x 0–4800, y 256–511). Entry `0x16` gives correct art in the lower rooms (y 512–1023), in the
  corridor to the boss gate and in the boss room (x 4800 and up). The laser rooms look the same in
  both. The other parts are garbled in each drawing.
- `learn` now also tries entry `0x16` (read row by row) when entry `0x10000` is not in VRAM. It then
  writes `"texture": 22` in the catalog entry, and `stages` draws with that block
  (`mmxdemake/learn.py`, `stages.py`, `vram.pages_from_rows`).

### Camera and gates

| state | camera (x, y) | min x | max x | min y | max y (target) |
|---|---|---|---|---|---|
| `sheldon-8_lower-1` | 3840, 711 | 3840 | 4480 | 496 | 713 (768), easing at 2 px/frame |
| `sheldon-9_lower-2` | 3408, 752 | 3408 | 4480 | 752 | 752 (752) |

- `sheldon-8`: min x 3840 is zone 6 (record 9, live). Max x 4480 is zone 4. The target max y 768
  is floor gate `0x55` (record 87), which is live in state 3 (open) [certain]. Min y 496 is the
  lock of door `0x54` after the camera passed it: y min = y max = door y + 32 = 464 + 32
  (`docs/enemies/laser-reflector-puzzle.md`, "After the camera passes"); `sheldon-15` and
  `sheldon-16` have y 496–496 [certain for the numbers].
- `sheldon-9`: min x 3408 is zone 5 (record 8). Min y = max y = 752, so the view of the
  lower-left room is one screen high (y 752–991). 752 is the lock of door `0x55`: 720 + 32
  [certain for the numbers].
- Door 3 (`0x53`, record 85) is live in state 3 (open) in both states. Floor gate `0x54` (record 86)
  is not live. The states do not show when door 3 opened.
- In `sheldon-8` the lower laser puzzle runs: 20 laser-puzzle objects (main 13) are live, with 11
  laser probes (weapon 79), 11 laser visuals (visual 11) and 34 misc type 7 objects.
- The zone table has an entry 7 (trigger x 5024–5055, y 800–1039; min x 4704) that no record uses
  [certain for the table].
- Byte `0x8008EAB0` is 2 in states 8 and 9: the texture swap is done (see "The texture swap").

### Why X cannot climb back up (`sheldon-15`)

The user asked why X cannot climb the wall back up into the upper section in `sheldon-15`. The
live collision of the state gives the answer [certain for the cells and the code]:

1. **The hole is closed.** In `sheldon-15` the door `0x54` gap (x 4688–4767, y 448–479) is
   collision 56 in all 10 cells, with 57 at x 4672 and x 4768. Door `0x54` copied these cells from
   (4592, 448) when the camera passed y 480, and locked the camera at y 496–496. In `sheldon-14`
   the gap was open at x 4688–4751. So there is no opening above X; the ceiling over the recess
   (x 4672–4783, y 480–511) is solid.
2. **No wall to climb near the hole.** The room's right wall (x 4784–4799, y 480–703), which is
   also the right side of the recess, and the left side of the recess (x 4656–4671, y 480–511)
   are collision 57. The side-wall
   test `0x80030C58` (jump table `0x80010F18`, values 56–63) sets `obj+0x4A` = 1 only for 56 and 58
   (and for 62 and 63 when byte `0x80097187` is 2); 57 and 60 are solid with `+0x4A` = 0; 59 and 61
   return 0. With `+0x4A` = 0 there is no wall slide and no wall kick. The left wall (56, x
   4480–4543) is far from the hole, and the ceiling over it (y 480–511) is solid.
3. **Too high.** The floor is at y 704 and the top of the recess at y 480: 224 px. A block of 56
   (x 4608–4687, y 608–639) is in the room; its top is 128 px below the recess top. The highest
   jump is about 68 px. So even with the hole open, X cannot reach it from the floor or the block.
4. **Not the camera.** `CameraClamp` (`0x8002820C`) clamps the player only in x. In y it only sets
   the player y to 0 above the map top, and starts the fall death when the player is 8 px or more
   below camera max y + 256.

So the lower section is one way: after the camera passes door `0x54`, the route up is closed for
good (until a restart).

### Checkpoint 2

Both states have checkpoint 2: record 1 (`id` 130, the y test y ≥ 832) [certain: X is at y 843 in
`sheldon-8`].

### The Light capsule room (lower-left room)

- In `sheldon-9`, X (step 20) is at (3520, 955), next to the Light capsule (stage object 26, record
  42, `var` 1, at (3456, 964), state 1/4). A second stage object 26, `var` 16, at (3456, 886), has
  no record: the capsule made it [likely: the hologram of Dr. Light; the export shows it].
- `game+0x69` (armour part bits) is `0x08` in states 8 and 9 and `0x0C` in states 10–12. So this
  capsule gives bit `0x04` [certain for the bytes]. Bit `0x04` is the **body part of the Blade
  Armor** (capsule `id` 1; part picture 4, a chest with shoulder plates) [certain for the code
  path; the part name is from the picture]. The live part picture in `sheldon-9` is effect type 35,
  `var` 4, sprite 12 (animation 4). The full map is in `docs/enemies/light-capsule.md`. With the
  Yammark capsule (bit `0x08`, legs), X has 2 of the 4 Blade parts in states 10–17.
- Rescue Reploids 99 and 100 (records 13, 14) are live.
- Layer 0 has almost no art in this room (black in both drawings), but the collision is in the map
  [observed; why is not known].

### The hidden brick block (main 32, `id` 1)

In `sheldon-9`, record 79 (`id` 1, at (3672, 992)) is live: `display` 0, timer `+0x7C` = 24,
`+0x7E` = 0, sprite 1. The timer is below 31, so the block is in its blink phase after a touch or
a hit [certain for the bytes]. Its collision (value 56) is in the map all the time.

### The teleporter crystal

Record 110 (main 28, `id` 1, at (4744, 928)) is live in states 8 and 9. It takes X to stage 22,
area 0: the hidden area [certain: `sheldon-10_section-2` is the next state;
[`sheldon-alt.md`](sheldon-alt.md)].

## The second record list (`fixed_set_tbl`)

At `0x800FB278`, 26 records and an end record [certain for the records]:

| records | kind | type | `id` | x, y | object |
|---|---|---|---|---|---|
| 18 | 3 | 3 | 0–23 (not 3, 10, 11, 13–15) | 0, 0 | stage logic 3 (`0x80052898`, main program; not read) |
| 1 | 3 | 7 | 0 | 0, 0 | stage logic 7 (`0x800F0BB0`, overlay): the glass slide (see "The glass slide (stage logic 7)"); state 1 in `sheldon-5`, state 2 (X slides) in `sheldon-6` |
| 1 | 7 | 1 | 0 | 0, 0 | kind-7 object type 1: `kind7_types[1]` (`0x8007136C` + 4) = `0x800E9E4C`, `SheldonBgSwitch` (see "The background change"); live in kind-7 slot 0 in all states [certain] |
| 6 | 3 | 14 | 129–134 | (32, 256), (1504, 256), (1360, 256), (3968, 256), (3856, 896), (2544, 320) | stage logic 14 (`0x8005335C`): **navigator call points** (below), not a background setting |

Record positions as read by the stage tool. The rectangles that the logic-14 objects test come from
a table, not from the record positions (below).

### Navigator call points (stage logic 14) [certain for the code; the "navigator" use is likely]

Stage logic 14 (`0x8005335C`, main program, state table `0x8007ACD4`) shows a text message (the
navigator) when the player is in a rectangle. It is in many stages; the full rule is proposed for
`docs/engine.md`. In short:

- Rectangle and "seen" bits: k = `0x8007A124`[stage × 2 + area]; rectangle = `0x8007A154` + k × 128
  + (`id` & 0x0F) × 8 (x0, x1, y0, y1; the player position must be strictly inside); seen bits =
  halfword `game+0x9E` + 2 × k, bit (`id` & 0x0F). A message that was seen deletes its object
  (`DeleteObjectKeep`).
- `id` bit 7 clear (a **forced** message): `PlayerEvent(20)`, the world stops (`game+0x11`…`+0x19`
  = 1), `TextOpen(id & 0x0F)`; when the text closes (`0x8008EAFC` = 0), `PlayerEventEnd`, the world
  moves again.
- `id` bit 7 set (an **optional** message): while the player is inside, `mega+0x14E` = `id` & 0x0F
  (15 for 0) and sound 42 plays once on entry. The HUD code (`0x80024528`) draws an icon while
  `mega+0x14E` is not 0 [likely: the "call" icon]. When bit `0x100` of the pad halfword
  `0x800C4570` is pressed [guess: Select], `game+0x10` = 2 (the player stops) and the same text
  sequence runs. Leaving the rectangle clears `mega+0x14E`.

Stage 7 uses k = 7; all six records are optional (`id` 129–134):

| `id` | rectangle x | rectangle y | where |
|---|---|---|---|
| 129 | 32–192 | 256–448 | the start |
| 130 | 1504–1600 | 256–448 | before the hidden glass floors |
| 131 | 2880–2960 | 256–448 | laser room 0, before laser door `0x50` |
| 132 | 3968–4096 | 256–448 | laser room 2 |
| 133 | 3856–3952 | 896–944 | lower room 2 |
| 134 | 4912–5072 | 320–416 | before the boss gate |

In all Sheldon states the six objects are in state 0 and the stage-7 seen word (`game+0x9E` + 14) is
0: no optional message was called. The intro word (k 0) is 1 (forced message 0 of the intro) and the
Turtloid word (k 6) is 4 (forced message 2) [certain for the bytes].

### Stage logic 27: the stage start ("READY") [certain for the code; the name is likely]

Stage logic 27 (`0x800541D4`, main program, state table `0x8007AD28`) has no record. The stage start
code `0x8001E730` (called from `0x8001D59C` and `0x8001E2CC`) makes it while `game+0x1E` (stage
ready) is 0. It waits until `fade_dir` (`0x80097424`) is 0, then makes three waves of ray objects of
type 7 (`0x8004CF68`; screen-space quads): 1 with `var` 0, then 10 with `var` 1, then 8 with
`var` 2 (random `+0x07`); each wave waits until the ray objects of the wave before are gone
(`+0x14` counts them). Ray type 7 `var` 0 also makes two misc objects of type 18 (`0x800481D0`);
they move to the screen centre (160, 120) and write `game+0x1E` = 1, which lets the player set-up
go on. After the last wave the object stays live with `+0x16` = 1 (state 1, step 3 in all states).
So it is the start-of-stage banner [likely: the "READY" display], in every stage.

## Sprite exports

`spriteexport` on all seven states: no clean export from a live enemy in states 1–4 (no main enemy
is live). Exports from a code search, in every state: `hover-gunner` (type 5, no record in this
stage), `teleport-crystal` (28), `nightmare-virus` and `nightmare-virus-orb` (15),
`guardian` (30), `mettaur-d2` (27), `nightmare-bug` (22) and `laser-reflector-puzzle` (13,
live in `sheldon-7_lasers`). `rescue-reploid` (stage type 4) is live in states 5 and 6. Skipped:
type 32 (see above), type 0 and type 51 (no texture loaded: Shield Sheldon's texture comes with his
room).

States 8 and 9: the same set, with no new failure. Live exports: `laser-reflector-puzzle`,
`teleport-crystal`, `nightmare-bug` (both states), `guardian` (state 8), `rescue-reploid` (state 9)
and one new clean export in state 9: `enemy-4cfc820dcf`, stage type 26, 20 animations: the Light
capsule (closed, opening, and the hologram of Dr. Light). Its name is proposed, not set (the enemy
catalog has another owner).

## Save states that are still needed

1. A state after a death and restart at checkpoint 1, 2 or 3: to see whether the restart loads
   texture entry `0x10000` again, and which texture and BG setting the restart gives.
2. A state just after X walks past x 4352 (zone record 3) and one just before it, to see the swap
   frame by frame (optional; the code is known).
3. The laser puzzle just after gates `0x51`–`0x53` open (state 14 shows them all open), to check
   each limit in play.
4. The same hidden floors with the Nightmare Effect off, if the game allows it, to see whether they
   are then always shown.
5. A state with X on a hidden brick block (`id` 1) while it is shown, and one with Zero after
   the step 59 attack (weapon id 44) near a hidden floor.
6. A state with Nightmare Effect 5 in this stage (after a pick of stage 5, Ground Scaravich), for the
   crates.

## Open questions

1. Whether a restart after a death loads texture entry `0x10000` again (the layer set-up writes
   `0x8008EAB0` = 0; entry `0x10000` is not in RAM).
2. Stage logic 3 and the misc type 14 record.
3. Main types 22, 27 and 30: names and sheets (`docs/enemies/`).
4. Why `game+0x5E` is 1 in states 13–17, which the user called "no armour".
5. The other types of the BG pool at `0x800C4778` (type table `0x8007136C`) in other stages.
6. The game name of Zero's step 59 (weapon id 44) and of the stage 8 Maverick.
7. The glass slide in play (a save state while X slides after a dash), and the pad button of the
   optional navigator call (bit `0x100` of `0x800C4570`).
8. The byte `0x80097428`: when it is not 0, stage logic 14 deletes itself (no message).

Answered in this round: weapon id 44 (Zero's step 59), stage logic 7 (the glass slide), 14 (the
navigator call points) and 27 (the stage start banner), stage object 15 (the Nightmare crate, the
Nightmare Effect 5 object), the kind-7 record (the background switch) and bit `0x04` of
`game+0x69` (the Blade Armor body part). Before: what `0x8008EAB0` does and which code copies entry
`0x16` (the texture swap), and which code sets the camera y limits 496 and 752 (the locks of doors
`0x54` and `0x55`).
