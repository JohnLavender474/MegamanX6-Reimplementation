# Illumina (stage 11, main object type 24): behaviour sheet

Illumina is the giant Reploid in the background of the first section of Infinity Mijinion's stage
(the Weapon Center). It does not walk through the stage: it is a picture fixed on the screen,
made of sprites in screen coordinates, so it "follows" the player while the camera moves. Two
energy **cables** with a red **lens** are drawn on BG layer 1 in the stage; the lens is the only
place that can be hurt. Illumina has two fights, one per cable, with 64 HP each. Between the
fights, and in the second fight, it sends three **drones** that fire converging lasers at a point
that its **head** locks on the player. In the second fight it also closes the screen sides with two
walls that carry eight laser **cannons**. On Hard and above it also drops orb enemies (main type 11).
Jet enemies (main type 8) come from the screen sides until the second fight.

Stage overlay `11` (`rock_bin/11.bin`, loaded at `0x800E9860`; stage 8, area 0) [certain: the
main table at `0x800FB830` has entry 24 = `0x800F20D4`]. Save states (in `~/mmx-data/x6/dumps/`):
`mijinion-1_start.state.gz` (checkpoint 0, Zero at (24, 186), camera (0, 16); phase 0, the robot
shoots) and `mijinion-2_illumina-helpers.state.gz` (Zero at (2439, 554), camera (2279, 384);
phase 2, the drones have locked their target on Zero). Written with the RE kit
(`python3 -m mmxdemake.rekit --state mijinion-2_illumina-helpers.state.gz --type 24`; more kits
for main 8, 11, 19, 28 and stage logic 5 and 27) and `docs/re-guide.md`. Units, confidence tags
and the shared rules: `docs/engine.md`. The name: "Illumina" [certain: the game text, member 219,
message 1, Alia: "A huge Reploid... I assume it's Illumina." and "There has to be cables which are
sending energy to Illumina!"; also Mijinion: "...say you destroyed my sweet Illumina..."].

The objects:

| class, type, `var` | object | made by |
|---|---|---|
| main 24, 0 | the **body**: the fight state, the HP, the lens boxes; not drawn | record 51 (kind 0, `id` 0, at (160, 128)) |
| main 24, 16 | the **head**: comes out of the robot and aims the scan beam; cannot be hit | the body, attack "drones" |
| main 24, 17, 18, 19 | the three **drones** (HP 16) | the body, attack "drones" |
| main 24, 32 | the **probe**: the end point of the scan beam (invisible) | the head |
| main 24, 64 | the **wall controller** (second fight) | the body, attack "walls" |
| main 24, 49–56 | the eight **wall cannons** (HP 20) | the wall controller |
| effect 11, many | the **pieces** of the robot picture (screen space) | the body, every attack |
| visual 28, 0 / 16 / 32 / 48 | zoom shot / laser hit point / wall hit box / cannon beam | body / drones / wall controller / cannons |
| ray 0, 0 / 1 / 16–19 | scan beam / its copies / drone lasers | head / beam / drones |
| main 8 | jet enemies (leader `var` 0 and 4 followers, `var` 16–19) | the body, phases 0–2 |
| main 11, 16 | orb enemies, two at the top corners | the body, attack "orbs" (Hard and above) |
| main 28, 1 | a teleporter crystal at (4384, 864): the way into the hidden area, stage 22 area 1 (`docs/stages/mijinion-alt.md`) | the body at its death, when fight 2 took < 5401 updates |

## Coordinates and drawing [certain]

- The body and all its pieces have `layer` (`+0x14`) = −1. `DrawObject4` (`0x800232D4`) then uses
  `x`, `y` as screen coordinates, without the camera. The body sets itself to **(160, 136)** every
  update, so the robot is fixed on the screen and seems to follow the player.
- `BoxOverlap` (`0x8002D654`) and `WeaponBoxTest` (`0x8002D868`) add the layer 0 camera to the x and
  y of an object with `layer` < 0. So screen-space boxes hit world objects normally.
- The robot picture is **effect 11** sprites (the head, the shoulders, the arms, the hands, the eye
  flash, the sparks), not a BG layer. BG layer 2 is a separate animated tunnel (mode 5, frames
  y 0 / 256 / 512, one per 16 updates; stage logic 5 `TunnelBgAnimator`, `0x800FB13C`).
- **The cables are BG layer 1 tiles** (layer 1 mode 2, with layer 0). Cable 1 is at world x
  2240–2384, y 368–640; cable 2 at x 4416–4560, y 752–1024. The lens is near (2318, 481) and
  (4494, 865). The body's attack and hurt box `0x800FC2B0` = (104, −46, 24, 26) is at screen x
  264–288, y 90–116; with the camera locked at (2048, 384) or (4224, 768) it covers the lens
  [certain for the boxes and the locks; likely that this is the lens: the tile drawings agree within
  6 px].
- At the end of each fight, `CopyTiles(1, 2240, 368, …)` / `CopyTiles(1, 4416, 752, …)` draws the
  broken cable (9 × 17 cells from page (2880, 0)).

## Spawn and set-up [certain]

- Record 51 of area 0: main 24 `id` 0 at (160, 128). It spawns at the stage start (screen 0, 0).
  The body never checks the screen edge, so it lives until its death.
- Body: HP 0, contact 4, no boxes, not drawn (`display` 0), priority 5, `dmg_tbl_illumina`.
  **Camera target max x = 2048**: the camera cannot go past x 2048 until the first fight ends.
  Jet timer `+0x84` = 120. Phase (`illumina_phase`, `0x800FD514`) 0. Step 2 (INTRO).
- All parts copy the body's table, sprites and palette. `var` 16–19 and 32, 48–56 take the sprite
  data, texture and palette of **bank + 1** (the drone bank).

## Phases (`IlluminaPhase`, `0x800F1BE8`, every update after the step) [certain]

| phase | while | change |
|---|---|---|
| 0 | the walk to the first cable | at camera x ≥ 2048: phase 1. Camera target min x 2048, min y = max y = 384 (old max y kept). HP fill. |
| 1 | **fight 1** | at HP 0: phase 2, `illumina_clear`, step 8 (cable 1 breaks) |
| 2 | the walk to the second cable | at camera x ≥ 4224: phase 3, fight timer 0. Camera target min x 4224, min y = max y = 768. HP fill. |
| 3 | **fight 2** | fight timer + 1 per update. At HP 0: **checkpoint 2**, `illumina_clear`, phase 4, step 9 (death) |

**HP fill** (`IlluminaHpFill`, `0x800F3D24`): the HP bar comes on (`game+0x24` = 1), HP 1, no
boxes; then **+1 HP per update** up to `boss_hp_max` = **64** (written in the intro), with the tick
(bank 0, id 14) every 3 updates. At 64 the lens boxes come on. So each fight has 64 HP; the bar
fills in about 1 s. The player can move during the fill.

**Jets** (`IlluminaJetSpawner`, `0x800F1DC4`): in phases 0 and 2 (not during the intro) a jet group
(main 8 `var` 0, which makes 4 followers) starts at camera + (336, 64), the right edge; in phase 1
at the left (−16, 64) or the right edge at random. A new group comes **120 updates after no main 8
is live**. No jets in fight 2.

## State machine

States `0x800FCA00`: 0 set-up, 1 body, 2 parts, 3 delete (`DeleteObject3`), 4 delete
(`DeleteObject2`). Body steps `0x800FCA14`: 0 hit reaction (not used), 1 nothing, 2 INTRO, 3 (not
in a pattern), 4 ZOOM SHOTS, 5 DRONES, 6 WALLS, 7 ORBS, 8 CABLE 1 BREAKS, 9 DEATH, 10 SHOCK.

### Body (state 1, `IlluminaBodyMain`) [certain]

1. `x, y` = (160, 136); `AnimeTick`; the step.
2. HP fill.
3. `ContactWeapon`. HP < 0 is set to 0. A hit by **weapon id 6, 15 or 42** starts SHOCK (step 10)
   and sets `illumina_clear`, unless SHOCK runs.
4. `ContactMega` (the lens box, contact 4).
5. `IlluminaPhase`.

### THINK (`IlluminaThink`, `0x800F0050`) [certain]

Patterns `illumina_patterns` (`0x800FC0B0`, 8 bytes per pattern, 16 per phase, `0xFF` ends):

| phase | patterns |
|---|---|
| 0, 1 | ZOOM SHOTS, always |
| 2 | DRONES, always |
| 3 | WALLS, then ZOOM SHOTS or DRONES (1/2 each), and in half of the patterns ORBS after it |

A pattern is picked at random (`GetRNG() & 15`) when it ends or when the phase changed.

### INTRO (step 2) [certain for the order; the palette steps are not read in full]

| sub | what |
|---|---|
| 0 | `PlayerEvent(20, 64)`: the player is held, facing right |
| 1 | when the player is in step 20: `game+0x24`, `+0x25` = 0, `game+0x20` = the body, `boss_hp_max` = 64 |
| 2, 3 | a palette fade over 96 updates (the robot appears) [likely]; then **text 1** once (`TextSeen(1)`): Alia names Illumina and the cables |
| 4, 5 | when the text box closes: piece `var` 1 (animation 1), 50 updates |
| 7–10 | pieces `var` 2, 16–19 (the robot opens), palettes |
| 11 | `PlayerEventEnd`, THINK |

### ZOOM SHOTS (step 4) [certain]

1. Pieces `var` 32–36 (the eye charge), 180 updates; `+0x7E` = 3–5 (phase 3).
2. **Phase 0:** a zoom shot every **90 updates** (fired 30 updates before the end of each wait);
   it never ends (the step runs until the phase changes). **Phase 1:** one every **45 updates**
   (fired 22 before the end), also without end. **Phase 3:** 4–6 shots (`+0x7E` + 1), 90 updates apart, then
   pieces `var` 37–40 and 60 updates, THINK.

**Zoom shot** (visual 28 `var` 0, `IlluminaZoomShot` `0x800F0330`): sound 2/48. It takes the
player's position at the shot. For **40 updates** it moves on the screen from the eye (160, 56) to
that place (linear, x = 160 + (target − 160) × t / 39), and it grows (sprites 0–13 of the drone
bank, frame t / 3), with a trail. Then it stays in the world at that place as a **40 × 40 blast**
(attack box (−20, −20, 40, 40), **contact 4**) for **16 updates**, and ends. So the player avoids it
by moving away from the place where they were when it was fired.

### DRONES (step 5) [certain]

1. Pieces `var` 2, 16–19; 40 updates. Pieces `var` 48–53 (a hatch opens; sound 2/49 at its flag);
   180 updates.
2. 18 updates before the end: the **head** (`var` 16) and the **drones** (`var` 17, 18, 19) come
   out at the body (160, 136).
3. Wait until the head is home (`illumina_laser` = −3); pieces `var` 54–59 (the hatch closes);
   180 updates; THINK.

**The head** (`var` 16; animation 80, sprite 69 of the body bank: Illumina's head with a red eye):
HP 40 but **no boxes** (cannot be hit, does not hurt). Priority 48.

1. It moves to screen (160, 32) at **2 px/update**. There it makes the probe and the scan beam.
2. It bobs ± 6 px (sine, +20 per update). In phase 3 it goes home after **360 updates** (or at
   once when the fight timer is < 11: a drone attack that was running when fight 2 starts ends).
   In phase 2 it goes home only when **all three drones are dead**. Each time the drones have
   fired (`illumina_laser` = −1), it waits 60 updates and makes a new probe and beam.
3. Home: it moves back to (160, 72) at 2 px/update and deletes itself (`illumina_laser` = −3). The
   drones then explode (`illumina_drones_done`).

**The probe** (`var` 32, `IlluminaProbeScan` `0x800EFCC0`): every update it marches a point from
the head along an angle in 8 px steps (at most 70) until a solid tile or the screen edge. The angle
sweeps between 2560 and 3584 (down-left to down-right) at 20 per update and back. A red scan beam
(ray 0 `var` 0, colour 31, semi-transparent, sound 2/50) goes from the head to the point; a marker
(piece `var` 4) is drawn there. Its box (−16, −16, 32, 32) does not hurt. **When the box touches
the player**, the point **locks on the player's place at that moment**; **60 updates later** the
drones fire at it and the probe ends.

**The drones** (`var` 17–19; drone bank; HP 16; hurt box (−8, −8, 16, 16); no attack box, contact
0: they do not hurt by contact):

1. They fly at **3 px/update** to screen (288, 96), (32, 32), (80, 176).
2. They bob ± 8 px and turn to the target (8 aim poses, animations 33–40).
3. Fire: a fire animation (41–48), a laser ray (ray 0 `var` 16–19) to the target, and a hit point
   (visual 28 `var` 16) at the target: box (−8, −8, 16, 16), **contact 6**, for 3 updates, then
   sound 2/57. Three drones make three hit points at the same place. After 50 updates the head
   starts a new scan.
4. A killed drone: explosion, 4 debris (animations 49–52), the drone count − 1.

### WALLS (step 6, fight 2) [certain for the code; the solid walls are likely]

Skipped while the walls are up. Pieces `var` 2, 16–19, 40 updates; pieces `var` 64–68; then the
**wall controller** (`var` 64, sound 2/56); 180 updates; pieces 69–73; 75 updates; THINK.

The wall controller:

1. Invisible hit boxes (visual 28 `var` 32: (−16, −8, 32, 24), **contact 4**, 6 updates) at screen
   (16, 8) and (304, 8).
2. Every 3 updates the two walls grow 16 px down: `CopyTiles` on **layer 0** (solid tiles) at the
   camera x and x + 288, 32 px wide, and the two hit boxes move down with the wall end. After 15
   rows (240 px): `Rumble(5)`, sound 2/53.
3. Every 8 updates a **cannon** (`var` 49–56) at the left wall (32, 56/96/136/176), facing right,
   and at the right wall (288, 36/76/116/156), facing left. The cannons are world objects (layer 0)
   at camera + place.
4. Every **90 updates** the cannons of the next mask fire (`illumina_cannon_masks`, `0x800FCB40`;
   a random pattern of 0–3: {left 4, right 4}, {2 pairs 4 times}, {crossed}, {two pairs}).
   40 updates after its bit comes, a cannon fires a **beam** (visual 28 `var` 48, sound 2/58): attack box
   (−127, −6, 255, 12), **contact 4**; it extends at 12 px/update; it ends with the cannon's
   animation.
5. When fewer than **5 cannons** live: the walls go (tiles restored, explosions down both sides),
   the cannons break (debris) and the step can run again later.

**Cannons:** HP 20, hurt box (−8, −10, 24, 20) after the opening; killed: animation 65, explosion,
4 debris (animations 66–69).

### ORBS (step 7, fight 2, Hard and above) [certain for the spawn; the orb is not read]

On Easy and Normal (`game.difficulty` < 2) THINK at once. Else pieces `var` 2, 16–19, 40 updates,
pieces 80–84 (a big arm), 108 updates; then main 11 `var` 16 at camera + (40, 8) and, one update
later, at camera + (280, 8); 40 updates; THINK.

### CABLE 1 BREAKS (step 8, end of fight 1) [certain]

`Rumble(7)`, pieces `var` 112–115 (sparks on the lens), 40 updates; a palette flash fades out with
`Quake2` every 4 updates; the HP bar goes off; `CopyTiles` draws the broken cable; 8 pairs of
explosions down the cable and 5 + 4 + 4 debris; camera **target max x = 4224**, max y restored;
THINK (phase 2: DRONES). The player is not held.

### DEATH (step 9, end of fight 2) [certain]

1. `Rumble(7)`, `PlayerEvent(20, 64)` (the player is held), sparks (`var` 112–115).
2. The palette flash fades with quakes and `Rumble(3)` every 32 updates.
3. When the player is in step 20: HP bar off, cable 2 breaks (`CopyTiles`), explosions,
   `WhiteFadeOut(60)`.
4. Explosions around screen (150, 120) every 4 updates and piece `var` 96 every 8, until the screen
   is white. Then the **reward**: when fight 2 took **< 5401 updates** (90 s), a teleporter crystal
   (main 28 `var` 1) at (4384, 864). All pieces close.
5. 60 updates, `WhiteFadeIn(25)`. When the fade ends: `PlayerEventEnd`, camera target max x =
   **5168**, max y restored; the body deletes itself (`DeleteObject3`, the record stays spawned).

`checkpoint` = 2 is written at HP 0. No Nightmare Soul, no drop, no exit request [certain for this
code].

### SHOCK (step 10) [certain]

Weapon ids **6, 15, 42** (slot 6 uncharged and charged, and id 42; `docs/engine.md`: slot 6 is
Rainy Turtloid's weapon [likely]). `illumina_clear` deletes every part, shot and piece at once
(the drones, the head, the walls and cannons too). `Rumble(7)`, `Quake2(8, 3, 2)`, sparks; for 80
updates the cable tiles flicker (4 frames, every 3 updates); then THINK starts a new pattern. The
damage of the hit counts as usual.

## Animations (table `illumina_anims`, `0x800FC84C`, 84 animations) [certain for the frames]

- 0–31, 75–81: body bank (bank 7 in both states, 105 sprites): the robot pieces. 0 eye flash
  (sprites 31–39), 2–4 shoulder pieces, 5–11 charge and arm moves, 12–16 the hatch and the heads
  coming out (59–69), 17–28 arms and hands, 29 sparks, 30, 31 the full robot (sprites 10, 12), 75–79
  closing moves, 80 the head (69), 81 eye flash.
- 1: sprites 100–104 with the palette of bank + 2 (piece `var` 1, intro). Those sprites are empty in
  both states [open].
- 32–74, 82, 83: drone bank (bank + 1, 170 sprites): 32 the zoom shot (14 hold frames, sprites
  0–13, picked by `SetAnimeFrame`), 33–40 drone aim poses, 41–48 drone fire, 49–52 drone debris, 53
  probe marker, 54, 55 cannons opening, 56–59 cannon parts, 60–63 the cannon beam, 64–74 debris.

The pieces (effect 11) pick their animation by `var`: group `var & 0xF0` and index `var & 15`
(0x00: 0–1, 0x10: 2–4, 0x20: 5–11, 81; 0x30: 12–16, 75–79; 0x40: 17–24; 0x50: 25–28; 0x70: 29–31).
A piece follows the body (+ its offset `+0x08`, `+0x0C`) and ends when the body's `act4` bits ask
(bit 0, 1 or 2 by the piece's step), or with `illumina_clear`.

## Damage

`dmg_tbl_illumina` (`0x8007483C`) [certain, read from the executable]:

| ids | damage |
|---|---|
| 0–15 | 1 2 2 3 3 3 3 – D 1 6 6 6 6 8 6 |
| 16–31 | 8 D 5 5 5 D 8 D 2 2 5 4 4 4 2 2 |
| 32–47 | 2 2 4 4 4 2 2 2 2 2 4 2 D 1 4 8 |
| 48–63 | 2 D 4 4 1 8 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

Hits to kill one cable (64 HP), before the modifiers: lemon (1) 64, half charge (1) 64, full charge
(5) 13, sabre 24/25 (2) 32. The drones (16 HP) and cannons (20 HP) use the same table. Charged
special weapons (10–16) take 6–8.

Damage to the player (before the difficulty scaling): drone laser hit point 6; zoom shot blast 4;
lens contact 4; wall hit boxes 4; cannon beam 4.

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 47 | piece `var` 1 (intro) |
| 48 | zoom shot |
| 49, 51, 52, 53, 54 | piece animation flags 5–9 (charge, hatch, arms) |
| 50 | scan beam (`StopSound(2, 23)` at its end) |
| 53 | the walls are down |
| 56 | wall controller starts |
| 57 | drone laser hit |
| 58 | cannon beam |
| bank 0, 14 | HP fill tick |

## Summary for a reimplementation

- A screen-fixed robot picture (sprites), not a stage object. Its only weak spot is a lens on a
  cable drawn in the stage tiles; the robot's hurt box sits on the lens while the camera is locked.
- Walk 1 (camera up to x 2048): zoom shots every 90 updates; jets from the right edge.
- Fight 1 at camera x 2048 (locked, y 384): 64 HP, zoom shots every 45 updates; jets from both
  sides. The cable breaks; the camera opens to x 4224.
- Walk 2: the drone attack without end: a head scans the floor with a beam; when the beam touches
  the player, the place locks, and 60 updates later three drones hit it (6 damage). Killing the
  three drones only restarts the attack.
- Fight 2 at camera x 4224 (locked, y 768): 64 HP; walls with 8 laser cannons first, then zoom
  shots or the drone attack (360 updates), and orbs on Hard. Weapon ids 6/15/42 cancel any attack.
  Death: white flash; a teleporter crystal when the fight took under 90 s; camera to x 5168.

## Open questions

1. The palette steps of the intro (`0x800EDD28`, `0x800EDF24`) and the sprites 100–104 of
   animation 1 (empty in both states).
2. Main 28 `var` 1 at (4384, 864): its destination (the crystal table of overlay 11 was not read).
3. Main 8 (jets) is not read beyond its spawn. Main 11 (orbs): `docs/enemies/illumina-orb.md`
   (overlay 23 has the same code). The names are not known.
4. The text messages 2 and 3 ("I've located what look like energy cables", "Illumina is still
   active!") are not opened by this code (talk zones, likely record 1, `id` 132).
5. Weapon id 42 (with 6 and 15): which weapon makes it (`docs/engine.md`).
6. A play check of the wall tiles (solid?) and of the cannon beam reach.
