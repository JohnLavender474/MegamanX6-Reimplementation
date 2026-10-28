# Guardian (stage 10, main object type 30): behaviour sheet

The Guardian is a flying robot of Shield Sheldon's stage. It has a large shield in front and two
thrusters. It hovers in place and never moves. When the player comes near, it lifts its shield, fires
four aimed shots, and lowers the shield again. The shield deflects every weapon, so from the front
the body can be hit only while the shield is up. From behind, the body can always be hit. It turns to
face the player when the player gets behind it. The official name is from the Mega Man Knowledge
Base list of X6 enemies. The larger variant (`id` ≥ 16) is the **Death Guardian**: see
[Death Guardian](#death-guardian-id--16) below.

Overlay `10` (stage 7, Shield Sheldon), code at `0x800EFE48`–`0x800F0BB0`. Save state
`sheldon-8_lower-1` (stage 7, area 0, X, Normal; one Guardian live). The stage 22 state
`sheldon-11_laser-shot` has the same code in another overlay (`0x800EEC0C`; every instruction the
same after relocation, the same boxes). Kit: `python3 -m mmxdemake.rekit --name guardian`. Export:
`guardian` (`enemy-6575747cb9`). Units, confidence tags and the shared engine rules:
`docs/engine.md`. Records: `docs/stages/sheldon.md`.

## Spawn and set-up [certain]

- **HP 6**, contact damage **6** (the Death Guardian: **7**), `iframes` 0, draw priority 6, layer =
  `mega.layer`.
- **It does not move.** All speeds are 0 and the code calls no move routine. It hovers at its record
  point (both live Guardians are exactly at their record x and y). It has a terrain box (32 × 32 px,
  centre at the origin), used only for the squeeze test.
- Animation 0, step WAIT.
- The box pointers are 4 **overlay globals** (`0x800FCFF0`…`0x800FCFFC`), written by each set-up.
  So all live Guardians use the boxes of the Guardian that was set up last. This matters only for the
  Death Guardian, which can lose its large boxes (below) [certain for the code; seen in
  `sheldon-16_death-guardian`].

## The `id` parameter [certain]

| id | behaviour | records in area 0 |
|---|---|---|
| low digit 0 | faces left at spawn | 12 (`id` 0) + 1 (`id` 16) |
| low digit not 0 | faces right at spawn | 5 (`id` 1) |
| high digit not 0 (`id` ≥ 16) | **Death Guardian**: contact 7, the larger boxes (below), and the sprites, texture and palette of the next sprite bank | 1 (`id` 16, at (3928, 664)) |

- The stage type list holds type 30 twice (banks 7 and 8). The normal Guardian uses bank 7 (the
  first). `id` ≥ 16 takes bank 8: the Death Guardian, with a wide spiked shield and the same
  animation table [certain]. The export `guardian` has bank 7; the Death Guardian has its own
  export (below).
- 18 records: 12 kind 0, 6 kind 2 (Hard and above).

## Geometry [certain for the numbers]

Facing left (the shield side is the front); mirrored when facing right.

| box | normal | Death Guardian | area (normal) |
|---|---|---|---|
| shield, closed: hurt box with `dmg_tbl_deflect` | (−24, −27, 17, 54) | same | x − 24 … x − 7, y − 27 … y + 27 |
| shield, open: hurt box with `dmg_tbl_deflect` | (−31, −32, 62, 20) | same | x − 31 … x + 31, y − 32 … y − 12 |
| body: hurt box with `dmg_tbl_normal` | (−8, −7, 34, 16) | (−15, −7, 40, 16) | x − 8 … x + 26, y − 7 … y + 9 |
| contact box 1, closed | (−17, −16, 10, 34) | (−24, −27, 17, 54) | x − 17 … x − 7, y − 16 … y + 18 |
| contact box 1, open | (−21, −26, 42, 16) | (−42, −33, 80, 17) | x − 21 … x + 21, y − 26 … y − 10 |
| contact box 2 | (−8, −2, 33, 6) | (−15, −2, 39, 6) | x − 8 … x + 25, y − 2 … y + 4 |
| terrain box (centre, half size) | (0, 0), 16 × 16 | same | x − 16 … x + 16, y − 16 … y + 16 |

- Closed, the shield covers the front of the body: a shot from the front touches the shield
  (x − 24) before the body (x − 8).
- Open, the shield is above the body (y − 32 … y − 12) and the front of the body is free.
- From behind (x + 26 side) the body box is never covered.

## State machine

States (table `0x800FC460`): 0 set-up `0x800EFE48`, 1 main `0x800F006C`, 2 death `0x800F0434`,
3 `DeleteObject`, 4 `DeleteObject2`. Steps (table `0x800FC474`): 0 engine hit reaction, 1 resume
(unused), 2 and 6 empty, 3 WAIT `0x800F04F8`, 4 TURN `0x800F0648`, 5 ATTACK `0x800F073C`, 7 four
empty sub-steps.

### Main (state 1) [certain]

Each update, in this order:

1. `AnimeTick`, then the step.
2. Two contact tests (`ContactMega`): contact box 1 (closed or open), then contact box 2.
3. **Shield test:** hurt box = the shield box (closed or open), `dmg_tbl_deflect`, `ContactWeapon`.
4. **Body test** (when the shield test did not kill): hurt box = the body box, `dmg_tbl_normal`,
   `ContactWeapon` again. A shot that the shield deflected in this update returns "already hit"
   here: the object keeps the shot's hit tag (`+0x65`), so the same shot cannot also hurt the body.
5. Killed: death (below).
6. Off screen 96 → `DeleteObject2`. Else visible 64.
7. Squeeze test (floor and ceiling, or both walls): death with no drop. Then `CollideCheck`.

"The player is behind" means: the Guardian does not face the player (facing left and the player's x
greater, or facing right and the player's x smaller or equal).

### WAIT (step 3) [certain]

| update | event |
|---|---|
| 0 | animation 0 (hover); timer 120 |
| 25, 57, 89 | when the player is behind: TURN |
| 120 | the wait ends |
| 121 on | each update: when `player.x − x < 188` (signed), TURN if the player is behind, else ATTACK |

- The 188 px rule has no lower limit: a player to the **left** counts at any distance. For a
  Guardian that faces left with the player coming from the left, it attacks as soon as the wait ends.
  A player to the right must come within 188 px (11.75 blocks).

### TURN (step 4) [certain]

| update | event |
|---|---|
| 0 | animation 2 (turn) |
| 11 | it faces the other way (frame 6); animation 1 (thrust) |
| 31 | ATTACK |

A turn always goes on to an attack, also when the player is far away.

### ATTACK (step 5) [certain for the code; timing ±1 update]

| update | event |
|---|---|
| 0 | animation 3: the shield lifts |
| 22 | **open** (frame 7, flag 2): the shield boxes go up, the body is free from the front |
| 37 | lifted (frame 11, flag 1) |
| 67 | aim 1 |
| 68 | shot 1 |
| 88, 89 | aim 2, shot 2 |
| 109, 110 | aim 3, shot 3 |
| 130, 131 | aim 4, shot 4 |
| 141 | animation 12: the shield comes down |
| 149 | **closed** (frame 2, flag 2) |
| 176 | WAIT |

- **Aim:** 1 update before each shot it aims at the player in 32 directions (`AimAt`), from a point
  6 px in front of its origin. Each shot uses its own aim, so the four shots follow the player.
- **Front half only:** facing left it fires only in the directions from straight up, through left,
  to straight down (directions 8–24); facing right, from straight down through right to straight up
  (24–31, 0–8). When an aim falls in the back half, it fires no more shots in this attack: 10
  updates later it lowers the shield (animation 12), so the attack ends early.
- Each shot also starts animation 4 (the recoil, 9 updates).
- So the shield is open for about **127 updates (2.1 s)** per attack, and a full cycle (WAIT and
  ATTACK) is about **297 updates (5 s)** with 4 shots 21 updates apart.

### Death (state 2) [certain]

- At the kill: one explosion, 4 debris pieces (animations 8–11), the drop roll (table 20; none when
  weapon id 55 kills it), the body is hidden at once.
- Then for 32 updates: one more explosion every 6 updates (5 in all) at a random point within
  ±16 px. Then `DeleteObject`: the record can spawn it again.

## Animations (table `0x800FC428`) [certain]

| # | what | frames |
|---|---|---|
| 0 | hover (WAIT) | sprites 0–3, 3 updates each, loop |
| 1 | strong thrust (after a turn) | sprites 4–11, 3 updates each, loop |
| 2 | turn | sprites 12–17, flag 1 on frame 6 (update 11), held |
| 3 | lift the shield | 12 frames: flag 2 on frame 7 (update 22), flag 1 on frame 11 (update 37), held |
| 4 | recoil at each shot | sprites 27–30, 26; 9 updates, held |
| 5 | the shot | sprites 31–36, 3 updates each, loop |
| 6 | lower the shield (another end) | not started by this code |
| 7 | thrust, 18 frames (sprites 38–55) | not started by this code |
| 8–11 | debris | one sprite each |
| 12 | lower the shield | 10 frames: flag 2 on frame 2 (update 8), flag 1 on frame 9 (update 35), held |

The Death Guardian uses the same table with the sprites of bank 8.

## The shot (visual object type 35) [certain]

- Copies the parent (`docs/engine.md`), also its `var`. It starts at the Guardian's origin and moves
  6 px to the front in its first update (no other move in that update).
- Speed **3 px/update** in the aimed direction (32 directions; `SetSpeedDir` × 3). No gravity.
  Seen live in `sheldon-16_death-guardian` (speed 3.00, 6 px start offset).
- HP 1, contact damage **3**. Box `0x800FC4D4` (−3, −2, 5, 4): about 5 × 4 px. Terrain box half
  size 5 × 5 (for `CornerCheck`). `dmg_tbl_one_hit`: one hit destroys it (with an
  explosion), but only from a weapon with `+0x9C` ≠ 0. `ContactWeapon` skips the other shots, so
  plain buster shots pass through it (`docs/damage.md`, "Shots that can hit enemy shots") [certain
  for the code].
- Standard shot ends: it vanishes on the player, bursts on walls (`SpawnHitEffect`), vanishes 32 px
  outside the view. Visible within 16 px. Draw priority 0.
- Sound: bank 2, id 79, from the shot's set-up (once per shot).

## Damage

- Shield: `dmg_tbl_deflect`. Every weapon is deflected, except ids 7, 23 and 79, which pass through.
- Body: `dmg_tbl_normal`, 6 HP: **2 lemons**, 1 half charge, 1 full charge, 2 sabre hits
  (`docs/engine.md`). No invincibility, no knock-back [certain].
- How to hit it: from behind at any time; from the front only while the shield is open (ATTACK,
  updates 22–149) [likely for play: the shield box is in front of the body box].
- Dealt: body **6** (Death Guardian: 7) by two contact boxes; shot **3** (before the player's damage
  scaling: Easy 4 / 2, Hard 7 / 3).

## Death and drops [certain]

Drop table **20** (the same as Meta Wheel F and Batton Bone B81). No drop for a kill by weapon id 55,
and none for a squeeze.

## Sounds

Bank 2, id 79: each shot.

## Death Guardian (`id` ≥ 16)

The Death Guardian is the larger Guardian with a wide spiked shield. It is the same object (main
type 30) and the same code; only the set-up branches on `var & 0xF0`. The official name is from the
user. Record 102 (`id` 16, (3928, 664), kind 0, faces left at spawn) is the only one. Save state
`sheldon-16_death-guardian` (stage 7, area 0, X, Normal; the Death Guardian live). Export:
`death-guardian` (proposed name; now `enemy-2dadc31ecb`; 13 animations, from bank 8 of stage 7 and
bank 7 of stage 22). Kit: the Guardian kit (the same code).

### What the code changes [certain]

The code reads `var` in 3 places only: the set-up kind (`var & 0xF0`, `0x800EFEB8`), the facing
(`var & 0x0F`, `0x800F002C`), and the shot, which copies `var` (`0x800F0308`) but never reads it.
So only the set-up differs:

| item | Guardian (`var & 0xF0` = 0) | Death Guardian (`var & 0xF0` ≠ 0) |
|---|---|---|
| HP | 6 | 6 (written before the branch) |
| contact damage | 6 | **7** |
| box globals | `0x800FC29C`, `0x800FC2A0`, `0x800FC2A8`, `0x800FC2A4` | `0x800FC2B0`, `0x800FC2B4`, `0x800FC2BC`, `0x800FC2B8` |
| terrain box | `0x800FC2AC` (0, 0, 16, 16) | `0x800FC2C0` (0, 0, 16, 16): the same values |
| sprite bank | the spawner's: the first bank of type 30 (bank 7) | `GetSpriteBank(30)` + 1 (bank 8) |
| texture (`+0x40`) | `bank_tpage[7] >> 7` = `0x2D0` | `bank_tpage[8] >> 7` = `0x340` |
| palette (`+0x42`) | `0x78CC` (x 192, y 483) | `0x7900` (x 0, y 484) |
| sprite data (`+0x3C`) | `sprite_bank_base` entry 7 | entry 8 |
| animation table | `0x800FC428` | the same |

- The same for both: the shield boxes (`0x800FC2C4`, `0x800FC2C8` are not globals), the states and
  steps, all timings (WAIT 120, TURN 11 + 20, ATTACK 176), the 188 px rule, the back-half rule, the
  death, drop table 20, the sound.
- The shot is the same (visual type 35: HP 1, contact 3, 3 px/update, the same box). It copies the
  texture, palette and sprite data of its parent, so the Death Guardian's shot uses sprites 31–36
  of bank 8.
- The large boxes (from the table under "Geometry"), facing left: contact box 1 closed x − 24 … x − 7,
  y − 27 … y + 27 (the same area as the closed shield); contact box 1 open x − 42 … x + 38,
  y − 33 … y − 16 (wider than the open shield, x − 31 … x + 31); contact box 2 x − 15 … x + 24,
  y − 2 … y + 4; body x − 15 … x + 25, y − 7 … y + 9. The closed shield (x − 24 … x − 7) still
  covers the front of the body (x − 15).
- **The box globals are shared.** A normal Guardian that is set up after the Death Guardian writes
  the normal boxes. Then the Death Guardian keeps contact 7 but uses the normal contact and body
  boxes until the next Death Guardian set-up.

### What the save state shows

`sheldon-16_death-guardian`, main slot 12 [certain for the values]:

- Type 30, `var` 16, record `0x800FB210` (bytes `01 1E 10 10 58 0F 98 02`: (3928, 664)); the object
  at (3928, 664): it did not move.
- HP 6, contact 7, priority 6, `iframes` 0. Sprite data `0x801583D8` (bank 8), texture `0x340`,
  palette `0x7900`: the values of the formula for bank 8. Animation table `0x800FC428`.
- `flip` `0x40` (faces right): the player (4032, 683) is behind a Guardian that spawned facing left,
  so it turned.
- State 1, step 5 (ATTACK), step2 2 (aim wait): `+0x7C` = 2 shots left, `+0x7E` = 3, last aim
  `+0x81` = 31 (right and a little down; the player is 98 px right of the aim point and 19 px
  down, about 11°).
- **The box globals hold the normal boxes** (`0x800FC29C`, `0x800FC2A0`, `0x800FC2A8`,
  `0x800FC2A4`), and the object's last attack and hurt boxes are `0x800FC2A4` and `0x800FC2A8`. No
  other Guardian is live. So a normal Guardian was set up after it and then removed: here the Death
  Guardian hits with 7 through the normal boxes. This confirms the shared globals in play.
- One shot (visual slot 1): type 35, `var` 16, the bank 8 sprite data, texture and palette, HP 1,
  contact 3, speed (2.942, −0.585) = **3.00 px/update** in direction 31, at (3984.02, 673.95). That
  is 17.0 updates from (3934, 664) (the origin + 6 px to the front), the same as
  20 − `+0x7E` = 17 updates after shot 2. This is the first live Guardian shot: the speed and the
  start point are confirmed. The player is in step 17 (hit reaction), probably from shot 1.

### Summary

- The same HP (6), attack, timing and shots as the Guardian. The differences: contact 7, larger
  contact and body boxes, and the bank 8 sprites (also for its shots).
- In play, its boxes are the normal ones when a normal Guardian was set up after it.

## Summary for a reimplementation

- Stationary flying turret, 6 HP, 6 contact damage. A shield in front deflects everything.
- Waits 2 s, then attacks when the player is within 188 px to its right (or anywhere to its left).
  It turns to face the player (11 + 20 updates) when the player is behind it, and then attacks.
- Attack: lift the shield (body open from the front for about 2.1 s), 4 aimed shots at 3 px/update
  (3 damage), 21 updates apart, then lower the shield. It stops shooting when the player goes behind.
- From behind, the body can always be hit.
- `id` 1 faces right; `id` 16 is the Death Guardian (7 contact, larger boxes, second sprite set;
  the same HP, timing and shots).

## Open questions

1. Which weapon id 55 is (no drop).
2. Which normal Guardian wrote the box globals in `sheldon-16_death-guardian` (no other Guardian is
   live there).
3. Steps 1 and 7 and animations 6 and 7 have no use in this code.
4. Not checked in play: the cycle timing, the open window, the back-half rule, the 188 px rule and
   whether a shot from the front really cannot reach the closed body.
