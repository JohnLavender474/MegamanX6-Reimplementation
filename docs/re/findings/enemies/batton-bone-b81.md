# Batton Bone B81 (stage 09, main object type 18): behaviour sheet

Batton Bone B81 is a bat robot. One variant carries a bomb under its body. It flies to a point above X
and drops the bomb. The other variants carry no bomb: they fly straight, fly off in a rising scatter,
or follow X for 5 seconds. The bomb is a separate object (visual type 17). It falls and explodes on the
floor, on X, or when a shot hits it.

Gate stage 1 (stage 16, overlay 22) has 17 bats with a bomb (main type 18, all `id` 0, records in
`docs/stages/gate-1.md`). The overlay 22 kit has the same 33 functions as the overlay 09 kit, of the
same sizes and identical after the addresses are masked, and the same animation table [certain for
the code match].

Overlay `09` (Rainy Turtloid), save state `turtloid-1.state.gz` (stage 6, area 0). Written with the
RE kit (`python3 -m mmxdemake.rekit --name batton-bone-b81`) and `docs/re-guide.md`. Units,
confidence tags and the shared engine rules: `docs/engine.md`. Records: `docs/stages/turtloid.md`.

## Spawn and set-up [certain]

- **HP 3**, contact damage **3**, `iframes` = 0.
- Draw priority 6.
- Speeds, acceleration and gravity are 0 at spawn. `flip` is not set by the set-up, so it is 0 (faces
  left) when the object comes from a stage record: the main slot is cleared when it is taken [likely].
- Boxes (y down):

  | address | box | area (x, y from the origin) | use |
  |---|---|---|---|
  | `0x800F5F04` | (−11, −10, 21, 23) | x −11..+10, y −10..+13 | body: hurt box 0; attack box after the bomb is gone |
  | `0x800F5F08` | (−9, 13, 16, 19) | x −9..+7, y +13..+32 | the held bomb: hurt box 1 |
  | `0x800F5F0C` | (−11, −10, 20, 42) | x −11..+9, y −10..+32 | attack box at spawn (body and bomb) |

  The 4 bytes at `0x800F5F10` (`F6 1B 1A 03`) look like a fourth box (−10, 27, 26, 3). No code in this
  object reads them.
- No terrain box: the bat does not collide with walls or floors.

## The `id` parameter [certain]

| id | bomb | animation | first step | behaviour |
|---|---|---|---|---|
| 0 | yes | 0 | 2 CHASE | flies above X, drops the bomb, flies away |
| 1 | no | 3 | 5 STRAIGHT | flies straight in its facing direction |
| 2 | no | 3 | 6 SCATTER | waits 0–63 frames, then flies off and rises |
| 4 | no | 3 | 7 HOMING | follows X for 300 frames, then flies up |
| any other (3, 5+, negative) | no | 3 | 6 SCATTER | as id 2, but without the wait |

The work word `+0x88` is 0 while the bat holds the bomb and 1 when it has none. Only id 0 starts
with 0.

## State machine

States (table `0x800F5FB0`): 0 `0x800EBFA4`, 1 `0x800EC108`, 2 and 3 `0x800EC2D8`. Not quite
standard: state 2 uses `DeleteObject`, but `DeleteObject2` when `game.stage` = 2, area 0; state 3
uses `DeleteObject2`.

### Main (state 1) [certain]

Each update:

1. The previous position is saved.
2. The step runs (table `0x800F5FC8`): 0 engine hit reaction, 1 RESUME, 2 CHASE, 3 DROP, 4 FLY AWAY,
   5 STRAIGHT, 6 SCATTER, 7 HOMING, 8 RISE.
3. Contact test. For **id 4 only**: when the contact hits the player (result 1), the step becomes
   8 (RISE).
4. When the step is not 0, it is saved in `+0x94` (for RESUME).
5. Weapon test. With the bomb: two hurt boxes, body (`0x800F5F04`) first, then bomb (`0x800F5F08`).
   Without the bomb: the body box only. The test stops at the first box that is hit.
6. **Killed:** when it is killed through the body box while it holds the bomb, the bomb is released
   (it falls as a live bomb). When it is killed through the bomb box, no bomb is released. Then death
   (see Death), state 2.
7. Otherwise: off screen 32 → state 3; else visible 32.

### RESUME (step 1) [certain]

The step becomes the value saved in `+0x94`. This returns the bat to its step after an engine hit
reaction (step 0). Which engine code sets step 0 and step 1 was not read.

### CHASE (step 2, id 0) [certain]

The bat faces X every update.

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | animation tick; timer `+0x7C` = 200 |
| 1 | up to 200 updates | see below |

Sub-step 1, each update:

1. Animation tick; move by the speed of the last update.
2. New speed: 1 px/frame towards the point (X.x, X.y − 80 px), that is 80 px (5 blocks) above X. Then
   `speed_x` × 1.5 (`speed_x + speed_x / 2`). So the bat flies at up to 1.5 px/frame across and up to
   1 px/frame up or down.
3. **Drop test:** when |X.x − bat.x| ≤ |`speed_x`| + 2 px, the step becomes DROP. The height is not
   tested: the bat drops when it is above X horizontally, at any height.
4. Otherwise the timer counts down; at 0 (after 200 updates, 3.3 s) the step becomes DROP.

### DROP (step 3, id 0) [certain]

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | animation 1 (release); `speed_x` = 0; attack box and hurt box = body box; timer `+0x80` = 5 |
| 1 | 5 updates | animation tick; at the end: `+0x88` = 1, **the bomb is made**, timer = 13 |
| 2 | 13 updates | animation tick; at the end: animation 3, step FLY AWAY |

The bat does not move in DROP (no move call). The bomb appears 1 update before the bat picture
without the bomb (animation 1 frame 3 starts after 6 ticks) [certain, from the frame times]. Total:
19 updates (0.32 s).

### FLY AWAY (step 4, id 0) [certain]

- Sub-step 0 (1 update): animation 3; `speed_x` = 3 px/frame in the facing direction (table
  `0x800F5F14`: −3 left, +3 right); `speed_y` = +0.3125 px/frame (up).
- Sub-step 1: animation tick and move, with no end. The facing is the one from the last CHASE update,
  so the bat goes on past X. It is deleted when it is more than 32 px off screen.

### STRAIGHT (step 5, id 1) [certain]

- Sub-step 0: animation 3; `speed_x` = ±3 px/frame by `flip`; `speed_y` = 0; attack box = body box;
  `+0x88` = 1.
- Sub-step 1: animation tick and move, with no end. With `flip` = 0 at spawn it flies left at
  3 px/frame (180 px/s) [likely].

### SCATTER (step 6, id 2, 3 and others) [certain]

- Sub-step 0 (1 update): animation 3. `speed_x` = ±3.5 px/frame by `flip` (table `0x800F5F1C`), then
  plus or minus (one random bit) a random amount of 0 to 0.99998 px/frame (two `GetRNG` bytes). So
  |`speed_x`| is 2.5 to 4.5 px/frame. `speed_y` = +1.5 px/frame (up). Attack box = body box;
  `+0x88` = 1. `accel_x` = −0.0625 px/frame² is written, but `MoveObject` does not use it.
- id 2: timer `+0x7C` = random 0–63, then sub-step 1. Other ids: sub-step 2 at once.
- Sub-step 1 (id 2 only): waits until the timer is 0 (timer + 1 updates). No animation tick and no
  movement: the bat shows the first frame of animation 3 and stays still.
- Sub-step 2: animation tick; move; `speed_y` += 0.0039 px/frame² (`0x100`). So it flies sideways and
  rises a little faster each update, with no end.

### HOMING (step 7, id 4) [certain]

The bat faces X every update.

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | attack box = body box; animation 3 and one tick; `+0x88` = 1; timer `+0x7C` = 300 |
| 1 | 300 updates (5 s) | animation tick; every 4th update a new speed of 1 px/frame towards X (the origin of X, not a point above); move |

At the end of the timer, or when it touches X (main step 3), the step becomes RISE. The speed is 0 for
the first 3 updates of sub-step 1 (the first aim is on the 4th update).

### RISE (step 8, id 4) [certain]

`speed_x` = 0, `speed_y` = +2 px/frame (up, 120 px/s); move; animation tick. No end: it is deleted when
it is more than 32 px off screen.

## Animations (table `0x800F5F8C`)

| # | sprites | ticks | loop | what (from the export) |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 | 3 3 4 3 3 5 (21) | loop | flying, holding the bomb under the body; the wings flap |
| 1 | 6 7 3 8 9 10 | 2 2 2 3 3 3 (15) | hold | release: frames 0–2 with the bomb, frames 3–5 without it, the wings open |
| 2 | 11 12 11 13 | 2 2 2 2 (8) | loop | the bomb alone (used by the bomb object) |
| 3 | 14 15 16 17 18 19 | 5 2 2 5 3 3 (20) | loop | flying without the bomb |
| 4 | 20 | 1 | hold | debris: a wing |
| 5 | 21 | 1 | hold | debris: a wing (purple) |
| 6 | 22 | 1 | hold | debris: a body piece (orange) |
| 7 | 23 | 1 | hold | debris: a small yellow piece (a horn or a claw) |

No frame has an event flag; the code tests none [certain].

## The bomb (visual object type 17, `0x800ECB48`) [certain]

Made by `0x800ECAA0` (in DROP, or at death of a bat that holds the bomb): flags `0x41`, type 17, var 0;
copies the parent (`docs/engine.md`). Draw priority 6. No bomb code reads its parent pointer.

Set-up (state 0):

- **HP 1**, contact damage **5**; `dmg_tbl_minor`.
- `speed_y` = −1 px/frame (down); gravity 0.1875 px/frame² (`0x3000`); `speed_x` = 0, no acceleration:
  it falls straight down. The fall speed reaches `fall_max` after about 29 updates.
- Attack box and hurt box `0x800F6028` = (−8, 10, 13, 26): x −8..+5, y +10..+36 from the origin.
- Terrain box `0x800F6024` = (−2, 22, 7, 14): centre (−2, +22), half sizes 7 × 14.
- Animation 2.

Main (state 1), each update:

1. Animation tick; `MoveObject2`.
2. Killed by a weapon: explosion, delete.
3. Else it touches X (`ContactMega` ≠ 0): explosion, delete.
4. Else on terrain: y += 32 px, explosion, delete. So the explosion shows 32 px below the bomb
   origin, near the floor [likely].
5. Else 32 px off screen: delete (no explosion). Else visible within 16 px.

State 2 deletes it (`DeleteObject3`). State 3 is an empty routine.

The bomb does no area damage: only its own contact box hurts X [certain for this code].

### Misc type 56 (not a real neighbour)

The misc table entry 56 points to `0x800ECBAC`, which is inside the bomb set-up, after its stack
set-up. Code that starts there has `v1` undefined and pops a stack frame it did not push. So it
cannot run correctly; this object never makes a misc type 56 [certain]. It is probably a stale table
entry [guess].

## Damage

- Bat: `dmg_tbl_normal`, 3 HP: nearly every hit kills it (ids 21 and 23 do 1). Both hurt boxes use the
  bat's HP: a shot on the held bomb hurts the bat.
- Bomb: `dmg_tbl_minor`, 1 HP: nearly any shot destroys it.
- Dealt: bat body 3 (with the bomb, the attack box also covers the bomb); bomb 5, and it explodes on
  that contact.

## Death

One explosion (`SpawnExplosion`), 4 debris pieces (animations 4–7), drop table 20, then state 2. A bat
with the bomb that is killed through its body releases the bomb.

## Sounds

No sound call in this code.

## Summary for a reimplementation

- HP 3; contact damage 3; almost every weapon kills it in one hit. No terrain collision.
- id 0: carries a bomb. Flies to a point 80 px above X at 1.5 px/frame across and 1 px/frame up or
  down; when it is above X (|dx| ≤ |vx| + 2 px) or after 200 frames, it stops for 19 frames, drops the
  bomb at frame 6, then flies away at 3 px/frame, rising 0.3125 px/frame.
- Bomb: HP 1, contact 5; falls from −1 px/frame with 0.1875 px/frame² gravity; explodes on the floor,
  on X, or when shot.
- id 1: flies straight at 3 px/frame. id 2 (after 0–63 frames) and others: flies at 2.5–4.5 px/frame
  across, rising from 1.5 px/frame. id 4: follows X at 1 px/frame (re-aimed every 4 frames) for 300
  frames or until it touches X, then rises at 2 px/frame.
- All variants leave only by flying off screen (32 px margin).

## Open questions

1. Which ids the Rainy Turtloid records use (the save state has no live Batton Bone).
2. `flip` at spawn: the set-up does not set it. 0 (left) is assumed from the cleared main slot; check
   whether the stage places id 1 and SCATTER bats to the right of X only.
3. The `game.stage` = 2 / area 0 test in the delete routine: stage 2 is not Rainy Turtloid (stage 6).
   It is probably copied code from another stage [guess].
4. Which engine code sets step 0 (hit reaction) and step 1 (RESUME).
5. The unused box at `0x800F5F10` and the unused misc table entry 56.
