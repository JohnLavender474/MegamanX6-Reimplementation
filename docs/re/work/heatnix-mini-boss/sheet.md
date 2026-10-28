# Nightmare Snake (stage 05, main object type 25): behaviour sheet

Nightmare Snake is the mid-boss of Blaze Heatnix's stage (Magma Area). It is a red mechanical
serpent that bites its own tail, so it is a ring of about 128 px ("the red dragon donut"). The
ring itself is drawn on **BG layer 1**: the code moves the layer 1 camera to move it. On the ring
are four green **gems** (main 25 `var` 0–3): only the gems can be hurt. Four **jet flames**
(visual 29 `var` 0–3) show on the side behind the move, and the gems fire **green energy balls**
(visual 29 `var` 4, 5). One main type holds all the Nightmare Snake fights of the stage (`var` 4–10);
this sheet is about the first one, `var` 5, which rolls left and right on the floor of one room.

Stage overlay `05` (`rock_bin/05.bin`, loaded at `0x800E9860`; stage 3, area 0) [certain: the
overlay file matches RAM in `heatnix-2_mini-boss`, 4 of 44084 bytes differ]. Save states (in
`~/mmx-data/x6/dumps/`): `heatnix-1_start.state.gz` (the stage start: `var` 5 and `var` 4 wait in
state 0) and `heatnix-2_mini-boss.state.gz` (the fight runs: Zero at (366, 618), the ring at
(617, 584), rolling right at full speed). Written with the RE kit (`python3 -m mmxdemake.rekit
--state heatnix-2_mini-boss.state.gz --type 25`, and `--class visual --type 29`) and
`docs/re-guide.md`. Units, confidence tags and the shared engine rules: `docs/engine.md`. The name:
"Nightmare Snake" (ナイトメア・スネーク, Naitomea Sunēku), "the name of five red Mechaniloid mid-bosses
that appear in Blaze Heatnix's stage", modelled on the Ouroboros [Fandom / Mega Man wiki, through a
web search; the page itself was not read].

The objects of the fight:

| class, type, `var` | object | made by |
|---|---|---|
| main 25, 5 | the snake body (the boss object: the HP bar, the moves, the ring on BG layer 1) | the fixed record list of area 0 (`fixed_set_tbl`), index 0, at (448, 512) |
| main 25, 0–3 | the four gems (the only parts that take damage) | the body set-up |
| visual 29, 0–3 | the jet flames (no damage) | the body set-up |
| visual 29, 4, 5 | energy balls (4: can be shot down; 5: cannot) | each gem, every 120 updates |
| stage 8, 7 | the boss gate at (944, 608), locked until the snake dies (`docs/enemies/boss-gate.md`) | the fixed record list, index 2 |

## Coordinates

The room is the floor at y 640 between the left wall (inner face x 352, from y 432 down) and the
right wall (inner face x 928, the gate at x 944) [certain from the collision layer]. The floor has
three 64 px wide, 32 px deep dips (x 448–512, 608–672, 768–832) [certain for the collision]. The
camera limits in the state are x 336–624, y 480–480 [certain for the state].

**The ring on layer 1.** Each update the body writes the layer 1 camera = (512 − x, 512 − y) and
asks a redraw (`cam1+0x4C`). The layer is drawn relative to the layer 0 camera (as for Nightmare
Pressure), so layer 1 pixel (512, 512) is always on the body point [certain: the state has body
(617, 584), layer 1 (−105, −72); the ring art in the layer 1 map is at x 433–591, y 432–576, centre
(512, 504), which is the centre of the terrain box]. The ring is shown (`cam1+0x03` = 1) only while
the body is within 96 px of the view; the body object itself is never drawn (`display` = 0).

## Spawn and set-up (state 0) [certain]

- **Trigger.** At the stage start the fixed record makes the body in state 0. It waits while
  `game.checkpoint` ≠ 0 (then it never starts), and while the player is at x < 363 **and**
  y < 464 (above the room, left of the first gem). When the player passes x 363 or y 464:
  `PlayerEvent(20, 64)` (the player is held, facing right) and the set-up.
- **The body:** x 576, y 584. HP 32 (filled in the intro), contact damage **8**, priority 5,
  layer = `mega.layer`. Attack box `0x800F3DDC` (−51, −67, 106, 118); hurt box `0x800F3DE0`
  (−42, −62, 100, 100) with `dmg_tbl_deflect`; terrain box `0x800F3DD8` centre (0, −8), half
  size 64 × 64. `+0x8D` = 1 (the gems may shoot). `game+0x26` = 0 (the gate locks).
- **Four gems** (main 25 `var` 0–3, in the next free main slots, `+0x80` = the body) and **four jet
  flames** (visual 29 `var` 0–3, `+0x7C` = the body), all at the body point, with the body's
  texture, palette (without bit 15) and animation table `0x800F3ED8`.
- Layer 1 shown (`cam1+0x03` = 1), `cam1+0x52` = 0 [meaning open]. Then state 1, step 9 (INTRO).

**The gems** [certain]: HP **32** each, contact 8 but **no attack box** (a gem does not hurt the
player), damage table `dmg_tbl_nightmare_snake` (`0x800748DC`, below), no terrain box, step 1.
Each update its place is the body + an offset (`0x800E987C`):

| `var` | gem | offset | hurt box (`0x800F3DE4` + 4 × `var`) | animation |
|---|---|---|---|---|
| 0 | top left | (−44, −57) | (−12, −14, 16, 16) | 0 |
| 1 | top right | (+58, −54) | (−12, −14, 16, 16) | 1 |
| 2 | bottom left | (−28, +50) | (−14, −13, 16, 16) | 2 |
| 3 | bottom right | (+66, +36) | (−12, −13, 16, 16) | 3 |

The gems are visible 32 px off screen. The ring does not turn (a BG layer cannot); each gem
animation is a slow glow.

## The `var` parameter

| `var` | made by | start | after the intro | what |
|---|---|---|---|---|
| 0–3 | the body | – | step 1 | gems |
| 4 | fixed record 1 at (1248, 2624); again by boss gate `id` 7 | (1072, 2752), when the player y ≥ 2384; deleted when checkpoint ≥ 3 | step 2 | runs round the room walls (right, up, left, down) [likely] |
| **5** | fixed record 0 at (448, 512) | (576, 584), see above; waits forever when checkpoint ≠ 0 | **step 3** | **this fight: rolls left and right on the floor** |
| 6 | record 22, `id` 6 (1664, 2464) | the record | step 4 | rolls left and right and hops at fixed x (1953, 2241 / 2400, 2112) [likely] |
| 7 | record 23, `id` 7 (1664, 2784) | the record | step 5 | rolls left and right [likely] |
| 8, 9 | records 25, 24, `id` 8 (3021, 2976), `id` 9 (3021, 2224); 8 is deleted at checkpoint 5, 9 at checkpoint 4 | (3024, layer 0 camera y + 352) | step 10, then 6 | leaps out of rising lava at x 2944 or 3104 (two lava kill boxes, visual 29 `var` 6) [likely] |
| 10 | record 26, `id` 10 (3378, 1024) | x 3616 | step 7 | over lava (layer 2), jumps when the player is within 48 px in x (four kill boxes, visual 29 `var` 7) [likely] |

`var` 4 and 6–10 are not in a save state yet; their rows come from the code only.

## State machine

States `0x800F3F1C`: 0 set-up, 1 main, 2 and 3 end (`0x800EC580`). Steps `0x800F3F2C`: 0
`HitReactionDispatch` (not used), 1 gem, 2–7 the moves of `var` 4, 5, 6, 7, 8/9, 10, 8 DEATH,
9 INTRO, 10 INTRO of `var` 8/9.

### Main (state 1) [certain]

Body (`var` ≥ 4), each update:

1. The step.
2. `ContactWeapon` (the ring deflects every shot; the result is not used).
3. Outside the intro (step ≠ 9, 10): **HP = 0**. The gems, which update after the body, then add
   their HP to it (see "Damage"). So the bar shows the sum of the gem HP.
4. When `act4` (the dead-gem count) ≥ 4: step 8 (DEATH), no attack and hurt box.
5. `ContactMega` (contact 8), `CollideCheck`, then the screen test: off screen 128 px (the code
   does nothing with it), else visible 96 px; that value goes to `cam1+0x03`, `display` = 0; layer 1
   camera = (512 − x, 512 − y), redraw.

Gem (`var` 0–3), each update:

1. Step 1 (the shot, below), `ContactWeapon`.
2. Outside the body intro: body HP += gem HP (a byte add; −128 is changed to 127).
3. Killed: `SpawnExplosion`, state 2 (deleted), body `act4` + 1. Else: visible 32 px, place = body
   + offset.

### INTRO (step 9) [certain]

| sub | what |
|---|---|
| 0 | When the player is in step 20 (held): `game.player_lock` = 1, HP bar on (`game+0x24` = 1), `boss_hp_max` = **127**, `BossLevel` (the level is not read by this code), `game+0x25` = 0, `game.boss` = the body. |
| 1 | **+1 HP every 2nd update** (even `frame_count`) from 32 to 127 (about 190 updates), a tick (bank 0, id 14) every 3 updates. At 127: `PlayerEventEnd` (the player can move), step 3, sub 0. |

During the intro the ring stands at (576, 584); its contact damage acts. The gems do not shoot
(body step ≥ 8).

### ROLL (step 3, `var` 5) [certain for the code]

Each update first: **when the player y < 432, it is set to 432**. So the player cannot climb out of
the room over the left wall (the left wall ends at y 432).

| sub | what |
|---|---|
| 0 | `speed_x` = +1 px/frame, `accel_x` = −1/16 (`−0x1000`). Sub 1. |
| 1 | Roll right: `speed_x` capped at 2, `MoveObject2` (the speed goes up 1/16 px/frame each update: 1 → 2 px/frame in 16 updates). On the right wall (`collide_flags` bit 1): sound 24, `Rumble(3)`, `Quake2(60, 2, 1)`, `speed_x` = −1, `accel_x` = +1/16, **wait 60 updates**, sub 2. |
| 2 | Sub 3. |
| 3 | Roll left the same way (speed −1 → −2 px/frame). On the left wall (bit 2): the same sound, rumble and quake, wait 60 updates, sub 0. |

With the terrain box (half width 64) the ring stops at about x 864 on the right and x 416 on the
left: 448 px, about 228 updates per pass, so a full cycle of about **576 updates** (9.6 s) [likely:
from the walls; not timed in play]. The ring is 128 px high and touches the floor; its attack box
covers the hole in the middle, so the player must go over it (wall jump) [likely].

The state confirms the sign of `accel_x` in `MoveObject2`: the stored speed is 2.0625 (capped to 2,
moved, then + 1/16) [certain for the state].

### GEM SHOTS (gem step 1) [certain]

Each gem, when `frame_count` is a multiple of **120**, the body step < 8 and body `+0x8D` ≠ 0
(always 1 for `var` 5): one energy ball at the gem point, sound 31. So all four gems fire in the
same update, every 2 s. The type depends on the difficulty (`game+0x68`):

| difficulty | ball |
|---|---|
| 0 (Easy) | none |
| 1 (Normal) | `var` 4 |
| 2, 3 | X: `var` 4 or 5 at random (`GetRNG() & 1`); Zero: `var` 4 |

### DEATH (step 8) [certain]

When the fourth gem dies:

| sub | what |
|---|---|
| 0 | Timer 120. |
| 1 | **120 updates**: the ring flickers (`cam1+0x03` toggles every update); every 8 updates an explosion (effect type 4, priority 19) at the body + (−32…31, −32…31) (`GetRNG2`). At the end: `ResetLayerCache(0)`, palette fades cleared, `bg_refresh` = 1. |
| 2 | One update with the channel 0 fade at full (r, g, b = 31): a white flash [likely]. |
| 3 | `PaletteFxClear(0)`; HP bar off (`game+0x24` = 0), `game.boss` = 0; ring hidden; **12 explosions** (`SpawnExplosionRand(body, 1, 64, 64)`). |
| 4 | State 3. |

**State 3 (end):** `game+0x26` = **1**: boss gate `id` 7 at (944, 608) can now open. While the player
x < 944 the body stays and keeps the player y ≥ 432; then it deletes itself (`DeleteObject3`). The
gate trigger calls `ClearEnemies` and makes a new main 25 `var` 4 (the next snake, down the shaft
at x 944–976; new limits min x 944, max x 976, max y 2624, min y 448) (`docs/enemies/boss-gate.md`).

No drop, no Nightmare Soul, no checkpoint change, no exit request, no white screen object, no
sound of its own at the death (the explosions have theirs) [certain for this code].

## Animations (table `0x800F3ED8`, 13 animations) [certain for the frames]

The export draws all of them from bank 6 (type 25, 49 sprites). The ring is not a sprite (layer 1).

| # | sprites | ticks | what | used by |
|---|---|---|---|---|
| 0–3 | 3 2 1 0 1 2 (+4 per animation) | 9 8 7 6 7 8, loop | green gem glow, four angles | gems `var` 0–3 |
| 4–7 | 0 3, 4 7, 8 11, 12 15 | 3 3, hold | gem bright / dark | no user in this code [likely: unused] |
| 8 | 31–36 | 2 3 3 2 3 3, loop | green burst | no user in this code [likely: unused] |
| 9 | 37–40 | 2 3 3 2, loop | green energy ball | visual 29 `var` 4, 5 |
| 10 | 16–20 | 2 2 3 3 3, loop | jet flame, sideways | visual 29 `var` 0 (mirrored), 1 |
| 11 | 21–25 | 2 2 3 3 3, loop | jet flame, down | visual 29 `var` 2 |
| 12 | 26–30 | 2 2 3 3 3, loop | jet flame, up | visual 29 `var` 3 |

Sprites 41–48 of bank 6 (other green and cyan balls) are in no animation.

## Jet flames and shots (visual type 29, entry `0x800EDA5C`, states `0x800F4008`)

Set-up `0x800EBF1C`: priority 4, the parent's graphics (copies the parent, `docs/engine.md`).

**Jet flames** (`var` 0–3, step 0, `0x800EC148`) [certain]: no boxes, no damage. Each update: the
place = body + offset (`0x800E988C`): `var` 0 (−84, −11), 1 (+82, −18), 2 (+26, +70), 3 (−4, −83).
A flame is drawn only while the body moves in the matching direction (body `step2` 1 = right: `var`
0, the flame on the left; 3 = left: `var` 1; 2 = up: `var` 2; 4 = down: `var` 3), and not while the
body waits (`+0x7C` ≠ 0) or dies (step ≥ 8). While a flame is drawn: sound 3 every 32 updates
(`frame_count & 31` = 0). A flame deletes itself when the body is in state 3. `var` 5 uses only
sub-steps 1 and 3, so only flames 0 and 1 are ever drawn. The state agrees:
the body rolls right, flame 0 is drawn, 1–3 are not.

**Energy balls** (`var` 4, 5, step 1) [certain]:

| | `var` 4 | `var` 5 |
|---|---|---|
| damage table | `dmg_tbl_normal` (HP 2: one lemon kills it) | `dmg_tbl_deflect` |
| contact | 4 | 4 |
| boxes | attack and hurt (−6, −6, 12, 12), terrain centre (0, 0) half 6 × 6 | the same |

Aimed at the player when made (`ratan2`), **4 px/frame** straight (`MoveObject`), animation 9. It
ends (explosion) when it touches the player, touches terrain, or is killed; it ends without
explosion 96 px off screen; visible 64 px.

**Kill boxes** (`var` 6, 7; the lava fights of `var` 8–10): contact **127**, attack box `var` 6
(−80, −56, 160, 112), `var` 7 (−80, −64, 160, 128); never drawn [certain for the code; not in a state].

## Damage [certain]

**HP.** The bar maximum is 127. Outside the intro the body HP is the sum of the four gem HP each
update, capped at 127 (4 × 32 = 128 shows as 127). **The fight ends when all four gems are
destroyed**, not by the body HP. The ring (`dmg_tbl_deflect`) deflects every shot.

**Gems:** `dmg_tbl_nightmare_snake` (`0x800748DC`, new; read from the executable), 32 HP each:

| ids | damage |
|---|---|
| 0–15 | 1 2 5 D 5 5 4 – 2 1 4 8 D 8 8 8 |
| 16–31 | 2 8 4 4 4 D 8 D 3 3 5 2 2 2 2 2 |
| 32–47 | 2 5 D 5 6 3 2 2 2 2 4 2 8 1 4 8 |
| 48–63 | 4 4 3 3 1 5 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

- **Fire is deflected**: ids 3, 12 and 34 (slot 3, the stage's own weapon, and Zero's rising fire
  wave), also 21 and 23.
- The charged special weapons of the other slots (ids 11, 13, 14, 15, 17) and ids 22, 44, 47, 57
  do 8; the uncharged ones of slots 2, 4, 5 do 5. There is no single weakness and no stun: no
  weapon id has its own code.
- The X-Buster is weak: lemon 1, half charge (id 9) 1, full charge 4.

Hits to kill (before the part bonuses and Easy × 1.25):

| weapon | per gem | all four |
|---|---|---|
| lemon (1) | 32 | 128 |
| half charge (1) | 32 | 128 |
| full charge (4) | 8 | 32 |
| charged special, 8 | 4 | 16 |
| Zero sabre combo (ids 24, 25, 26: 3 + 3 + 5) | 3 combos | 12 combos |
| Zero buster (id 58, 4) | 8 | 32 |

A gem has no invincibility time of its own: only the standard hit tag stops a double hit by one shot.

**Damage to the player** (before the difficulty scaling): the ring 8 (contact, the whole ring
including the hole); energy ball 4. Gems and flames 0.

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 24 | the ring hits a wall (with `Rumble(3)`, `Quake2(60, 2, 1)`) |
| 3 | jet flame, every 32 updates while a flame is drawn |
| 31 | an energy ball is made (up to 4 in one update) |
| bank 0, 14 | HP bar tick in the intro |

## The Rescue Reploid

The Rescue Reploid (stage object 4, `id` 32) at (624, 656) stands in the same room
(`docs/enemies/rescue-reploid.md`). The type 25 and visual 29 code does not read or write it
[certain for the code]. The ring can roll over it; whether its attack box or the energy balls hurt
the Reploid depends on the Reploid's code, not on the snake [not checked].

## Summary for a reimplementation

- A ring 128 px across (a red serpent biting its tail; art on BG layer 1, move it as one sprite in
  Maverick) with four 16 × 16 green gems at fixed offsets. The ring deflects all shots and does 8
  contact damage; the gems take damage and do no contact damage.
- Start: the player is held when it enters the room; the bar fills from 32 to 127 in about 190
  updates.
- Move: roll right from 1 to 2 px/frame (+1/16 per update) until the wall, wait 60 updates with a
  screen shake, roll left the same way, wait 60, repeat. Keep the player y ≥ 432 (no exit over the
  left wall).
- Every 120 updates, on Normal and up, each live gem fires a 4 px/frame aimed ball (contact 4); on
  Hard some of X's balls cannot be shot down.
- Bar = sum of gem HP (32 each, cap 127). When all four gems are gone: 120 updates of flicker and
  explosions, a white flash, 12 explosions, then set the stage flag that opens the boss gate. No
  drop.

## Open questions

1. The other snakes (`var` 4, 6–10): the rows above come from the code only; save states of these
   fights would check them (the lava of `var` 8–10 is on BG layer 2).
2. Whether the ring hurts the Rescue Reploid in the room (the Reploid's code).
3. `cam1+0x52` (written 0 at the set-up; 255 on the other layers in the state).
4. The meaning of the `Quake2` arguments (`docs/engine.md`).
5. `game+0x25` (written 0 at the intro, as other bosses do).
6. Play check: the roll cycle (about 576 updates) and whether the player can pass through the hole
   of the ring (the attack box says no).
