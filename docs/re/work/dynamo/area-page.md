# Stage: Infinity Mijinion, hidden area (stage 22, area 1, overlay 32)

The teleporter crystal of Infinity Mijinion's stage takes the player to this hidden area. The
crystal is not a record: Illumina makes it (main 28, `id` 1, at (4384, 864)) at the end of her
second fight, only when that fight took less than 5 401 updates (90 s)
(`docs/enemies/illumina.md`, "DEATH"; `IlluminaReward` `0x800F1FBC`). `id` 1 requests `0x41`;
`0x80042F7C` changes stage 8 to stage (8 − 1) / 2 + 19 = **22**, area (8 − 1) & 1 = **1**
[certain: `mijinion-4_dynamo` has `game.stage` 22, `game.area` 1]. It is one of the 8 hidden areas
(stages 19–22; `docs/engine.md`, "Boss overlay"). Stage 22 area 0 is Shield Sheldon's hidden area
([`sheldon-alt.md`](sheldon-alt.md)). The area is a tower, a long cable down over a pit, a field of
short ropes, a short floor, the corridor between two boss gates and the boss room. With
`boss_choice` 2 the boss is Dynamo ([`dynamo.md`](../enemies/dynamo.md)).

## Save state

| state | checkpoint | player (x, y), HP | camera (x, y), limits | what |
|---|---|---|---|---|
| `mijinion-4_dynamo` | 1 | Zero, 2038, 394, 26 | 1936, 224; x 1936–1968, y 224 | the boss room: Dynamo (main 63, `var` 0) throws his first blade |

- Zero (`game+0x38` = 1, form 5); difficulty 1; lives 5; `boss_choice` 2; boss level 2;
  `nightmare_effect` 3; stage time 10 874 (the time of stage 8 goes on: 6 582 in `mijinion-3`).
  `game+0x60` = `0xFF`: all 8 Mavericks are defeated, so `boss_choice` 2 comes from Gate's lab
  (`docs/engine.md`, "`boss_choice`") [certain for the bytes].
- `game+0x69` (armour parts) = `0x0E`: the Blade Armor arms bit (`0x02`) is set, so the Light
  capsule of this area was taken [likely: no other capsule with `id` 2 is known].

## Overlay, member and lists

- Overlay 32 at `0x800E9860`; stage member 122, a layout of 10 × 4 screens, catalog entry
  "Infinity Mijinion, hidden area" (`~/mmx-data/x6/stages/m122-infinity-mijinion-hidden-area/`,
  TMX `~/mmx-data/x6/tmx/m122-infinity-mijinion-hidden-area/`). Overlay 32 = 25 + 2 × (22 − 19)
  + 1: the third point of the rule in `docs/engine.md` [certain].
- `enemy_set_tbl` (`0x8007349C`) stage 22 area 1: `0x800ED770`, 32 records and the end record.
  `fixed_set_tbl`: `0x800ED878`, 5 records [certain].
- `nightmare_effect` is 3, the value of stage 8: a hidden area keeps the value of the stage before
  it [certain for the byte].
- Text bank: member 242 for Zero, 241 for X (`text_members_alt`, index ((22 − 19) × 2 + 1) × 2 +
  character) [certain for the decode].

## Map

Collision of layer 0, one character per 32 × 32 px (two cells; the most important value wins).
`#` 56 solid, `=` 57, `|` 59 boss gate, `\` hang bars 41–44 and 47 (the cable), `/` slopes 5–8
and 16, `!` rope 46. Markers: `Z` start (checkpoint 0), `T` talk zone, `L` Light capsule, `I`
item, `R` Rescue Reploid, `v` Nightmare Virus, `B` grey block, `z` camera zone, `D` boss gate,
`C` checkpoint marker, `S` `BossSpawner`. Column 0 is x 0; each column is 32 px.

```
  192 ===================================================         ============
  224         Z                           !        B    =#########=          =
  256                                     ! v!      v   ###########          #
  288  L     T             R  v         ! !! !          ###########          #
  320 I########//    \\\\\        R     !  v    !       ###########          #
  352 #########//////    \\\\\         !!  ! v! ! v     |         |          #
  384 #############//#       \\z\\  v  !      !   ###R/ D       C D S        #
  416 ################  v        \z\\\ !  R   !   ###//#######################
  448 ################                            ############################
  480               ==                                  =
  512               ==                       !          =
  544               ==                       !   ! ! !  =
  576               ==              R     !v     ! v v  =
  608               ==                ! ! !    !   ! ! v=
  640               ==                ! !   R  !   v    =
  672               ==                !        ! R R R  =
  704               ==                                  =
```

Collision values in layer 0 (cells): 0 (9 224), 56 (638), 57 (224), 46 (60), 59 (16), 16 (16),
47 (14), 41–44 (8 each), 5–8 (4 each) [certain].

| part | x (px) | y (px) | what [certain for the tiles] |
|---|---|---|---|
| tower | 0–495 | 320–1023 | the start: a flat roof at y 336 (x 0–287), then a slope down to the right (x 288–479) to y 384–400. The right face (x 480–511) is collision 57 down to y 1023 |
| cable | 496–1007 | 320–447 | a long diagonal hang bar (values 41–44 and 47), from the tower roof down to the right; the picture is a red cable |
| rope field | 1024–1535 | 240–687 | 60 rope cells: short vertical ropes in the air, in two groups (y 240–447 and y 512–687) |
| the pit | 512–1759 | 448–1023 | no floor cell: a fall is a death |
| floor | 1424–1599 | 400–416 | a short floor at y 400 (x 1424–1503), a slope down to y 416 (x 1504–1567, values 5–8 and 16), then the floor of the corridor, in front of boss gate 50 |
| corridor | 1632–1919 | 352–415 | between boss gate 50 (cells x 1600–1631, y 352–415) and boss gate 51 (cells x 1920–1951); solid above (y 224–335) |
| boss room | 1952–2271 | 224–415 | ceiling 57 at y 208, floor at y 416, right wall x 2272–2303 |

The art (`stage.png`): the tower and the rooms are red girders; the background behind the cable
and the ropes is black. The boss room has a broken reploid head in its background.

## Objects

32 records (list `0x800ED770`). `kind` low digit 0–2 main, 3 stage logic, 4 stage object.

| # | kind | b | `id` | x | y | object |
|---|---|---|---|---|---|---|
| 0 | 3 | 6 | 1 | 1872 | 384 | checkpoint marker 1: checkpoint 1 when the player's x ≥ 1872 (in the corridor) |
| 1 | 3 | 0 | 0 | 896 | 416 | camera zone 0 (below) |
| 2 | 3 | 0 | 1 | 816 | 384 | camera zone 1 (below) |
| 3 | 3 | 14 | 132 | 224 | 304 | talk zone, optional, message 4 (below) |
| 4–12 | 4 | 4 | 119–127 | (696, 304), (920, 336), (984, 576), (1160, 432), (1240, 656), (1392, 672), (1464, 672), (1528, 672), (1504, 384) | | 9 Rescue Reploids, global `id` 119–127 (stage 8 uses 112–118). Records 6–11 are over the pit or the rope field, with no floor cell under them |
| 13–25 | 0–2 | 15 | 0, 1, 2, 32, 33, 34 | | | 13 Nightmare Virus records: `id` 2 (576, 432); 1 (784, 304); 0 (976, 384); 33 (1200, 328); 0 (1184, 600); 0 (1232, 256); 1 (1272, 368); 0 (1408, 352); 34 (1488, 272); 1 (1448, 592); 32 (1464, 640); 0 (1512, 592); 0 (1584, 608). Kinds: 0 ×4, 1 ×3, 2 ×6 |
| 26 | 4 | 12 | 1 | 1456 | 248 | grey block (sprite type 142): only with `nightmare_effect` 4; it deletes itself at checkpoint 1 or more (`var` 1) [certain for the overlay 32 code, set-up `0x800EC6B4`]. Not live here (effect 3) |
| 27 | 4 | 26 | 2 | 48 | 312 | **Light capsule `id` 2**: Blade Armor arms (bit `0x02`), message 5 (`docs/enemies/light-capsule.md`). Message 5 of member 242: Dr. Light gives Zero "the arm program for the Blade Armor" (the charged blade: hold Up and press shot with a full charge) |
| 28 | 0 | 47 | 7 | 8 | 320 | item, `id` 7 (not read) |
| 29 | 4 | 8 | 50 | 1616 | 384 | boss gate 50: the corridor gate |
| 30 | 4 | 8 | 51 | 1936 | 384 | boss gate 51: the boss-room gate |
| 31 | 3 | 13 | 0 | 2000 | 384 | `BossSpawner`: camera minimum (1936, 224) + (64, 160), as in stage 22 area 0 |

The second list (`0x800ED878`) [certain]:

| # | kind | b | `id` | x, y | object |
|---|---|---|---|---|---|
| 0–2 | 3 | 3 | 0, 3, 4 | 0, 0 | `PaletteCycle`; all three live in `mijinion-4` (logic slots 1–3) |
| 3 | 3 | 5 | 0 | 160, 120 | `TunnelBgAnimator` (the overlay 32 copy, entry `0x800ED4AC`); live (logic slot 4) |
| 4 | 0 | 19 | 1 | 0, 0 | the Nightmare Effect 3 fire spawner (main 19, `var` 1; overlay 32 entry `0x800ED308`) |

**The fire (main 19).** The overlay 32 copy of the spawner (`0x800ECED4`) has zones by stage: stage
2, stage 8 (x 2513–4223), stage 19 area 1 (`game+0x0C` halfword 275: x 321–2143, y < 1008) and
**stage 22 area 1** (halfword 278: player x 385–1423) [certain for the code]. So with Nightmare
Effect 3 fire balls fall from the top of the view over the cable and the rope field. The spawner
is not live in `mijinion-4` (main class: the boss gate trigger deletes the main objects
[likely]).

Stage logic 27 (the stage start banner) is live; it has no record.

### Talk zone (record 3)

`id` 132: optional, message 4. k = `talk_stage_idx`[45] = 20; box `talk_boxes` + 20 × 128 + 4 × 8
= x 192–272, y 224–320, over the start [certain]. `talk_seen` k 20 = `0x0004`: bit 4 is clear,
so the user did not open the call; bit 2 is Dynamo's intro text. Message 4: "Zero: Hmm? I sense
something exists near here..." (X: "Hmm? I sense something...").

## Camera and checkpoints

### Checkpoints

`checkpoint_start_tbl` entry 45 → `0x800732F8`: two records [certain].

| checkpoint | record | player (x, y) | camera (x, y) | limits x, y | facing | `beam_thru` |
|---|---|---|---|---|---|---|
| 0 | `0x800730EC` | 256, 240 | 96, 224 | 0–1296, 224–224 | right | 1 |
| 1 | `0x80073110` | 1776, 352 | 1616, 224 | 1616–1616, 224–224 | right | 1 |

Checkpoint 0 is the arrival from the crystal, on the tower roof. Checkpoint 1 is in the corridor:
marker 1 (x 1872) sets it after the pass of gate 50. Max x 1296 + 320 = 1616: the view ends at
gate 50.

### Camera zones (stage logic 0)

The `camera_zone_tbl` word of stage 22 area 1 is `0x80079D50` (not 0: the first hidden area with
camera zones) [certain]:

| zone (`id`) | record | rectangle (x, y) | limits written |
|---|---|---|---|
| 0 | 896, 416 | x 896–911, y 224–495 | max y = 480 |
| 1 | 816, 384 | x 816–831, y 224–495 | max y = 224 |

So on the way right, at x 896 (on the cable) the camera can go down to y 480 (view y 480–719):
the lower ropes. Back at x 816–831 it goes up to y 224 again [certain for the data]. The camera
y is 224 in the corridor and the boss room.

### Camera limits

| where | min x – max x | min y – max y | source |
|---|---|---|---|
| tower, cable, ropes | 0–1296 | 224–224 (480 after zone 0) | checkpoint 0, zones |
| corridor | 1616–1616 | 224–224 | gate 50 (case 50: X, X, Y − 160) |
| boss room | 1936–1968 | 224–224 | gate 51 (case 51: X … X + 32, Y − 160); `mijinion-4` [certain] |

## The boss gates

| gate | x, y | open condition | new limits | state in `mijinion-4` |
|---|---|---|---|---|
| 50 | 1616, 384 | always | x 1616, y 224 | 2 (closed for good) |
| 51 | 1936, 384 | always | x 1936–1968, y 224 | 2 (closed for good) |

Neither writes `gate_time` (`0x800CD30C` = 0) [certain]. Both triggers delete the main objects
(`ClearEnemies`; `docs/enemies/boss-gate.md`).

## The boss: Dynamo

- The `BossSpawner` (stage logic 13, `0x80052E44`) with `boss_choice` 2 loads overlay 45 at
  `0x800FA000` (`boss_pack_loaded` = 1) and makes main 63. It is gone in `mijinion-4` (it deletes
  itself) [certain].
- Dynamo is main slot 0 at (2192, 384): the room spawn point (256, −16) + (1936, 224), on the floor
  (y 384 + 32 = 416). His blade (visual 34, `var` 0) is at (2126, 384). The fight:
  [`dynamo.md`](../enemies/dynamo.md).
- Sprite banks 12–15 are types 53, 55, 55, 63 (the three possible bosses); bank 15 has Dynamo's
  texture in this state.

## How the area ends

Dynamo's death sets `game.exit_req` = `0x10`. With `boss_choice` 2, `StageExit` goes to game mode
11, the report screen (`docs/enemies/high-max.md`, "Where the clear leads") [certain for the code;
no state after the fight]. The player does not go back to stage 8. Dynamo gives no part and writes
no progress byte.

## Sprite exports

`spriteexport` on `mijinion-4` (scratch root `~/mmx-data/x6/re/dynamo/sprites-root`):

- Banks: 0 type 128 (gate, capsule), 1 type 15 (virus), 2 type 141 (Rescue Reploid), 3 type 8,
  4–6 types 11, 19, 142 (cells overwritten by type 63), 7–11 type 0, 12–15 types 53, 55, 55, 63
  (bank 15 live: Dynamo, 137 sprites).
- Clean: `dynamo` (`enemy-f49a77c40e`, new), `boss-gate`, `nightmare-virus`,
  `nightmare-virus-orb` (the same IDs as before).
- Wrong: `enemy-06ef7e2514` (main type 8, search, bank 3): broken shapes and wrong colours,
  because Dynamo's texture covers the cells; the tool does not mark bank 3 as overwritten (the same
  fault as `enemy-08bc9efa49` in `mijinion-3`, `docs/TODO.md`).
- The exporter writes to `banks/sheldon-alt/`: `catalog/enemies.json` maps stage 22 to
  "sheldon-alt" without the area.

## Open questions

1. A state before the boss room (the cable, the ropes, the fire of Nightmare Effect 3).
2. The item `id` 7 (main 47) at (8, 320).
3. How the Rescue Reploids over the pit stand (no floor cell under records 6–11).
4. What the palette cycles 0, 3 and 4 light here, and the layer 2 picture of `TunnelBgAnimator`.
