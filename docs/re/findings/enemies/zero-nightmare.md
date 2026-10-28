# Zero Nightmare (boss overlay 43, main object type 53): behaviour sheet

Zero Nightmare is the boss of the alternate Turtloid section. He is a copy of Zero with a purple
palette. He stays on the floor for all of the fight: he never jumps and has no gravity. He fights with
a ground dash, buster shots, saber slashes that throw waves, a ground punch that makes pillars rise
from the floor, a large crescent wave, and a **warp**: he blinks, vanishes and comes back on the
player's position or on the far side of the room. The attack chain depends on the distance and the
height of the player, on his HP (above or below half), on Hard, and on the boss level.

Boss overlay `43` (`rock_bin/43.bin`, loaded at `0x800FA000` by `BossSpawner`; `docs/engine.md`,
"Boss overlay"), save state `turtloid-5_zero.state.gz` (stage 21, area 1; X and the boss in the boss
room, the boss in his first dash). Written with the RE kit (`python3 -m mmxdemake.rekit --name
zero-nightmare --state turtloid-5_zero.state.gz --type 53`) and `docs/re-guide.md`. Units,
confidence tags and the shared engine rules: `docs/engine.md`. The room: `docs/stages/turtloid.md`,
"The boss room behind gate 45". Shots and effects are visual objects of type 19 (`var` 0–9). "The
room" below has its origin at the camera's target minimum x and y at the set-up (`camera_layer0`
`+0x26`, `+0x2A`; x 1264, y 704 in the save state). The X6 Recomp reference calls him "Nightmare
Zero".

## Spawn and set-up (`0x800FA054`) [certain]

- **HP: 48 + a level bonus.** `boss_hp_max` (`game+0x8C`) = 48 + {0, 2, 4, 8}[boss level] (table
  `0x800FA004`; level from `BossLevel`, `game+0x8F`): **48, 50, 52, 56**. The save state has level 0
  (48 HP). The HP starts at 0 and fills in the intro. The X6 Recomp reference gives the same four
  values for levels 1–4.
- **Contact damage 10**, the same in all poses.
- Boxes (body):

  | when | attack box | hurt box |
  |---|---|---|
  | most frames | (−20, −14, 32, 38): 32 × 38 px | the same |
  | slash frames (sprites 31–33) | (−80, −30, 60, 54): 60 × 54 px, in front of him only | (−26, −10, 28, 34) |
  | slash recovery (sprites 34–39) | (−26, −10, 28, 34) | (−26, −10, 28, 34) |
  | punch frames (sprites 101–109) | (−30, −10, 24, 34) | the same |

  The main routine picks the box from the current sprite (`+0x47`) on every update. Terrain box
  (0, 0, 24, 24): a 48 × 48 px body, so his origin is 24 px above the floor.
- Damage table `0x800750FC` (named `dmg_tbl_zero_nightmare` here; below). Draw priority 5. Animation
  table `0x800FE070`. Own sprite source (texture `0x140`, palette `0x7948`; `docs/engine.md`).
- Position: room (224, 160). Facing left. Speed, acceleration and gravity 0.
- `game+0x20` = this object, `game+0x25` = 0. `PlayerEvent(20, 64)`: the player is held.
- Room points: x 16 and 304 (the limits; their midpoint, x 160, splits the room in two halves), x 224
  (the intro point), y 192 (the floor), y −32 (the top limit of the pillars).
- Three effect objects (type 6, `var` 0, 1, 2, palette `0x794C`, priority 6), each linked to the one
  before (the first to the boss): the **after-images** [likely: they are drawn while `+0x9A` = 1,
  that is during the dash, step 5].
- Then state 1, step 2 (INTRO).

## The `id` parameter

He has no record: `BossSpawner` makes him. The code does not read `var` [certain].

## State machine

States (table `0x800FE134`): 0 set-up, 1 main (`0x800FA2F4`), 2 death (`0x800FD3EC`). Every update
the entry clears `display`, runs the state and then `CollideCheck`.

### Main (state 1) [certain]

1. Set the boxes from the current sprite (table above).
2. If `+0x8A` = 0: `ContactWeapon`. A kill: state 2 (death). `+0x8A` ≠ 0 means **intangible**: no
   weapon test and no contact test (the warps set it).
3. If `+0x8A` = 0: `ContactMega`.
4. A hit (result > 0, not 127): **30 updates of invulnerability** (`+0x84`; 2 updates after weapon
   ids 24 and 25, the first sabre hits): `dmg_tbl_deflect`, and the palette flash toggles every 2
   updates. At the end: his own table again.
5. A hit by **weapon id 50** (result 51), when the step is not 9 or 11: step 12 (STUN WARP)
   [certain for the id]. Id 50 is **X's saber slash on the floor** (51 is the slash in the air or
   on a wall). Its routine (`0x801EDCB0`) is a function only in X's overlay, so Zero cannot make
   it. X makes slash objects 50 / 51 in the saber poses (steps 45–47: the weapon button with the
   buster selected, `docs/players/attacks.md`, and the slot 3 weapon, Magma Blade,
   `docs/players/weapons.md`), in the Shadow Armor charged pose (step 38) and in the Shadow and
   Blade special attacks (steps 37, 77; `docs/players/steps-extra.md`) [certain for the code; the
   names are likely].
6. `+0x9A` = 1 while the step is 5 (dash), else 0 (the after-images use it).
7. The step (table `0x800FE140`): 0 engine hit reaction, 1 nothing, 2 INTRO, 3 idle (not used),
   4 THINK, 5–11 attacks, 12 STUN WARP.

Each step moves him with `MoveObject2` (no gravity) and runs `AnimeTick`; most also run
`DisplayCheck(64, 64)`. "Blink" below means `DisplayCheck` runs only on even updates of the step's
counter, so he is drawn every other update.

### INTRO (step 2, sub-steps `0x800FE174`) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800FC1CC` | Not drawn. Waits until the player is in step 20 (held). Then camera limit speed 2, `PlayerEvent(29, 64)`, a 40-update timer, engine object 24 (probably "WARNING" [guess]) and the dash jet (visual `var` 0). |
| 1 | `0x800FC28C` | Not drawn. Waits while `game.exit_req` ≠ 0. At the end of the timer: `PlayerEvent(20, 64)`. When object 24 is gone: animation 2, sounds 153 and 149, he falls at **4 px/frame** to the floor. |
| 2 | `0x800FA52C` | Warp-in: for 16 updates his x jitters around x 224 (the warp table, below). Then animation 0 at x 224, and he blinks until update 110. Then his text (`TextOpen`, `game+0x2E` = 1). |
| 3 | `0x800FC340` | Waits until the text is closed. `BossMusicRequest`, `game+0x24` = 1. |
| 4 | `0x800FC3B0` | Waits while `BossMusicWait` (the boss music wait). Then **+1 HP per update**, a tick (bank 0, id 14) every 3 updates, up to `boss_hp_max`. Then `PlayerEventEnd` and THINK. |

### THINK (step 4, `0x800FA650`) [certain]

THINK has no wait: it picks a pattern in the same update. `r` = `GetRNG() & 15`. "High HP" means
HP > `boss_hp_max` / 2 (24 at level 0). "Hard" means difficulty 2 (the code compares with 2, so
difficulty 3 uses the Normal choices).

| where the player is | high HP | low HP |
|---|---|---|
| near: \|dx\| ≤ 64 px | 7 | 8 |
| far, same height: \|dy\| < 48 px | r < 6: 0; r < 12: 1; else 4 (Hard: 5) | r < 4: 2; r < 8: 3; r < 12: 4 (Hard: 5); else 6 |
| far, other height: \|dy\| ≥ 48 px | 9 (Hard: 10) | r < 8: 9 (Hard: 10); else 11 |

A pattern is a list of steps (`0x800FE0B4`, 6 bytes each). Each step ends by going to the next entry;
the last entry is THINK.

| pattern | steps |
|---|---|
| 0 | BUSTER, BUSTER, DASH |
| 1 | BUSTER, BUSTER, WARP TO PLAYER, SLASH |
| 2 | BUSTER, RAPID FIRE, WARP TO PLAYER, SLASH |
| 3 | BUSTER, RAPID FIRE, DASH, SLASH |
| 4, 9 | DASH, SLASH |
| 5, 10 | DASH, CRESCENT (Hard) |
| 6 | WARP TO PLAYER, GROUND PUNCH |
| 7 | SLASH, DASH |
| 8 | GROUND PUNCH |
| 11 | DASH, GROUND PUNCH |

So: RAPID FIRE and GROUND PUNCH come only at low HP. CRESCENT comes from THINK only on Hard (it also
comes after a STUN WARP on every difficulty). The player's character is not tested.

### The warp (shared by steps 6, 11 and 12) [certain]

The counter starts at 0 (step 12: at −40). He is intangible (`+0x8A` = 1) for the whole warp.

| counter | what |
|---|---|
| < 50 | He stands and blinks in place. |
| 50 | Animation 2, sound 153. |
| 50–65 | Warp-out: x = his x + the table value at index 65 − counter, so he flickers from side to side, farther each update (±24 up to ±80 px). |
| 66–125 | Not drawn, no move (60 updates). |
| 126 | Sound 153. The new x (below). |
| 126–141 | Warp-in: x = new x + the table value at index counter − 126 (±80 down to ±24 px). |
| 142–191 | Animation 0 at the new x; he blinks. |
| 192 | `+0x8A` = 0 (he can be hit again). The next step. |

Warp table `0x800FE114`: −80, 80, −72, 72, −64, 64, −56, 56, −48, 48, −40, 40, −32, 32, −24, 24.
The y does not change. The whole warp is **192 updates** (step 12: 232).

### DASH (step 5, `0x800FC508`) [certain]

1. Animation 1 (dash), face the player, speed 0.
2. At the animation's flag 1 (update 1): face the player, **x speed 4.125 px/frame** (`0x42000`),
   sounds 133 and 151, the engine dust puff `0x8003C6A4`. The dash jet (visual `var` 0) shows at flag
   9 (update 14). At flag 2 (update 24): sub 2.
3. Sub 2: after **11 more updates**, he stops (sound 134, speed 0) as soon as the player is less than
   64 px in front of him, or behind him. So the dash is at least about 34 updates (about 140 px), even
   when the player is near. The code has no wall test; `CollideCheck` keeps him in the room
   [likely].
4. At the end of animation 1 (update 50): animation 0, the next step.

Contact damage 10 during the whole dash. The after-images run while `+0x9A` = 1 [likely].

### WARP TO PLAYER (step 6, `0x800FC800`) [certain]

Animation 0, face the player, then the warp. **The new x is the player's x** at counter 126, and he
faces the player. He is solid again (contact 10) at counter 192, on that spot, unless the player
moved.

### BUSTER (step 7, `0x800FC8BC`) [certain]

1. Animation 3 (buster pose), face the player.
2. At flag 1 (update 1): a **buster shot** (visual `var` 1) and a muzzle flash (`var` 3).
3. 40 updates later: animation 3 again, face the player, a second shot and flash.
4. 40 updates later: animation 4 (saber slash), face the player. At its flag 1 (7 updates in): sound
   135 and an **aimed saber wave** (`var` 4). The slash box is out for 6 updates after that
   (sprites 31–33).
5. At the end of animation 4 (28 updates): animation 0, the next step.

So one BUSTER step is two shots and one slash with a wave, about 110 updates.

### RAPID FIRE (step 8, `0x800FCC18`, low HP only) [certain]

1. Animation 3, face the player. At flag 1: the counter starts.
2. Every 6 updates (counter 0, 6, … 42) he faces the player and fires a **spread shot** (`var` 2)
   with a muzzle flash: **8 shots**.
3. At counter 48: the next step (animation 3 is already on its last frame).

### GROUND PUNCH (step 9, `0x800FCEE8`, low HP only) [certain]

1. Animation 5 (the fist goes up, then down to the floor), sound 152, face the player.
2. At flag 1 (update 48): sound 142, `Rumble(3)`, `Quake(16, 6, 3)`, and **8 ground pillars**
   (`var` 6, below).
3. At the end of animation 5 (update 76): animation 0, the next step.

### SLASH (step 10, `0x800FD0BC`) [certain]

1. Animation 6 (the slash with a long wind-up: one frame of 30 updates), face the player.
2. At flag 1 (update 35): sound 135. **Only when HP < half:** a **straight saber wave** (`var` 5).
   The slash box is out for 6 updates after that.
3. At the end of animation 6 (update 56): animation 0, the next step.

### CRESCENT (step 11, `0x800FD264`) [certain]

1. Animation 0, face the player, then the warp. **The new x is on the far side**: when the player's
   x is right of the room's midpoint, x 48 facing right; else x 272 facing left.
2. At counter 192: animation 7 (crescent slash), a 60-update timer, then sub 2.
3. Sub 2: at flag 1 of animation 7 (update 5) a spark (`var` 9); at flag 2 (update 17) a **big
   crescent wave** (`var` 8). At the end of the animation: animation 0.
4. When the timer ends, the slash count goes up by 1. When it is equal to {1, 2, 3, 4}[boss level]
   (table `0x800FA008`): the next step. Else animation 7 again, with a timer of 120 updates (before
   the third slash) or 60 updates (other slashes).

So at level 0 he throws **one crescent**; at level 3, four.

### STUN WARP (step 12, `0x800FD320`) [certain for the code]

- Entered on a hit by weapon id 50 (the 5 damage of the hit is applied first), except during GROUND
  PUNCH and CRESCENT.
- `0x800FE294` = 1 at once: **all his shots, waves, pillars and crescents end**.
- Sound 148, animation 8 (hurt), speed 0, intangible. `+0x84` is kept between 15 and 30, so he
  flashes for the whole step.
- The warp from counter −40: 90 updates of blinking in the hurt pose, then the warp-out, and he comes
  back on the **far side** (as CRESCENT).
- At counter 192: `0x800FE294` = 0, `+0x8A` = 0. Then at random (`GetRNG() & 1`): GROUND PUNCH, or
  the crescent slashes (CRESCENT from sub 2). After that the pattern goes on from where it was.

### Death (state 2, steps in `0x800FE1E8`) [certain]

1. Animation 8, sound 148, palette flash off, `PlayerEvent(20, <the player's facing>)` (the player
   is held). `0x800FE290` = 1 and `mega+0x7A` = 1 at the kill: all his shots and effects end.
2. **127 updates.** The palette flash toggles at shorter and shorter intervals (25, 20, 15, 10,
   then 5 updates). Then engine object 26 (probably the boss explosion [guess]).
3. While object 26 runs he flashes every 6 updates; while its `+0x07` is not 0 he is not drawn. When
   it is gone: `game.exit_req` = `0x10`; at boss level 3 also `game+0x5F` |= `0x20` [certain for the
   writes]. Then he is deleted.

He leaves **no Nightmare Virus soul** (Commander Yammark does).

**`game+0x5F` bit `0x20` is Black Zero** [certain for the code]. `game+0x5F` is `game.armors`
(`docs/enemies/light-capsule.md`). Three writers set the bit: the Light capsule `id` 8 when the
player is Zero (`0x80051F38`), the new game when the title code word `0x800E3134` is 4
(`0x8001E1A4`; the word next to it, `0x800E3130` = 4, gives the Ultimate Armor), and this death at
boss level 3. With the bit and the character Zero: damage taken is halved as for an armour
(`PlayerDamageScale`, `0x80030E78`), the hit class is 4 as with part `0x200` (`ContactMega`,
`0x8003114C`), the sabre ids get +1 damage as with the sabre part (`ApplyWeaponHit`,
`0x8003179C`), `CharacterIcon` gives 6, and the weapon energy cost is 4 × (`docs/players/steps-extra.md`).

**After the clear** (`StageExit`, `boss_choice` 0): Zero can be played (`game+0x5F` |= `0x10`),
then scene 2 and the report screen. The full path is in `docs/enemies/high-max.md`, "Where the
clear leads".

## Animations (table `0x800FE070`, 17 animations) [certain for the frames]

| # | what | used by |
|---|---|---|
| 0 | stand (idle, loops) | boss |
| 1 | ground dash; flags 1 (go), 9 (jet), 2 (sub 2) | DASH |
| 2 | warp pose (one frame) | warps, intro fall |
| 3 | buster pose; flag 1 (fire) | BUSTER, RAPID FIRE |
| 4 | saber slash; flag 1 (wave) | BUSTER |
| 5 | ground punch (the fist flashes white); flag 1 (impact) | GROUND PUNCH |
| 6 | saber slash with a 30-update wind-up; flag 1 | SLASH |
| 7 | crescent slash; flags 1 (spark), 2 (crescent) | CRESCENT |
| 8 | hurt (one frame) | STUN WARP, death |
| 9 | dash jet | `var` 0 |
| 10 | buster shot (spiral) | `var` 1, 2 |
| 11 | muzzle flash | `var` 3 |
| 12 | saber wave (small crescent) | `var` 4, 5 |
| 13 | pillar | `var` 6 |
| 14 | ground burst | `var` 7 |
| 15 | big crescent | `var` 8 |
| 16 | spark (loops from frame 3) | `var` 9 |

## Shots and effects (visual object type 19)

Each copies the boss's graphics (`docs/engine.md`, "copies the parent"). Before the state, every
object of this type runs `CornerCheck`. The `var` picks the state table (`0x800FA00C`). All of them
end when the boss dies (`0x800FE290`); all except `var` 0 also end at a STUN WARP (`0x800FE294`).
Positions are from the boss origin, "in front" = in his facing direction.

| `var` | object | boxes, HP, damage | movement and end |
|---|---|---|---|
| 0 | dash jet (`0x800FD664`) | none | 30 px behind him, 8 px down. Drawn while his x speed is 4.125 px/frame and its animation runs. Stays for the whole fight. |
| 1 | buster shot, straight (`0x800FB328`) | first 12 updates: attack (−6, −9, 18, 18); then attack (−24, −9, 36, 18), hurt (−24, −11, 32, 22); 1 HP, `dmg_tbl_one_hit`; contact **4** | Starts 30 px in front, 10 px up; sound 139. Still for 3 updates, then **4 px/frame** forward. Not drawn for its first 12 updates (the muzzle flash covers it). Ends after 112 updates, or when a weapon hits it (explosion). |
| 2 | buster shot, spread | as `var` 1 | As `var` 1, but at update 3 the direction is forward + one of −11.25°, 0°, +11.25° (`GetRNG() % 3`), 4 px/frame. |
| 3 | muzzle flash (`0x800FB66C`) | none | 30 px in front, 10 px up; ends with animation 11 (12 updates). |
| 4 | saber wave, aimed (`0x800FB764`) | attack (0, −16, 18, 32), hurt (−6, −12, 24, 24); `dmg_tbl_deflect` (it cannot be destroyed); contact **16** | 70 px in front, at his height; sound 143. **4 px/frame** toward the player, but only level or upward: the angle is limited to 0° … 45° up. Ends after 100 updates. |
| 5 | saber wave, straight | as `var` 4 | As `var` 4, but always horizontal. |
| 6 | ground pillar (`0x800FB964`) | after the rise starts: attack (−12, −24, 24, 36), hurt (−12, −28, 24, 40); 1 HP, `dmg_tbl_one_hit`; contact **8** | 8 pillars around a point 24 px in front of him, at y + 4: pairs at ±32, ±96, ±160, ±224 px, with delays 2, 10, 18, 26 updates (so the eruption runs outward at 8 px/frame). After its delay: a ground burst (`var` 7) 12 px lower; 4 updates later it **rises at 6 px/frame** and ends above the room top (y < −32), or when a weapon hits it. |
| 7 | ground burst (`0x800FD914`) | none | Sound 141; ends with animation 14. |
| 8 | big crescent (`0x800FBD38`) | attack = hurt (24, −66, 16, 132): 16 × 132 px; 1 HP, `dmg_tbl_one_hit`; contact **10** | 64 px in front, 76 px up; sounds 146 and 150. At random **2 or 8 px/frame** forward. No boxes test for the first 7 updates, and it does not move then. A weapon hit removes its boxes; it blinks for 20 updates and ends. Else it ends when it leaves the view (margins 32 × 128 px). |
| 9 | spark (`0x800FC02C`) | none | 13 px in front, 50 px up; ends after 18 updates. |

Notes [certain for the numbers]:

- The big crescent's box is from 166 px to 34 px above the floor. The player X in the save state has a
  hurt box from 38 px above the floor to the floor, so a standing X is hit by its lowest 4 px. A lower
  pose (dash) may pass under it [likely; not checked].
- The buster shot, the pillar and the crescent have `dmg_tbl_one_hit`: one hit destroys them, but
  only from a weapon with `+0x9C` ≠ 0. `ContactWeapon` skips the other shots, so plain buster shots
  pass through them (`docs/damage.md`, "Shots that can hit enemy shots") [certain for the code].
  The saber waves cannot be destroyed.

## Damage

`0x800750FC` (`dmg_tbl_zero_nightmare`), not yet in `docs/engine.md`:

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 2 2 2 – 2 1 2 2 2 2 2 2 |
| 16–31 | 2 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 2 2 2 2 2 2 2 2 2 8 1 4 8 |
| 48–63 | 2 1 5 5 1 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

The ids that matter:

| ids | damage |
|---|---|
| 0 (lemon), 9 (half charge) | 1 |
| 18, 19, 20 (full charge parts) | 4 each |
| 24, 25 / 26 (sabre combo) | 2 each / 5 |
| 50 (STUN WARP) | 5 |
| 17, 22, 44, 47, 57 | 8 |
| 60, 62 | 15 |
| 21, 23, 64–78 | deflected |
| 7 | no effect |

With 48 HP (before the part bonuses and the Easy × 1.25): 48 lemons, or 12 full charges (the
30-update invulnerability lets only the first part of a charge count [likely]), or about 6 sabre
combos (2 + 2 + 5).

Dealt (before the difficulty scaling): body 10; buster shot 4; saber wave 16; pillar 8; big
crescent 10.

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 149 | intro, when he appears |
| 153 | warp: the start of the warp-out and of the warp-in; intro |
| 133, 151 | dash start |
| 134 | dash stop |
| 135 | saber slash (BUSTER, SLASH) |
| 139 | buster shot |
| 143 | saber wave |
| 152 | ground punch start |
| 142 | ground punch impact |
| 141 | ground burst |
| 146, 150 | big crescent |
| 148 | STUN WARP, death |
| bank 0, 14 | HP bar tick |

## Summary for a reimplementation

- A floor boss, 32 × 38 px, contact 10, HP 48 (50 / 52 / 56 at higher boss levels). After a hit: 30
  updates of invulnerability (2 for the first sabre hits). Weapon id 50 (X's saber slash on the
  floor) makes him warp away.
- THINK picks a short chain at once: near (≤ 64 px) → slash + dash (high HP) or ground punch (low HP);
  far and level → buster pairs, dashes, warp to the player and slash; far and higher/lower → dash +
  slash (low HP: or dash + ground punch). Below half HP: rapid fire, ground punch, and the slash also
  throws a wave.
- Moves: dash 4.125 px/frame until near the player (at least about 140 px); two buster shots
  (4 px/frame, contact 4) 40 updates apart, then a slash that throws an aimed wave (4 px/frame,
  contact 16, not destroyable); rapid fire of 8 spread shots, 6 updates apart; ground punch: 8
  pillars rise at 6 px/frame, spreading outward from his fist; a long wind-up slash; the warp
  (192 updates, intangible) to the player's x or to the far side.
- Hard: the far-and-level and far-other-height chains use CRESCENT instead of SLASH: a warp to the far
  side, then 1–4 big crescents (2 or 8 px/frame, contact 10), the count by boss level.
- Death: he holds the player, flashes 127 updates, explodes, no soul; exit request `0x10`; at boss
  level 3 he gives Black Zero (`game+0x5F` bit `0x20`).

## Open questions

1. The weapons of ids 22, 47, 57 and 60 (all X's; 47 is made by the Shadow special attack, step
   37). Id 17 is the charged Ray Arrow (`docs/players/weapons.md`), 44 Zero's Rekkoha
   (`docs/players/zero.md`), 62 a Ride Armor punch. The owners: `docs/enemies/high-max.md`,
   "Weapon ids of the guard break and the stun".
2. Engine objects 24 and 26, and player event step 29 (intro), as for the other bosses.
3. What the boss-level counter `game+0x8D` counts: boss level 3 (counter 0 or 1) is the condition
   for Black Zero.
4. The after-images: the kit grouped `0x800FDC6C`, `0x800FDCD4`, `0x800FDD84` into the table of
   `var` 9; they are probably the routines of the three effect objects (type 6) [likely; not read
   through the effect class table].
5. Whether a dashing or crouching player passes under the big crescent.
6. The dash end against a wall (no wall test in his code).
7. Timings are hand-read from the animation table; check the dash length and the warp in play.
