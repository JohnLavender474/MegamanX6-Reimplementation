# Stage: Metal Shark Player, hidden area (stage 20, area 1, overlay 28)

The teleporter crystal `id` 1 in the bottom room of Metal Shark Player's stage (main 28, at (1552,
1584); [`shark.md`](shark.md), "The teleporter crystals") asks for exit `0x41`. The stage exit code
changes stage 4 to stage (4 − 1) / 2 + 19 = **20**, area (4 − 1) & 1 = **1** [certain:
`shark-10_alt` has `game.stage` 20 and `game.area` 1]. It is one of the 8 hidden areas (stages
19–22; `docs/engine.md`, "Boss overlay"). The Fandom page of the stage calls it the "Another
Route". The other hidden areas with pages are stage 21 area 0 ([`scaravich-alt.md`](scaravich-alt.md)),
stage 21 area 1 ([`turtloid.md`](turtloid.md), "Alternate section"), stage 22 area 0
([`sheldon-alt.md`](sheldon-alt.md)) and stage 22 area 1 ([`mijinion-alt.md`](mijinion-alt.md)).

The area is one long corridor on a conveyor belt under the Nightmare Press, with a forced scroll
to the right. An empty Ride Armor waits at the start. At the end are a high block, the corridor
between two boss gates and the boss room. With `boss_choice` 2 the boss is Dynamo
([`dynamo.md`](../enemies/dynamo.md)).

## Save state

| state | checkpoint | player (x, y), HP | camera (x, y), limits | what |
|---|---|---|---|---|
| `shark-10_alt` | 0 | X, 79, 363, 50 | 0, 224; x 0–2576, y 224 | the start: X stands on the start ledge, right of the empty Ride Armor; the forced scroll waits |

- X (`game+0x38` = 0) in step 2, facing right, not riding (`mega+0xD5` = 0); difficulty 1; lives 4;
  `boss_choice` 2; `nightmare_effect` 2; stage time 24 286 [certain for the bytes].
- `game+0x60` = `0xFF`: all 8 Mavericks are defeated, so `boss_choice` 2 comes from Gate's lab
  (`docs/engine.md`, "`boss_choice`") [certain for the bytes].
- Rescue Reploids 48–62 (stage 4 areas 0 and 1) have status 2 (saved); 63 (this area) has 0
  [likely for the nibble order of `game.reploid`].
- The preview picture shows X, the red Ride Armor at the left, and the talk "!" icon at the bottom
  right (see "Talk zone").

## Overlay, member and lists

- Overlay 28 at `0x800E9860`, `0x9D8E` bytes. Overlay 28 = 25 + 2 × (20 − 19) + 1, the fifth
  point of the rule in `docs/engine.md` [certain]. The RAM is a byte match to the disc except 63
  bytes: two record flags and the belt variables (`0x800F35AC`–`0x800F35EC`) [certain].
- Stage member **118**, a layout of 15 × 6 screens at overlay offset `0x8B28`, catalog entry
  "Metal Shark Player, hidden area" (`~/mmx-data/x6/stages/m118-metal-shark-player-hidden-area/`,
  TMX `~/mmx-data/x6/tmx/m118-metal-shark-player-hidden-area/`). Only row 1 (y 240–479) of layer 0
  is used for play; the belt frame store is in row 2 [certain]. So the hidden-area members are 115
  (stage 19 area 0) to 122 (stage 22 area 1), in stage and area order [likely: 118–122 are known].
- `learn` needs a fix to find member 118 (see "The map drawing").
- `enemy_set_tbl` (`0x8007349C`) stage 20 area 1: `0x800F2498`, 31 records and the end record.
  `fixed_set_tbl` (`0x80073554`): `0x800F2598`, 27 records [certain].
- No camera zone: the `camera_zone_tbl` word of stage 20 area 1 is 0 [certain].
- BG layers: layer 1 mode 9 (the press moves its y), layer 2 mode 4 [certain: `tmx`].
- `nightmare_effect` is 2, the value of stage 4: a hidden area keeps the value of the stage before
  it [certain for the byte]. No object of this area uses 2 (see "The Nightmare Effect").
- Text bank: member 233 for X, 234 for Zero (`text_members_alt`, index ((20 − 19) × 2 + 1) × 2 +
  character) [certain for the decode].

## Map

Collision of layer 0, one character per 32 × 32 px (two cells; the most important value wins).
`#` 56 solid, `=` 57, `~` 58 belt, `|` 59 boss gate. Markers: `X` the player in `shark-10_alt`,
`A` Ride Armor place, `T` talk zone, `v` Nightmare Virus, `J` junk block, `Q` Nightmare Cube, `M`
Mettaur D2, `R` Rescue Reploid, `D` boss gate, `C` checkpoint marker, `S` `BossSpawner`. Each
column is 32 px; the digits on top mark every 320 px (one screen width).

```
       0         1         2         3         4         5         6         7         8         9         0
  192  ==============================================================================================================
  224                                                                                            =       =          =
  256   T                                                                                     v  #       #          #
  288                                                                                            #       #          #
  320                                     v  v        v             v                  v   vv##  #       #          #
  352   AX                        v v                     v    v         v               v   ##  |   C   |          #
  384  ###                                   v     J QJ QQ                  MM M             ##R D       D    S     #
  416  ###~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~########################
  448  ##############################################################################################################
```

Collision values in layer 0 (cells): 0 (21 802), 56 (800), 57 (224), 58 (198), 59 (16) [certain].

| part | x (px) | y (px) | what [certain for the tiles] |
|---|---|---|---|
| start ledge | 0–95 | 384 | floor at y 384; the Ride Armor stands here |
| belt | 96–2751 | 416 | the conveyor belt surface (value 58, y 416–431), 2 656 px long; ceiling line (value 57) at y 208 over the whole area |
| block | 2752–2815 | 320–415 | a solid block, 96 px above the belt |
| gate floor | 2816–2879 | 416 | a normal floor in front of boss gate 37; the Rescue Reploid stands here |
| corridor | 2912–3135 | 224–415 | between boss gate 37 (cells x 2880–2911, y 352–415) and boss gate 38 (cells x 3136–3167) |
| boss room | 3168–3487 | 224–415 | floor at y 416, right wall x 3488–3519 |
| frame store | 96–223, 2736–2863 | 512–591 | belt tiles outside the camera limits (the frames of the belt art) |

The art (`stage.png`): orange and blue machines over the press, a pipe, junk piles behind the belt,
and a junk room with a crane in the boss-room background.

## Objects

31 records (list `0x800F2498`). `kind` low digit 0–2 main (the digit is also the lowest
difficulty), 3 stage logic, 4 stage object [certain for every field].

| # | kind | b | `id` | x | y | object |
|---|---|---|---|---|---|---|
| 0 | 3 | 6 | 1 | 3026 | 375 | checkpoint marker 1: checkpoint 1 when X's x ≥ 3026 (in the corridor) |
| 1 | 3 | 14 | 132 | 32 | 256 | talk zone, optional, message 4 (below); live |
| 2 | 4 | 4 | 63 | 2832 | 400 | Rescue Reploid, global `id` 63 (stage 4 uses 48–62), on the gate floor |
| 3–5 | 0 | 27 | 0 | 2224, 2256, 2320 | 400 | Mettaur D2 ([`mettaur-d2.md`](../enemies/mettaur-d2.md)), on the belt |
| 6–21 | 0–2 | 15 | 0, 1, 2, 34 | | | 16 Nightmare Virus records (below) |
| 22–24 | 4 | 15 | 2 | 1480, 1576, 1624 | 392 | Nightmare Cube, brown (`id` 2); only with `nightmare_effect` 5 |
| 25–26 | 4 | 11 | 128 | 1432, 1528 | 392 | junk block ([`junk-block.md`](../enemies/junk-block.md)), `id` bit 7: waits until X's x ≥ 1088 |
| 27 | 4 | 8 | 37 | 2896 | 384 | boss gate 37: the corridor gate |
| 28 | 4 | 8 | 38 | 3152 | 384 | boss gate 38: the boss-room gate |
| 29 | 3 | 13 | 0 | 3312 | 384 | `BossSpawner`: camera minimum (3152, 224) + (160, 160) |
| 30 | 3 | 30 | 1 | 32 | 354 | Ride Armor place (`RideSpawner`): `actor2` type 1; used (record flag `0x81`) |

Nightmare Virus records (main 15, [`nightmare-virus.md`](../enemies/nightmare-virus.md)) [certain]:

| `id` | kind 0 | kind 1 | kind 2 |
|---|---|---|---|
| 0 | (1120, 336), (1232, 384), (2736, 336) | (2640, 368) | (1232, 336), (1504, 320), (1968, 320), (2688, 336) |
| 1 | | (2560, 336) | |
| 2 | (864, 352), (928, 352), (1656, 360), (1816, 360), (2112, 376) | (2560, 336) | |
| 34 (snatcher, row 2) | | (2800, 280) | |

On difficulty 1 the 12 records of kind 0 and 1 spawn. Records 16 and 17 are at the same point. The
snatcher (record 21) is 120 px above the only Rescue Reploid (record 2), at the end of the belt
[certain for the places].

No Light capsule, no item, no health capsule [certain].

The second list (`0x800F2598`) [certain]:

| # | kind | b | `id` | x, y | object |
|---|---|---|---|---|---|
| 0–11 | 4 | 9 | 1 | 128 + 240 n, 176 | press bodies, 240 × 128 px, x 8–2887; all live |
| 12–23 | 4 | 9 | 66, 68, 70, 82 | see "The press" | press teeth; all live |
| 24 | 7 | 2 | 32 | 0, 0 | `PressUpdate`, the Nightmare Press controller; live (kind-7 slot 0) |
| 25 | 3 | 15 | 0 | 0, 0 | the belt (`BeltUpdateAlt`, below); live (logic slot 2) |
| 26 | 7 | 4 | 0 | 0, 0 | the forced scroll (`AutoScrollAlt`, below); live (kind-7 slot 1) |

There is no palette cycle in this area. Stage logic 27 (the stage start banner) is live; it has no
record.

### Object types

| class, type | entry | code | sheet |
|---|---|---|---|
| main 1 | `0x800EBA48` | overlay 28 (= overlay 06 Junkroid) | no record here |
| main 15 | `0x80041AB8` | main program | [`nightmare-virus.md`](../enemies/nightmare-virus.md) |
| main 27 | `0x800EB0FC` | overlay 28 (= overlays 06 and 07) | [`mettaur-d2.md`](../enemies/mettaur-d2.md) |
| stage 9 | `0x800EA000` | overlay 28 (= overlay 07) | [`nightmare-press.md`](../enemies/nightmare-press.md) |
| stage 11 | `0x800EA698` | overlay 28 (= overlay 06) | [`junk-block.md`](../enemies/junk-block.md) |
| stage 15 | `0x800F2188` | overlay 28 (= overlay 07) | [`nightmare-cube.md`](../enemies/nightmare-cube.md) |
| stage 4, 8 | `0x8004F834`, `0x80050DFC` | main program | [`rescue-reploid.md`](../enemies/rescue-reploid.md), [`boss-gate.md`](../enemies/boss-gate.md) |
| logic 6, 13, 14, 27 | main program | main program | `docs/engine.md` |
| logic 15 | `0x800F0984` | overlay 28, **new** | [`conveyor-belt.md`](../enemies/conveyor-belt.md), "Stage 20 area 1" |
| logic 30 | `0x800EBEE8` | overlay 28 (= overlay 06 `RideSpawner`) | [`ride-armor.md`](../players/ride-armor.md) |
| kind-7 2 | `0x80046E14` | main program | [`nightmare-press.md`](../enemies/nightmare-press.md) |
| kind-7 4 | `0x800E9864` | overlay 28, **new** | [`conveyor-belt.md`](../enemies/conveyor-belt.md), "Stage 20 area 1" |
| `actor2` 1 | `0x800F01A4` | overlay 28 (= overlay 06 type 0 `RideUpdate`) | [`ride-armor.md`](../players/ride-armor.md) |

"=" means: the same number of instructions, and every instruction that differs is a jump, a branch
or an address part (relocation) [certain]. Overlay 28 is a cut of the object code of stage 4: the
crystal (main 28), Nightmare Pressure, Nightmare Freeze, the warp logic and the area 1 belt
(logic 9) are not in it.

## The belt and the forced scroll

Both act together, as in the mini-boss room of section 2. The details are in
[`conveyor-belt.md`](../enemies/conveyor-belt.md), "Stage 20 area 1 (overlay 28)". In short
[certain for the code]:

1. The belt (stage logic 15) moves X 1 px to the left in each update while X stands on a cell 58,
   as in section 2. It also moves the Ride Armor (`actor2`) when the ride stands on a cell 58 and is
   not against a wall on its left. Enemies are not carried; the junk blocks ride by their own code.
2. The belt ends when the camera x reaches 2752: after gate 37 has moved the camera to x 2848
   [likely: the camera max x is 2576 before the gate].
3. The forced scroll (kind-7 type 4) starts when X's x reaches 128 (X steps off the ledge onto the
   belt): X's walk speed 2.0 → 2.5, and the camera moves right at 0.5 px per update, with min x =
   max x = the camera x. At camera x 2576 the walk speed goes back to 2.0 and the scroll ends.
   That is 5 152 updates (86 s).
4. So a standing X goes left at 1 px per update until the left edge of the view stops X (camera x
   + 8, `CameraClamp`); X walking right moves at 1.5 px per update. In `shark-10_alt` the scroll
   waits (state 1, X at x 79) [certain].

## The press

The same Nightmare Press as in stage 4 ([`nightmare-press.md`](../enemies/nightmare-press.md)):
controller `PressUpdate` (`var` 32), 12 bodies and 12 teeth (stage object 9, the overlay 07 code).
What is particular to this area [certain for the code unless marked]:

- The set-up gives a 32 px intro drop in a stage ≥ 19 (stage 4: 16 px). So the top rest is offset
  32 and the bottom is offset 112. In `shark-10_alt` the press waits in the first top wait (sub 1,
  timer 143 of 180, offset 32; layer 1 y 224 = 224 + 32 − 32).
- The kill line (case stage ≥ 19) is layer-0 y − layer-1 y + 240 = offset + 208: y 240 at the top
  and y 320 at the bottom, the same world line as in stage 4. The test runs at every x; the press is
  deleted when X's x ≥ 2896 (gate 37) or at checkpoint 1.
- A standing X on the belt (floor 416) has its box top at 378, and on the ledge (floor 384) at 346:
  the kill line never reaches them. On the block at x 2752–2815 (floor 320) the box top is at 282:
  the kill line reaches it at offset ≥ 74, under body 11 (x 2648–2887) [likely: from the formula].
- Body bottoms: offset + 207 (y 239 / 319). The teeth (box half sizes from `var`: high nibble × 8,
  low nibble × 8; bottom = record y + offset − 32 + half height):

| `var` | x (px) | size (px) | bottom: top / bottom of the cycle | a standing X on the belt is touched at offset | the ride is touched at offset |
|---|---|---|---|---|---|
| 68 | 256–319, 608–671, 928–991, 1728–1791 | 64 × 64 | 304 / 384 | ≥ 106 | ≥ 86 |
| 66 | 1280–1343, 1536–1599 | 64 × 32 | 272 / 352 | never | never |
| 82 | 1344–1423 | 80 × 32 | 272 / 352 | never | never |
| 70 | 1856–1919, 2272–2527 (4 teeth side by side) | 64 × 96 | 336 / 416 | ≥ 74 (the tooth meets the belt) | ≥ 54 |

  The touch offsets use the `SolidObject` squeeze rule of `nightmare-press.md` [likely]. The ride
  is 26 px higher than X (top = y − 26, y 384 on the belt). When the ride is held between a floor
  and a solid above, `game+0x26` = 1 and the press stops and rises
  ([`ride-armor.md`](../players/ride-armor.md)) [certain for the code]. This is why the Ride Armor
  is the safe way through (Fandom: "Another Route").

## The Ride Armor

Record 30 (stage logic 30, `id` 1) puts `actor2` type 1 at (32, 354); it stands on the ledge at
(32, 352) (box bottom 352 + 3 + 29 = 384). In `shark-10_alt` it is empty: state 1/0/1, HP 32,
texture `0x0140`, CLUT `0x788C`, the same as the stage 4 ride [certain]. Type 1 (`0x800F01A4`)
is the overlay 06 code of type 0 after relocation, so it is the same mech
([`ride-armor.md`](../players/ride-armor.md)). It is the large red figure at the left of the
preview picture.

## The Nightmare Effect

Overlay 28 reads `nightmare_effect` only in the Nightmare Cube set-up (`0x800F0D38`, value 5)
[certain for the reads with the address `0x800CD30A`]. Stage 4 can have only the values 2 and 5
([`shark.md`](shark.md), "Nightmare Effect"), and the area keeps that value:

- 5: the three brown cubes (records 22–24) are live. In a stage ≥ 19 the cube code also rides the
  belt (`conveyor-belt.md`, "Other riders").
- 2 (this state): no object. Overlay 28 has no Nightmare Freeze code (main 35 of overlay 07).

## Talk zone (record 1)

`id` 132: optional, message 4. k = `talk_stage_idx`[41] = 16; box `talk_boxes` + 16 × 128 + 4 × 8 =
x 0–224, y 240–416, over the ledge and the start of the belt [certain]. In `shark-10_alt` X is in
the box: `mega.hint` = 4, and the HUD draws the "!" icon (a 16 × 16 sprite at screen (276, 196)
in the HUD list) [likely: the sprite texture was matched by eye]. `talk_seen` k 16 is 0: the user
did not open the call. Message 4 of member 233 (X): "...What happened? / Where am I now?" (Zero,
member 234: the same words).

## Camera and checkpoints

### Checkpoints

`checkpoint_start_tbl` entry 41 → `0x800732D8`: two records [certain].

| checkpoint | record | player (x, y) | camera (x, y) | limits x, y | facing | `beam_thru` |
|---|---|---|---|---|---|---|
| 0 | `0x80072FCC` | 79, 224 | 0, 224 | 0–2576, 224–224 | right | 1 |
| 1 | `0x80072FF0` | 3024, 224 | 2848, 224 (layer 2: 1424) | 2848–2848, 224–224 | right | 1 |

Checkpoint 0 is the arrival from the crystal: X beams in at x 79 and lands on the ledge (y 363 in
`shark-10_alt`). Checkpoint 1 is in the corridor: marker 1 (x 3026) sets it after the pass of gate
37. After a restart at checkpoint 1, the press and the forced scroll delete themselves in their
first update [certain for the code].

### Camera limits

| where | min x – max x | min y – max y | source |
|---|---|---|---|
| belt | 0–2576 (forced: min = max = camera x during the scroll) | 224–224 | checkpoint 0, `AutoScrollAlt` |
| corridor | 2848–2848 | 224–224 | gate 37 (case 37: 2848, 2848; push 0.83 px/frame) |
| boss room | 3152–3184 | 224–224 | gate 38 (case 38: X … X + 32, Y − 160) |

Max x 2576 + 320 = 2896: at the end of the scroll the view shows the block, the gate floor and gate
37 [certain for the numbers].

## The boss gates

| gate | x, y | open condition | new limits | time stamp |
|---|---|---|---|---|
| 37 | 2896, 384 | always | x 2848, push 0.83 | no |
| 38 | 3152, 384 | always | x 3152–3184, y 224 | no |

Both triggers delete the main objects (`ClearEnemies`; [`boss-gate.md`](../enemies/boss-gate.md))
[certain for the code]. Gate 37 also ends the press (X's x ≥ 2896) and, through the camera, the
belt [likely].

## The boss: Dynamo

- `boss_choice` is 2 and `boss_pack_tbl`[20] = 140 (Dynamo's data member) [certain for the bytes].
  The `BossSpawner` (record 29) is not live yet and overlay 45 is not loaded (`boss_pack_loaded` =
  0) [certain].
- With `boss_choice` 2 the spawner loads overlay 45 and makes main 63
  ([`dynamo.md`](../enemies/dynamo.md)); with 0 Zero Nightmare, with 1 High Max
  (`docs/engine.md`, "`boss_choice`") [certain for the code; no boss-room state of this area].
- Dynamo's texts for X are messages 2 and 3 of member 233: "Long time no see!" … "I will get the
  Souls that you collected!", and at the end "I still can't win! Bye, X!" [certain for the decode].
- Sprite banks 12–15 are types 53, 55, 55, 63 with no texture (the three possible bosses).

## How the area ends

As in the other hidden areas: Dynamo's death sets `game.exit_req` = `0x10`, and with `boss_choice`
2 `StageExit` goes to the report screen (`docs/enemies/high-max.md`, "Where the clear leads")
[certain for the code; no state after the fight]. The player does not go back to stage 4.

## The map drawing

`learn` stops on this state with the code at the time of writing:

```
Metal Shark Player, hidden area: the screens join less than the limit (edges 0.900/0.000), but the layout is the disc layout of overlay 28 @0x8b28; kept
error: texture block found at 0 VRAM positions; need exactly 1 (closest: (0, 0) with 2016 differing rows)
```

It took member 98 (stage 4 area 0): a stale copy of member 98's tile info is still in RAM, and the
tile info of member 118 was not found because the belt animation changes cell numbers above 255
(byte 3 of the 4-byte tile word), and `info_in_ram` masked only byte 2. With both bytes masked,
`learn` gives member 118, edges 0.986 / 1.000, 31 objects, palettes from 211 tiles, and the texture
block `0x10000` (in VRAM at (320, 256)) [certain].

## Sprite exports

`spriteexport` on `shark-10_alt` (scratch root `~/mmx-data/x6/re/shark-alt/sprites-root`, with
`"20.1": "shark-alt"` in the stage names):

- Banks: 0 type 128, 1 type 15, 2 type 141 (the stage 4 Rescue Reploid), 3 type 225 (Ride Armor),
  4 type 138 (junk block), 5 type 27, 6 type 1 (Junkroid, no record), 7 type 28 (crystal, no
  record), 8 type 147 (Nightmare Cube), 9–11 type 0, 12–15 types 53, 55, 55, 63 (no texture). The
  same bank list as stage 4 area 0, and the 9 bank sheets are byte-equal to those of
  `shark-1_ride` [certain].
- Exports: `ride-armor` (`actor2` type 1, live), `junkroid`, `mettaur-d2`, `nightmare-virus`,
  `nightmare-virus-orb` (search): the IDs that exist already. No new ID [certain].
- Skipped: type 28 (no table found), 0, 53, 63 (no texture), 55 (code not loaded).

## Open questions

1. States on the belt during the scroll, with the press low, on the ride under a tooth, at the
   gates and in the boss room (Dynamo against X).
2. The tooth touch offsets in play (the `SolidObject` squeeze rule).
3. A state with `nightmare_effect` 5 (the cubes on the belt).
4. Whether the press kills X on the block at x 2752–2815 (the formula says yes at offset ≥ 74).
