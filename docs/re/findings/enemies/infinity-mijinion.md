# Infinity Mijinion (stage 11, main object type 58): behaviour sheet

Infinity Mijinion is the dragonfly Maverick of stage 8. He flies in a closed room. He fires an
arrow that turns 90° toward the player, dives to the floor and sends ground runners that drop
lightning bolts, makes green bubbles that hold small copies of himself, and makes full copies
(clones) of himself when he is hit hard. Lemon hits fill a counter; at 3 and 6 the next special
move becomes an 8-way shot burst, or a heal followed by many bursts. Guard Shell (slot 7, ids 16
and 53) is his weakness: it knocks him back to a wall and makes a green hang bar in the room.

Overlay `11` (stage 8), save state `mijinion-3_boss-fight.state.gz` (stage 8, area 0, checkpoint 1,
Zero, the first move of the fight). Kit:
`python3 -m mmxdemake.rekit --state mijinion-3_boss-fight.state.gz --type 58 --name infinity-mijinion`;
the effect objects: `--class effect --type 10`. Units, confidence tags and the shared rules:
`docs/engine.md`. Record: record 54 of member 103 (kind 0, `b` 58, `id` 0) at (5968, 1032), the last
record. "The room" is the boss room; room positions are from the camera target minimum
(`cam.target_min_x`, `cam.target_min_y`) = (5808, 832) in the save state. The other objects of the
fight are main type 58 with other `var` values (clones, bubbles, hang bars), visual type 25 (arrow,
ground runners, bolts, star shots) and effect type 10 (particles, glows, the bar beam).

## Spawn and set-up (`MijinionInit`, `0x800F5C34`) [certain]

- All `var` values: contact damage 6, damage table `dmg_tbl_mijinion` (`0x8007569C`), HP 0, no hit
  timer, display 0, draw priority 5, layer = `mega.layer`, animation table `0x800FD278`, speeds 0,
  no boxes. Stage 12: sprite data from `ray_table+4` entry 0 and palette `0x7948`.
- `var` & `0xF0` = 0 (the boss and the clones): animation 0, texture `0x140`, `+0x86` = −1 (no move
  table yet), `+0x87`…`+0x8E` = 0, terrain box `0x800FCED8` = (0, 0, 16, 18) (a 32 × 36 px body).
  - **`var` 0, the boss:** `flip` 0, the bar slots `0x800FDA78`[3] = 0, the clone count `0x800FDA7C`
    = 0, the bubble count `0x800FDA7E` = 0, the fight timer `0x800FDA84` = 0. State 1, step 2 (the
    entry).
  - `var` 1–15, a **clone**: HP 16, state 2, step 3 (appear).
- `var` `0x10`, a **bubble** (small copy): contact 4, HP 5, priority 5, flags |= `0x10`, state 4.
- `var` `0x20`–`0x22`, a **bar emitter**: animation 21, HP 50, priority 6, state 5.
- Boxes of the boss and the clones (`0x800F9DD0`, each update): attack box `0x800FCEDC` and hurt box
  `0x800FCEE0` = (−10, −16, 20, 32); both null while `+0x8A` ≠ 0 (teleport blink, appear).
  The boxes `0x800FCEE4` (−16, −26, 32, 42) and `0x800FCEEC` (−64, −16, 96, 32) follow in the data
  but no code reads them [likely: a copy of Wolfang's box list].
- **HP: 48 + a level bonus.** The entry sets `boss_hp_max` = 48 + {0, 4, 6, 8}[level] (bytes at
  `0x800E9B90`), level = `game+0x8F` from `BossLevel`. The HP fills from 0 at the end of the entry.
  The state: level 1, `boss_hp_max` 52, HP 52 [certain].

## The `id` parameter

The record `id` is 0. Other `var` values come only from the boss code.

## State machine [certain for the code]

Entry `0x800F7E38`: `prev_x`, `prev_y` = x, y; the state from `0x800FD2F0`; then `CollideCheck` for
`var` & `0xF0` = 0 (the boss and the clones; nothing moves them into walls but the knock-back).

| state | function | use |
|---|---|---|
| 0 | `0x800F5C34` | set-up |
| 1 | `0x800F5E20` | the boss: main (steps `0x800FD30C`) |
| 2 | `0x800F610C` | clone (steps `0x800FD344`) |
| 3 | `0x800F7EA8` | the boss: death (steps `0x800FD358`) |
| 4 | `0x800F8064` | bubble (steps `0x800FD438`) |
| 5 | `0x800F8390` | bar emitter (steps `0x800FD44C`) |
| 6 | `0x800F8638` | delete: clone count − 1 (`var` 1–15) or bubble count − 1 (`var` `0x10`), `DeleteObject3` |

### Main (state 1, `0x800F5E20`) [certain]

1. `AnimeTick`; the step from `0x800FD30C` (14 entries); the boxes (above).
2. The fight timer `0x800FDA84` += 1, except in step 2 (the entry) and for `var` ≠ 0.
3. `ContactWeapon`:
   - a kill: `mega+0x7A` = 1, state 3;
   - a hit (not 127): damage table `dmg_tbl_deflect`, hit timer `+0x84` = 30. Then by weapon id:
     - **ids 7, 16, 53 (Guard Shell):** `+0x84` = 10000, step 3 (stun knock-back);
     - **ids 24–29, 31, 37, 39, 50, 51 (sabre hits):** `+0x84` = 10000, step 12 (knock-back);
     - **id 0 (lemon):** sound (2, 36); the lemon counter `+0x8B` += 1 (+ 2 on Hard and up,
       `game.difficulty` ≥ 2), at most 8; at ≥ 6 a glow (effect 10 `var` `0x11`), at 3–5 a glow
       (`var` `0x10`); then the quarter test below;
     - every other id: the quarter test.
   - **Quarter test** with the counter `+0x8E` (0, 1, 2): when HP ≤ `boss_hp_max` × 3/4, × 1/2,
     × 1/4 in turn: `+0x84` = 10000, step 12 at sub-step 3 (knock-back that always makes a clone),
     `+0x8E` += 1. So each quarter of the HP bar gives one forced clone.
4. While `+0x84` > 0: −1 each update; palette bit 15 on when bit 1 of the old value is set (2
   updates on, 2 off). At 0: `dmg_tbl_mijinion` again, flash off. A normal hit gives 30 updates of
   full deflect. The stun and the knock-back keep the deflect (10000) until they end, then 40 more.
5. `ContactMega` in each update.

Id 7 is class 2 (no effect) in `dmg_tbl_mijinion` and in `dmg_tbl_deflect`, so `ContactWeapon` never
returns it: the id 7 case is dead code [certain for the tables]. Only ids 16 and 53 stun.

The step table (`0x800FD30C`):

| step | function | what |
|---|---|---|
| 0 | `HitReactionDispatch` | engine hit reaction (no writer here) |
| 1 | `0x800F9DC8` | empty (no writer) |
| 2 | `0x800F996C` | entry (10 sub-steps, `0x800FD404`) |
| 3 | `0x800F9CB0` | stun knock-back, makes a bar emitter |
| 4 | `0x800F869C` | float (between moves) |
| 5 | `0x800F8798` | track the player's height and fire arrows |
| 6 | `0x800F89AC` | teleport, dive, ground runners |
| 7 | `0x800F8CA4` | pick 8, 9 or 10 by the lemon counter |
| 8 | `0x800F8CD8` | glow, lemon counter + 1 (or + 2) |
| 9 | `0x800F8D90` | fly at the player and burst, n times |
| 10 | `0x800F8F78` | heal, then n bursts |
| 11 | `0x800F92A0` | make bubbles |
| 12 | `0x800F9588` | knock-back (sabre, quarter HP) |
| 13 | `0x800F9864` | appear (animation 5); the boss has no writer, the clones use it |

### The next move (`MijinionNextMove`, `0x800F6E4C`) [certain]

Each move ends in step 4; step 4 ends with this function (also the stun and knock-back ends). It
sets sub-step 0 and takes the next step from a **move row**: 8 bytes, ended by `0xFF`. `+0x87` is the
row (RNG % 16), `+0x88` the place in the row, `+0x86` the table.

- The boss: `+0x86` = −1 (the first call, at the end of the entry): a row of table A, `+0x86` = 0.
  `+0x86` = 0 and HP ≤ max / 2: a row of table B at once, `+0x86` = 1 (back to A if HP > max / 2,
  which only a heal can do). Else the next byte; at `0xFF` a new random row of the same table.
- A clone: table C (`0x800FCE58`): every row is `2, 0xFF`: always step 2 of the clone table.

| table | rows | sequence |
|---|---|---|
| A, `0x800FCD58` (HP above half) | 0–7 (8 in 16) | 5, 7: arrows, then a special |
| | 8–11 (4 in 16) | 6, 7: dive, then a special |
| | 12–15 (4 in 16) | 11, 7: bubbles, then a special |
| B, `0x800FCDD8` (HP half or less) | 0–4 (5 in 16) | 5, 7 |
| | 5–9 (5 in 16) | 6, 7 |
| | 10–15 (6 in 16) | 11, 7 |

**Step 7** (`0x800F8CA4`): lemon counter `+0x8B` ≥ 6: step 10; 3–5: step 9; else step 8. So
every row ends with a special that depends on how many lemons hit him. Step 8 adds to the counter,
so without lemon hits the counter still grows by 1 (2 on Hard) per row; the stun resets it to 0.

The state: `+0x86` = 0, `+0x87` = 11, `+0x88` = 1, step 6: the first row was table A row 11 (6, 7)
[certain].

### Entry (step 2, sub-steps `0x800FD404`) [certain for the code]

0. `PlayerEvent(20, right)`.
1. When the exit request is 0 and the player is in step 20: `PlayerEvent(29, right)` (not in stage
   12). Makes `BossWarning` (stage logic 24, in `0x800FDA74`). Timer 20.
2. At the end of the timer `PlayerEvent(20, right)`. When `BossWarning` is gone: `game+0x24` = 1,
   `game+0x25` = 0, `game+0x20` = the boss, `BossLevel`, `boss_hp_max` = 48 + bonus. The boss jumps
   to room (208, 208) = (6016, 1040) (not drawn). Sound (2, 66). Timers 2 and 16.
3. Every 3 updates a green particle (effect 10 `var` 0) at a random angle (`0x800FDA80` += RNG + 255),
   15 in all. Each one starts 64 px from the boss, blinks in for 30 updates and flies to the boss at
   0.17–0.33 px/update (a new speed every 5–12 updates). The boss counts them in `+0x07`.
4. When all particles have arrived (`+0x07` = 0): sound (2, 67), animation 23 (the bubble forms and
   Mijinion comes out), **display 1**.
5. Frame flag 1: sound (2, 68), 8 sparks (effect 10 `var` `0x30`) at angles 0, 512 … 3584; flag 2: 8
   more at 256, 768 … 3840; each flies off at 9–12 px/update. Flag 3: animation 0,
   `TextOpen(0, 255, game+0x2E)`, `game+0x2E` = 1 (not in stage 12).
6. When the text box is closed: `BossMusicRequest`.
7. When `BossMusicWait` returns 0: sub-step 8.
8. HP + 1 each update to `boss_hp_max`, sound (0, 14) every 3 updates.
9. `PlayerEventEnd`, `MijinionNextMove`.

No camera change, no palette fade, no `CopyTiles` in the entry. The limits of boss gate `id` 27
(x 5808–5824, y 832–928) stay for the fight [certain: the state].

### Step 3: stun knock-back (`0x800F9CB0`) [certain]

Any hit by id 16 or 53, in any step.

0. Sound (2, 33), animation 8 (hurt), contact 6, lemon counter = 0. **A bar emitter** (main 58
   `var` `0x20` + slot) in a free slot of three (start at RNG % 3, then the next free one); none when
   all three are in use. **A clone** (`var` 1) when the clone count < {1, 3, 5, 7}[level]
   (`0x800FD2EC`). Face the player; `speed_x` 5.0 away from the player, `accel_x` −0.25 (it slows and
   turns), no gravity.
1. `MoveObject2` until a wall on the side he moves to: sound (2, 78), `Rumble(5)`,
   `Quake2(20, 6, 2)`, timer 90.
2. At the end: hit timer `+0x84` = 40, `MijinionNextMove`.

### Step 4: float (`0x800F869C`) [certain]

0. Animation 0; face the room centre (room x 168); `speed_x` 2.0 toward it; target height = one of
   room y 112, 144, 176 (RNG % 3, in `+0x07`); timer 60.
1. 60 updates: `MoveObject`; at a wall turn and go back at 2.0; y goes to the target at
   1 px/update.
2. `MijinionNextMove`.

### Step 5: arrows (`0x800F8798`) [certain]

0. Face the player, animation 1. Duration `+0x7C` = {180, 270, 360, 360}[level] updates
   (`0x800E9B80`). Window `+0x89` = 8 + RNG % 24.
1. Face the player each update. While no cool-down: when |y − player y| ≤ window, animation 2 and
   sub-step 2; else y moves toward the player's y at 1.5 px/update. In the cool-down (`+0x7E`): it
   counts down and y still follows the player. At the end of the duration: step 4.
2. Frame flag 1 (31 updates into animation 2): a voice (2, 34 or 35, random); an **arrow**
   (visual 25 `var` 0, or `var` 1 when HP ≤ max / 2). Flag 2 (the end): animation 1, cool-down 90,
   a new window, sub-step 1.

### Step 6: teleport, dive, ground runners (`0x800F89AC`) [certain]

0. Face the player, `+0x8A` = 1 (no boxes), timer 30.
1. 30 updates of blink (display on and off each update). The state is here (`+0x7C` = 1).
2. Sound (2, 72), animation 5 (appear), x, y = room (168, 144) = (5976, 976), face the player.
3. Frame flag 1 (46 updates): animation 3 (dive), boxes on, `speed_y` 9.0 down.
4. Until the floor: `MoveObject`. On the floor: sound (2, 73), `Quake2(8, 3, 1)`, animation 4,
   `+0x07` = 0.
5. Animation 4: flag 3 (48 updates): sound (2, 74); flag 1 (92 updates): a voice, sound (2, 75), two
   **ground runners** (visual 25 `var` `0x10` right and `0x11` left); flag 2: sub-step 6.
6. When `+0x07` = 0 (all bolts of the runners have ended): step 4.

### Step 8: charge (`0x800F8CD8`) [certain]

0. A glow (effect 10 `var` `0x10`); lemon counter + 1 (+ 2 on Hard and up).
1. Step 4.

### Step 9: fly and burst (`0x800F8D90`) [certain]

0. Animation 1; count `+0x8C` = lemon counter (3–5), lemon counter = 0; timer 30.
1. 30 updates: fly toward the player at 2.0 px/update (the angle again each update, `ratan2`).
2. Face the player, animation 2.
3. Frame flag 1: sounds (2, 37) and (2, 71), **8 star shots** (visual 25 `var` `0x30`) at angles
   0, 1024, 2048, 3072, 512, 1536, 2560, 3584. Flag 2: count − 1; 0: step 4; else animation 1,
   timer 30, sub-step 1.

### Step 10: heal and burst (`0x800F8F78`) [certain]

0. Face the player, damage table `dmg_tbl_deflect`, `+0x84` = 10000 (no damage), timer 60, count =
   lemon counter (6–8), lemon counter = 0.
1. After 60 updates: sound (2, 37), `+0x7E` = `boss_hp_max` / 3.
2. Every 8 updates: face the player, **HP + 1** (at most `boss_hp_max`), until `+0x7E` HP are given:
   16 HP at level 0, 17 at level 1, in 128–136 updates.
3. Face the player, animation 2.
4. Frame flag 1: the 8 star shots (as step 9). Flag 2: count − 1; 0: `+0x84` = 1 (the deflect ends
   at the next update), step 4; else sub-step 3 (the next burst at once).

### Step 11: bubbles (`0x800F92A0`) [certain]

0. When the bubble count `0x800FDA7E` ≥ 10: step 4. Else face the player, animation 7.
1. Frame flag 1 (81 updates): a voice, sound (2, 77), {3, 4, 6, 8}[level] **bubbles** (main 58
   `var` `0x10`, `0x800E9B88`), but only while the count < 10. Flag 2: sub-step 2.
2. Step 4.

The clones run the same function as their step 2.

### Step 12: knock-back (`0x800F9588`) [certain]

0. (A sabre hit.) Animation 8. A clone (`var` 1) when the clone count < {1, 3, 5, 7}[level] and
   RNG % 16 ≥ {0, 1, 3, 7}[level] (`0x800E9B8C`): always at level 0, 9 in 16 at level 3. Away from
   the player at 5.0, `accel_x` −0.25, no gravity.
1. Until a wall on the side he moves to: sound (2, 78), `Rumble(5)`, `Quake2(20, 6, 2)`, timer 30.
2. At the end: `+0x84` = 40, `MijinionNextMove`.
3. (A quarter of the HP lost.) Animation 8, **a clone always** (no count test), away at 5.0,
   sub-step 1.

### Death (state 3, steps `0x800FD358`) [certain for the code]

The same as Blizzard Wolfang and Ground Scaravich:

0. Sound (2, 32), `mega+0xCA` = 0, `PlayerEvent(20, the player's facing)`, animation 8, timer 127,
   flash counter `+0x7E` = `+0x61` = 25, palette bit 15 off.
1. The flash at intervals 26, 21, 16, 11, 6, then 6; at the end of the timer: `BossExplosion`
   (stage logic 26, in `0x800FDA74`) at the boss.
2. `display` = 0 each update; while the explosion is live and its `+0x07` = 0: the flash every 6
   updates and `DisplayCheck`. When it is gone: `+0x80`…`+0x97` = 0, `game+0x24` = 0, `game+0x20` =
   0. Stage 12: exit request `0x80`, `game[0x2C + checkpoint]` = 1, checkpoint += 8. Else the
   Nightmare Virus soul (main 15, flags `0x41`) with `var` `0x10` when the fight timer < 5401
   (90 s), else `0x11`. `DeleteObject3`.

The fight timer `0x800FDA84` is the last word of overlay 11's data used here. The state: 31.

Every clone, bubble, bar emitter, visual 25 and effect 10 deletes itself when the boss leaves state 1
(each one tests its parent's `+0x04` = 1; effect 10 also ends when the boss is in step 3).

## Animations (table `0x800FD278`, 29 animations) [certain for the frames]

| # | sprites | use |
|---|---|---|
| 0 | 3 2 1 0 (loop) | hover |
| 1 | 7–4 (loop) | hover, facing the player (arrows, fly) |
| 2 | 8–17 39 (flag 1 at frame 5, 2 at the end) | fire: arrow, star burst |
| 3 | 18–20 (loop) | dive |
| 4 | 18 21 31 22–30 … (flags 3, 1, 2) | dive landing, the floor flash |
| 5 | 32–39 (flag 1 at the end) | appear (teleport, clone) |
| 6 | 39–48 | shrinks into a green blob; no user found [likely unused] |
| 7 | 39 49–56 … (flags 1, 2) | make bubbles |
| 8 | 57 58 57 59 (loop) | hurt: stun, knock-back, death |
| 9 | 60–63 | arrow, horizontal |
| 10, 11 | 64–67; 68–71 | arrow, up; down |
| 12 | 72–80 (flag 1) | arrow end (ring) |
| 13 | 84–81 | ground runner (orange ball); also the star shot |
| 14 | 85–88 | lightning bolt |
| 15 | 89–93 (flag 1) | bolt hits the floor |
| 16 | 94–105 (flag 1) | glow: dots close in (effect `var` `0x10`) |
| 17 | 106–116 105 (flag 1) | glow: a ring opens (effect `var` `0x11`) |
| 18 | 117–120 167 122 (flag 1) | bubble forms |
| 19 | 123–126 122 (loop) | bubble with a small Mijinion inside |
| 20 | 122 127 122 121–117 (flag 1) | bubble bursts |
| 21 | 128–134 (loop) | bar emitter (green light) |
| 22 | 135–139 (loop) | entry particle |
| 23 | 154 46–40 153 140–149 (flags 1, 2, 3) | entry: the bubble forms and Mijinion comes out |
| 24, 25, 26 | 150; 151; 152 | entry sparks |
| 27 | 166 | bar piece, 48 px |
| 28 | 155–166 | bar end pieces, 4–48 px; the code sets the sprite directly, not by this animation |

## The other objects

### Clones (main 58 `var` 1, state 2) [certain for the code]

Made by the stun, by a sabre knock-back and at each quarter of the HP (above); each copies the
boss's x, y, `flip` and graphics; parent in `+0x80`; the clone count + 1. **HP 16**, contact 6,
`dmg_tbl_mijinion`, the boss boxes. The live count is capped only for the stun and sabre clones:
{1, 3, 5, 7}[level].

- Step 3 (appear): animation 5, no boxes; at the flag, boxes on, the next move.
- Step 2 (always): make bubbles (as boss step 11; the shared count of 10 holds).
- Step 4: animation 0, wait 120–247 updates (face the player every 16), the next move.
- A clone does not move: it stays where the boss was when it was made.
- A hit: 20 updates of deflect and flash. **Sabre ids 24–28, 37, 39, 50, 51**: while 2 or more clones
  are live, each such hit counts in `+0x8D`; the 4th kills the clone (explosion, delete), whatever
  its HP. A kill by damage: `SpawnExplosion`, delete. No soul, no drop.

### Bubbles (main 58 `var` `0x10`, state 4) [certain for the code]

Green bubbles with a small Mijinion inside. HP 5, contact 4, `dmg_tbl_mijinion`, flags `0x10`.

0. Animation 18 (forms), display 1; placed 32–63 px from the maker at the angle RNG × 4 (+ 1024
   when the maker faces left); the range of `GetRNG` was not read, so the spread is open.
1. At the flag: animation 19 at a random frame; attack and hurt box `0x800FCEF4` = (−13, −13, 26,
   26); `+0x7E` = 8; wait 20–89 updates.
2. Aim at the player: 1.5 px/update (3.0 on Hard and up); then fly 50–89 updates.
3. Fly; at the end `+0x7E` − 1: back to 2 (stop, wait 60–89 updates, aim again) — 9 dashes in all;
   then animation 20 (bursts), 4. at its flag delete.

Off screen by 32 px: delete. Shot down: `SpawnExplosion`, delete. At most 10 live (`0x800FDA7E`).

### Bar emitters and the hang bar (main 58 `var` `0x20`–`0x22`, state 5) [certain for the code]

Made by each stun (step 3), one per free slot of three (`0x800FDA78`).

0. Display 1, hurt box `0x800FCEFC` = (−8, −8, 16, 16), no attack box: it does no damage. Face the
   room centre (left when x > room x 168).
1. Move vertically at 5 px/update to room y 242 (slot 0), 194 (slot 1) or 146 (slot 2):
   y 1074, 1026, 978. Then a life of 3600 updates (60 s).
2. **The bar.** The target length `+0x88` starts at 0 and grows by 32 px each time the emitter is
   hit: for Zero by any weapon hit, for X only by the lemon (id 0); also when a boss arrow touches its
   hurt box (the arrow stops there). The length is capped at the wall face (room x 16 or 320). The
   drawn length `+0x86` follows the target at 4 px/update. Each update the beam is drawn (effect 10
   `var` `0x20`: 48 px pieces and an end piece). Each time the length passes a 16 px step,
   **`CopyTiles` writes one cell** from map (5632, 1200) — collision **45, a flat hang bar** — at the
   end of the beam, row y & ~15 (1072, 1024 or 976). The player can hang on it and move along it
   (`docs/players/air.md`, steps 27 and 28). HP is set back to 50 after each hit: it cannot be
   destroyed.
3. At the end of the life: boxes off; 60 updates of beam flicker; then 19 `CopyTiles` from map
   (5632, 1216) (collision 0) at x 5824…6112 on its row: the bar is removed. The slot is freed;
   delete.

When the boss dies the emitters delete themselves at once (parent test), with **no restore**: the
collision 45 cells stay in the map [certain for the code; not seen in a state].

### Visual 25 (`0x800FA76C`, states `0x800FD490`) [certain for the code]

Made at the maker's place, with its graphics; parent (the boss or a clone) in `+0x7C`. Set-up: HP 1,
`dmg_tbl_deflect` or `dmg_tbl_one_hit`, priority 4, `+0x67` = 1. All end when the parent leaves state
1, or off screen by (32, 176). `CollideCheck` only for `var` & `0xF0` = 0.

| var | what | contact | boxes (attack / hurt / terrain) | moves |
|---|---|---|---|---|
| 0, 1 | **arrow** (sound (2, 69)), from x ± 32, y − 8 | 4 | (−20, −2, 24, 4) / none / (−8, 0, 10, 4) | 1.25 px/update forward (`var` 1: 2.5) |
| `0x10`, `0x11` | **ground runner**, y = room 288 (the floor, 1120) | 6 | (−4, −4, 8, 8) both | 3.0 right (`0x10`) or left (`0x11`); every 12 updates a bolt, 4 in all |
| `0x20` | **lightning bolt** (sound (2, 76)), from room y −32 | 8 | (−4, −16, 8, 32) / none | 16 px/update down to room y 272, then animation 15, delete |
| `0x30` | **star shot**, angle from the maker | 6 | (−4, −4, 8, 8) both | 6.0 px/update straight |

- **The arrow turns.** When it is within 8 px of the player's x (or at a wall on its side): sound
  (2, 70), it stops and flies vertically toward the player (up animation 10 / down 11) at 1.5
  (`var` 1: 3.0) px/update, with the attack box (−2, −20, 4, 24) up or (−2, −4, 4, 24) down. It ends
  (animation 12, no boxes, delete at the flag) at a ceiling or a floor, at a cell with collision 45
  (a hang bar), or when it touches a bar emitter. It cannot be shot (deflect table, no hurt box).
- Ground runners and star shots have `dmg_tbl_one_hit`: only shots with `+0x9C` ≠ 0 destroy them
  (`docs/damage.md`). Each bolt adds 1 to the parent's `+0x07` and takes it off at its end; boss step
  6 waits for 0.
- A `var` & `0xF0` = `0x50` / `0x60` branch (no reaction / explode on contact) has no maker here.

### Effect 10 (`0x800FB020`, states `0x800FD4DC`) [certain for the code]

Priority 4; no boxes. It does nothing while `mega+0xCC` is not 0 (the kit names `0x8009716C` `mega.grab+0x2`; the
meaning is open). It ends when its parent leaves
state 1 or enters step 3.

| var | made by | what |
|---|---|---|
| 0 | entry sub-step 3 | green particle (animation 22, flags `0x10`): 64 px out, flies in, parent `+0x07` − 1 on arrival, ends when the boss is drawn |
| `0x10`, `0x11` | lemon counter 3–5 / ≥ 6, step 8 | glow on the boss (animation 16 / 17), follows it, ends at its flag |
| `0x20`, `0x21` | bar emitter | the beam, redrawn each update (1 update life): pieces of 48 px (animation 27) and the end sprite 155–166; `0x8002C6F0` gets the piece slots (not read) |
| `0x30` | entry sub-step 5 | spark (animation 24–26), 9–12 px/update at the angle ± 63, flags `0x10` |

## Damage

`dmg_tbl_mijinion` (`0x8007569C`), HP 48 at level 0, 52 at level 1 [certain for the table]:

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 2 2 2 – D 1 2 2 2 2 2 2 |
| 16–31 | 2 D 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 2 2 2 2 2 2 2 2 2 D 1 4 8 |
| 48–63 | 2 D 2 2 1 8 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

- **Weakness: Guard Shell (slot 7): id 16 (2) and id 53 (8), the Guard Shell shards.** These stun and
  knock him back, and each makes a bar emitter. Id 7 (Guard Shell uncharged) has no effect.
- His own weapon (slot 8: ids 8, 17, and 44) is deflected; also ids 21, 23, 49.
- Lemon 1 (it fills the lemon counter), half charge 1, full charge 4, sabre 2 (id 26: 5), and every
  sabre hit knocks him back.
- After each hit 30 updates of `dmg_tbl_deflect`; 40 after a stun or knock-back.
- The heal (step 10) gives back `boss_hp_max` / 3.
- Contact damage: boss and clones 6, bubbles 4, arrow 4, runner 6, bolt 8, star shot 6.

## The boss room [certain for the map]

Collision of `mijinion-2` and `mijinion-3` (the same; the bar is the only `CopyTiles`):

| part | x | y | collision |
|---|---|---|---|
| ceiling | 5808–6143 | 816–831 | 57 |
| left wall | 5808–5823 | 832–1119 | 57 at the top, 56; the gate cells 59 at x 5792–5823, y 1056–1119 |
| right wall | 6128–6143 | 832–1119 | 57 at the top, 56 |
| floor | 5440–6143 | 1120–1167 | 56, then 57 |
| inside | 5824–6127 (304 px) | 832–1119 (288 px) | 0 |

- No platforms and no slopes; the walls (56) allow the wall slide and kick. The only platforms are
  the hang bars (collision 45) that the bar emitters write at y 1072, 1024 and 976.
- The source cells of the bar: (5632, 1200) holds 45 and (5632, 1216) holds 0, below the floor of the
  corridor (hidden cells used as a palette of tiles).
- The way in: camera zone `id` 128 (record 2, x 5024–5055, y 800–1039: min x 4704), boss gate `id` 0
  (record 52, x 5488: camera fixed at x 5488), the corridor with checkpoint marker 1 (record 0,
  x 5632), boss gate `id` 27 (record 53, x 5808: camera x 5808–5824, y 832–928, time stamp). The
  view is x 5808–6143, so the whole room width is on screen with 16 px of scroll; y scrolls 96 px.
- The boss record (x 5968, y 1032) spawns at the gate pass [likely]; its code moves it to (6016,
  1040) in the entry.
- The boss uses the room through the camera target (5808, 832): wall faces at room x 16 and 320,
  the floor at room y 288.

## Sounds (bank 2 = the boss pack, member 132, files in `sounds/m082-e02/`) [likely for the use]

| id | file | when |
|---|---|---|
| 32 | `vag-14` | death |
| 33 | `vag-15` | stun |
| 34, 35 | `vag-16`, `vag-17` | voices, one at random (arrow, runners, bubbles) |
| 36 | `vag-18` | lemon hit |
| 37 | `vag-19` | star burst; heal start |
| 66 | `vag-01` | entry: the particles start |
| 67 | `vag-02` | entry: the bubble forms |
| 68 | `vag-03` | entry: the sparks |
| 69 | `vag-04` | arrow fired |
| 70 | `vag-05` | arrow turns |
| 71 | `vag-06` | star burst |
| 72 | `vag-07` | teleport |
| 73 | `vag-08` | dive landing |
| 74 | `vag-09` | landing flash (animation 4 flag 3) |
| 75 | `vag-10` | ground runners |
| 76 | `vag-11` | lightning bolt |
| 77 | `vag-12` | bubbles made |
| 78 | `vag-13` | knock-back: wall hit |
| (0, 14) | | HP bar fill, every 3 updates |

`vag-20` has no id in the word list.

## Difficulty and Nightmare Effect

- Hard and up (`game.difficulty` ≥ 2): the lemon counter goes up by 2 (per lemon hit and per step 8),
  so the specials 9 and 10 come sooner; the bubbles fly at 3.0 instead of 1.5 [certain for the code].
- Boss level: HP bonus, arrow duration, bubble count, clone cap and clone chance (above).
- The Nightmare Effect is not read by this code (`nightmare_effect` is 3 in the state) [certain for
  the walked code].

## Summary for a reimplementation

- 48 HP (+0/4/6/8 by level), contact 6; Guard Shell ids 16 (2) and 53 (8) stun him: knock-back to a
  wall, a clone and a temporary hang bar; sabre hits also knock him back. 30 updates of deflect after
  each hit.
- Entry: WARNING; 15 green particles gather, a bubble forms and Mijinion comes out; text; music; HP fill.
- Moves in pairs: arrows (track the player's height, the arrow turns 90° at the player's x), or a
  teleport-dive with two ground runners that drop 4 bolts each, or bubbles (3–8 small copies that
  dash at the player 9 times); then a special by the lemon counter: charge (+1), 3–5 rounds of fly
  and 8-way burst, or (≥ 6) a heal of max/3 and 6–8 bursts.
- At each quarter of the HP, a knock-back and a clone (HP 16, it makes bubbles, 4 sabre hits kill
  it when 2+ clones live).
- Bar emitters: shoot them to extend a hang bar (collision 45) up to the wall; 60 s life.
- Death and soul as Blizzard Wolfang (fight timer < 5401 updates: soul `var` `0x10`).

## Open questions

1. What `+0x8D`/`+0x8E` of `game` count (the boss level source; 3 and 3 in the state).
2. Steps 0, 1, 13 of the boss and animation 6 have no user in the walked code.
3. Flags `0x10` of the bubbles and particles (semi-transparent?).
4. `0x8002C6F0` (the slot getter of the beam pieces) and `mega+0xCC` (the effect 10 pause test)
   were not read.
5. The hang bar cells stay after the boss dies (no restore on that path): not checked in play.
6. No state shows a stun, a clone, a bubble, a bar or the death; the state shows only the first move.
7. The stage-12 rematch uses another overlay; its copy of this code was not read.
