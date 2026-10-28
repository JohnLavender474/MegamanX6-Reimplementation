# Meta Wheel F (stage 02, main object type 2): behaviour sheet

Meta Wheel F is the spiked wheel of the intro. It rolls along the floor, stops at walls, and turns
round. Only the Z-sabre hurts it. A second object (misc type 2) follows it and draws the glowing
studs on its rim.

Overlay `02` (intro). Written with the RE kit (`python3 -m mmxdemake.rekit --name meta-wheel-f`) and
`docs/re-guide.md`. Units, confidence tags and the shared engine rules: `docs/engine.md`. Records
(31–39): `docs/stages/intro.md`.

## Spawn and set-up [certain]

- **HP 32**, contact damage **4**.
- Boxes:
  - terrain box `0x800FAB7C` = (0, 0, 30, 30): a 60 × 60 px body for the walls and the floor;
  - contact box `0x800FAB80` = (−20, −20, 40, 40): 40 × 40 px around the origin;
  - hurt box `0x800FAB84` = (−23, −23, 46, 46): 46 × 46 px around the origin.
- Gravity 0.125 px/frame² (`0x2000`); the fall speed is limited by `fall_max`.
- Draw priority 6.
- It starts in the fall step, so it drops onto the floor first.

## The `id` parameter [certain]

| id | starting direction | animation |
|---|---|---|
| 0 | rolls **right** | 2 (sprites 7 → 0) |
| ≠ 0 | rolls **left** | 0 (sprites 0 → 7) |

All intro records have id 1 except record 38 (Hard only), which has id 0. The direction is kept in the
work byte `+0x07` (0 = right, `0x40` = left). The sprite is never mirrored (`flip` stays 0); the
wheel is symmetric, and the direction shows only in the order of the roll frames.

## State machine

Standard states (table `0x800FAD04`): 0 `0x800EB450` (set-up; also makes the stud overlay),
1 `0x800EB5E4`, 2 `0x800EBC98`, 3 `0x800EBCB8`.

Each frame, before the state routine: the previous position is saved, and the vertical speed is set
to 0 when the wheel is on the floor. After it: `CollideCheck`, then the wheel-to-wheel test (below).

### Main (state 1)

1. **Fall check:** when it was on the floor and the floor is gone, it goes to FALL with vertical
   speed 0.
2. Runs its step (table `0x800FAD14`): 0 engine hit reaction, 1 ROLL, 2 FALL, 3 TURN.
3. **Speed limit:** 0.75 px/frame in the rolling direction.
4. **Death:** when a weapon kills it, **or when it is squeezed** (floor and ceiling contact in the same
   frame): see Death, then state 2.
5. Otherwise: contact test; off screen 128 → state 3; visible 96.

### ROLL (step 1)

- Moves by its speed; accelerates **0.047 px/frame²** (`0xC00`) in its direction, up to 0.75 px/frame
  (about 16 frames from a stop); gravity applies.
- The roll animation runs: 8 frames of 4 updates (32 updates per turn of the picture).
- **Wall ahead** (on the side it rolls to): sound (bank 2, id 4), the position snaps to a whole pixel,
  speed and acceleration become 0, then TURN.

### FALL (step 2)

- Same movement as ROLL in the air; the animation keeps running.
- **Lands:** sound (bank 2, id 4), back to ROLL with the normal acceleration.

### TURN (step 3) [certain for the timing]

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | sets a timer of 90 |
| 1 | 90 updates | waits (the wheel picture does not turn) |
| 2 | 20 updates | waits; the stud overlay plays its flash animation (4–7) |

Then: the direction flips, the roll animation of the new direction starts at the frame next to the
current picture (so the image does not jump), and the wheel goes to FALL, which on the floor becomes
ROLL at once. So at a wall it stops for **about 111 updates (1.85 s)**, then rolls back.

### Wheel against wheel [certain for the code, likely for the effect]

After its own update, each wheel tests the terrain box of every later main object of the same type.
When two boxes overlap:

- each wheel gets a "wall" flag on the side of the other one, so **both stop and turn round**, as at a
  wall;
- when one of them is in the air and the other is not, the one below gets a "ceiling" flag. With its
  floor flag that is a squeeze, so **a wheel that lands on another one destroys the lower one** (in the
  next main update) [likely].

## Animations (table `0x800FACD4`)

| # | what | frames |
|---|---|---|
| 0 | roll, moving left | sprites 0–7, 4 updates each, loop |
| 1 | stud lights (overlay) | sprites 8–23, 4 each, loop |
| 2 | roll, moving right | sprites 7–0, 4 each, loop |
| 3 | stud lights, other order (overlay) | 16 frames, 4 each, loop [not seen used] |
| 4–7 | stud flash before the turn (overlay), one per roll phase | 8 frames (4 3 2 1 2 3 3 1 updates = 19), flag 1 on the last, hold |
| 8, 9, 10 | debris pieces | 1 frame each |

## The stud overlay (misc object type 2, `0x800EBFEC`) [certain]

- Made in the set-up; copies the parent (`docs/engine.md`). Draw priority 5.
- Each update it copies the wheel's position, and picks its sprite from table `0x800FAD30` by the
  wheel's sprite and the frames left in the wheel's frame: the four studs change four times per roll
  frame, so the lights seem to run round the rim.
- During TURN it shows a fixed sprite; in the last 20 updates it plays the flash (animation 4–7, by
  the roll phase), then follows again.
- It deletes itself when the wheel dies or when it is far off screen.

## Damage

- Taken: `dmg_tbl_sabre_only` (in full in `docs/engine.md`). Every X-Buster shot (ids 0–23) is
  deflected; the sabre (ids 24–31) does 4 (id 25: 6). So 32 HP is about 8 sabre hits.
- Dealt: contact 4.

## Death

5 explosions at random offsets (`SpawnExplosionRand`, ±16 px), 3 debris pieces (animations 8–10),
drop table 20, then state 2.

## Sounds

Bank 2, id 4: wall hit and landing.

## Summary for a reimplementation

- 32 HP; only the sabre hurts it (4 per hit, 8 hits); contact damage 4.
- Rolls along the floor: 0.047 px/frame² acceleration, 0.75 px/frame top speed; falls with gravity
  0.125 px/frame².
- At a wall (or another wheel): stop, wait about 111 frames (the rim lights flash in the last 20),
  then roll the other way.
- Squeezed between floor and ceiling, or landed on by another wheel: destroyed.
- 60 × 60 px body; 40 × 40 contact box; 46 × 46 hurt box.

## Open questions

1. Whether a wheel landing on another one really destroys it in play (the squeeze rule says so).
2. Animation 3 (a second stud order) has no user in this code.
3. The exact meaning of weapon ids 32–63 (some hurt it).
