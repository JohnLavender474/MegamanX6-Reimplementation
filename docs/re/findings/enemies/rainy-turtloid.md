# Rainy Turtloid (stage 09, main object type 57): behaviour sheet

Rainy Turtloid is the Maverick of the acid-rain stage. He is a large turtle (a 128 × 128 px body)
that stays on the floor, except for some jumps. His shell turns toward the player and deflects all
frontal shots. Two **green crystals** sit in the two holes of his shell. They fire his attacks: a
volley of homing missiles, and, while he rolls in his shell, water bubbles or (Hard only) falling
water balls. He spins in his shell and rolls from wall to wall. When both crystals are destroyed,
his whole body takes damage for some time. Three weapon ids stun him. The attack chain depends on
his HP (above or below half) and on Hard.

Stage overlay `09` (`rock_bin/09.bin`, loaded at `0x800E9860`; no boss overlay), save state
`turtloid-7_boss.state.gz` (stage 6, area 0, checkpoint 5; Zero is the player, the boss is in his
first missile volley). Written with the RE kit (`python3 -m mmxdemake.rekit --name rainy-turtloid
--state turtloid-7_boss.state.gz --type 57`) and `docs/re-guide.md`. Units, confidence tags and the
shared engine rules: `docs/engine.md`. The room: `docs/stages/turtloid.md`, "The boss room". The
crystals, shots and effects are visual objects of type 23 (`var` 0–11). "The room" below has its
origin at the camera's target minimum x and y at the set-up (`camera_layer0` `+0x26`, `+0x2A`;
x 8944, y 640 in the save state). In room units the inner walls are at x 16 and x 400, the
ceiling at y 16, the floor at y 240, and the middle at x 208.

## Spawn and set-up (`0x800F06B4`) [certain]

- **HP: 56 + a level bonus.** `boss_hp_max` (`game+0x8C`) = 56 + {0, 0, 4, 8}[boss level] (table
  `0x800E98C0`; level from `BossLevel`, `game+0x8F`): **56, 56, 60, 64**. The save state has level 0
  (56 HP). The HP starts at 0 and fills in the intro.
- **Contact damage 6.**
- Boxes (body). "Back" means the side away from his facing; his sprite and his boxes face the
  player with the shell (see "Where he can be hit", below):

  | box | value | size | use |
  |---|---|---|---|
  | terrain | (0, 0, 64, 64) | 128 × 128 px | his origin is 64 px above the floor; set in the intro |
  | attack, normal | (−40, −40, 104, 104) | 104 × 104 px | most of the time |
  | attack, spin | (−60, −60, 120, 120) | 120 × 120 px | while animation 30 (spinning shell) runs |
  | hurt, shell | (16, −48, 32, 96) | 32 × 96 px | tested first, with `dmg_tbl_deflect` |
  | hurt, back | (48, −48, 24, 96) | 24 × 96 px | tested when the shell box had no hit, with his own table |
  | hurt, whole | (−48, −48, 96, 96) | 96 × 96 px | shell closed, stunned, or both crystals lost |

- Damage table `0x800755FC` (named `dmg_tbl_turtloid` here; below). Draw priority 5. Animation
  table `0x800F689C`. Own sprite source (texture `0x140`, palette `0x7948`; `docs/engine.md`). In
  stage 12 he uses other graphics (`stage_object_table+0x8`).
- Position: room (240, −48), that is 48 px above the top of the room. Facing left. Speeds, gravity
  and acceleration 0. No terrain box yet.
- `game+0x20` = this object, `game+0x25` = 0. `PlayerEvent(20, 64)`: the player is held.
- Two crystals (visual type 23, `var` 0 and 1), each with the boss as its parent.
- Globals cleared: the fight timer `0x800F6C0C`, the death flag `0x800F6BF8`, the stun flag
  `0x800F6BFC`, the lost-crystal bits `0x800F6C04`. `0x800F6C08` = this object.
- Then state 1, step 2 (INTRO).

## The `id` parameter

His record (main 57, `docs/stages/turtloid.md`) makes him. The code does not read `var` [certain].

## State machine

States (table `0x800F69A4`): 0 set-up, 1 main (`0x800F0910`), 2 death (`0x800F46F0`). Every update
the entry clears `display`, runs the state and then `CollideCheck`.

### Main (state 1) [certain]

1. **Hurt box and table**, in this order:
   - step 9 (stunned): the whole box, `dmg_tbl_deflect`;
   - both crystals lost (`0x800F6C04` = 3): the whole box, his own table (`dmg_tbl_deflect` while
     the invulnerability counter runs);
   - animation 13, 16, 28, 29 or 30 (shell closing, opening or spinning): the whole box,
     `dmg_tbl_deflect`;
   - else: `ContactWeapon` with the shell box and `dmg_tbl_deflect`. With no hit there, the back box
     and his own table.
2. `ContactWeapon`. A kill: sounds 20 and 32, state 2 (death), speeds 0, `0x800F6BF8` = 1 and
   `mega+0x7A` = 1.
3. If the step is not 9: the attack box (spin or normal), then `ContactMega`. So he does **no
   contact damage while stunned**.
4. A hit (result > 0, not 127): **30 updates of invulnerability** (`+0x84`; 2 updates after weapon
   ids 24 and 25, the first sabre hits): `dmg_tbl_deflect`, and the palette flash toggles every 2
   updates. At the end: his own table again.
5. A hit by **weapon id 2, 11 or 33**: sounds 20, 33 and 120, `0x800F6BFC` = 1, step 9 (STUNNED).
   The damage of the hit is applied first. A deflected hit returns 127, so it cannot stun.
6. The step (table `0x800F69B0`): 0 engine hit reaction, 1 nothing, 2 INTRO, 3 idle (never set),
   4 THINK, 5 MISSILES, 6 SPIN ROLL, 7 BUBBLE ROLL, 8 RAIN ROLL, 9 STUNNED.
7. Outside the intro, the fight timer `0x800F6C0C` counts 1 per update (for the soul at death).

Each step moves him with `MoveObject2`, runs `AnimeTick` and `DisplayCheck(128, 128)`.

### Where he can be hit [certain for the boxes; likely for the picture]

He turns toward the player with "face the player" (`flip` = `0x40` when his x is less than the
player's x). The shell box and the back box are on the side **away** from the player (a facing-left
box with `ox` = 16 is at his x + 16 … x + 48; the player is at his left). The near half of his body
has no hurt box. So a shot from the player's side passes his centre, reaches the shell box and is
deflected. Only a shot that reaches the far edge first (24 px wide, the full 96 px height) does
damage: in practice the player must be **behind him** (jump over him or pass under a jump) while
his shell is open. The sprites agree: at `flip` 0 his head and arm are on the +x side, behind the
shell.

When the shell is closed (spinning or rolling), nothing hurts him, unless both crystals are lost.
When both crystals are lost, every hit on the 96 × 96 px body counts, also while he spins.

### INTRO (step 2, sub-steps `0x800F69D8`) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800F2EC4` | Waits until the player is in step 20 (held). Then camera limit speed 2, `PlayerEvent(29, 64)`, a 40-update timer, and engine object 24 (stage-logic class; probably "WARNING" [guess]). |
| 1 | `0x800F2F5C` | Waits while `game.exit_req` ≠ 0. At the end of the timer: `PlayerEvent(20, 64)`. When object 24 is gone: he faces the player, animation 31 (shell spinning) and falls at **1 px/frame**. |
| 2 | `0x800F0C8C` | Falls (no gravity). Below room y 128 (the midpoint of y 16 and y 240) he gets his terrain box. Sound 56 every 20 updates. On the floor, at the first frame of the spin (sprite 17): animation 4 (he stands up out of the shell). |
| 3 | `0x800F3014` | At update 28: sound 57, `Quake(60, 2, 2)`, `Rumble(3)`. At update 100 (animation 4 has ended): his text (`TextOpen`, `game+0x2E` = 1; not in stage 12). |
| 4 | `0x800F30F8` | Waits until the text is closed (`text_open` = 0). Animation 7 (a taunt). |
| 5 | `0x800F315C` | At update 13: sound 57, `Quake(60, 2, 2)`, `Rumble(5)`. At the end of animation 7 (61 updates): animation 10 (stand), `BossMusicRequest`, `game+0x24` = 1. |
| 6 | `0x800F3220` | Waits while `BossMusicWait` (the boss music wait), then 60 updates. |
| 7 | `0x800F32B0` | **+1 HP per update**, a tick (bank 0, id 14) every 3 updates, up to `boss_hp_max`. Then `PlayerEventEnd` and THINK. |

The fall is 224 px (from room y −48 to y 176, his origin on the floor): about 224 updates, plus up
to 47 updates to reach the spin frame.

### THINK (step 4, `0x800F0DB4`) [certain]

THINK has no wait: it picks a pattern in the same update. `r` = `GetRNG() & 15`. "High HP" means
HP > `boss_hp_max` / 2 (28 at level 0). "Hard" means difficulty 2 (the code compares with 2, so
difficulty 3 uses the Normal choices).

| | high HP | low HP |
|---|---|---|
| Easy, Normal | 0 | r < 8: 0; else 1 |
| Hard | r < 8: 0; else 1 | r < 8: 0; else 2 |

A pattern is a list of steps (`0x800F6924`, 6 bytes each); each step ends by going to the next
entry, and the last entry is THINK:

| pattern | steps |
|---|---|
| 0 | MISSILES, SPIN ROLL |
| 1 | MISSILES, BUBBLE ROLL |
| 2 | MISSILES, RAIN ROLL |

So every cycle starts with the missile volley. BUBBLE ROLL comes only at low HP on Normal (and at
any HP on Hard). RAIN ROLL comes **only on Hard at low HP**. The player's character and position are
not tested.

### MISSILES (step 5, sub-steps `0x800F69FC`) [certain]

1. Sub 0: he faces the player (once; he does not turn again in this step), sub 2. (Sub 1,
   `0x800F0ED8`, a walk at 3 px/frame to room x 336 or x 80, is never entered.)
2. Sub 2, every 8 updates: when the count is {10, 14, 18, 22}[boss level] (table `0x800E98C4`):
   animation 10, sub 3. Else: at the first time, sound 34 and animation 19 (the volley pose); then
   `act4` = `GetRNG() & 1` (which crystal fires) and the count + 1.
3. Each crystal fires a **missile** (visual `var` 2 or 3, below) when the boss counter is 0 and
   `act4` is its `var`. The first count does not fire (the crystals only start their volley step
   then). So **9 missiles at level 0** (13, 17, 21 at higher levels), one every 8 updates, each from
   a random crystal [likely for the count: it depends on the update order; the save state has 8
   missiles at count 9, which agrees].
4. Sub 3: he stands (animation 10) for **300 updates**, while the missiles attack. Then the next
   step.

So the step is about 380 updates at level 0. The shell is open for all of it: the crystals and his
back can be hit.

### SPIN ROLL (step 6, sub-steps `0x800F6A0C`) [certain]

Start (subs 0–2, the same in steps 6, 7 and 8, except the sound):

1. Sub 0: face the player, sound 35 (step 7: 36; step 8: 37), animation 13 (he pulls into the
   shell). Sub 1 comes on the next update, so animation 13 shows for 1 update only.
2. Sub 1: face the player, animation 28 (the shell closes; 10 updates), sound 61.
3. Sub 2: at the end of animation 28: sound 62, animation 30 (the shell spins). **60 updates** of
   spinning in place, facing the player. Then the count = 0 and (step 6) sub 12 on Hard, else sub 3.

Easy and Normal:

| sub | what |
|---|---|
| 3 | Face the player. If the count ≥ 4: sub 9 (the end). If the player is less than 64 px away horizontally: **jump straight up at 3 px/frame** (sub 6). Else: **roll at 3 px/frame toward the player** (sub 4). |
| 4 | Roll until he touches the wall in his direction: sound 63, `Rumble(3)`, `Quake(90, 2, 3)`, stop, count + 1, sub 5. |
| 5 | Wait **60 updates**, then sub 3. |
| 6 | Rise until he touches the ceiling: sound 63, `Rumble(3)`, `Quake(90, 2, 3)`, stop, count + 1, sub 7. |
| 7 | Stay at the ceiling for **60 updates**. Then: if the player is 64 px or more away horizontally, x speed 4 px/frame toward the other half of the room (right when he is left of the middle, else left); else no x speed and he faces the player. Gravity 0.0625 px/frame² (`0x1000`). Sub 8. |
| 8 | Fall until the floor: sound 63, `Rumble(3)`, `Quake(90, 2, 3)`, stop, count + 1, sub 5. |

A roll counts 1 and a jump counts 2, so the step ends after 4 rolls, 2 jumps, or 2 rolls and a
jump. The ceiling is 96 px above his floor position, so a rise takes 32 updates.

Hard (instead of subs 3–8):

| sub | what |
|---|---|
| 12 | If the count is 2: sub 9. Else a **jump at the player**: y speed 5.5 px/frame up, gravity 0.171875 px/frame² (`0x2C00`), x speed = (player x − his x) / 64 px/frame. The flight is 64 updates and 88 px high, so he lands on the player's x of the jump start (if no wall stops him). Sub 13. |
| 13 | On the floor: sound 63, `Rumble(3)`, `Quake(90, 2, 3)`, stop, sub 14. |
| 14 | After **60 updates**: face the player, **roll at 4 px/frame toward the player**, sub 15. |
| 15 | At a wall: sound 63, `Rumble(3)`, stop, count + 1, sub 12. |

So on Hard: two cycles of "jump onto the player, wait, roll to the wall".

End (subs 9–11, the same in steps 6, 7 and 8):

1. Sub 9: at the next spin frame with sprite 0 (up to 48 updates): face the player, sounds 20 and
   64, animation 29 (the shell opens; 12 updates).
2. Sub 10: animation 16 (he comes out of the shell; 12 updates).
3. Sub 11: animation 10, and he stands for **270 updates**. Then the next step (THINK).

### BUBBLE ROLL (step 7, sub-steps `0x800F6A4C`) and RAIN ROLL (step 8, `0x800F6A70`) [certain]

The two steps have the same movement; only the crystals' shots differ.

1. Start as in step 6 (sound 36 or 37), 60 updates of spinning.
2. Sub 3: if the count ≥ 4: the end (as step 6, subs 6–8 here). Else he faces **away from the
   middle side**: right when he is left of the room middle, else left. So each roll crosses the room.
3. Sub 4: the x speed grows by **1 px/frame each update, up to 5 px/frame**. At the wall: stop,
   sound 63, `Rumble(3)`, `Quake(90, 2, 3)` at the right wall or `Quake(30, 2, 3)` at the left wall,
   and **that wall column changes** (below). Count + 1, sub 5.
4. Sub 5: after **50 updates**, both wall columns are put back. Sub 3.
5. While he rolls (x speed ≠ 0), the crystals fire:
   - step 7: a **water bubble** (`var` 4 or 5) and a splash (`var` 6 or 7) every **14 updates**;
   - step 8: a **water ball** (`var` 8 or 9) and a flash (`var` 10 or 11) every {60, 50, 40, 30}
     updates by boss level (table `0x800E98F8`).

   Each time, crystal 0 picks `act4` = `GetRNG() & 1`, and only the crystal whose `var` is `act4`
   fires.

So the step is 4 crossings of 256 px (the 384 px room less his 128 px body), 50 updates at each
wall, then 270 updates of standing.

**The wall column** [certain for the tiles; the effect is a guess]. `CopyTiles` copies a column of
1 × 14 cells (16 px each, from room y 16 to the floor) onto the wall that he hit (room x 0 for the
left wall, x 400 for the right wall). The source cells are below the room, at y 976 (absolute):
x 8944 and x 9344 hold the "hit" columns, collision value 57; x 8960 and x 9360 hold the normal
columns (left: 4 cells of value 59, the gate, then 56; right: 56). After 50 updates, and at the start
of a stun, both normal columns are copied back. What value 57 does to the player is not known
[guess: the player cannot hold on to the wall, so a wall-clinging player falls off at the impact].

### STUNNED (step 9, sub-steps `0x800F6A94`) [certain]

- Entered on a hit by weapon id 2, 11 or 33 (not deflected). `0x800F6BFC` = 1 at once: **all
  missiles, bubbles, water balls and their effects end**, and the crystals wait.
- Sub 0: both wall columns are put back. Animation 33 (stunned), speeds and gravity 0.
- Sub 1: after **50 updates**, gravity 1 px/frame² (he falls if he was at the ceiling or in a jump).
- Sub 2: on the floor: stop.
- Sub 3: after **50 updates**: `0x800F6BFC` = 0, animation 10.
- Sub 4: after **40 updates**: the next step of the pattern (the step after the one that he was in).

While stunned: no contact damage, and every hit is deflected. On the floor the stun is
140 updates.

### Death (state 2, steps in `0x800F6AA8`) [certain]

1. Animation 32, palette flash off, `PlayerEvent(20, <the player's facing>)` (the player is held).
   The kill set `0x800F6BF8` = 1: all crystals, shots and effects end.
2. **127 updates.** The palette flash toggles at shorter and shorter intervals (26, 21, 16, 11, then 6
   updates). Then engine object 26 (stage-logic class; probably the boss explosion [guess]).
3. While object 26 runs he flashes every 6 updates; while its `+0x07` is not 0 he is not drawn. When
   it is gone:
   - in stage 12: `game.exit_req` = `0x80`, `game[0x2C + checkpoint]` = 1, checkpoint + 8 (as
     Commander Yammark; probably the rematch stage [likely]);
   - else a **Nightmare Virus** (main type 15) with `var` **`0x10` (200 souls) when the fight took
     less than 5,401 updates (90 s)**, else `var` `0x11` (100 souls)
     (`docs/enemies/nightmare-virus.md`, BOSS SOUL).

   Then he is deleted.

## Animations (table `0x800F689C`, 34 animations) [certain for the frames]

The routines `0x800F0434` (boss) and `0x800F14D4` (crystals and shots) pick the sprite bank by the
animation number: 0–27 the boss's bank, 28–32 the next bank, 33 the bank after that (texture page
`0x540`, the palette of the next bank) [certain]. The export (`rainy-turtloid`) takes these sets
from the `sets` rules of `catalog/enemies.json` (stage 6, type 57: set 1 = animations 28–32,
texture `0x140`, palette of the bank; set 2 = animation 33, texture `0x540`, palette of bank + 1).
`turtloid-7_boss` draws all 34 strips correctly (entries 14, 15 and 16). The cell grew to
205 × 205 for the new strips, so strips 0–27 have more empty border, with the same pixels.

| # | what | used by |
|---|---|---|
| 0, 1 | crystal, spinning with the shell (crystal 0, 1) | `var` 0, 1 during animation 30 |
| 2, 3 | crystal, one frame | `var` 0, 1 during animations 28, 29 |
| 4 | he stands up out of the shell (28 updates) | intro |
| 5, 6 | crystals with animation 4 | `var` 0, 1 |
| 7 | taunt (61 updates to the last frame) | intro |
| 8, 9 | crystals with animation 7 | `var` 0, 1 |
| 10 | stand (loops) | boss |
| 11, 12 | crystals with animation 10 | `var` 0, 1 |
| 13 | into the shell (3 frames) | steps 6, 7, 8 start |
| 14, 15 | crystals with animation 13 | `var` 0, 1 |
| 16 | out of the shell (3 frames) | steps 6, 7, 8 end |
| 17, 18 | crystals with animation 16 | `var` 0, 1 |
| 19 | volley pose (loops) | MISSILES |
| 20, 21 | crystals with animation 19 | `var` 0, 1 |
| 22 | missile (loops) | `var` 2, 3 |
| 23 | water bubble (loops) | `var` 4, 5 |
| 24 | splash (3 frames, 2 updates each) | `var` 6, 7 |
| 25 | water ball, rising (loops) | `var` 8, 9 |
| 26 | flash (3 frames, 2 updates each) | `var` 10, 11 |
| 27 | water ball, falling (loops) | `var` 8, 9 |
| 28 | shell closes (10 updates, next bank) | steps 6, 7, 8 |
| 29 | shell opens (12 updates, next bank) | steps 6, 7, 8 |
| 30 | shell spins (16 frames of 3 updates, loops; next bank) | steps 6, 7, 8 |
| 31 | shell spins (16 frames of 3 updates, loops; next bank) | intro fall |
| 32 | death pose (one frame, next bank) | death |
| 33 | stunned (loops from frame 4; third bank) | STUNNED |

## Crystals, shots and effects (visual object type 23)

Each copies the boss's graphics (`docs/engine.md`, "copies the parent"). Before the state, every
object of this type runs `CornerCheck` while its `+0x84` is 0 and it has no terrain contact yet. The
`var` picks the state table (`0x800E98C8`, one table for each pair). All of them end when the boss
dies (`0x800F6BF8`). All except the crystals also end at a stun (`0x800F6BFC`). Positions are from
the boss origin.

### The crystals (`var` 0 and 1, `0x800F1900`) [certain]

- **HP 16**, his table `dmg_tbl_turtloid`, **no attack box** (they do no contact damage).
  Hurt boxes: crystal 0 (−16, −30, 32, 16), the upper hole; crystal 1 (−16, 10, 32, 16), the lower
  hole. Draw priority 4. They take his x, y and facing every update.
- They can be hit **only while the shell is open**: in their idle step and in the volley step.
- They follow his animation with their own (table above), in steps: 0 idle, 1 intro, 2 volley,
  3 shell, 4 wait (stun).
- **Lost.** A kill: an explosion, HP 0, bit 0 (crystal 0) or bit 1 (crystal 1) set in `0x800F6C04`.
  A lost crystal is not drawn. On Easy and Normal it waits until **both** are lost; on Hard it starts
  at once. Then after **660 updates** it comes back with 16 HP (sound 65) and its bit is cleared. It
  blinks for the last 60 updates.
- So on Normal: after the second crystal is lost, his whole body takes damage for **660 updates
  (11 s)**, then both crystals come back. On Hard each crystal comes back 660 updates after its own
  loss, so both must be destroyed within 660 updates.
- **A lost crystal still fires** its missiles, bubbles and water balls (the fire code has no HP test)
  [certain for the code].

### Shots and effects

| `var` | object | boxes, HP, damage | movement and end |
|---|---|---|---|
| 2, 3 | missile (`0x800F2348`) | terrain (0, 0, 14, 5); attack = hurt (−14, −5, 28, 10): 28 × 10 px; 4 HP, `dmg_tbl_turtloid`; contact **4**; priority 3 | Starts 8 px in front of him, `var` 2 at y − 24, `var` 3 at y + 16. **1 px/frame** forward for 24 updates, at an angle of 0°, 40° or 80° (`GetRNG() % 3`), up for `var` 2, down for `var` 3. Then 40 updates still. Then sound 60, it faces the player and flies **4 px/frame horizontally**, and its y moves toward the player's y at **0.5 px/frame**. Ends (explosion) on terrain, on the player, or on a weapon kill. |
| 4, 5 | water bubble (`0x800F49C8`) | attack = hurt (−14, −14, 28, 28): 28 × 28 px; 4 HP, `dmg_tbl_turtloid`; contact **4**; priority 3 | Starts on a 24 px circle around him, at the point of the current spin frame (`0x800F6AD8`; `var` 5 the opposite point, `0x800F6B18`). x speed **+2 px/frame (`var` 4) or −2 px/frame (`var` 5)**, y speed **5, 6, 7 or 8 px/frame up** (`GetRNG() % 4`), gravity 0.1875 px/frame² (`0x3000`): an arc to each side. Ends on the player, on a weapon kill (no explosion), or 64 px below the floor. No terrain test: it passes through the floor. |
| 6, 7 | splash (`0x800F4A90`) | none; priority 2 | At the bubble's start point. Ends with animation 24 (4 updates). |
| 8, 9 | water ball (`0x800F2B1C`, step 8 only) | at first none; after the turn attack = hurt (−14, −14, 28, 28); 4 HP, `dmg_tbl_turtloid`; contact **4**; priority 3 | Starts as the bubble. Rises at **8 px/frame** with no boxes until it is 24 px above the room top. Then 50 updates still. Then animation 27, sound 59, the boxes, and it falls diagonally: **2 px/frame** in his facing at the spawn (the roll direction) and **2 px/frame down**. Ends on the player, on a weapon kill (no explosion), or 64 px below the floor. |
| 10, 11 | flash (`0x800F4B50`) | none; priority 2 | At the water ball's start point. Ends with animation 26 (4 updates). |

Note: the missile code has a 3 px/frame speed for "boss HP > `boss_hp_max`", which cannot occur, so
it is always 4 px/frame [certain for the code].

## Damage

`0x800755FC` (`dmg_tbl_turtloid`), for the boss, the crystals and all his shots; not yet in
`docs/engine.md`:

| ids | damage |
|---|---|
| 0–15 | 1 2 5 2 2 2 D – 2 1 2 8 2 2 2 D |
| 16–31 | 2 8 4 4 4 D 8 D 1 1 2 2 2 2 2 2 |
| 32–47 | 2 5 2 2 2 2 2 2 2 2 D 2 D 1 4 8 |
| 48–63 | 2 1 2 2 3 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

The ids that matter:

| ids | damage |
|---|---|
| 0 (lemon), 9 (half charge) | 1 |
| 18, 19, 20 (full charge parts) | 4 each |
| 24, 25 (sabre) | 1 each (+1 sabre bonus: 2) |
| 26–31 (sabre combo) | 2 each (+1: 3) |
| 2 / 11 / 33 (the stun ids) | 5 / 8 / 5 |
| 17, 22, 47, 57 | 8 |
| 60, 62 | 15 |
| 6, 15, 21, 23, 42, 44, 64–78 | deflected |
| 7 | no effect |

With 56 HP (before the part bonuses and the Easy × 1.25): 56 lemons on the back box. A crystal
(16 HP) takes 16 lemons. Missiles, bubbles and water balls (4 HP) take 4 lemons or one full-charge
part.

Dealt (before the difficulty scaling): body 6; missile 4; bubble 4; water ball 4. The crystals do
no damage.

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 56 | intro fall, every 20 updates |
| 57 | intro: after the landing (update 28) and in the taunt (update 13) |
| 34 | MISSILES, the first volley count |
| 60 | a missile starts its dash |
| 35, 36, 37 | start of SPIN ROLL, BUBBLE ROLL, RAIN ROLL |
| 61 | the shell closes |
| 62 | the shell starts to spin |
| 63 | an impact: wall, ceiling or floor in the roll steps |
| 58 | a crystal fires a bubble or a water ball |
| 59 | a water ball turns to fall |
| 20, 64 | the shell opens |
| 65 | a lost crystal comes back |
| 20, 33, 120 | STUNNED |
| 20, 32 | death |
| bank 0, 14 | HP bar tick |

Id 20 comes with three other sounds; it is probably a "stop" request, as in Commander Yammark's
sheet [guess].

## Summary for a reimplementation

- A floor boss with a 128 × 128 px body, contact 6, HP 56 (60 / 64 at boss levels 2 and 3). After a
  hit: 30 updates of invulnerability (2 for the first sabre hits).
- He turns his shell to the player. Frontal shots are deflected; only the 24 px far edge takes
  damage, and only while the shell is open. Two crystals (16 HP each, upper and lower shell holes)
  can be shot while the shell is open; with both lost, his whole body takes damage for 660 updates,
  then they come back (on Hard each comes back 660 updates after its own loss).
- Each cycle: a missile volley (9 missiles, 8 updates apart, from a random crystal; each drifts out
  at 1 px/frame for 24 updates, waits 40, then flies 4 px/frame at the player and follows the
  player's height at 0.5 px/frame), 300 updates of standing, then a shell attack, 270 updates of
  standing.
- Shell attacks: SPIN ROLL (3 px/frame rolls toward the player, or a jump to the ceiling and a drop
  when the player is near; Hard: two jumps onto the player, each followed by a 4 px/frame roll),
  BUBBLE ROLL (low HP; wall-to-wall rolls up to 5 px/frame while bubbles arc out every 14 updates),
  RAIN ROLL (Hard, low HP; water balls rise, then fall diagonally at 2 px/frame).
- Weapon ids 2, 11 and 33 stun him for about 140 updates and clear all his shots.
- Death: he holds the player, flashes 127 updates, explodes, and leaves a boss soul: 200 souls under
  90 s, else 100.

## Open questions

1. Which weapons are ids 2, 11 and 33 (the stun; probably his weakness [guess]).
2. What collision value 57 does on the hit wall column (no wall cling? a play check: cling to the
   wall when he hits it).
3. Engine objects 24 and 26 (stage-logic class) and player event step 29, as for the other bosses.
4. Re-export animations 28–33 from the next two sprite banks (the stunned pose 33 is not seen yet).
5. The missile count per volley (9 at level 0) depends on the update order of main and visual
   objects; check in play.
6. Whether a player behind him can really hit the 24 px back box with the buster before the shell
   box (a play check: shoot him from behind during the volley).
7. Sound id 20.
