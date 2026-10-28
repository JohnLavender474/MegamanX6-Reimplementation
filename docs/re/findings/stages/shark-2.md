# Stage: Metal Shark Player, section 2 (stage 4, area 1, overlay 07)

Section 2 of Metal Shark Player's stage is stage 4, area 1 [certain: `game.stage` = 4, `game.area` =
1 in all six states]. Section 1 (area 0, overlay 06) is in [`shark.md`](shark.md). Overlay 07 holds
the code and the record lists of this area [certain: the record lists and the type tables are in
overlay 07 in all six states].

Save states (in `~/mmx-data/x6/dumps/`; X in the Falcon Armor, form 1; difficulty 1; `boss_choice`
1; `nightmare_effect` 0):

| state | checkpoint | X at (x, y) | camera (x, y) | limits x / y | what |
|---|---|---|---|---|---|
| `shark-2_section-2` | 0 | 67, 395 | 0, 224 | 0–4672 / 224 | the start; X has just spawned on the belt |
| `shark-3_press-low` | 0 | 1044, 347 | 885, 226 | 885–4672 / 224 | the Nightmare Press at its lowest point, right above X; the screen shakes |
| `shark-4_mini-boss` | 1 | 138, 1115 | 10, 944 | 10–10 / 944 | the mini-boss room; Nightmare Pressure (main 16, 7 parts) has just spawned |
| `shark-5_after-mini-boss` | 2 | 5604, 395 | 5456, 224 | 5456–5456 / 224 | after the mini boss: X in front of boss gate `id` 21 |
| `shark-6_before-boss` | 2 | 5935, 395 | 5760, 224 | 5760–5760 / 224 | the room between the boss gates `id` 21 and 22 |
| `shark-7_boss` | 2 | 6124, 395 | 6064, 224 | 6064–6096 / 224 | the boss room; Metal Shark Player (main 56) has just spawned |

The limits are the current and the target limits of layer 0 (they are the same in all six states;
ease speed 4).

## Map

- Stage member 99, a layout of 26 × 5 screens (6656 × 1280 px). Layout at overlay 07 offset
  `0xE1F0` (RAM `0x800F7A50`), 0 differing cells. Records at offset `0xE378` (`0x800F7BD8`; list entry
  4, area 1): 79 records [certain].
- Two catalog entries (both member 99, the same layout and records):
  - "Metal Shark Player, section 2" (from `shark-2_section-2`, texture entry `0x10000`):
    `~/mmx-data/x6/stages/m099-metal-shark-player-section-2/`;
  - "Metal Shark Player, section 2, second texture" (from `shark-5_after-mini-boss`, `"texture":
    22`): `~/mmx-data/x6/stages/m099-metal-shark-player-section-2-second-texture/`.
- Screens and texture pages: `~/mmx-data/x6/tiles/m099/` and `~/mmx-data/x6/collision/m099/` (made
  again: the same bytes as before).

### Texture blocks

Member 99 has a texture entry `0x16` too, as members 98 and 102 [certain for the entry list]. Entry
`0x10000` is in VRAM in `shark-2` and `shark-3`; entry `0x16` is in VRAM in `shark-4` to `shark-7`
[certain: the `learn` fit test of each state]. The swap request comes from the warp to the
mini-boss room: `LowerPathWarp` writes `stage_texture_swap` = 1 (`0x800EDBEC`; see "The
transitions") [certain]. In `shark-4` to `shark-7` the byte is 2 (done).

| part of the map | entry `0x10000` | entry `0x16` |
|---|---|---|
| upper path, x 0–5439 (the belt, the junk, the ceiling at y 208–239) | correct | garbled (the junk piles and the ceiling art) |
| mini-boss room (the lower path, y 944–1199) and the art at x 0–319, y 768–943 | garbled | correct |
| boss part, x 5440–6431 (the room before the gates, the corridor, the boss room) | garbled | correct |

So the stage draws the upper path with entry `0x10000` and everything after the warp with entry
`0x16`. Use each drawing for its part.

### `learn` fit test: failures and fixes

At first all four states of the first batch failed:

1. `shark-2`, `shark-4`, `shark-5` (and later `shark-6`, `shark-7`): "0 stage members have their tile
   info in RAM". Cause: the belt animation. `BeltUpdate` (stage logic 9;
   [`docs/enemies/conveyor-belt.md`](../enemies/conveyor-belt.md)) changes byte 2 (the texture cell,
   `cx` and `cy`) of 32 tile info words (tiles 56–59 and 208–235) every 5 updates. So the RAM copy of
   the tile info of member 99 differs from the disc in 32 bytes. In `shark-3` the animation is at its
   first frame and all bytes agree. In `shark-5` to `shark-7` the bytes are the same in each state:
   the animation stops at checkpoint 2. **Fix** (`mmxdemake/ramlayout.py`): `find_member` also accepts
   a RAM copy that differs from the disc only in byte 2 of each word.
2. `shark-3`: "no camera for any layout layer". Cause: the screen shakes when the press reaches the
   floor. The two draw buffers in RAM have camera y 224 and 226, so the two best layer-0 cameras got
   the same vote count (268 each) and failed the margin test. **Fix** (`mmxdemake/palettes.py`):
   `cameras` does not count a rival camera that is 8 px or nearer to the best camera. Only results
   that failed before can change.

After the fixes all six states pass: member 99, overlay 7 at `0xE1F0`, edges 0.976 (horizontal) /
0.896 (vertical), 0 differing cells, 79 records, palettes at (0, 486) from 149, 268, 126, 328, 336
and 337 tiles. Entry `0x16` is found in `shark-4` to `shark-7`. The layout bytes are the same in all
six states.

### The parts of the map

Layer 0 uses all 26 columns of rows 0–1 (the upper path), row 2 columns 0, 2–4, 7–8 (small pieces),
and rows 3–4 (the lower path) [certain for the tiles; the names are from the art and the code]:

| x (px) | y (px) | part |
|---|---|---|
| 0–5439 | 208–431 | the upper path (checkpoint 0): ceiling (value 57) at y 208–239, the belt floor (value 58) at y 416; junk piles of value 56 at y 336–415 with spikes (63) in four places |
| 5440–5759 | 208–431 | the room before boss gate `id` 21 (dark machines); floor value 56 |
| 5760–5791 | 352–415 | boss gate `id` 21 (value 59) |
| 5792–6047 | 240–431 | the corridor between the gates (purple) |
| 6048–6079 | 352–415 | boss gate `id` 22 (value 59) |
| 6080–6431 | 208–431 | the boss room (Metal Shark Player) |
| 0–6431 | 944–1199 | the lower path (checkpoint 1): the mini-boss room; ceiling 57 at y 944, belt floor 58 at y 1136 |
| 0–4095 | 544–879 | small pieces of belt and junk: the frame store of the belt art (`docs/enemies/conveyor-belt.md`); not reachable |

Collision values in layer 0 (cells): 0 (28 313), 56 (3 358), 57 (743), 58 (732), 60 (90), 63 (28),
59 (16).

- **Value 58** is the belt surface: the upper path floor (x 0–5439, with gaps) and the whole lower
  path floor (x 0–6399). `BeltUpdate` moves the player 1 px to the left per update on it.
- **Value 63** (spikes) at x 1664–1727 (y 384), 2000–2015 (y 352–415), 2400–2463 (y 368) and
  3712–3839 (y 384) on the upper path, and x 2000–2127 at y 544 (frame store).
- **Value 60** only in the frame store (10 blocks at y 800–879, x 864–4095).
- The camera y is 224 on the upper path and 944 on the lower path, so the views are y 224–463 and
  y 944–1183.
- The lower path continues the upper path: the art at x 0–319, y 944–1199 is the same as the art at x
  4672–4991, y 224–479. The warp at camera x 4672 uses this (see "The transitions").

## Objects

79 records in the first list (`enemy_set_tbl`, `0x800F7BD8`) and 48 in the second
(`fixed_set_tbl`, `0x800F7E58`), by class and type [certain for the counts and the places]:

| class, type | records (kind 0 / 1 / 2) | object |
|---|---|---|
| main 15 | 4 / 3 / 1 | Nightmare Virus (`docs/enemies/nightmare-virus.md`): `id` 0 ×2, 2 ×2, 32 ×1, 34 ×3; records 12–19 |
| main 16 | 1 | Nightmare Pressure, the mini boss (head, `id` 0) at (160, 1064), record 75: [`docs/enemies/nightmare-pressure.md`](../enemies/nightmare-pressure.md) |
| main 27 | 10 | Mettaur D2 (`docs/enemies/mettaur-d2.md`), records 20–29: `id` 0, except record 24 (`id` 1); record 29 is at (0, 0) |
| main 35 | 44 | Nightmare Freeze, the Nightmare Effect 2 object (below; [`docs/enemies/nightmare-freeze.md`](../enemies/nightmare-freeze.md)), all `id` 0, records 31–74, x 880–4080, in rows 32 px apart at y 336–400 |
| main 47 | 1 | item, `id` 3, at (3984, 408), record 11 |
| main 56 | 1 | Metal Shark Player, `id` 0 at (6231, 350), record 78: [`docs/enemies/metal-shark-player.md`](../enemies/metal-shark-player.md) |
| stage object 4 | 3 | Rescue Reploids, `id` 60, 61, 62 at x 1488, 2608, 4016 (y 400), records 8–10 |
| stage object 8 | 2 | boss gate `id` 21 at (5776, 384), `id` 22 at (6064, 384), records 76, 77 (`docs/enemies/boss-gate.md`) |
| stage object 11 | 4 | junk block ([`docs/enemies/junk-block.md`](../enemies/junk-block.md)), `id` 0 at y 344, x 3224, 3320, 3432, 3480, records 3–6 |
| stage object 15 | 1 | `id` 1 at (3272, 344), record 30: a Nightmare Cube, the Nightmare Effect 5 object (below; [`docs/enemies/nightmare-cube.md`](../enemies/nightmare-cube.md)) |
| stage logic 6 | 1 | checkpoint marker `id` 2 at (5920, 384), record 7 |
| stage logic 14 | 3 | background setting `id` 133, 134, 135 at x 16, 1872, 5808 (y 256), records 0–2 |

The second list [certain for the records]:

| records | kind, type | `id` | x, y | object |
|---|---|---|---|---|
| 0 | stage logic 9 | 0 | 0, 0 | `BeltUpdate`, the belt: [`docs/enemies/conveyor-belt.md`](../enemies/conveyor-belt.md) |
| 1 | kind-7 type 3 | 0 | 0, 0 | `AutoScroll`, the forced scroll of the mini-boss room (same sheet) |
| 2 | stage logic 10 | 0 | 0, 0 | `LowerPathWarp`, the warp to the mini-boss room (see "The transitions") |
| 3–5 | stage object 11 | 128 | x 1320, 1368, 1416, y 392 | junk blocks on the belt |
| 6 | kind-7 type 2 | 16 | 0, 0 | the Nightmare Press controller (`PressUpdate`): [`docs/enemies/nightmare-press.md`](../enemies/nightmare-press.md) |
| 7–25 | stage object 9 | 0 | x −120 to 4200 every 240 px, y 176 | press blocks (the invisible solid blocks that follow the press; same sheet) |
| 26–46 | stage object 9 | 66–70, 99, 112 | x 288–4128, y 256–296 | more press blocks, at other heights |
| 47 | stage logic 7 | 0 | 0, 0 | overlay 07 `0x800F0530`: the slide on collision 60 (the ice of Nightmare Freeze) and on slopes 17–28; the same code as overlays 04 and 10 [certain]; `docs/engine.md`, "Slippery floors and slopes (stage logic 7)" |

The stage object 9 records have byte `a` = `0x30` (the other records have 0). In `shark-3` all 40
are 80 px lower than their records (for example 176 → 256), the same distance that the layer 1
camera moved (224 → 144): they follow the press [certain for the positions].

Live objects [certain]:

| state | live (besides the belt, `AutoScroll`, logic 7, 14 and 27) |
|---|---|
| `shark-2`, `shark-3` | junk blocks records 3–5 (x 1320, 1368, 1416), the 40 press blocks, the press controller (kind-7 slot 1), `LowerPathWarp` |
| `shark-4` | main 16 (7 objects: head `var` 0, pods 16–18, turrets 32–34), stage 10 (5 objects: `var` 0, 1, 16, 17, 32), junk blocks records 3–4 (now at x 1176, 1224: the belt moved them 144 px), logic 14 `id` 134 |
| `shark-5` | boss gate `id` 21 (`display` 1, state 1), logic 14 `id` 135; the belt, `AutoScroll`, the press and the press blocks are gone |
| `shark-6` | boss gates `id` 21 (state 2) and 22 (state 1) |
| `shark-7` | Metal Shark Player (main slot 0, at (6304, 300), state 1/5/3), visual type 22 (at 6255, 310), both gates in state 2 |

Where the code is (entries from the type tables in `shark-4`; tables: main `0x800F7FEC`, stage
`0x800F835C`, stage logic `0x800F82A8`):

| class, type | entry | in |
|---|---|---|
| main 16, 27, 35, 56 | `0x800EB4C8`, `0x800F1958`, `0x800F04A4`, `0x800F4230` | overlay 07 |
| main 15, 47 | `0x80041AB8`, `0x8004E5D8` | main program |
| stage 9, 10, 11, 15 | `0x800EE3F8`, `0x800ED494`, `0x800EEA90`, `0x800F00B4` | overlay 07 |
| stage 4, 8 | `0x8004F834`, `0x80050DFC` | main program |
| logic 7, 9, 10 | `0x800F0530`, `0x800ED998`, `0x800EDD00` | overlay 07 |
| logic 6, 14, 26, 27 | `0x80052D30`, `0x8005335C`, `0x80053CDC`, `0x800541D4` | main program |
| kind-7 2, 3 | `0x80046E14`, `0x800E9C98` | main program, overlay 07 |

### The Nightmare Effect objects

The two `nightmare_effect` reads of overlay 07 (`docs/engine.md`, "Nightmare Effect") are these types
[certain for the code]:

- **Value 2: main 35, Nightmare Freeze** (44 records; official name from the Fandom "Nightmare"
  page). Its set-up `0x800F01BC` (state table `0x800F8DE0`) deletes the object (`DeleteObject3`)
  unless the value is 2, else HP 2. Each block changes the two map cells under it (collision 56) to
  ice cells (collision 60, copied from 480 px lower in the map); a hit of Ice Burst (ids 2, 11, 52) or
  id 33 breaks it and puts the 56 cells back. Its sprites (`spriteexport`, found by search) are a
  blue ice block of 32 × 62 and two small pieces. The rows of records are on the tops of the junk
  piles of the upper path (y 336–400). Only the stage 2 pick (Blizzard Wolfang) can give stage 4
  the value 2 (`shark.md`, "Nightmare Effect"). The sheet:
  [`docs/enemies/nightmare-freeze.md`](../enemies/nightmare-freeze.md).
- **Value 5: stage object 15, Nightmare Cube** (1 record, `id` 1, at (3272, 344), among the junk
  blocks). Its set-up `0x800EEC50` (state table `0x800F8D94`) deletes the object unless the value is
  5. It is the same object as stage object 15 of area 0: the code of the two overlays has the same
  1 448 instructions [certain]. The sheet: [`docs/enemies/nightmare-cube.md`](../enemies/nightmare-cube.md).

## The transitions

The six states show two jumps in position: from the upper path (camera x 4672) to the mini-boss room
(x about 138, y 1115), and from the mini-boss room to the room before boss gate `id` 21 (x 5604, y
395). Neither is an area change or a teleporter: `game.stage` and `game.area` stay 4 and 1, the
exit request (`game+0x0F`) is 0, and the layout, records and overlay are the same [certain]. Both are
**warps inside the area**: overlay 07 code writes the camera and the player position directly and
sets a new checkpoint.

### Warp 1: the upper path to the mini-boss room (`LowerPathWarp`, stage logic 10)

Record 2 of the second list. Entry `0x800EDD00`, state table `0x800F8C28` [certain for the code]:

| state | function | does |
|---|---|---|
| 0 | `0x800EDD3C` | checkpoint ≠ 0: delete (`DeleteObject3`). Else state 1 |
| 1 | `0x800EDD7C` | waits until the layer 0 camera x is ≥ 4672 (the camera max x of the upper path). Then `mega.x` = 4832 (the middle of the view), `PlayerEvent(20, flip)` (X holds), state 2 |
| 2 | `0x800EDBB8` | waits until the player step is 20, then does the warp once (below) and deletes itself |

The warp (`0x800EDBB8`):

- `stage_texture_swap` = 1 (`StageTextureSwap` then loads entry `0x16`).
- Layer 0 camera: x = x − 4672 (so 0), y = 944; min x 0, max x 6080, min y = max y = 944 (current and
  target); redraw request.
- Layer 1: mode 5, x 0, y 960. Layer 2: x = layer 0 x / 2, y 944. Redraw requests for layers 1 and 2.
- `game.checkpoint` = 1.
- The player: x = x − old camera x + new camera x, y = y − old camera y + 944 (the same place on the
  screen, so x 160, y 1115). The code changes both position fields of the player (`+0x0A`/`+0x0E`
  and `+0x1A`/`+0x1E`).
- `SpawnInRect(x − 48, x + 368, 896, 1232, 0)`: the records in that rectangle spawn at once; that is
  record 75, Nightmare Pressure at (160, 1064).

The art of the lower path at x 0–319 is the same as the upper path at x 4672–4991, so the player sees
no jump. Then `AutoScroll` (kind-7 type 3) scrolls the camera to the right at 0.5 px per update
(`shark-4`: camera x 10, min x = max x = 10, target 6080) [`docs/enemies/conveyor-belt.md`].

### Warp 2: the mini-boss room to the boss part (the death of Nightmare Pressure)

The death of the head (main 16, state 2, entry `0x800EB540`, sub-state table `0x800F86CC`;
[`docs/enemies/nightmare-pressure.md`](../enemies/nightmare-pressure.md)) [certain for the code]:

| sub-state | function | does |
|---|---|---|
| 0 | `0x800EB5C0` | `game.checkpoint` = 2, `game+0x26` = 1 (this ends `AutoScroll`), timer 127, `PlayerEvent(20, flip)` |
| 1 | `0x800EA1CC` | the head flashes faster and faster; after 127 updates `GetSysObject` makes **stage logic 26** (`var` 1) at x = layer 0 x + layer 1 x + 160, y = layer 0 y − layer 1 y + 912; its pointer goes to `+0x88` |
| 2 | `0x800EA2E0` | while the type 26 object has `+0x07` = 0: the head flashes. When `+0x07` ≠ 0: the warp, once (below) |

The warp (`0x800EA2E0`):

- Layer 0 camera x = 5456, y = 224; min x = max x = 5456, min y = max y = 224 (current and target).
  Layer 1 x 5456, y 224. Layer 2 x 2728, y 224. Redraw requests for the three layers.
- `SpawnInRect(5408, 5824, 176, 512, 0)`: boss gate `id` 21 (x 5776) and background setting `id` 135
  (x 5808) spawn at once (both are live in `shark-5`).
- The player: x = 5604, y = 395 (both position fields).

`BeltUpdate` deletes itself at checkpoint 2, so the belt stops ("conveyor belt is gone").

### The white screen

Stage logic 26 (entry `0x80053CDC`, state table `0x8007AD08`, main program) is the engine's boss death
explosion (`docs/engine.md`, "Shots and attached objects") [certain for the code]:

1. States 0–3: ray objects (type 4) at three angles, explosions (`SpawnExplosion`), `Rumble(10)`.
2. Then `0x80013AA4(25)`: **a fade to white**. It sets `white_fade_dir` (`0x80097425`) = −1 and the
   speed (`0x80090C8D`) = 25, and starts the task `0x80013788`.
3. The next state waits until `white_fade_dir` is 0 (the screen is fully white). Then it sets its
   **`+0x07` = 1** (the signal that warp 2 waits for), a timer of 40 updates (200 when the flag bit 7
   is set), `game+0x24` (the boss HP bar) = 0, `PlaySound(0, 5, 0)` and `0x800C4560` = 1.
4. After the timer: `0x80013A48(25)`: the fade back (`white_fade_dir` = +1).
5. When the fade ends: `game.player_lock` = 1 and the object deletes itself.

The task `0x80013788` draws one GPU command over the whole view each frame: command `0x62` (a
monochrome, semi-transparent rectangle) of 240 px height, with draw mode `0xE1000025`
(semi-transparency mode 1: add). Its colour is r = g = b = the fade level (`0x80090C8A`) shifted
right by 4. The task sets the level to 0 for a fade to white and to 255 for a fade back [certain for
the command words and the start values; the step and the end level are not read]. So the "white" is
an additive grey-to-white rectangle over the picture. The warp happens while
the screen is white, so the player does not see the camera jump. In `shark-5` the fade has ended
(`white_fade_dir` 0).

This is the same pattern as `FadeIn`/`FadeOut` (`0x800139A0`, `0x800139F4`, `fade_dir`
`0x80097424`), with white instead of black. Names proposed in
`~/mmx-data/x6/stages/shark-2-helpers-proposed.json`.

## Camera

No camera zone in area 1: the lists have no stage logic 0 record [certain]. The 4 entries of
`camera_zone_tbl` word 9 (`0x80079690`) are not used by a record. The limits come from the code:

| part | camera x | camera y | set by |
|---|---|---|---|
| upper path | 0 → 4672; min x = camera x (no scroll back) | 224 | the area start (limits x 0–4672, y 224 in `shark-2`); `BeltUpdate` (min x) |
| mini-boss room (lower path) | 0 → at most 6080, forced to the right at 0.5 px per update | 944 | `LowerPathWarp`; `AutoScroll` (min x = max x = camera x) |
| room before gate 21 | 5456 | 224 | the warp 2 code |
| corridor between the gates | 5760 | 224 | boss gate `id` 21: min x = max x = X − 16 |
| boss room | 6064–6096 | 224 | boss gate `id` 22: min x X, max x X + 32 |

The two gate rows are in `docs/enemies/boss-gate.md` ("The new camera limits"); the values in
`shark-6` and `shark-7` agree. Neither gate id has an open condition (both open always).

## Checkpoints

| checkpoint | set by | place |
|---|---|---|
| 0 | the area start | X at (67, 395), camera (0, 224) (`shark-2`, just after the spawn) |
| 1 | `LowerPathWarp` (`0x800EDC4C`) | the mini-boss room, X at (160, 1115) after the warp |
| 2 | the death of Nightmare Pressure (`0x800EB5D4`) | X at (5604, 395), camera (5456, 224) |
| (2) | checkpoint marker `id` 2 at x 5920 (stage logic 6) | `CheckpointMarker` deletes itself at once when the checkpoint is already 2, so it has no effect after the mini boss [likely: it is not live in `shark-5` to `shark-7`] |

`LowerPathWarp` deletes itself when the checkpoint is not 0, so a restart at checkpoint 1 or 2 does
not run it again. Where the game puts X and the camera on a restart at checkpoint 1 or 2 is not known.

## Sprite exports

`spriteexport` on the six states. Banks: 0 = 128 (Light capsule; only in `shark-2` to `shark-4`), 1 = 15,
2 = 141 (Rescue Reploid), 3 = 16, 4 = 138 (junk block), 5 = 147, 6 = 27, 7 = 35, 8–13 = 0 (no
texture), 14 and 15 = 56 (no texture until the boss is live) [certain]. Images:
`~/mmx-data/x6/sprites/banks/shark/<state>/`.

| export | ID | class, type | found | clean |
|---|---|---|---|---|
| `nightmare-virus` | `enemy-363dce0a1a` | main 15 | search | yes |
| `nightmare-virus-orb` | `enemy-a96f0b9d66` | main 15 | search | yes |
| `mettaur-d2` | `enemy-46d9e2249f` | main 27 | search | yes (`shark-2` to `shark-6`) |
| `enemy-74e4e39589` | `enemy-74e4e39589` | main 16, stage 10 | live in `shark-4`, else search | yes: Nightmare Pressure, 17 animations, cell 255 × 120 |
| `enemy-4c32312ebb` | `enemy-4c32312ebb` | main 35 | search | yes: the effect 2 block, 4 animations, cell 32 × 62 |
| `enemy-4185141b5a` | `enemy-4185141b5a` | stage 11 | live | yes: the junk block, 8 animations, cell 48 × 64 |
| `boss-gate` | `enemy-30419fd669` | stage 8 | live (`shark-5` to `shark-7`) | yes |
| `enemy-3f9602d9d5` | `enemy-3f9602d9d5` | main 56 | live (`shark-7`) | yes: Metal Shark Player, 41 animations, cell 240 × 131 |
| `enemy-65ec66c9da` | `enemy-65ec66c9da` | main 16 | search (`shark-7`) | **no** |
| `enemy-c5fe34fa82` | `enemy-c5fe34fa82` | stage 11 | live (`shark-7`) | **no** |

- In `shark-7` the texture of Metal Shark Player covers the cells of banks 4–7 (the tool skips types 27
  and 35 for this reason). The type 16 and stage 11 exports of that state are wrong (green garbage,
  other cell sizes), so they get new IDs. Do not name them; the clean exports of the same types are
  `enemy-74e4e39589` and `enemy-4185141b5a`.
- Names proposed (not set; `catalog/enemies.json` has another owner): `nightmare-pressure` for
  `enemy-74e4e39589` (official name of the mini boss), `metal-shark-player` for `enemy-3f9602d9d5`
  (official), `junk-block` for `enemy-4185141b5a` (descriptive, as in `shark.md`), and
  `nightmare-freeze` for `enemy-4c32312ebb` (main 35; official, Fandom "Nightmare" page).
- No export for the belt, the press, the press blocks, `AutoScroll` and `LowerPathWarp` (no sprites).

## Save states that are still needed

1. A restart at checkpoint 1 and at checkpoint 2 (after a death): the start places and the camera
   limits on a restart.
2. Just before and just after warp 1 (camera x about 4660, and the first updates in the mini-boss
   room): to confirm the seamless jump and the texture swap in play.
3. The white screen of warp 2 (during the fade) and just after it.
4. The boss fight later on (Metal Shark Player attacks; the boss agent has its own list) and the
   state after the boss (the soul and the stage exit).
5. A save with Nightmare Effect 2 in this stage (after a stage 2 pick) for main 35, and with value 5
   for stage object 15.
6. The way from area 0 to area 1 (the last part of section 1 and the first frame of area 1).

## Open questions

1. (answered) Stage logic 7: the ice slide (`docs/engine.md`, "Slippery floors and slopes").
2. The meaning of `0x800C4560` (written 1 by the boss explosion) and why the explosion sets
   `game.player_lock` at its end.
3. (answered) Main 35 is Nightmare Freeze (only in this area); stage object 15 is the Nightmare
   Cube, the same code as in area 0. Whether the cubes of stage 7 use the same code is open
   (`docs/enemies/nightmare-cube.md`).
4. The 4 unused entries of `camera_zone_tbl` word 9.
