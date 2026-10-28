# Player air movement (X and Zero, main program): behaviour sheet

Part 2b of the player RE plan: jump, fall, wall grab, wall kick, wall slide, air dash, the rope and
bar hangs, and the super jump. The code is in the main program and shared by X and Zero; it calls
each character's overlay for the character tests. The ground part (inputs, `PlayerMove`,
`PlayerMoveAccel`, the dash, the shooting-pose rule) is in `docs/players/movement.md`. The step
numbers and the "try" chain are in `docs/players/map.md`.

Source: the X kit (`python3 -m mmxdemake.rekit --player --state intro-1_start.state.gz`), with
notes, pseudo code and reconstructed C in [`~/mmx-data/x6/re/x/air/`](../../work/x/air/). Units and tags as in
`docs/engine.md` ("Conventions"): speeds in px per update (16.16), times in updates, a positive
`speed_y` is up. No save state is in the air, so all numbers come from the code and the
constants [certain for the code; the feel needs a play check].

## Fields used in the air

| field | meaning |
|---|---|
| `mega+0x89` | blocked sides (`movement.md`), and bit `0x4` ceiling, bit `0x8` floor [certain for the use] |
| `mega+0x67` | air latch: 1 rising (jump, wall kick), −1 in the air, 0 on the ground [certain] |
| `mega+0x4A` | the side tile allows a wall slide and a wall kick (below) [certain] |
| `mega+0x84` | not 0: the air speed is the dash minimum speed (1 after a dash jump, −1 in the air dash) [certain] |
| `mega+0x86` | not 0: no air dash. 1 ground, rope or bar dash; 2 dash jump; 3 air dash; 4 Ultimate air jump; 6 Zero's super jump; 7 Mach Dash [certain for the values] |
| `mega+0x8A` | wall timer: 8 (grab), 10 (kick), 4 (push-off) [certain] |
| `mega+0x8B` | set to 10 by the jump: no wall grab or kick for 10 updates [certain] |
| `mega+0xE3` | 4 after a hang ends: no new hang for 4 updates [certain] |
| `mega+0xE7` | form, from `game+0x5E`: 0 no armour, 1 Falcon, 2 Shadow, 3 Blade, 4 Ultimate, 5 Zero [certain for the values; names likely]; 1 in the Falcon states, 0 in `sheldon-4_no-armor`, 5 in the Zero state |

The armour select lists form n + 1 for armour bit n (bits 0–3); form 0 (no armour) is always in the
list (`docs/players/steps-extra.md`). Form 1 has the jump air dash and form 3 the Mach Dash
(`+0x86` = 7); form 2 walks on spike walls, does not slide and has the super jump [certain for the
code].

## Air physics constants

The values are in `docs/players/movement.md` ("Physics constants"). Their use [certain for the
readers; no other main-program reader was found, X or Zero]:

| field | value | use |
|---|---|---|
| `+0xF4` | 2.0 | air speed (with a direction held); wall kick and push-off speed |
| `+0xF8` | 5.8125 | jump speed (2 × for the super jump) |
| `+0xFC` | −6.5 | `fall_max`: `PlayerMoveAccel` clamps `speed_y` at it |
| `+0x100` | 0.2578125 | fall gravity: set by every "start to fall" (from a ledge, a wall, a hang, the air dash end). Not changed by parts |
| `+0x108` | 4.125 | air speed after a dash jump or a dash wall kick |
| `+0x110` | −1.4375 | wall slide speed (0 in form 2) |
| `+0x114` | 5.625 | wall kick speed (up) |
| `+0x118`, `+0x11C` | 1.0, 1.5 | ladder speeds (part 2c) |
| `+0x120` | 2.0 | no reader found [likely unused] |
| `+0x124` | 2.0 | bar move speed |
| `+0x128` | 1.5 | rope climb speed |
| `+0x12C` | 4.125 | bar dash speed (+1.03125 with part bit `0x10`) |
| `+0x130` | 4.125 | rope dash speed (+1.03125 with part bit `0x10`) |
| `+0x134` | 0.2578125 | jump gravity (also the wall kick); −0.064453125 with part bit `0x8` |

A jump keeps `+0x134` until it lands: the change from step 6 to step 7 does not set the gravity.
So with part bit `0x8` the whole jump (up and down) is lighter, but a fall from a ledge is not.

## Air control (`0x8003D18C`)

All air steps that allow control call it; it also makes the move. With right or left held:
face that way, and `speed_x` = ± 2.0 (± 4.125 when `+0x84` is not 0), or 0 when that side is
blocked. With no direction held: `speed_x` = 0. Then `PlayerMoveAccel`: x += `speed_x`,
y −= `speed_y`, `speed_y` −= gravity, clamp at −6.5 [certain]. So there is no air inertia: the
speed changes at once.

While the player is locked (`+0xD3`), `speed_x` = 0 and only gravity acts.

## Jump (step 6)

Entered by `StartJump` (`0x8003AF1C`) from the ground, the dash, the land step and the hang exits:
`speed_y` = 5.8125, gravity = jump gravity, `speed_x` = 0, `+0x67` = 1, `+0x8B` = 10,
animation 10, a voice 1 in 2 (below), then one air-control move in the same update. When `+0x84`
is not 0 (in a ground dash) **or dash is held**, it is a dash jump: `+0x84` = 1, `+0x86` = 2,
`+0x8C` = 1 [certain].

Each update, in order:

1. `speed_y` = 0 when locked, when the ceiling is blocked, or **when jump is not held**. So the
   rise stops at once when the button is released.
2. X: the air weapon pose (`0x8003E174`, animation 122 for weapons 5 and 14). Zero: three
   overlay tries (`0x801EA560`, `0x801EA454`, `0x801EBF94`).
3. `AnimeTick`; at the frame with flag `0x80` (animation 10, frame 1): the jump sound (bank 1,
   id 1).
4. Ladder from the air (`0x8003A488`: up held on a ladder tile 32 → animation 31, part 2c).
5. Hang (`0x80039984`, below).
6. Wall (`0x8003A2CC`, below), but only when `+0x8B` is 0; else `+0x8B` −= 1.
7. Air dash (`0x8003906C`, below).
8. Air control and move; the water splash check (`0x8003D394`: tile 36 under the feet, once).
9. When `speed_y` ≤ 0: animation 11, `+0x67` = −1, step 7. Else the shooting-pose sync
   (animation 10 or 122).

Numbers [certain for the arithmetic]: a held jump moves up 23 times (5.8125, 5.5547, … 0.1406)
and rises **68.46 px (4.3 blocks) in 23 updates**. With part bit `0x8` (gravity 0.193359375):
31 updates, 90.28 px. A tap (one update held) rises 5.8 px. The horizontal reach to the same
height is about 47 updates: 94 px at 2.0, 194 px for a dash jump at 4.125.

## Fall (step 7)

Entered at the top of a jump (above), and by `ToFall` (`0x8003AFF4`) from all other air steps
and from the ground: animation 11, `speed_x` = `speed_y` = 0, gravity = fall gravity,
`+0x67` = −1, and when `+0x84` is set: `+0x84` = 0, `+0x8C` = −1 [certain].

Walking off a ledge: each update, before the step, `0x80039DFC` calls `ToFall` when the player is
on the ground (`+0x67` = 0) and the floor is not blocked (and `+0xA4` ≤ 0, `+0xD5` ≥ 0, no gate).
There is no grace time [certain]. `ToFall` clears `+0x84` but not `+0x86`, so a walk off a ledge
during a ground dash falls at 2.0, not 4.125, and has no air dash [certain for the code].

Each update: on the floor, the super jump test (up and jump pressed) and else land
(`ToLand`, part 2a). Then the same tries as the jump (weapon pose or Zero's tries `0x801EA560`,
`0x801EA454`, `0x801EC028`; ladder; hang; wall with no delay; air dash), then air control and
move. Animation 11 (123 while shooting).

From rest, the fall reaches −6.5 after 26 updates. It falls 16 px in 12 updates, 32 px in 17,
64 px in 23 [certain for the arithmetic].

## Wall grab, slide and kick (steps 9, 11, 10)

### Which walls

The side-wall tile test (`0x80030C58`, jump table `0x80010F18`) sets `+0x4A` each update
[certain]:

| tile type | solid | `+0x4A` |
|---|---|---|
| 56, 58 | yes | 1: slide and kick |
| 57, 60 | yes | 0: no slide, no kick |
| 59, 61 | no | |
| 62, 63 | yes | form 2: 1. Other forms: 0, and `+0x79` = 1 (the update helper `0x80039228` then kills the player when not invincible): spike walls [likely] |

### The wall test (`0x8003A2CC`)

Only when a side is blocked:

- jump pressed and `+0x4A` > 0: wall kick (`0x8003A33C`);
- else, `+0x4A` not 0 and the direction held points into the blocked side (`0x8003A394`): wall
  grab.

So a jump pressed next to a wall starts a kick from any air step that runs the test, also
without a grab first [certain].

### Wall grab (step 9)

Entry `0x8003B3B8`: animation 13, sound (bank 1, id 4), speeds and gravity 0, `+0x84` =
`+0x86` = 0 (the air dash is allowed again), `+0x8C` = −1, face the wall, `+0x8A` = 8.

Each update: floor → land; locked or no wall → fall; the air weapon pose; the wall kick test. At
the frame with flag `0x80` (animation 13 frame 1, after 3 updates) the player turns to face away
from the wall. After **8 updates** without a move: the wall slide. The direction does not need to
be held during these 8 updates [certain].

### Wall slide (step 11)

Entry `0x8003B548`: animation 15, `speed_y` = −1.4375, gravity 0, `speed_x` = 0, `+0x67` = −1,
dash flags cleared, and (not in form 2) a dust effect (effect type 33, var 0, `0x8003D2BC`).

- Sub-step 0, each update: floor → land; locked or no wall → fall; weapon pose (X) or Zero's tries
  (`0x801EA560`, `0x801EC178`); the wall kick test. With the direction still held into the wall:
  `PlayerMove` with no gravity, so **a constant slide at 1.4375 px per update** (0 in form 2: the
  player stays on the wall). Animation 15 (127 while shooting).
- When the direction is released (`0x8003B5B8`): animation 11, `speed_x` = 2.0 away from the
  wall, `speed_y` = 0, fall gravity, sub-step 1. Sub-step 1 moves with gravity for **4 updates**
  (8 px out), then `speed_x` = 0 and step 7. There is no control in these 4 updates.

### Wall kick (step 10)

Entry `0x8003B440`: animation 14, sound (bank 1, id 3), voice 2, `+0x67` = 1, a spark (effect 33,
var 3, at the offset of `0x80073D6C`). `speed_y` = 5.625, gravity = jump gravity, `speed_x` =
2.0 away from the wall; the player faces the wall. With dash held it is a dash wall kick:
`speed_x` = 4.125, `+0x84` = `+0x86` = `+0x8C` = 1 (so no air dash after it). Else the dash
flags are cleared. `+0x8A` = 10 [certain].

- Sub-step 0 (`0x80036C68`): **10 updates of a fixed move**: no control, no release cut. It ends
  to fall on a ceiling or a wall in the move direction. After 10 updates: `speed_x` = 0,
  sub-step 1. In these updates the player rises 44.65 px and moves 20 px (41.25 px for the dash
  kick) [certain for the arithmetic].
- Sub-step 1 (`0x80036D4C`): as the jump (release cut, ceiling, the tries, air control) but with
  no wall delay, so the next wall test runs at once. With jump held to the top: 22 updates and
  64.20 px in total (84.64 px with part bit `0x8`).

## Air dash (step 13)

### The test (`0x8003906C`)

No air dash when locked or when `+0x86` is not 0: **one air dash per time in the air, and none
after a dash jump, a dash wall kick or a super jump of Zero**. Landing, the wall grab, the hangs
and the wall kick without dash clear `+0x86` [certain].

The test then depends on the character:

| who | jump pressed in the air | dash pressed in the air |
|---|---|---|
| X, form 1 (Falcon) | Falcon dash (step 35) | Falcon dash (step 35) |
| X, form 3 (Blade) | Mach Dash (step 74, overlay) | Mach Dash |
| X, form 4 (Ultimate) | step 33 (overlay; animation 21, speeds 0, `+0xD9` = 180: a hover of up to 180 updates [certain for the code]) | air dash (step 13) |
| X, forms 0 and 2 | nothing | nothing |
| Zero | (overlay try `0x801EC298` first) | air dash (step 13) |

For X the air dash of step 13 also needs `+0x92` = 0 [certain for the code]. The dash input is
the same as on the ground (`mega+0x80` bit `0x100` or `+0x87`); the player faces the held
direction and the dash fails when that side is blocked. `speed_x` = ± 6.5.

### Step 13

Entry `0x8003B298`: animation 18 (no shooting version), `speed_y` = gravity = 0, `+0x84` = −1,
`accel_x` = −0.1875, timer `+0x85` = **18** (10 with part bit `0x10`), `+0x86` = 3, the shot
pose fields cleared (`0x8003CDBC`).

Each update, before the sub-step: floor → land; locked → fall; hang; the ground ladder test
(`0x8003A3DC`); X: the air weapon pose, and **when X is in the shooting pose (`+0x8E`), the air
dash ends** (`0x8003B360`: animation 132, fall). Then `AnimeTick` and the sub-step from
`air_dash_steps` (`0x80073C10`):

1. Start (`0x80037220`): no move, no gravity: the player hangs in the air. Frame flag `0x40`: a
   jet effect (effect 33, var 1, `0x8003D26C`). Frame flag `0x80` (animation 18, frame 2, after
   about 4 updates [likely ± 1]): sound (bank 1, id 5), `+0x8C` = 1, sub-step 1.
2. Dash (`0x800372DC`): `DashStep` (`0x80039FFC`, part 2a) moves the player at 6.5 − 0.1875 per
   update down to 4.125, with no gravity, so in a straight line. It ends on a wall, at timer 0,
   when dash and the directions are released, or when the opposite direction is held. A full air
   dash: **18 moves, 90.5 px** (part `0x10`: 10 moves of 8.125 down, 74.92 px) [certain for the
   arithmetic].
3. End (`0x8003B30C` then `0x8003737C`): animation 19, `speed_x` = 0, fall gravity, `+0x84` = 0,
   `+0x8C` = −1. The player falls straight down with no control until a direction is held (or
   animation 19 ends; it loops, so [likely] only the direction): animation 20 and step 7. The
   air dash cannot reach a wall grab: step 13 has no wall test.

Zero: in each sub-step, Zero's try `0x801EC028` can stop the dash (`speed` 0, fall gravity).

### X form 1: the Falcon dash (step 35)

`0x8003F28C`: with jump or dash pressed in the air: the air dash entry above (animation 18 values,
timer 18 or 10), then `speed_y` = 0, sound (bank 1, id 5), animation 169, step 35, and two objects
(`0x8003D3F8` type 23, `0x8003D510` type 18) [certain]. Step 35 (overlay 0, `0x801EB8B8`) waits
for animation 169 (about 8 updates, no move [likely]), then moves with `0x8003A0E8`: as `DashStep`, but it
keeps `speed_x` at **4.125 or more in the facing direction** and **releasing the buttons does not
end it**; only a wall, the timer or the opposite direction does. Then fall [certain for the code].
With the jump button, `speed_x` is not set to 6.5 first, so the Falcon jump dash runs at 4.125
[likely; animations 171–173 by the sign of `speed_y` suggest other cases, open].

## Hang: rope and bar (steps 27, 28)

The map called these "wall slide / wall kick". They are hangs on overhead tiles [certain for the
code; the names from the animations, likely]. The hang test (`0x80039984`) runs in the jump, fall,
wall kick, air dash and Falcon steps, with **up held**:

- Tile 46 (rope): x snaps to the tile centre ((x & ~15) + 8), `speed_x` = 0, step 27, animation
  160 (144 in X's shooting pose), sound (bank 0, id 38).
- Tiles 37–45 (bars; 37–44 slanted by tile, 45 flat), only when not rising (`speed_y` ≤ 0): y
  snaps to the bar (`0x800397F8`), `speed_y` = 0, step 28, animation 163 + n (148 + n while
  shooting), n = 0, 1, 2 by the tile and the facing. Sound (bank 0, id 38).

Both clear the dash flags (so `+0x86` = 0). In the hang:

| | rope (27) | bar (28) |
|---|---|---|
| move | up or down held: ± 1.5 px per update | left or right held: ± 2.0 px per update |
| stop | at the end of the rope tiles; on the floor when going down | at the end of the bar tiles (37–47) |
| dash | dash pressed: ± 4.125 (up held: up, else down) for up to 30 updates (15 with part `0x10`), while the dash direction is held | dash pressed: ± 4.125 in the facing direction, 30 (15) updates |
| sound | bank 0, id 39 every 21 updates while moving | the same |

Each move is first tried and undone, and only made when the new position is still on a rope or bar
tile [certain]. Exits (`0x8003A764`): jump pressed → a full jump (`StartJump`), or with down held
→ fall; off the tiles → fall. After each exit `+0xE3` = 4: no new hang for 4 updates. X's special
weapons on the hang: `0x8003E06C` (animations 145 rope, 151 bar; weapon 50 → fall).

## Super jump (step 31)

Entered by `TrySuperJump` (`0x8003A870`, part 2a) on the ground, and on the landing update of a
fall: up held and jump pressed, X in form 2 or Zero with `+0xC9` bit 1. `speed_y` = 11.625,
animation 10, sound (bank 1, id 1); Zero also `+0x86` = 6. Sub-steps `0x80073C58`:

0. Rise (`0x8003649C`): `PlayerMove` with no gravity and no control, 11.625 px per update. After
   the move where the height reaches 144 px or more (13 moves, **151.1 px**): animation 11 and
   sub-step 1. When the ceiling is blocked: on tile 57 above, fall; else sound (bank 1, id 4),
   animation 83, sub-step 2, and a ceiling time of **180 updates** (Zero 120).
1. Top (`0x80038248`): **5 updates** still in the air (the animation runs at half speed), then
   fall with the fall gravity.
2. Grab (`0x800382C8`): animation 83 to its end, then animation 84, sub-step 3.
3. Ceiling hold (`0x800365D4`): the player holds on to the ceiling. Dash: an air dash along the
   ceiling (sub-steps 4 and 5 as step 13, animation 85, then step 13 sub-step 2). Down held, jump
   pressed or no ceiling: the time ends at once. At the end of the time: stand when on the floor,
   else fall (`0x8003B114`).

## Animations used here

All exist in the X export (`~/mmx-data/x6/sprites/players/x/`), except 85 [certain]. Animations 83,
84, 169 and 189 use frame set 1 and show other sprites in the export (the frames of the loaded
armour) [likely].

| animation | what | frames (ticks) |
|---|---|---|
| 10 | jump start (122 while shooting) | 2, 4 (flag `0x80`: sound), 6, hold |
| 11 | fall (123 while shooting) | 4, 4, 4, hold |
| 13 | wall grab (125) | 3, 3 (flag `0x80`: turn away), hold |
| 14 | wall kick (126) | 3, 3, 3, hold |
| 15 | wall slide (127) | hold |
| 18 | air dash start and dash | 3, 1, 1 (flag `0x80`), 1 (flag `0x40`), 1 × 5, loop from frame 6 |
| 19 | air dash end | 3, 4, hold (loop to 1) |
| 20 | from the air dash end to the fall | 5, 3, 4, 4, hold |
| 21 | Ultimate air jump | 4, 4, 1 (flag `0x80`), hold |
| 31 | ladder from the air (part 2c) | |
| 83, 84, 85 | super jump: ceiling grab, ceiling hold, ceiling dash | 83: 4, 4, 4, 3, hold |
| 132 | air dash ended by a shot | 5, 3, 4, 4, hold |
| 140 | ceiling hold with a weapon | |
| 144, 160 | rope hang (shooting, normal) | 2, 2, hold |
| 145, 151 | rope, bar with a special weapon | 16, hold |
| 148–150, 163–165 | bar hang by shape (shooting, normal) | 2, 2, hold |
| 154 | bar move while shooting | 1 × 4, loop |
| 169 | Falcon dash start | 2 × 4, hold |
| 189 | Mach Dash start | 6, 3, hold |

## Sounds

| bank, id | when |
|---|---|
| 1, 1 | jump (animation 10 frame 1), super jump |
| 1, 3 | wall kick |
| 1, 4 | wall grab; super jump hits the ceiling |
| 1, 5 | air dash (at the flag `0x80` frame), Falcon dash, rope and bar dash (stopped at its end) |
| 0, 38 | hang start (not in X's shooting pose) |
| 0, 39 | rope or bar move, every 21 updates |
| 5, 5 | water splash |
| 3, n | voices, only when `+0x8D` ≤ 0. Jump, 1 in 2: X 2, 3, 6, 6; Zero 2, 4, 7, 7 (table `0x80073CE4`, random of 4). Wall kick: 2 |

## Zero's differences in the shared code

Where the shared code tests `var` [certain for the tests; the overlay functions are not read]:

- Zero has no air weapon pose; the steps call Zero's overlay tries instead (listed in each step).
- The air dash is always available to Zero (no form test), after Zero's try `0x801EC298`.
  Zero's try `0x801EC028` can stop an air dash in each sub-step.
- No shooting-pose end of the air dash and no shooting animations (n + 112 is X only).
- Super jump: needs skill bit 1, sets `+0x86` = 6, ceiling time 120 updates.
- Walking off a ledge clears the shot pose fields first (`0x8003CDBC`).
- On the rope with no move, X shows animation 160 at frame 2 and Zero its animation 160 at frame 3.

## Summary for a reimplementation

1. Jump: 5.8125 up, gravity 0.2578125, max fall 6.5; releasing jump sets the up speed to 0 at
   once. Full jump 68 px in 23 updates. Air speed 2.0 (4.125 after a dash jump), with no inertia.
2. Dash jump: jump in a ground dash or with dash held. It keeps 4.125 until landing and blocks the
   air dash.
3. Wall: a wall of tile 56 or 58 next to the player. Direction into it: 8 updates still, then a
   slide at 1.4375. Release: 4 updates of push-off at 2.0. Jump: a kick of 5.625 up and 2.0 (4.125
   with dash held) away, fixed for 10 updates, then a normal jump arc. No wall grab in the first
   10 updates of a jump.
4. Air dash (X with Ultimate, Zero): once per air time; about 4 updates still, then up to 18
   updates from 6.5 down to 4.125 with no gravity (90.5 px), then a straight drop until a
   direction is held. A shot ends X's air dash.
5. Falcon (X's form in all states): jump or dash in the air gives an 18-update dash at 4.125 or
   more that only a wall, the timer or the opposite direction stops.
6. Up held near a rope (tile 46) or a bar (37–45): hang. Rope 1.5, bar 2.0, hang dash 4.125.
   Jump to leave, down + jump to drop.
7. Super jump (Shadow, Zero skill): 13 updates at 11.625 up (151 px), 5 updates at the top; at a
   ceiling, hold it for 180 updates.

## Open questions

- What sets `+0x87` (dash without the button), `+0x92` (blocks X's air dash) and `+0xA4` (blocks
  the walk-off fall).
- The overlay steps: 33 (Ultimate), 35 after its start, 74 (Mach Dash); Zero's overlay tries.
- `0x80038594` (bar dash step), `0x8003F700` (X on the ceiling), tile 47, tile 57 above the super
  jump.
- The effect objects (type 33 var 0, 1, 3; type 65) and the objects of the Falcon and Blade starts.
- Animation 85 is not in the export; the frame set 1 sprites of 83, 84, 169, 189.
- Play checks: the jump height and the tap jump, the 8-update wall grab, the 4-update push-off,
  the air dash hover and its end drop, the shot that ends the air dash, the Falcon dash speed with
  the jump button, the 10-update wall delay after a jump.
