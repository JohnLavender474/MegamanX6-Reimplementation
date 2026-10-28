# Stage tools: proposed doc changes (2026-10-22)

Probes are in [`~/mmx-data/x6/re/stage-tools/`](./) (`texture_survey.py`, `vram_match.py`,
`find_small.py`, `header26.py`, `typelist.py`, `mapcheck.py`, `valuecmp.py`, `ovdis.py`).
Result of the VRAM match of all 44 save states: `vram_match.txt`.

## 1. `docs/engine.md`, "Layout"

### 1a. Replace the sentence about the 26 bytes

Old: "On the disc, 26 bytes (`ff ff ff ff ff ff 80 …`) come before each layout; what they are is
not known."

New:

- **The layout source** [certain for the code]. `LayoutLoad` (`0x8002798C`) copies
  `3 × layout_size` bytes from `stage_layout_tbl[stage × 2 + area]` (`0x800762E0`, one word per
  stage and area; stage 11 takes `0x800EA514[game+0x1D]`) to scratchpad `+0x04`
  (`0x80097438`), then calls `0x800268B0(-1)`. The table is directly before the size table
  `0x80076398` (`0x800762E0 + 23 × 8`). The pointers of the eight learned layouts are exactly the
  catalog offsets (`0x800E9860 + offset`).
- **The bytes before a layout are not part of it** [certain]. No code reads them through the
  layout pointer. They are, in this order: a run of `0xFF` (44–52 bytes; no pointer in the main
  program or the overlay points at its start), the **stage type list** (`stage_bank_types`,
  `0x800733C4`, read by `GetSpriteBank` `0x8002A47C`; byte list, `0xFF` ends), and 1–3 zero bytes
  that align the layout to 4 bytes. The type-list pointer is the layout pointer − 20 for most
  stages, − 12 for stage 4 area 0 (a 9-type list), − 20 for stage 21 area 1 (with 1 pad byte).
  "26 bytes" is only the distance of the wrong address `0x8009741E`; it has no meaning on the
  disc. Example, intro (overlay 02): fill `0x10798`–`0x107CB`, type list `0x107CC`–`0x107DC`
  (`80 81 00 01 02 05 84 91 92 00 00 00 89 37 37 30 ff`), pad 3, layout `0x107E0`.
- In RAM, `0x8009741E`–`0x80097437` hold other game data (for example `0x80097426`, read by the
  set-up at `0x800142D8`), not the disc bytes.

### 1b. Stage member table (new bullet)

- **Stage member** [certain for the code]. The stage load (`0x80014054`) takes the map member
  from `stage_member_tbl` (`0x8006D9FC`, halfword per stage × 2 + area). Exceptions: stage 15
  area 0 takes `0x8006DA58[game+0x38]` (110 X, 111 Zero); stage 13 area 1 takes
  `0x8006DA5C[game+0x36 − 1]` (141–148); stage 11 takes `0x8006DA6C[game+0x1D]` (149–161).
  Stages 9 and 10 have no member (0).

### 1c. Replace "A stage member can have two texture blocks [certain for member 102]" and the
member 98/99 bullet with:

- **Texture blocks** [certain where a save state shows it, else observed in the entry list]. A
  map member has one or two 256 KB texture blocks:

  | members | stage, area | `0x10000` (pages) | `0x16` (rows) | `0x10014` (pages) | proof |
  |---|---|---|---|---|---|
  | 94 | 0 intro | yes | yes | – | `0x10000`: `intro-1`, `intro-2`; `0x16`: `intro-3_boss` |
  | 95 | 1 Yammark | yes | yes | – | `0x10000`: `yammark-1`, `-3`; `0x16`: `yammark-2_boss` |
  | 96, 97 | 2, 3 | yes | yes | – | no state |
  | 98 | 4 area 0 Shark | yes | yes | – | `0x10000`: `shark-1`; `0x16` not in a state yet |
  | 99 | 4 area 1 | yes | yes | – | `0x10000`: `shark-2`, `-3`; `0x16`: `shark-4` to `-9` |
  | 100 | 5 Scaravich | yes | yes | – | `0x10000`: `scaravich-1`, `-2` |
  | 101 | 6 Turtloid | yes | yes | – | `0x10000`: `turtloid-1`; `0x16`: `turtloid-2`, `-3`, `-6`, `-7` |
  | 102 | 7 Sheldon | yes | yes | – | `0x10000`: `sheldon-1` to `-7`, `-13`; `0x16`: `-8`, `-9`, `-14` to `-17` |
  | 103 | 8 | yes | yes | – | no state |
  | 104, 105 | 12 area 0, 1 | yes | yes | – | no state |
  | 112, 113, 114 | 16, 17, 18 | yes | yes | – | no state |
  | 115–122 | 19–22 (area 0, 1) | yes | – | – | `0x10000`: 120 (`turtloid-4`, `-5`, `-8`), 121 (`sheldon-10` to `-12`) |
  | 106 | 13 area 0 | – | 32 KB only | yes | no state |
  | 141–148 | 13 area 1 | – | – | yes | no state |
  | 107, 108 | 14 area 0, 1 | – | – | yes | no state |
  | 110, 111 | 15 area 0 (X, Zero) | – | – | yes | `0x10014` at VRAM (512, 0) in `report-1`, `report-2_shark` |
  | 109 | 15 area 1 | – | yes | yes | no state |
  | 149–161 | 11 (by `game+0x1D`) | – | yes | yes | no state |

  Entry `0x10014` is stored page by page like `0x10000`, but the stage load puts it at VRAM
  (512, 0), not (320, 256) [certain for 110 and 111: all 2048 rows match]. Member 156 has two
  `0x10014` entries (2 KB and 256 KB); `learn` takes the 256 KB one. Entry `0x16` is row by row in
  every state where it is in VRAM (members 94, 95, 99, 101, 102). Whether `0x16` of members 109
  and 149–161 (next to `0x10014`) is also a row-order block is not known.
- Other entries of type `0x1xxxx` in the map members are not 256 KB texture blocks:
  `0x10001` (32 KB; in 110 it is at VRAM (960, 256), one page), `0x10007`, `0x10013` (2–32 KB),
  `0x10016` (64 KB in 110, 111; 96 KB in 106, 107; not found in VRAM in `report-1`), `0x10515`
  (42–142 KB), `0x10102`, `0x1060d`. Member 137 is not a map member: it has no screens or tile
  info; its entries (`0x1040a` 160 KB, `0x7`, `0x20202`) are the same kinds as members 76–82 and
  123–140 (sprite-bank members). Member 73 has screens and tile info but no texture, and no stage
  uses it.

### 1d. "Wall tiles" bullet: replace with the collision value section of item 4 below.

## 2. `docs/TODO.md`

- "Check the other stage members for a second texture entry (`0x16`) …": mark done; point to
  the table in 1c. Open: a save state with `0x16` in VRAM for member 98, and states for stages
  11, 13 and 14 (the `0x10014` members, to confirm `learn` on a real map with that block).
- New open item: the intro, Yammark and Turtloid stages also have a second texture in VRAM in
  `intro-3_boss`, `yammark-2_boss`, `turtloid-2_mid` (and `-3`, `-6`, `-7`). Their catalog
  entries draw only the `0x10000` art, so the boss parts of these maps are drawn with the wrong
  tiles. `learn` with no tool change gives the entries (tested in a catalog copy):
  - `learn --state intro-3_boss.state.gz --name "Intro, boss part"`: member 94, texture `0x16`,
    0 cells differ, 57 objects, palettes from 387 tiles.
  - `learn --state yammark-2_boss.state.gz --name "Commander Yammark, section 1, second texture"`:
    member 95, texture `0x16`, 108 objects, palettes from 412 tiles.
  - `learn --state turtloid-2_mid.state.gz --name "Rainy Turtloid, section 1, second texture"`:
    member 101, texture `0x16`, 137 objects, palettes from 136 tiles.
  The main session should add them (the stage-tools agent may not add entries).
- "Re-learn the older catalog entries once …": done 2026-10-22. Changed: only `edge.v` and
  `edge.v_joins`. Intro 0.944/39 → 0.980/16; Yammark 0.956/83 → 0.964/38; Turtloid section 1
  0.846/74 → 0.946/36; Turtloid alternate section (three entries) 0.735/17 → 0.979/6. All other
  fields (layout, offsets, differing cells, objects, palettes) are the same. Every entry still
  fits; no score went down. Metal Shark Player section 1 lower part keeps `"texture": 22`; a new
  `learn` from `shark-1_ride` would remove it (that state has `0x10000` in VRAM), so the field was
  not re-learned.
- "The 26 bytes on the disc before each layout": done; see 1a.
- "Check every map again by eye after the fix": the code check is done (item 3 below); the eye
  check of the suspect joins is still open.
- Collision legend: name the values as in item 4; the to-do "name the open values (32, 45, 16, 63,
  1–8, 33, 34)" is done from the code.
- Deferred minor: `stages --out ~/mmx-data/x6/stages` now refuses that folder, because other agents
  put `probes-*`, `*-doc-changes.md` and `*-helpers-proposed.json` files in it. Move them out, or
  draw into a new folder.

## 3. Map check by code (after the layout-address fix)

Method (`mapcheck.py`): for every catalog layout, layer 0 only: (a) the layout read at offsets
−40…+40 from the catalog offset: offset 0 has the most matched joins in every map (the ties at
−1, −2 and +1 come from the zero pad bytes and an empty corner cell), so no map has a shift, a row-0
header or a wrap; (b) every join between two screens with collision, with its edge match;
(c) empty cells with three or four full neighbours. The current drawings in [`~/mmx-data/x6/stages/`](../stages/)
all use the catalog offsets; the `stages` output of the old and the new tool code is the same (the
`.ora` files differ only in zip time stamps).

| map | h / v score | weak joins (< 0.6) | inner empty cells | state of check |
|---|---|---|---|---|
| Intro | 0.929 / 0.980 | 3 | (17, 4) | by eye: start part (TODO 2026-10-19, before the fix); code: no shift |
| Commander Yammark, section 1 | 0.923 / 0.964 | 8 | – | code: no shift |
| Metal Shark Player, section 1 | 0.964 / 1.000 | 1 | (5, 1), (5, 4), (4, 6) | code: no shift; `docs/stages/shark.md` |
| Metal Shark Player, section 2 | 0.976 / 0.896 | 2 | (1, 2), (5, 2), (6, 2), (9, 2) | code: no shift; `docs/stages/shark-2.md` |
| Rainy Turtloid, section 1 | 0.910 / 0.946 | 12 | – | code: no shift |
| Shield Sheldon, section 1 | 0.975 / 0.792 | 1 | (8, 2) | code: no shift; `docs/stages/sheldon.md` |
| Rainy Turtloid, alternate section | 0.928 / 0.979 | 1 | – | code: no shift |
| Shield Sheldon, alternate section | 0.952 / 1.000 | 1 | – | code: no shift |

Suspect joins for the eye check (match < 0.1; layout cell (x, y), pixel position):

- Rainy Turtloid, section 1: (4, 0) → (5, 0), screens 1 → 2, x 1280, y 0–255 (match 0.06);
  (9, 1) → (10, 1), screens 32 → 33, x 2560, y 256–511 (0.06); (29, 2) → (30, 2), screens 79 → 80,
  x 7680, y 512–767 (0.00; stage object 83 kind 4 b 4 and object 131 kind 0 b 47 near).
- Commander Yammark, section 1: (19, 7) → (20, 7), screens 89 → 90, x 5120, y 1792–2047 (0.06);
  stage object 36 (kind 4, b 8: a boss gate) is at x 5104, so the wall is probably the gate.

Other weak joins (0.19–0.56) are next to boss gates (kind 4 b 8: Turtloid (34, 2), Turtloid
alternate (4, 3)), camera-zone records (kind 3) or enemies, or are walls that end at a screen edge.
The full list with the objects near each join is printed by the probe in this folder (see the
report).

## 4. Collision values from the code

All tile readers return byte 0 of the tile definition (`TileCollisionAt` `0x80030234`, `TileAt`
`0x8003015C`, `TileAbove` `0x8002FD3C`, `TileBelow` `0x80030068`, and `0x8002FE30`,
`0x8002FF70`, `0x80030304`). Each one returns 56 for 59, 0 for an empty layout cell or y < 0, and
59 (so 56) for x < 0. So 59 is solid like 56 in every test, and the table entries for 59 are never
used.

The tests that use the value:

| test | address | solid values |
|---|---|---|
| block solid (`IsSolidValue`, corner tests of `CornerCheck` and `0x8002E974`, `TerrainPointCheck`) | `0x80030D80` (table `0x80010F38`), `0x8002EE10`/`0x8002F06C` (8 tables `0x800104B0`…`0x80010B78`), `0x8002E974` (4 tables `0x800100D0`…`0x800103B8`) | 3, 4, 5, 6, 10, 11, 16, 56, 57, 58, 60, 62, 63 |
| row test, ceiling (mode 0) | `TileSolid` `0x80030AE8` (table `0x80010E78`), from `SolidRow` `0x800309F8` (top edge, sets `obj+0x70` bit 4) | 56, 57, 58, 60, 62, 63 |
| row test, floor (mode 1) | `TileSolid` (table `0x80010E98`, value − 33) (bottom edge, bit 8) | 33, 34, 56, 57, 58, 60, 62, 63 |
| side wall | `SideWallTest` `0x80030C58` (table `0x80010F18`), from `SolidColumn` `0x80030B80` (bits 1, 2) | 56, 57, 58, 60, 62, 63 |
| floor height (slopes) | `FloorHeight` `0x8002F9DC` (table `0x80010C70`, 64 entries) | slopes 1–12, 17–28; 16 (look one tile up); full floor 33, 34, 56, 57, 58, 60, 62, 63 |
| hanging bar height | `BarHeight` `0x800397F8` | 37–45, 47 |

62 and 63 in every solid test: solid, and `obj+0x79` = 1 (death) unless `mega+0xE7` (form,
`0x80097187` for the player) is 2 (Shadow Armor; then 62, 63 act as 56).

Slope geometry (`FloorHeight`; `x` = x & 15; the floor is at this y inside the tile, 0 = top):

| values | slope | piece offset | floor y in the tile |
|---|---|---|---|
| 1, 2, 3, 4 | 1 : 4, up to the right, 4 tiles | 0, 16, 32, 48 | 15 − (x + offset) / 4 |
| 5, 6, 7, 8 | 1 : 4, down to the right, 4 tiles | 0, 16, 32, 48 | (x + offset) / 4 |
| 9, 10 | 1 : 2, up to the right, 2 tiles | 0, 16 | 15 − (x + offset) / 2 |
| 11, 12 | 1 : 2, down to the right, 2 tiles | 0, 16 | (x + offset) / 2 |
| 17–24, 25–28 | the same as 1–8 and 9–12 (value & 15) | | |

The upper half of each slope (3, 4, 5, 6, 10, 11) is solid in the block test; the lower half
(1, 2, 7, 8, 9, 12) is not. 17–28 are not solid in the block test.

Slippery slopes: the stage logic in overlays 04, 07, 10, 14, 22 and 26 (`SheldonFloorMove`
`0x800F0BB0` in overlay 10, `FloorValueLogic` `0x800F0530` in overlay 07; table `0x800E9C34` in
overlay 10) reads the value under the player: 17–20 and 25–26 move the player x by up to −3.5 px
per update (down the slope to the left), 21–24 and 27–28 by up to +3.5 px (down the slope to the
right); 16 reads the tile above; 60 has its own case. So 17–28 are slopes that slide the player
when that stage logic is in the stage.

Bars (`BarHeight`): 37–40 are 1 : 4 bars up to the right, 41–44 1 : 4 bars down to the right (the
same formula as the slopes), 45 a flat bar (y 0), 47 "the bar is in the tile above" (if that tile
is 0 or 56, the tile below). The hang test (`0x8003864C` and others) accepts 37–47 (value − 37 <
11). 46 is the rope (`0x80022A68`).

Disc counts (cells in the screen sets of all map members): 61 is in no member, so it is not used;
62 is only in member 94 (13 cells); 35 only in 101 (380 cells; next to 36, the water surface;
no code test found yet); 30 only in member 73 (no stage).

## 5. Proposed `catalog/collision.json`

```json
{
  "schema_version": 1,
  "note": "low 6 bits of the tile info word (byte 0 of the tile definition); from the code of SLUS_013.95 (2026-10-22) unless marked",
  "values": {
    "0":  {"name": "empty", "meaning": "no collision", "target": "none"},
    "1":  {"name": "slope_1x4_up_1", "meaning": "1:4 slope up to the right, lowest piece; floor y = 15 - x/4", "target": "slope"},
    "2":  {"name": "slope_1x4_up_2", "meaning": "1:4 slope up to the right, piece 2; floor y = 15 - (x+16)/4", "target": "slope"},
    "3":  {"name": "slope_1x4_up_3", "meaning": "1:4 slope up to the right, piece 3; floor y = 15 - (x+32)/4; solid in the block test", "target": "slope"},
    "4":  {"name": "slope_1x4_up_4", "meaning": "1:4 slope up to the right, top piece; floor y = 15 - (x+48)/4; solid in the block test", "target": "slope"},
    "5":  {"name": "slope_1x4_down_1", "meaning": "1:4 slope down to the right, top piece; floor y = x/4; solid in the block test", "target": "slope"},
    "6":  {"name": "slope_1x4_down_2", "meaning": "1:4 slope down to the right, piece 2; floor y = (x+16)/4; solid in the block test", "target": "slope"},
    "7":  {"name": "slope_1x4_down_3", "meaning": "1:4 slope down to the right, piece 3; floor y = (x+32)/4", "target": "slope"},
    "8":  {"name": "slope_1x4_down_4", "meaning": "1:4 slope down to the right, lowest piece; floor y = (x+48)/4", "target": "slope"},
    "9":  {"name": "slope_1x2_up_1", "meaning": "1:2 slope up to the right, lower piece; floor y = 15 - x/2", "target": "slope"},
    "10": {"name": "slope_1x2_up_2", "meaning": "1:2 slope up to the right, upper piece; floor y = 15 - (x+16)/2; solid in the block test", "target": "slope"},
    "11": {"name": "slope_1x2_down_1", "meaning": "1:2 slope down to the right, upper piece; floor y = x/2; solid in the block test", "target": "slope"},
    "12": {"name": "slope_1x2_down_2", "meaning": "1:2 slope down to the right, lower piece; floor y = (x+16)/2", "target": "slope"},
    "16": {"name": "slope_fill", "meaning": "solid block under a slope; the floor code reads the tile 16 px above", "target": "block"},
    "17-24": {"name": "slide_slope_1x4", "meaning": "as 1-8; with the slide stage logic the player moves down the slope up to 3.5 px per update", "target": "slope (slippery)"},
    "25-28": {"name": "slide_slope_1x2", "meaning": "as 9-12; slides like 17-24", "target": "slope (slippery)"},
    "30": {"name": "unknown_30", "meaning": "only in member 73 (no stage)", "target": "none"},
    "32": {"name": "ladder", "meaning": "TryLadder, TileAt = 32", "target": "ladder"},
    "33": {"name": "ladder_top", "meaning": "solid from above only (floor tests); TileBelow = 33 starts the climb down", "target": "ladder top (one-way platform)"},
    "34": {"name": "one_way_floor", "meaning": "solid from above only, like 33 but no ladder; next to the ladder tops", "target": "one-way platform"},
    "35": {"name": "unknown_35", "meaning": "Rainy Turtloid only (380 cells), below the water surface 36; probably water [not confirmed]", "target": "water (to check)"},
    "36": {"name": "water_surface", "meaning": "splash when the feet reach it (0x8003D394)", "target": "water"},
    "37-40": {"name": "bar_1x4_up", "meaning": "hanging bar, 1:4 up to the right (pieces 1-4)", "target": "hang bar"},
    "41-44": {"name": "bar_1x4_down", "meaning": "hanging bar, 1:4 down to the right (pieces 1-4)", "target": "hang bar"},
    "45": {"name": "bar", "meaning": "flat hanging bar", "target": "hang bar"},
    "46": {"name": "rope", "meaning": "rope (0x80022A68)", "target": "rope"},
    "47": {"name": "bar_fill", "meaning": "hang tile; the bar code reads the tile above (or below when that is 0 or 56)", "target": "hang bar"},
    "56": {"name": "solid", "meaning": "solid on all sides; wall slide and kick", "target": "block"},
    "57": {"name": "solid_no_wall_kick", "meaning": "solid; no wall slide or kick (obj+0x4A = 0)", "target": "block (no wall kick)"},
    "58": {"name": "solid_belt", "meaning": "solid with wall slide and kick; the conveyor belt surface of stage 4 area 1 (BeltUpdate)", "target": "block / conveyor"},
    "59": {"name": "gate", "meaning": "every tile reader returns 56 for it; opened gates replace it with 0", "target": "gate"},
    "60": {"name": "solid_no_wall_kick_2", "meaning": "solid; no wall slide or kick; own case in the slide stage logic; under hidden floors and press blocks", "target": "block (no wall kick)"},
    "61": {"name": "unused", "meaning": "not solid in any test; in no map member", "target": "none"},
    "62": {"name": "spikes_2", "meaning": "as 63; only in the intro (13 cells)", "target": "spikes"},
    "63": {"name": "spikes", "meaning": "solid; obj+0x79 = 1 (death) unless form 2", "target": "spikes"}
  }
}
```

Open: values 13–15, 29, 31, 37–40 and 48–55 are in no map member (the bar pieces 37–40 are
named from the code only); whether a value means the same in every
stage depends on the stage logic (17–28 slide only where the slide logic is loaded).
