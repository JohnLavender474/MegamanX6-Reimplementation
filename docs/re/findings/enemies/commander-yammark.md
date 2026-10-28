# Commander Yammark (stage 03, main object type 49): behaviour sheet

Commander Yammark is the dragonfly Maverick of the jungle stage. He flies in the boss room and never
lands. His attacks come mostly from his **dragonfly drones**: small copies that he summons, that
circle him, and that attack in a different way for each of his moves. He himself dashes across the
room or chases the player. The fight changes with his HP (more drones, harder patterns) and with the
character: some of his moves are only used against Zero, and Zero can knock a drone back into him.

Overlay `03` (Commander Yammark), save state `yammark-2_boss.state.gz` (stage 1, area 0; the boss, 3
drones and the player in the boss room). Written with the RE kit (`python3 -m mmxdemake.rekit --name
commander-yammark`) and `docs/re-guide.md`. Units, confidence tags and the shared engine rules:
`docs/engine.md`. Records: `docs/stages/yammark.md` (record 107). The drones are main type 49 with
`var` 1; the shots and effects are visual type 15. "The room" below is the boss room; positions are
given from its top-left corner (the camera's target minimum x and y at the set-up, `camera_layer0`
`+0x26` and `+0x2A`; x 5104, y 1904 in the save state).

## Spawn and set-up (`0x800EF810`) [certain]

- **HP: 32 + a level bonus.** `boss_hp_max` (`game+0x8C`) = 32 + {0, 6, 12, 16}[level] (table
  `0x800E990C`). The level is `game+0x8F`, written by `0x80025FD8` just before: 3 in stage 12, else
  `0x80071D80[game+0x8D + character]` = {3, 3, 2, 1, 0, 0, 0, 0} for `game+0x8D` = 0–7. `game+0x8D` is
  7 in the save state (level 0, 32 HP) [certain for the code; what `game+0x8D` counts is open]. The HP
  starts at 0 and fills in the intro.
- Contact damage **4**. Attack box and hurt box `0x800F7954` / `0x800F7958` = (−8, −16, 24, 48):
  24 × 48 px. No terrain box until the intro gives him one (below).
- Damage table `dmg_tbl_yammark` (`0x80074BFC`, below). Draw priority 5. Animation table
  `0x800F7B44`. In stage 12 he uses other graphics (from `stage_object_table+0x8`, CLUT `0x7948`).
- He is moved to room (240, −32): above the top edge of the view.
- `game+0x20` = this object (the boss pointer), `game+0x25` = 0.
- `PlayerEvent(20, 64)`: the player is held (event hold, `docs/engine.md`, "The player").
- Room points (in `0x800F7DC0`…`0x800F7DDE`): x 16 and 336 (the left and right limits), x 48 and 304
  (his two side points), y 96 and 150 (his two heights), y 192 (the floor test), x 32, 176, 320 and
  y −32 (where the intro drones enter; slots 3 and 4 use the midpoints 104 and 248, computed in the
  drone code), y 48, 96, 144 (drone column heights).
- The drone counts (`0x800F7E00`…`0x800F7E0B`, copied from `0x800F7BD0`): pairs (drones, drones that
  stay back) = **(3, 1)**, **(4, 2)** and **(6, 3)**. On Hard (difficulty 2) the second number is 2
  less (minimum 0): (3, 0), (4, 0), (6, 1). The pair in use depends on the HP (THINK, below).
- Then step 2 (the intro).

## The `id` parameter

Record 107 has `id` 0. The code does not read the record `id`. `var` (`+0x02`) = 1 makes the object
a drone; the boss has `var` 0 [certain].

## State machine

States (table `0x800F7BDC`): 0 set-up, 1 main (`0x800EFB9C`), 2 death (`0x800F54A4`). Every update the
entry clears `display` and runs `CollideCheck` after the state.

### Main (state 1) [certain]

1. `ContactWeapon`. A kill: state 2 (death).
2. `ContactMega`.
3. A hit (result > 0, not 127; so also 126, "already hit"): **30 updates of invulnerability** (`work.84`; 2 updates after weapon ids
   24 and 25, the first sabre hits): `dmg_tbl_deflect` and the flash (CLUT bit 15 toggles every 2
   updates). At the end: `dmg_tbl_yammark` again.
4. A hit by **weapon id 8, 17 or 44**, while no knocked drone is hitting him: step 10 (stunned) and the
   global `0x800F7DE8` = 1 [certain for the ids; which weapons they are is not known; probably his
   weakness].
5. The step (table `0x800F7BE8`): 0 engine hit reaction, 1 nothing, 2 INTRO, 3 nothing (not used),
   4 THINK, 5–9 attacks, 10 STUNNED.
6. Outside the intro, the fight timer `0x800F7DE0` counts 1 per update (used for the soul at death).

Each attack step moves him with `MoveObject2` (no gravity is set, so he flies) and runs
`AnimeTick` and `DisplayCheck(64, 64)`.

### INTRO (step 2, sub-steps `0x800F7C14`) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800F4188` | Waits until the player is in step 20 (held). Then camera limit speed 2, `PlayerEvent(29, 64)` (event step 29, not read), a 40-update timer, and engine object 24 (probably the "WARNING" display [guess]). |
| 1 | `0x800F4220` | Does nothing while `game.exit_req` ≠ 0. At the end of the timer: `PlayerEvent(20, 64)`. When object 24 is gone: the wings effect (visual 15, `var` 4), animation 0, and he descends at 1 px/frame. |
| 2 | `0x800F42D8` | Below room y 64 he gets the terrain box `0x800F7950` = (0, 8, 16, 32) (a 32 × 64 px body). When it touches the floor: he stops and makes the first drones (3, or the count of the current pair), which fly in from above (drone step 0). |
| 3 | `0x800F43F4` | Waits until the drones have arrived and spread out. Then, except in stage 12, his text (`TextOpen`; `game+0x2E` = 1 after it), drone mode 2. |
| 4 | `0x800F44A4` | Waits until the text is closed (`0x8008EAFC` = 0). Animation 1 (wings open, the spear comes out). |
| 5 | `0x800F4508` | At the end of animation 1: animation 0, `BossMusicRequest`, `game+0x24` = 1. |
| 6 | `0x800F4588` | Waits while `BossMusicWait` (the boss music wait) returns non-zero. Then the HP bar fills: **+1 HP per update**, a tick (bank 0, id 14) every 3 updates, up to `boss_hp_max`. Then `PlayerEventEnd` (the player can move) and THINK. |

Note: he touches the floor only in the intro. The terrain box stays, so he collides with the walls,
the floor and the ceiling during the fight.

### THINK (step 4, `0x800EFD38`) [certain]

1. **HP phase.** HP < 1/3 of `boss_hp_max`: the drone pair (6, 3). HP < 2/3: (4, 2). Else the pair
   does not change (it starts at (3, 1)). The same test runs again at the start of each attack.
2. `r` = `GetRNG() & 15`. A pattern (`self+0x86`):

   | | HP > half | HP ≤ half |
   |---|---|---|
   | X | r < 8: 0, else 2 | r < 4: 0; r < 12: 4; else 5 |
   | Zero | r < 8: 1, else 3 | r < 4: 1; r < 12: 4; else 5 |

3. The pattern is a list of steps (`0x800F7BA0`, 6 bytes each); each attack ends by going to the next
   entry:

   | pattern | steps |
   |---|---|
   | 0 | 5, 7, THINK |
   | 1 | 6, 7, THINK |
   | 2 | 5, 7, 7, THINK |
   | 3 | 6, 7, 7, THINK |
   | 4 | 9, 8, THINK |
   | 5 | 9, 7, 7, THINK |

So against X he uses step 5 and never step 6; against Zero step 6 and never step 5. Steps 8 and 9
come only below half HP.

### The common start of each attack (sub-steps 0–4) [certain]

Steps 5–9 start the same way:

1. **Sub 0:** the HP phase test (above); animation 0. If the drone mode is not 2 (in formation): mode 1
   (the drones come back to him) and sub 1; else sub 2.
2. **Sub 1:** waits until all drones are back in formation.
3. **Sub 2:** he flies to a side point at **2 px/frame**: x 48 when he is in the left half of the room
   (then he faces right), else x 304 (he faces left). The height: steps 5 and 6: y 96 or y 150 at
   random; step 7 and 8: y 96; step 9: y 150. Animation 2 (horizontal), 3 (up) or 4 (down).
4. **Sub 3:** when he is there: drone mode 0, and he **summons new drones**: as many as the pair
   allows, minus the drones alive. Each new drone is main type 49, `var` 1, step 1. If he summoned
   any: animation 5 (arms up), sound (bank 2, id 34), the summon flash (visual 15, `var` 5).
5. **Sub 4:** waits until the new drones are in place (at least about 100 updates after the summon:
   38 + the appear animation + 60 [likely]). Then the attack. In steps 5 and 6, sub 5 first waits 2 updates
   before it sets the attack mode.

### Step 5: drone bombardment (X only, `0x800F46E0`) [certain]

- Drone mode 3: the drones leave the formation and fly straight up and down (drone step 4, below) and
  shoot.
- He stays at his side point. Sound (bank 2, id 38) after 120 updates. After **500 updates**: the next
  step of the pattern.

### Step 6: drone lance (Zero only, `0x800F4980`) [certain]

- Drone mode 4: the drones form a column in front of him and dash at the player one at a time (drone
  step 5, below). The rhythm: his counter `work.7E` counts up each update; a drone that reaches its
  column height sets it to 60; a drone in the column dashes when it is 75 or more and sets it to 0.
  So a dash comes **15 updates after the latest drone arrives or returns**, and **75 updates after the
  last dash** when no drone arrives. The drone that dashes is the first one updated, not the next slot.
- `act4` (`+0x07`) swings between −30 and +30 by 2 per update; the drones in the column add it to their
  height, so the column bobs up and down by ±30 px.
- **A drone that Zero hits during its dash is knocked back** (any non-zero `ContactWeapon` result
  while the player is Zero; the drone is not killed). When it comes near him (|dx| ≤ 16 and
  −32 ≤ his y − drone y ≤ 16), while he is in step 6 and `0x800F7E30` is 0: he loses **3 HP**
  (`0x800F7E30` = 30), animation 8, sound (bank 2, id 33), no contact damage and `dmg_tbl_deflect`
  for 29 updates. If the HP reaches 0, he dies.
- Sound (bank 2, id 35) after 120 updates. After **600 updates**: the next step.

### Step 7: dash (`0x800F4B98`) [certain]

- He flies to the side point at y 96 (sub 2), summons (sub 3); when the drones are in place: sound (bank 2, id 39), drone mode 2:
  the drones circle him (drone step 3).
- **Sub 5:** he faces the player and follows the player's height at 2 px per update for 120 updates
  (sound bank 2, id 40 after 90).
- Then he **dashes horizontally toward the player at 2 px/frame** until he hits a wall. During the dash
  each drone fires a shot (visual 15, `var` 1) every 20–45 updates (table `0x800F7D34`, by drone slot:
  30, 25, 45, 35, 25, 20, 35, 40).
- At the wall: drone mode 0, the next step.

### Step 8: wall dash (`0x800F4DE0`, HP ≤ half) [certain]

- Side point at y 96, summon; when the drones are in place: sound (bank 2, id 39), drone mode 5: the drones form a vertical wall
  above and below him (drone step 6).
- Sound (bank 2, id 41) after 90 updates. After **260 updates** he dashes horizontally at **2.5
  px/frame** in his facing direction.
- At a wall he stops. After 3 walls: the next step. Else he waits **50 updates**, turns and dashes the
  other way.

### Step 9: trail chase (`0x800F517C`, HP ≤ half) [certain]

- Side point at y 150, summon; when the drones are in place: animation 2, sound (bank 2, id 39), drone mode 6: the drones follow his
  path (drone step 7).
- Sound (bank 2, id 42) after 90 updates. Every 8 updates he aims at the player and flies at **2
  px/frame** in that direction (a homing chase). Each update his position and facing are written to a
  210-entry history (`0x800F7E40`, 8 bytes each), which the drones follow.
- It ends after **500 updates**, or when no drone is alive.

### STUNNED (step 10, `0x800F5358`) [certain]

- Entered on a hit by weapon id 8, 17 or 44 (the damage of the hit is applied first).
- He stops, animation 8, sound (bank 2, id 33), no contact damage. He keeps flashing and cannot be
  hurt (`work.84` is held between 15 and 30).
- `0x800F7DE8` = 1 for the whole stun: new drones (step 1) are deleted, the other drones freeze in
  place (drone step 99), and all shots, bombs and bursts are deleted.
- After **90 updates**: animation 0, boxes and table back, `0x800F7DE8` = 0, THINK.

### Death (state 2, steps in `0x800F7CC8`) [certain]

1. Animation 7, sound (bank 2, id 32), `PlayerEvent(20, …)` (the player is held). The drones and shots
   delete themselves (`0x800F7DE4` = 1).
2. **127 updates** of blinking; the interval gets shorter (26, 21, 16, 11, then 6 updates). Then
   engine object 26 (probably the boss explosion [guess]).
3. While object 26 runs he blinks every 6 updates; while its `+0x07` is not 0 he is not drawn. When
   it is gone:
   - in stage 12: `game.exit_req` = `0x80` and a flag in `game+0x2C`… (`game+0x1D` += 8) [certain for
     the writes; the meaning is not read];
   - else a **Nightmare Virus** (main type 15) with `var` **`0x10` (200 souls) when the fight took
     less than 5,401 updates (90 s)**, else `var` `0x11` (100 souls)
     (`docs/enemies/nightmare-virus.md`, BOSS SOUL).

   Then he is deleted.

## The drones (main type 49, `var` 1)

States (table `0x800F7D54`): 0 set-up `0x800F1A3C`, 1 main `0x800F1B54`, 2 delete `0x800F56B4` (frees
the slot, alive count − 1).

### Set-up [certain]

- Copies the boss's graphics and animation table. **HP 8**, contact damage **2**,
  `dmg_tbl_yammark`, priority 4. No boxes at first.
- Takes the first free slot `i` (0 to the pair's drone count − 1) in `0x800F7E20`, and adds 1 to the
  alive count `0x800F7E10`.
- Boxes when they attack: terrain box `0x800F795C` = (0, 0, 16, 8) (32 × 16 px); attack box
  `0x800F7960` and hurt box `0x800F7964` = (−16, −6, 32, 12): **32 × 12 px**.

### Main [certain for the code]

1. The boss is dead (`0x800F7DE4`): delete.
2. `ContactWeapon`. A kill: an explosion, 2 debris (animations 21, 22), delete. Exception: a drone in
   its lance dash (step 5, sub 5) against Zero is not killed; it is knocked back (below).
3. `ContactMega`.
4. The boss is stunned (`0x800F7DE8`): a drone in step 1 is deleted; the others write mode 0 every
   update and go to step 99 (a knocked drone, slot −1, keeps its step but also stops moving).
5. Else the **drone mode** `0x800F7E0E` sets the step: 1 → step 2; 2 → step 3; 3 → step 4; 4 → step 5;
   5 → step 6; 6 → step 7 (it saves the boss's position). In modes 3 and 4 only the drones with slot
   < (count − stay back) take part; the others stay in step 2/3.
6. The step, then the display. Each step has its own rule [certain for the code]:
   - **drawn every update:** step 0 (intro); step 1 sub-steps 0–1 (appearing); the attack sub-steps
     (step 4–6 from sub 2, step 7 from sub 1); step 99;
   - **drawn every other update** (flicker, `work.7E` odd): step 1 from sub 2, all of steps 2 and 3
     (back and circling), steps 4–6 sub-steps 0–1, step 7 sub 0.

   So the drones flicker for all the time they circle him; they are solid when they enter, appear,
   attack or are frozen. A summoned drone is not drawn and does not move for its first 38 updates.

**Turning to a point on the circle** (steps 4, 5, 6) [certain for the code]: the drone counts its
arrivals at the target angle in `+0x8E` and starts its attack at the **second** arrival. After the
first arrival it keeps turning, so it turns **one more full lap (120 updates)**. So the attack starts
120–240 updates after the mode change.

The drones' circle is **32 px** around the boss. Each drone's angle comes from `0x800F7CD4`: equal
parts of a circle for 3, 4, 5, 6, 7 or 8 drones. Turning on the circle is 546 per update (of 65,536):
**3° per update**, one turn in 120 updates; the direction follows the boss's facing.

| step | when | what |
|---|---|---|
| 0 | the first drones (intro) | They enter at room y −32, at x 32, 176, 320, 104 or 248 by slot, and fly to the boss at 2 px/frame (slot 0: 2.81 px/frame). When all are within 2 px of him, they move out to the circle at 1 px per update. |
| 1 | a summoned drone | After 38 updates it appears on its circle place (animation 9). 60 updates after the animation, it counts as "in place". |
| 2 | mode 1: back | No boxes. It moves 4 px per update in x and in y to its circle place, then stays there. |
| 3 | mode 2: circle | It turns around the boss. During his dash (step 7) it fires a shot (`var` 1) every 20–45 updates. |
| 4 | mode 3 (step 5, X) | It turns to the top of the circle (even slots) or the bottom (odd slots), with the extra lap (above). Then boxes on, animation 10, and it flies **straight up or down at 3.75 px/frame**, bouncing on the ceiling and the floor. A shot (`var` 0) **every 30 updates**. |
| 5 | mode 4 (step 6, Zero) | It turns to the front of the circle (with the extra lap), moves to 32 px in front of the boss at 3 px per update, then to its column height (slot 0–6: y 48, 144, 96, 128, 64, 112, 80; all + the boss's `act4`) and sets his counter to 60. Then it waits until his counter is 75 or more (the rhythm in step 6, above), faces the player, animation 13, sound (bank 2, id 51), and **dashes at the player's position at 6 px/frame**, with a trail (`var` 6). At the target (within 5 px) or on terrain it stops, waits 60 updates and goes back to the column. |
| 5, knocked | Zero hits it in the dash | No boxes, slot freed, it flies back at 4 px/frame in x (away from its facing) and 4 px/frame up, with gravity 0.156 px/frame² (`0x2800`). Within 16 px of the boss (step 6): he loses 3 HP. Deleted when it is more than 32 px outside the room's x limits. |
| 6 | mode 5 (step 8) | Its x stays the boss's x. It turns to the top or the bottom of the circle (with the extra lap), gets an attack box and a hurt box (no terrain box), and moves to boss y + {−40, 40, −60, 60, −20, 20, −80, 80}[slot] at 3 px per update. From his first dash on it bobs 1 px per update: 32 updates one way, 32 the other (a 64-update period, 32 px of travel; bit 5 of his `work.7E`; odd slots opposite). It also bobs while he waits at a wall. |
| 7 | mode 6 (step 9) | It circles the boss's saved position. Drone `i` starts at update (i + 1) × 30 of his chase: boxes on, and it follows his 210-entry path at 3 px per update. When the player is within 12 px horizontally and more than 48 px below it: a bomb (`var` 2, animation 12), then 30 updates before the next bomb. |
| 99 | the boss is stunned | It stops in place (no move). |

## Animations (table `0x800F7B44`, 23 animations) [certain for the frames]

| # | what | used by |
|---|---|---|
| 0 | hover | boss |
| 1 | intro pose: the wings open, the spear comes out | boss (intro) |
| 2, 3, 4 | fly horizontally, up, down | boss |
| 5 | summon: arms up | boss |
| 6 | a long spear pose | not started by the code |
| 7 | death | boss |
| 8 | hurt / stunned | boss |
| 9 | drone appears | drone step 1 |
| 10 | drone attack pose | drones |
| 11 | drone flying | drones |
| 12 | drone drops a bomb | drone step 7 |
| 13 | drone lance dash | drone step 5 |
| 14 | the wings | visual `var` 4 |
| 15 | summon flash | visual `var` 5 |
| 16 | a green ring | not started by the code |
| 17 | drone shot | visual `var` 0, 1 |
| 18 | bomb | visual `var` 2 |
| 19 | dash trail | visual `var` 6 |
| 20 | burst | visual `var` 3 |
| 21, 22 | drone debris | drone death |

## Shots and effects (visual object type 15)

Each copies its parent's graphics (`+0x7C` = the parent). Before the state, every object of this type
runs `CornerCheck` while its `+0x84` is 0 and it has no terrain contact yet. The `var` picks the
state table (`0x800E99B8`):

| `var` | object | boxes and damage | movement and end |
|---|---|---|---|
| 0, 1 | drone shot (`0x800F3BE4`) | terrain (0, 0, 7, 7), attack and hurt (−7, −7, 14, 14): 14 × 14 px; contact **2**; 1 HP; `dmg_tbl_yammark` | Sound (bank 2, id 50). Easy and Normal: **3.75 px/frame horizontally** toward the player. Hard: **aimed at the player at 3 px/frame**. Ends (with an explosion) on terrain, on a weapon kill, or when it touches the player. |
| 2 | bomb (`0x800F57DC`) | terrain (0, 0, 2, 6), attack (−2, −6, 4, 12): 4 × 12 px; contact **2**; cannot be hit | Starts at 1 px/frame down and speeds up by 0.0625 px/frame² to 3 px/frame. On the floor: 3 bursts (at +10/−8, −16/−4, +4/−12 px, after 1, 8 and 16 updates), then it is deleted. It does not end on the player. |
| 3 | burst (`0x800F58E8`) | attack (−8, −8, 16, 16): 16 × 16 px; contact **4** | After its delay: an explosion, then 29 updates of contact. |
| 4 | wings (`0x800F5AB8`) | none | On the boss, 3 px behind and 6 px up. When his animation (`+0x17`) becomes 0, 2, 3 or 4: the wing sound (bank 2, id 48), once. While it is another animation: sound (bank 2, id 20) on every update [certain for the code; id 20 probably stops the wing sound, guess]. Drawn only while his animation is 0. |
| 5 | summon flash (`0x800F5B90`) | none | On the boss; sound (bank 2, id 49) at update 22; deleted at the end of its animation. |
| 6 | dash trail (`0x800F5D58`) | none | On the drone, while the drone is in its dash (step 5, sub 5). |

The shots, the bomb, the bursts and the summon flash are deleted when the boss dies or is stunned, or
while a knocked drone hits him. The dash trail is deleted only when the boss dies or its drone leaves
the dash. The wings stay.

## Damage

`dmg_tbl_yammark` (`0x80074BFC`), for the boss, the drones and the drone shots (full table in
`docs/engine.md`). The ids that matter:

| ids | damage |
|---|---|
| 0 (lemon), 9 (half charge) | 1 |
| 18, 19, 20 (full charge parts) | 4 each |
| 24, 25 (sabre) | 2 each |
| 8 / 17 / 44 (stun ids) | 5 / 8 / 8 |
| 1, 10, 21, 23, 54, 64–78 | deflected |
| 55, 79 | 127 |

So with 32 HP: 32 lemons, or 8 full charges, or 16 sabre hits (the sabre gives only 2 updates of
invulnerability). A drone (8 HP) takes 8 lemons or 2 full charges.

Dealt (before the difficulty scaling): boss body 4; drone body 2; drone shot 2; bomb 2; burst 4.

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 32 | death |
| 33 | stunned, or hit by a knocked drone |
| 34 | summon |
| 35 | step 6, after 120 updates |
| 38 | step 5, after 120 updates |
| 39 | steps 7, 8, 9: when the summoned drones are in place (sub 4) |
| 40 | step 7: update 90 of the 120-update track, 30 updates before the dash |
| 41 | step 8: update 90 of the 260-update wait, 170 updates before the dash |
| 42 | step 9: update 90 of the chase (the chase moves from update 0) |
| 48 | wings, when he starts to hover or fly |
| 20 | every update while the wings effect is not shown (probably "stop", guess) |
| 49 | summon flash |
| 50 | drone shot |
| 51 | drone lance dash |
| bank 0, 14 | HP bar tick |

## Summary for a reimplementation

- A flying boss, 24 × 48 px, contact 4, HP 32 (+6/+12/+16 at higher boss levels). After a hit: 30
  updates of invulnerability (2 for sabre hits).
- He fights through dragonfly drones (8 HP, 32 × 12 px, contact 2) that circle him at 32 px: 3 drones
  above 2/3 HP, 4 below, 6 below 1/3. Each attack starts at a side point, where he refills the drones.
  Circling drones flicker; before an attack they turn an extra lap (120 updates) to their start
  point on the circle.
- Attacks: drones bounce up and down and shoot (vs X, 500 updates); drones dash at the player one by
  one, 15 updates after the latest drone reaches the column or 75 after the last dash (vs Zero, 600
  updates; a drone that Zero hits flies back and takes 3 HP from him); he dashes at
  the player while the drones shoot; below half HP, a wall dash (3 crossings, drones in a column) and
  a homing chase with the drones in his trail dropping bombs.
- Weapon ids 8, 17 and 44 stun him for 90 updates and freeze the drones.
- Death: he holds the player, blinks 127 updates, explodes, and leaves a boss soul: 200 souls under
  90 s, else 100.

## Open questions

1. What `game+0x8D` counts (the boss level is high when it is small), and the level in the real game
   flow. A save state later in the game would show it.
2. Which weapons the ids 8, 17 and 44 are (the stun).
3. Player event step 29 (intro) and engine objects 24 and 26.
4. The stage 12 branch (other graphics, no text, the exit request): probably the boss rematch stage.
5. Animations 6 and 16 are never started by this code.
6. The drone timings are hand-read; check the column, the lance rhythm and the trail in play.
