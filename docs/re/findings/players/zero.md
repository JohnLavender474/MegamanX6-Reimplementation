# Zero: sabre, techniques and moves (steps 48–63): behaviour sheet

Part of the player RE plan. This sheet gives Zero's own steps 48–63, the weapon objects in
Zero's overlay, the sabre combo, the techniques ("skill bits") and which Maverick gives each,
Zero's air moves, the stop pose and idle, and the Black Zero form. The shared moves are in
`docs/players/movement.md` and `air.md`; the step map is in `docs/players/map.md`; the list of
steps 32–77 is in `docs/players/steps-extra.md`.

Sources: the Zero player kit (`python3 -m mmxdemake.rekit --player --state
turtloid-6_zero-player.state.gz`), single functions with `rekit --at`, and the states
`turtloid-6_zero-player`, `turtloid-7_boss`, `turtloid-8_high-max`, `scaravich-1_start`,
`scaravich-2_totem-section`, `heatnix-3_rekkoha` (Rekkoha in progress). Notes, scripts, pseudo
code and reconstructed C are in [`~/mmx-data/x6/re/zero/attacks/`](../../work/zero/attacks/) and, for the Rekkoha,
[`~/mmx-data/x6/re/zero-rekkoha/`](../../work/zero-rekkoha/). Units and tags as in `docs/engine.md` ("Conventions"): speeds
in px per update, times in updates (60 per second), a positive `speed_y` is up. Update 0 is the
update of the button press.

## Buttons and fields

| bit | button (`mega+0x7C` held, `+0x80` pressed) |
|---|---|
| `0x10` | attack: the Z-Saber |
| `0x20` | special: Z-Buster, Yammar Option (up), Ensuizan (down) |
| `0x40` | giga: Rekkoha |
| `0x80` | jump; `0x100` dash; `0x200`/`0x400` select the energy gauge (L/R, likely) |

[certain for the tests; the button names from what each test starts, likely]

| field | use for Zero [certain for the code] |
|---|---|
| `mega+0x8E` | "busy": an attack runs; no new attack starts while it is 1 |
| `mega+0xCB` | the hit number of the combo (0, 1, 2); the spin count of Ensuizan; the burst flag of Sentsuizan |
| `mega+0xC9` | the technique bits (= `game+0x60`, below) |
| `mega+0x93` | the selected energy gauge: 0 none, 1 Hyoroga, 2 Rakukojin, 3 Yammar Option, 4 Guard Shell, 5 Rekkoha |
| `mega+0xA8 + 2n` | energy of gauge n; full = `+0x14A` = 6 × `game+0x62` (Zero's own upgrade byte; `docs/players/weapons.md`): 288 in the first five states, 312 in `gate-1_before-boss` |
| `mega+0xA6` | 1 Yammar Option out, 2 Guard Shell out |
| `mega+0x86` | 6 after the double jump or the super jump (no air dash, no second double jump; the air slash is the spin slash); 8 after Ensuizan |

## Technique bits: `game+0x60`

**`game+0x60` is one byte with three uses** [certain for the code]:

- the beaten Mavericks: bit n is the Maverick of stage n + 1 (`docs/engine.md` saw `0x01` →
  `0x21` → `0x29` at the clears of stages 6 and 4);
- X's owned special weapons: `PlayerLoadForm` (`0x8003D718`) copies it to `mega+0xC9`, except in
  form 2 (`mega+0xC9` = 0);
- Zero's techniques: the same copy, with no character test.

So `docs/engine.md` is right on both counts; it is not a conflict. Both characters share the byte:
a Maverick beaten by X also gives Zero its technique [certain for the code]. All five states have
`mega+0xC9` = `game+0x60` (`0x01`, `0x01`, `0x21`, `0x69`, `0x69`) [certain]. `0x69` (Scaravich
states) = stages 1, 4, 6, 7.

| bit | stage | Maverick | technique (official name) | input | step / object |
|---|---|---|---|---|---|
| `0x01` | 1 | Commander Yammark | Yammar Option | up + special, on the ground | weapon object type 1 (main program) |
| `0x02` | 2 | Blizzard Wolfang | Hyoroga | up + jump on the ground (super jump), then attack on the ceiling | step 31 (`air.md`), step 63, type 33 |
| `0x04` | 3 | Blaze Heatnix | Shoenzan | up + attack, on the ground | step 55, type 34 |
| `0x08` | 4 | Metal Shark Player | Rakukojin | down + attack while falling | step 56, types 35 and 4 |
| `0x10` | 5 | Ground Scaravich | Sentsuizan | up + attack in the air | step 57, type 36 |
| `0x20` | 6 | Rainy Turtloid | Ensuizan | down + special, anywhere | step 58, type 42 |
| `0x40` | 7 | Shield Sheldon | Guard Shell | select gauge 4 in the pause menu | weapon object type 7 (main program) |
| `0x80` | 8 | Infinity Mijinion | Rekkoha | giga, on the ground | step 59, type 44 |

Tags: the bit tests, inputs and steps are [certain for the code]. The stage numbers 1, 4, 5, 6, 7
are known from the stage overlays. 2 (Wolfang), 3 (Heatnix), 8 (Mijinion) follow from two
matches [likely, close to certain]: the code against the wiki descriptions (super jump and
ceiling = Hyoroga, up + attack fire slash = Shoenzan, giga = Rekkoha), and the boss stun ids of
`docs/engine.md`: each Maverick's stun list is (slot n, slot n + 9, one Zero id), and the Zero id
is always the technique of stage n: Rainy Turtloid 2 / 33 (icicle, Hyoroga), Shield Sheldon 4 /
35 (Rakukojin), Metal Shark Player 6 / 42 (Ensuizan), Commander Yammark 8 / 44 (Rekkoha beam). The technique names and their Mavericks are from the Mega Man
Knowledge Base (`megaman.miraheze.org`, "Zero's weaponry", "Rekkoha") and StrategyWiki ("Mega Man
X6/Zero's Techniques", inputs quoted in a search result) [certain for the sources; fan wikis].

## Steps 48–63

| step | name [likely] | entered from | input | animation | hit object |
|---|---|---|---|---|---|
| 48 | ground combo | stand, walk start, walk, dash, stop pose, land | attack | 88, 89, 90 (92 after a landing) | 24, 25, 26 (28) |
| 49 | jump slash | jump, wall kick | attack | 91 | 27 |
| 50 | fall slash | fall, wall push-off, air dash, 49 | attack | 91 | 27 |
| 51 | ladder slash | ladder up, ladder down | attack | 93 | 30 |
| 52 | wall slash | wall slide | attack | 94 | 31 |
| 53 | spin slash, rising | as 49, with `+0x86` = 6 | attack | 96 | 29 |
| 54 | spin slash, falling | as 50, with `+0x86` = 6 | attack | 96 | 29 |
| 55 | Shoenzan | ground steps, crouch, step 48 | up + attack | 90 | 26 (no box), 34 |
| 56 | Rakukojin | fall, push-off, air dash, 50, 54 | down + attack | 98, 99 | 35, then type 4 |
| 57 | Sentsuizan | jump, fall | up + attack | 101, 102 | 36 |
| 58 | Ensuizan | most steps | down + special | 103 | 42 |
| 59 | Rekkoha | ground steps, 48, 56 | giga | 104, 111 | 44 |
| 60 | crouch slash | crouch | attack (down held) | 105 | 37 |
| 61 | hang slash | rope, bar | attack | 106 (rope), 107 + n (bar) | 38 + frame flag (38 in all animations read) |
| 62 | Z-Buster | ground steps, crouch | special | 110 | type 58 |
| 63 | Hyoroga attack | ceiling hold (step 31 sub-step 3) | attack | 100 | 48, then 3 × type 33 |

[certain for the code.] 49 and 53, and 50 and 54, are the same routines. **Steps 53 and 54 run
after the double jump too, not only after the super jump**: both set `+0x86` = 6 [certain for the
code].

The sabre and technique tries, in the stand step (`0x80035274`): requests, ladder, Shoenzan,
Ensuizan, Rekkoha, super jump, dash or jump, sabre, Yammar Option, Z-Buster, crouch, walk, idle
[certain]. So up + attack is Shoenzan (not the sabre) when bit `0x04` is set, and the special
button is Yammar Option with up, Ensuizan with down, else the Z-Buster.

## The Z-Saber

### How a hit works

A sabre attack is a weapon object of the weapon id (`SpawnSabre`, `0x801ECD90`): flag 1, `var` =
the combo number, `+0x84` = Zero's animation, Zero's facing [certain]. Most steps make it at the
frame of Zero's animation with flag `0x80` (`0x801ECD54`). All ids 24–31, 35–42 and 48 run one
routine, `0x801ED2A8` [certain]:

- **Position**: it copies Zero's x, y and facing each update.
- **Animation and box**: animation `sabre_tbl[id − 24]` (`0x801EE68C`) of Zero's table. The
  hit box is the box of index = the object's frame flag in `0x801EE414` (91 boxes, `{ox, oy, w,
  h}`; `docs/engine.md` "Boxes"). Flag 0 = no box.
- **Deleted** when Zero's animation changes, Zero is locked, dies or has HP ≤ 0, Zero turns (ids
  24–26), or a new Ensuizan spin starts (id 42); else at the hold frame of its animation. So a
  cancel ends the hit at once [certain].
- **Hit tag** (one hit per tag, `docs/engine.md` "Hit tag"): ids 24, 25, 37–41: one tag for the
  whole swing (one hit per enemy). Id 26: a new tag at each box change (up to 7 hits). Ids 27–31,
  42, 48: the tag follows the box index, updated every 4th update. Ids 35, 36: tag 0; after each hit
  the box is off for 2 updates, at most 4 hits [certain for the code; the hit counts are a play
  check].
- **Saber parts** [certain for the code; names are guesses]: part `0x800000`: the box table
  `0x801EE580`, the same boxes 12 px longer in front, for ids 24–31, 37–41; and an after-image
  effect (effect type 3, var 16). Part `0x400000`: +1 damage for ids 24–31 and 37–41
  (`ApplyWeaponHit`) and sabre palette 4. Part `0x1000000`: sabre palette 5.

### The ground combo (step 48)

Three hits. Each press in the "next" window (frames with flag `0x20`) starts the next hit at once
(`0x801EB12C`) [certain]:

| hit | id | damage (`dmg_tbl_normal`) | Zero animation | next-hit window | free window (`0x40`) | end (stand) | box reach (front / back / y) | box live |
|---|---|---|---|---|---|---|---|---|
| 1 | 24 | 4 | 88 | updates 6–27 | 28–51 | 52 | 56 / 28 px, y −20…+12 | 19 updates |
| 2 | 25 | 6 | 89 | 8–30 | 31–54 | 55 | 63 / 33 px, y −34…+22 | 23 updates |
| 3 | 26 | 4 (up to 7 hits) | 90 | — | 31–54 | 55 | 74 / 22 px, y −63…+25 | 23 updates |

[certain for the frame data; "box live" counts the updates of the hit object with a box.]

- **First hit**: the press update shows frame 0; the hit object is made 1 update later (frame 1,
  flag `0x81`). Updates 0–1 (flag `0x01`) cannot be cancelled.
- **Dash and jump cancel** any hit from update 2 of hit 1 and from update 0 of hits 2 and 3
  (`TryDashJump`) [certain]. Crouch also cancels hits 1 and 2.
- **Free window** (flag `0x40`, hits 1 and 2 only): Rekkoha, a new attack press (starts again at
  hit 1, not the next hit), ladder, crouch and walk. Hit 3 has no free window: only dash or jump,
  or wait for the end [certain].
- **Turn-around hit**: in the frames with flag `0x10` (hit 1: updates 2–5; hit 2: updates 3–7),
  when the direction behind Zero is held, Zero turns and does hit 2 (id 25) to the new side, once
  per combo. No press is needed [certain for the code; play check].
- **Shoenzan** can start in place of hits 2 and 3 when up + attack is pressed (bit `0x04`).
- Sounds (bank 1) and voices by hit: (7, 3), (8, 4), (7, 5) (`0x801EE3BC`); the voice comes 1 time
  in 2 for hits 1 and 2 and always for hit 3 [certain].
- Zero does not move in step 48 (no move call): a slash from a walk or dash stops Zero [certain].

### Air, ladder, wall, crouch and hang

| step | id | damage | live box / reach | rules [certain for the code] |
|---|---|---|---|---|
| 49, 50 | 27 | 4 | 17 updates; 68 front, 19 back | hit at update 2; repeat with a new press after the animation (29 updates) or in its free frames (19–28); air control, ladder, wall and air dash still work; jump released → rise stops; when the rise ends, 49 goes on as 50 |
| 50 → land | 28 | 4 | 19 updates; 72 / 16 | landing before the free frames: the ground animation 92 at the same frame, step 48 (combo hit 1 rules); later: a plain landing |
| 53, 54 | 29 | 4 | 14 updates; 56 front and 56 back (all round) | as 49 / 50, animation 96 (18 updates) |
| 51 | 30 | 4 | 14 updates; 68 / 8 | animation 93 (20 updates); a new press in updates 16–19 repeats; faces the pressed side; then back to the ladder |
| 52 | 31 | 4 | 22 updates; 48 / 4 | animation 94 (27 updates); the slide goes on (`PlayerMove`); wall kick allowed; no wall → fall; release → push-off |
| 60 | 37 | 6 | 20 updates; 68 / 33, low (y −8…+29) | animation 105 (30 updates); dash or jump cancel at any time; then crouch |
| 61 | 38 | 4 | 21 updates; 55 / 20 | animation 106 or 107 (25 updates); jump press → jump; then back to the rope or bar |

## Techniques

Energy: gauge n costs `0x80073DF4[n]` × 6 (× 3 with part `0x20`, × 4 as Black Zero): 12 for
gauges 1–4, 72 for Rekkoha, of 288 [certain for the code]. So 24 uses of Hyoroga, Rakukojin,
Yammar Option and Guard Shell, and 4 of Rekkoha (8 with part `0x20`; MMKB: "4 uses normally, 8
with Energy Saver") [certain]. Rekkoha's gauge fills when Zero takes damage
(`docs/players/ladder-hurt-events.md`).

### Shoenzan (step 55, bit `0x04`)

Up + attack on the ground. Animation 90 (55 updates) with no move and no cancel [certain]. Two
objects: id 26 drawn with palette 8 (the fire blade) and no box, and the fire wave id 34
(`0x801EDAF4`): at Zero x ± 8 (front), y − 8; box (−48, −8, 48, 56) for 5 updates, then (−64,
−96, 64, 144) (a wall of fire 144 px high in front) for 16 updates; up to 8 hits, 4 updates
apart; 6 damage each [certain for the code]. Sounds (1, 7), (1, 22), voice 5.

### Rakukojin (step 56, bit `0x08`)

Down + attack **while falling** (the try is in the fall, push-off, air dash and fall-slash steps,
not in the rising jump) [certain].

1. Animation 98: Zero stops in the air for 13 updates; id 35 at update 2 (front 35 px, down to
   61 px below). Sound (1, 23), voice 5.
2. Dive: `speed_y` = −3.0, then the fall gravity 0.2578 to −6.5. Left or right held: 1.5 px per
   update. Ladder, wall and air dash can still stop it. Id 35 hits up to 4 times (8 damage).
3. Floor: animation 99, 4 explosions around (x ± 11, y + 19), and **with enough energy (12) a
   Metal Anchor**, weapon object type 4 (X's slot 4 weapon). With no energy: no anchor, the dive
   still works (MMKB: "without it, no anchors appear") [certain].
4. On the floor: dash or jump at once; from update 25 the step-48 free window; stand at 49.

### Sentsuizan (step 57, bit `0x10`)

Up + attack in the air (jump or fall) [certain]. Animation 101; `no_hit` = 1; 1 update still,
then **3.0 px forward and 5.0 px down per update** with no gravity and no control, until the
floor or the wall in front. Id 36 from update 2, up to 4 hits, 8 damage. At the floor: sound (1,
26), screen quake (`Quake(30, 2, 1)`), animation 102, a burst (id 36 var 1, animation 124: front
92 px, back 41 px), `no_hit` = 0, stand after 49 updates. At a wall: stand or fall at once
[certain]. MMKB: "shakes the screen on ground contact"; search result: "invincibility frames until
touching the ground".

### Ensuizan (step 58, bit `0x20`)

Down + special, on the ground, in the air, on a wall, ladder or hang [certain]. Animation 103: a
spin of 8 updates, **no move** (Zero stays in place in the air), `no_hit` = 1 for updates 0–4.
Id 42 boxes go all round (56 px front and back, 96 px high), 4 damage. In updates 5–7, down +
special again starts a new spin, up to 3 spins. It sets `+0x86` = 8: only once per air time (the
landing clears it) [certain for the code; MMKB/fan wiki: "only once in the air"].

### Rekkoha (step 59, bit `0x80`)

Giga button on the ground with 72 energy [certain]. Zero is locked (`game.player_lock`), cannot
be hit (`no_hit`), and has no terrain box for the whole attack. Voice 7 at the start. Sub-steps
(`rekkoha_steps`, `0x801EE39C`) [certain for the code; update numbers ±1]:

| update | what |
|---|---|
| 0 | animation 104; `PaletteFxReset`, channel 1 scale mode, `mega+0xE4` \|= 1 |
| 1–15 | `fade_r[1]` + 2 per update: the BG goes dark |
| 16, 17 | **white flash**: `fade_r[1]` = 31, `fade_mask[1]` saved (`0x801EF974`) and set to all 512 CLUTs, add mode (+31): the whole screen, sprites too, is white |
| 18 | mask written back, scale mode with 31: the BG CLUTs are transparent black; 10 backdrop objects (effect type 17) |
| ≈23 | animation 104 frame flag `0x80`: sound (1, 27) |
| ≈27 | frame flag `0x40`: sound (1, 28); the waves start |
| every 10 | a wave (below) until 11 beams are counted |
| +71 | animation 111; `fade_r[1]` − 8 per update (4 updates); at 0 `+0xE4` &= ~1 |
| hold frame of 111 (31 updates) | `+0x93` = 0 (the gauge selection goes back to none), busy = 0; unlock and `no_hit` = 0 (when `+0xD3` = 0); then stand or fall |

Total about 181 updates with Zero in the middle of the view [likely, from the timers].

- **Waves** (`RekkohaWave`, `0x801EAA24`): wave k makes one beam at Zero's x (k = 0) or two at
  x ∓ 32k. A beam outside the view (x < cam x − 16 or > cam x + 336) deletes itself and is not
  counted. With Zero in the middle: 6 waves (1 + 2 × 5 = 11 beams, the last pair at the view
  edges); near an edge: up to 10 waves [certain for the code].
- **Beam** (weapon id 44, `RekkohaBeam` `0x801EDFC8`): starts at cam y − 128, falls 8 px per
  update, deleted at cam y + 368 (62 updates). Box (−16, −128, 32 × 255) (`0x801EE3DC`). Hit tag
  + 1 every 8 frames (the global frame counter): one hit per 8 updates on a target. Each hit **or
  deflect** counts; after 4 the box is removed, but the beam goes on falling. Damage 8 in
  `dmg_tbl_normal` (other tables below). Hit sound (1, 9), kill (1, 10); spark variant 7.
- **Top part** (effect type 26, `RekkohaBeamTop` `0x801EE254`): made with each beam at the same x,
  160 px higher (cam y − 288), the same speed, no box; deleted at cam y + 400.
- **Backdrop** (effect type 17, `GigaBackdrop` `0x8004AE1C`): 5 rows of 2 sprites (83: 256 × 64
  at screen x 128; 84: 64 × 64 at screen x 384, which fills x 256–319), a red and yellow streak
  curtain over the full view. It scrolls up 8 px per update (wraps at the view top to 320). It
  deletes itself when `fade_r[1]` ≠ 31 or Zero leaves step 59. Priority 55.

Id 44 in the other damage tables: 8 for `nightmare_insect`, `nightmare_snake`, `yammark` (his
stun id), `sabre_only`, `reflector`; 4 `minor`; 2 `d1000`; 127 `one_hit`; deflected by
`deflect`, `illumina`, `nightmare_bug`, `sheldon`, `sheldon_guard`, `shark`, `wolfang`,
`mijinion`, `ice_block`, `grey_block`; no effect on `reflector_wait`, `nightmare_shadow`,
`capsule` [certain, read from the executable]. A deflect uses one of the beam's 4 hits.

Live check (`heatnix-3_rekkoha`, Zero in the middle of the view, step 59.4, `+0xE1` = 20)
[certain]: `act4` = 3 beams after the waves at `+0xE1` = 1 and 11; beam y = cam y − 128 + 8 ×
moves (504 after 19 updates, 424 after 9: the set-up of a weapon object runs in the update of its
spawn); hit tags 3, 2, 2; Nightmare Snake's gem took 2 × 8, its body deflected beam 0 three times
(`+0x8C` = 3); energy slot 5 = 216 (288 − 72); `mega+0xE4` = 1; channel 1 in scale mode at 31.

Sprites: Zero's animations 104 and 111 are in `players/zero/`. The beam (animation 11 of the
shared table `0x8007798C`: sprites 0–2, orange, blue and cyan, one update each) and the top part
(animation 12: sprites 3–5) use sprite data `scratch+0x1C` entry 15 and CLUT `0x7802`. The
backdrop uses `scratch+0x3C` entry 0, texture `0x1E00`, CLUT `0x7E50`. `spriteexport` does not
export them (`docs/TODO.md`); kit exports ([`~/mmx-data/x6/re/zero-rekkoha/scripts/rekkoha_export.py`](../../work/zero-rekkoha/scripts/rekkoha_export.py))
are in `sprites/enemies/rekkoha-beam/` and `sprites/enemies/rekkoha-backdrop/`.

### Hyoroga (bit `0x02`)

The super jump (up + jump on the ground, `air.md`: 151 px up, ceiling hold 120 updates for Zero).
On the ceiling, attack (with 12 energy; **no energy, no attack**) starts step 63: animation 100
(31 updates), id 48 at update 4, and at update 9 three icicles (type 33, `0x801ED928`): from Zero
x, y + 24, at 5 px per update in directions 22, 24, 26 (straight down and 22.5° to each side; not
mirrored), box 24 × 36, 3 damage, one hit; a wall or floor breaks them (sound (1, 21), 6 pieces).
Then back to the ceiling hold [certain for the code]. MMKB: "shooting icicles uses Weapon Energy;
leaping and air-dashing from the ceiling does not".

### Yammar Option (bit `0x01`) and Guard Shell (bit `0x40`)

Both make a main-program weapon object, the same as X's slot weapon [certain for the code]:

- Yammar Option: up + special on the ground (stand, walk, dash), when none is out (`+0xA6` ≠ 1)
  and 12 energy: weapon type 1, gauge 3. Zero does not stop.
- Guard Shell: only from the **pause menu**: when the menu closes with gauge 4 selected
  (`0x80034D78`), with none out (`+0xA6` ≠ 2) and 12 energy: weapon type 7. The tweak hack's
  default "Menu Activation" agrees (`mmx-reference`, `mmx6.tweaks.zero-techniques` manifest).

What the objects do is X's code: see `docs/players/weapons.md`.

### Z-Buster (step 62, no bit)

Special on the ground with no direction (or crouched) [certain]. All speeds 0, animation 110 (25
updates, no cancel). At update 3, a shot (type 58, `0x801EDCC8`) at Zero x ± 48, y − 6: it waits
6 updates, then flies at **8.75 px per update** (12.75 with part `0x100`), box 48 × 16, ends at the
first hit, at a wall (spark) or after **160 px** (it fades); Zero hurt → it is deleted. Damage 4
(+1 with part `0x80`, the Buster Plus list) [certain]. With `mega+0x144` = 6: four more shots, 3
updates apart [certain for the code; what sets `+0x144` is open]. Name: "Z-Buster" [likely, from
the game's other Zero sheets; not in the X6 technique lists].

## Air moves

- **Double jump** (`0x801EC298`, first in `TryAirDash`): jump pressed in the air while `+0x86` = 0:
  animation 95, sound (1, 1), voice 7, `speed_y` = 5.8125 (the jump speed), the jump gravity,
  `+0x86` = 6, step 6 [certain]. So **one double jump or one air dash per air time**, not both
  (both need `+0x86` = 0) [certain]. The air slash after it is the spin slash (step 53).
- **Air dash**: always available to Zero (no form test): `docs/players/air.md` (18 moves, 90.5 px;
  a sabre press ends it: fall slash with `speed` 0 and the fall gravity).
- **Super jump**: needs bit `0x02` (Hyoroga): `docs/players/air.md`.

## Stop pose and idle

- Step 5 (stop pose, animation 9: 8 updates): `docs/players/ladder-hurt-events.md`.
- Idle (`0x801EC9C4`): animation 5 (a 240-update loop); animation 6 (low HP) changes back to 5 when
  HP ≥ max HP / 3 (`game+0x5B + var`). No weapon pose [certain]. All five states stand in
  animation 5.

## Black Zero (`game+0x5F` bit `0x20`)

Given with the new-game cheat (`docs/players/steps-extra.md`). Effects [certain for the code]:

| what | where |
|---|---|
| +1 damage for ids 24–31 and 37–41 (as part `0x400000`) | `ApplyWeaponHit` `0x80031780` |
| damage taken / 2 (as X's armour forms and part `0x200`) | `PlayerDamageScale` `0x80030E78` |
| no hit knock-back (classes 1, 2, 4), as part `0x200` | `0x80035C58`, `0x80039740` |
| energy cost × 4 instead of × 6 | `0x8003EB64` |
| other palettes (the black body) | `PlayerPaletteRestore` `0x8003C63C` |
| sabre palette 4 | `0x801ED56C` |
| form index 6 (portrait or icon, guess) | `0x80021B40`, `0x800228AC` |

None of the five states has the bit (`game+0x5F` = `0x11`).

## Weapon objects of Zero's overlay

| type | function | what | damage |
|---|---|---|---|
| 24–26 | `0x801ED2A8` | ground combo hits 1–3 | 4, 6, 4 |
| 27, 28, 29 | `0x801ED2A8` | air slash, landing slash, spin slash | 4 |
| 30, 31 | `0x801ED2A8` | ladder, wall slash | 4 |
| 33 | `0x801ED928` | Hyoroga icicle | 3 |
| 34 | `0x801EDAF4` | Shoenzan fire wave | 6 |
| 35 | `0x801ED2A8` | Rakukojin blade | 8 |
| 36 | `0x801ED2A8` | Sentsuizan blade and burst | 8 |
| 37 | `0x801ED2A8` | crouch slash | 6 |
| 38–41 | `0x801ED2A8` | hang slash | 4 |
| 42 | `0x801ED2A8` | Ensuizan | 4 |
| 44 | `0x801EDFC8` | Rekkoha beam | 8 |
| 48 | `0x801ED2A8` | Hyoroga ceiling slash | 4 |
| 58 | `0x801EDCC8` | Z-Buster shot | 4 |

Damage from `dmg_tbl_normal`; other tables differ (`docs/engine.md`, "The damage tables")
[certain]. The engine sheet's "33–44" stun ids: 35 is Rakukojin and 42 Ensuizan (Metal Shark
Player's stun id 42 is Ensuizan: Turtloid's technique, as X's Turtloid weapon) [certain for the
ids]. `weapon_types` entries 32, 43, 45–47 point into X's overlay and are not Zero's.

## Play checks

1. Ground combo: three hits with three presses; a press late in a hit (free window) starts at
   hit 1 again; hit 3 can only be cancelled by dash or jump.
2. Turn-around: start hit 1, then hold back at once: Zero turns and does hit 2 behind, with no
   second press.
3. Hit 3 (and the air slash) hit a big enemy more than once; hits 1 and 2 hit only once.
4. Slash during a walk or a dash: Zero stops.
5. Land during an air slash: the swing goes on as the ground swing (animation 92).
6. Double jump: once; no air dash after it; the air slash after it spins (animation 96).
7. Shoenzan: up + attack replaces the plain slash; fire wall about 144 px high; Zero cannot move
   for about 55 updates.
8. Rakukojin: only while falling; 13-update stop, then the dive with left/right steering; anchor
   only with energy.
9. Sentsuizan: dive at 3 forward, 5 down; invincible until it lands; screen shake.
10. Ensuizan: Zero stays in place in the air; up to 3 spins; only once per jump.
11. Rekkoha: white flash, then the BG goes black behind a red curtain that scrolls up; 11 beams
    32 px apart, one wave every 10 updates (more waves near a view edge); Zero cannot be hit; at
    the end the gauge selection is back to none.
12. Hyoroga: icicles fan down (not mirrored by facing); no ceiling attack without energy.
13. Guard Shell starts only from the pause menu.
14. Z-Buster: shot range about 160 px; no cancel of the pose.
15. Black Zero (cheat): half damage, no knock-back, stronger sabre, 6 Rekkoha uses (288 / 48).

## Open items

- What sets `mega+0x144` (6: five buster shots; 2: +1 damage to all hits; 3: free energy).
- `mega+0xCD` (set when ids 26–31, 35, … hit): no reader found. (answered: `ContactWeapon` reads
  the sabre object's `+0x9C`; only a shot with `+0x9C` ≠ 0 can hit an object with
  `dmg_tbl_one_hit`, `docs/damage.md`.)
- Why the Sentsuizan object (type 36) gets a terrain box on some frames.
- Whether a weapon object made in the player step runs its first box test in the same update
  or the next one (the "box live" times above count from its first own update). (partly
  answered: its set-up runs in the same update, `heatnix-3_rekkoha`; its first move in the next.)
- The official names of the hit steps (49–54, 60–63) and of the double jump; the stage numbers of
  Wolfang (2), Heatnix (3) and Mijinion (8) from a stage overlay.
- A state in a technique (steps 48, 55–58) to test the timings. (step 59: tested with
  `heatnix-3_rekkoha`.)
- Rekkoha: what `mega+0xE0` (2 during step 59) and `mega+0xD3` do; who uploads the backdrop
  CLUT 416, which the dark mode does not clear.

## Sources

- Code: the Zero kit and the functions named above.
- Technique names and Mavericks: [MMKB, Zero's weaponry](https://megaman.miraheze.org/wiki/Zero%27s_weaponry),
  [MMKB, Rekkoha](https://megaman.miraheze.org/wiki/Rekkoha); inputs: StrategyWiki "Mega Man X6/Zero's
  Techniques" (via a search result; the page itself was not readable).
- Guard Shell menu activation: `mmx-reference/MegaManX6Recomp`, package `mmx6.tweaks.zero-techniques`
  (facts only).
