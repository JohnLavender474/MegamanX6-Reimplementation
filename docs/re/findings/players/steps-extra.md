# Player steps 32–77 and the armour forms: behaviour sheet

Part 3 of the player RE plan. The player step table `player_steps` (`0x80073ACC`) has 78 entries,
not 32. This sheet gives the entries 32–77 (X's armour actions, special attacks and weapon poses,
and Zero's sabre techniques) and the armour forms of X: their numbers, what each form changes
and which disc member holds its sprites. The step map is in `docs/players/map.md`; the air
actions of each form are also in `docs/players/air.md`.

Sources: the player kits (`python3 -m mmxdemake.rekit --player`) for `intro-1_start` (X, Falcon),
`sheldon-4_no-armor` (X, no armour) and `turtloid-6_zero-player` (Zero); the stage select and
title overlays (16, 19); the states `sheldon-1_lemons` and `sheldon-4_no-armor`. Notes, pseudo
code and reconstructed C are in [`~/mmx-data/x6/re/x/steps-extra/`](../../work/x/steps-extra/). Units and tags as in
`docs/engine.md` ("Conventions"): speeds in px per update (16.16), times in updates, a positive
`speed_y` is up.

## The forms

**`mega.form` (`mega+0xE7`) = `game+0x5E`: 0 no armour, 1 Falcon, 2 Shadow, 3 Blade, 4 Ultimate,
5 Zero** [certain for the values; the names are likely]. The earlier sheets said "X 1–4, Zero 5"
and "form 1 in every X state". That was the state sample, not the rule: `sheldon-4_no-armor` has
form 0.

How the value is set [certain for the code]:

- **New game** (overlay 19): X gets armour bit `0x01` (`game.armors`, `game+0x5F`) and form 1, so
  X starts in the Falcon Armor. Zero gets bit `0x10` and form 5.
- **Cheat** at the new game (an input counter that reaches 11): X gets bit `0x08` and form 4; Zero
  gets bits `0x31` (bit `0x20` is Black Zero).
- **Armour select** (overlay 16, before a stage): the list always starts with form 0, then has
  form 1 for bit `0x01`, 5 for bit `0x10`, 2 for bit `0x02`, 3 for bit `0x04`, 4 for bit `0x08`.
  The choice is written to `game+0x5E`; form 5 also sets `game+0x38` (the character) to 1, other
  forms to 0.
- **Beam-in** (`PlayerLoadForm`, `0x8003D718`): `mega.form` = `game+0x5E`.

So form n + 1 belongs to armour bit n. With the Light capsule bits (`docs/enemies/light-capsule.md`:
bit `0x02` full Shadow Armor, `0x04` full Blade Armor, `0x08` Ultimate Armor), form 2 is Shadow, 3
Blade, 4 Ultimate [likely names]. The Light capsule gives the Ultimate Armor only when X wears no
armour (`mega.form` = 0) [certain for the code].

### What each form changes

All X forms use the same physics constants: `PlayerPhysicsDefaults` (`0x8003D598`) has no form
test, and `PlayerPhysicsParts` (`0x8003D630`) tests only form 2 [certain for the code]. The
no-armour and the Falcon states have the same constants `+0xF0`…`+0x120` and the same part bits
`+0xEC` (0) [certain]. Walk, jump, dash and wall kick speeds do not change with the form.

| | 0 none | 1 Falcon | 2 Shadow | 3 Blade | 4 Ultimate |
|---|---|---|---|---|---|
| jump pressed in the air | nothing | Falcon dash (step 35) | nothing | Mach Dash (step 74) | hover (step 33) |
| dash pressed in the air | nothing | Falcon dash (step 35) | nothing | Mach Dash (step 74) | air dash (step 13) |
| special attack (button `0x40`, full gauge) | none | step 36 | step 37 | step 77 | step 34 |
| special weapons can charge | no | yes | no | yes | yes |
| weapon energy per shot | full | 2/3 | full | 2/3 | 2/3 |
| hit reaction | light or heavy (classes 1, 2) | always light (class 4) | class 4 | class 4 | class 4 |
| shot type at full charge (`0x8003F7BC`) | 18 | 19 | 18 | 43 | 20 |
| spike tiles (both tests, `0x80030B5C`, `0x80030CA4`) | death | death | no harm | death | death |
| wall slide speed `+0x110` | −1.4375 | −1.4375 | 0 (X holds on) | −1.4375 | −1.4375 |
| super jump (step 31) | no | no | yes | no | no |
| sabre pose animations (steps 45, 46) | 183, 184 | 183, 184 | 185, 186 | 183, 184 | 183, 184 |
| hurt boxes (`0x8003C3C8`) | table A | table B | table C | table A | table A |
| sprite member (`ROCK_X6.DAT`) | 86 | 87 | 88 | 89 | 86 |

[certain for the code in each cell.] More form tests:

- Weapon energy (`0x8003EB0C`): X pays 6 × the table value of the weapon slot; part bit `0x20`
  halves it; else forms 1, 3, 4 pay 2/3 of it (at least 1). Zero pays 6 ×, 3 × with part bit
  `0x20`, 4 × as Black Zero [certain for the code].
- Special-weapon charge (`0x8003EEE4`): level 1 after 25 updates, level 2 after 90 (12 and 45 with
  part bit `0x100000`); only in forms 1, 3, 4 [certain for the code].
- Form 2 at the beam-in: Zero's skill bits are cleared (no effect for X), an effect object (type
  27, var 5) starts and palette 2 of the player palette list loads into palette slots 25 and 24
  (CLUTs `0x7849`, `0x7848`; `PaletteSlotLoad(palette, slot, n)`) [certain for the code;
  what the effect shows is open].
- Form 2: no dust effect at the start of a wall slide; a weapon object of X uses another
  animation [certain for the code].
- Form 4: up to 3 weapon objects of type 21 at a time, else 1 [certain for the code; which shot
  type 21 is: `docs/players/attacks.md`].
- Form 4: the per-update routine `0x8003EC78` (X's shot code, `docs/players/attacks.md`) does
  not run while the form 4 special attack runs [certain for the code].
- Two functions (`0x80021B40`, `0x800228AC`) turn the form into an index: X 0 → 0, 4 → 2, 1 → 3,
  2 → 4, 3 → 5; Zero 1, or 6 as Black Zero [certain for the code; a portrait or icon index
  is a guess].

### Sprites by form

The stage load (`0x80013BF0`) loads one player member of `ROCK_X6.DAT` [certain for the code]:

| who | member | proof |
|---|---|---|
| Zero (`game+0x38` ≠ 0) | 85 | byte match in the Zero state |
| X form 0 | 86 | byte match in `sheldon-4_no-armor` |
| X form 1 | 87 | byte match in all Falcon states |
| X form 2 | 88 | code only (no state) |
| X form 3 | 89 | byte match in `yammark-4_blade-armor` (form 3, `game.armorParts` = `0x0F`) |
| X form 4 | 86 | code only: the same member as form 0 |

The member loads to the same address for all forms, so the player fields point to the same
places: `mega+0x30` (animation table, in overlay 0) and the frame and sprite directories hold the
same addresses in the no-armour and Falcon states; only the bytes differ [certain]. With member 89
the frame directory has 20 entries (not 19), so the frame table `mega+0x38` is `0x80103B30`, not
`0x80103ACC` [certain]. `mega+0x3C` differs because the sprite data is at another offset in each
member. The player `SetAnime` and the palette code do not test the form [certain for the code].

**The armour is a second layer** [certain for the code]. Each packed frame unpacks to the tiles of
two sprites: the body (X with no armour) and the armour. The sprite directory has two piece lists
per frame set: entries 0 and 5 (set 0), 18 and 19 (set 1). `PlayerSetAnime` puts the body list in
`mega+0x3C` and the armour entry (5 or 19) in `mega+0xDC`. `DrawSprites` draws the body with
`DrawObject4`, then, for X in forms 1–4 only, `PlayerArmorLayer` (`0x8002619C`) draws the armour
list with `DrawObjectLayer` (`0x80025984`). That routine takes the CLUT of each piece from the
table `0x80071E68` (`0x7843`, `0x7800`, `0x7844`, `0x7801`, by piece flags >> 2 & 3), not from
`mega+0x42`. Forms 1–3 skip the layer for the beam (step 1, sprite 126); form 4 always draws it.
The armour layer is on top of the body [likely: by the look of the composites].

- CLUT `0x7843` is palette buffer slot 19, which `PlayerPaletteSet` fills with list palette n + 1
  (member offset `0x820` for n = 0; palette 4w + 1 with weapon w). CLUT `0x7844` is slot 20, which
  `PlayerPaletteRestore` fills for some part bits [certain for the code].
- Form 0 draws no armour layer, so it shows the body only. Member 86 holds armour pieces too:
  drawn with its palette 1 they show a black armour with red and white, the Ultimate Armor, which
  form 4 draws [likely]. So **forms 0 and 4 share the frames and the body palette, but only form 4
  has the armour layer**.
- `playerexport` draws the layer for X in forms 1–4: `x-blade` (member 89, `yammark-4_blade-armor`)
  and `x-falcon` (87, `shark-1_ride`) in `~/mmx-data/x6/sprites/players/`.

Each X member has one 228-frame main set and a second large set: 66 frames (86), 93 (87), 94 (88),
76 (89) [certain]. With member 86, 22 animations use sprites past the 66 of set 1: 83–85
(ceiling hold), 140, 169–179 (Falcon dash), 180–182, 189–192 (Mach Dash) [certain]. These are the
actions that form 0 does not have [likely: the member holds only what its form can do].

The exporter (`python3 -m mmxdemake.playerexport`) names the output by member:
`catalog/players.json` has `86` → `x-no-armor`, `87` → `x-falcon`, `89` → `x-blade`, `85` →
`zero`.

## Owners of the entries 32–77

X and Zero use the same table, and the entries past 31 point into the player overlay at
`0x801EA000`. Overlay 0 (X) and overlay 1 (Zero) load at the same address, so each entry is a
function start only in one overlay [certain]:

| entries | who | what |
|---|---|---|
| 32–47 | X | ladder shot, armour actions, special attacks, weapon poses |
| 48–63 | Zero | sabre attacks and skills (49 = 53 and 50 = 54: the same routines) |
| 64–71 | both | stand (`0x80035274`) |
| 72 | — | points into the stage overlay area (data in one state, code in another) |
| 73 | — | main program `0x80037E1C`: runs the animation until `game.exit_req` is set, then player state 3 |
| 74–77 | X | Mach Dash, weapon poses, Blade special attack |

No code writes step 72 or 73: no constant store in the player code, no `PlayerEvent` call with 72
or 73 in the main program or any overlay, and no direct store to `mega.step` [certain for the
search; so they are unused, likely].

## X steps

### Step 32: ladder shot

Entered by `0x801EB6E8` from `TryLadderAir` and `XLadderShoot` (`docs/players/ladder-hurt-events.md`):
X is on a ladder in the shooting pose. The animation comes from a table by `+0x97`; x snaps to the
tile centre. Each update: the ladder exit test, the ladder shot code, the facing from a pressed
direction; when the shot pose ends (`+0x90`): animation 32 at frame 3 and back to the ladder
(climb down) [certain for the code].

### Armour air actions: steps 33, 35, 74

Entered from `TryAirDash` (`0x8003906C`, `docs/players/air.md`) when the air dash is allowed.

**Step 33: hover (form 4)** [certain for the code]. Jump pressed in the air: all speeds 0, no
gravity, `+0x86` = 4 (no other air action), animation 21. After the frame with flag `0x80` (an
effect object type 33), X hovers:

- The hover lasts **180 updates**. When X first moves to a side, the time left becomes **40
  updates**. A new jump press ends it at once. Then X falls.
- X bobs: y changes by a 16-entry table each update.
- Left or right held: X moves at **2.0 px per update** (`+0xF4`) when that side is free.
  Animations: 22 still, 23 forward, 25 back, 24 and 26 to stop. The shooting versions are used
  in the shooting pose.
- A wall test runs (wall grab). In the first hover sub-step, dash pressed starts the air dash
  (step 13).

**Step 35: Falcon dash (form 1)**: `docs/players/air.md`. After animation 169: animation 172
(rising), 173 (falling) or 171 (level), then `FalconDashStep` until it ends, then fall.

**Step 74: Mach Dash (form 3)** [certain for the code]. Jump pressed or a dash input in the air:
animation 189, all speeds 0, `+0x86` = 7, dash timer **15**, a weapon object (type 32) that
follows X (the hit part of the dash, likely). Then:

1. X faces the held side. At the end of animation 189, while jump or dash is held, X waits up
   to 120 more updates.
2. X dashes at **8.25 px per update** (2 × `+0x108`): up with up held (animation 190), down with
   down held (191), else forward (192). X cannot be hit (`no_hit`) for the first 7 moves.
3. The dash ends after 15 moves (**123.75 px**) or when X hits terrain in the dash direction
   (sound bank 1, id 6). A down dash that lands with down held goes to crouch; else stand or
   fall.

### Armour special attacks: steps 34, 36, 37, 77

One entry (`0x8003E344`) for all four. The special-weapon pose code of the stand, crouch, hang,
ladder and air steps tries it [certain for the code]. Conditions: X, no shot wait (`+0x92` = 0), the gauge `+0xA8` full (= `+0x14A`),
button `0x40` pressed, form 1–4. Form 4 also needs `+0x86` = 0 and the facing side free. Then:
the action flags and the shot set-up are cleared, `no_hit` = 1, the player lock is on, and the
gauge goes to 0 (not in form 4). The game's name for these is likely "Giga Attack".

| step | form | start animation (floor / air) | what [certain for the code] |
|---|---|---|---|
| 36 | 1 | 174 / 177 | the screen fades (51 updates), then animation 175 / 178 and, for 120 updates, a weapon object type 49 every 3–5 updates; effect objects; animation 176 / 179; the screen fades back |
| 37 | 2 | 185 / 186 | no terrain box; the fade; two weapon objects type 47; for 148 updates a slash object (50 floor, 51 air) at each flag frame; the fade back |
| 77 | 3 | 185 / 186 | no terrain box; the fade; one slash object (50 / 51) at the flag frame; the fade back at the end of the animation |
| 34 | 4 | 106 | X rises at (± 2.5, 3.5) with the fall gravity until the flag `0x80` frame; at the flag `0x20` frame: weapon object type 22, palette 73, sound (1, 18), then **8.0 px per update** forward with no gravity for **24 updates (192 px)** or until the facing side is blocked |

After each: `no_hit` and the lock are off, the terrain box and the palette come back, and X
stands or falls. A lock (`+0xD3`) during the attack ends it at once.

### Weapon poses: steps 38–47, 75, 76

`0x8003F864` starts a pose step for the current shot type `mega+0x96` (a table of 54 entries,
type − 3) [certain for the code]. It is called from the shot code of the stand, crouch, hang,
ladder and air poses. The shot types are the weapon objects of X; their names are in
`docs/players/attacks.md`.

| shot type | step | animation | the pose ends [certain for the code] |
|---|---|---|---|
| 3, 50 | 45 (floor), 46 (air), 47 (wall) | 183, 184, 187 (form 2: 185, 186) | at the animation end; a slash object (50 floor, 51 air or wall) at the flag frame. 45: a jump or crouch can cancel it after the flag `0x40` frame. 46: air control, wall test, air dash test; then jump or fall. 47: wall kick, wall hold; then wall slide |
| 5 | 44 | 96 (from frame 2) | when the slot `+0x93` is not 5, or at the animation end |
| 11 | 39 | 95 | at the animation end (weapon object 11 and an effect at the start) |
| 12 | 43 | 95 | when the weapon object ends (`+0x99` = 0) or the slot is not 3 |
| 13 | 40 | 97 | when the weapon object ends or the slot is not 4 |
| 14 | 75 | — | all object classes stop (`game.stop`); 60 updates after the weapon object signals (`act4`), they move again |
| 16 | 42 | 95 | when the weapon object ends or the slot is not 7 |
| 17 | 41 | 95 | no terrain box, `no_hit`, lock; at the animation end, 10 weapon objects in 5 columns 32 px apart around X, below the view (camera y + 368, + 528) [guess: they rise]; ends when the weapon object ends |
| 46 | 38 | 185, 186 | at the animation end; slash objects 50 / 51 and a weapon object 46 at the flag frames |
| 56 | 76 | 187, 183, 184 | at the animation end (wall: back to the wall slide); weapon object 56 and a slash object |

## Zero steps 48–63

Zero's steps are entered by Zero's overlay "try" functions in the shared steps (stand, walk,
dash, jump, fall, wall slide, ladder, crouch, hang, ceiling hold). The skill tests use
`mega+0xC9` (Zero's skill bits, from `game+0x60`) [certain for the code]. Button `0x10` is
Zero's attack (sabre), `0x20` the special, `0x40` the special attack [likely names].

| step | entered from | condition | animation | notes [certain for the code unless tagged] |
|---|---|---|---|---|
| 48 | ground steps | attack | 88 | the ground sabre combo; can go on to 55, 48 again, a jump or a crouch |
| 49 | jump, wall kick | attack in the air | 91 | goes on to 50 or the fall |
| 53 | the same | the same, after the super jump (`+0x86` = 6) | 96 | the same routine as 49 |
| 50 | fall, wall slide, air dash | attack in a fall | 91, 92, 96 | landing goes to 48 |
| 54 | the same | after the super jump | 96 | the same routine as 50 |
| 51 | ladder | attack | 93 | back to the ladder |
| 52 | wall slide | attack | 15 | back to the wall slide |
| 55 | ground steps | skill `0x04`, up held, attack | 90 | two effect objects (26, 34) |
| 56 | fall, step 50 | skill `0x08`, down held, attack in the air | 98, 99 | costs energy, a weapon object, moves with acceleration; landing goes to 48 |
| 57 | jump, fall | skill `0x10`, up held, attack in the air | 101, 102 | `no_hit`; starts at (± 3.0, −5.0); on the floor: sound, screen quake, animation 102, then stand |
| 58 | ground and air steps | skill `0x20`, down held, special | 103 | `no_hit`, `+0x86` = 8 |
| 59 | 48 and the ground tries | skill `0x80`, button `0x40`, enough energy | 104, 111 | the special attack: no terrain box, lock, `no_hit`, a screen fade and effects |
| 60 | crouch | attack | 81 | back to the crouch |
| 61 | rope, bar hang | attack | — | back to the hang |
| 62 | ground steps | special | 110 | all speeds 0; a weapon object; ends on landing |
| 63 | ceiling hold (step 31) | attack | 84 | back to the ceiling hold |

Skill bits that start no step: `0x01` (up held and special: a weapon object type 1, costs energy),
`0x02` (the super jump, `docs/players/air.md`), `0x40` (a weapon object type 7, costs energy; no
caller found in the walk) [certain for the code]. The game's names of these techniques are not
in the code [open].

## Play checks

1. X with no armour: jump or dash pressed in the air does nothing; the special attack button
   does nothing; a special weapon does not charge; heavy hits knock X back (class 2).
2. X with no armour: a weapon uses 3/2 of the energy that it uses in the Falcon Armor.
3. X in the Ultimate Armor (cheat): check that X is black with red and white (the armour layer
   of member 86), not like X with no armour.
4. Ultimate hover: about 180 updates when X does not move to a side, 40 updates after the first
   side move; a second jump press ends it; the up-and-down bob.
5. Mach Dash: 15 moves of 8.25 px (about 124 px), up, down or forward; X cannot be hit for the
   first moves; holding jump or dash delays the start.
6. Ultimate special attack (step 34): a short rise, then about 192 px forward.
7. Shadow Armor: spikes do not hurt, X does not slide down walls, the super jump works.
8. Shot type 14 pose (step 75): all enemies stop for about 60 updates.
9. Zero: each skill (bits `0x04`, `0x08`, `0x10`, `0x20`, `0x80`) and its button and direction.

## Open items

- (answered: the armour layer of member 86, black; play check 3) How the Ultimate Armor looks.
- A state in forms 2 and 4, to check member 88 and the Ultimate layer in play. Member 89 is
  checked (`yammark-4_blade-armor`): the rule "frames 10, sprites 18" holds, set 1 has 76 frames.
- The effect of form 2 at the beam-in (effect type 27 var 5, palette 2 in slots 24 and 25).
- The draw order of the two layers in the order table (the look says armour on top).
- Zero's step bodies 48–63 in detail (timings, speeds of each technique).
- Who sets `act4` for the poses of steps 44 and 75 (the weapon object, likely).
