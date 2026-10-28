# Mettaur D2 (stage 10, main object type 27): behaviour sheet

Mettaur D2 is the small hard-hat enemy of Shield Sheldon's stage, with a helmet and a shield. It
walks slowly, fires a straight shot when the player stays near it, and drops under its helmet when
the player fires a lemon at it from the front. Under the helmet every weapon is deflected. Its shot
is a separate visual object (type 32). The official name is from the Mega Man Knowledge Base list
of X6 enemies.

Overlay `10` (stage 7, Shield Sheldon), code at `0x800F1748`–`0x800F2600`. Save state
`sheldon-6_tile-shown` (stage 7, area 0, X, Normal; one Mettaur D2 live). Kit:
`python3 -m mmxdemake.rekit --name mettaur-d2`. Export: `mettaur-d2` (`enemy-46d9e2249f`). Units,
confidence tags and the shared engine rules: `docs/engine.md`. Records: `docs/stages/sheldon.md`.

## Spawn and set-up [certain]

- **HP 3**, contact damage **1**, `iframes` 0, draw priority 6, layer = `mega.layer`.
- Damage table `dmg_tbl_normal`; under the helmet `dmg_tbl_deflect` (see Damage).
- It faces the player at spawn: right when the player's x is equal or greater, else left.
- Gravity `0x4200` (0.2578 px/update²) while it falls; maximum fall 6.5 px/update (`MoveObject2`).
- Boxes (facing left; mirrored when facing right):

  | box | data | area |
  |---|---|---|
  | terrain box | `0x800FC4F4` (0, 5, 11, 8) | x − 11 … x + 11, y − 3 … y + 13 (22 × 16 px) |
  | contact box | `0x800FC4F8` (−11, −16, 21, 29) | x − 11 … x + 10, y − 16 … y + 13 |
  | hurt box | `0x800FC500` (−12, −6, 21, 20) | x − 12 … x + 9, y − 6 … y + 14 |
  | helmet box (contact and hurt, in GUARD) | `0x800FC4FC` (−12, −4, 21, 18) | x − 12 … x + 9, y − 4 … y + 14 |

- No move in the first update: it has no floor flag yet, so it starts in FALL (animation 3), drops
  to the floor and lands (animation 4), then walks. In `sheldon-6` it stands 3 px below its record
  point (y 435 on a floor at 448).

## The `id` parameter [certain]

| id | behaviour | in stage 7 area 0 |
|---|---|---|
| 1 | turns around at a ledge | all 4 records |
| 2, 3 | waits inside a solid tile at a 16 px grid point (x & ~15 + 8, y & ~15 + 20), with no contact and no hurt box; when that tile is not solid, it becomes `id` 0. `id` 2 is visible while it waits, `id` 3 is not | not used |
| other (0) | jumps off a ledge (when it does not touch a wall) | not used |

**The records stand on the hidden glass floors.** The four records (x 1656, 1824, 2080, 2256, y 432)
are over hidden floors 73, 75, 76 and 78 (`docs/stages/sheldon.md`). Each floor is 64 px wide and
solid also while it is not shown. With `id` 1 the Mettaur turns at the floor ends, so it paces on one
invisible floor. On floor 75 (x 1792–1855) it walks between x 1803 and 1844 [likely: the ledge probe
reads the floor value 60 as solid, as the player's floor test does].

## State machine

States (table `0x800FC64C`): 0 set-up `0x800F1D4C`, 1 main `0x800F1E8C`, 2 `DeleteObject`, 3 wait in
a tile `0x800F2014` (`id` 2 and 3), 4 `DeleteObject2`. Steps (table `0x800FC634`): 0 engine hit
reaction, 1 WALK `0x800F22FC`, 2 FALL `0x800F2150`, 3 GUARD `0x800F1C10`, 4 SHOOT `0x800F21F8`,
5 JUMP `0x800F20B4`.

### Main (state 1) [certain]

Each update, in this order:

1. **Ledge check** (only on the ground, `latch` = 0). No floor flag: step FALL. Else it tests one
   point 11 px in front of its origin and 21 px below it (8 px under the terrain box). When that
   point is not solid: `id` 1 turns around; another `id` jumps (step JUMP), unless a wall flag is set.
2. **Shot trigger** (see SHOOT).
3. **Guard trigger** (see GUARD).
4. The step.
5. Contact test (`ContactMega`), then the weapon test. Killed: death (below).
6. Off screen 64 → `DeleteObject2`. Else visible 37.
7. Squeeze test: floor and ceiling flags, or both wall flags, in the same update → explosion and
   debris, no drop.
8. `CollideCheck`.

### WALK (step 1) [certain]

- Speed **0.375 px/update** (`0x6000`) in the facing direction; no gravity. Animation 1.
- At a wall in front (wall flag on the facing side) it turns around.
- At a ledge: `id` 1 turns around (ledge check).

### FALL (step 2) [certain]

- `MoveObject2` with gravity 0.2578 px/update². The x speed of the walk or the jump is kept.
- On the floor: all speeds 0, animation 4 (land, 20 updates), then WALK.

### JUMP (step 5, `id` 0 only) [certain]

- Animation 2: no move for 16 updates (the crouch, until the frame with flag 1).
- Then up at 3.5 px/update (`0x38000`) and 1 px/update forward, with gravity 0.2578. After 14 updates
  (25.5 px up, 14 px forward) `speed_y` is below 0: step FALL, animation 3, still 1 px/update
  forward.

### SHOOT (step 4) [certain for the code; timing ±1 update]

- **Trigger:** each update on the ground, while it does not guard or shoot and the shot cooldown is
  0, a counter goes up when the player is less than 40 px above or below it and less than 80 px in
  front of it. The test has no back limit: the player **behind** it, at any distance, also counts.
  Out of the zone the counter goes back to 0. When the counter reaches **61** (about 1 s in the
  zone), SHOOT starts.
- It stands still. It does not turn to the player: it fires in the direction it faces.

  | update | event |
  |---|---|
  | 0 | animation 7 (raise the shield and aim) |
  | 20 | the shot (frame 3); sound bank 2, id 46 |
  | 36 | the end: WALK |

- Then a shot cooldown of **120 updates**. So the shortest time from one shot to the next is about
  36 + 120 + 61 = 217 updates (3.6 s).

### GUARD (step 3) [certain for the code; timing ±1 update]

- **Trigger:** each update on the ground, while it does not guard and the guard cooldown is 0, it
  looks at the 32 player weapon objects for a **lemon** (weapon id 0). It guards when all of these
  are true:
  - the player is in front of it and faces it;
  - the lemon is less than 24 px above or below it;
  - the lemon is less than 128 px in front of it (or behind it).
- Charged shots and the other weapons do not start the guard.

  | update | event |
  |---|---|
  | 0 | animation 5: it drops under the helmet; normal table and hurt box until update 9 |
  | 9 | helmet down (frame 3, flag 1): `dmg_tbl_deflect`; contact box and hurt box = the helmet box |
  | 60 | animation 6: it comes out again (still deflecting) |
  | 80 | normal table and boxes again; WALK |

- Then a guard cooldown of **120 updates**: no guard in that time.
- **A lemon during SHOOT** starts GUARD at once; the shot of that SHOOT is lost if it was not fired.
  The SHOOT end never runs, so the "shooting" bit stays set, and **this Mettaur never shoots again**
  until it is deleted and spawned again [certain for the code; not checked in play].

### Wait in a tile (state 3, `id` 2 and 3) [certain]

- Each update: when the tile at its origin is not solid, it goes to state 1 (WALK) with `id` 0 and
  its normal boxes. Off screen 64 → deleted. `id` 2 is visible (32 + 5 px margin) while it waits.

## Animations (table `0x800FC5F8`) [certain]

| # | what | frames |
|---|---|---|
| 0 | stand | sprite 0, held |
| 1 | walk | sprites 1–10, 64 updates, loop |
| 2 | jump crouch | 6 frames, flag 1 on frame 2 (update 16), held |
| 3 | fall | sprites 15, 16, loop |
| 4 | land | 6 frames; the last frame after 20 updates, held |
| 5 | drop under the helmet | 10 frames, flag 1 on frame 3 (update 9), held |
| 6 | come out from the helmet | animation 5 backwards; the last frame after 20 updates, held |
| 7 | shoot | 8 frames, flag 1 on frame 3 (update 20); the last frame after 36 updates, held |
| 8 | the shot | sprites 30–32, 2 updates each, loop |
| 9–12 | debris | one sprite each |
| 13 | sprite 37 | not started by this code |

## The shot (visual object type 32) [certain]

- Copies the parent (`docs/engine.md`). Starts 16 px in front of the Mettaur and 3 px lower.
- Speed **2.5 px/update** (`0x28000`) straight forward. No gravity.
- Contact damage **2**. Box `0x800FC660` (−5, −4, 9, 7): about 9 × 7 px. Terrain box half size 9 × 7
  (for `CornerCheck`). `dmg_tbl_one_hit` with HP 0: one hit destroys it, with no explosion, but only from
  a weapon with `+0x9C` ≠ 0. `ContactWeapon` skips the other shots, so plain buster shots pass
  through it (`docs/damage.md`, "Shots that can hit enemy shots") [certain for the code].
- Standard shot ends: it vanishes on the player, bursts on walls (`SpawnHitEffect`), and vanishes 32
  px outside the view. Visible within 16 px.

## Damage

- Taken: `dmg_tbl_normal`, 3 HP: every weapon with 3 or more damage kills in **1 hit** (lemon, half
  charge, full charge, sabre). Ids 21 and 23 (1 damage) need 3 hits. No invincibility, no
  knock-back [certain].
- Under the helmet (GUARD from update 9 to 80): `dmg_tbl_deflect`. Every weapon is deflected, except
  ids 7, 23 and 79, which pass through [certain].
- Dealt: body **1**, shot **2** (before the player's damage scaling).

## Death [certain]

- Explosion, 4 debris pieces (animations 9–12), one drop roll with table 18, then `DeleteObject`
  (the record can spawn it again).
- **No drop when weapon id 55 kills it** (the kill result −56). Which weapon id 55 is, is not known.
- Squeezed (floor and ceiling, or both walls): explosion and debris, no drop.

## Sounds

Bank 2, id 46: each shot.

## Summary for a reimplementation

- Small ground enemy, 3 HP (1 hit from almost anything), 1 contact damage.
- Walks at 0.375 px/update and turns at walls; `id` 1 also turns at ledges (in stage 7 it paces on
  one hidden glass floor); other ids jump off ledges.
- After about 1 s with the player within 40 px vertically and 80 px in front (or anywhere behind),
  it stops and fires one shot forward at 2.5 px/update, 2 damage. 120 updates of rest after a shot.
- When the player faces it from the front and fires a lemon that comes within 128 px at its height
  (±24 px), it drops under its helmet for 80 updates and deflects everything from update 9 to 80.
  120 updates of rest after a guard.
- Charged shots do not start the guard, so a charged shot kills it at once.

## Open questions

1. Which weapon id 55 is (no drop).
2. Whether Zero can start the guard: the trigger looks only for weapon id 0.
3. Not checked in play: the guard timing, the shot when the player is behind, the shot lock after a
   guard during SHOOT, the pace on the hidden floors.
4. `id` 0, 2 and 3 are not used in stage 7 area 0; other stages are not searched.
5. Animation 13 (sprite 37) has no user in this code.
