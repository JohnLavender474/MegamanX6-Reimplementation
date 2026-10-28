# Blizzard Wolfang (stage 04, main object type 54): behaviour sheet

<!-- Proposed file: docs/enemies/blizzard-wolfang.md. The room part ("The boss room") can also go
into a future docs/stages/wolfang.md (docs/TODO.md asks for that page); doc-changes.md has the text. -->

Blizzard Wolfang is the wolf Maverick of the ice stage (stage 2). He fights on the ice floor, on the
walls and on the ceiling of a closed room. He dashes and jumps on the floor, leaps to a wall and
dives at the player, bounces from wall to wall, slides on the floor and on the ceiling, and spits
ice balls. At boss level 2 and 3 his slides and wall hits leave ice spikes on the floor, the walls
and the ceiling. At half HP he changes to a second set of move sequences, with the ceiling moves.
His weakness is Magma Blade (slot 3): ids 3, 12 and 34 stun him.

Overlay `04` (Blizzard Wolfang), save states `wolfang-6_boss-warning.state.gz` (the WARNING) and
`wolfang-7_boss-fight.state.gz` (the first move of the fight); stage 2, area 0, checkpoint 5. Kit:
`python3 -m mmxdemake.rekit --state wolfang-7_boss-fight.state.gz --type 54 --name blizzard-wolfang`.
Units, confidence tags and the shared engine rules: `docs/engine.md`. Record: record 66 of member 96
(kind 0, `b` 54, `id` 0) at (6485, 416), the last record. "The room" is the boss room; positions
are given from the camera's target minimum x and y (`camera_layer0` `+0x26`, `+0x2A`): (6320, 304)
in the save states. The other objects of the fight are main type 54 with other `var` values (ice
spikes, trail images), visual type 20 (ice balls, debris, wall runners) and effect type 8 (the ice
that forms in front of the mouth).

## Spawn and set-up (`WolfangInit`, `0x800F14B0`) [certain]

- All `var` values: contact damage 4, damage table `dmg_tbl_wolfang` (`0x8007519C`), HP 0, no
  hit timer, draw priority 5, layer = `mega.layer`, animation table `0x800F7478`, speeds and
  gravity 0, no boxes. Stage 12: sprite data from `ray_table+4` entry 0 and palette `0x7948`.
- **`var` 0, the boss:** texture `0x140`, animation 0, `flip` 0 (left). `+0x8A` = −1 (no move
  table yet), `+0x8B` = `+0x8C` = 0, fight timer `0x800F7840` = 0. Attack box `0x800F70B4` and
  hurt box `0x800F70B8` = (−16, −16, 32, 42); terrain box `0x800F70B0` = (0, 0, 20, 32), a 40 × 64
  px body. Display 0 (not drawn). State 1, step 2 (the entry).
- **HP: 48 + a level bonus.** The entry sets `boss_hp_max` (`game+0x8C`) = 48 + {0, 2, 4, 6}[level]
  (bytes at `0x800E9B2C`), level = `game+0x8F` from `BossLevel` (the same rule as Commander Yammark,
  `docs/enemies/commander-yammark.md`). The HP fills from 0 at the end of the entry. Both states:
  level 0, `game+0x8D` = 4; `wolfang-7` has `game+0x8C` = 48 and HP 48 [certain].
- `var` 1: a trail image (below). `var` 16, 17, 18: ice spikes (below). Each is made by the boss.

## The `id` parameter

The record `id` is 0. Other `var` values come only from the boss code.

## State machine [certain for the code]

Entry `0x800F2E8C`: `prev_x`, `prev_y` = x, y; the state from `0x800F7500`; then `CollideCheck`.

| state | function | use |
|---|---|---|
| 0 | `0x800F14B0` | set-up |
| 1 | `0x800F1738` | the boss: main |
| 2 | `0x800F2EE8` | the boss: death (steps `0x800F7550`) |
| 3 | `0x800F30A4` | `var` 1: trail image |
| 4 | `0x800F31A4` | `var` 16–18: ice spike |
| 5 | `0x800F3368` | `DeleteObject3` |

### Main (state 1, `0x800F1738`) [certain]

1. `AnimeTick`; the step from `0x800F7518` (14 entries).
2. The boxes by sprite (`0x800F4FD8`): sprites 7–12, 26–29, 91, 96, 97 (the wall and ceiling
   poses) use `0x800F70BC` / `0x800F70C0` = (−16, −26, 32, 42); sprite 25 (the stun pose, flat)
   uses `0x800F70C4` / `0x800F70C8` = (−64, −16, 96, 32); all other sprites the set-up boxes.
3. The fight timer `0x800F7840` += 1, except in step 2 (the entry) and for `var` ≠ 0.
4. `+0x8D` = 0. `ContactWeapon`:
   - a kill: `mega+0x7A` = 1, state 2;
   - a hit: damage table `dmg_tbl_deflect` and hit timer `+0x88` = 30 (2 for weapon ids 24 and 25,
     the first two sabre hits). **Weapon ids 3, 12, 34**: `+0x88` = 10000 and step 3 (stun);
   - blocked (127): `+0x8D` = 1 (no reader found).
5. While `+0x88` > 0: −1 each update; the palette bit 15 is on when bit 1 of the old value is set
   (a flash of 2 updates on, 2 off). At 0: `dmg_tbl_wolfang` again, flash off. So a hit gives 30
   updates in which every shot is deflected.
6. `ContactMega` in each update, also during the hit timer.

The step table (`0x800F7518`):

| step | function | what |
|---|---|---|
| 0 | `HitReactionDispatch` | engine hit reaction (no writer here) |
| 1 | `0x800F4FD0` | empty (no writer) |
| 2 | `0x800F48D4` | entry (12 sub-steps, `0x800F763C`) |
| 3 | `0x800F4DF8` | stun |
| 4 | `0x800F3388` | stand 120 updates (no writer found: not used [likely]) |
| 5 | `0x800F343C` | dash |
| 6 | `0x800F3648` | jump |
| 7 | `0x800F383C` | leap to a wall, then dive |
| 8 | `0x800F3CBC` | ice slide on the floor |
| 9 | `0x800F3E74` | leap to the player's wall, bounce down |
| 10 | `0x800F4094` | ice balls |
| 11 | `0x800F41C0` | jump to the ceiling |
| 12 | `0x800F43C4` | ice balls from the ceiling |
| 13 | `0x800F45B8` | ceiling slide, then bounce down |

### The next move (`WolfangNextMove`, `0x800F25A0`) [certain]

Each move ends with this function. It sets contact damage 4 and sub-step 0, then takes the next
step from a **move sequence**: a row of 8 bytes, ended by `0xFF`. `+0x8B` is the row (RNG % 16),
`+0x8C` the place in the row.

- `+0x8A` = −1 (the first call, at the end of the entry): a row of table 1, `+0x8A` = 0.
- `+0x8A` = 0 and HP ≤ `boss_hp_max` / 2 (the first time): a row of table 2 at once, `+0x8A` = 1.
  The rest of the old row is dropped.
- Else the next byte of the row; at `0xFF` a new random row of the same table.

| table | rows | sequence (steps) |
|---|---|---|
| 1, `0x800F6FB0` (HP above half) | 0–4 (5 in 16) | 5, 5, 6: dash, dash, jump |
| | 5–9 (5 in 16) | 5, 5, 7: dash, dash, wall leap and dive |
| | 10–15 (6 in 16) | 10: ice balls |
| 2, `0x800F7030` (HP half or less) | 0–1 (2 in 16) | 10: ice balls |
| | 2–5 (4 in 16) | 8, 8, 10: slide, slide, ice balls |
| | 6–8 (3 in 16) | 11, 13: to the ceiling, ceiling slide |
| | 9–12 (4 in 16) | 11, 12, 13: to the ceiling, ice balls from the ceiling, ceiling slide |
| | 13–15 (3 in 16) | 8, 8, 8: three slides |

Step 9 is not in the tables: the slide (step 8) goes to step 9 when the player is on a wall. The
stun also starts a new row (below). `wolfang-7`: `+0x8A` = 0, `+0x8B` = 14, `+0x8C` = 1, step 10:
the first row was table 1 row 14 (ice balls) [certain].

### Entry (step 2, sub-steps `0x800F763C`) [certain for the code]

0. `PlayerEvent(20, right)`.
1. When the exit request is 0 and the player is in step 20: `PlayerEvent(29, right)` (the event walk
   at 1 px/update; not in stage 12). Makes `BossWarning` (stage logic 24, in `+0x80`). Timer 20.
2. At the end of the timer `PlayerEvent(20, right)`: X walks 20 px. When `BossWarning` is gone:
   `game+0x24` = 1, `game+0x25` = 0, `game+0x20` = the boss, `BossLevel`, `game+0x8C` = 48 + bonus.
   **The room goes dark:** `PaletteFxClear(0)`, the subtract flags of channel 0 for r, g and b
   (`fade_mode[0]`, `0x8009742C`, `0x80090D56`) = 1, `fade_mask[0]` = CLUTs 32–35 and 96–479 (the
   BG), `bg_refresh` |= 1. Timer 3.
3. Every 4 updates `fade_r` += 1, `fade_g` += 32, `fade_b` += 1024 (one step of each 5-bit
   component), until `fade_r` = 31: 124 updates to black. Then the boss jumps to room (240, 144)
   = (6560, 448), speeds 0, gravity 0.0458 px/update² (3000).
4. The fall (`MoveObject2`). On the floor: x + 6, y − 24, animation 7, **display 1**, timer 180.
   Animation 7 has only sprites 69 (empty) and 98–100 (a few red pixels): in the dark room only
   the eyes shine [likely for the picture]. The boss is not drawn before the landing.
5. At the end of the timer: `TextOpen(0, 255, game+0x2E)`, `game+0x2E` = 1 (not in stage 12). So the
   text shows the first time only; after a retry `TextOpen` waits 60 updates with no text.
6. When the text box is closed: `BossMusicRequest`.
7. When the music starts: CLUTs 88–95 join the mask (CLUT 88 is the boss palette, `0x7948`), x − 6,
   y + 24, animation 0, timer 3.
8. Every 4 updates the offsets go down by one step, 124 updates back to the normal colours. At 0:
   sound (2, 16), animation 8 (a howl, 154 updates to its flag 1).
9. At the flag: `+0x7E` = 3.
10. HP + 1 each update to `boss_hp_max`, sound (0, 14) every 3 updates.
11. `PlayerEventEnd`, `WolfangNextMove`.

The boss never changes the camera: the limits of boss gate `id` 6 (x 6320–6384, y 304–320) stay
for the fight [certain: both states].

### Step 3: stun (`0x800F4DF8`) [certain]

Any hit by weapon id 3, 12 or 34, in any step, also in the air.

0. Sound (2, 33), animation 25 (sprite 90, lying), contact damage 4. On the floor: sub-step 2. Else
   speeds 0, gravity 0.25 px/update² (16384), sub-step 1.
1. Fall; on the floor sound (2, 18), `Rumble(8)`.
2. Timer 50.
3. At the end: hit timer `+0x88` = 40 (40 more updates of deflect and flash), a new random row of the
   current table (table 1 while `+0x8A` ≤ 0, table 2 when 1), step = its first byte.

So a stun is 50 updates on the floor plus the fall; the boss deflects everything during the stun
and 40 updates after it. The ice spikes, the ice balls that are not yet thrown and the mouth ice
react to the stun (below).

### Step 5: dash (`0x800F343C`) [certain]

0. Face the player, animation 2 (run).
1. At the frame flag 1: sound (2, 18), `speed_x` = 4.0 px/update toward the facing side.
2. `MoveObject`; sound (2, 18) at each frame flag 2 (steps). When the player is no longer in front
   (or a wall is hit on the facing side): animation 23, `speed_x` = 2.0, timer 16.
3. Slide 16 updates (32 px), then the next move.

### Step 6: jump (`0x800F3648`) [certain]

0. Face the player, animation 4 (crouch), timer 60.
1. At the end: sound (2, 17), a random voice (2, 34 / 35 / 36), animation 5, `speed_y` 8.889 up,
   gravity 0.494, `speed_x` 4.444 toward the facing side: 36 updates in the air, 160 px forward,
   80 px high. Timer 12.
2. After 12 updates animation 6, timer 11; 3. after 11 updates animation 5.
4. On the floor: sound (2, 18), `Rumble(8)`, animation 4, timer 13; 5. then the next move.

### Step 7: leap to a wall and dive (`0x800F383C`) [certain]

0. Animation 9, timer 60.
1. At the end: sound (2, 17), a voice, animation 10. Target: room (368, 128) = the right wall face
   (6688, 432) when the boss is left of room x 192, else room (16, 128) = the left wall face (6336,
   432).
2. Fly at 6 px/update to the target (direction by `ratan2`, the angle again each update). Within 6
   px in x and y, or at a wall on the facing side: sound (2, 18), `Rumble(8)`.
3. Animation 11 (wall cling), timer 90; the player's x, y now are the dive target.
4. At the end: sound (2, 19), animation 10, 6 px/update toward that point.
5. Fly straight until a floor, a ceiling or a wall on the facing side; 6. then speeds 0, gravity
   0.0916 (6000); 7. fall to the floor, sound (2, 18), `Rumble(8)`; 8. animation 9, timer 20;
   9. then the next move.

### Step 8: ice slide (`0x800F3CBC`) [certain]

0. Face the player, animation 16, speeds 0.
1. Sound (2, 19) each update until the frame flag 1; then a voice (2, 37 or 38), `speed_x` 6.0
   toward the facing side, **contact damage 6**, the next drop point = x ± (32 + RNG % 32), a floor
   ice spike (`var` 16), timer 8.
2. Slide at 6 px/update. At each drop point a new floor spike and a new drop point 32–63 px further.
   Every 8 updates a trail image (`var` 1). When the player is behind the boss, or at a wall on the
   facing side: animation 23, timer 8, same speed. **When the player is in step 9, 11 or 52 (wall
   grab, wall slide, Zero's wall attack): step 9.**
3. Slide 8 more updates (48 px), then the next move.

### Step 9: leap to the player's wall and bounce (`0x800F3E74`) [certain]

0. Face the player, animation 18.
1. At the frame flag: sound (2, 17), a voice; 6 px/update at 20° above the horizontal, toward the
   facing side (angles 227 and 1821 of 4096).
2. At a wall on the facing side: sound (2, 18), `Rumble(8)`, animation 11, **two wall runners**
   (visual 20 `var` 32 up and 33 down, below), timer 30.
3. At the end: sound (2, 19), animation 10, turn, 6 px/update at 20° below the horizontal, away from
   the wall (angles 2275 and 3869).
4. At the other wall: as sub-step 2 (wall runners again), back to sub-step 3. On the floor: sound
   (2, 18), `Rumble(8)`, animation 9, timer 30. So the boss zigzags down between the walls (about
   110 px lower for each crossing of the 352 px room).
5. At the end: the next move.

### Step 10: ice balls (`0x800F4094`) [certain]

0. Ball count `+0x7E` = {3, 4, 5, 5}[level] (`0x800F75F0`; the X and Zero rows are the same).
1. Face the player; animation 13 for X, 14 for Zero (13 + `mega+0x02`).
2. Frame flag 1: sound (2, 21), mouth ice (effect 8, `var` = character). Frame flag 2: sound
   (2, 24), an **ice ball** (visual 20, `var` = character: 0 a lob for X, 1 an aimed ball for
   Zero). Frame flag 3 (the end): count − 1; not 0: timer 40 (X) or 70 (Zero), sub-step 3; 0:
   animation 0, timer 60, sub-step 4.
3. At the end of the timer: sub-step 1 (the next ball).
4. At the end of the timer: the next move.

`wolfang-7`: step 10 sub-step 2, animation 13, `+0x7E` = 3, and the first ball (visual 20 `var` 0)
in flight [certain].

### Step 11: jump to the ceiling (`0x800F41C0`) [certain]

0. Face the player, animation 19.
1. At the frame flag: sound (2, 17), a voice, `speed_y` 6.0 up, no gravity.
2. At the ceiling: sound (2, 18), `Rumble(8)`, face the player, animation 20 (hang).
3. At the frame flag 1: the next move (12 or 13 in the rows).

### Step 12: ice balls from the ceiling (`0x800F43C4`) [certain]

0. Volley count `+0x7E` = {1, 2, 3, 4}[level] (`0x800F7620`).
1. Sound (2, 39), face the player, animation 15.
2. Frame flag 1: sound (2, 21), mouth ice (effect 8 `var` 2). Each frame flag 2 (4 in the
   animation): an ice ball `var` 2 (aimed, 7 px/update). Frame flag 3: count − 1, timer 40;
   not 0: sub-step 3, else sub-step 4.
3. Wait, then sub-step 1 (the next volley of 4 balls). 4. Wait, then the next move.

### Step 13: ceiling slide and bounce down (`0x800F45B8`) [certain]

0. Face the far side (right when x < room x 192, else left), animation 17, speeds 0.
1. At the frame flag: sound (2, 19), `speed_x` 6.0, the drop point (32–63 px), a ceiling icicle
   (`var` 18), timer 8.
2. Slide on the ceiling at 6 px/update: a ceiling icicle at each drop point, a trail image every 8
   updates. At the wall: sound (2, 18), `Rumble(8)`, animation 11.
3. At the end of the timer that is left from the trail count (1–8 updates): sound (2, 19),
   animation 10, turn, 6 px/update at 20° below the horizontal, away from the wall.
4. As step 9 sub-step 4, but no wall runners. 5. At the end of 30: the next move.

### Death (state 2, steps `0x800F7550`) [certain for the code]

The same as Ground Scaravich (`docs/stages/scaravich.md`, "The boss (main 50)"):

0. Sound (2, 32), `mega+0xCA` = 0, `PlayerEvent(20, the player's facing)`, animation 25, timer 127,
   flash counter `+0x7E` = `+0x61` = 25, palette bit 15 off.
1. The flash at intervals 26, 21, 16, 11, 6, then 6; at the end of the timer: `BossExplosion`
   (stage logic 26, in `+0x80`) at the boss.
2. `display` = 0 each update; while the explosion is live and its `+0x07` = 0: the flash every 6
   updates and `DisplayCheck`. When it is gone: `+0x80`…`+0x97` = 0, `game+0x24` = 0, `game+0x20` =
   0. Stage 12: exit request `0x80`, `game[0x2C + checkpoint]` = 1, checkpoint += 8. Else the
   Nightmare Virus soul (main 15, flags `0x41`) with `var` `0x10` when the fight timer < 5401
   (90 s), else `0x11`. `DeleteObject3`.

The fight timer `0x800F7840` is the last word of overlay 04 (the overlay ends at `0x800F7844`).
`wolfang-7`: 78 [certain].

When the boss leaves state 1, every ice spike, trail image, ice ball, debris piece and mouth ice
deletes itself in its next update (each one tests `parent+0x04` = 1).

## Animations (table `0x800F7478`, 34 animations) [certain for the frames]

| # | sprites | use |
|---|---|---|
| 0 | 16 30 31 32 31 30 (loop) | stand |
| 1 | 17 0–5 (loop) | not used |
| 2 | 3 4 4 5 0–5 5 (loop to 4) | dash (flag 1 frame 1: start; flag 2 at the end: step sound) |
| 3 | 7–12 (loop) | not used |
| 4 | 13 | crouch (jump start, landing) |
| 5, 6 | 6; 0 | jump |
| 7 | 69 98 99 100 99 98 (loop) | entry: the eyes in the dark |
| 8 | 17–21 20 19 18 18 (flag 1 at the end) | entry howl |
| 9 | 13 | landing |
| 10 | 25 | leap, dive |
| 11 | 92 23 | wall cling |
| 12 | 93 | not used |
| 13, 14 | 18–21…; 18 88 89… (flags 1, 2, 3) | ice balls (X, Zero) |
| 15 | 26–29 … (flags 1, 2 × 4, 4, 3) | ice balls from the ceiling |
| 16 | 17 25 (flag 1) | slide start |
| 17 | 97 96 (flag 1) | ceiling slide start |
| 18 | 17 25 (flag 1) | wall leap start |
| 19 | 13 14 (flag 1) | ceiling jump start |
| 20 | 15 91 (flag 1) | ceiling hang |
| 21, 22 | 91 94; 95 13 | not used |
| 23 | 3 | end of dash and slide |
| 24 | 10 | not used |
| 25 | 90 | stun, death |
| 26 | 57–67 × 3, 63–65 68 69 (flag 1 at the end) | mouth ice (effect 8) |
| 27 | 70–77 × 3, 78–85 (flag 1 at frame 24) | ice ball: spins 48 updates in front of the mouth, then flies |
| 28, 29, 30 | 86; 87; 101 | debris |
| 31 | 33–44 (flag 1 at frame 9) | floor ice spike (`var` 16) |
| 32 | 45–56 | wall ice spike (`var` 17) |
| 33 | 102–113 | ceiling icicle (`var` 18) |

The table has exactly 34 entries: the state table `0x800F7500` follows it. The kit's note "animation
34 uses sprite 175" reads that state table.

## The other objects

### Trail image (main 54 `var` 1, state 3) [certain for the code]

Made every 8 updates in the floor and ceiling slides (`0x800F5360`): the boss's x, y, `flip` and
current sprite, flags | `0x10`, draw priority 6, the palette of the next sprite bank (`GetSpriteBank`
+ 1; stage 12: `0x794C`). It shows that sprite for 20 updates, then the boss's current sprite for 4
updates, then it deletes itself. No boxes, no `ContactMega`: it does no damage. What flag `0x10`
does (semi-transparent?) is open.

### Ice spikes (main 54 `var` 16, 17, 18, state 4) [certain for the code]

Deleted at once when the boss level (`game+0x8F`) < 2. So at levels 0 and 1 there are **no ice
spikes** (the slides and the wall runners still run, but make nothing that stays).

| var | made by | place | animation | hurt / attack box |
|---|---|---|---|---|
| 16 | floor slide (step 8) | the drop point, y = room 192 (496) | 31 | (−6, −16, 12, 32) / (−5, −15, 10, 30) |
| 17 | wall runners (step 9) | x = room 32 (6352) or 352 (6672), the side of the boss | 32 (`flip` 0x40 on the right wall) | (−16, −6, 32, 12) / (−15, −5, 30, 10) |
| 18 | ceiling slide (step 13) | the drop point, y = room 32 (336) | 33 | as `var` 16 |

- HP 2, `dmg_tbl_minor`, contact damage 6, draw priority 4.
- Step 0: the spike grows; at the frame flag 1 (frame 9) it gets its boxes and a life of 300 updates.
- Step 1: at the end of the life it breaks (`SpawnHitEffect`, 3 debris pieces, delete). A ceiling
  icicle falls instead, and also as soon as the player is within 32 px in x: terrain box (0, 0, 8,
  8), gravity 0.3 (19660); on the floor it breaks (step 2).
- On the first update of a boss stun, each spike breaks (icicles fall) with a chance of 1 in 2.
- A kill: `SpawnExplosion`, delete.

### Wall runners (visual 20 `var` 32 and 33) [certain for the code]

Two hidden objects (display 0) made at each wall hit of step 9. `var` 32 moves up, 33 down, 16 px
every 8 updates; at each move a wall ice spike (main 54 `var` 17) at its place. They end at a solid
cell (`CornerCheck`). So the wall gets a line of spikes 16 px apart from the boss up to the ceiling
and down to the floor.

### Ice balls (visual 20 `var` 0, 1, 2) [certain for the code]

Made at the boss with its texture, palette, sprite data and animation table; parent in `+0x7C`.
Offset: `var` 0 (± 24, −8), `var` 1 (± 30, + 8), `var` 2 (± 24, + 8). HP 2, `dmg_tbl_minor`, contact
damage 4, draw priority 4, animation 27.

- Sub-step 0: the ball spins in front of the mouth, with no boxes. When the boss is stunned, it is
  deleted. At the frame flag 1 (48 updates): sound (2, 22), hurt box (−6, −6, 12, 12), attack box
  (−10, −10, 20, 20), terrain box (0, 0, 8, 8), and the throw:
  - `var` 0 (X): a lob that lands at the player's x after 46 updates: `speed_y` 8.364 up, gravity
    0.3636 (`0x600000 / 264`), `speed_x` = (player x − x) / 46.
  - `var` 1 (Zero): aimed at the player, 4.0 px/update.
  - `var` 2 (from the ceiling): aimed at the player, 7.0 px/update.
- Sub-step 1: `MoveObject2`; at a solid cell (`CornerCheck`): sound (2, 23), `Rumble(8)`,
  `SpawnHitEffect`, 3 debris pieces, delete.
- On the player: sound (2, 23), `Rumble(8)`, `SpawnExplosion`, delete. Shot down: the same.
- Off screen (32, 80): delete.

`wolfang-7`: visual 20 `var` 0 at (6508, 415), `speed_x` −3.3913, `speed_y` 5.4545, gravity 0.3636.
`speed_x` × 46 = 6380 − 6536: the throw was from boss x − 24 at the player's x 6380 [certain].

### Debris (visual 20 `var` 16, 17, 18) [certain for the code]

From a broken spike or ball. Animation 28, 29 or 30 by (frame count + `var`) % 3, HP 1, contact 2,
`dmg_tbl_minor`. A random arc: up 30–59 px in 15–24 updates, 10–49 px to the side (`var` 16 left, 17
right, 18 random). **Only on Hard and up** (`game.difficulty` ≥ 2) it gets boxes (hurt (−2, −2, 4,
4), attack (−4, −4, 8, 8)) and so can hurt the player; else it is only a picture.

### Mouth ice (effect 8, `0x800F599C`) [certain for the code]

Made at the frame flag 1 of steps 10 and 12, at the ball offset of its `var`. Animation 26 (the ice
forms). `var` 0, 1: deleted at its own last frame; `var` 2: at the boss's frame flag 4. Deleted at
once when the boss leaves state 1 or is stunned. No boxes.

## Damage

`dmg_tbl_wolfang` (`0x8007519C`), HP 48 at level 0 [certain for the table]:

| ids | damage |
|---|---|
| 0–15 | 1 2 D 5 2 2 2 – 2 1 2 D 8 2 2 2 |
| 16–31 | 2 8 4 4 4 D 8 D 1 1 2 2 2 2 2 2 |
| 32–47 | 2 2 8 2 2 2 2 2 2 2 2 2 D 1 4 8 |
| 48–63 | 2 1 2 2 D 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

- **Weakness: slot 3 (Magma Blade), ids 3 (5) and 12 (8), and id 34 (8), Zero's rising fire wave.**
  These three ids also stun him. Hits to kill at level 0: 10 × id 3, 6 × id 12.
- His own weapon (slot 2, ids 2 and 11, and the shards 52) is deflected.
- Lemon 1 (48 hits), half charge 1, full charge 4 (12 hits), sabre hits 24, 25: 1, other sabre ids 2.
- After each hit 30 updates of `dmg_tbl_deflect` (2 after ids 24 and 25), so the rate of damage is
  limited.
- Contact damage: 4; 6 during the floor slide (step 8, from its start until the next move).

## The boss room [certain for the map]

Collision of `wolfang-5`, `-6`, `-7` (all the same: the boss makes no `CopyTiles`):

| part | x | y | collision |
|---|---|---|---|
| ceiling | 6272–6719 | 304–319 | 57 |
| left wall | 6304–6335 | 320–511 | 57 at the top, inner column 58, outer column 60; the gate cells 59 at y 400–463 |
| right wall | 6688–6719 | 320–511 | 57 at the top, inner column (6688) 58, outer column 60 |
| floor | 6272–6719 | 512–559 | 60: **ice** |
| inside | 6336–6687 (352 px) | 320–511 (192 px) | 0 |

- The floor is ice: stage logic 7 (`docs/engine.md`, "Slippery floors and slopes") is live in both
  states, so the player slides. The room has no slopes and no platforms.
- The inner wall faces are 58, so the player can wall-slide and wall-kick on them; 57 and 60 do not
  allow it (`docs/engine.md`, "Tile collision").
- The way in: camera zone 14 (record 20, trigger x 5760–5999, y 368–463: max x 5744, min y 320),
  boss gate `id` 5 (record 27, x 6064: camera fixed at x 6016, y 304), the corridor with checkpoint
  marker 5 (record 5, x 6192) and talk zone `id` 133 (record 25, x 6144), boss gate `id` 6 (record
  28, x 6320: camera x 6320–6384, y 304–320, time stamp). The view is x 6320–6703, so the whole room
  is on screen with 64 px of scroll.
- The boss record (x 6485) spawns during the gate 6 pass [likely]; in `wolfang-6` it is live in step
  2 sub-step 2 at (6485, 416), not drawn.
- The boss uses the room through the camera target (6320, 304): the wall targets x 6336 and 6688 are
  the wall faces; the floor spikes are at y 496 (floor 512), the ceiling icicles at y 336 (ceiling
  320).

## Sounds (bank 2 = the boss pack `m126-e01`, files in `sounds/m076-e02/`, unless marked) [likely for the use]

| id | file | when |
|---|---|---|
| 16 | `vag-01` | entry: the howl after the room is lit again |
| 17 | `vag-02` | jump, leap, ceiling jump (with a voice) |
| 18 | `vag-03` | landing, wall and ceiling hit, dash steps |
| 19 | `vag-04` | slide start, dive, bounce |
| 21 | `vag-05` | mouth ice |
| 22 | `vag-06` | ice ball thrown |
| 23 | `vag-07` | ice ball or spike breaks |
| 24 | `vag-08` | ice ball made |
| 32 | `vag-09` | death |
| 33 | `vag-10` | stun |
| 34, 35, 36 | `vag-11`–`vag-13` | voices, one at random (jump, leap, wall leap, ceiling jump) |
| 37, 38 | `vag-14`, `vag-15` | voices of the slide (one at random) |
| 39 | `vag-16` | each volley from the ceiling |
| (0, 14) | | HP bar fill, every 3 updates |

The file match uses the word list of `m126-e01` (bank slot 2, 40 words, ids 16–24 and 32–39 set);
`boss_pack_tbl` gives member 126 for stage 2.

## Summary for a reimplementation

- 48 HP (+0/2/4/6 by boss level), contact 4 (6 in the floor slide); weak to Magma Blade (5/8) and id
  34 (8), which also stun him for 50 updates; 30 updates of full deflect after each hit.
- Entry: WARNING; the room fades to black (124 updates); the boss drops in at room (240, 144), only
  his eyes shine for 180 updates; text; music; the room fades back; howl; HP fill.
- Above half HP the moves come in rows: dash-dash-jump, dash-dash-wall dive, or ice balls (3–5
  balls: a lob for X, an aimed 4 px/update ball for Zero). At half HP, rows of slides (with ice
  balls), and ceiling sequences (jump up, 1–4 volleys of 4 aimed 7 px/update balls, ceiling slide,
  zigzag down between the walls).
- Every movement is 6 px/update, except the dash (4) and the jump (4.44 forward).
- At boss level 2 and 3: ice spikes (contact 6, HP 2, 5 s) along the floor slide, the ceiling slide
  and the walls he hits; icicles drop on the player.
- A player on a wall during a floor slide makes him leap at that wall.
- Death and soul as Ground Scaravich (fight timer < 5401 updates: soul `var` `0x10`).

## Open questions

1. What `game+0x8D` counts (it sets the boss level; 4 in both states).
2. Steps 1 and 4 and animations 1, 3, 12, 21, 22, 24 have no user in the walked code.
3. `+0x8D` (set on a blocked hit) has no reader in the boss code.
4. Flag `0x10` of the trail image (`var` 1): the draw mode it gives.
5. The CLUTs 32–35 in the darkness mask: which objects use them.
6. No state shows the ice spikes (boss level 2 or 3), the ceiling moves or the stun; a Hard state
   would show the debris boxes.
7. The stage time `game+0x90` advances by 76 between the states, but the boss went through the whole
   entry (more than 600 updates) in that time: the stage time does not count during the entry
   [likely: while `game.player_lock` is set; not read].
