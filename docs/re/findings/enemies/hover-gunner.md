# Hover Gunner (stage 02, main object type 5): behaviour sheet

Hover Gunner is the small flying mechaniloid of the intro stage (called "small jet" and "propeller
flyer" in earlier notes). It flies in fast, stops near X, fires aimed shots, then fires a fan of shots
and flies up out of the screen. Its shot is a separate visual object (type 4 in the intro visual
table), and each shot has a muzzle flash (misc object type 6).

Overlay `02` (intro). Read by hand from the disassembly (before the RE kit). Units, confidence tags
and the shared engine rules: `docs/engine.md`. Records (20–24): `docs/stages/intro.md`.

## Spawn [certain]

- Hover Gunner has no difficulty code of its own. Difficulty only selects records by `kind`.
- Record 24 is a real Hover Gunner on every difficulty. It flies in from the right as soon as it
  spawns, so it is not seen at its marker [likely].

## The `id` parameter

| id | behaviour | in the intro |
|---|---|---|
| 0 | starts at once: faces left and flies left (enters from the right edge) | 22, 23, 24 |
| 1 | waits, invisible and inactive, until X is 176 px (11 blocks) or more to its right; then faces right and flies right (comes from behind X, from the left edge) | 20, 21 |
| 2 | like 0, but the fly speed is 0: it hovers in place | not used |
| 3 | a turret: bobs up and down and fires one aimed shot about every 90 frames | not used |
| bit 7 set | made by another object, not by the stage list: drifts 0.5 px/frame for 48 frames, then as id 0 | not used |

[certain for the code paths; ids 2, 3 and bit 7 are outside the intro, so they are summarised only.]

## Set-up (spawn frame) [certain]

- HP 5. Contact damage 3. Draw priority 4.
- No terrain box: Hover Gunner ignores walls and floors.
- All speeds 0. `range` = one of 64, 80, 96, 112 px (4, 5, 6, 7 blocks), chosen at random once per
  spawn (`random & 3`).
- Animation 0 (fly). Faces left (id 0) or waits (id 1, see above).

## Geometry [certain for the numbers]

| Box | Data | Facing left | In blocks |
|---|---|---|---|
| Hurt box | (−9, −9, 23, 18) | X − 9 … X + 14, Y − 9 … Y + 9 | 1.4 × 1.1 |
| Contact box | (0, 0, 11, 11) | X … X + 11, Y … Y + 11 | 0.7 × 0.7 |

The contact box is only the lower-rear part of the hurt box. That is what the data says; check it in
play before you copy it [guess about the intent].

## Behaviour

Each frame in the main state, in this order [certain]:

1. Weapon test. If HP ≤ 0: death (see below).
2. **Range trigger.** If Hover Gunner is flying (step FLY) and `|X.x − gunner.x| ≤ range`, it goes to
   step BURST at once. Height does not matter.
3. The current step.
4. Contact test.
5. Off screen 64 → deleted (the record can spawn it again).

### Steps

**FLY** (start of a Hover Gunner with id 0, or after id 1 wakes up):
- Speed 4 px/frame (15 blocks/s) in the facing direction, no vertical motion. Animation 0.
- After 60 frames (240 px, 15 blocks) without a trigger it gives up: go to LEAVE.

**BURST** (3 aimed shots):

| frame | event |
|---|---|
| 0 | stops (all speeds 0) |
| 20 | aims at X: 16 directions (22.5° apart). Faces X horizontally and shows the pose for that direction |
| 22 | shot 1 |
| 31 | shot 2 |
| 40 | shot 3 |
| 41–61 | pause (20 frames) |
| 62 | burst done |

- All three shots go in the direction aimed at frame 20. Hover Gunner does not track X during the burst.
- After the first burst: back to FLY (speed 4 px/frame, 60-frame limit). If X is still within
  `range`, the trigger fires on the next frame, so Hover Gunner stays in place and fires again.
- After the second burst: go to FAN.
[certain for the order; the frame numbers are a hand simulation of the code, likely ±1]

**FAN** (9 shots that sweep around the aim):
- Frame 0: stops and aims at X (16 directions). At frame 20 it shows the pose for that direction and
  faces X.
- From the aim, it takes two side directions: for an aim along an axis, the directions 22.5° on each
  side; for a diagonal aim, the two axes next to it. Example: aim straight left → side A = 22.5° above
  left, side B = 22.5° below left. Aim up-left (45°) → side A = up, side B = left.
- Shots at frames 21, 31, 41, … , 101 (every 10 frames), in this order:
  aim, A, aim, B, aim, A, aim, B, aim.
- About 3 frames after the ninth shot: go to LEAVE.
[certain for the directions and order; frame numbers likely ±1]

**LEAVE**: speed 2 px/frame straight up, forever. The range trigger is off. It is deleted when it is
64 px above the view.

So a Hover Gunner that X does not outrun does: fly in, burst, burst, fan, fly up and away. It never comes
back down. Total time from the first stop to LEAVE: about 63 + 63 + 104 ≈ 230 frames (≈ 3.8 s).

## The shot (visual object type 4) [certain]

- Made at Hover Gunner's position plus a muzzle offset for the direction:

  | aim | offset (x, y) |
  |---|---|
  | right / left (along the axis) | (+13, 0) / (−13, 0), by facing |
  | up-right, 3 directions | (+10, −12) |
  | up | (0, −16) |
  | up-left, 3 directions | (−10, −12) |
  | down-left, 3 directions | (−10, +12) |
  | down | (0, +16) |
  | down-right, 3 directions | (+10, +12) |

- Speed 2 px/frame (7.5 blocks/s) in a straight line, in one of 16 directions. No gravity.
- HP 1, damage 3. Box (−3, −6, 8, 8): 0.5 × 0.5 block. Terrain box half size 4 × 4.
- Standard shot ends (`docs/engine.md`): it can be shot down (small explosion), it bursts on walls,
  it vanishes on the player and 32 px outside the view.
- Animation: 4 sprites, 2 frames each, looped.
- Sound: bank 2, effect 0, once per shot.

The muzzle flash (misc object type 6) is drawn at the same offset, with a 7-sprite animation of 17
frames. It does no damage.

## Damage

- Taken: `dmg_tbl_normal`, 5 HP: 2 lemons, 1 half charge, 1 full charge, 2 sabre hits. No
  invincibility; 1-frame flash; no knock-back [certain].
- Dealt: body 3, shot 3.

## Death [certain]

Explosion effect, one drop roll with table 18, 5 debris pieces (animations 8–12). Deleted; the record
can spawn it again.

## Summary for a reimplementation

- Flying enemy, 5 HP, 3 contact damage, ignores terrain.
- Flies in fast (4 px/frame), stops when X is 4–7 blocks away horizontally (random per spawn), fires
  2 bursts of 3 aimed shots, then a fan of 9 shots, then flies straight up (2 px/frame) and leaves.
- Shots: 2 px/frame, 16 directions, 3 damage, can be shot down, burst on walls.
- id 0 enters from the right at once; id 1 waits until X has passed it by 11 blocks, then comes from
  behind.
