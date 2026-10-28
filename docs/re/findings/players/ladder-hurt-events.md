# Player ladder, hurt, beam-in, beam-out and events (X and Zero, main program): behaviour sheet

Part 2c of the player RE plan: the ladder (steps 14–16), the hit reaction (step 17) and the death,
the beam-in (step 0) and the beam-out (step 1), the exit requests, the ride step (18), the event
steps (20–25, 29, 30), and Zero's stop pose (step 5) and idle. The code is in the main program and
shared by X and Zero; the character tests are marked. Ground movement is in
`docs/players/movement.md` (part 2a); the air steps (6, 7, 9–11, 13, 27, 28, 31) are part 2b. The
boss gate walk (step 19) is in `docs/enemies/boss-gate.md`.

Source: the X kit (`python3 -m mmxdemake.rekit --player --state intro-1_start.state.gz`) and the
Zero kit (`turtloid-6_zero-player.state.gz`), a disassembly of the main program for the code outside
the kit, and the exports `players/x` and `players/zero`. Notes, pseudo code and reconstructed C are
in [`~/mmx-data/x6/re/x/other/`](../../work/x/other/). Units, tags, inputs and `PlayerMove` / `PlayerMoveAccel` as in
`docs/players/movement.md`: speeds are px per update (16.16), a positive `speed_y` is up, and
`accel_x` acts in the facing direction (`speed_x += accel_x` facing right, `−=` facing left).

## Fields used here

| field | meaning |
|---|---|
| `+0x04` | player state: 0 set-up, 1 `PlayerUpdate`, 2 death, 3 nothing (table `0x80073ABC`) [certain] |
| `+0x5C` | HP. Bit 7 set and HP > 0 = a hit is waiting for its reaction; `0x80` (−128) = dead [certain] |
| `+0x61` | invulnerability timer, updates (`iframes`) [certain] |
| `+0x63` | hit class 0–5 (below), from the attacker's `+0x62` [certain] |
| `+0x67` | latch: 1 or −1 in the air steps here; 0 on the ground [certain for the values] |
| `+0x79` | spike tile touched (tiles 62, 63) [certain for the code] |
| `+0x89` | blocked sides: `0x1` right, `0x2` left, `0x4` up [likely], `0x8` floor [certain] |
| `+0x92` | after-shot timer: no ladder grab from the ground while it is not 0 [certain for the test] |
| `+0xA4` | hit lock: 1 in step 17, −1 during the blink, 0 off. `ContactMega` does nothing while it is not 0 [certain] |
| `+0xA5` | facing for the hit reaction (towards the attacker) [certain] |
| `+0xCA` | grab state of hit class 3 [certain for the values; the name is likely] |
| `+0xCE` | updates the beam-in passes through terrain, from the spawn record (`0x80029D14`) [likely] |
| `+0xD0`, `+0xD1`, `+0xD2` | event request (1), event step, event facing (`docs/engine.md`, "Events"). `+0xD0` = −1 after the set-up [certain] |
| `+0xD5` | ride: > 0 request, < 0 riding, 0 free [certain for the code] |
| `+0xD6`, `+0xD7` | death wave count and timer [certain] |
| `+0xD8` | ride animation, written by the ride object [certain for the use] |
| `+0xE2` | Light capsule phase; step 24 waits for 4 [certain] |
| `+0xE5` | squeeze timer: 75 after a squeeze hit, −1 per update. Read by stage objects only [certain for the writes] |
| `+0xE6` | squeeze damage: the contact damage of the solid object that pushes (`SolidPush`), or 127 [certain] |
| `+0xE7` | form: `game+0x5E`: 0 no armour, 1–4 the armours (Falcon, Shadow, Blade, Ultimate), 5 Zero [certain for the values; names likely] |

## Ladder (steps 14, 15, 16)

Tile values (the tile at a point, `docs/TODO.md` collision legend) [certain for the code]:

- **32** ladder. **33** ladder top: the tile above the ladder, solid from above (the tile switch at
  `0x80010E98` gives 33 the "solid" result).
- `TileAt` (`0x8003015C`) reads the tile at the origin (x, y). `TileAbove` (`0x8002FD3C`) and
  `TileBelow` (`0x80030068`) read it at the top and bottom edge of the terrain box (X's box:
  `0x80073D50` = centre (0, 2), half size 11 × 19, so y − 17 and y + 21).

### Grab

| entry | when | what |
|---|---|---|
| `LadderFromGround` `0x8003B63C` | ground steps (`TryLadder`, part 2a): up held, `+0x92` = 0, `TileAbove` = 32 | animation 27, x to the tile centre (`x & ~15` + 8), `speed_y` = `+0x118` (1.0 up), latch 1, shot flags cleared, step 14 sub-step 0 |
| `LadderDown` `0x8003B708` | ground steps: down held, `+0x92` = 0, `TileBelow` = 33 | animation 29, x to the tile centre, no terrain box, latch 1, step 14 sub-step 2 |
| `TryLadderAir` `0x8003A488` | jump (6), fall (7), and `0x80036D4C` (step 10): up held and `TileAt` = 32 | X in the shooting pose: X's ladder shot (step 32, below). Else animation 31 and `ToLadderUp` |

`ToLadderUp` (`0x8003B7B4`): x to the tile centre, `speed_x`, `accel_x`, `gravity` = 0, `speed_y` =
`+0x11C` (1.5 up), dash flags cleared, step 15. `ToLadderDown` (`0x8003B810`): the same with
`speed_y` = −1.5, step 16 [certain].

### Step 14: the ladder moves (sub-steps `ladder_steps`, `0x80073C1C`)

Each update: `AnimeTick`, then the sub-step [certain]:

| sub | function | from | what |
|---|---|---|---|
| 0 | `0x80037488` | `LadderFromGround` | `PlayerMove` (1.0 px up per update). At the last frame of animation 27 (after 9 updates, 9 px): animation 31, `ToLadderUp` |
| 1 | `0x800374D8` | climb onto the top | at the frame with flag `0x80`: y − 16 px, `old_y` = the new y − 16, terrain box on. At the last frame: latch 0, `Stop` (X stands; Zero's stop pose). Animation 28: 20 updates, the y move after 10 |
| 2 | `0x80037548` | `LadderDown` | at the frame with flag `0x80` (after 5 updates): y = `y & ~15` + 32. At the last frame (after 15 updates): y + 16, terrain box on, animation 32 from frame 3, `ToLadderDown` |
| 3 | `0x800375D0` | floor touched while climbing | animation 30; at its last frame (5 updates): latch 0, `Stop` |
| 4 | `0x800377FC` | let go | animation 30; at its last frame (5 updates): fall (`0x8003AFF4`, part 2b). X does not move during the 5 updates |

The `old_y` write in sub-step 1 makes the floor test see a move down onto the ladder top, so the
player stands on tile 33 [likely].

### Steps 15 (up) and 16 (down)

Each update, in order [certain]:

1. `LadderExit` (`0x8003A514`), with `t` = `TileAt`:
   - floor touched (`+0x89` bit `0x8`): sub-step 3 (get off at the bottom);
   - up held and `t` = 33 (the origin has reached the ladder top): `LadderClimbTop` (`0x8003B6B4`):
     animation 28, no terrain box, y to the tile top (`y & ~15`), sub-step 1;
   - `t` is not 32 or 33 (the ladder has ended), or jump pressed: animation 30, sub-step 4 (let go).
     Jump does not jump: the player falls.
2. X: `0x8003E260` (shoot on the ladder: animation 92 for weapons 5 and 14, the shot code, and
   weapon 50 falls). Zero: `0x801EA560` (a skill: down held and button `0x20` pressed, skill bit
   `0x20`; Zero overlay step 58), then `0x801EC0F0` (the sabre: animation 93, sound (1, 7), Zero
   overlay step 51).
3. Left or right pressed sets the facing (`flip`). There is no move left or right.
4. Step 15: up held: `AnimeTick` and `PlayerMove` (1.5 px up). Down held: animation 32 at the
   frame number in the flag of the current frame, the frame time kept, `ToLadderDown`.
   Step 16: up held: animation 31 the same way, `ToLadderUp`. Down held: `AnimeTick`, `PlayerMove`
   (1.5 px down). Neither held: no move, the animation stops.

Animations 31 and 32 are the same 8 poses in opposite order, and the frame flags hold the frame
number of the same pose in the other animation (31: flags 7…0; 32: flags 7…0). So a change of
direction keeps the pose [certain for the data]. One cycle of animation 31 is 34 updates (51 px).

So the rules [certain for the code]:

- Climb speed 1.5 px per update, up and down; 1.0 px per update for the first 9 updates from the
  ground.
- Top: when the origin reaches tile 33 with up held, the player climbs onto the top (20 updates) and
  stands.
- Bottom: a floor under the player ends the climb (5 updates), then stand.
- The ladder ends (the origin leaves tiles 32 and 33) or jump is pressed: a 5-update hang, then fall.
- Grab in the air: only with the origin on tile 32 (not 33).

### X's ladder shot (step 32)

`player_steps[32]` is X's overlay function `0x801EB730` (the table goes on past the 32 shared
steps). `0x801EB6E8` starts it: animation from the table `0x801F36C8` by the shot type (`+0x97`;
animation 93 has its first frame cut by the shot timer), x to the tile centre, step 32. Each update:
`LadderShotExit` (`0x8003A5D8`: `TileAt` not 32 or jump pressed: fall), the shot code again,
`AnimeTick`, the facing from the direction pressed. When the shot pose ends (`+0x90`): animation 32
from frame 3, `ToLadderDown` (the player hangs still until a direction is held) [certain for the
code].

## Hit reaction (step 17) and death

There is no "hurt" request from the objects. A hit only changes the HP, and the player code finds
it at its next update [certain].

### The hit (`ContactMega`, `0x80030ECC`)

It does nothing when `0x800C4568` is not 0, when `no_hit` (`+0x7A`) is 1, when HP bit 7 is set (a
hit waits, or dead), or when `+0xA4` is not 0. `no_hit` = 2 lets only the objects in
`0x80093C98`…`+0x1400` (visual objects) through and is then cleared. Then `BoxOverlap(object,
mega)`, which fails while `+0x61` is not 0 [certain]. On a hit, with `d` = the object's contact
damage (`+0x60`, ≥ 0):

1. Hit class `c` = the object's `+0x62`, and `+0xCA` = 0. When `c` = 0, the code picks one: class 4
   for forms 1–4 (X in every armour, not form 0), for form 5 with part bit `0x200`, and for Zero with
   `game.armors` (`game+0x5F`) bit `0x20`; else class 1 when `d` < 5, class 2 when `d` ≥ 5. When
   `c` = 3, `+0xCA` = 1 [certain].
2. HP −= `PlayerDamageScale(d)` (`docs/engine.md`); `game+0x98` += the damage (127 counts the
   rest of the HP). HP > 0: HP bit 7 is set. Else HP = `0x80` and `game.player_lock` = 1 [certain].
3. `+0xA5` (the facing for the reaction): class 3: not set. Class 0 with part bit `0x200` or Zero
   with `game.armors` bit `0x20`: the current facing. Else `0x40` (right) when the player's x is
   less than the object's x, else 0: the player faces the attacker [certain].

While the player rides (`+0xD5` < 0), the hit goes to `actor2` (`0x800CD340`) instead: no class
choice (1 when `d` < 5, else 2), no damage scaling [certain for the code].

### The check each update (`PlayerHitCheck`, `0x80039228`)

`PlayerUpdate` calls it before the step, each update that the player is not stopped. It does
nothing in steps 0 and 1 (the beams). In order [certain]:

1. **Crush:** terrain on both sides (`collide_flags` `0xC` both set, or `0x3` both set): HP =
   `0x80` at once, whatever the invulnerability.
2. **Squeeze:** else, both sides blocked with an object push in it (`+0x89` = `collide_flags` |
   `+0x71`): `no_hit` = 0 (X: also `+0xE0` = 0 and `game.player_lock` = 0 when `+0xE0` was 1),
   `+0xE5` = 75, HP −= `+0xE6` (no scaling), `+0x72` and `+0x76` of the 80 stage objects cleared.
   HP > 0: HP bit 7, class 1 (4 for forms 1–4), `+0xA5` = the current facing. Else HP = `0x80`. The
   stage objects that read `+0xE5` probably push again only after it reaches 0 [likely].
3. **Spikes:** `+0x79` set, `+0x61` = 0 and `no_hit` ≠ 1: HP = `0x80`. The tile switch sets `+0x79`
   for tiles 62 and 63, except in form 2 [certain for the code; form 2 is likely the Shadow Armor,
   guess].
4. **Death:** HP = `0x80`: HP = 0, `game.player_lock` = 1, the dash and shot flags cleared, the
   charge stopped (`0x8003F3DC`), no terrain box and no hurt box, state 2 step 0. `PlayerUpdate`
   then returns at once because HP is 0.
5. **Invulnerability:** `+0x61` not 0: −1 per update. Below 60, with part bit `0x400`, only on odd
   `frame_count` values (half speed). At 0: `+0xA4` = 0 and the palette set again
   (`0x8003C4EC`).
6. **Reaction:** HP < 0 as a signed byte (bit 7 set, a hit waits): `PlayerHurt` (`0x8003963C`), then
   the energy refill (below).

### `PlayerHurt` (`0x8003963C`) and step 17

Always: terrain box on, HP bit 7 cleared, `Rumble(1)`, sounds (3, 0 or 1 at random) and (3, 17).
When the step is already 17, nothing more. Else [certain]: `+0xA4` = 1, `+0xCF` = 1, the dash and
shot flags cleared, `flip` = `+0xA5`, and from the hit class `c`:

| class | animation (`0x80073C70`) | `speed_x` | `accel_x` | `speed_y` | `gravity` | `+0x61` (`0x80073CD8`) | handler (`0x80073C30`) | latch |
|---|---|---|---|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 0 | 0 | 0 | `HurtLight` | |
| 1 light | 33 | 2.5 | 0.1875 | 0 | 0 | 75 | `HurtLight` `0x80037880` | |
| 2 heavy | 34 | 0.75 | 0 | 2.75 | 0.2578125 | 100 | `HurtHeavy` `0x80035BDC` | 1 |
| 3 grab | 36 | 0 | 0 | 0 | 0 | 0 | `HurtGrab` `0x800379AC` | 1 |
| 4 forms 1–4 | 35 | 2.5 | 0.1875 | 0 | 0 | 75 | `HurtLight` | |
| 5 push | 34 | 14.0 | 0 | 0 | 0 | 63 | `HurtPush` `0x80037920` | 1 |

The speeds are in px per update (`0x80073C78`, 16 bytes per class). `speed_x` is away from the
attacker: it is negated when `flip` = `0x40`, and `accel_x` slows it down. With part bit `0x200`
(Shock Buffer, `docs/engine.md`), or Zero with `game.armors` bit `0x20`, classes 1, 2 and 4 get no
motion (all four values 0).

Step 17 (`0x80037838`) runs `AnimeTick`, then the handler of the class. The reaction ends when
`+0x61` comes down to 60 [certain]:

- **`HurtLight`** (classes 0, 1, 4): `HurtMove` (`0x80037A60`): `speed_x` = 0 at a wall on that
  side, then `PlayerMoveAccel`, and `speed_x` = 0 when it changes sign. X: the palette flash
  `0x8003CF54(82)` while sprite 208 shows (animation 35), else the normal palette. At 60:
  `HurtEnd` (`0x8003B114`): on the floor, the requests, then stand (animation 94 while shooting,
  as `Stop`); in the air, animation 11, `gravity` = `+0x100`, latch −1, fall (step 7).
- **`HurtHeavy`** (class 2): `HurtMove`, the water splash check (`0x8003D394`). Sub-step 0 → 1 after
  one update. On landing (latch and floor): latch 0, `speed_x` = 0.5 backward (0 with the Shock
  Buffer), `speed_y`, `gravity` = 0. At 60: on the floor, `ToStand`; in the air with gravity,
  animation 11, latch −1, step 7; else fall.
- **`HurtPush`** (class 5): `HurtMove2` (`0x80037B10`, also `speed_y` = 0 at a wall), the splash
  check; at 60, fall (`0x8003AFF4`) [certain].
- **`HurtGrab`** (class 3): left or right held sets the facing. `+0xCA` > 0: wait (the grabber
  holds the player). 0: `+0xA4` = 0, `HurtEnd`. −1 (set by `PlayerGrabDamage`, `0x8003D07C`, a hit
  while held): animation 37, `+0xCA` = −2; at its last frame, animation 36 and `+0xCA` = 1 [certain
  for the code]. `+0x61` stays 0, so there is no blink after a grab.

At the end of classes 0–2, 4 and 5: when the player is locked (`+0xD3`), `+0xA4` = 0 and `+0x61` =
0 (no blink). Else `0x8003D014` (HP < max HP / 3: `+0x8D` = 120, the low-HP warning timer [likely])
and `+0xA4` = −1 (the blink) [certain for the code].

So, with the hit update counted [certain for the code; the feel needs a play check]:

| class | reaction | motion | then invulnerable |
|---|---|---|---|
| 1, 4 | 16 updates | 2.5 px back, −0.1875 per update: about 18 px in 14 updates; no gravity, so the player stays in the air | 60 updates (120 with part bit `0x400`) |
| 2 | 41 updates | a hop back: 0.75 px per update, 2.75 up, gravity 0.2578: about 16 px high, about 21 updates in the air on flat ground; then 0.5 px per update back on the floor | 60 (120) |
| 5 | 4 updates | 14 px per update back, about 56 px | 60 (120) |
| 3 | until the grabber lets go | none | none |

Invulnerability: `ContactMega` does nothing from the hit until `+0x61` reaches 0, because `+0xA4`
is set: 75 updates for classes 1 and 4, 100 for class 2 (135 and 160 with part bit `0x400`).

### Energy refill on a hit

After `PlayerHurt` [certain for the code; the names are likely]:

- X: `+0xA8` += max / 8 (`+0x14A`), capped at the max; less than max / 8 left fills it. This is
  probably the Giga attack gauge.
- Zero: `+0xB2` += `0x8003EB0C(m, 5)` / 2, capped at `+0x14A`.
- Part bit `0x800`: the lowest of the 9 energies `+0xA8`…`+0xB8` gets +12, capped at `+0x14A`.

### Other deaths

- **Pit:** the camera code (`0x8002820C`) sets HP = `0x80` when the camera is at its lowest limit
  and the player's y − 8 is 256 px or more below that limit, so about 24 px below the bottom of the
  view [certain for the code; the view height 240 is from `OffScreenCheck2`].
- **Grab damage** (`PlayerGrabDamage(d)`, `0x8003D07C`): `+0xCA` = −1; HP −= the scaled `d`; HP ≤ 0:
  `+0xCA` = 0, lock, HP = `0x80`.

### Death (state 2, steps `0x80073D20`)

| step | function | what |
|---|---|---|
| 0 | `0x8003D8A8` | stop sounds (1, 5) and (1, 7), `Rumble(0)`. Riding (`+0xD5` ≠ 0): hide the player, `+0xD7` = 1. Else stop flags `game+0x12`…`+0x1A` = 1 (all objects freeze), animation 33, `+0xD7` = 8 |
| 1 | `0x8003D954` | `+0xD7` −1 per update; at 0: stop flags 0, hide the player, sounds (3, 16) and (3, 14), step 2 at once |
| 2 | `0x8003D9F8` | every 4 updates (`+0xD7` & 3 = 0): 4 particles (misc type 17, `var` = random 0–31 + `0x80`). Waves: wave 0, then 19 updates later wave 1, then 19 later waves 2 and 3 together (8 orbs each, misc type 17, `var` = the direction from `0x80073D30`: 0, 4, …, 28; 2, 6, …; 1, 5, …; 3, 7, …) |
| 3 | `0x8003DACC` | 20 updates, then state 3 (nothing) |

So: 9 updates frozen in the hit pose, then the orbs for 39 updates, then 21 updates: about 70
updates from the death to state 3 [certain for the code]. The orb object (misc type 17) and what restarts the stage are not read
[open].

## Beam-in (state 0, step 0)

`PlayerInit` (state 0, `0x8003CBE4`) waits until `game+0x1E` is not 0, then sets `+0x03` = 1,
`game+0x1F` = 1, state 1, and runs one of the table `0x80073CF4` [certain]:

- **Normal** (`0x8003D850`): animation 1 (the beam), sound (1, 13), `speed_y` = −8.0 (8 px down
  per update), `speed_x`, `accel_x`, `gravity` = 0, latch −1, step 0.
- **No beam** (`0x8003C1AC`), in stage 12 area 0 with checkpoint (`game+0x1D`) 2–17: y from the
  table `0x80073CFC` by checkpoint (Zero 1 px higher), the form and parts and the physics constants
  at once, `ToStand`, `+0x92` = 4, the floor bit in `+0x71`, the three cameras follow, the lock off.

Step 0 (`0x80035144`), each update `AnimeTick`, then [certain]:

- Sub-step 0 (fall): while `+0xCE` is not 0, it counts down (terrain box on at 0) and
  `PlayerMove`: the beam passes through the terrain. Then, on the floor (`+0x89` bit `0x8`):
  animation 2, latch 0, the three cameras follow the player (`+0x44` of each), sub-step 1. Else
  `PlayerMove` and the splash check.
- Sub-step 1 (land): at the first frame with a flag (X's animation 2, frame 14, 48 updates after the
  landing): `PlayerLoadForm` and `PlayerPhysicsParts`. At the hold frame (87 updates for X):
  `game.player_lock` = 0; Zero: `PlayerLoadForm` and `PlayerPhysicsParts` here (Zero's animation 2
  has no flag); `ToStand`.

`PlayerLoadForm` (`0x8003D718`): `+0xE7` = `game+0x5E`, `+0xEC` = `game+0x74 + 4 × var`. Form 2:
`+0xC9` = 0 and three calls (`0x8003D510(1, 27, 5, 0)`, `0x8003CF24(2, 25, 1)`, `0x8003CF24(2, 24,
1)`; palette or sprite loads [guess]). Else `+0xC9` = `game+0x60` (Zero's skill bits) [certain for
the code].

The beam falls at 8 px per update, with no gravity and no limit other than the floor.

## Beam-out and exits

`PlayerRequests` (`0x8003A628`) runs `PlayerExit` (`0x8003B870`) when `game.exit_req` is not 0.
`PlayerExit`: dash and shot flags cleared, charge stopped, `game.player_lock` = 1, then [certain]:

- `exit_req` bit `0x10` (after a boss soul): the stand animation, `0x8003F000` (X: the charge
  off), `StreamPlay(4, 127)` for X, `(5, 127)` for Zero (the victory music [likely]), step 25.
- bit `0x40` (area change, the teleporter): state 3. The player stops; the teleporter does the rest.
- else: animation 3 (X: the warp start), `0x8003F000`, `BeamOut` (`0x8003B954`).

`BeamOut`: sound (1, 11), `speed_y` = +8.0 (8 px up per update), the other speeds 0, no terrain box,
latch 1, step 1.

Step 1 (`0x8003674C`): sub-step 0: `AnimeTick` until the hold frame (animation 3: 15 updates), then
the camera's y limits (current and target) = the camera y (the camera stops), animation 4 (the
column), sub-step 1. Sub-step 1: `PlayerMove` and the splash check until the player is 32 px left
or right or 64 px above or below the view (`OffScreenCheck2(32, 64)`); then hidden, state 3 [certain].

### Step 25: victory and beam-out

Sub-step 0 waits until `stream_state` = 0 (the music has ended), then animation 39 (the victory
pose), sub-step 1. Sound (0, 33) at the frame with flag `0x80` (X: 23 updates into animation 39).
At the hold frame: animation 40, `BeamOut` (step 1 then waits for animation 40, 15 updates for X)
[certain].

### Step 18: ride

`PlayerRideCheck` (`0x8003A268`, each update in `PlayerUpdate`): when `+0xD5` > 0, the dash and shot
flags cleared, the charge stopped, no terrain box, latch 1, step 18 sub-step 1, `+0xD5` = −`+0xD5`.
Stage overlays 06 and 28 write `+0xD5` (1 or 2) and `+0xD8` (animations 41–64, which show only X's
head) [certain for the writes]. Step 18 (`0x80037BC4`) [certain]:

- `exit_req`: `PlayerExit`.
- `+0xD5` = 0 (the ride lets go): terrain box on, `StartJump` (part 2b), `+0x86` = 1, the lock off.
- Else sub-step 0: animation `+0xD8`, sub-step 1; then `AnimeTick` only. The ride object moves the
  player.

While `+0xD5` < 0 the player has no dash box, and hits go to `actor2`. The ride is the Ride Armor
of Metal Shark Player's stage, `actor2` type 0 (`docs/players/ride-armor.md`) [certain]. The ride
writes `+0xD5` = 1 and `+0xD8` (animations 41–57); it sets `+0xD5` = 0 when X leaves (ride step 11)
and when it is destroyed.

## Events

`PlayerStartEvent` (`0x8003AAD0`): step = `+0xD1`, sub-step 0, the dash and shot flags cleared, then
the set-up from the switch `0x800113C4` (index step − 20, 54 entries; the cases not listed do
nothing). Most set-ups end with `+0xD0` = −1 (request taken) and `flip` = `+0xD2` [certain].

| step | set-up | each update | ends |
|---|---|---|---|
| 20 hold | the stand animation unless it is 5 or 6 | terrain box, the requests, `AnimeTick` | `+0xD0` = 0 (`PlayerEventEnd`): step 2 |
| 21 walk | animation 38; `speed_y`, `accel_x`, `gravity` = 0. `+0xD2` = 0: `EventWalkTo` at once; else face right. `+0xD0` stays 1 | `exit_req`: `PlayerExit`. Speed = walk start speed (0.5) while the frame flag is 0 (the first 8 updates), then walk speed (2.0), towards `flip`; `PlayerMove`, `AnimeTick`; `+0xD2` = 0: `EventWalkTo` | `+0xD2` = 0: at the camera x + 64 (± 2 px): x set there, the stand animation, face right, `+0xD0` = −1, step 20. Else only by an exit |
| 22 warp | animation 3 | the requests; `AnimeTick`; at the hold frame (15 updates): hidden, sub-step 1 | a new request |
| 23 jump | animation 10, terrain box, `speed_y` = jump speed (5.8125), `gravity` = `+0x100` (0.2578), `speed_x` = 0, latch 1 | `AnimeTick`, `PlayerMoveAccel`; at `speed_y` ≤ 0: animation 11, latch −1, `speed_y` = 0 | on the floor: the stand animation, `+0xD0` = −1, step 20 |
| 24 capsule | animation 107 (X) or 5 (Zero), `0x8003F000` | sub-step 0: wait for `+0xE2` = 4, then animation 39. Sub-step 1: sound (0, 33) at flag `0x80` | the hold frame: `PlayerEventEnd`, `ToStand` |
| 29 walk | animation 38, `speed_x` = 1.0 towards `+0xD2` (−1.0 when 0) | the requests, `PlayerMove`, `AnimeTick` | a new request |
| 30 leap | animation 10, latch 1, `speed_x` = 2.0, `speed_y` = 2.90625, `gravity` = 0.0322265625, a random voice (`0x80073CE4`) | `AnimeTick`, `PlayerMoveAccel`. Sub 0: sound (1, 1) at flag `0x80`; `speed_y` ≤ 0: animation 11, latch −1; latch −1 and floor: animation 81, `speed_x` = 2.0, `accel_x` = −0.0625, `speed_y`, `gravity` = 0, sub 1. Sub 1: `speed_x` ≤ 0: animation 82, `speed_x`, `accel_x` = 0, sub 2. Sub 2: at the hold frame, the requests | a new request |
| 73 | animation 6 unless it is 6 | (no step 73 exists in `player_steps`) | |

`EventWalkTo` is `0x8003A678`. Step 30's `speed_x` is not negated for the facing, and the landing
slide uses `accel_x` (in the facing direction): so it works as written only facing right [certain
for the code]. Step 30 rises for about 90 updates (about 131 px) and slides about 33 px on landing
[certain for the numbers].

Users found (calls of `PlayerEvent` with a constant step): 20 and 29 in most stage overlays (the
boss intros; D-1000), 22 in overlay 14, 24 (the Light capsule, `0x80051CA4`) and 20 in the main
program (the health capsule) [certain]. No constant call asks for 21, 23 or 30; some calls pass the
step in a register (overlays 05, 14, 24, 35, 43, 45) [open].

## Zero: step 5 and idle

- **Step 5** (`0x80036800`, Zero's stop pose, animation 9 from `Stop`): the requests, `TryLadder`,
  Zero's tries A, B, C (`0x801EC86C`, `0x801EA560`, `0x801EBE04`), `TryDashJump`, Zero's tries D and
  F (`0x801EBE54`, `0x801EBEDC`), `TryDirection` → walk start; else `AnimeTick`, and `ToStand` at the
  last frame (8 updates) [certain].
- **Idle** (`0x801EC9C4`, overlay 1): in animation 6 (low HP) with HP ≥ max HP / 3: animation 5.
  Else `AnimeTick`. There is no weapon pose and no shot-pose code, unlike X's idle [certain].

Other Zero differences in this part: the beam-in loads the form at the end of the landing; the exit
uses stream 5 and clears `+0x93`; the hit class 4 needs part bit `0x200` or `game.armors` bit
`0x20`; the ladder has the sabre and a skill instead of X's shot.

## Animations used here

All are in both exports [certain]. Timings (updates) from the X export unless marked.

| animation | what | frames (ticks) |
|---|---|---|
| 1, 4 | the beam column (X: one held frame; Zero: 2-frame loop) | |
| 2 | beam lands, the character forms | X: 17 frames, 87 updates to the hold; flag 1 at frame 14 |
| 3, 40 | warp start (X: the same frames) | X: 3, 5, 2, 2, 3, hold: 15 updates |
| 9 | Zero's stop pose | Zero: 4, 4, hold |
| 10, 11 | jump start, fall (part 2b) | 10: frame 1 flag `0x81` |
| 27 | ladder from the ground | 3, 3, 3, hold |
| 28 | climb onto the top | 5, 5, 5 (flag `0x80`), 5, hold |
| 29 | ladder down from the top | 5, 5 (flag `0x80`), 5, hold |
| 30 | ladder end (get off, let go) | 5, hold |
| 31, 32 | climb up, climb down | 8 frames, 34 updates per cycle, flags 7…0 |
| 33 | light hit (class 1; the death pose) | 3, 1, 2, 2, 1, 3, loop from frame 1 |
| 34 | heavy hit (classes 2, 5) | 15 frames, 36 updates, hold |
| 35 | hit in forms 1–4 (class 4) | 3, 2, 2, 2, 2, loop from frame 2; sprite 208 at frame 2 |
| 36, 37 | held (class 3); hit while held | 36: one frame; 37: 8 frames, 15 updates, hold |
| 38 | event walk | 16 frames, loop from frame 2; flag 1 from frame 3 |
| 39 | victory pose | 2, 3, 16, 2, 48 (flag `0x80`), hold |
| 41–64 | ride poses (head only) | |
| 81, 82 | crouch down, stand up (step 30) | |
| 92, 93 | X: shoot on the ladder; Zero 93: sabre on the ladder | |
| 107 | X: stand still for the capsule | one frame |

## Sounds

| bank, id | when |
|---|---|
| 1, 13 | beam-in start |
| 1, 11 | beam-out start |
| 3, 0 or 3, 1 (random) and 3, 17 | hit |
| 3, 16 and 3, 14 | death, when the orbs start |
| 0, 33 | victory pose and capsule pose (flag `0x80` of animation 39) |
| 1, 1 | step 30 jump (flag `0x80` of animation 10) |
| 1, 7 | Zero's ladder sabre |
| stream 4 (X), 5 (Zero) | after a boss soul (step 25) |

## Summary for a reimplementation

1. Ladder: tile 32 = ladder, 33 = the top (solid from above). Up on 32 at the head or down on 33
   under the feet grabs it; x snaps to the tile centre. Climb 1.5 px per update only while up or
   down is held; left and right only turn. Up at the top tile: a 20-update climb onto the top. Floor
   below: get off. Ladder ends or jump: hang 5 updates, then fall.
2. A hit sets HP bit 7; the player reacts at its next update. X in forms 1–4 (every armour)
   always gets class 4: 2.5 px back, slowing by 0.1875, 16 updates, no gravity. Else
   (Zero, or X in another form), damage < 5 is light (the same motion) and ≥ 5 is heavy (a
   backward hop, 41 updates).
3. Invulnerable from the hit for 75 updates (100 after a heavy hit); the last 60 blink (double with
   part bit `0x400`). The Shock Buffer removes the knock-back.
4. Terrain on both sides, spikes (tiles 62, 63, not in form 2), and a fall 24 px below the view
   kill at once. HP 0: everything freezes 8 updates, then the player vanishes into orbs.
5. Beam-in: 8 px per update down, passes through terrain for `+0xCE` updates, then lands and forms
   (87 updates for X). Beam-out: warp pose 15 updates, then 8 px per update up until off screen.
6. After a boss: wait for the music, victory pose, beam out.
7. Events: hold (20), walk to 64 px from the left of the view (21), warp out in place (22), jump up
   (23), capsule pose (24), walk at 1 px per update (29), long leap with a landing slide (30).

## Corrections to `docs/players/map.md`

- Step 0 is the beam-in (fall and land, animations 1 and 2), not only the landing. Step 1 is the
  beam-out (rise, animation 4), entered by `BeamOut`; it does not go to step 0.
- Step 14 is the ladder moves (sub-steps), 15 climb up, 16 climb down.
- Step 17 is the hit reaction (hurt), entered by `PlayerHurt` `0x8003963C`.
- Step 18 is the ride (entered by `PlayerRideCheck` `0x8003A268`), not a warp out.
- Step 21 is the event walk (animation 38); step 22 the event warp-out in place (animation 3);
  step 23 the event jump; step 24 the Light capsule pose; step 29 the event walk at 1 px; step 30
  the event leap.
- Step 32 exists: X's ladder shot (overlay `0x801EB730`).

## Open questions

- The ride object of overlays 06 and 28, and what it does with `actor2`.
- Who asks for event steps 21, 23 and 30 (the register calls in overlays 05, 14, 24, 35, 43, 45).
- (answered: forms 0 no armour, 1–4 the armours, 5 Zero; `docs/players/steps-extra.md`) The part
  names of bits `0x200` (Shock Buffer, from `docs/engine.md`), `0x400` (double blink
  time) and `0x800` (refill on a hit).
- Which objects use hit classes 3 (grab) and 5 (push), and the users of `PlayerGrabDamage`.
- The blink drawing for `+0xA4` = −1 and `+0x8D`; what `+0xCF` does; `0x800C4568` (it turns off
  all contact hits).
- The death orb object (misc type 17) and what restarts the stage after state 3.
- The stage objects that read `+0xE5`.
- What `game+0x98` is used for (the total damage taken).
- Play checks: the ladder speed and the top climb; the knock-back distances; the beam-in time; the
  invulnerability time.
