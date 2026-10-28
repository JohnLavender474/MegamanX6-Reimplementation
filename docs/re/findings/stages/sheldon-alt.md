# Stage: Shield Sheldon, alternate section (stage 22, overlay 31)

The teleporter crystal in the lower rooms of Shield Sheldon's stage (record 110, main 28, `id` 1,
at (4744, 928); [`sheldon.md`](sheldon.md)) takes X to a hidden area with its own stage number:
`game.stage` = 22, area 0 [certain: `sheldon-10_section-2` is the next state]. It is one of the
8 hidden areas (stages 19–22; `docs/engine.md`, "Boss overlay"). The alternate Turtloid section
(stage 21, area 1, [`turtloid.md`](turtloid.md)) is the same kind of area.

Save states (in `~/mmx-data/x6/dumps/`; X with no armour, form 0; difficulty 1; `boss_choice` 1):

| state | checkpoint | X at (x, y) | camera (x, y) | what |
|---|---|---|---|---|
| `sheldon-10_section-2` | 0 | 160, 395 | 0, 224 | start; no main object is live yet |
| `sheldon-11_laser-shot` | 0 | 493, 363 | 333, 224 | a fixed beamer (record 29, x 552) has just shot a laser |
| `sheldon-12_high-max` | 1 | 2617, 395 | 2576, 224 | boss room; High Max (main 55, `var` 1) is spawning |

## Map

- Overlay 31, stage member 121, a layout of 26 × 3 screens (catalog entry "Shield Sheldon,
  alternate section", from `sheldon-10_section-2`;
  `~/mmx-data/x6/stages/m121-shield-sheldon-alternate-section/`). Layer 0 uses screen columns 0–11
  (x 0–3071): rows 0 and 1 in all of them, and row 2 in columns 4–8.
- `learn` fit-test history: all three states fit at the first try, with no tool change. Each gives
  member 121, overlay 31 at `0x8D44`, edges 0.952 / 1.000, 0 differing cells, 42 records at
  `0x8E30` (list entry 22, area 0). Palettes from 115 (`sheldon-10`), 137 (`sheldon-11`) and 336
  (`sheldon-12`) tiles. Only `sheldon-10` is in `catalog/stages.json`. The layout bytes of
  `sheldon-12` are the same as those of `sheldon-10`.
- Overlay numbers: stage 21 area 1 is overlay 30, stage 22 area 0 is overlay 31. Both agree with
  overlay = 25 + 2 × (stage − 19) + area [likely; two points only].
- The Nightmare Effect: `nightmare_effect` is still 1 (the Nightmare Bugs of stage 7).
  `StageStartReset` writes it only for stage < 19 and area 0, so a hidden area keeps the value of the
  stage before it [certain for the code; the Nightmare Bugs are live in `sheldon-11`].

From left to right [certain for the tiles; the names are from the art]:

| x (px) | y (px) | part |
|---|---|---|
| 0–319 | 256–511 | the start: a floor under a building |
| 320–1279 | 256–511 | a spike floor (collision 63) with pillars; hanging bars (45) over it |
| 1280–2239 | 256–767 | a pit with hanging bars, short pillars and one rope (46, x 2016–2031, y 352–447) |
| 2240–2575 | 256–511 | the corridor between boss gates 48 and 49 (green machines) |
| 2576–2927 | 256–511 | the boss room |

Row 0 (y 0–255) holds the ceiling (collision 57 at y 208) and the fixed beamers. The camera y is
always 224, so the view is y 224–463.

Collision values in layer 0 (cells): 0 (6 162), 56 (958), 57 (175), 63 (45), 45 (62), 59 (16),
46 (6). Value 45 is a hanging bar (also in the intro) and 46 a rope (`docs/TODO.md`, from the
player code); this is the first stage map of the project with value 46. The bars are one-cell lines: x 368–463 (y 304),
832–975 (336), 1088–1199 (336), 1280–1391 (304), 1440–1599 (368), 1648–1727 (368), 1744–1839
(352), 1872–1967 (416), 2096–2191 (320). Value 59 is on the two boss gates (2 × 4 cells at x 2240
and 2560, y 352–415).

## Camera

- No camera zone: the zone table word of stage 22 area 0 (`0x80079D70` + 4 × 44) is 0, and the
  list has no stage logic 0 record [certain].
- Start limits: x 0–1936, y 224–224 (`sheldon-10`, `sheldon-11`; ease speed 4). 1936 + 320 = 2256,
  the x of boss gate 48.
- Boss gate 48 (x 2256) sets min x = max x = 2256: the corridor. Boss gate 49 (x 2576) sets
  x 2576–2608 (`docs/enemies/boss-gate.md`, cases 48 and 49). `sheldon-12` has these limits:
  x 2576–2608, y 224–224 [certain].
- Gate 48 has no open condition (the Turtloid gate 44 opens only when `game+0x26` = 0, after
  Weather Analyze machine 4). In `sheldon-12` gate 48 is in state 1 and gate 49 in state 2 (closed
  for good).

## Objects

42 records (list entry 22, area 0), by class and type [certain for the counts]:

| class, type | records (kind 0 / 1 / 2) | object |
|---|---|---|
| main 13 | 9 | fixed beamers, all `id` 6 (fixed beamer, down), at y 248: x 552, 984, 1144, 1480, 1576, 1736, 1880, 1960, 2040 |
| main 15 | 3 / 0 / 4 | Nightmare Virus: `id` 0 ×6, `id` 34 at (1976, 272) |
| main 22 | 6 | Nightmare Bug (`id` 0) |
| main 30 | 2 / 1 / 0 | Guardian (`id` 0) at (632, 360), (1256, 328), (1624, 392) |
| main 47 | 1 | item, `id` 41, at (720, 400) |
| stage object 4 | 8 | Rescue Reploids, `id` 104–111 |
| stage object 15 | 3 | Nightmare crates (`docs/enemies/nightmare-crate.md`): a column at x 1416, y 296 (`id` 3, red, explodes), 344 (`id` 0, grey, slides 48 px), 392 (`id` 3); only with Nightmare Effect 5, so not live here (the value is 1) [certain for the code: the same code as overlay 10, entry `0x800F1DC4`] |
| stage object 8 | 2 | boss gates `id` 48 (2256, 384) and 49 (2576, 384) |
| stage logic 6 | 1 | checkpoint marker `id` 1, x 2272 |
| stage logic 14 | 1 | navigator call point `id` 132 at (16, 224): optional message 4, rectangle x 16–224, y 224–400 (table index 19; [`sheldon.md`](sheldon.md), "Navigator call points") |
| stage logic 13 | 1 | `BossSpawner` at (2640, 384) |

The second list (`fixed_set_tbl`, stage 22 area 0, at `0x800F27E8`): 7 records of stage logic 3
(`id` 0–4, 7, 8), then the end record. Stage logic 27 is live, as in section 1: it is the stage
start banner, made by the stage start code and not by a record ([`sheldon.md`](sheldon.md), "Stage
logic 27"). Stage 22 has no glass slide (no stage logic 7 record and no collision 60).

### Lasers

The laser puzzle of this area is only fixed beamers: no reflector, no turning part and no laser
gate. Each beamer is on the ceiling and shoots down across the bar route
([`docs/enemies/laser-reflector-puzzle.md`](../enemies/laser-reflector-puzzle.md)). In
`sheldon-11`, record 29 (x 552) is in state 1/3/3; its laser is one visual object (type 11) at
(552, 424) and three misc type 7 objects at (552, 264), (552, 392) and (552, 424).

## The boss: High Max

- The `BossSpawner` (stage logic 13, `0x80052E44`, main program) is the same code as in stage 21:
  index 13 of the stage-logic type table points to it in both stages. With `boss_choice` 1 it loads
  overlay 44 at `0x800FA000` and makes main type 55, `var` 1: High Max [certain:
  `sheldon-12_high-max`, where 1 of 33 304 bytes of overlay 44 differs from the disc, as in
  `turtloid-8_high-max`].
- In `sheldon-12`, High Max is main slot 0, type 55, `var` 1, state 1/2/3 (spawning), at
  (2816, 265). X stands at (2617, 395) (step 20).
- Behaviour: `docs/enemies/high-max.md`. Export: `high-max` (`enemy-b15382edbf`), live, 15 of 23
  animations: the same folder and the same animations as from `turtloid-8_high-max`.

Compared with the Turtloid hidden area:

| | stage 21, area 1 (Turtloid) | stage 22, area 0 (Sheldon) |
|---|---|---|
| overlay, member, layout | 30, 120, 8 × 5 | 31, 121, 26 × 3 |
| records | 41 | 42 |
| route | one tall room around a shaft | one long corridor: bars over spikes, a pit, lasers from the ceiling |
| first boss gate | `id` 44, x 1072: needs `game+0x26` = 0 | `id` 48, x 2256: always opens |
| boss room camera | x 1264–1296, y 704 | x 2576–2608, y 224 |
| `BossSpawner` record | (1360, 864): camera min + (96, 160) | (2640, 384): camera min + (64, 160) |
| boss banks without texture | 14–17: types 53, 55, 55, 63 | 12–15: types 53, 55, 55, 63 |

The boss room is the same size (a view of 320 + 32 px wide, one screen high). The spawner record is
32 px further left relative to the room. High Max's room points are relative to the camera minimum
(`docs/enemies/high-max.md`), so the fight is the same [likely].

## Sprite exports

`spriteexport` on the three states, with no failure:

- `sheldon-10`: 7 search exports (no main enemy is live): `nightmare-virus`,
  `nightmare-virus-orb`, `laser-reflector-puzzle`, `hover-gunner`, `guardian`, `mettaur-d2`,
  `nightmare-bug`. Banks 11–15 have no texture (types 0, 53, 55, 55, 63).
- `sheldon-11`: live `laser-reflector-puzzle`, `guardian`, `nightmare-bug`, `rescue-reploid`; the
  others from a search.
- `sheldon-12`: live `high-max` and `boss-gate`. The High Max texture covers banks 4–10, so the
  search export of type 13 in this state (`enemy-9504756be1`) is not clean; do not name it.

## Open questions

1. Where the area leads after the boss (the exit of a hidden area).
2. Stage logic 3. (Stage object 15 is answered: the Nightmare crates.)
3. The item `id` 41 (main 47) at (720, 400).
4. A state with `boss_choice` 2 (Dynamo) in this area. Stage 22 area 1 has one:
   `mijinion-4_dynamo` ([`mijinion-alt.md`](mijinion-alt.md)).
