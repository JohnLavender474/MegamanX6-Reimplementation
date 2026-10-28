# Dynamo in Infinity Mijinion's hidden area (`mijinion-4_dynamo`): notes

## Task and input

The user: "optional fight against Dynamo in Infinity Mijinion's stage". State
`~/mmx-data/x6/dumps/mijinion-4_dynamo.state.gz` (SHA-1 `304866452aea348f0d8d335237bd04c9a2538f7f`).
Stage 22, area 1, checkpoint 1; Zero at (2038, 394); camera (1936, 224), limits x 1936–1968, y 224.

Prior work: `docs/enemies/dynamo.md` exists, read from the disc overlay only
([`~/mmx-data/x6/re/hidden-areas/`](../hidden-areas/), `kit45.py`); the Dynamo helper names proposed there were never
applied to `catalog/helpers.json`. No stage page for stage 8 or for this area.

## Method

1. Kit: `python3 -m mmxdemake.rekit --state mijinion-4_dynamo.state.gz --type 63 --name dynamo`
   → `kit/kit-main/` (52 functions, 11 state tables, neighbour visual 34);
   `--type 34 --class visual` → `kit/kit-vis34/` (7 functions). Scratch kits of main 19, stage 12,
   stage 26 and logic 5 for the area (scratchpad `dynamo/kit-*`).
2. `scripts/state_check.py`: game bytes, player, camera, all live objects, overlay 45 against the
   disc (`rock_bin/45.bin`), overlays 43 and 44 (no match).
3. `scripts/area_tables.py`: the record lists, camera zones, checkpoint records, talk box, of the
   area (and of stage 8 with `mijinion-1_start`).
4. `scripts/ascii_map.py`, `scripts/small_map.py`: collision maps from the TMX.
5. `python3 -m mmxdemake.text` into the scratchpad: the hidden-area text banks (227–242).
6. `spriteexport --root ~/mmx-data/x6/re/dynamo/sprites-root`; every sheet checked by eye.
7. `scripts/make_helpers.py`: `helpers-proposed.json` and the clash check.

## Key facts

### Identity [certain]

- Main type 63 entry `0x800FB214` is in overlay 45, which is in RAM: 18 of 11 878 bytes differ
  from `rock_bin/45.bin`, all variables (`0x800FCE48`–`0x800FCE55` room points made world,
  `0x800FCE60` the boss pointer, `0x800FCE64` the floor y). `boss_choice` = 2.
- The code calls `BossLevel`, `BossMusicRequest` / `BossMusicWait`, `TextOpen(2)` (intro, once by
  `TextSeen(2)`) and `TextOpen(3)` (end). Messages 2 and 3 of member 242 (Zero) / 241 (X), and of
  every hidden-area bank 227–242, have the speaker "Dynamo". Messages 0 and 1 are High Max.
- The export shows a dark armoured reploid with a long hair tail, a pink sabre, a big crescent
  slash, green floor energy and blue/purple light pillars.

### Live check against the sheet [certain]

| sheet | live |
|---|---|
| HP {50, 58, 62, 66}[level] | level 2 (`game+0x8F`), `boss_hp_max` 62, HP 62 |
| spawn room (256, −16), floor 32 px below origin | at (2192, 384) = (1936 + 256, …); floor y 416; `0x800FCE64` = 384 |
| room points made world | `0x800FCE48`.. = 2192, 208, 1984, 2240, 2112, 240, 400 |
| contact 4, body boxes `0x800FCA88` / `0x800FCA94` | the same |
| THINK pattern 0 at high HP | `+0x8A` 0, `+0x8B` 2 (IDLE, BOOMERANG done; THINK next) |
| BOOMERANG: `+0x7E` throws, `+0x07` live blades | `+0x7E` 1, `+0x07` 1, animation 2 again (second throw) |
| blade `var` 0 straight, 4.5 px/update, HP 100, contact 6 | visual 34 `var` 0, speed −4.5, HP 100, contact 6, boxes `0x800FCE24`/`28`, parent `+0x7C` = Dynamo |
| intro end: camera `+0x48` = 8, target min x restored, `PlayerEventEnd`, HP bar on | `+0x48` 8, limits 1936–1968, `mega+0xD0` 0, `game+0x24` 1, `game+0x20` = slot 0 |
| text once per area | `talk_seen` k 20 = `0x0004` (bit 2) |
| shared flags 0 | `0x800FCE58` 0, `0x800FCE5C` 0; `game+0x33` 0 |

Blade place: 66 px in front of him = 16 px start + about 11 updates at 4.5 px; the animation says
12–13 updates since the flag. The difference is one or two updates (the update order of the spawn
update) [likely].

### New facts for the sheet

- Animation 0 is a **beam-in** and 12 a **beam-out** (pictures). The sheet called them "intro
  pose" and "leaps away".
- INTRO sub 1 sets the camera target min x = target max x − 16 (min + 16): the camera moves 16 px
  right during the intro, and sub 4 restores it [certain for the code].
- INTRO sub 2 sets the terrain box when the **world** y ≥ 300 (`slti v0, v0, 300` at
  `0x800FB43C`). In the Turtloid hidden area (room y 704) the box is set at once [certain for the
  code].
- No walked instruction reads `nightmare_effect` or `game+0x68` (difficulty) [certain].
- Soul drop: main 15 with flags `0x41`, `var` `0x32` (code `0x800FA8DC`–`0x800FA8F0`).
- Sprites 102–117 and 126–133 are in no animation.

### The area [certain unless marked]

- Way in: Illumina's reward crystal (main 28 `id` 1 at (4384, 864), fight 2 < 5 401 updates).
  `0x80042F7C`: 8 → stage 22, area 1. Stage 8's own list has no main 28 record.
- Overlay 32 (third point of the overlay rule), member 122, 10 × 4 screens; 32 records, 5 in the
  second list.
- Camera zones exist (word `0x80079D50`): zone 0 (x 896–911) max y 480, zone 1 (x 816–831) max y
  224. The other hidden areas have none.
- Checkpoints: 0 at (256, 240) limits x 0–1296; 1 at (1776, 352), camera fixed at x 1616.
- Gates 50 (corridor, camera x 1616) and 51 (boss room, x 1936–1968), both state 2.
- Light capsule `id` 2: Blade Armor arms; message 5; `game+0x69` = `0x0E` (bit `0x02` set).
- Grey block (stage 12 `id` 1): only with effect 4; deleted at checkpoint ≥ 1.
- Fire spawner (main 19 `id` 1, second list): overlay 32 code has a stage 22 area 1 zone, player x
  385–1423, and a stage 19 area 1 zone (x 321–2143, y < 1008). Not live in the state.
- `game+0x60` = `0xFF` (all 8 Mavericks), `game+0x5F` = `0x11`.

## Sprite result

| export | state | verdict |
|---|---|---|
| `enemy-f49a77c40e` (main 63, live, bank 15, 15 animations, 137 sprites) | new | clean: copy as `dynamo` |
| `boss-gate` `enemy-30419fd669` | known | clean |
| `nightmare-virus` `enemy-363dce0a1a`, `nightmare-virus-orb` `enemy-a96f0b9d66` | known | clean |
| `enemy-06ef7e2514` (main type 8, search, bank 3) | new ID | **wrong**: broken shapes and colours; the cells are covered by the Dynamo texture but not marked |

Banks 0–2 clean; banks 4–6 (types 11, 19, 142) reported "cells overwritten by type 63". The
exporter put the banks under `banks/sheldon-alt/` because `stages` maps 22 → `sheldon-alt` with no
area. The shared outputs also hold an old `banks/stage-22/` and `stages/stage-22.json` (a run from
before the map entry).

## What the docs depend on (check these first)

1. Overlay 45 in RAM (`scripts/state_check.py`, line "overlay45 ... differ 18").
2. Text speaker "Dynamo" in messages 2 and 3 (scratchpad `dynamo/text/m242/text.txt`).
3. The camera zone data (`scripts/area_tables.py`).
4. The fire zone 278 = stage 22 area 1 (scratchpad `dynamo/kit-type19/code.txt`, `0x800ED014`–
   `0x800ED028`): `lhu game+0x0C` reads the stage byte and the area byte together.

## Hazards (for a reimplementation)

- The intro terrain-box test uses a fixed world y (300), not the room y. Use the room y.
- `boss_hp_max` = 58 + (−8) is stored as a byte and only works by wrap-around (306 → 50).
- The stun escape gives x speed 8 only when he faces left (open question in the sheet).
- The shots end through two shared globals (`dynamo_dead`, `dynamo_stunned`): a second Dynamo
  would share them.

## Open questions

1. States of the other moves, the stun and the death; a state after the clear (the report screen).
2. Camera `+0x48` (1 in the intro, 8 after it); why the intro moves the camera 16 px.
3. The boss music track of stage 22 (the per-stage boss track table was not read).
4. The item `id` 7 at (8, 320); how the Rescue Reploids over the pit stand; the palette cycles.
5. Whether overlay 32's main 19 is the same code as overlay 11's after relocation (not compared).
