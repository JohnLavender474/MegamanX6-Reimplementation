# Metal Shark Player (stage 07, main object type 56): behaviour sheet

Metal Shark Player is the Maverick of the scrapyard stage (stage 4, area 1). They are a shark
Reploid with an anchor. They jump and throw a **bouncing anchor**, dive into the scrap floor and
slide at the player as a **fin**, jump high and **slam down** on the player, and **burrow** under
the floor to come up below the player. At low HP they also call up one of three **old Mavericks
made of junk** (a "resurrection"): a short-lived side kick that attacks and then breaks up. Three
weapon ids stun them; the stun freezes them in the air and makes them fall.

Stage overlay `07` (`rock_bin/07.bin`, loaded at `0x800E9860`, to `0x800F987C`; no boss overlay:
nothing is loaded at `0x800FA000` in the boss states) [certain: 156 of 65 564 bytes differ in
`shark-7_boss`, all of them variables]. The boss code is at `0x800F1F80`–`0x800F79D0`. Save states
(stage 4, area 1, checkpoint 2, difficulty 1; X in form 1):

| state | what |
|---|---|
| `shark-6_before-boss` | the room before the boss room; no boss live |
| `shark-7_boss` | the first anchor throw: boss in step 5 sub 3, HP 48, the anchor (visual 22 `var` 0) in flight |
| `shark-8_boss-stun` | after a hit by weapon id 6 (Rainy Turtloid's weapon, slot 6): step 3 sub 1, HP 23 |
| `shark-9_boss-revive` | a side kick (main 56 `var` 2) forms; the boss waits in step 8 sub 4, HP 8 |

Written with the RE kit (`python3 -m mmxdemake.rekit --state shark-7_boss.state.gz --type 56 --name
metal-shark-player`) and `docs/re-guide.md`. Units, confidence tags and the shared engine rules:
`docs/engine.md`. The stage: `docs/stages/shark.md` ("Area 1"). The side kicks are main type 56
with `var` 1–3; all shots, debris and effects are visual type 22 (in overlay 07).

"The room" below has its origin at the camera target minimum x and y (`camera_layer0` `+0x26`,
`+0x2A`; x 6064, y 224 in the states). The camera can scroll 32 px (x 6064–6096), so the room is
352 px wide; the code takes room x 176 as the middle. The floor is at room y 176 (X stands at y
395). Standing, the boss origin is 41 px above the floor (terrain box below).

## Spawn and set-up (`0x800F1F80`, `var` 0) [certain]

- **HP: 48 + a level bonus.** `boss_hp_max` (`game+0x8C`) = 48 + {0, 0, 4, 8}[boss level] (table
  `0x800E9A6C`; level from `BossLevel`, `game+0x8F`): **48, 48, 52, 56**. The states have level 0
  (`game+0x8D` = 5) and `boss_hp_max` 48. The HP starts at 0 and fills in the intro.
- **Contact damage 4** (8 in the slam of step 6).
- Boxes. The hurt box is always the same as the attack box. While they are drawn, `0x800F66FC` picks
  both boxes each update by the current sprite; while `display` is 0 both are null:

  | sprites | box | size | when |
  |---|---|---|---|
  | 0–19, 27, 31–100 | (−16, −24, 32, 56) (`0x800F9088`, `0x800F90B0`, the same values) | 32 × 56 px | standing, jumping |
  | 20–26 | (−32, −16, 64, 32) (`0x800F90A8`) | 64 × 32 px | the fin slide (step 4) |
  | 28–30 | (−8, −8, 16, 16) (`0x800F90A0`) | 16 × 16 px | the fin of the burrow (step 7) |
  | 101, 102 | (−8, 8, 16, 32) (`0x800F9090`) | 16 × 32 px | going into the floor |
  | 103, 104 | (−8, 32, 16, 16) (`0x800F9098`) | 16 × 16 px | the same, last frames |

  Terrain box `0x800F907C` (0, 5, 16, 36): 32 × 72 px, bottom 41 px below the origin. The fin slide
  uses `0x800F9080` and the burrow `0x800F9084`, both (0, 0, 16, 4): 32 × 8 px.
- Damage table `0x8007555C` (named `dmg_tbl_shark` here; "Damage", below). Draw priority 5.
  Animation table `0x800F952C` (40 animations). Texture page `0x140`; sprite data and palette
  from their bank, or from the next bank for 4 animations (see "Animations"). In stage 12 they use
  `stage_object_table+0x8` (palette `0x7948`, set 1 `0x794C`).
- Position: their record (main 56 `id` 0 at (6231, 350), `docs/stages/shark.md`); the intro moves
  them. `display` = 0. Facing left. Speeds 0. No terrain box yet.
- Fields cleared: the fight timer `0x800F9874`, the piece counters `0x800F9868` and `0x800F986A`,
  the pattern fields (`+0x8A` = −1, `+0x8B`, `+0x8C`), `+0x8D` (buried), `+0x8E` (fin).
- Animation 2 (stand), then state 1, step 2 (INTRO).

## The `var` parameter [certain]

| `var` | object | set-up |
|---|---|---|
| 0 | Metal Shark Player | their record |
| 1 | side kick "beam" (seen with Zero) | HP 24, priority 6, state 3, step 2 |
| 2 | side kick "ceiling" (seen with X) | HP 24, priority 6, state 3, step 3 |
| 3 | side kick "hornet" (Hard only) | HP 24, priority 6, state 3, step 4 |

All four get `dmg_tbl_shark` and contact damage 4. The record has `id` 0; the boss makes 1–3
(`0x800F67D0`), copying their graphics, position and facing, with the boss in `+0x80`.

## State machine

States (table `0x800F95CC`): 0 set-up, 1 boss main (`0x800F213C`), 2 boss death (`0x800F428C`), 3
side kick (`0x800F2EE4`), 4 delete (`0x800F4454`). The entry (`0x800F4230`) saves the position,
runs the state and then `CollideCheck`. Step table of the boss `0x800F95E0`: 0 engine hit
reaction, 1 nothing, 2 INTRO, 3 STUNNED, 4 FIN, 5 ANCHOR, 6 SLAM, 7 BURROW, 8 RESURRECTION, 9 POP-UP.

### Main (state 1) [certain]

1. `AnimeTick`, the step, then the boxes by sprite (above).
2. Outside the intro: the fight timer `0x800F9874` counts 1 per update (for the soul).
3. `ContactWeapon`:
   - a kill: `mega+0x7A` = 1, state 2 (death);
   - a hit (result > 0, not 127): `dmg_tbl_deflect` and **30 updates of invulnerability**
     (`+0x88`; 2 after weapon ids 24 and 25, the first sabre hits). Then, by the step:

     | step | weapon id 6, 15 or 42 | weapon id 35 |
     |---|---|---|
     | 7 BURROW | 120 updates of invulnerability, no stun | while buried (`+0x8D` = 1): POP-UP (step 9) |
     | 8 RESURRECTION | 120 updates of invulnerability, no stun | normal hit |
     | any other step | **STUNNED** (step 3) | normal hit |

     The stun and the pop-up set the invulnerability to 10 000 (it lasts until the step ends it).
     The damage of the hit is applied first. A deflected hit returns 127, so it cannot stun.
4. While the invulnerability counts down, the palette flash toggles every 2 updates. At 0:
   `dmg_tbl_shark` again, flash off.
5. `ContactMega`.

So **they can be hit at any time while they are drawn**, except in the invulnerability after a hit and
in the stun. They are not drawn (no boxes) for the first 90 updates of the burrow.

### INTRO (step 2, sub-steps `0x800F96A8`) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800F44CC` | `PlayerEvent(20, 64)`: the player is held. |
| 1 | `0x800F4504` | Waits until `game.exit_req` = 0 and the player is in step 20. Then `PlayerEvent(29, 64)` (not in stage 12), engine object 24 (stage-logic class; probably "WARNING" [guess]), a 20-update timer. |
| 2 | `0x800F45A0` | At the end of the timer: `PlayerEvent(20, 64)`. When object 24 is gone: `game+0x24` = 1, `game+0x25` = 0, `game+0x20` = this object, `BossLevel`, `boss_hp_max` (above); 120 updates. |
| 3 | `0x800F2D98` | **120 updates of rumble**: debris (visual 22 `var` 16) at room (240, 224) every 4 updates, sound 94 and `Rumble(5)` every 16 updates, and `Quake2(2, 4, 2)` every 4 updates in the last 60. At the end: animation 0, `display` = 1, they are at room (240, 272) (32 px below the view), y speed **4.40 px/update up** (`0x46640`), gravity **0.055 px/update²** (`0xB00000 / 3200`). |
| 4 | `0x800F4674` | They rise out of the scrap. At the top (y speed < 0; about 80 updates, 176 px up, room y 96): animation 1, the terrain box. |
| 5 | `0x800F46CC` | They hang at the top (no move) for animation 1 (a spin and a pose, 71 updates; sound 93 at its flag 2, update 32). At its flag 1: animation 6 (fall). |
| 6 | `0x800F472C` | They fall (same gravity). On the floor: sound 92, animation 7, their text (`TextOpen`, `game+0x2E` = 1; not in stage 12). |
| 7 | `0x800F47C8` | When the text is closed (`text_open` = 0): `BossMusicRequest`. |
| 8 | `0x800F480C` | Waits while `BossMusicWait` (the boss music wait). |
| 9 | `0x800F4848` | **+1 HP per update**, a tick (bank 0, id 14) every 3 updates, up to `boss_hp_max`. |
| 10 | `0x800F48CC` | `PlayerEventEnd`, THINK. |

### THINK (`0x800F33F8`) [certain]

THINK has no wait. It runs at the end of every step and picks the next step from a pattern (8
bytes each; `0xFF` ends a pattern). It also sets contact damage 4. "High HP" means HP >
`boss_hp_max` / 2 (24 at level 0).

| table | patterns (`GetRNG() & 15`) |
|---|---|
| high HP (`0x800F8F7C`) | 0–2: FIN, ANCHOR; 3–5: ANCHOR; 6–8: FIN, FIN, SLAM; 9–11: SLAM, SLAM, BURROW; 12–15: FIN, FIN |
| low HP (`0x800F8FFC`) | 0–5: ANCHOR, FIN, RESURRECTION; 6–11: ANCHOR, SLAM, RESURRECTION; 12–15: FIN, FIN |

1. The first time (`+0x8A` = −1): a pattern from the high table.
2. In the high mode (`+0x8A` = 0), each THINK first tests the HP: at HP ≤ half it changes **at once**
   (also in the middle of a pattern) to the low mode with a new pattern from the low table.
3. Else the next entry of the pattern. At `0xFF`, a new pattern from the same table.

So **BURROW comes only at high HP** and **RESURRECTION only at low HP**. The difficulty and the
player are not tested here (the side kick is picked in step 8).

### FIN (step 4, sub-steps `0x800F9614`) [certain]

1. Sub 0: animation 13, face the player, a **straight-up jump of 70 px**: apex time t = 10–24
   updates (`GetRNG() % 15 + 10`), gravity = 70 px / (t² / 2), y speed = gravity × t.
2. Sub 1: at the apex: animation 14. Sub 2: on the floor: animation 38 (the dive).
3. Sub 3, at the first flag of animation 38 (update 6): sound 95, they go into the floor (y + 28,
   flat terrain box, `+0x8E` = 1), **x speed 4 px/update** in their facing, no gravity.
4. Sub 4: they slide as a fin. Sound 95 every 50 updates. Debris (`var` 21 or 20, thrown behind them)
   every 8 updates. When they **passes the player's x**, or meets a wall in front: animation 39, they
   comes out (y − 28, the normal terrain box), stop.
5. Sub 5: **90 updates** (animation 39: they jump out and stand), then THINK.

The fin box is 64 × 32 px, from 29 px above the floor to 3 px below it: it hits a standing player.
Jump over it. Contact 4.

### ANCHOR (step 5, sub-steps `0x800F962C`) [certain; confirmed in `shark-7_boss`]

1. Sub 0: sound 91, animation 3, face the player.
2. Sub 1: at the end of animation 3 (13 updates): animation 4, a **straight-up jump of 60–79 px**
   (`GetRNG() % 20 + 60`), apex time 10–24 updates (as FIN).
3. Sub 2: at the apex: face the player, animation 8 (the throw).
4. Sub 3: they **hangs at the apex** (no move). At the first flag of animation 8 (update 21): sound
   34, 35 or 36 (random; a voice [guess]), sound 93, and the **anchor** (visual 22 `var` 0, below).
   At the second flag (update 31): animation 6.
5. Sub 4: they fall. On the floor: sound 92, animation 7. Sub 5: at its end (13 updates): THINK.

The save state agrees: the boss is in sub 3, animation 8, gravity 0.355 px/update² (a jump of 71 px
in 20 updates); the anchor is 6 updates out at 4.0 px/update.

### SLAM (step 6, sub-steps `0x800F9644`) [certain]

1. Sub 0: animation 10, face the player.
2. Sub 1: at its end (10 updates): the target is the **player's x, 80 px above the player**
   (x limited to room x 40–312, y to room y 48 or lower). Sound 91.
3. Sub 2: they fly straight to the target at **8 px/update** (facing their move), until they are within
   8 px on both axes, touches the ceiling or meets a wall in front. Then animation 11.
4. Sub 3: they **hangs for 30 updates**. Then sound 37, **contact damage 8**, they **drops straight down
   at 5 px/update** (no gravity).
5. Sub 4: on the floor: sound 94, `Rumble(5)`, `Quake2(50, 3, 2)`, animation 12, 4 debris.
6. Sub 5: at the end of animation 12: contact 4, animation 2. Sub 6: **50 updates**, then THINK.

So the player has the 30-update hang to leave the spot under them. A slam from the top of the room
falls about 100 px (20 updates).

### BURROW (step 7, sub-steps `0x800F9660`; high HP only) [certain]

1. Sub 0, 1: sound 91, animation 15, face the player; at its end: target = the player's x, 112 px
   above the player (same limits as SLAM).
2. Sub 2: they fly to it at 8 px/update. Then animation 16, they drop at 5 px/update.
3. Sub 3: on the floor: sound 94, `Rumble(5)`, animation 32 (they drill in).
4. Sub 4: debris every 4 updates. At the end of animation 32 (16 updates): animation 33 (the fin),
   y + 32, flat terrain box, `display` = 0 (no boxes), `+0x8D` = 1 (buried), a total timer of
   **180 updates** and a delay of **90 updates**.
5. Sub 5: hidden, they wait out the delay. Then they show the fin (`display` = 1), face the player
   and move at **5 px/update** toward the player (sub 6). In sub 6, when they pass the player's x
   or meet a wall, they go back to sub 5 with a 4-update delay: they turn and come back. Sound 95
   at timer 87 and 27; debris every 4 updates.
6. When the 180 updates end (in sub 5 or 6): sound 94, `Rumble(5)`, they **comes up**: animation 33
   from frame 4 (sprite 14, then 27), y − 32, `+0x8D` = 0, **4 px/update up** until 72 px above the
   floor point, 4 debris. Then animation 4, gravity 0.064 px/update² (4189), they fall; on the floor:
   sound 92, animation 7; then animation 2 and **20 updates**, then THINK.

The fin box is 16 × 16 px, 1–17 px above the floor: jump over it. Contact 4 also on the way up.
While they are buried (`+0x8D` = 1), **weapon id 35** knocks them out (POP-UP). The stun ids do not stun
them in this step.

### RESURRECTION (step 8, sub-steps `0x800F968C`; low HP only) [certain; confirmed in `shark-9_boss-revive`]

1. Sub 0: sound 91, animation 3; they face the room middle.
2. Sub 1: at the end of animation 3: animation 4; target = the **far upper corner**: room (48, 80)
   when they face left, (304, 80) when they face right.
3. Sub 2: they fly there at **6 px/update** (debris every 4 updates). Within 6 px: sound 38,
   animation 17, they face the room middle.
4. Sub 3: at flag 2 of animation 17 (after 40 updates): sound 96. At flag 1 (after 44 updates):
   the **side kick**:
   - on Hard (difficulty ≥ 2), with a chance of 1/2 (`GetRNG() & 1`): sound 41, `var` 3;
   - else, when the player is X (`mega+0x02` = 0): sound 40, `var` 2;
   - else (Zero): sound 39, `var` 1.

   `act4` = 1.
5. Sub 4: they hang in the corner until the side kick is deleted (it clears their `act4`). Then
   `StopSound(2, 20)`, animation 4, gravity 0.064 px/update².
6. Sub 5: they fall; on the floor: sound 92, animation 7. Sub 6: at its end: THINK.

They can be hit all the time in the corner (normal table; the stun ids give only 120 updates of
invulnerability). The save state agrees: the boss is in sub 4 at room (302, 81) with `act4` = 1;
the side kick `var` 2 (the player is X) is at room (80, 16), facing right.

### STUNNED (step 3, sub-steps `0x800F96D4`) [certain; confirmed in `shark-8_boss-stun`]

1. Sub 0: `StopSound(2, 20)`, sounds 33 and 119, the **second sprite set** (texture `0x540`, the
   next bank), animation 37 (electric shock, loops), contact 4. When they were a fin (`+0x8E` = 1): they
   comes out of the floor (y − 28, the normal terrain box). A 90-update timer.
2. Sub 1: they **freezes in place for 90 updates** (no move; in the air too). Then speeds 0, gravity
   0.25 px/update² (`0x4000`).
3. Sub 2: they fall. On the floor: sounds 20 and 92, first sprite set, animation 2. 50 updates.
4. Sub 3: after **50 updates**: invulnerability 40, THINK (a new step).

All the time from the hit to 40 updates after THINK they are invulnerable (deflect). The stun does not
end their anchor or a side kick (they test only their state, not their step). The stun state agrees: 47
updates after the hit (`+0x88` = 9953), sub 1 with 45 updates left, animation 37, set 1 (texture
`0x540`), speeds and gravity 0. Their `+0x84`/`+0x86` hold a SLAM target, so the hit came in the slam:
they froze in the air.

### POP-UP (step 9, sub-steps `0x800F96E4`) [certain]

Weapon id 35 while they are buried in BURROW. Animation 28, contact 4, y − 32, y speed 5.33 px/update
up, gravity 0.178 px/update² (`0x500000 / 450`), x speed 1.07 px/update **away from the player**.
At the top: the terrain box. On the floor: sound 92, **50 updates**, invulnerability 40, THINK.

### Death (state 2, steps in `0x800F9608`) [certain]

1. `StopSound(2, 20)`, sound 32, `PlayerEvent(20, <player facing>)` (`mega+0xCA` = 0), animation 28,
   flash off. The kill set `mega+0x7A` = 1.
2. **127 updates**; the flash toggles at shorter intervals (26, 21, 16, 11, then 6 updates). Then
   engine object 26 (stage-logic class; probably the boss explosion [guess]).
3. While object 26 runs they flash every 6 updates; while its `+0x07` is not 0 they are not drawn. When
   it is gone: `game+0x24` = 0, `game+0x20` = 0;
   - in stage 12: `game.exit_req` = `0x80`, `game[0x2C + checkpoint]` = 1, checkpoint + 8;
   - else a **Nightmare Virus** (main type 15) with `var` **`0x10` (200 souls) when the fight took
     less than 5,401 updates (90 s)**, else `var` `0x11` (100 souls)
     (`docs/enemies/nightmare-virus.md`, BOSS SOUL).

   Then they are deleted. Their side kick and all visual 22 objects end when they leave state 1.

## The side kicks (main 56 `var` 1–3, state 3, steps `0x800F96F4`) [certain for the code]

Each side kick is an old Maverick made of junk. It comes together from 9 pieces, attacks a fixed
number of times, blinks for 30 updates and is deleted. The attack is the "Helper Attack". The names
are from the Fandom page "Metal Shark Player" (read as wikitext through the MediaWiki API): with X
he calls Magna Centipede, with Zero Sting Chameleon, and in Xtreme mode he may call Blast Hornet
[Fandom; it agrees with the code: `var` 2 for X, `var` 1 for Zero, `var` 3 only at difficulty ≥ 2
with a chance of 1/2]. The Atrocious Gameplay wiki page of the stage gives the same three [second
source].

| `var` | Maverick (source) | sprites | looks like | attack |
|---|---|---|---|---|
| 1 | **Sting Chameleon** (X1; Japanese "Sting Chameleao") [Fandom] | 53, 54, 118, 119 | a chameleon-like Maverick with a curled tail, on the back wall | a long diagonal tongue (sprites 55–59, 105–114): Iron Tongue [Fandom] |
| 2 | **Magna Centipede** (X2; Japanese "Magne Hyakulegger") [Fandom] | 44–46, 115–117 | a Maverick that hangs from the ceiling | 3 shots (sprites 47–52): his shuriken [Fandom: "stands on the ceiling and attacks with Shuriken"] |
| 3 | **Blast Hornet** (X3; Japanese "Explose Horneck") [Fandom] | 61–72, 120–131 | a hornet with wings and a stinger | homing bees (sprites 73–77, 132–136): Mini Bee Summon [Fandom] |

The earlier guess "Spark Mandrill" for `var` 2 was wrong.

Common rules:

- **Main** (`0x800F2EE4`): when the boss is not in state 1, state 4 (delete). Else `AnimeTick`, the
  step, `ContactWeapon` and `ContactMega`. **24 HP**, `dmg_tbl_shark`, contact 4. A hit: 30 updates
  of invulnerability (2 for ids 24, 25) with the flash. A kill: an explosion and state 4. The stun
  ids have no special effect on it.
- **Delete** (`0x800F4454`): clears the boss's `act4`, so a killed side kick ends the boss's step 8
  at once.
- **Forming** (sub 1): 9 pieces (visual 22 `var` 32) start 512 px from the side kick's point, at
  the angles 2560, 0, 1536, 2760, 200, 1736, 2960, 400, 1936 (+ 2048), and fly in at **8 px/update**
  (about 63 updates). `0x800F9868` counts them. When all are in: sound 97, `display` = 1, the second
  sprite set and animation 34, 35 or 36 (its silhouette, 34 updates); then the first sprite set.
- It has boxes only while it attacks. The pieces have no boxes.
- **Repeat count** (`+0x7E`) = {2, 3, 4, 4}[boss level] for `var` 1 and 2 (table `0x800E9A70`);
  {120, 160, 200, 240}[boss level] updates of fire for `var` 3 (`0x800E9A74`).

### `var` 1: the beam (Zero) (sub-steps `0x800F9708`)

1. Each time: it forms **112 px from the player**, on the side away from the room middle, at room
   y 128, facing the player (x limited to room 40–312).
2. At the end of animation 35: animation 21; boxes (−16, −24, 32, 60) (32 × 60 px); it faces the
   player; **50 updates**.
3. Sound 98, animation 22 from frame 1, and the **beam** (visual 22 `var` 48, below).
4. When the beam ends: no boxes, 30 updates of blinking. The count − 1. At 0: deleted; else it
   forms again near the player (step 1).

At level 0: 2 beams, each about 63 + 34 + 50 + 124 + 30 ≈ 300 updates.

### `var` 2: the ceiling sparks (X) (sub-steps `0x800F972C`)

1. It forms at the **top of the room**, room (80, 16) facing right when it starts in the right half
   (it starts at the boss's x), else (272, 16) facing left.
2. At the end of animation 34: animation 18. Then animation 19 and boxes (−24, −16, 48, 48)
   (48 × 48 px).
3. At flag 1 of animation 19 (update 6): sound 99, **3 sparks** (visual 22 `var` 64) from (x ± 16,
   y + 52), aimed at the player and at **±40°** (±455) from that line. 60 updates.
4. The count − 1. Not 0: back to 2 (animation 19, fire). At 0: animation 18, no boxes, 30 updates of
   blinking, deleted.

At level 0: 2 volleys, 60 updates apart; the side kick stays about 63 + 34 + 130 + 30 ≈ 260 updates.

### `var` 3: the homing bees (Hard) (sub-steps `0x800F974C`)

1. It forms at room (64, 64) facing right when it starts in the right half, else (288, 64) facing
   left.
2. At the end of animation 36: animation 25. Then animation 26 and boxes (−16, −24, 32, 72)
   (32 × 72 px).
3. At flag 1 of animation 26 (update 10): sound 100 and a **fan of n bees** (visual 22 `var`
   80–83), n = {3, 5, 6, 10}[boss level] (`0x800E9A7C`), from (x ∓ 36, y − 12). The fan starts just
   in front of straight down (angle 3042 facing left, 3102 facing right) and opens 964 units (85°)
   to the front. `0x800F986A` counts the live bees.
4. It fires again (back to 2) when a third of the last fan (rounded up) has ended. It stops when
   its fire time (120 updates at level 0, counted in 3 and 4) ends: no boxes, 30 updates of
   blinking, deleted. Bees that are still out live on (they stop counting).

## Shots and effects (visual object type 22, entry `0x800F6FA0`)

All copy the parent's graphics ("copies the parent", `docs/engine.md`) and keep the boss in `+0x7C`.
The set-up (`0x800F35A8`) and the main routine (`0x800F37DC`) delete the object when the boss is
not in state 1. All have **1 HP and `dmg_tbl_deflect`** (they cannot be destroyed), except the bees.
The `var` high nibble picks the kind:

| `var` | object | boxes, damage | movement and end |
|---|---|---|---|
| 0 | **anchor** (steps `0x800F97D4`) | attack = hurt (−12, −12, 24, 24): 24 × 24 px; terrain (0, 0, 8, 8); contact **4**; priority 4; animation 9 (spins) | Starts 32 px in front of them. Flies **4 px/update straight at the player** until it touches terrain. Stays **50 updates** there (the spin stops). Then it faces the player and **bounces**: x speed **1.28 px/update** toward the player, y speed **6.4 px/update up**, gravity **0.256 px/update²** (16777): each floor bounce is 80 px high and 50 updates long. The ceiling sends it down at 3.2 px/update; a wall turns it. It explodes at the 10th terrain contact, or on the player. Every shot is deflected. |
| 16–21 | debris (steps `0x800F97CC`) | none; no damage | Animation 29 or 30 (`var` ≥ 19) or 29–31, random facing. A jump of 50–109 px, apex in 15–24 updates, 20–59 px to the side: random (16, 19), left (17, 20) or right (18, 21). Deleted 48 px off screen. |
| 32 | forming piece (steps `0x800F97E4`) | none | Animation 29–31. Flies at 8 px/update to its point; deleted within 8 px (`0x800F9868` − 1). |
| 48 | beam (steps `0x800F97EC`) | none itself; priority 4 | At (x − 144, y + 45) of the side kick (mirrored when it faces right). Animation 23 (32 updates) draws the beam out; at its flags 1–7 it makes the segments `var` 49–55. At flag 16: **60 updates**, then animation 24 (32 updates) draws it back. Then it is deleted and clears the side kick's `act4`. |
| 49–55 | beam segment (step `0x800F75D8`) | attack (−16, −8, 32, 16): 32 × 16 px, contact **4**; not drawn; no hurt box | Segment n at (x + 112 − 32 (n − 49), y − 56 + 16 (n − 49)) of the beam (facing left): a line down and forward, 16 px down per 32 px. A segment ends when the beam's flag is below n − 48 (so they go in reverse order) or the side kick ends. |
| 64 | spark (steps `0x800F97F8`) | attack (−6, −6, 12, 12): 12 × 12 px, terrain (0, 0, 6, 6); contact **4**; animation 20 | Flies **6 px/update** at its angle. On terrain it stops (the animation stops) for **60 updates**, then blinks 30 updates and is deleted. Ends (explosion) on the player; deleted 48 px off screen. No hurt box. |
| 80–83 | bee (steps `0x800F9808`) | attack = hurt (−4, −4, 8, 8): 8 × 8 px; terrain (0, 0, 4, 4); **1 HP, `dmg_tbl_minor`**; contact **4**; animation 27 | Flies **3 px/update** at its fan angle for 32 updates, then stops for **40 updates** (a random loop of animation 27). Then for **180 updates** it **homes**: each update it moves straight at the player at **0.5, 1, 1.5 or 2 px/update** (`var` 80–83, random). Then it **explodes**: not drawn, attack box (−16, −16, 32, 32), contact 4, explosions every 8 updates for **34 updates**, deleted. One weapon hit destroys it. |

The debris is harmless. The beam segments and the sparks cannot be shot.

## Animations (table `0x800F952C`, 40 animations) [certain for the frames]

The kit lists 41; number 40 is the state table `0x800F95CC`, not an animation. Animations 34–37 use
the **second sprite set** (`0x800F4144`: the next bank's sprite data and palette, texture page
`0x540`); all others use the first (`0x800F6E78`: their bank, texture `0x140`).

| # | what | used by |
|---|---|---|
| 0 | one frame | intro rise |
| 1 | spin and pose at the top (71 updates; flags 2, 1) | intro |
| 2 | stand (loops) | after steps |
| 3 | crouch (13 updates) | ANCHOR, RESURRECTION start |
| 4, 5 | jump poses | ANCHOR, BURROW, RESURRECTION |
| 6 | fall (loops) | intro, ANCHOR |
| 7 | landing (13 updates) | most steps |
| 8 | anchor throw (flag 1 at update 21, flag 2 at 31) | ANCHOR |
| 9 | anchor spin (16 frames of 2 updates, loops) | `var` 0 |
| 10, 11, 12 | SLAM: crouch, hang, landing | SLAM |
| 13, 14 | FIN jump, falling | FIN |
| 15, 16 | BURROW crouch, drop | BURROW |
| 17 | summon pose (flags 2, 1; loops from frame 3) | RESURRECTION |
| 18, 19 | side kick `var` 2: idle, fire (flag 1) | `var` 2 |
| 20 | spark (loops) | `var` 64 |
| 21, 22 | side kick `var` 1: idle, fire | `var` 1 |
| 23, 24 | beam out (flags 1–7, 16), beam back (flags 7–0, 16) | `var` 48 |
| 25, 26 | side kick `var` 3: idle, fire (flag 1) | `var` 3 |
| 27 | bee (10 frames: frames 0–1 loop, 2–9 a second loop) | `var` 80–83 |
| 28 | knocked (loops) | POP-UP, death |
| 29, 30, 31 | debris pieces | `var` 16–32 |
| 32 | drill into the floor (16 updates, flag 1) | BURROW |
| 33 | fin (loops; frames 4–5: come up) | BURROW |
| 34, 35, 36 | side kick silhouettes `var` 2, 1, 3 (34 updates; second set) | forming |
| 37 | electric shock (loops; second set) | STUNNED |
| 38 | dive into the floor, fin (flag 1, loops from frame 2) | FIN |
| 39 | jump out of the floor, then stand (loops) | FIN end |

The export (`spriteexport --state shark-7_boss.state.gz`, `enemy-3f9602d9d5`, bank 14, from the
live boss; folder `metal-shark-player`) takes the second set from the `sets` rule of
`catalog/enemies.json` (stage 4, type 56: animations 34–37, texture `0x540`, palette of bank + 1).
So:

- **clean**: strips 0–39; strips 34–37 come from entry 15 (`shark-7_boss` and
  `shark-9_boss-revive` both draw them correctly); `anim-40.png` is not an animation;
- **cut**: the strips stop at the first loop, so animation 27 shows only frames 0–1 (frames 2–9
  are the bees that the code starts with `SetAnimeFrame(27, 2 + 2 × (GetRNG() & 3))`) and
  animation 33 does not show frames 4–5.

The second set is not in the texture table (bank 15 has no texture). `spriteexport` draws it only
as a live sprite source: `banks/shark/shark-9_boss-revive/bank-14-live-1.png` (13 sprites:
sprites 0–8 the three silhouettes, 9–12 the shocked boss) and
`banks/shark/shark-8_boss-stun/bank-14-live-0.png` (the same 13 sprites with the boss's flash
palette, 3 of them with no palette). The side kicks' own animations (18–27) are in the first set
and are clean in the export.

## Damage

`0x8007555C` (`dmg_tbl_shark`), for the boss and the side kicks; not yet in `docs/engine.md`
(the kit names it `dmg_tbl_high_max_ball+0xa0`). It is the table just before Rainy Turtloid's
(`0x800755FC`):

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 D 2 5 – 2 1 2 2 2 D 2 8 |
| 16–31 | 2 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 4 2 2 2 2 2 2 6 2 D 1 4 8 |
| 48–63 | 2 1 2 2 1 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

The ids that matter:

| ids | damage |
|---|---|
| 0 (lemon), 9 (half charge) | 1 |
| 18, 19, 20 (full charge) | 4 |
| 24, 25 (sabre) | 2 (+1 sabre bonus: 3) |
| 26 | 5 |
| 27–31 (sabre combo) | 2 (+1: 3) |
| **6 / 15 / 42 (the stun ids)** | **5 / 8 / 6** |
| 35 (pops them out of the floor in BURROW) | 4 |
| 17, 22, 47, 57 | 8 |
| 60, 62 | 15 |
| 4, 13, 21, 23, 44, 64–78 | deflected |
| 7 | no effect |

With 48 HP (before the part bonuses and the Easy × 1.25): 48 lemons, or 12 Falcon full charges
(id 19). A side kick (24 HP) takes 24 lemons or 6 full charges.

**The stun ids 6, 15 and 42** follow the boss pattern of `docs/engine.md` (n, n + 9 and one id of
33–44). The stun state shows which weapon they are: X's selected weapon `mega+0x93` = 6, the
weapon button id `+0x94` = 6, the last shot `+0x97` = 6, and the energy of slot 6 went down. The
player code (`0x8003DF58`) sets `+0x94` = the slot, or the slot + 9 when the weapon is charged. So
**id 6 is the uncharged and id 15 the charged weapon of slot 6** [certain for the code and the
state]. The user names it Rainy Turtloid's weapon [user; the owned bits `mega+0xC9` = `0x21`
agree: slots 1 and 6, the stages 1 (Yammark) and 6 (Turtloid)]. Id 42 is not traced: its weapon
code is the same function as id 35 (`0x801ED2A8`, X's overlay).

Note: because a stun makes them invulnerable until 40 updates after the stun step, the stun weapon
hits only once per stun (about 190 updates on the floor). In BURROW and RESURRECTION it is a normal
hit with 120 updates of invulnerability.

Dealt (before the difficulty scaling): body 4 (SLAM drop 8); anchor 4; beam segment 4; spark 4;
bee 4 (explosion 4); side kick body 4. Debris and pieces do no damage.

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 94 | intro rumble (every 16 updates); SLAM and BURROW impacts; BURROW come-up |
| 93 | intro pose (anim 1 flag 2); the anchor throw |
| 92 | landings |
| 91 | start of ANCHOR, SLAM, BURROW, RESURRECTION |
| 95 | FIN dive and every 50 updates of the slide; BURROW fin (timer 87, 27) |
| 34, 35, 36 | one of them at the anchor throw (random) |
| 37 | SLAM drop |
| 38, 96 | RESURRECTION: corner reached; summon pose |
| 39, 40, 41 | the side kick `var` 1, 2, 3 is called |
| 97 | a side kick is formed |
| 98, 99, 100 | side kick attacks: beam, sparks, bees |
| 33, 119 | STUNNED |
| 20, 92 | STUNNED: on the floor |
| 32 | death |
| bank 0, 14 | HP bar tick |

`StopSound(2, 20)` comes before the stun, the death and the end of RESURRECTION; id 20 is probably a
looping sound (the shock?) [guess].

## Summary for a reimplementation

- A 32 × 56 px floor boss, contact 4, HP 48 (52 / 56 at boss levels 2 and 3), 30 updates of
  invulnerability after a hit. They can be hit whenever they are drawn.
- High HP patterns: FIN (70 px hop, then a 4 px/update fin that stops past the player), ANCHOR (hop
  60–79 px, hang, throw an anchor at 4 px/update that sticks 50 updates and then bounces toward the
  player 10 times), SLAM (fly at 8 px/update to 80 px above the player, hang 30, drop at 5
  px/update with contact 8), BURROW (drop in, hide 90 updates, a 5 px/update fin that turns under
  the player, come up at 4 px/update after 180 updates).
- Below half HP: no BURROW; RESURRECTION ends two of three patterns: they fly to the far upper
  corner and calls a junk Maverick (24 HP; Zero: a diagonal beam twice; X: 3-way sparks from the
  ceiling twice; Hard 50 %: a fan of 3 homing bees for 120 updates). They hang until it is gone.
- Weapon ids 6 / 15 (slot 6, Rainy Turtloid's weapon) and 42 stun them (not in BURROW or
  RESURRECTION): 90 updates frozen, fall, 50 updates, all of it invulnerable.
- Death: they hold the player, flash 127 updates, explode, and leave a boss soul: 200 souls
  under 90 s, else 100.

## Open questions

1. (answered: Sting Chameleon, Magna Centipede, Blast Hornet; "The side kicks") A play check of
   each call is still useful (`var` 1 needs Zero, `var` 3 Xtreme).
2. (answered from the code) **Ids 35 and 42 are Zero's techniques, not X's weapons.**
   `weapon_types` (`0x80071220`) is one static table for both characters. Its entry `0x801ED2A8`
   (ids 24–31, 35–42, 48) is a function start only in Zero's overlay 1 (`turtloid-6_zero-player`);
   in X's overlay 0 that address is inside another function (`shark-8_boss-stun`). So only Zero
   makes these ids; in X's overlay no code makes them (the shot ids of `ShotIds` `0x8003DF58` are
   0–17, 43, 45, 46, 50) [certain for the code]. In Zero's overlay, `0x801ECD90(player, id)` makes a
   weapon object of that id, and `0x801ECD54` calls it when the bit `0x80` of `+0x45` is set:
   - **id 35**: `0x801ECED4`, sub-step 0 of Zero's step 56 (sub-step table `0x801EE3C4`): skill bit
     `0x08`, down held and attack in the air (`docs/players/steps-extra.md`). Skill bit `0x08` is the
     slot 4 technique, from this stage: **Rakukojin** ("falling steel blade", a downward sabre
     dive) [likely for the name: Fandom gives Rakukojin as Metal Shark Player's technique; the slot
     follows the bit order]. So a Rakukojin onto the buried boss knocks them out of the floor.
   - **id 42**: `0x801EA60C` (the try `0x801EA560`) and `0x801EC5E0` (step 58), both with animation
     103: skill bit `0x20`, down held and the special button. Skill bit `0x20` is the slot 6
     technique, from Rainy Turtloid: **Ensuizan** [likely for the name: Fandom gives Rainy Turtloid's
     technique as Ensuizan and Metal Shark Player's weakness as "Meteor Rain / Ensuizan", which are
     exactly ids 6 / 15 (slot 6) and 42].

   The same pattern fits the other bosses: Rainy Turtloid's id 33 (slot 2: Hyoroga; Nightmare
   Freeze also takes id 33, `docs/enemies/nightmare-freeze.md`), Shield Sheldon's id 35 (slot 4:
   Rakukojin; Fandom: "Metal Anchor / Rakukojin"), Commander Yammark's id 44 (slot 8: Rekkoha, Zero's
   skill `0x80` special attack; Fandom: "Ray Arrow / Rekkoha"). These ids are the Zero column of the
   stun rule in `docs/engine.md`. Proposed for `docs/engine.md` and `docs/players/attacks.md` (other
   owners).
3. Engine objects 24 and 26 and player event step 29, as for the other bosses.
4. Sound 20 (stopped at the stun, the death and the end of RESURRECTION).
5. Re-export animations 34–37 from the next bank.
6. The floor under the stun state's x: the boss is at y 375 there, 16 px lower than the standing
   origin on X's floor (y 359). Whether the floor is lower at that x or they were still falling was
   not checked.
