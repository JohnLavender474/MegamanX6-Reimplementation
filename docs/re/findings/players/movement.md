# Player ground movement (X and Zero, main program): behaviour sheet

Part 2a of the player RE plan: stand, walk start, walk, stop, land, the ground dash and the crouch.
The code is in the main program and shared by X and Zero; the few character tests are marked. The
step numbers and the "try" chain are in `docs/players/map.md`.

Source: the X kit (`python3 -m mmxdemake.rekit --player --state intro-1_start.state.gz`), with notes,
pseudo code and reconstructed C in [`~/mmx-data/x6/re/x/movement/`](../../work/x/movement/). Units and tags as in
`docs/engine.md` ("Conventions"); speeds are 16.16 values in px per update. `PlayerMove`
(`0x8002D5D0`) adds `speed_x` to x and subtracts `speed_y` from y, so a positive `speed_y` is up.

## Inputs

| field | meaning |
|---|---|
| `mega+0x7C` | buttons held: `0x1` right, `0x2` left, `0x4` up, `0x8` down, `0x100` dash [certain for the tests; the names from what each test starts] |
| `mega+0x80` | buttons pressed this update: `0x1` right, `0x2` left, `0x80` jump, `0x100` dash [likely: new presses] |
| `mega+0x89` | blocked sides: bit 0 right, bit 1 left (from `collide_flags`, set by `PlayerUpdate`) [certain for the use] |
| `mega+0xD3` | any lock: `PlayerUpdate` sets it to `+0xD0` (event request) or `game+0x0F` (exit request) or `+0xD4` (gate). All try functions do nothing while it is not 0 [certain] |

Facing: `flip` = `0x40` faces right, 0 faces left. Right held sets `0x40`, left held sets 0.

## Physics constants

`0x8003D598` writes the base values on the beam-in landing (step 0). `0x8003D630` then adds the part
bonuses from `mega+0xEC` (the part bits of the character: `game+0x74 + 4 × var`, copied by
`0x8003D718`). The same values were in RAM in `intro-1_start`, `yammark-1_start` (X) and
`turtloid-6_zero-player` (Zero), all with no parts [certain].

| field | base | used here for | part bonus |
|---|---|---|---|
| `+0xF0` | 0.5 | walk start speed | bit `0x4`: +0.25 |
| `+0xF4` | 2.0 | walk speed | bit `0x4`: +1.0 |
| `+0xF8` | 5.8125 | jump speed (up) | |
| `+0xFC` | −6.5 | maximum fall speed (`fall_max`) | |
| `+0x100` | 0.2578125 | (air, part 2b) | |
| `+0x104` | 6.5 | dash start speed | bit `0x10`: +1.625 |
| `+0x108` | 4.125 | dash minimum speed, and the speed at the dash end | bit `0x10`: +1.03125 |
| `+0x10C` | −0.1875 | dash slow-down per update | bit `0x10`: +0.046875 |
| `+0x110` | −1.4375 | (part 2b) | 0 when `mega+0xE7` = 2 |
| `+0x114` … `+0x130` | 5.625, 1.0, 1.5, 2.0, 2.0, 1.5, 4.125, 4.125 | (parts 2b, 2c) | `+0x12C`, `+0x130`: bit `0x10`: +1.03125 |
| `+0x134` | 0.2578125 | jump gravity | bit `0x8`: −0.064453125 |

The part names are not read yet. By their effect, bit `0x4` speeds up walking, bit `0x8` gives a
higher jump and bit `0x10` a faster dash [likely].

## The ground steps

Each ground step runs the same checks in order and stops at the first one that starts something
(it returns not 0). The X-only and Zero-only checks are in the overlays; the list is in
`docs/players/map.md`. The checks of this part:

| check | function | starts |
|---|---|---|
| requests | `PlayerRequests` `0x8003A628` | an exit or event step |
| ladder | `0x8003A3DC` | up held on a ladder tile (32): ladder from below (`0x8003B63C`, animation 27); down held on a ladder top (33): ladder down (`0x8003B708`, animation 29) |
| weapon pose | `0x8003F13C` (X only) | a special-weapon pose (animation 88 for weapons 5 and 14) |
| super jump | `0x8003A870` | up held and jump pressed, X with `mega+0xE7` = 2 or Zero with `mega+0xC9` bit 1: step 31 at 2 × jump speed (11.625), animation 10 |
| dash and jump | `0x80039C40` | dash (below), else jump pressed: jump (step 6, part 2b) |
| crouch | `0x80039C9C` | down held and `game+0xC9` (`0x800CCF99`) bit 3: step 26 |
| direction | `0x80039CFC` / `0x80039D7C` | right or left held and that side not blocked: sets `flip` and `speed_x` = ± walk start / walk speed |

### Stand (step 2)

The checks above, then walk start when a direction is held. Else the character's idle. X
(`0x801EB61C`): in the update of a shot (`+0x8F`), the stand shot animation by id
(`0x801F3698`, `docs/players/attacks.md`); when the shot pose has just ended
(`+0x90`), the stand animation again; in animation 6 with HP back at the limit or more, animation 5;
else it runs the stand animation. Zero (`0x801EC9C4`) is in part 2c [open].

The stand animation (`0x8003D10C`): animation 5, or 6 (low HP) when HP < max HP / 3, with max HP =
`game+0x5B + var` (32 in the three states).

### Walk start (step 3) and walk (step 4)

- Walk start (`0x8003AE8C`): animation 7, `speed_y`, `accel_x` and `gravity` = 0, one `PlayerMove`,
  step 3. Each update: the checks, then the direction test with the walk start speed (0.5). With a
  direction held: `AnimeTick`, `PlayerMove`; when the animation reaches its last frame (`next` < 0),
  walk.
- Walk (`0x8003AED4`): animation 8, the same set-up, step 4. Each update: the checks (walk start
  has no super jump and no crouch check; walk has both), then the direction test with the walk speed
  (2.0), `AnimeTick` and `PlayerMove`.
- With no direction held (or the side blocked), stop (`0x8003ADB4`).

Animations 7 and 8 become 119 and 120 while X is in the shooting pose (`mega+0x8E` not 0: the
animation number + 112, `0x8003F4C4`). Each update, `0x8003F508` changes between the two at the
same frame and the same remaining time, so a shot does not restart the walk cycle [certain].

### Stop (`0x8003ADB4`) and step 5

- X: stand at once (step 2). In the shooting pose: animation 94, from frame 2 when the shot timer
  `+0x91` < 5, frame 1 when < 9, else frame 0 with its time cut by `+0x91` − 8; else the stand
  animation.
- Zero: animation 9 and step 5 (a stop pose), which goes back to walk start or stand [certain for
  the step; step 5 itself is Zero's, read in part 2c].

### Land (step 8)

Entered by `0x8003B054` from the air steps: sound (bank 1, id 2), clears the dash flags
(`0x8003CD9C`: `+0x84`, `+0x86` = 0, `+0x8C` = −1 when it was set) and the jump latch `+0x67`. Then
the checks; with nothing started: animation 12 (or its shooting version), step 8. Each update: the
checks (also dash or jump, `0x80039BC4`, and walk when a direction is held); when the animation
ends, stop. So a landing can be cut short by any action, and by walking.

### Dash (step 12)

Entered by `0x8003A9C4` when `0x80039EB0` finds the dash: dash pressed (`mega+0x80` bit `0x100`), or
`mega+0x87` not 0 [open: what sets `+0x87`]. It faces the held direction and sets `speed_x` = ±
dash start speed (6.5) unless that side is blocked.

- With jump pressed in the same update: a dash jump. `speed_x` = 0, `speed_y` = jump speed,
  `gravity` = jump gravity, `+0x67` = 1, `+0x8C` = 1 (dash flag), `+0x86` = 2, `+0x8B` = 10,
  animation 10, a random voice (`0x80073CE4` + `var` × 4, 1 in 2), step 6.
- Else: animation 16, `speed_y` and `gravity` = 0, `accel_x` = slow-down (−0.1875), dash timer
  `+0x85` = 30 updates (15 with part bit `0x10`), step 12.

Step 12 runs `AnimeTick`, then a sub-step from the table `0x80073C04`, then sets the terrain box
again when the step is still 12:

1. **Start** (`0x800357A8`). X does not move in this sub-step (no move call). The weapon pose, the
   Zero checks, and jump (a plain jump: `+0x8C` is not set yet). When a frame has event flag `0x20`: the dust effect (`0x8003C6A4`). When a frame
   has flag `0x80`: the dash sound (bank 1, id 5), `+0x8C` = 1, sub-step 1.
2. **Dash** (`0x800358D4`). The checks; jump starts the jump with `+0x8C` = 1 still set, so a dash
   jump [likely; the jump is part 2b]. Then `0x80039FFC`: when the side in front is blocked, the dash
   ends. Else `PlayerMoveAccel` (`0x8002CFB0`) moves X by `speed_x`, then adds `accel_x` (the
   slow-down) in the facing direction; `speed_x` stays at the minimum speed (4.125) or more, and the
   timer counts down. The dash ends when the timer reaches 0,
   when neither dash nor a direction is held (`mega+0x7C` & `0x103` = 0), or when the opposite
   direction is held. Flag `0x40` in a frame calls `0x8003D26C` [open]; every 4 updates (timer & 3 =
   0) the afterimage or dust (`0x8003C7D8`) [likely]. The terrain box is the dash box
   (`0x8003CD00`: `0x80073D58` for X, `0x80073D5C` for Zero, none when `+0xD5` < 0).
3. **End** (`0x8003B218`, then `0x80035A2C`). Animation 17, stop sound (bank 1, id 5) with
   `StopSound` (`0x8001750C`), `speed_x` = ± minimum speed (4.125), `accel_x` = slow-down, `+0x8C` = −1,
   `+0x84`, `+0x86` = 0, sub-step 2. Each update: the checks; a sound (bank 1, id 6) at flag `0x80`;
   after flag `0x40` and with the timer at 0, the slow-down until `speed_x` reaches 0 (it is set to 0
   at a wall); stand when the animation ends.

So after the start frames, a dash lasts at most 30 updates: X moves 6.5, 6.3125, … px per update
(−0.1875 each update) down to the minimum 4.125 after 13 updates; then the end slide starts at
4.125 and slows by 0.1875 per update to 0 [certain for the numbers; the length of the start frames
is in animation 16; the feel needs a play check]. In a head wind (`mega.wind`), `PlayerMoveAccel`
moves x by 86/100 of `speed_x` (`PlayerMove` uses 3/4) [certain for the code].

### Crouch (step 26)

Entered by `0x8003B9AC`: animation 81, or 109 when X is in the shooting pose (the halfword
`+0x8E` is not 0: `+0x8E` shot pose or `+0x8F` shot in this update), step 26. Each update: `flip`
from the direction pressed, the requests, `0x8003F644` [open], then dash and jump. A dash from the
crouch starts at the dash's main phase (animation 128 in the shooting pose, else 16, at frame 5; the
dust, the dash sound, `+0x8C` = 1, sub-step 1). Else the Zero checks; with down released and a
direction held: walk start. Sub-steps [certain for the code]:

- 0: crouch down; sub-step 1 at the last frame.
- 1: crouched. `DashTerrainBox` (`0x8003CD00`). In the update of a shot (`+0x8F`), X only:
  animation 139 when the fired id (`+0x96`) is 18, 19, 20, 43 or 46 (the full charges), else 108.
  Else, while `+0x8E` = 0 (no shooting pose), X only: animation 81 from frame 2
  (`PlayerSetAnimeAt`, each such update).
- Down released: animation 110 when X is in the shooting pose (the halfword `+0x8E` is not 0), else
  82; sub-step 2; stand at the last frame.

Seen in `wolfang-4_x-crouch-shot`: X (Falcon) in sub-step 1, animation 108 frame 1 (sprite 212),
`+0x8E` = 1, `+0x91` = 12, one lemon 8 updates after the shot [certain].

`game+0xC9` is `0x18` in all states read, so the crouch is on in normal play [likely].

## Animations used here

| animation | what (from the strips) |
|---|---|
| 5, 6 | stand; stand with low HP |
| 7, 8 | walk start, walk (119, 120 while shooting) |
| 9 | Zero's stop |
| 10 | jump start (dash jump, super jump) |
| 12 | land (124 while shooting) |
| 16, 17 | dash, dash end (128, 129 while shooting) |
| 27, 29 | ladder from the ground, ladder down |
| 81, 82 | crouch down, stand up (109, 110: the same in the shooting pose; 108: crouch shot; 139: crouch full-charge shot) |
| 88 | stand shot of the lemon; also the special-weapon pose of weapons 5 and 14 |
| 90, 91 | stand shot of the half and full charge |
| 92 | pose weapons |
| 94 | stand after a stop while shooting |

The frame flag byte holds the frame number in its low 6 bits (`0x8003F508` uses `flag & 0x3F` as
the frame to restart at) and events in bits `0x20`, `0x40`, `0x80` [certain]. Timings from the X
export (ticks per frame, in updates) [certain for the data; the update of the change depends on
the order of `AnimeTick` and the test, likely ± 1]:

| animation | frames (ticks) | so |
|---|---|---|
| 7 walk start | 3, 3, then a hold frame | walk after 6 updates |
| 8 walk | 14 frames, 2–3 each, loop | one cycle = 32 updates |
| 12 land | 3, 3, 2, then a hold frame | stop after 8 updates |
| 16 dash | 1, 1, 2 (flag `0x20`: dust), 1, then the frame with flag `0x80`; loop from frame 8 | 5 updates still, then the dash |
| 17 dash end | 3 (flag `0x40`), 2, 7 (flags `0x80`, `0x40`: sound 6), 5, hold | |
| 5 stand | 14 frames, 11–13 ticks (a blink cycle) | |

## Sounds

| bank, id | when |
|---|---|
| 1, 2 | land |
| 1, 5 | dash start (stopped at the dash end) |
| 1, 6 | dash end, at a frame with flag `0x80` |
| 1, 1 | super jump |
| voice from `0x80073CE4` | dash jump, 1 in 2 |

## Ice and glass floors

Collision 60 with stage logic 7 (Shield Sheldon's glass, Blizzard Wolfang's ice, stage 4 area 1
under Nightmare Effect 2) adds a slide to the steps above, and the ice slopes (17–28) push the
player down at 3.5 px/update. This is stage code, not player code: `docs/engine.md`, "Slippery
floors and slopes (stage logic 7)".

## Summary for a reimplementation

1. A ground step runs, in order: requests, ladder, weapon pose (X), super jump, dash or jump,
   crouch, then the direction test; the first one that starts an action ends the update.
2. Walk start at 0.5 px/update until its animation ends, then walk at 2.0. No acceleration.
3. Release the direction: X stands at once; Zero plays a stop pose (step 5).
4. Dash: 6.5 px/update, −0.1875 per update down to 4.125, at most 30 updates; it ends at a wall,
   when the opposite direction is held, or when dash and the directions are released. Then a slide
   from 4.125 down to 0.
5. Jump during the dash or on the same update as the dash: a dash jump (`+0x8C` = 1).
6. Land: a short animation that any action cuts.
7. Parts change the constants: walk +0.25/+1.0, dash +1.625 and 15 updates, jump gravity −0.064.

## Open questions

- What sets `mega+0x87` (a dash without the button).
- `0x8003D26C`, `0x8003F644`, the dust and afterimage objects (`0x8003C6A4`, `0x8003C7D8`).
- Zero's idle (`0x801EC9C4`) and step 5.
- (answered) The part names for `mega+0xEC` bits `0x4` Speedster, `0x8` Jumper, `0x10` Hyper Dash
  [likely] are in `docs/engine.md`; `+0xE7` = 2 is the Shadow Armor (spike-proof, no wall slide,
  super jump) [names likely].
- What `game+0xC9` bit 3 is (it allows the crouch).
- Play checks: the dash length and slide, the walk start time.
