# Worm Spider (stage 03, main object type 3): behaviour sheet

The Worm Spider is a segmented worm that lives in a nest in the floor or in the ceiling. When the
player comes near, it comes out, sways, and fires an aimed energy ball from its head. A hit makes it
hide in the nest for 2 seconds. A third form lives under the water: it jumps out three times in place,
then makes a long jump at the player.

Overlay `03` (Commander Yammark), save state `yammark-1_start.state.gz`. Written with the RE kit
(`python3 -m mmxdemake.rekit --name worm-spider`) and `docs/re-guide.md`. Units, confidence tags and
the shared engine rules: `docs/engine.md`. Records: `docs/stages/yammark.md`.

## Spawn and set-up [certain]

- **HP 16**, contact damage **4**.
- Draw priority 7.
- It faces the player once, at spawn (equal x → left). It does not turn again.
- The object keeps a box of its own in its work area (`+0x80`, 4 bytes), set to (−4, −26, 8, 52) at
  spawn. The emerge animation changes this box (see EMERGE).
- Fixed boxes:
  - contact box `0x800F6CBC` = (−4, −26, 8, 52): 8 × 52 px, centred on the origin;
  - hurt box `0x800F6CC0` = (−4, −26, 8, 52): the same box;
  - no terrain box (no terrain collision).

## The `id` parameter [certain]

| id | form | set-up |
|---|---|---|
| 0 | **floor nest** | animation 0, origin moved **32 px up**, no boxes, step WAIT |
| 1 | **ceiling nest** | animation 14, origin moved **32 px down**, no boxes, step WAIT |
| 2 | **water jumper** | animation 7, contact and hurt box `0x800F6CBC`/`0x800F6CC0`, jump counter (`+0x86`) = 3, base y (`+0x84`) = record y, a water ripple (misc type 5, var 16) at the record position, origin moved **26 px down**, step JUMP |
| other | – | no step is set; not used [likely] |

In the nest (ids 0 and 1, step WAIT) the contact box and the hurt box are null. So the closed nest
cannot be hurt and does not hurt the player.

## State machine

Standard states (table `0x800F70E0`): 0 `0x800E99D4`, 1 `0x800EA318`, 2 `0x800EA404`, 3 `0x800EA424`.

### Main (state 1) [certain]

1. Saves the previous position. Runs `AnimeTick`.
2. Runs its step (table `0x800F70F0`): 0 engine hit reaction, 1 nothing, 2 WAIT, 3 EMERGE,
   4 ATTACK, 5 SWAY, 6 RETRACT, 7 JUMP.
3. `ContactWeapon`:
   - result > 0 (any hit, also "blocked" 127): hit flag `+0x87` = 1;
   - result = 0 (no hit, also "already hit": `ContactWeapon` returns 0 for 126): hit flag = 0;
   - result < 0 (killed): death (see Death), then state 2. No contact test in that update.
4. Contact test.
5. Off screen 80 (x) / 160 (y) → state 3. Else visible 48 (x) / 64 (y).

### WAIT (step 2, ids 0 and 1)

| sub-step | what |
|---|---|
| 0 | animation 0 (floor) or 14 (ceiling): the closed nest |
| 1 | waits until the player is **within 96 px in x and 96 px in y** of the origin (both tests `≤ 96`), then EMERGE |

### EMERGE (step 3)

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | sound (bank 2, id 6); contact box = hurt box = the own box `+0x80`; animation 2 (floor) or 16 (ceiling); one debris object (misc type 5, var 0 floor / var 1 ceiling) at the worm |
| 1 | 57 updates | the worm comes out of the nest (28 frames, 58 updates). Each update the own box height is set from table `0x800F70C4` by the frame's event flag (1–28): floor `oy = 26 − h`, ceiling `oy = −30` |
| 2 | 30 updates (15 on Hard) | at flag 28: contact box `0x800F6CBC`, hurt box `0x800F6CC0`; animation 4 (floor) or 18 (ceiling), one lean pose; then waits |

Height table `0x800F70C4` (px, by flag 1–28): 2, 4, 12, 12, 12, 12, 16, 16, 16, 16, 20, 28, 28, 28,
28, 36, 36, 36, 44, 44, 44, 44, 46, 48, 48, 48, 48, 48 [certain]. So on the floor the box bottom stays
at +26 px and the box grows upwards to 48 px; on the ceiling the box top stays at −30 px and the box
grows downwards.

### ATTACK (step 4)

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | animation 8 (floor) or 21 (ceiling): the lunge (6 frames, 21 updates, loops); timer 30 |
| 1 | 30 updates | waits; at the end: sound (bank 2, id 7) and a spark at the head (misc type 5, var 2 floor / var 3 ceiling); timer 20 |
| 2 | 20 updates | waits, then SWAY |

The spark fires the shot (see "Shots and effects").

### SWAY (step 5)

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | animation 5 (floor) or 19 (ceiling): the sway (6 frames of 12 updates, loops); timer 90 (45 on Hard) |
| 1 | up to 90 updates (45 on Hard) | when the hit flag is 1 → RETRACT; at the end of the timer → ATTACK |

So the worm fires once every **142 updates (2.37 s)** on Easy and Normal, and every **97 updates
(1.62 s)** on Hard [certain]: ATTACK1 30 + ATTACK2 20 + SWAY0 1 + SWAY1 90 (45) + ATTACK0 1
(`0x800EA608`–`0x800EA7E4`). A hit makes it hide **only during SWAY**. A hit during EMERGE or ATTACK
takes HP but does not stop it [certain]. In the same update, the end of the timer wins over the hit
(ATTACK) [certain].

### RETRACT (step 6)

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | sound (bank 2, id 6); terrain, contact and hurt boxes = null; animation 22 (floor) or 23 (ceiling): back into the nest |
| 1 | 55 updates (floor) / 53 (ceiling) | waits for the event flag 1 on the last frame; timer 120 |
| 2 | 120 updates | waits in the nest, then EMERGE (no proximity test) |

So a hit sends the worm into the nest for about **176 updates (2.9 s)** before it starts to come out
again. It cannot be hurt in that time [likely].

### JUMP (step 7, id 2: the water jumper)

| sub-step | what |
|---|---|
| 0 | animation 7; origin y = base y − 26 (the tail at the water surface); a water spray (misc type 5, var 17) at (x, base y); `speed_x` 0, `speed_y` **8.53 px/frame** (`0x88888`), gravity **0.284 px/frame²** (`0x48D1`); moves once |
| 1 | moves (`MoveObject2`). At the event flag 3 (last frame of animation 7, 59 updates after the start): jump counter − 1; not 0 → sub-step 0 (jump again); 0 → sub-step 2 |
| 2 | animation 7; origin y = base y − 26; water spray; `speed_y` **8 px/frame** (`0x80000`), gravity **0.5 px/frame²** (`0x8000`), `speed_x` = (player x − x) × 2048 / 65536 = **dx / 32 px/frame**; moves once |
| 3 | moves for ever (no terrain box): it falls out of the screen and is deleted (state 3) |

- Each small jump rises about **132 px** (apex after 31 updates). At the event flag 3 it is still about
  8 px above the start; the next jump puts it back at the start height (a small snap) [certain from the
  numbers].
- There are **3 small jumps in place** (60 updates each), then **the long jump**: about 68 px high
  (apex after 16 updates), back at the start height after 34 updates. In 32 updates it moves dx in x,
  so it lands about where the player stood [certain from the numbers].
- The water jumper has no WAIT: it jumps as soon as it spawns. It can be hurt (HP 16) and hurts on
  contact for all its life [certain]. The hit flag has no effect on it [certain].

## Animations (table `0x800F7064`)

| # | what | frames |
|---|---|---|
| 0 | closed nest, floor | sprite 36, hold |
| 1 | debris: dirt bits from the floor nest | sprites 48–49, 3 updates each, loop |
| 2 | worm comes out of the floor nest | sprites 50–75, 0; flags 1–28 (one per frame); 58 updates, hold |
| 3 | worm straight up, floor | sprite 0, hold [no user in this code] |
| 4 | worm leans, floor (pose after EMERGE) | sprite 5, 12 updates, flag 1, hold |
| 5 | worm sways, floor | sprites 6–11, 12 updates each (72), loop |
| 6 | worm leans the other way, floor | sprite 12, hold [no user in this code] |
| 7 | water jumper: body without nest, wiggles | 26 frames (sprites 76, 1–4), 60 updates; flag 1 on frame 0, flag 2 on frame 13, flag 3 on frame 25, hold |
| 8 | lunge, floor (ATTACK) | sprites 7, 45, 46, 45, 7, 44; 4 3 2 3 4 5 updates (21), loop |
| 9 | spark at the head (shot flash) | sprites 37–43, 47; 19 updates; flag 2 on frame 3 (after 5 updates), flag 1 on the last (after 18), hold |
| 10 | energy ball (the shot) | sprites 13–25, 24 frames, 68 updates, loop |
| 11 | water ripple | sprites 27–29, 14 updates each, loop |
| 12 | water splash, then ripple | sprites 30–32, 27–29; loops to frame 3 (the ripple) |
| 13 | water spray | sprites 33–35; 13 updates; flag 1 on the last, hold |
| 14 | closed nest, ceiling | sprite 115, hold |
| 15 | debris, ceiling nest | sprites 116–117, 3 updates each, loop |
| 16 | worm comes out of the ceiling nest | sprites 78–103, 77; flags 1–28; 58 updates, hold |
| 17 | worm straight down, ceiling | sprite 77, hold [no user in this code] |
| 18 | worm leans, ceiling | sprite 104, 12 updates, flag 1, hold |
| 19 | worm sways, ceiling | sprites 105–110, 12 updates each, loop |
| 20 | worm leans the other way, ceiling | sprite 111, hold [no user in this code] |
| 21 | lunge, ceiling | sprites 106, 113, 114, 113, 106, 112; 21 updates, loop |
| 22 | worm goes back into the floor nest | animation 2 in reverse; 56 updates; flag 1 on the last (after 55), hold |
| 23 | worm goes back into the ceiling nest | animation 16 in reverse; 54 updates; flag 1 on the last (after 53), hold |

Event flags that the code tests: anim 2/16 flags 1–28 (box height), 27 (debris end), 28 (end of
EMERGE); anim 7 flags 1 (ripple splash) and 3 (end of jump); anim 9 flags 2 (fire) and 1 (end); anim
13 flag 1 (end); anim 22/23 flag 1 (in the nest). The flag 1 of anims 4 and 18 and the flag 2 of anim
7 have no test in this code.

## Shots and effects

### Effect object (misc type 5, `0x800EA2DC`) [certain]

Made by `0x800EAB3C` (at the worm's position) or `0x800EABF0` (at the worm's x and a given y). It
copies the parent (`docs/engine.md`). Draw priority 6 (spray: 5). The `var` picks the effect:

| var | what | position | animation | ends |
|---|---|---|---|---|
| 0 | debris, floor nest | the worm | 1 | when the worm's animation flag is 27 |
| 1 | debris, ceiling nest | the worm | 15 | as var 0 |
| 2 | spark, floor worm | worm x − 12 (facing left) or + 12 (facing right), y − 30 | 9 | at its own flag 1 |
| 3 | spark, ceiling worm | worm x + 12 (facing left) or − 12 (facing right), y + 30 | 9 | as var 2 |
| 16 | water ripple | the record position | 11; at the worm's flag 1 (start of a jump) → 12 (splash) | see below |
| 17 | water spray | (worm x, base y) | 13 | at its own flag 1 |

- At the flag 2 of animation 9 (5 updates after the spark starts), the spark makes **the shot** at its
  own position.
- When the worm is no longer in state 1 (dead or gone): the ripple blinks (visible every second
  update) for **60 updates**, then it is deleted; every other effect is deleted at once.

### Shot: energy ball (visual object type 3, `0x800EA2A0`) [certain]

- Made by the spark: flags `0x41`, type 3, `+0x7C` = the worm; copies the parent.
- **HP 1**, contact damage **4**, `dmg_tbl_one_hit`: one hit destroys it, but only from a weapon
  with `+0x9C` ≠ 0. `ContactWeapon` skips the other shots, so plain buster shots pass through it
  (`docs/damage.md`, "Shots that can hit enemy shots") [certain for the code].
- Boxes: terrain `0x800F7154` = (0, 0, 3, 3) (a 6 × 6 px body), contact `0x800F7158` =
  (−3, −3, 6, 6): 6 × 6 px, hurt `0x800F715C` = (−4, −4, 8, 8): 8 × 8 px. Draw priority 5.
- Direction: `ratan2` from the spark to the player's position at the moment the shot starts (one aim,
  no homing). Speed **5 px/frame** (`rcos`/`rsin` × 80; 4096 × 80 = `0x50000`). No gravity.
- Animation 10 (energy ball, loops).
- Each update: moves; a weapon kill → one explosion, deleted; contact test (4); terrain (`+0x70` set,
  or `CornerCheck`) → `SpawnHitEffect`, deleted; 32 px off screen → deleted.

## Damage

- Taken: `dmg_tbl_normal`, 16 HP: 6 lemons, 3 half charges, 2 full charges, 4 sabre hits.
- Dealt: worm 4, shot 4.

## Death

One explosion (`SpawnExplosion`), drop table 18, then state 2. No debris. The ripple of a water jumper
blinks for 60 updates.

## Sounds

| bank, id | when |
|---|---|
| 2, 6 | the worm comes out (EMERGE) and goes back (RETRACT) |
| 2, 7 | the spark at the head (the shot) |

The water jumper plays no sound in this code.

## Summary for a reimplementation

- Nest in the floor or ceiling; closed, it cannot be hurt and does not hurt. HP 16, contact 4.
- Player within 96 × 96 px → sound, comes out in 57 frames (hurt/contact box grows to 8 × 48 px), then
  an 8 × 52 px box; waits 30 frames (15 on Hard).
- Loop: lunge 30 frames → spark at the head, an energy ball at 5 px/frame aimed at the player
  (4 damage, 1 HP) → 20 frames → sway 90 frames (45 on Hard) → lunge again.
- A hit during the sway: back into the nest (55 frames, no boxes), 120 frames closed, then out again.
- Water form: 3 jumps in place (8.53 px/frame up, 0.284 px/frame² gravity, about 132 px high), then a
  long jump at the player (8 px/frame up, 0.5 px/frame², dx/32 px/frame across), then falls away.
- Death: one explosion, drop table 18.

## Open questions

1. Which code sets the shot's `+0x70` before `CornerCheck` (the first test). It may be a flag from a
   hit on the player; not found in `ContactMega`.
2. Animations 3, 6, 17 and 20, the flag 1 of anims 4 and 18, and the flag 2 of anim 7 have no user in
   this code.
3. Ids other than 0–2: no step is set, so the object would run step 0 (the engine hit reaction).
   Probably no record uses them.
4. The ceiling spark is on the side away from the facing direction; the ceiling sprites are likely
   turned by 180°, so this is in front of the head. Check in play.
