# Stage: Commander Yammark (stage 1, overlay 03)

Section 1 of Commander Yammark's stage: DAT member 95, 26 × 9 screens, 108 object records.
Overlay `03` at RAM `0x800E9860`. Save states: `yammark-1_start`, `yammark-2_boss`,
`yammark-3_caterpirade` (in `~/mmx-data/x6/dumps/`). Stage files: `~/mmx-data/x6/stages/m095-commander-yammark-section-1/`.

The record format and the kinds are in `docs/objects-legend.md`; the shared rules in
`docs/engine.md`. Sheets in `docs/enemies/`: `worm-spider`, `killer-moth`, `nightmare-insect`,
`caterpirade`, `rescue-reploid`, `nightmare-virus`, `teleport-crystal`, `light-capsule`, `boss-gate`,
`rain-effect`, `darkness-effect`, `commander-yammark`.

## Objects

All 108 records [certain for the counts]: records 0–33 are kind 3 (class: stage logic; see "Stage
logic records" below), record 34 and records 53–107 except 106 are kinds 0–2 (class: main
objects), records 35–52 and 106 are kind 4 (class: stage objects). No record has kind 5, 6 or 7.

The table below has kinds 0–2 and 4, from left to right. The type is the record `b`: for kinds 0–2
it is the main object type [certain, as in the intro]; for kind 4 it is the stage-object type.
Names come from the sprite banks (`docs/objects-legend.md`) unless marked.

| record | kind | difficulty | type | object | id | x | y |
|---|---|---|---|---|---|---|---|
| 42 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 5 | 48 | 1168 |
| 41 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 4 | 64 | 816 |
| 54 | 0 | all | 15 | Nightmare Virus (normal) | 1 | 64 | 1296 |
| 91 | 0 | all | 23 | Nightmare Insect | 0 | 96 | 1152 |
| 105 | 0 | all | 47 | item object (main type 47; `id` = item kind) [likely] | 39 | 128 | 816 |
| 55 | 0 | all | 15 | Nightmare Virus (normal) | 1 | 192 | 1424 |
| 92 | 0 | all | 23 | Nightmare Insect | 0 | 336 | 1536 |
| 63 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 384 | 1004 |
| 59 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 544 | 380 |
| 40 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 3 | 544 | 800 |
| 90 | 0 | all | 23 | Nightmare Insect | 0 | 576 | 992 |
| 71 | 0 | all | 3 | Worm Spider | 1 | 736 | 864 |
| 62 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 768 | 972 |
| 60 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 784 | 460 |
| 53 | 0 | all | 15 | Nightmare Virus (normal) | 1 | 800 | 896 |
| 70 | 0 | all | 3 | Worm Spider | 1 | 928 | 816 |
| 37 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 0 | 992 | 480 |
| 43 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 6 | 1008 | 1360 |
| 61 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 1056 | 428 |
| 69 | 0 | all | 3 | Worm Spider | 0 | 1112 | 912 |
| 88 | 0 | all | 23 | Nightmare Insect | 0 | 1248 | 416 |
| 104 | 0 | all | 47 | item object (main type 47; `id` = item kind) [likely] | 0 | 1328 | 2080 |
| 57 | 0 | all | 15 | Nightmare Virus (snatcher) | 32 | 1352 | 1984 |
| 51 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 14 | 1352 | 2080 |
| 45 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 8 | 1424 | 2080 |
| 38 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 1 | 1440 | 272 |
| 80 | 0 | all | 4 | Killer Moth | 0 | 1440 | 576 |
| 89 | 0 | all | 23 | Nightmare Insect | 0 | 1440 | 800 |
| 56 | 0 | all | 15 | Nightmare Virus (snatcher) | 32 | 1552 | 1744 |
| 50 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 13 | 1552 | 1824 |
| 93 | 0 | all | 23 | Nightmare Insect | 0 | 1632 | 1384 |
| 39 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 2 | 1728 | 800 |
| 81 | 0 | all | 4 | Killer Moth | 0 | 1744 | 1712 |
| 72 | 0 | all | 3 | Worm Spider | 1 | 1760 | 1224 |
| 106 | 4 | – | 26 | Light capsule (`id` 3 = armour part bit `0x08`) | 3 | 1800 | 776 |
| 64 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 1888 | 1820 |
| 73 | 0 | all | 3 | Worm Spider | 0 | 1952 | 1344 |
| 82 | 0 | all | 4 | Killer Moth | 0 | 2032 | 1696 |
| 74 | 0 | all | 3 | Worm Spider | 1 | 2080 | 1272 |
| 83 | 0 | all | 4 | Killer Moth | 0 | 2192 | 1680 |
| 65 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 2208 | 1788 |
| 75 | 0 | all | 3 | Worm Spider | 0 | 2224 | 1280 |
| 76 | 0 | all | 3 | Worm Spider | 1 | 2304 | 1208 |
| 66 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 2368 | 1772 |
| 94 | 0 | all | 23 | Nightmare Insect | 0 | 2384 | 1344 |
| 67 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 2432 | 1868 |
| 77 | 0 | all | 3 | Worm Spider | 0 | 2448 | 1376 |
| 84 | 0 | all | 4 | Killer Moth | 0 | 2480 | 1680 |
| 78 | 0 | all | 3 | Worm Spider | 0 | 2528 | 1296 |
| 85 | 0 | all | 4 | Killer Moth | 0 | 2592 | 1808 |
| 68 | 0 | all | 7 | Caterpirade (main type 7 spawns it) | 0 | 2592 | 1948 |
| 79 | 0 | all | 3 | Worm Spider | 1 | 2608 | 1224 |
| 86 | 0 | all | 4 | Killer Moth | 0 | 2656 | 1760 |
| 95 | 0 | all | 23 | Nightmare Insect | 0 | 2688 | 1312 |
| 87 | 0 | all | 4 | Killer Moth | 0 | 2688 | 1760 |
| 52 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 15 | 2832 | 1360 |
| 34 | 0 | all | 28 | teleporter crystal (area change) | 1 | 3008 | 1632 |
| 58 | 0 | all | 15 | Nightmare Virus (snatcher) | 33 | 3098 | 1240 |
| 46 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 9 | 3120 | 1728 |
| 44 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 7 | 3136 | 1264 |
| 96 | 0 | all | 23 | Nightmare Insect | 0 | 3264 | 1744 |
| 97 | 2 | Hard only | 23 | Nightmare Insect | 0 | 3448 | 2064 |
| 98 | 0 | all | 23 | Nightmare Insect | 0 | 3728 | 2032 |
| 99 | 0 | all | 23 | Nightmare Insect | 0 | 4016 | 1904 |
| 49 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 12 | 4304 | 1872 |
| 48 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 11 | 4304 | 2048 |
| 102 | 0 | all | 23 | Nightmare Insect | 0 | 4352 | 2016 |
| 100 | 0 | all | 23 | Nightmare Insect | 0 | 4432 | 1776 |
| 101 | 2 | Hard only | 23 | Nightmare Insect | 0 | 4480 | 1936 |
| 103 | 0 | all | 23 | Nightmare Insect | 0 | 4608 | 2048 |
| 47 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 10 | 4672 | 1792 |
| 35 | 4 | – | 8 | stage object 8: boss gate 1 (case 3) | 67 | 4848 | 1968 |
| 36 | 4 | – | 8 | stage object 8: boss gate 2, before the boss | 4 | 5104 | 2016 |
| 107 | 0 | all | 49 | Commander Yammark | 0 | 5280 | 2016 |

Notes:

- Worm Spider: `id` 0 = floor nest, 1 = ceiling nest. No water jumper (`id` 2) in this section.
- All 8 Killer Moths have `id` 0 (they start in HOVER).
- Caterpirade: main type 7 makes the stage object (type 1) and deletes itself; `id` is not read
  (`docs/enemies/caterpirade.md`).
- Rescue Reploids (records 37–52): the `id` is the global Reploid number 0–15, the index of its
  status nibble in `game.reploid`. Records 40, 41, 45, 46, 50 and 51 stand on the floor; the others
  hover [likely]. In `yammark-2_boss`, Reploids 0, 1, 2, 5, 8, 9, 10, 12, 13 and 14 are saved
  (`docs/enemies/rescue-reploid.md`).
- Nightmare Virus: records 53–55 (`id` 1) are normal viruses. Records 56 and 57 (`id` 32) are
  snatchers 80–96 px above Reploid records 50 and 51; record 58 (`id` 33) is a snatcher next to
  record 44 (`docs/enemies/nightmare-virus.md`).
- Light capsule (record 106): `id` 3 gives the armour part bit `0x08` (Blade Armor; which part is
  open). `game.armor_parts` went from 0 to `0x08` between `yammark-1_start` and `yammark-2_boss`.
- Teleporter crystal (record 34): `id` 1 asks for an area change (`game.exit_req` = `0x41`); the
  destination is not known (`docs/enemies/teleport-crystal.md`).
- Boss gates: gate 1 (record 35, `id` 67 = case 3) sets the camera to (4800, 1904) and walks the
  player at 0.83 px/frame; gate 2 (record 36, `id` 4) gives camera x 5104–5136 and stores the stage
  time. Both checked in the save state (`docs/enemies/boss-gate.md`).
- Item objects (main type 47, records 104 and 105): placed items, `id` = item kind [likely].
- Wind and rain (Nightmare Effect 6): one zone, x 3200–4800, y 1648–2160. The second record list
  places the generator (below; `docs/enemies/rain-effect.md`).
- Commander Yammark (record 107, at 5280, 2016): the code ignores the record position and moves him
  to 240 px right of and 32 px above the room's top-left corner (the camera's target minimum after
  gate 2: x 5104, y 1904). His x limits are 5120–5440 (room x 16–336). He makes his drones
  himself; no record places them (`docs/enemies/commander-yammark.md`).

## Stage logic records (kind 3, records 0–33)

All 34 are stage-logic objects (class `0x8009B7B0`, update table scratchpad `+0x8C`). The types
are main-program code, the same as in the intro and in Turtloid: 0 `CameraZone` (`0x80052258`),
6 `CheckpointMarker` (`0x80052D30`), 14 `TalkZone` (`0x8005335C`) [certain for the
table in `yammark-1_start`].

### Checkpoint markers (type 6) and the respawn points

| record | `id` | x, y | checkpoint | respawn: player x, y | respawn camera | respawn limits |
|---|---|---|---|---|---|---|
| – | – | – | 0 (start) | 160, 256 | 0, 224 | x 0–1200, y 208–720 |
| 0 | 1 | 608, 1568 | 1 | 512, 1504 | 432, 1424 | x 432–1040, y 1424 |
| 2 | 3 | 3168, 1920 | 3 | 3136, 1920 | 3024, 1904 | x 3024–4528, y 1648–1904 |
| 1 | 2 | 4864, 1968 | 2 | 4960, 1968 | 4800, 1904 | x 4800, y 1904 |

The marker sets `game.checkpoint` = `id & 0x7F` when the player's x reaches the record x [certain
for the code]. The respawn values come from `checkpoint_start_tbl` (`0x80073300`, list
`0x80073140` for stage 1 area 0; record format in `docs/stages/turtloid.md`, "Checkpoints"). All
face right. The numbers are not in stage order: checkpoint 3 (x 3168) comes before checkpoint 2
(x 4864, after boss gate 1). The list has 4 entries (the list of stage 2 starts at `0x80073150`).

### Camera zones (type 0, 25 records)

Each record takes entry `id & 0x7F` of `camera_zone_tbl` (`0x80079D70`, word 2 for stage 1 area 0:
`0x800792C0`). While the player is inside the rectangle, the zone sets these target limits
("keep" = `0xFFFF`, not changed) [certain for the table and the code]. Most zones come in pairs at
one place: one for each direction the player moves through.

| record | `id` | x, y | player rectangle | target limits |
|---|---|---|---|---|
| 8 | 5 | 224, 1504 | x 224–239, y 1456–1583 | y min 1424 |
| 9 | 6 | 224, 1504 | x 208–223, y 1456–1583 | y min 672 |
| 7 | 132 | 592, 1568 | x 592–607, y 1536–1615 | x 432–1040; `id` bit 7: also the texture swap, then deleted |
| 5 | 2 | 1088, 864 | x 1072–1087, y 832–911 | y 672–1424 |
| 6 | 3 | 1088, 864 | x 1088–1103, y 832–911 | y 208–720 |
| 10 | 7 | 1120, 1424 | x 1088–1151, y 1408–1423 | x 896–2752, y 1200 |
| 11 | 8 | 1120, 1424 | x 1088–1151, y 1424–1439 | x 432–1040, y 1424 |
| 12 | 9 | 1136, 1648 | x 1088–1199, y 1648–1663 | x 944–1280, y 1648 |
| 13 | 10 | 1136, 1648 | x 1088–1199, y 1632–1647 | x 432–1040, y 1424 |
| 14 | 11 | 1344, 1344 | x 1344–1359, y 1312–1407 | x min 1168 |
| 20 | 17 | 1376, 1760 | x 1376–1391, y 1728–1807 | x min 1216 |
| 21 | 18 | 1504, 1872 | x 1472–1535, y 1872–1887 | x min 1280, y max 1904 |
| 22 | 19 | 1504, 1824 | x 1472–1535, y 1824–1839 | x min 1216, y max 1648 |
| 3 | 0 | 1552, 736 | x 1504–1519, y 704–799 | x max 1568, y 640 |
| 4 | 1 | 1552, 736 | x 1488–1503, y 704–799 | x max 1200, y 208–720 |
| 23 | 20 | 1584, 1776 | x 1584–1599, y 1712–1839 | x max 4528 |
| 24 | 21 | 1584, 1776 | x 1568–1583, y 1712–1839 | x max 1280 |
| 25 | 22 | 2400, 1728 | x 2400–2415, y 1664–1791 | y max 1904 |
| 26 | 23 | 2400, 1728 | x 2384–2399, y 1664–1791 | y max 1648 |
| 15 | 12 | 2880, 1296 | x 2880–2895, y 1216–1375 | y max 1904 |
| 16 | 13 | 2880, 1296 | x 2864–2879, y 1216–1375 | y max 1200 |
| 19 | 16 | 2960, 1824 | x 2912–3023, y 1824–1839 | x 2672–4528, y min 1648 |
| 17 | 14 | 3040, 1248 | x 3040–3055, y 1216–1279 | x max 2880 |
| 18 | 15 | 3040, 1248 | x 3024–3039, y 1216–1279 | x max 2752 |
| 27 | 24 | 3104, 1984 | x 3104–3135, y 1920–2031 | x min 3024 |

### Talk zones (type 14, `TalkZone`, 6 records)

| record | `id` | x, y |
|---|---|---|
| 28 | 129 | 80, 384 |
| 29 | 130 | 512, 256 |
| 30 | 131 | 752, 896 |
| 31 | 132 | 80, 768 |
| 32 | 133 | 1760, 1792 |
| 33 | 134 | 4944, 2048 |

State 0 tests the player against a rectangle (per stage, `id & 0x0F`, table `0x8007A154`) and a bit
of `game+0x9E` (`0x800533F4`), then sets `mega+0x14E` = `id & 0x0F` and plays sound 42 or asks for
`PlayerEvent(20)` [certain for the code; read in `docs/stages/sheldon.md`]. These are Alia's optional
calls: the "!" icon, and Select opens the text (`docs/engine.md`, "Talk zones") [certain for the code].

## The second record list (`fixed_set_tbl`, 21 records)

`fixed_set_tbl` (`0x80073554`) entry for stage 1 area 0 points to `0x800F66D0`. All records are at
(0, 0) [certain]:

| records | kind | type | `id` | class | object |
|---|---|---|---|---|---|
| 17 | 3 | 3 | 0–16 | stage logic | palette cycles (`PaletteCycle`, `docs/stages/turtloid.md`); all 17 live in `yammark-1_start` (slots 1–17) |
| 3 | 6 | 5 | 0, 1, 2 | ray | the darkness effect, zones 0–2 (`docs/enemies/darkness-effect.md`); deleted at once when the Nightmare Effect is not 8 |
| 1 | 0 | 37 | 0 | main | the rain generator (`docs/enemies/rain-effect.md`); deleted at once when the Nightmare Effect is not 6 |

The palette cycles of this stage (target palette ← source palettes, updates per step, loop length):

| `id` | target | sources | updates | loop |
|---|---|---|---|---|
| 0 | 177 | 0, 1, 2, 1 | 20 | 80 |
| 1 | 164 | 3, 4, 5, 4 | 16 | 64 |
| 2 | 150 | 6, 7, 8, 9, 8, 7 | 16 | 96 |
| 3 | 119 | 10, 11, 12, 11, 10, 11, 12, 13, 12, 11 | 16 | 160 |
| 4 | 107 | 14, 15, 16, 17, 16, 15 | 20 | 120 |
| 5 | 103 | 18, 19, 20, 21, 20, 19 | 16 | 96 |
| 6 | 174 | 22, 23, 24, 25, 24, 23 | 16 | 96 |
| 7 | 166 | 26, 27, 28, 29, 28, 29, 28, 27 | 18 | 144 |
| 8 | 167 | 30, 31, 32, 33, 32, 31 | 14 | 84 |
| 9 | 158 | 34, 35, 36, 37, 36, 35 | 6 | 36 |
| 10 | 163 | 38, 39, 40, 41, 41, 40, 39 | 16 | 112 |
| 11 | 161 | 42, 43, 44, 45, 44, 45, 44, 43 | 12 | 96 |
| 12 | 156 | 46, 47, 48, 49, 50, 49, 48, 47 | 12 | 96 |
| 13 | 179 | 51, 52, 53, 54, 53, 52 | 16 | 96 |
| 14 | 182 | 55, 56, 57, 56, 55, 56, 57, 58, 57, 56 | 16 | 160 |
| 15 | 188 | 59, 60, 61, 62, 61, 60 | 14 | 84 |
| 16 | 130 | 63, 64, 65, 64, 65, 64 | 12 | 72 |

Each cycle moves one palette of 16 colours. Which tiles use each palette was not checked for this
stage (the Turtloid probe `p8_paltiles.py` can do it).

## Open questions

1. The destination of the teleporter crystal (record 34).
2. Which Blade Armor part is bit `0x08`.
3. What the event zones (type 14) show, and which tiles the 17 palette cycles animate.
4. Camera zone table entry 25 (x 2720–2751, y 224–431, y max 1184) has no record in this list.
