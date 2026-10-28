# Gate (stage 24, main object type 59): behaviour sheet

Gate, the scientist who made the Nightmare, is the boss of section 2 of Gate stage 2 (stage 18). He
does not take damage from weapons. He summons coloured orbs; when the player destroys an orb, it
breaks into six fragments, and a fragment that flies into Gate costs him 4 HP. A direct hit on Gate
makes his orbs dissolve. He also dashes at the player and raises a column of energy that breaks the
platforms of the room.

Overlay 24 (stage 18, area 0; no boss overlay is loaded: the main table `0x800F5F88` points into the
stage overlay, which ends at `0x800F780A`). Save states: `gate-4_before-gate` (the corridor before
the room, Gate not live) and `gate-5_gate-fight` (the fight has started). Kit:
`python3 -m mmxdemake.rekit --state gate-5_gate-fight.state.gz --type 59` (output in
[`~/mmx-data/x6/re/gate-stage-3/kits/main-59/`](../../work/gate-stage-3/kits/main-59/)). The shared rules are in
[`docs/engine.md`](../engine.md); the room and the records are in
[`docs/stages/gate-2b.md`](../stages/gate-2b.md).

Marks: [certain for the code] = read in the code, not seen in a state; [live] = seen with the same
value in `gate-5_gate-fight`.

## Spawn and set-up (`0x800EF400`, state 0)

- Record 64 (main 59, `id` 0) at (3312, 1056). The set-up moves Gate to (3456, 912), above the
  room [certain for the code].
- HP 0 at the set-up; the intro fills the bar to `boss_hp_max` (`game+0x8C`) = 48 + {4, 6, 8,
  10}[boss level] (`0x800E98B8`): **52, 54, 56, 58** for boss level 0–3 [certain for the code].
  `gate-5`: boss level (`game+0x8F`) 2, `game+0x8C` = 56, HP 56 [live].
- Contact damage 6 [live]. Attack box and hurt box (−12, −24, 24, 48) (`0x800F6F2C`,
  `0x800F6F30`) [live for the pointers; certain for the data].
- Damage table `dmg_tbl_gate` (`0x8007573C`) [live]. Draw priority 5, layer = `mega.layer`.
- `game+0x20` (the boss) = Gate [live]; `game+0x25` = 0; `BossLevel()`; `PlayerEvent(20, 64)`.
- Clears the shared globals of the fight (table below) and sets `gate_obj` (`0x800F7800`) = Gate
  [live].
- Texture `0x0140` and the palette of his own bank (type list bank 14) [live]. Animations 31–33
  use the next bank (15) with texture `0x0540` (`sets` rule, below).

### Shared globals of the fight (overlay 24)

| address | name (proposed) | meaning |
|---|---|---|
| `0x800F77D8` | `gate_dead` | 1 from Gate's death: every orb, fragment and shot deletes itself |
| `0x800F77DC` | `gate_hit` | 1 when a fragment touches Gate; Gate then loses 4 HP and goes to step 8; every orb, fragment and shot deletes itself; step 8 clears it after 90 updates |
| `0x800F77E4` | `gate_dash_count` | dashes since the last reset |
| `0x800F77E8` | `gate_slow` | 1 while the red orb (`var` 3) works: on odd frames the stop flags `game+0x10`, `+0x11`, `+0x13` are 1 |
| `0x800F77EC` | `gate_orbs` | live orbs (`var` 0–4); `gate-5`: 1 [live] |
| `0x800F77F0`, `0x800F77F4` | `gate_red_live`, `gate_blue_live` | a red (`var` 3) or blue (`var` 4) orb is live |
| `0x800F77F8`, `+0x2`, `+0x4` | `gate_column`, `_x`, `_y` | the energy column is active, and its position |
| `0x800F7800` | `gate_obj` | pointer to Gate [live] |
| `0x800F7806` | `gate_damage` | the damage of the last weapon hit on Gate (old HP − new HP) |
| `0x800F7808` | `gate_frame` | a frame counter of Gate's main state |

## The `id` parameter

Not used [certain for the code].

## State machine

State table `0x800F75F0`: 0 set-up `0x800EF400`, 1 main `0x800EF590`, 2 death `0x800F50E4`.

### Main (`0x800EF590`, state 1), each update

1. `ContactWeapon`. For type 59 the engine lowers the HP, makes the shot bounce off and returns
   127 (`docs/damage.md`, step 9). Gate writes the old HP back and puts old − new in
   `gate_damage` [certain for the code]. **Weapons never cost Gate HP.**
2. If `gate_hit` ≠ 0 and the step is not 8: HP − 4. HP ≤ 0: Gate goes to state 2 (death),
   animation 7, `mega+0x7A` = 1 (the player cannot be hit), `gate_dead` = 1. Else step 8 (hurt).
   [certain for the code]
3. Else, when the step is not 8: `ContactMega` (contact damage 6).
4. Flash timer `+0x84`: while it is not 0 it counts down and toggles palette bit 15 every 2
   updates; at 0 the damage table goes back to `dmg_tbl_gate`.
5. Slow motion: when `gate_slow` & `gate_frame` & 1: the stop flags `game+0x10`, `+0x11` and
   `+0x13` = 1, else 0. So those classes run at half speed while the red orb works [certain for
   the code; which classes are +0x11 and +0x13 is in `docs/engine.md`, "Game state"].
6. The step (table `0x800F75FC`): 0 `HitReactionDispatch`, 1 none, 2 intro, 3 idle, 4 choice,
   5 dash, 6 summon, 7 column, 8 hurt.

### Step 2: the intro (sub-steps, table `0x800F7620`) [certain for the code]

| sub | function | what |
|---|---|---|
| 0 | `0x800F4A94` | waits until the player is in step 20 (the event step); then `PlayerEvent(29, 64)`, limit speed (`cam+0x47`) 2, a 40-update wait and engine object 24 at Gate (the same frame as the other bosses, `docs/engine.md`, "Boss sequence") |
| 1 | `0x800EF820` | after 40 updates `PlayerEvent(20, 64)`; when object 24 has ended: speed y −1 px/frame (down), faces the player, animation 0 |
| 2 | `0x800EF95C` | falls 1 px/frame until y ≥ 1112 (200 updates from y 912); animation 1, stop |
| 3 | `0x800F4B2C` | at the end of animation 1: `TextOpen(1, 255, game+0x2F)`, then `game+0x2F` = 1. So the talk shows only the first time; after a retry `game+0x2F` = 1 and `TextOpen` makes a 60-update wait with no text (`TextOpen` note) |
| 4 | `0x800F4BB0` | when the text box is closed: `CopyTiles(0, 3200, 1136, 0x800F7588)` (closes part of the room floor [guess]) and six visual 26 `var` 15–20 (the platforms, below) |
| 5 | `0x800EFA58` | every 3 updates `fade_r` − 1, `fade_g` − 32, `fade_b` − `0x400` until `fade_r` = 0: it undoes the fade that boss gate 31 made (`docs/enemies/boss-gate.md`, FADE); `PaletteFxClear(0)`; animation 2 |
| 6 | `0x800EFBB4` | animation 2 (he opens his cape); on its flag frame `PlaySound(2, 137)`; at its end animation 3, `BossMusicRequest`, `game.boss_bar` = 1 |
| 7 | `0x800F4C74` | waits for the boss music; then HP + 1 per update up to `boss_hp_max`, sound (0, 14) every 3 updates; then `PlayerEventEnd`, step 4 |

`gate-5`: Gate at (3456, 1112), `game+0x2F` = 1, `game.boss_bar` = 1, the six platforms live at
the table positions [live].

### Step 4: the choice (`0x800EFDCC`) [certain for the code]

r = `GetRNG() & 15`; cap = {1, 2, 2, 3}[boss level] (`0x800E98BC`); half = `boss_hp_max` / 2.

- `gate_orbs` < cap, and no red or blue orb is live:
  - HP > half: r < 8 → pattern 0, else pattern 1;
  - HP ≤ half: r < 5 → pattern 0, r < 10 → pattern 1, else pattern 2;
  - `gate_dash_count` = 0.
- Else: pattern 3 while `gate_dash_count` < 4 (< 3 when HP ≤ half); else pattern 4 and
  `gate_dash_count` = 0.

Patterns (table `0x800F7528`, 6 steps per pattern; each move takes the next entry when it ends):

| pattern | steps |
|---|---|
| 0 | 6 summon, then 4 |
| 1 | 5 dash, then 4 |
| 2 | 5 dash, 7 column, then 4 |
| 3 | 5 dash, 3 idle (100 updates), then 4 |
| 4 | 5 dash, 7 column, then 4 |

`gate-5`: step 6 sub-step 1, `+0x86` = 0 (pattern 0), `+0x87` = 1 [live].

### Step 3: idle (`0x800EFCDC`, `0x800F4DB8`)

Faces the player, animation 3, waits 100 updates, then the next step of the pattern.

### Step 5: dash (`0x800EFFA4`, `0x800F0098`, `0x800F02AC`)

1. Faces the player, animation 3, waits 90 updates (he floats in place).
2. Voice: `PlaySound(2, 34)` for r < 3, `(2, 35)` for r 3–5 (r = `GetRNG() & 15`). Takes the
   player's position at that update as the target and flies straight to it at {2, 2.5, 3, 3}
   px/frame for boss level 0–3 (`0x800E98C0` × `rcos`/`rsin`).
3. Stops on each axis at the target; when both are reached, `gate_dash_count` + 1, next step.

He does not touch walls (no terrain box after the intro); the target is always the player's
position, so it is inside the room [certain for the code].

### Step 6: summon (`0x800F03BC`, `0x800F04CC`)

1. Faces the player, `PlaySound(2, 36)` with chance 1/4, animation 6.
2. First update: k = `GetRNG() % 5` (in `+0x07`); k = 3 becomes 2 and k = 4 becomes 1 when an orb
   is already live. A charge effect, visual 26 `var` 30 + k, follows Gate for 50 updates.
3. Update 22: the orb, visual 26 `var` k, 24 px in front of Gate and 14 px up; `PlaySound(2, 138)`.
4. Update 50: next step.

### Step 7: energy column (`0x800F0654`, `0x800F4F44`)

1. Faces the player, `PlaySound(2, 37)` or `(2, 38)` (1/2 each), animation 4.
2. Update 23: visual 26 `var` 14 (the column) 40 px in front of Gate, at his y.
3. Update 90: next step.

### Step 8: hurt (`0x800F0778`, `0x800F5070`)

`PlaySound(2, 33)`, animation 7, speeds 0, damage table `dmg_tbl_deflect`. The flash timer is held
at 100 while it is below 80. After 90 updates: flash timer 40, `gate_hit` = 0, then the next step
of the current pattern. One fragment hit = one step 8, so Gate loses at most 4 HP per 90 updates
[certain for the code].

## Hits to kill

4 HP per hurt: **13, 14, 14, 15 fragment hits** for boss level 0–3 (52, 54, 56, 58 HP)
[certain for the code]. `gate-5` (level 2): 14.

## Animations (table `0x800F74A0`, 34 animations)

| # | what | user |
|---|---|---|
| 0 | falls (cape closed) | intro sub 1 |
| 1 | lands | intro sub 2 |
| 2 | opens the cape (flag frame: sound 2, 137) | intro sub 5–6 [live in `gate-5`] |
| 3 | floats | idle, dash, summon wait |
| 4 | casts the column | step 7 |
| 5 | the column (magenta, grows) | visual 26 `var` 14 |
| 6 | summons | step 6 |
| 7 | hurt / death flash | step 8, death |
| 8 | defeated (one sprite) | death sub 2 |
| 9 / 10 | orb appears / orb blinks | `var` 0 (yellow) |
| 11 | fragment | `var` 5–10 of a `var` 0 orb |
| 12 | charge effect | `var` 30 |
| 13 | orb shot | `var` 12 |
| 14–17 | the same for the purple orb | `var` 1, its fragments, `var` 31 |
| 18–21 | the same for the green orb | `var` 2, `var` 32 |
| 22–25 | the same for the red orb | `var` 3, `var` 33 |
| 26–29 | the same for the blue orb | `var` 4, `var` 34 |
| 30 | small sparks | visual 26 `var` 11 (an orb hit that did not kill) |
| 31 | the small creature appears | `var` 13, bank 15 |
| 32 | the small creature flies | `var` 13, bank 15 |
| 33 | the platform block | `var` 15–20, bank 15 |

Animations 31–33 use the next bank: the code takes bank + 1, texture `0x0540` and the palette of
bank + 1 for every animation number ≥ 31 (`0x800F3348`, `0x800F3BFC`, `0x800F43B8`) [certain for
the code; the export with the rule below looks right by eye].

## Shots and effects (visual type 26, entry `0x800F0A24`)

The entry runs `CornerCheck` unless `+0x84` or `+0x70` is set, then dispatches by `var` (switch
`0x800E98C8`, 35 cases; `var` 21–29 do nothing). All parts copy Gate's animation table and layer.

### The orbs (`var` 0–4)

| `var` | colour | animations | after it arrives [certain for the code] |
|---|---|---|---|
| 0 | yellow | 9, 10 | every 90 updates a shot (`var` 12) at the player |
| 1 | purple | 14, 15 | every 180 updates a small creature (`var` 13), {3, 4, 5, 5} in all for boss level 0–3 (`0x800E99AC`) |
| 2 | green | 18, 19 | drifts at 0.5 px/frame toward the player and follows the player's y at 1 px/frame; it turns 30 updates after it passes the player. A hit by weapon id 24, 25 or 26 (Zero's sabre) pushes it away at 1 px/frame for 32 updates, then it waits 60 |
| 3 | red | 22, 23 | 60 updates after it arrives: `gate_slow` = 1 for 180 updates (half-speed frames, above), then it fades away. Its death also ends the slow motion |
| 4 | blue | 26, 27 | after 180 updates it pulls the player: player x ± 0.75 px/frame toward the orb, while the player has HP |

Common to all five [certain for the code; `var` 2 live in `gate-5`: HP 8, contact 4,
`dmg_tbl_deflect`, step 0, `+0x8C` = 6]:

- Set-up: 24 px in front of Gate, 14 px up; HP 8, contact 4, `dmg_tbl_deflect`, draw priority 4;
  `gate_orbs` + 1 (red: also `gate_red_live` = 1; blue: `gate_blue_live` = 1).
- Step 0: after 12 updates, boxes `0x800F76A4` and `0x800F76A8` = (−16, −16, 32, 32); it flies
  at 3 px/frame to the player's position of that update.
- Step 1: at the target, a visual 26 `var` 11 (sparks that show when the orb is hit but not
  killed) and damage table `dmg_tbl_gate`; it can now be hurt.
- Step 2: faces the player for 60 updates, animation 10 (15, 19, 23, 27): the blink.
- Each update: `ContactWeapon`, then `ContactMega`. When `gate_damage` added up in `+0x98`
  reaches 16 (16 damage dealt to Gate himself while the orb lives), the orb loses its boxes,
  blinks for 60 updates and goes away without fragments.
- Killed (HP ≤ 0): `PlaySound(2, 139)`, an explosion, and **six fragments** (`var` 5–10) at its
  position; then it fades for 20 updates and `gate_orbs` − 1.
- `gate_hit` or `gate_dead`: it fades away at once.

### Fragments (`var` 5–10, `0x800F3348`, `0x800F3660`) [certain for the code]

- Animation by the parent's `var` (`0x800F775C` = 11, 16, 20, 24, 28). HP 4, contact 4,
  `dmg_tbl_gate`, attack box (−11, −11, 22, 22) (`0x800F76AC`), hurt box `0x800F76B0`.
- Speed 3 px/frame at angle 157.5°, 180°, 202.5° (`var` 5–7) or 22.5°, 0°, −22.5° (`var`
  8–10): three to each side.
- Each update (from the first): if Gate is within 24 px in x and 32 px in y and Gate's step is not
  7, `gate_hit` = 1 and the fragment ends. **This is the only way to hurt Gate.** While he raises the
  column (step 7) the fragments fly through him.
- From the 5th update a weapon can destroy a fragment (an explosion, no hit on Gate).
- 60 updates, then no boxes and 80 updates of blinking, then it is deleted.

### Orb shot (`var` 12, `0x800F3A0C`, `0x800F56A4`)

Animation 13, aimed at the player at 4 px/frame, HP 1, contact 4, `dmg_tbl_gate`, box
(−6, −6, 12, 12), `PlaySound(2, 140)`. It can be shot down; it ends on a hit on the player or
after 150 updates [certain for the code].

### Small creature (`var` 13, `0x800F3BFC`, `0x800F3D74`)

Animation 31 (appears), at the orb, 12 px up; HP 2, contact 4, `dmg_tbl_gate`. After 30 updates
animation 32, box (−6, −6, 18, 18), 0.5 px/frame toward the player and 1 px/frame toward the
player's y; it turns 30 updates after it passes the player. It ends when shot or when it touches
the player [certain for the code].

### Energy column (`var` 14, `0x800F407C`, `0x800F4240`)

Animation 5, 40 px in front of Gate; contact 8; `PlaySound(2, 141)`. Update 5: `Rumble(3)`.
Updates 10–51: attack box (−16, −72, 32, 144) (`0x800F76C8`) and `gate_column` = 1 at its
position. At the end of the animation it is deleted. It has no `ContactWeapon`: it cannot be
shot [certain for the code].

### Platforms (`var` 15–20, `0x800F43B8`, `0x800F4560`)

Six blocks made by the intro at (3264, 1144), (3344, 1160), (3456, 1144), (3392, 1080),
(3296, 1000), (3424, 968) (`0x800F7768`) [live: the six objects at these positions]. Each
writes its solid tiles with `CopyTiles` (source table `0x800F7518`) and draws animation 33.
When the active column is within 24 px in x and 96 px in y: an explosion; 20 updates later the
tiles are replaced (source `0x800F7548` + 8 × `var`; that the result is empty is a [guess]);
after 180 updates and 60 updates of blinking the tiles come back [certain for the code].

### Charge effects (`var` 30–34, `0x800F4790`, `0x800F57E4`)

Animation 12, 17, 21, 25, 29 (`0x800F7780`); they follow Gate for 50 updates; no boxes. `gate-5`:
`var` 32 live, `+0x8C` = 27 [live].

## Damage

`dmg_tbl_gate` (`0x8007573C`, read from the executable; `D` = deflected, `–` = no effect):

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 2 2 2 – 2 1 2 2 2 2 2 2 |
| 16–31 | 2 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 2 2 2 2 2 2 2 2 2 8 1 4 8 |
| 48–63 | 2 1 2 2 1 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

- On Gate the table only gives `gate_damage`; the HP comes back. The highest values: ids 17 and 44
  (the charged weapon of slot 8 and the Commander Yammark stun id, 8), 22, 47 and 57 (8), 26 (5).
  16 damage while an orb lives makes that orb vanish without fragments, so a strong hit on Gate
  works against the player [certain for the code].
- The orbs (after they arrive), fragments, shots and creatures use the same table; before they
  arrive the orbs use `dmg_tbl_deflect`.
- An orb (HP 8) dies to 8 lemons (id 0, 1 each), 2 sabre hits that include id 26 (5), or 1
  charged slot 8 shot (id 17, 8) [certain for the table; the part bonuses of `docs/damage.md`
  can add 1].

Damage to the player, before the difficulty scaling: Gate 6; orbs, fragments, shots, creatures 4;
the column 8 [certain for the code].

## Death (state 2, table `0x800F768C`) [certain for the code]

1. `0x800F5120`: waits until the player has HP and `mega+0xE0` = 0; `mega+0x7A` = 1;
   `PlaySound(2, 32)`; flash timers; `ClearPlayerShots`; `game.stop` (`+0x10`) = 1.
2. `0x800F51CC`: 127 updates of flashing that gets faster; then engine object 26
   (`BossExplosion`) at Gate.
3. `0x800F0884`: while it runs, Gate flashes; when the screen is white (`+0x07` = 1 of the
   explosion) and `game+0x10` ≠ 0: the player goes to (3264, 1104) facing right,
   `PlayerSetTerrainBox`, `game+0x10` = 0, `PlayerEvent(20)`; Gate goes to (3344, 1152) with
   animation 8 (lying).
4. `0x800F52CC`: 120 updates (blinks from update 60).
5. `0x800F5344`: 16 updates of shaking: x = x0 + 24, −24, 32, −32 … 80, −80 (`0x800F766C`).
6. `0x800F53C8`: `game.exit_req` = 1 and Gate deletes itself.

No Nightmare Virus soul and no drop. `StageExit` then sends a stage 18 clear to game mode 19 with
scene 9 (`game+0x1D` = 9) and `game+0x66` = 5 [certain for the code; `docs/stages/gate-2.md`,
"Stage end"]. Scene 9 loads scene overlay 55 (46 + 9) at `0x800FA000`: a short scene of text
lines (ids 431–447 for X, 452–470 for Zero, picked by `game+0x38`) whose last step writes 11 to
byte 0 of its object, which is game mode 11 (the report) if the object is `game` [likely]. Where
the Sigma fight comes after that is not read: overlay 24 has no other boss code (its live main
types are 2, 8, 9, 14, 28, 59 and the main-program types 15 and 47) [certain for the main table].

## Sounds

Bank 2: 137 (cape), 34 and 35 (dash voices), 36 (summon voice), 138 (orb appears), 37 and 38
(column voices), 141 (column), 33 (hurt voice), 32 (death), 139 (orb breaks), 140 (orb shot).
Bank 0: 14 (HP bar fill). Boss music: track 26 (`boss_music[18]`).

## Talk (message 1 of members 225 and 226)

Zero's text (member 226, message 1, 21 pages): Gate: "So impressive. All of my Reploids have
already been destroyed. I confess that I have nothing to use... I couldn't analyze you enough...
Or should I say it was impossible to analyze you completely..." Zero: "It is too dangerous to put
my DNA to use! You must have been aware of that. Delete all the remaining program you have!" Gate:
"Not yet. Just give me one more chance to fight you! I've never thought I'd be able to fight the
original one... This is a long-awaited opportunity! Fight me, Zero." Zero: "It is useless to fight
you anymore! Just give up and surrender." Gate: "I won't. I am so close... Prepare yourself,
Zero!" Member 225 (X) does not decode with `mmxdemake.text` [certain for the run].

## Summary for a reimplementation

- Boss with HP 48 + {4, 6, 8, 10}[level]; weapons only bounce off. The HP goes down 4 per
  fragment that touches him (not during his column cast), with 90 updates of hurt after each.
- He loops: choose → summon one of five orbs, or dash at the player's position, optionally then
  an energy column or a 100-update rest. More orbs at higher levels (cap 1, 2, 2, 3); patterns
  with the column get more likely below half HP.
- Orbs: HP 8, fly to the player's position, then shoot (yellow), spawn creatures (purple), drift
  (green), slow the game (red) or pull the player (blue). Killed, each makes six 3 px/frame
  fragments in two fans of three.
- 16 damage dealt to Gate himself while an orb lives makes that orb vanish.
- The column (contact 8, 32 × 144 box) breaks the six platforms for 240 updates.
- Death: flash, boss explosion, the player and Gate are placed, a shake, then the stage exit.

## Open questions and what a later state must check

1. A state in step 8 (after a fragment hit): HP 4 lower, `gate_hit` = 1, every orb part gone.
2. A state with a red orb working: the stop flag pattern on odd frames, and which objects slow.
3. A state with a blue orb pulling, and one with the purple orb's creatures (animations 31–32).
4. A state after a column: the platform tiles that `0x800F7548` writes (empty or broken), and
   the floor that the intro `CopyTiles` changes (x 3200, y 1136).
5. The death and what follows the scene (the Sigma stage): a state after the fight.
6. X's version: member 225 does not decode; whether X sees the same talk.
7. Which object loads the boss pack (member 137): `boss_pack_loaded` is 0 in `gate-4` and 1 in
   `gate-5`; Gate's code does not call `BossPackLoad`.
