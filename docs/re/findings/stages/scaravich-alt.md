# Stage: Ground Scaravich, hidden area (stage 21, area 0, overlay 29)

The teleporter crystal in section 7 of Ground Scaravich's stage (record 15, main 28, `id` 1, at
(3064, 1488); [`scaravich.md`](scaravich.md)) sets `game.exit_req` = `0x41`. `StageExit` changes
stage 5 to stage 21, area 0 [certain: all five save states below have `game.stage` = 21 and
`game.area` = 0]. It is one of the 8 hidden areas (stages 19–22; `docs/engine.md`, "Boss
overlay"). The other hidden areas that have pages are stage 20 area 1 (Metal Shark Player,
[`shark-alt.md`](shark-alt.md)), stage 21 area 1 (Turtloid,
[`turtloid.md`](turtloid.md), "Alternate section"), stage 22 area 0
([`sheldon-alt.md`](sheldon-alt.md)) and stage 22 area 1 ([`mijinion-alt.md`](mijinion-alt.md),
the only one with camera zones). This area is one room with a maze, a short corridor between
two boss gates, and the boss room. With `boss_choice` 1 the boss is High Max.

## Save states

All in `~/mmx-data/x6/dumps/`. Zero is the player (`game+0x38` = 1, form 5); difficulty 1;
`boss_choice` 1; `nightmare_effect` 4. The order below is the game order, from `frame_count`
(`0x80097420`). It is not the order of the file names [certain].

| state | `frame_count` | checkpoint | Zero at (x, y), HP | camera (x, y) | lives | what |
|---|---|---|---|---|---|---|
| `scaravich-5_alt-arrival` | 3 124 364 | 0 | 128, 298, 32 | 0, 240 | 5 | the start: Zero stands on the start platform after the beam-in, in the talk box (`mega.hint` = 4). No main object is live |
| `scaravich-6_alt-ledge-dup` | 3 124 878 | 0 | 53, 426, 32 | 0, 255 | 5 | Zero on the left ledge (floor y 448). Nightmare Virus (main 15, `var` 0) live at (272, 528) |
| `scaravich-7_alt-ledge` | 3 125 037 | 0 | 53, 426, 32 | 0, 255 | 5 | the same place, 159 frames after `scaravich-6` (see "The duplicate state") |
| `scaravich-8_alt-gates` | 3 134 147 | 1 | 812, 394, 24 | 608, 240 | 3 | in the corridor between the gates: gate 17 is closed for good (state 2), gate 35 waits (state 1, step 0) |
| `scaravich-9_high-max` | 3 135 291 | 1 | 953, 394, 24 | 912, 224 | 3 | the boss room: High Max (main 55, `var` 1, state 1/2/11, HP 48) at (1200, 321); both gates closed (state 2) |

- `scaravich-5` is the first state, not a "climb" state: Zero is at the checkpoint 0 point (128,
  272) dropped to the platform top (y 320 − 22) and faces left, as the checkpoint record says
  [certain].
- Between `scaravich-7` and `scaravich-8` the lives go from 5 to 3. So Zero lost two lives in the
  area [certain for the count; the place is not known].
- The stage time (`game.time`, `0x800CCF60`) and the kill count continue from stage 5: 11 965 in
  `scaravich-4_low-path` (stage 5), 24 070 in `scaravich-5`. The move to the hidden area does not
  clear them [likely].
- Rescue Reploid 79 goes from status 0 (`scaravich-7`) to 2, saved (`scaravich-8`) [certain].

### The duplicate state

`scaravich-6` and `scaravich-7` are the same game moment, 159 frames apart. `scaravich-6` is the
earlier one [certain]:

- Zero: the same x, y, HP, step and facing. The camera, the limits and all live objects (the same
  types, `var`, slots and positions) are the same. The record flags of the area list are the same.
- Only 1 681 bytes of 2 MB differ: `game.time` (+159), `game+0xCC` (the play time, +159), the RNG
  state, the virus animation (`step2` 1 → 2), the draw buffers, the sound and the stack.

So `scaravich-6` gives no new fact. Keep `scaravich-7` and drop `scaravich-6` [certain].

## Overlay, member and lists

- Overlay 29 at `0x800E9860`, `0x2F54` bytes. In all five states the RAM is a byte match to the
  disc, except the record flags and kinds of the area list (21 bytes in `scaravich-9`) [certain].
- Stage member 119, catalog entry "Ground Scaravich, hidden area". The layout is at overlay offset
  `0x2404` (`0x800EBC64`): 6 × 12 screens, but only columns 0–4 and rows 0–3 of layer 0 are used
  (x 0–1279, y 0–1023). Layer 1 uses 13 screens; layer 2 is empty [certain].
- `enemy_set_tbl` (`0x8007349C`) stage 21 area 0: `0x800EBD3C` (overlay offset `0x24DC`), 28
  records and the end record. `fixed_set_tbl` (`0x80073554`): `0x800EBE24`, 2 records [certain].
- No camera zone: the `camera_zone_tbl` word of stage 21 area 0 (`0x80079D70` + 4 × 42) is 0, and
  the list has no stage logic 0 record [certain].
- `bg_mode_tbl` (`0x800720AC` + 21 × 4): layers 1 and 2 are in mode 2 (layer 0 position + offsets
  0, 0) [certain]. So the three layers scroll together.
- The Nightmare Effect: `nightmare_effect` (`0x800CD30A`) is 4 in all states, the value of stage 5
  (`StageStartReset` writes it only for stage < 19 and area 0) [certain]. With 4, the grey blocks
  are live; with 6 the rain would be live instead (see "Objects").
- Text bank: member 236 (`text_members_alt`, `0x8006DC30`, index ((21 − 19) × 2 + 0) × 2 + 1 for
  Zero; X has member 235) [certain for the index; the X bank was not read].

## Map

Collision of layer 0, one character per 16 × 16 px cell (`#` 56 solid, `=` 57, `|` 59 boss gate,
`^` 63 spikes). Markers: `Z` start (checkpoint 0), `T` talk zone record, `v` Nightmare Virus, `B`
grey block (record point), `R` Rescue Reploid, `D` boss gate, `C` checkpoint marker, `S`
`BossSpawner`. Column 0 is x 0; column 40 is x 640; column 80 is x 1280.

```
 208                                       ###################======================
 224                                       ###################=                    =
 240 ##########################################################                    ##
 256 ##              T ##                  ####################                    ##
 272 ##      Z         ##              v   ####################                    ##
 288 ##                ##                  ####################                    ##
 304 ##                ##     v            ####################                    ##
 320 ##    ##############        ######    ####################                    ##
 336 ##    ##############        ######    ####################                    ##
 352 ##        ##      ##        ######    ||                ||                    ##
 368 ##      B ##      ##        ######    ||                ||    S               ##
 384 ##        ##                ######    |DC               |D                    ##
 400 ##        ##             v  ###### R  ||                ||                    ##
 416 ##        ##                ####################################################
 432 ##        ##                ####################################################
 448 ######    ####    ##############################################################
 464 ######    ####    ##############################################################
 480 ##        ##      ####                ##
 496 ##        ##      ####    v   B       ##
 512 ##        ##      ####                ##
 528 ##        ##     v####  v   v       v ##
 544 ##        ##      ####                ##
 560 ##        ##      ####    v           ##
 576 ##    ######    ########    ######    ##
 592 ##    ######    ########    ######  v ##
 608 ##        ##                ##        ##
 624 ##        ##  B             ##        ##
 640 ##      v ##                ##        ##
 656 ##        ##     v          ##        ##
 672 ######    ##                ##    ######
 688 ######    ##                ##    ######
 704 ######    ##        B       ##    ######
 720 ######    ##                ##    ######
 736 ##        ##      ####      ##        ##
 752 ##        ##^^^^^^####^^^^^^##        ##
 768 ##        ####################        ##
 784 ##        ####################        ##
 800 ##    ######   v            ######    ##
 816 ##    ######                ######    ##
 832 ##           v   v                    ##
 848 ##                                    ##
 864 ##             v  ####     v    v     ##
 880 ##                ####                ##
 896 ########################################
```

(Rows 912–959 are solid floor. Records 9, 13, 16 overlap other markers.)

Collision values in layer 0 (cells, all 30 screens of the member): 0 (6 538), 56 (1 090), 57 (24),
59 (16), 63 (12) [certain]. Value 57 is only the boss-room ceiling (y 208, x 912–1263, and one cell
at each end at y 224). Value 59 is the two boss gates (2 × 4 cells at x 608 and 896, y 352–415).
Value 63 is two spike rows at y 752: x 192–287 and x 352–447.

### Rooms [certain for the tiles]

| part | x (px) | y (px) | what |
|---|---|---|---|
| main room | 32–607 | 256–895 | a tall maze with walls of 32 px. The start platform is at x 96–319, y 320. A shaft on the left (x 32–159), a shaft on the right (x 480–607), a floor corridor at the bottom (y 832–895), a spike room in the middle (x 192–447, y 608–751) with a pillar at x 288–351 between two spike rows |
| corridor | 640–895 | 352–415 | between boss gate 17 (x 608–639) and boss gate 35 (x 896–927); solid above (y 240–351) and below |
| boss room | 928–1247 | 224–415 | one screen wide (320 px) and 192 px high; ceiling 57 at y 208, floor at y 416, right wall at x 1248–1279 |

The camera limits of the main room (x 0–320, y 240–720) give a view of x 0–639, y 240–959: the
full room with its walls.

### The path to the boss room [likely]

From the shortest path through the open cells (the collision only, not seen in play; wall jumps
let the player climb any shaft):

1. From the start platform, go left and drop down the left shaft (x 32–159). Grey block 4 (x 128)
   moves up and down in this shaft between y 376 and y 552.
2. At the bottom (y 832–895), go right to the right shaft (x 480–607).
3. Climb the right shaft to y 480–575. Go left in the middle part (grey block 5 at x 480 moves
   between y 504 and 552).
4. Cross the top of the spike room (y 608). Grey block 7 (x 224) falls into the left spike row and
   gives a floor there; grey block 6 (x 320) stands on the pillar.
5. Climb at x 224–287 to y 384–447, go right, then climb at x 320–447 to y 256–319.
6. Go right over the block at x 448–543 and drop to the floor at x 544–607, y 416, in front of boss
   gate 17 (x 624).

The Rescue Reploid (record 26, x 560, y 400) stands on that last floor, in front of gate 17. The
snatcher virus (record 10, `id` 33) is above it at (552, 280).

## Objects

28 records (list `0x800EBD3C`). `kind` is the low digit (the disc value); the RAM value can have a
high digit (the respawn counter). The class comes from `kind`: 0–2 main, 3 stage logic, 4 stage
object [certain for every field: read from RAM and from the disc].

| # | kind | b | `id` | x | y | object |
|---|---|---|---|---|---|---|
| 0 | 3 | 6 | 1 | 640 | 384 | checkpoint marker 1 (`CheckpointMarker`, `0x80052D30`): `game.checkpoint` = 1 when Zero's x ≥ 640, that is during the pass of gate 17 |
| 1 | 3 | 14 | 132 | 256 | 256 | talk zone (`TalkZone`, `0x8005335C`), optional, message 4 (below) |
| 2 | 4 | 8 | 17 | 624 | 384 | boss gate 17: the corridor gate |
| 3 | 4 | 8 | 35 | 912 | 384 | boss gate 35: the boss-room gate |
| 4 | 4 | 12 | 3 | 128 | 376 | grey block, left shaft; falls to y 552 |
| 5 | 4 | 12 | 3 | 480 | 504 | grey block, middle part; falls to y 552 |
| 6 | 4 | 12 | 3 | 320 | 712 | grey block on the pillar of the spike room; it is already on the floor (y 712 + 24 = 736), so it does not move |
| 7 | 4 | 12 | 3 | 224 | 632 | grey block over the left spike row; falls to y 728 |
| 8 | 0 | 15 | 2 | 400 | 304 | Nightmare Virus, row 2 (IDLE, DASH, SHOOT, DASH) |
| 9 | 2 | 15 | 2 | 400 | 400 | Nightmare Virus, row 2; Hard and up |
| 10 | 1 | 15 | 33 | 552 | 280 | Nightmare Virus, snatcher, row 1; difficulty 1 and up |
| 11 | 0 | 15 | 1 | 136 | 640 | Nightmare Virus, row 1 (IDLE, DASH) |
| 12 | 1 | 15 | 0 | 272 | 528 | Nightmare Virus, row 0 (IDLE, SHOOT); live in `scaravich-7` and `-6` (slot 0, HP 10) |
| 13 | 2 | 15 | 0 | 384 | 528 | Nightmare Virus, row 0; Hard and up |
| 14 | 0 | 15 | 0 | 416 | 496 | Nightmare Virus, row 0 |
| 15 | 0 | 15 | 0 | 416 | 560 | Nightmare Virus, row 0 |
| 16 | 2 | 15 | 0 | 448 | 528 | Nightmare Virus, row 0; Hard and up |
| 17 | 0 | 15 | 2 | 584 | 528 | Nightmare Virus, row 2 |
| 18 | 1 | 15 | 0 | 272 | 656 | Nightmare Virus, row 0 |
| 19 | 0 | 15 | 1 | 576 | 592 | Nightmare Virus, row 1 |
| 20 | 2 | 15 | 0 | 208 | 840 | Nightmare Virus, row 0; Hard and up |
| 21 | 2 | 15 | 0 | 240 | 808 | Nightmare Virus, row 0; Hard and up |
| 22 | 0 | 15 | 0 | 240 | 872 | Nightmare Virus, row 0 |
| 23 | 0 | 15 | 0 | 272 | 840 | Nightmare Virus, row 0 |
| 24 | 2 | 15 | 0 | 432 | 864 | Nightmare Virus, row 0; Hard and up |
| 25 | 0 | 15 | 1 | 512 | 864 | Nightmare Virus, row 1 |
| 26 | 4 | 4 | 79 | 560 | 400 | Rescue Reploid, global `id` 79 (stage 5 uses 64–78) |
| 27 | 3 | 13 | 0 | 992 | 368 | `BossSpawner` (`0x80052E44`) |

Totals: 18 Nightmare Virus records (kind 0: 9, kind 1: 3, kind 2: 6), 4 grey blocks, 2 boss gates,
1 Rescue Reploid, 1 checkpoint marker, 1 talk zone, 1 `BossSpawner`. No item (main 47), no health
capsule, no Light capsule [certain].

The second list (`0x800EBE24`) [certain]:

| # | kind | b | `id` | x, y | object |
|---|---|---|---|---|---|
| 0 | 3 | 3 | 0 | 0, 0 | `PaletteCycle` (stage logic 3, `0x80052898`); live in all states (logic slot 1, flags `0x09`) |
| 1 | 0 | 37 | 0 | 0, 0 | rain effect generator (main 37, overlay code `0x800EB008`); it deletes itself at once when `nightmare_effect` ≠ 6, so it is not live in any state |

Stage logic 27 (`ReadyControl`) is live in all states: it is the stage start banner and has no
record (`docs/engine.md`, "Stage start").

### Object types

| class, type | entry | code | sheet | notes |
|---|---|---|---|---|
| main 15 | `0x80041AB8` | main program | [`nightmare-virus.md`](../enemies/nightmare-virus.md) | the same entry as in Yammark; HP 10 on difficulty 1 (live: 10) [certain] |
| stage object 12 | `0x800EAB54` | overlay 29 | draft [`grey-block.md`](../enemies/grey-block.md) | the same code as overlay 08 (stage 5) after relocation [certain] |
| stage object 8 | `0x80050DFC` | main program | [`boss-gate.md`](../enemies/boss-gate.md) | ids 17 and 35 |
| stage object 4 | `0x8004F834` | main program | [`rescue-reploid.md`](../enemies/rescue-reploid.md) | |
| stage logic 3 | `0x80052898` | main program | [`turtloid.md`](turtloid.md), "Palette cycles" | |
| stage logic 6, 13, 14, 27 | `0x80052D30`, `0x80052E44`, `0x8005335C`, `0x800541D4` | main program | `docs/engine.md` | |
| main 37 | `0x800EB008` | overlay 29 | [`rain-effect.md`](../enemies/rain-effect.md) | the same code as overlay 08 after relocation [certain] |
| main 55 | `0x800FDA98` | boss overlay 44 | [`high-max.md`](../enemies/high-max.md) | made by the `BossSpawner` |

### Grey blocks (stage object 12, `var` 3)

The blocks of stage 5 (`scaravich.md`, "The grey blocks"). The code is the same; the hidden area
gives new facts [certain for the code unless marked]:

- They are crushers, not single falls. After the first fall the block waits 181 updates, rises at
  3 px/frame to its record y, and falls again at 4 px/frame. In `scaravich-5` block 4 rises (step
  2, y 465); in `scaravich-7` it waits on the floor (step 1, y 552) [certain]. See the draft sheet.
- `var` 3: the block deletes itself only when `game.checkpoint` ≥ 3. This area has checkpoints 0
  and 1 only, so the blocks stay for the whole area [certain].
- In `scaravich-8` and `-9` all four blocks are live at their floor points: (128, 552), (480,
  552), (320, 712), (224, 728) [certain].
- The blocks are only live with `nightmare_effect` 4 (from the stage 4 pick, which writes 4 to
  stage 5). With another value the maze has no crushers [certain for the code].

### Nightmare Virus (main 15)

The standard virus (`nightmare-virus.md`). The `id` values here are 0, 1, 2 and 33 (row 1
snatcher). The snatcher (record 10) and the only Rescue Reploid (record 26) are 120 px apart, at
the end of the route [certain for the places]. In `scaravich-7` the live virus (record 12) is in
state 1/2/2, `var` 0, HP 10 [certain]. In `scaravich-8` most virus records have a high kind digit
(`0x10`, `0x20`, `0x40`): they were killed and came back (`docs/objects-legend.md`) [likely].

### Rain effect (main 37)

The rain zone table of overlay 29 (`0x800EC64C`) has the same 8 rectangles as in Yammark's
overlay (`rain-effect.md`). Zone 7 (stage 21: x 16–608, y 240–912) is exactly this main room
(x 32–607, y 256–895) [certain for the numbers]. `StageStartReset` does not write
`nightmare_effect` in a hidden area, so the area keeps the value of stage 5. A pick of stage 6
(Rainy Turtloid) writes 6 to stage 5. So a player who comes from stage 5 with effect 6 gets rain in
this room; the zone can match [likely: no state with effect 6]. The zone test reads only
`game.stage`, not the area; stage 21 area 1 (overlay 30) has no main 37 code, so no rain there
[certain for the table entry].

### Talk zone (record 1)

`id` 132: optional (bit 7), message 4. Box: `talk_boxes` (`0x8007A154`) + 17 × 128 + 4 × 8 = x
64–288, y 256–320 (k = `talk_stage_idx`[42] = 17) [certain]. The box covers the start platform: in
`scaravich-5` `mega.hint` = 4 (the "!" call shows). `talk_seen` k 17 is 0 in all states: the user
did not open it. Message 4 of member 236 (Zero) [certain for the decode]:

> Zero: Have I been transferred again? / Zero: Alia? What's going on?

Messages 0 and 1 of the same bank are the High Max entry text, 2 and 3 the Dynamo texts.

### Palette cycle (second list, record 0)

`id` 0: target palette 105, count 1 (`0x80079F64`[42] = `0x800EBC08`). Script (`0x8007A01C`[42]
→ `0x800EBC04`[0] = `0x800EBBF4`): palettes 16, 17, 18, 17, 16, 19, 16 updates each, then a jump 6
entries back: a loop of 96 updates [certain for the data]. What it lights is not known.

## Camera and checkpoints

### Checkpoints

`checkpoint_start_tbl` (`0x80073300`) entry 42 → `0x800732E0`: two records [certain]. Each hidden
area (stages 19–22, entries 38–45) has a list of two records.

| checkpoint | record | player (x, y) | camera, layers 0/1/2 | limits x, y | facing | `beam_thru` |
|---|---|---|---|---|---|---|
| 0 | `0x80073014` | 128, 272 | 0, 240 | 0–320, 240–720 | left (0) | 1 |
| 1 | `0x80073038` | 768, 368 | 608, 240 | 608–608, 240–240 | right (`0x40`) | 1 |

- Checkpoint 0 is the arrival from the crystal. Zero beams in at (128, 272) and lands on the start
  platform at y 298 (`scaravich-5`) [certain].
- Checkpoint 1 is in the corridor between the gates. Marker 1 (x 640) sets it during the pass of
  gate 17, because the gate push takes Zero past x 640 [likely: the push moves Zero about 92 px
  while the camera goes from x 320 to 608 at 2 px/frame]. `scaravich-8` and `-9` have checkpoint 1.
- After a death past x 640, Zero starts again in the corridor, with the camera fixed at x 608 and
  y 240 [certain for the record].

### Camera limits

| where | min x – max x | min y – max y | source | seen in |
|---|---|---|---|---|
| main room | 0–320 | 240–720 | checkpoint 0 record | `scaravich-7`, `-6`, `-5` (ease speed 4) |
| corridor | 608–608 | 240–720 | gate 17 (case 17: X − 16; y not changed) | `scaravich-8` |
| boss room | 912–944 | 224–224 | gate 35 (case 35: X … X + 32, Y − 160) | `scaravich-9` |

All three are [certain]: the values in the states match the gate cases of `boss-gate.md`.

## The boss gates

Both gates use the main program code (`docs/enemies/boss-gate.md`):

| gate | x, y | open condition | new limits | push | time stamp | state in `-8` / `-9` |
|---|---|---|---|---|---|---|
| 17 | 624, 384 | always | x 608–608 | 0.64 px/frame | no | 2 / 2 (closed for good) |
| 35 | 912, 384 | always | x 912–944, y 224–224 | 0.64 px/frame | no | 1 (WAIT) / 2 |

- Neither gate needs a stage flag. Neither writes `gate_time` (`0x800CD30C` = 0 in `scaravich-9`)
  [certain].
- The trigger of each gate deletes all main objects except items, and all enemy shots
  (`ClearEnemies`). So no virus follows Zero into the corridor or the boss room [certain for the
  code].
- Gate 17 is one screen (288 px of camera move) from the main room; gate 35 opens the boss room.

## The boss: High Max

- The `BossSpawner` record (27) is at (992, 368): camera minimum (912, 224) + (80, 144). It enters
  the spawn band only after the camera leaves x 608: the record is 65 px right of the corridor
  view, and the band is 48 px [certain by the spawn rule]. So the spawner starts in the gate 35
  pass.
- With `boss_choice` 1 it loads overlay 44 at `0x800FA000` and makes main 55, `var` 1. In
  `scaravich-8` overlay 44 is not in RAM (10 974 bytes differ); in `scaravich-9` 1 of 33 304 bytes
  differs, `boss_pack_loaded` (`0x800CC868`) = 1 [certain].
- In `scaravich-9`: High Max is main slot 0, state 1/2/11, HP 48, at (1200, 321) = camera minimum +
  (288, 97). `game+0x20` = `0x8008EF48` (slot 0), `game+0x24` = 1, `game+0x8C` = 48, `game+0x2E` =
  1 (`0` in `scaravich-8`) [certain]. The fight: `docs/enemies/high-max.md` (another agent).
- Sprite banks 12–15 are types 53, 55, 55, 63 with no texture entry: the three possible bosses
  [certain from `spriteexport`].

## How the area ends

High Max's death sets `game.exit_req` = `0x10` (not stage 12). `StageExit` with stage ≥ 19 and
`boss_choice` 1: game mode 19, scene 4, then the report screen, which sets `boss_choice` = 2
(`docs/enemies/high-max.md`, "Where the clear leads") [certain for the code; no state after the
fight]. The player does not go back to stage 5. A death in the area uses the normal lives path.

## Overlay code

Overlay 29 holds only stage 5 object code. Its own table entries [certain]:

| class, type | overlay 29 entry | overlay 08 entry | records here | object |
|---|---|---|---|---|
| main 12 | `0x800E9F54` | `0x800ED5C4` | none | section goal (stage 5) |
| main 31 | `0x800EB81C` | `0x800EF6B8` | none | not read |
| main 37 | `0x800EB008` | `0x800EFD04` | 1 (second list) | rain effect |
| stage object 6 | `0x800EA7C0` | `0x800EDE30` | none | not read |
| stage object 12 | `0x800EAB54` | `0x800EE1C4` | 4 | grey block |

For each of the five types, the kit walks the same number of instructions in both overlays, and
every instruction that differs is a branch, a jump or an address part (relocation) [certain]. The
teleporter pole (stage 3), the crystal (main 28), main 50 and visual 8 and 14 of overlay 08 are not
in overlay 29. The sprite banks still hold the stage 5 set: bank 3 type 130 (the pole sprites),
bank 4 type 12, bank 5 type 133, bank 6 type 31 [certain for the bank list]. Against
`scaravich-2` (stage 5), only type 28 (the crystal, stage 5 bank 6) and type 50 (stage 5 banks
14–15, no texture) are gone, and the boss types 53, 55, 63 are added.

Other entries of the tables:

- Main 53, 55, 63 and visual 19, 21, 34 point into the boss overlay (`0x800FC128`, `0x800FDA98`,
  `0x800FB214`, `0x800FB0E8`, `0x80100C60`, `0x800FC7D8`): the same values in overlays 29, 30 and 31
  [certain].
- Stage logic 1 and 29 and misc 1, 29 and 56 have the same overlay addresses (`0x800E9AE0`,
  `0x800EA2EC`, `0x800EA02C`, `0x800E9E98`, `0x800ECBAC`) in every stage read (intro, Yammark,
  stage 5, Turtloid, the three hidden areas). In overlays 29, 30 and 31 the bytes at these addresses
  differ, and `0x800ECBAC` is past the end of overlay 29. No record here uses these types
  [certain]. They are probably not valid in these overlays [likely].

## Compared with the other hidden areas

| | stage 21, area 0 (Scaravich) | stage 21, area 1 (Turtloid) | stage 22, area 0 (Sheldon) |
|---|---|---|---|
| overlay, member, layout | 29, 119, 6 × 12 (5 × 4 used) | 30, 120, 8 × 5 | 31, 121, 26 × 3 |
| records (first list, second list) | 28, 2 | 41, 5 | 42, 7 |
| own object code | stage 5's (main 12, 31, 37; stage 6, 12) | stage 6's (main 9, 10, 14, 18, 26; stage 5, 12, 13; logic 17; misc 20; visual 7, 9, 17) | stage 7's (main 5, 13, 22, 27, 30; stage 15; misc 6, 7, 14; visual 4, 11, 32, 35) |
| enemies | Nightmare Virus only (18) | Monbando, Batton Bone B81, Weather Analyze | fixed beamers, Nightmare Virus, Nightmare Bug, Guardian |
| Nightmare Effect object | grey blocks (4); rain (6) possible | Nightmare shadow spawner | Nightmare Bug (1), crates (5) |
| route | one maze room, down and up again | one tall room around a shaft | one long corridor |
| first boss gate | `id` 17, x 624: always opens; camera x 608 | `id` 44, x 1072: needs `game+0x26` = 0 | `id` 48, x 2256: always opens |
| boss room camera | x 912–944, y 224 | x 1264–1296, y 704 | x 2576–2608, y 224 |
| `BossSpawner` record | (992, 368): camera min + (80, 144) | (1360, 864): + (96, 160) | (2640, 384): + (64, 160) |
| boss banks without texture | 12–15 | 14–17 | 12–15 |
| camera zones | none | none (zone word 0) | none |
| checkpoints | 2 | 2 | 2 |
| talk zone | `id` 132, k 17, box x 64–288, y 256–320 | `id` 132, k 18, box x 128–288, y 48–176 | `id` 132, k 19, box x 16–224, y 224–400 |

Shared by all three [certain for the code addresses]: the `BossSpawner` (stage logic 13,
`0x80052E44`), the boss overlay entries, the boss gate, the Rescue Reploid, the Nightmare Virus,
`CheckpointMarker`, `TalkZone` (always `id` 132: optional message 4) and `PaletteCycle`. Each
hidden-area overlay is a cut of the object code of the stage that leads to it. The Turtloid area
has no camera zone either: its `camera_zone_tbl` word (stage 21 area 1) is 0 [certain].

## Sprite exports

`spriteexport` on `scaravich-5`, `-7`, `-8` and `-9` into a scratch root, with no failure:

- Banks: 0 type 128 (gate and capsule), 1 type 15, 2 type 141 (Rescue Reploid), 3 type 130, 4 type
  12, 5 type 133, 6 type 31, 7 type 142 (grey block, 8 sprites), 8 type 37 (rain, 5 sprites), 9–11
  type 0, 12–15 types 53, 55, 55, 63 (no texture).
- Live exports: `nightmare-virus` (`scaravich-7`), `grey-block` (stage type 12, 8 sprites,
  `enemy-8df182908d`), `boss-gate` (`-8`, `-9`), `high-max` (`enemy-1b0293f642`, 23 animations, the
  same as from the Turtloid and Sheldon states).
- In `scaravich-9` the High Max texture covers banks 4–8; take those banks from the earlier
  states.
- The exporter writes the banks to `banks/turtloid-alt/`: `catalog/enemies.json` maps stage 21 to
  "turtloid-alt" without the area. Stage 21 area 0 needs its own name (for example
  "scaravich-alt") [certain for the folder name].

## Open questions

1. A state with `nightmare_effect` 6 to see the rain in this room, and one with another value
   (no blocks).
2. Main 31, main 12's use here (none), stage object 6 and sprite type 133: what they are.
3. What palette 105 is in this area.
4. Where Zero lost the two lives between `scaravich-7` and `scaravich-8`.
5. The X text bank (member 235) and the `boss_choice` 0 and 2 states of this area.
