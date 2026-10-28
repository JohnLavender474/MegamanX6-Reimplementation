# Nightmare Press (stage 06, kind-7 object type 2): behaviour sheet

The Nightmare Press is the garbage compactor that forms the ceiling over most of Metal Shark Player's
stage (the Recycle Lab). It goes down 80 px, waits, and goes up again, all the time. When it meets
the head of X (or Zero) while the player is on the floor, the player dies at once. The player is
safe in the low parts of the floor. A Ride Armor under the press stops it and sends it back up.
"Nightmare Press" is the name on the Mega Man Fandom page of the Recycle Lab.

The press is three parts [certain]:

- the **art**: BG layer 1 (`art 1 decoration` in `stage.ora`). The press moves the layer camera;
- the **controller**: kind-7 object type 2 (record kind 7 makes it), code in the main program at
  `0x80046E14`. It moves layer 1 and does the crush test;
- the **solid body**: stage object type 9 of overlay `06`, 15 objects of 240 × 128 px. They follow
  layer 1 and are solid objects (`SolidObject`).

Section 2 (stage 4 area 1, overlay `07`) has a second press of the same code, with `var` 16 and
lower bodies ("teeth") under it. See "Section 2" below.

Overlay `06` (Metal Shark Player, stage 4), loaded at `0x800E9860`. Save state
`shark-1_ride.state.gz` (stage 4, area 0; X in a Ride Armor; the press waits at the bottom). Kit:
`python3 -m mmxdemake.rekit --state shark-1_ride.state.gz --type 9 --class stage --name shark-crusher`
(the kit finds only the type 9 entry; the press was found from the layer-1 camera, see the notes in
[`~/mmx-data/x6/re/shark-crusher/notes.md`](../../work/shark-crusher/notes.md)). Units, tags and the shared rules: `docs/engine.md`. Records:
`docs/stages/shark.md`.

## The kind-7 class [certain for the code]

`docs/engine.md` does not list this class yet:

| class | slots | size | base | update table | loop | made by |
|---|---|---|---|---|---|---|
| kind-7 objects | 4 | `0x30` | `0x800C4778` | `0x8007136C` (5 entries) | `0x800208EC` | record kind 7 (`0x8002C950`) |

The loop runs after `WeaponLoop` and before the main objects; while `game+0x19` is not 0 it runs only
the objects with flag bit `0x08`. In stage 4, type 2 is the press; types 0, 1, 3 and 4 point into
overlay `06` code that is not a function start (not used here) [likely]. With overlay `07` (area 1)
loaded, type 3 (`0x800E9C98`) is the forced scroll of the mini-boss room
(`docs/enemies/conveyor-belt.md`); types 0 (`0x800E9B90`), 1 (`0x800E9E4C`) and 4 (`0x800E9864`)
are not used by the area-1 records [certain for the records].

## Spawn and set-up

- Record: `fixed_set_tbl` of stage 4 area 0, record 59: kind 7, type 2, `id` 32, at (0, 0)
  (`0x800F300C`). It is made when the area loads.
- `var` (the record `id`) = 32: the layer-1 offset at the rest position, in px.
- No sprite, no box, no HP. It cannot be hit or destroyed. Its body (type 9, below) has contact damage
  127 but no weapon test (`ContactWeapon` is not called) [certain for the code].

### Fields

| field | meaning |
|---|---|
| `+0x04` | state: 0 set-up, 1 run |
| `+0x05` | sub-step 0–5 (table `0x800762C8`) |
| `+0x14` | timer (updates) |
| `+0x16` | count: updates of movement left |
| `+0x18` | speed, 16.16 px per update, positive = down |
| `+0x1C` | offset, 16.16 px (`+0x1E` = px), positive = lower |
| `+0x20` | the offset saved at the last stop |

## The update (`0x80046E14`) [certain for the code]

In order, each update:

1. Stage test: stage ≥ 19: only at checkpoint 0. Stage < 19: only in stage 4, or at checkpoint 2.
   `mega+0xE0` = 2: nothing. Value 2 means "the player does a screen-wide special attack" (see
   "`mega+0xE0`" below).
2. Layer-1 old y (`0x80097266`) = layer-1 y.
3. The state (table `0x800762C0`), then the sub-step.
4. Layer-1 y = layer-0 y + `var` − offset. So the press is drawn `offset − 32` px below its place in
   the layout.
5. **Carry up:** in player step 31 with sub-step < 4 (super jump, grab, ceiling hold) or step 63
   (attack in the ceiling hold): when layer-1 y went up by dy > 0 px (the press rises), the player's
   y −= dy.
6. **The kill line** (a world y) and the x limits, by stage and area. Layer-1 y was just set to
   layer-0 y + `var` − offset, so layer-0 y − layer-1 y = offset − `var`:

   | case | kill line | the test runs when | the press is deleted when |
   |---|---|---|---|
   | stage 4 area 0, checkpoint 0 | layer-0 y − layer-1 y + 256 = offset − 32 + 256 = **offset + 224** | player x < 4993 | player y ≥ 1280, or checkpoint ≥ 2 |
   | stage 4 area 0, checkpoint 1 | … + 736 = offset + 704 | player x < 5057 | (same) |
   | stage 4 area ≠ 0 (`var` 16) | … + 240 = offset − 16 + 240 = **offset + 224** | player x < 4257 | player x ≥ 4480, or checkpoint = 1 (the code compares with 1) |
   | stage ≥ 19 (stage 20 area 1, overlay 28: see "Other users") | … + 240 | always | player x ≥ 2896, or checkpoint 1 |
   | another stage, checkpoint 2 (stage 18 area 0, overlay 24) | … + 2001 / 2065 / 2097 | player x in 1857–1919 / 2304–3776 / 4240–6016 | checkpoint ≥ 3 |

   There is no lower x limit in stage 4. All cases first need a player terrain box
   (`mega+0x68` ≠ 0); else the test is skipped.
7. **The crush test** (`0x80047238`): top = player y + box `oy` − box `h` (the top of the terrain box:
   X standing y − 17, X dashing y − 9). When kill line ≥ top:
   - `mega+0xE6` (squeeze damage) = 127;
   - player y += kill line − top (the player is pushed down out of the press);
   - `mega+0x71` |= `0x04` ("blocked above by an object").

   The test does not look at the floor, the invulnerability (`mega+0x61`), `no_hit` or the squeeze
   timer `mega+0xE5`. It does not kill. The kill comes in the next update (below).

## The cycle (sub-steps) [certain for the code]

Speeds: "slow" = 1.0 px/update when the difficulty (`game+0x68`) < 2, else 1.25. Each move is 80 px:
`0x500000 / |speed|` updates (80 or 64).

| sub | what | speed | ends | then |
|---|---|---|---|---|
| 0 | intro: waits while the player step is 0 (beam-in), then moves down | +2.0 for 8 updates (16 px); stage ≥ 19: 16 updates (32 px) | count 0 | speed + slow, timer 180, sub 1 |
| 1 | wait | 0 | timer 0 | count = 80 px; speed > 0: sub 2; else sub 3 and sound (2, 34) |
| 2 | fall | + slow | count 0: **the slam**: `Quake(60, 2, 2)`, sound (2, 35), `Rumble(5)` | speed −1.0, timer 40, saved = offset, sub 4 |
| 3 | rise; `game+0x27` = 1 in each update | − slow | count 0 | speed + slow, timer 50, sub 1 |
| 4 | wait at the bottom | 0 | timer 0 | speed − slow, timer 120 (checkpoint 0) or 50, offset = saved, sub 1 |
| 5 | hold (after a Ride Armor stop) | 0 | timer 0 | speed − slow, offset = saved, sound (2, 34), sub 3 |

`PressRun` (state 1) clears `game+0x27` before the sub-step, so `game+0x27` = 1 only in the updates of
a rise.

So, at checkpoint 0 on Normal: top (offset 16) → 80 updates down → 40 + 120 = 160 updates at the
bottom (offset 96) → 80 updates up → 50 updates at the top → … A cycle is 370 updates (Hard: 338). The
first top wait is 180 updates. At checkpoint 1 the bottom wait is 40 + 50 = 90 updates.

In stage 4 area 0, checkpoint 0, the kill line goes from y 240 (top) to y 320 (bottom). In the save
state the press waits at the bottom (offset 96, timer 114, speed −1.0) [certain].

**The Ride Armor stop** (`game+0x26` ≠ 0, see below):

- in sub 2 (falling): `Quake(60, 2, 2)`, sound (2, 35), `Rumble(5)`, count = the updates moved so far,
  timer 50, sub 5. After 50 updates it rises by the same distance, back to its top;
- in sub 1 (waiting): `Quake(60, 2, 2)`, count = 80 px, timer 50, sub 5. Then it rises 80 px. When
  this comes at the top, the press goes 80 px **above** its normal top and stays 80 px higher from
  then on (its cycle is relative) [certain for the code; likely the "press goes higher" Ride Armor
  trick of the TAS forums].

## The solid body (stage object type 9, overlay 06)

- Records: `fixed_set_tbl` stage 4 area 0, records 9–58 (kind 4, flags `0x30`). The press body is
  `var` 0: 15 objects at x 1576 + 240 n (n = 0–14), y 192; `var` 1: 21 objects at x 200 + 240 n,
  y 672 (the lower press, checkpoint 1). `var` 67, 69, 98, 99, 101, 102 (x 1120–4560, y 752–784): blocks
  of the lower part [certain for the records].
- Set-up (`0x800E9878`): waits while the player step is 0; flags `0x41`; contact damage (`+0x60`) 127;
  damage table `0x80075B9C`; animation table `0x800F35A0` (2 animations; the object is never drawn,
  `display` is cleared each update).
- Boxes [certain]: `var` < 10 from the tables `0x800F35D4` (terrain) and `0x800F35A8`:
  `var` 0 and 1: terrain (0, −1, 120, 64) = **240 × 128 px**, centre 1 px above the origin; `var` 2:
  (0, 0, 120, 96). `var` 112: terrain (0, 0, 32, 8), attack box `0x800F35D0` (−32, −9, 64 × 18). Other
  `var`: terrain half sizes (high nibble × 8, low nibble × 8), for example `var` 67 = 64 × 48 px.
- Follow (`0x800E9D5C`): y = y0 + layer-0 y − layer-1 y. In the save state: 192 + 224 − 160 = 256.
  So the body bottom is offset + 223, 1 px above the kill line [likely: the edge rounding was not
  checked].
- Update (`0x800E9A30`), in order:
  1. Stage 4: nothing when y ≥ 672 at checkpoint 0 in area 0 (the lower press waits for checkpoint 1).
  2. `SolidWeaponHit` (`0x80032B34`, below); the follow sub-step (not while `mega+0xE0` = 2);
     `DisplayCheck(160, 160)`.
  3. In area 0 with `var` 0: always; else only within 160 px of the player in x and on screen:
     `SolidObject` (not while `mega+0xE5` ≠ 0). Then, when `game+0x27` ≠ 0 (the press rises), the
     player is under the body (touch `0x04`) and on the floor (`mega+0x70` bit `0x08`):
     `mega+0x71` = 0. So a rising press cannot squeeze. Then a side fix: when the player touches a
     side, the code tries the player's y − `speed_y` and keeps it when there is no contact.
  4. `0x80032BE8`, `0x8003208C`, `0x80031F4C` (below); `display` = 0.
- Delete: area 0, `var` 0 at checkpoint > 0; other `var` at checkpoint ≥ 2. Area ≠ 0: player
  x ≥ 4480 or checkpoint > 0; `var` 112 also calls `ContactMega` when the player is under it.

### The four other solid helpers [certain for the code]

The body calls four main-program helpers each update. Each loops over one object pool and makes the
body a wall or a floor for those objects. Junk blocks and Nightmare Cubes call the same helpers
(`docs/enemies/junk-block.md`, `docs/enemies/nightmare-cube.md`).

| helper | pool | what it does |
|---|---|---|
| `SolidWeaponHit` (`0x80032B34`) | the 32 weapon objects (player shots, `0x800950A0`) | for each live shot whose terrain box overlaps the body's terrain box: the hit point (`0x800312DC`), then `ApplyWeaponHit(body, shot)`. It returns the first result that is not 0 or 126; when no shot hits, it clears the body's hit tag `+0x65`. The body has `dmg_tbl_capsule`: class 7 for the buster and most weapon ids. **Class 7 stops the shot as a wall does**: `ApplyWeaponHit` sets the shot's `+0x70` \|= `0x0F` and returns 127; no damage, no sound. Class 2 (the sabre ids) has no effect. So shots end on the press, and the sabre goes through |
| `0x80032BE8` (`SolidStopShots`, proposed) | the 32 visual objects (enemy shots, `0x80093C98`) | for each live enemy shot whose attack box touches the body's hurt box (`0x8002D868`, with the hit point): the shot's `+0x70` \|= `0x0F` (blocked on all sides). It returns the type of the last one. The body's hurt box is `0x800F35A8` (live in the states). So enemy shots also end on the press |
| `SolidPushShots` (`0x8003208C`) | the visual objects, then the weapon objects | only objects with `+0x9D` ≠ 0: `SolidPush(body, obj)`. One bit per object in `+0x90` (visual) and `+0x94` (weapon) keeps "it stands on the body" (`0x8002E47C` returns 8); while the bit is set, the body carries it (`0x80032C88`). `+0x9D` is written by the code of weapon id 2 (`0x801EEEF0`, X's overlay; slot 2, Ice Burst [likely]) and of weapon id 79 (`0x800446A4`) [certain for the stores] |
| `0x80031F4C` (`SolidPushEnemies`, proposed) | the 48 main objects (enemies) | only objects with a terrain box: `0x800323D4` pushes the enemy out of the body. One bit per enemy in `+0x98` (2 words) keeps "it stands on the body" (`0x8002E664` returns 8), and `0x80032D24` carries it. When the enemy is then blocked on both sides (`+0x70` & `0x0C` = `0x0C`, or & `0x03` = `0x03`), its x and y go back to the values before the push: the body never squeezes an enemy into a wall |

So the press is a full solid: it stops the player's shots and the enemy shots, and it pushes and
carries the enemies.

`SolidObject` alone can also kill: a body touched from below sets `mega+0x71` bit `0x04` and
`mega+0xE6` = 127 (`SolidTouch`), and a player centre inside the body sets `mega+0x71` = 15
(`SolidPush`) [certain for the code].

## How the player dies [certain for the code]

The frame order (`0x80020C40`): the ride object (`0x80020A08`) → the player (`0x80034DCC`) →
`WeaponLoop` → **the press** (`0x800208EC`) → the main objects → … → **the stage objects** (type 9) →
… → `StageLogicLoop` → `CollideCheck(mega)` → the layer cameras.

Update N:

1. The player step runs; its `CollideCheck` sets the terrain flags `mega+0x70`. On the floor: bit
   `0x08`. After the step, `PlayerUpdate` clears `mega+0x71` and `mega+0x72`.
2. The press moves 1 px (1.25 on Hard) and does the crush test: when its kill line is at or below
   the top of the player's terrain box, `mega+0xE6` = 127, the player is pushed down by the overlap,
   `mega+0x71` |= `0x04`.
3. The type 9 body: `SolidObject` gives the same `0x04` and 127 when the boxes touch (not while the
   press rises).

Update N + 1, in `PlayerUpdate` (`0x80034FBC`):

4. `mega+0x89` = `mega+0x70` | `mega+0x71` = `0x08` (floor) | `0x04` (press) = `0x0C`.
5. `PlayerHitCheck` (`0x80039228`): (a) terrain crush: `mega+0x70` & `0x0C` = `0x0C` or `& 0x03` =
   `0x03`: not here (the press is not a tile). (b) **squeeze** (`0x800392CC`): `mega+0x89` & `0x0C` =
   `0x0C`: `no_hit` = 0, `mega+0xE5` = 75, HP = (HP & `0x7F`) − `mega+0xE6` = HP − 127 ≤ 0 →
   HP = `0x80`. The squeeze does not test the invulnerability (`+0x61`), and 127 is not scaled by the
   difficulty or the armour. (c) Death (HP = `0x80`): HP 0, state 2, in the same update.

So it is an **instant death**, not damage: the maximum HP is far below 127. Without the floor bit (the
player in the air), the press only pushes the player down; the player dies in the first update after
landing while the press still touches the head. A dash lowers the box top by 8 px.

The floor under the press (collision value 56; area 0, checkpoint 0): tops at y 320, 352, 368, 384 and
416. The kill line reaches 320. X standing on floor y F has the head at F − 38 (dashing F − 30):

| floor top | standing X dies at offset | dashing X dies at offset |
|---|---|---|
| 320 | ≥ 58 (during the fall) | ≥ 66 |
| 352 | ≥ 90 (the last 6 px) | never (needs 98) |
| 368, 384, 416 | never | never |

So the safe places are the floors at y 368 and lower [certain for the arithmetic; likely ±1 px for the
box edges].

## The Ride Armor case [certain for the code]

While the player rides (`mega+0xD5` < 0), the player has no terrain box (`mega+0x68` = 0):

- the press skips its crush test (step 7 above needs a terrain box);
- `SolidObject` of the type 9 body skips the player (`SolidPush` returns at once for a body without a
  terrain box) and acts on `actor2` (`0x800CD340`, the ride) instead: push, carry, `SolidTouch` with
  the flags in the ride's `+0x71`. While the player is in step 18, it copies the ride's x and y to the
  player;
- the ride update (actor2 type 0, `0x800F2414` in overlay `06`, table `0x80071380`; it runs first in
  the frame): ride `+0xA8` = `+0x70` | `+0x71`, `+0x71` = 0, the ride state, ride HP &= `0x7F`, then
  **`game+0x26` = 1 when `+0xA8` & `0x0C` = `0x0C`** (the ride has the floor under it and the press on
  it), else 0;
- the press reads `game+0x26` in sub-steps 1 and 2 and stops and rises (above).

So X in a Ride Armor is never crushed: the ride holds the press up. This works also for an empty ride
(the player out of it). The ride box (`0x800F3A44`) is (0, 3, 15, 29): 30 × 58 px, top y − 26. The
ride on floor y F touches the press at offset ≈ F − 281: floor 320 at offset 39, 352 at 71, 368 at 87;
floors 384 and 416 are too low for a touch. The ride takes no damage from the press in
`0x800F2414` (it does not read `mega+0xE6`); the ride states were not read [likely].

## Sounds

| bank, id | when |
|---|---|
| 2, 34 | a rise starts (sub 1 → 3, sub 5 → 3) |
| 2, 35 | the slam, and a Ride Armor stop in the fall |

Also `Quake(60, 2, 2)` and `Rumble(5)` at the slam and at a stop.

## Section 2 (stage 4 area 1, overlay 07)

Save states `shark-2_section-2` (X just spawned, the press at the top) and `shark-3_press-low` (the
press at the bottom, X alive under it). Notes and probes: [`~/mmx-data/x6/re/shark-conveyor/`](../../work/shark-conveyor/). The
belt under this press: `docs/enemies/conveyor-belt.md`.

### Records [certain]

`fixed_set_tbl` of stage 4 area 1 (`0x800F7E58`):

| records | kind, type | `var` | x | y (record) | what |
|---|---|---|---|---|---|
| 6 | 7, 2 | 16 | 0 | 0 | the press controller |
| 7–25 | 4, 9 | 0 | −120 + 240 n (n 0–18) | 176 | press bodies, 240 × 128 px, x −240 to 4319 |
| 26–42 | 4, 9 | 66–70, 99 | 288–4128 | 256–288 | the teeth (table below) |
| 43–46 | 4, 9 | 112 | 1568, 2912, 2976, 3376 | 296, 296, 296, 280 | bodies with a contact attack |

### The same press, another `var` [certain]

- The code is the same `PressUpdate`. Only `var` differs: 16 (area 0: 32). So layer-1 y = layer-0 y
  + 16 − offset, and at the top (offset 16) layer 1 is at its layout place.
- The kill line constant of area ≠ 0 is 240 (area 0: 256). Both are 16 less, so the world kill line is
  the same: **offset + 224**, y 240 at the top and y 320 at the bottom.
- The body (overlay 07 stage type 9, entry `0x800EE3F8`, states `0x800F8CD8`) has the same logic as
  in overlay 06: y = record y + layer-0 y − layer-1 y = 176 + offset − 16; `var` 0 box (0, −1, 120,
  64), so the bottom is offset + 223, as in area 0. In area ≠ 0 `SolidObject` runs only within 160 px
  of the player in x and while on screen; `var` 112 also calls `ContactMega` when the player is under
  it (touch `0x04`). The bodies are deleted when the player x ≥ 4480 or the checkpoint > 0.
- The crush test runs only while the player x < 4257. The press is deleted at x ≥ 4480 or checkpoint
  1. So the section-2 press always runs at checkpoint 0: the bottom wait is 40 + 120 = 160 updates,
  and the cycle is the same as in section 1 (370 updates on Normal, 338 on Hard; the first top wait
  180 after the 16 px intro).

### The check in `shark-3_press-low` [certain]

| value | number |
|---|---|
| press | state 1, sub 1 (wait), timer 117, speed −1.0, offset 96, saved 96: the bottom wait of checkpoint 0 (timer 120), 43 updates after the slam |
| layer-1 y | 144 = 224 + 16 − 96 (the press used layer-0 y 224; the quake moved layer-0 y to 226 after it) |
| kill line | 224 − 144 + 240 = **320** (offset + 224). The quake does not move it, because layer 1 follows layer 0 |
| X | (1044, 347), step 2, stand box (0, 2, 11, 19), HP 42 |
| top of X's terrain box | 347 + 2 − 19 = **330** (the floor is at y 368) |
| `var` 0 body over X | y 256, bottom 319 |

320 < 330, so the press does not reach X: X is 10 px below the kill line. The rule held. X stands on
the floor at y 368, which is a safe floor (the table above: floor 368 is never reached). X is at x
1033–1055, in the 64 px gap between a tooth (`var` 67, x 928–991) and the step to floor 320 at x 1056.
In `shark-2`: offset 16, timer 94 of the first top wait, speed +1.0, kill line 240 [certain].

### Floor tops under the section-2 press [certain for the cells]

| x | floor y | x | floor y |
|---|---|---|---|
| 0–319 | 416 (belt) | 2208–2271 | 384 |
| 320–383 | 368 | 2272–2335 | 416 (belt) |
| 384–479 | 416 (belt) | 2336–2399 | 352 |
| 480–543 | 368 | 2400–2463 | 368 |
| 544–767 | 416 (belt) | 2464–2527 | 352 |
| 768–863 | 320 | 2528–2655 | 416 (belt) |
| 864–1055 | 368 | 2656–2783 | 320 |
| 1056–1151 | 320 | 2784–3007 | 416 (belt) |
| 1152–1599 | 416 (belt) | 3008–3503 | 368 |
| 1600–1663 | 368 | 3504–3711 | 320 |
| 1664–1727 | 384 | 3712–3839 | 384 |
| 1728–1791 | 368 | 3840–3967 | 352 |
| 1792–1999 | 416 (belt) | 3968–4031 | 416 (belt) |
| 2000–2079 | 352 | 4032–4095 | 352 |
| 2080–2207 | 416 (belt) | 4096–4479 | 416 (belt) |

Under the `var` 0 bodies the section-1 table applies: floor 320 kills a standing X at offset ≥ 58,
floor 352 at offset ≥ 90; floors 368 and lower are safe.

### The teeth (type 9 `var` 66–70, 99, 112)

They follow layer 1 as the main bodies do (80 px down and up). Box half sizes from `var` (high nibble
× 8, low nibble × 8) [certain for the live boxes]. A standing X on floor F has its box top at F − 38;
a tooth touches it when its bottom reaches that y [likely, ±1 px: the `SolidObject` squeeze rule,
not seen in a state].

| `var` | x | size (px) | bottom: top / bottom of the cycle | floor under it | a standing X is touched at offset |
|---|---|---|---|---|---|
| 67 | 256–319 | 64 × 48 | 288 / 368 | 416 | never |
| 69 | 608–671 | 64 × 80 | 320 / 400 | 416 | ≥ 74 |
| 67 | 928–991 | 64 × 48 | 288 / 368 | 368 | ≥ 58 |
| 99 | 1280–1471 (2) | 96 × 48 | 288 / 368 | 416 | never |
| 67 | 1536–1599 | 64 × 48 | 288 / 368 | 416 | never |
| 67 | 1728–1791 | 64 × 48 | 288 / 368 | 368 | ≥ 58 |
| 70 | 1856–1919 | 64 × 96 | 336 / 416 | 416 | ≥ 58 (the tooth meets the floor) |
| 68 | 2272–2335 | 64 × 64 | 304 / 384 | 416 | ≥ 90 |
| 66 | 2464–2527 | 64 × 32 | 272 / 352 | 352 | ≥ 58 |
| 99 | 2848–3135 (3) | 96 × 48 | 288 / 368 | 416 and 368 | never on 416; ≥ 58 on 368 |
| 66 | 3344–3407 | 64 × 32 | 272 / 352 | 368 | ≥ 74 |
| 67 | 3776–3839 | 64 × 48 | 288 / 368 | 384 | ≥ 74 |
| 68 | 3968–4031, 4096–4159 | 64 × 64 | 304 / 384 | 416 | ≥ 90 |
| 112 | 1536–1599, 2880–3007 | 64 × 16 | 304 / 384 | 416 | ≥ 90, and `ContactMega` |
| 112 | 3344–3407 | 64 × 16 | 288 / 368 | 368 | ≥ 58, and `ContactMega` |

So in section 2 the low floors are not always safe: a tooth fills part of the gap.

## `mega+0xE0`: the player's special-attack state [certain for the writers]

The press (step 1 of the update), its bodies (the follow), the belt (`BeltUpdate`), `AutoScroll`,
the rocket platform and the Nightmare Cube do nothing while `mega+0xE0` = 2; Weather Analyze drains
only while it is 0. The writers:

| value | written by | when |
|---|---|---|
| 2 | `0x8003FD18` (main program) | X enters step 41, the pose of shot type 17 (the charged slot 8 weapon; `docs/players/steps-extra.md`): animation 95, no terrain box, `no_hit` = 1, `game.player_lock` = 1 |
| 2 | `0x8003E51C`, `0x8003E580` (main program) | X enters step 36, 37 or 77 (the armour special attacks with the screen fade; animations 185, 186) |
| 1 | `0x8003E5DC` (main program) | X enters step 34 (the special attack that moves 192 px forward) |
| 2 | `0x801EA744` (Zero's overlay) | Zero starts the skill `0x80` special attack (step 59: no terrain box, lock, `no_hit`, the screen fade) |
| 0 | `PlayerHitCheck` (`0x8003930C`), Zero's overlay (5 places), the teleporter crystal (overlay 06 `0x800EC328`) | the end of those attacks, a hit, a teleport |

So 2 = "a screen-wide special attack is running": the player has no terrain box and cannot be hit,
and the moving hazards stop so that they cannot push or kill the player in that time. 1 marks the
moving special attack (step 34) and does not stop the press [certain for the code; the names of the
attacks are not in the code].

## Other users of `PressUpdate` [certain for the records]

A scan of the second record list (`fixed_set_tbl`) of every stage and area against every overlay
finds the press controller (kind 7, type 2) in four lists:

| stage, area | overlay | record `id` (`var`) | case in the update |
|---|---|---|---|
| 4, 0 | 06 | 32 | stage 4 area 0 |
| 4, 1 | 07 | 16 | stage 4 area ≠ 0 |
| 18, 0 | 24 | 0 | "another stage, checkpoint 2" |
| 20, 1 | 28 | 32 | stage ≥ 19 |

- **Stage 20 area 1 (overlay 28)** is the hidden area ("Another Route") of Metal Shark Player's
  stage: the exit `0x41` of stage 4 goes to stage 19 + (4 − 1) / 2 = 20, area (4 − 1) & 1 = 1
  (`docs/stages/shark.md`, "The teleporter crystals"). Its lists also have 24 press bodies (stage
  object 9), a kind-7 type 4 object, the Ride Armor place (stage logic 30, `id` 1: `actor2` type 1),
  junk blocks, Nightmare Cubes and boss gates. The Fandom page of the stage says that in the Another
  Route "the Nightmare Press and conveyor belt are active, and the screen auto-scrolls" [Fandom]. So
  the stage ≥ 19 case of the kill line (+ 240, deleted at x ≥ 2896 or checkpoint 1) is that area.
  The save state `shark-10_alt` confirms it ([`docs/stages/shark-alt.md`](../stages/shark-alt.md),
  "The press"). The set-up (`0x800472D0`) gives a stage ≥ 19 a 32 px intro (count 16), so the top
  rest is offset 32 and the kill line is offset + 208 = y 240–320, the same world line as in stage 4.
  In `shark-10_alt`: sub 1, timer 143, offset 32, layer 1 y 224 [certain]. The teeth there (`var`
  66, 68, 70, 82) and the safe floors are in the stage page.
- **Stage 18 area 0 (overlay 24)**: the press with `var` 0, only at checkpoint 2, with three x
  ranges. The Fandom Nightmare page lists the Nightmare Press in "Secret Lab 2 (as Zero)" [guess:
  stage 18 is that part of the Secret Lab; not checked].

## Summary for a reimplementation

1. The press is a ceiling layer with a solid body 240 px wide per piece, x 1456–5056, 128 px high.
2. Cycle: down 80 px at 1 px/update (1.25 on Hard), wait 160 (40 + 120), up 80 px, wait 50; the first
   wait 180 after a 16 px intro drop. Slam: screen shake 60 updates, sound, rumble.
3. Each update after it moves: when its bottom (+1 px) is at or below the top of the player's box,
   push the player down by the overlap and flag "blocked above".
4. A player with "blocked above" and "on the floor" in the same update dies at once (no
   invulnerability, no damage scaling).
5. A Ride Armor (or any solid ride) with the press on it and the floor under it stops the press; the
   press waits 50 updates and goes back to its top.
6. No kill while the press rises.
7. Section 2: the same press and the same kill line (y 240–320), bodies x −240 to 4319, plus
   lower "teeth" that move with it; it stops at player x 4480 or checkpoint 1.

## Open questions

- (answered) `0x80032B34`, `0x80032BE8`, `0x8003208C`, `0x80031F4C`: "The four other solid
  helpers".
- (answered) `mega+0xE0` = 2: the screen-wide special attack ("`mega+0xE0`").
- `layer0+0x34` = 0 when `layer0+0x20` ≠ `layer0+0x22` (each update of the press): effect not read.
- The stage ≥ 19 case is stage 20 area 1 (save state `shark-10_alt`, the press in its first top
  wait) and the "checkpoint 2" stage is stage 18 area 0 ("Other users"; no save state). (Area 1 of
  stage 4: see "Section 2".)
- Section 2: the tooth kill offsets come from the `SolidObject` rule; check in play.
- The 1 px between the body bottom and the kill line: check in play.
