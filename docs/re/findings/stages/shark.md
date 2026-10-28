# Stage: Metal Shark Player (stage 4, overlay 06)

Overlay `06` at RAM `0x800E9860` (area 0). Stage member 98, a layout of 22 × 8 screens. Section 2
(area 1, overlay 07, member 99) is in [`shark-2.md`](shark-2.md). Save state (in
`~/mmx-data/x6/dumps/`):

| state | player | X at (x, y) | camera (x, y) | what |
|---|---|---|---|---|
| `shark-1_ride` | X, form 1 (Falcon) | 2045, 384 | 1885, 224 | stage 4, area 0, checkpoint 0, difficulty 1; X rides the Ride Armor on the upper path |

In the state, X is in player step 18 and `mega+0xD5` = −1. `actor2` (`0x800CD340`) is live at X's
position: that is the Ride Armor (see "Objects"). The Ride Armor itself is in
[`docs/players/ride-armor.md`](../players/ride-armor.md) (another sheet).

The stage map is extracted with `learn` and `stages` (catalog entry "Metal Shark Player, section 1",
from `shark-1_ride`): `~/mmx-data/x6/stages/m098-metal-shark-player-section-1/` has `stage.png`,
`collision.png`, `objects.json`, `summary.txt` and `stage.ora`. The screens and the texture pages
are in `~/mmx-data/x6/tiles/m098/` and `~/mmx-data/x6/collision/m098/` (made again: the same bytes as
before).

`learn` passed at the first try: member 98, layout offset `0x900C` in overlay 6, 0 differing cells,
118 records at `0x921C`, palettes from 149 tiles (agree 1.0). Edge scores: horizontal 0.964 over 70
joins, vertical 1.0 over 13 joins. The vertical edge rule (`docs/process/process-log.md`, section 79)
did not change the result here.

**Two texture blocks.** Member 98 also has a texture entry `0x16` of 256 KB, as member 102
(`docs/stages/sheldon.md`, "Two texture blocks") [certain for the entry list]. In `shark-1_ride`,
entry `0x10000` is in VRAM. A second catalog entry, "Metal Shark Player, section 1, lower part"
(`"texture": 22`), draws the same layout with entry `0x16`:
`~/mmx-data/x6/stages/m098-metal-shark-player-section-1-lower-part/`. This entry was added by hand
from the same save state, because no state with entry `0x16` in VRAM exists yet. Its palettes are the
palettes of `shark-1_ride` [the drawing looks correct; not checked against VRAM].

| part of the map | entry `0x10000` (first entry) | entry `0x16` (second entry) |
|---|---|---|
| upper path (y 208–479) and the red backdrop at x 800–1599 | correct | garbled in places (backdrop, machines at x 2100–2900) |
| right shaft, lower path, left shaft, bottom room | garbled in places (y 720–760 of the lower path, the left shaft) | correct |

So the game probably copies entry `0x16` to VRAM when the player goes down the right shaft [guess: a
state on the lower path is needed]. Use each drawing for the part that it shows correctly.

The record format and the kinds are in `docs/objects-legend.md`; the shared rules in
`docs/engine.md`.

## The map

Layer 0 uses columns 1–21 and rows 0–6. The route, from the camera zones and the ladders [likely for
the order of play; certain for the places]:

| x (px) | y (px) | part |
|---|---|---|
| 800–1599 | 200–400 | red backdrop, no collision; left of the camera minimum x 1600 |
| 1600–5311 | 208–479 | the upper path: ceiling line (value 57) at y 208, floor y 320–479; 15 ceiling crushers |
| 5312–5359 | 416–479 | ladder down (values 33, 32, 34) at x 5328 |
| 5120–5439 | 480–895 | the right shaft (walls of value 57) |
| 464–5439 | 704–959 | the lower path, from right to left; 21 ceiling crushers and the value-60 blocks |
| 960–1279 | 960–1199 | a small room under the ladder at x 1056; teleporter crystal `id` 0 |
| 336–607 | 960–1439 | the left shaft, under the ladder at x 432 |
| 320–1599 | 1408–1679 | the bottom room: a pit at x 1024–1343; Light capsule and teleporter crystal `id` 1 at the right end |

Collision values in layer 0 (cells): 0 (18 489), 56 (3 780), 57 (435), 60 (318), 32 (9), 34 (6),
33 (3).

- **Ladders.** Value 32 is the ladder, 33 the ladder top. Value 34 is on the left and on the right
  of each ladder top, at all three ladders (x 5328, 1056, 432) [certain for the places; what 34 does
  is not read].
- **Value 60** is in 10 blocks on the lower path, at y 624–719 (3 or 6 cells high), under the ceiling:
  x 1088–1343, 1440–1503, 1600–1727, 1856–1951, 2048–2143, 2432–2527, 2672–2767, 2912–3007,
  3712–3807 and 4512–4607. Each block has stage objects of type 9 with `var` ≥ 64 at its x (second
  list, below) [certain for the places]. In Shield Sheldon's stage value 60 is the floor under the
  hidden glass floors.

## Objects

118 records (list entry 4, area 0, `0x800F2A7C`), by class and type [certain for the counts]:

| class, type | records (kind 0 / 1 / 2) | object |
|---|---|---|
| main 1 | 10 | Junkroid (`docs/enemies/junkroid.md`), all `id` 1; records 88–97 |
| main 15 | 8 / 7 / 6 | Nightmare Virus (`id` 0 ×4, 1 ×9, 33 ×6, 34 ×2); records 33–53 |
| main 27 | 14 / – / 5 | Mettaur D2 (`docs/enemies/mettaur-d2.md`), all `id` 0; records 98–116 |
| main 28 | 2 | teleporter crystal (`docs/enemies/teleport-crystal.md`): `id` 0 at (1216, 1104) goes to area 1, `id` 1 at (1552, 1584) goes to the hidden area (see "The teleporter crystals") |
| stage object 4 | 12 | Rescue Reploids, `id` 48–59; records 21–32 |
| stage object 8 | 1 | boss gate `id` 35 at (1872, 1584) |
| stage object 11 | 11 | junk block (below; [`docs/enemies/junk-block.md`](../enemies/junk-block.md)), `id` 0 ×6, 1 ×1, 128 ×4; records 77–87 |
| stage object 15 | 23 | Nightmare Cube, the Nightmare Effect 5 object (below; [`docs/enemies/nightmare-cube.md`](../enemies/nightmare-cube.md)), `id` 0 ×4, 1 ×2, 2 ×17; records 54–76 |
| stage object 26 | 1 | Light capsule `id` 4 at (1424, 1592): a Shadow Armor part, bit `0x10` (`docs/enemies/light-capsule.md`) |
| stage logic 0 | 11 | camera zones (below) |
| stage logic 6 | 3 | checkpoint markers (below) |
| stage logic 14 | 3 | background setting, `id` 129–131, at x 1632, 1824, 2624, y 256 |
| stage logic 30 | 1 | the Ride Armor place (below), `id` 0 at (1872, 395) |

Live in `shark-1_ride`: Junkroid record 88 (main slot 0, `var` 1, at 2144, 360), Rescue Reploid `id`
48 (record 21), the 50 stage objects of type 9 (second list), stage logic 14 (records 14, 15), 9
stage logic 3 objects (second list) and stage logic 27 (no record, as in Shield Sheldon's stage).

Where the code is (entries from the type tables in this state):

| class, type | entry | in |
|---|---|---|
| main 1, 27, 28 | `0x800EB8E0`, `0x800EAF94`, `0x800EC678` | overlay 06 |
| main 15 | `0x80041AB8` | main program |
| stage 4, 8, 26 | `0x8004F834`, `0x80050DFC`, `0x800515B4` | main program |
| stage 9, 11, 15 | `0x800E9E98`, `0x800EA530`, `0x800EE050` | overlay 06 |
| logic 0, 3, 6, 14, 27 | `0x80052258`, `0x80052898`, `0x80052D30`, `0x8005335C`, `0x800541D4` | main program |
| logic 30 | `0x800EE158` | overlay 06 |

### The Ride Armor place (stage logic 30)

`0x800EE158` [certain for the code]: it writes the object `actor2` (`0x800CD340`): flags `0x41`,
type = the record `id` (0), `var`, the record pointer and the position of the record. Then it calls
`DeleteObjectKeep`, so the record never spawns again while the area is loaded. Record 20 puts the
Ride Armor at (1872, 395), on the upper path. In `shark-1_ride`, `actor2` holds `41 00 00 01`: type
0, state 1. The mech is in [`docs/players/ride-armor.md`](../players/ride-armor.md).

### Junk block (stage object 11)

Entry `0x800EA530`, state table `0x800F3664`, set-up `0x800E9F04` [certain for the code; the name is
descriptive: no official name found]. The behaviour is in
[`docs/enemies/junk-block.md`](../enemies/junk-block.md): solid, 16 HP, junk, explosions and an item
drop on a kill. The records [certain]:

| `id` | x, y | where |
|---|---|---|
| 0 | (3736, 392), (3864, 344) | upper path; deleted at checkpoint 1 |
| 1 | (4368, 336) | upper path (the small block, 32 × 40 px) |
| 0 / 128 | x 3896, 4088, 4136, 4824: `id` 0 at y 872, `id` 128 at y 824 | lower path: pairs, one on the other. The upper block (y 824) breaks when nothing is under it |

Not live in `shark-1_ride`.

### Nightmare Cube (stage object 15): the Nightmare Effect 5 object

Entry `0x800EE050`, set-up `0x800ECBEC` [certain for the code]. Earlier versions of this page called
it "falling crate". The official name is Nightmare Cube [Fandom, "Nightmare" page]; the behaviour is
in [`docs/enemies/nightmare-cube.md`](../enemies/nightmare-cube.md). The set-up deletes the object
unless `nightmare_effect` (`0x800CD30A`) is 5, so these 23 objects come only with Nightmare Effect 5.
Sprite type 147 (bank 8: red, brown, purple and grey boxes). The records, 48 px apart [certain]:

| `id` (kind) | places (x, y) |
|---|---|
| 2 (brown, breaks) | (4712, 328); (4920, 872); (3992, 824); (2600, 824), (2600, 872); (2264, 824); (888, 824), (936, 824), (984, 824), (984, 872); (664, 1544), (664, 1592), (808, 1544), (808, 1592), (856, 1496), (856, 1544), (856, 1592) |
| 1 (purple, slides until it hits something) | (3848, 872); (808, 1496) |
| 0 (grey, slides one space) | (2264, 872); (712, 1496), (712, 1544), (712, 1592) |

Not live in the state.

### Ceiling crushers and the other type 9 objects (second list)

Stage object 9, entry `0x800E9E98` (state table `0x800F35FC`, animation table `0x800F35A0`), all from
the second list. The crusher and the crush death are in the crusher sheet in `docs/enemies/` (another
sheet). The records [certain]:

| records (second list) | `var` | x | y (record) | y (live) |
|---|---|---|---|---|
| 9–23 | 0 | 1576–4936, every 240 px | 192 | 256 |
| 24–44 | 1 | 200–5000, every 240 px | 672 | 672 |
| 45, 46 | 101 | 4560, 3760 | 776 | 776 |
| 47 | 98 | 2960 | 752 | 752 |
| 48, 49 | 102 | 2720, 2480 | 784 | 784 |
| 50, 51 | 99 | 2096, 1904 | 760 | 760 |
| 52–54 | 69 | 1696, 1632, 1472 | 776 | 776 |
| 55–58 | 67 | 1312, 1248, 1184, 1120 | 760 | 760 |

`var` 0 is the upper path, `var` 1 the lower path. The objects with `var` ≥ 64 are at the x of the
value-60 blocks of the lower path. All 50 are live in `shark-1_ride` (`display` 0, state 1/2).

### Checkpoints (stage logic 6)

| record | `id` | test | checkpoint after |
|---|---|---|---|
| 0 | 1 | x ≥ 5328 (at y 352) | 1: the end of the upper path, at the ladder down |
| 1 | 130 | y ≥ 1440 (bit 7: y test; at x 480) | 2: the bottom of the left shaft |
| 2 | 3 | x ≥ 1600 (at y 1584) | 3 |

Record 2 is at x 1600, at the right wall of the bottom room. The boss gate `id` 35 (x 1872) is also
past that wall, and the layout has no screen there (row 6 ends at x 1791). So neither can be reached
from area 0 [likely; open: what they do].

## Camera

In `shark-1_ride`, the limits of layer 0 are x 1600–5120 and y 224–224 (current and target; ease 4).
So the upper path is one screen row high: the view is y 224–463.

**Camera zones (stage logic 0).** The table of stage 4 area 0 is at `0x800795E0` (`camera_zone_tbl`
word 8). The rules are in `docs/engine.md` ("Camera"). `0xFFFF` = keep:

| record | `id` | at (x, y) | trigger x | trigger y | sets |
|---|---|---|---|---|---|
| 3 | 128 | 5336, 392 | 5312–5359 | 224–415 | min x 5120, max x 5120, max y 704; `0x8008EAB0` = 1, one use |
| 4 | 1 | 5272, 728 | 5136–5423 | 720–735 | min y 704, max y 704 |
| 5 | 2 | 5120, 768 | 5120–5135 | 720–831 | min x 960 |
| 6 | 3 | 1064, 904 | 1040–1087 | 896–911 | max x 960, max y 944 |
| 7 | 4 | 1064, 888 | 1040–1087 | 848–895 | max x 5120, max y 704 |
| 8 | 5 | 968, 808 | 960–975 | 704–895 | min x 320 |
| 9 | 6 | 984, 808 | 976–991 | 704–895 | min x 960 |
| 10 | 7 | 440, 904 | 416–463 | 896–911 | max x 320, max y 1424 |
| 11 | 8 | 440, 888 | 416–463 | 848–895 | max x 5120, max y 704 |
| 12 | 9 | 608, 1520 | 592–607 | 1440–1615 | max x 1280, min y 1424 |
| 13 | 10 | 472, 1432 | 352–607 | 1424–1439 | min y 1424 |

**Rooms for the conversion** [certain for the numbers; the order of play is likely]:

| room | camera x (left edge) | camera y (top edge) | set by |
|---|---|---|---|
| upper path | 1600–5120 | 224 | the stage start (in the state) |
| right shaft | 5120 | 224 → 704 | zone `id` 128 (one use) at the ladder top |
| lower path, right part | 960–5120 | 704 | zones 1 and 2 |
| lower path, left part | 320–5120 | 704 | zone 5 (zone 6 sets min x 960 again) |
| small room under the ladder at x 1056 | up to 960 | 704–944 | zone 3; zone 4 restores |
| left shaft | 320 | 704–1424 | zone 7 at the ladder top; zone 8 restores |
| bottom room | 320–1280 | 1424 | zones 9 and 10 |

The camera starting limits for checkpoint 0 and the place where X starts are not in this state.

## Area 1 (overlay 07)

Section 2 is stage 4 area 1, overlay 07, member 99 [certain: six save states]. The full rip is in
[`shark-2.md`](shark-2.md): the map, the records, the two warps inside the area (to the Nightmare
Pressure room and to the boss part), the white screen, the camera and the checkpoints. The notes
below were made from `shark-1_ride` before those states existed:

- `camera_zone_tbl` word 9 (stage 4 area 1) is `0x80079690`: 4 zone entries, after the 11 of
  area 0. Word 10 (stage 5 area 0) is `0x800796D0`.
- `enemy_set_tbl` and `fixed_set_tbl` of stage 4 area 1 point to `0x800F7BD8` and `0x800F7E58`. These
  addresses are past the end of overlay 06 (`0x800F4020`). In overlay 07 (loaded at the same base),
  they hold valid lists: 79 records and 48 records. So overlay 07 is stage 4 area 1 [likely]. This also
  fits the overlay numbers of the next stages: stage 5 = 08, stage 6 = 09, stage 7 = 10.
- The first list of area 1 has main 35 (44 records), main 27 (10), main 15 (8), main 16 (1), main
  47 (1), stage objects 4 (3), 8 (2: `id` 21 at x 5776 and `id` 22 at x 6064, y 384), 11 (4) and 15
  (1), and stage logic 6 (1) and 14 (3). The last record is main 56 `id` 0 at (6231, 350): probably
  Metal Shark Player, behind the two boss gates [plausible].
- The second list has 40 stage objects of type 9 (more crushers), 3 of type 11, stage logic 7, 9 and
  10, and two kind-7 records.
- Overlay 07 also reads `nightmare_effect`: stage object 15 (set-up `0x800EEC50`, the Nightmare
  Cube) needs value 5, and main 35 (set-up `0x800F01BC`, Nightmare Freeze) needs value 2
  ([`shark-2.md`](shark-2.md), "The Nightmare Effect objects").

## The teleporter crystals (the way to area 1 and to the hidden area)

The two crystals (main 28, `docs/enemies/teleport-crystal.md`) use sequence A: they write the exit
request `game+0x0F` = `0x40` | `id`. The stage exit code (`0x8001E32C`, main program; another sheet
covers it in full) reads the request. For a Maverick stage (stage 1–8) [certain for the code]:

- request `0x41`: `0x80042F7C(stage, &stage, &area)`: stage = 19 + (stage − 1) / 2, area = (stage −
  1) & 1. Then mode 8 (load the new area).
- another request with bit `0x40`: when the byte `0x80071190`[stage] is 0 and the area is 0: area =
  area + 1, mode 8. The byte is 0 for stages 0 and 4 and 1 for stages 1–3 and 5–8 (read in
  `shark-1_ride`) [certain]; the other branch is not read here.

So in stage 4 [certain for the code; the save states after the crystals are still not made]:

| crystal | place | request | goes to | overlay |
|---|---|---|---|---|
| `id` 0 | (1216, 1104), the small room under the ladder at x 1056 | `0x40` | stage 4 area 1: section 2, the main route ([`shark-2.md`](shark-2.md)) | 07 |
| `id` 1 | (1552, 1584), the right end of the bottom room, beside the Light capsule | `0x41` | stage 20 area 1: the hidden area ("Another Route") of this stage ([`shark-alt.md`](shark-alt.md)) | 28 (25 + 2 × (20 − 19) + 1; confirmed by `shark-10_alt`) |

The Fandom page of the stage agrees: "descend the first ladder to enter a blue portal ... it is the
Main Route. This leads to the stage's second area"; the Shadow Armor capsule and the Another Route
portal are past the large gap [Fandom, "Recycle Lab", read through the MediaWiki API]. Overlay 28 has
the Nightmare Press, a belt, a forced scroll (kind-7 type 4), junk blocks, Nightmare Cubes and a Ride
Armor place: [`shark-alt.md`](shark-alt.md) (save state `shark-10_alt`). The teleporter
`id` 3 is the only one that reads `game+0x26`; stage 4 has none.

## The second record list (`fixed_set_tbl`)

At `0x800F2E34`, 60 records and an end record [certain for the records]:

| records | kind | type | `id` | x, y | object |
|---|---|---|---|---|---|
| 0–8 | 3 | 3 | 0–8 | 0, 0 | stage logic 3 (`0x80052898`, main program; not read); all 9 live |
| 9–58 | 4 | 9 | 0, 1, 67–102 | see above | ceiling crushers and the type 9 objects of the value-60 blocks |
| 59 | 7 | 2 | 32 | 0, 0 | kind-7 object type 2: the Nightmare Press controller (`docs/enemies/nightmare-press.md`) |

## Nightmare Effect

`nightmare_effect` is 0 in `shark-1_ride`, and `game.stage_effect` is `0 6 7 4 0 4 7 1 4`, so
`game.stage_effect[4]` is 0 [certain]. `game.effect_new` is `3 5 8`: the targets of the stage 4 pick.

The rule in `docs/engine.md` ("Nightmare Effect") agrees:

- The picks in this save were stage 1 (targets `3 7 7`), stage 6 (`1 5 5`), stage 7 (`2 6 6`) and
  stage 4 (`3 5 8`), in that order [likely: these four picks give exactly the 9 bytes]. None of them
  targets stage 4, so its byte stays 0.
- The stage 4 pick wrote 4 to stages 3, 5 and 8 (the bytes at index 3, 5 and 8 are 4).
- Only two picks target stage 4: stage 2 (`4 4 4`) writes 2, and stage 5 (`4 7 7`) writes 5. So
  stage 4 can have the effect values 2 or 5 only.
- Value 5: the Nightmare Cubes (stage object 15) of area 0 and of area 1
  ([`docs/enemies/nightmare-cube.md`](../enemies/nightmare-cube.md)). Value 2: Nightmare Freeze
  (main 35, area 1 only; [`docs/enemies/nightmare-freeze.md`](../enemies/nightmare-freeze.md)).
  Overlay 06 has no other read of `nightmare_effect` [certain for the reads with the address
  `0x800CD30A`].

Stage 7 also has a value-5 object (`docs/engine.md`): the stage 5 pick targets stages 4 and 7.

The official names [Fandom, "Nightmare" page, the Nightmare Phenomenon table]: the Recycle Lab can
have **Nightmare Cube** (from the Central Museum: stage 5, Ground Scaravich) and **Nightmare Freeze**
(from the North Pole Area: stage 2, Blizzard Wolfang), and it causes **Nightmare Iron** in the Magma
Area, the Central Museum and the Weapon Center (the stage 4 pick writes 4 to stages 3, 5 and 8). The
stage numbers and the code values agree with this table [certain for the values; the stage names
are from Fandom].

## Sprite exports

`spriteexport` on `shark-1_ride`: 9 banks drawn, 0 without texture, 6 clean exports, 0 skipped.

| export | ID | class, type | found |
|---|---|---|---|
| `junkroid` | `enemy-166c7c6f8f` | main 1 | live |
| `enemy-9782d63f15` | `enemy-9782d63f15` | stage 4 | live |
| `mettaur-d2` | `enemy-46d9e2249f` | main 27 | search |
| `nightmare-virus` | `enemy-363dce0a1a` | main 15 | search |
| `nightmare-virus-orb` | `enemy-a96f0b9d66` | main 15 | search |
| `teleport-crystal` | `enemy-efd9270cec` | main 28 | search |

- `enemy-9782d63f15` is a Rescue Reploid with other sprites: the same animation table
  (`0x80078BEC`) and the same 36 animations as `rescue-reploid` (`enemy-61922ae859`), but a blue
  armoured Reploid, and a cell of 47 × 98 (not 47 × 96). Its name is proposed, not set (the enemy
  catalog has another owner).
- Banks: 0 = sprite type 128 (Light capsule, 84 sprites), 1 = 15, 2 = 141 (the Rescue Reploid of this
  stage, 63), 3 = 225 (the Ride Armor, 77), 4 = 138 (junk block), 5 = 27, 6 = 1, 7 = 28, 8 = 147
  (Nightmare Cubes). Images: `~/mmx-data/x6/sprites/banks/stage-4/shark-1_ride/`.
- No export for the Ride Armor (`actor2` is not in an object class that the exporter reads), the
  junk block, the Nightmare Cubes or the crushers (not live, or no animation table found). The junk
  block has a clean export from area 1 (`enemy-4185141b5a`, [`shark-2.md`](shark-2.md)).

## Save states that are still needed

1. The stage start (checkpoint 0): the start position, the camera limits before the Ride Armor, and
   X without the Ride Armor.
2. The lower path (after the right shaft): to confirm that texture entry `0x16` is in VRAM there, and
   to `learn` the second catalog entry from a real state.
3. Just before and just after the texture change in the right shaft, to find the code that copies
   entry `0x16`.
4. The small room under the ladder at x 1056, and the state after teleporter crystal `id` 0 (the
   code says stage 4 area 1; a state would confirm it).
5. The bottom room. (done: `shark-10_alt`, the state after teleporter crystal `id` 1: stage 20 area
   1, overlay 28, [`shark-alt.md`](shark-alt.md).)
6. (done: `shark-2_section-2` to `shark-7_boss`, see [`shark-2.md`](shark-2.md)) Area 1 (overlay
   07): its start, and the boss room with Metal Shark Player (main 56) live.
7. A save with Nightmare Effect 5 in this stage (after a stage 5 pick), for the Nightmare Cubes, and
   one with value 2 (after a stage 2 pick) for Nightmare Freeze in area 1.
8. A state with a junk block (stage object 11) on screen, and one with a crusher in action (for the
   crusher sheet).

## Open questions

1. What collision value 34 does (beside each ladder top).
2. What the checkpoint marker at x 1600 and the boss gate `id` 35 at x 1872 do in area 0.
3. (answered, "The teleporter crystals") Crystal `id` 0 leads to area 1 (confirmed by
   `shark-2_section-2`); crystal `id` 1 leads to the hidden area, stage 20 area 1 (confirmed by
   `shark-10_alt`, [`shark-alt.md`](shark-alt.md)).
4. Which code copies texture entry `0x16` to VRAM.
5. Stage logic 3 and 27, the kind-7 record, and the type 9 `var` values ≥ 64.
6. (answered in [`shark-2.md`](shark-2.md)) Main types 35 and 16 of area 1: 35 is Nightmare Freeze
   (the Nightmare Effect 2 object), 16 is Nightmare Pressure, the mini boss.
