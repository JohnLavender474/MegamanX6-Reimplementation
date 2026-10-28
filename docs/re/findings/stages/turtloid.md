# Stage: Rainy Turtloid (stage 6, overlay 09)

Overlay `09` at RAM `0x800E9860`. Save states (area 0, in `~/mmx-data/x6/dumps/`): `turtloid-1`
(start, x 160), `turtloid-2_mid` (x 5736, checkpoint 3, Weather Analyze machine 2 live),
`turtloid-3_mid` (x 8132, checkpoint 4, after a death and respawn; machine 3 live),
`turtloid-6_zero-player` (Zero as the player, x 7726, checkpoint 4) and `turtloid-7_boss` (Zero in
the boss room, Rainy Turtloid). Stage 21, area 1 (the alternate section): `turtloid-4_alt` (start),
`turtloid-5_zero` (boss room, Zero Nightmare) and `turtloid-8_high-max` (boss room, High Max).
`report-1` is the report screen after the Turtloid clear (stage 15).
Stage member 101, a layout of 37 × 6 screens. The stage map is extracted with `learn` and `stages`
(catalog entry "Rainy Turtloid, section 1"): `~/mmx-data/x6/stages/m101-rainy-turtloid-section-1/`
has the art, the collision, the object records (`objects.json`) and `stage.ora`.

The map was drawn again on 2026-10-21 with the corrected layout address (`docs/engine.md`,
"Layout"); the older drawing was scrambled sideways by 11 screens. In the new drawing the sky and
the sea over the long bridge are still black, so layer 0 has no tiles there [certain for the
layout]. **BG layer 2 draws them** (see "The background layers" below): it scrolls at half the
speed of layer 0 in x, so `stage.ora` puts it in the wrong place. Layer 1 is the rain curtain.

The record format and the kinds are in `docs/objects-legend.md`; the shared rules in
`docs/engine.md`. Sheets in `docs/enemies/`: `monbando`, `sea-attacker-st`, `batton-bone-b81`,
`weather-analyze`, `teleport-crystal`, `boss-gate`, `nightmare-shadow`, `rocket-platform`,
`health-capsule`, `zero-nightmare`, `rain-effect` (the rain curtain), `darkness-effect`. The stage list of area 0 (list
entry 6) has 137 records.

## Objects

All 137 records by kind and type [certain for the counts]:

| class, type | records (kind 0 / 1 / 2) | object |
|---|---|---|
| main 9 | 5 / 2 / 3 | Monbando |
| main 10 | 13 / 3 / 2 | Sea Attacker ST |
| main 14 | 4 | Weather Analyze |
| main 15 | 10 / 3 / 4 | Nightmare Virus |
| main 18 | 2 / 7 / 7 | Batton Bone B81 (all `id` 0) |
| main 28 | 1 | teleporter crystal |
| main 47 | 8 | items |
| main 57 | 1 | Rainy Turtloid [plausible] |
| stage logic 0, 6, 14 | 25, 5, 7 | camera zones (`CameraZone`, `docs/engine.md`, "Camera"); checkpoint markers (below); talk zones (`TalkZone`, Alia's messages; `docs/engine.md`, "Talk zones") |
| stage object 4 | 12 | Rescue Reploid |
| stage object 5 | 5 | rocket platform |
| stage object 8 | 2 | boss gate |
| stage object 13 | 5 | health capsule |
| stage object 26 | 1 | Light capsule |

The tables below give the records that are read so far.

### Weather Analyze (main type 14, kind 0)

| record `id` | record x, y | machine index | the machine's own position | rain zone (X's x) |
|---|---|---|---|---|
| `0x10` | 1376, 544 | 0 | 1574, 592 | 1248–1631 |
| `0x11` | 1888, 624 | 1 | 2848, 384 | 1872–2879 |
| `0x12` | 3552, 368 | 2 | 7648, 384 | 3584–7695 |
| `0x13` | 8000, 384 | 3 | 8576, 672 | 7792–8655 |

The machine ignores its record position and jumps to the position of its index. While a machine is
alive and X is in its zone, the acid rain falls (1 HP every 180 frames). Destroying it stops the
rain and opens the camera limits (`docs/enemies/weather-analyze.md`). The records of kind 3, type 14
(`id` `0x81`–`0x87`, `0x02`) are stage-logic objects, not this enemy.

### Boss gates (stage-object type 8)

| `id` | x, y | effect |
|---|---|---|
| 34 | 8624, 688 | opens only when `game+0x28` (`0x800CCEF8`) ≠ 0, that is after Weather Analyze machine 3 (record `0x13`) is destroyed; camera fixed at x 8624 |
| 23 | 8944, 688 | the gate before the boss: camera x 8944–9040, y 640–688; stores the stage time |

So the player must destroy machine 3 (zone x 7792–8655) before the boss corridor opens.

### Rocket platforms (stage-object type 5)

Over the sea at x 4248–4880 (screens 16–19). `id` 0 moves horizontally ±96 px; `id` 1 moves
vertically ±80 px (`docs/enemies/rocket-platform.md`).

| record | `id` | x, y |
|---|---|---|
| 46 | 1 | 4248, 384 |
| 47 | 0 | 4368, 368 |
| 48 | 0 | 4560, 256 |
| 49 | 0 | 4752, 320 |
| 50 | 1 | 4880, 336 |

### Health capsules (stage-object type 13)

Stand on one to refill the HP; the rain does not drain HP meanwhile. `id` 0 can be used again;
`id` 1 breaks after one use (`docs/enemies/health-capsule.md`).

| record | `id` | x, y | rain zone |
|---|---|---|---|
| 41 | 0 | 1280, 584 | machine 0 (x 1248–1631) |
| 42 | 0 | 1920, 648 | machine 1 (x 1872–2879) |
| 43 | 0 | 2912, 408 | just after the machine 1 zone |
| 44 | 0 | 8096, 408 | machine 3 (x 7792–8655) |
| 45 | 1 | 8528, 712 | machine 3, before boss gate 34 |

### Other stage objects

| record | class, type | `id` | x, y | object |
|---|---|---|---|---|
| 132 | stage object 26 | 5 | 8880, 392 | Light capsule, armour part bit `0x40` (Shadow Armor) |
| 133 | main 28 | 1 | 7736, 808 | the blue teleporter crystal before the boss room: to the alternate section (stage 21, area 1; see below) |

### Nightmare shadows (Nightmare Effect 7)

Invisible spawners (main type 26, `var` 0–5) make the shadows when the player is in their zone. The
second record list places them (kind 0, type 26, `id` 2 and 3; see below) [certain for the records;
no spawner is live in the save states, because the Nightmare Effect is off].

| `var` | zone (player position, px) |
|---|---|
| 0 | 321 ≤ x ≤ 2559 |
| 1 | 3201 ≤ x ≤ 4159 |
| 2 | 1249 ≤ x ≤ 1631 |
| 3 | 4961 ≤ x ≤ 7311 |
| 4 | 1 ≤ x ≤ 1055 |
| 5 | 321 ≤ x ≤ 2143 and y < 1072 |

### Sea Attacker ST (main type 10)

| id | kind 0 (all) | kind 1 (Normal and Hard) | kind 2 (Hard only) |
|---|---|---|---|
| 0 (shooter) | 1 (x 3944) | 3 (x 3760, 6448, 6784) | 1 (x 7598) |
| 1 (diver) | 12 | – | 1 (x 6944) |

Four divers are at y 640–688 (x 432–872); all the other records are at y 448.

### Checkpoints (stage logic 6)

The five kind-3 records with type 6 are checkpoint markers (`CheckpointMarker`, `0x80052D30`, code in
the main program) [certain for the code; the three save states agree]. When the player's x is at or
past the record x, `game.checkpoint` (`game+0x1D`) = `id & 0x7F` and the marker deletes itself. With
`id` bit 7 set, the test uses y instead of x. A marker whose `id & 0x0F` is already the checkpoint
deletes itself at once.

| record | `id` | x | checkpoint after |
|---|---|---|---|
| 0 | 1 | 1128 | 1 |
| 1 | 2 | 1648 | 2 |
| 2 | 3 | 2944 | 3 |
| 3 | 4 | 7696 | 4 (just after the machine 2 zone) |
| 4 | 5 | 8784 | 5 (after boss gate 34) |

**Respawn points** [certain for the code and the table]. At a (re)start, `CheckpointStart`
(`0x80029B8C`) takes the record `checkpoint_start_tbl` (`0x80073300`, a word per stage × 2 + area)
→ [checkpoint] (36 bytes): player x, y (`+0x00`, `+0x02`), the camera of layers 0, 1, 2
(`+0x04` … `+0x0E`), the camera limits min x, max x, min y, max y (`+0x10` … `+0x16`, current and
target), the player facing (`+0x18`, `0x40` = right) and the offsets `+0x40`/`+0x42` of layers 1
and 2 (`+0x1A` … `+0x20`, all 0 here). The list of stage 6 area 0 is at `0x800731B0`:

| checkpoint | player x, y | camera 0 | camera 2 | limits x | limits y |
|---|---|---|---|---|---|
| 0 (start) | 160, 464 | 0, 464 | 0, 464 | 0–1296 | 464 |
| 1 | 1120, 464 | 1056, 464 | 528, 464 | 1056–1296 | 464 |
| 2 | 1760, 464 | 1600, 464 | 800, 464 | 1600–2272 | 464 |
| 3 | 3520, 224 | 3488, 224 | 1744, 224 | 3488–7360 | 224 |
| 4 | 7752, 224 | 7680, 224 | 3840, 224 | 7680–8320 | 224 |
| 5 | 8784, 624 | 8624, 528 | 4312, 528 | 8624 | 528 |

All face right; camera 1 is (320, 0) in all of them (the rain curtain out of view, see below).
`turtloid-3_mid` (after a respawn at checkpoint 4) agrees: the camera limits there are x 7808–8320,
y 224–386, after a camera zone changed the minimum. The alternate section (stage 21 area 1, list
`0x800732E8`): checkpoint 0 (112, 64), camera (0, 16); checkpoint 1 (1168, 816), camera (960, 704),
limits x 960, y 704.

## The second record list (`fixed_set_tbl`)

A second list (pointer table `0x80073554`, stage × 8 + area × 4) starts at `0x800F5378`, right after
the 137 records. It has 12 records and an end record. All four object classes in it are live in the
save states [certain]:

| records | kind | type | `id` | x, y | object |
|---|---|---|---|---|---|
| 2 | 3 | 3 | 0, 2 | 0, 0 | palette cycles (`PaletteCycle`, `0x80052898`; below); both live in `turtloid-1` (slots 1, 2) |
| 3 | 6 | 5 | 3, 4, 5 | 0, 0 | ray objects of type 5: the darkness effect of Nightmare Effect 8 (`docs/enemies/darkness-effect.md`); deleted at once when the effect is not 8, so none is live |
| 1 | 3 | 11 | 0 | 0, 0 | the acid-rain controller (`0x800ED510`, `docs/enemies/weather-analyze.md`) |
| 4 | 5 | 20 | 0 | 592, 664; 1008, 600; 3008, 424; 3200, 424 | scenery (below) |
| 2 | 0 | 26 | 2, 3 | 0, 0 | Nightmare shadow spawners |

**Scenery (misc type 20, `0x800ED92C`, table `0x800F60C0`)** [certain for the code]. The kind-5
records make misc objects (class `0x800CD410`). Each one draws sprite 0 of sprite type 140 (bank 10,
one sprite) with priority 19 and a 32 × 32 visibility test. It does not move and has no boxes. It
deletes itself when `game.checkpoint` ≥ 4, so it is not live in `turtloid-3_mid`.

**Palette cycles (stage logic 3, `PaletteCycle`, main program, state table `0x8007A0D4`)**
[certain for the code]. Stage logic 3 animates one palette (or `count` palettes in a row) by a
script:

- State 0 (`0x80052610`): flags `0x09` (runs during a pause). Script = `0x8007A01C`[stage × 2 +
  area][`id`] (`+0x1C`); a null script → `DeleteObjectKeep`. The target = `0x80079F64`[stage × 2 +
  area] + `id` × 2: byte 0 = the palette index in the working palettes (scratchpad `+0x28`, 32
  bytes each), byte 1 = the count (`+0x20`). It copies count × 32 bytes from the source palettes
  (scratchpad `+0x30`) at script byte 0, sets `bg_refresh` bit 0 (the palette upload [likely]) and
  the timer `+0x21` = script byte 1. In stage 8 with checkpoint 0 it sets the hold flag
  `0x800E4480` = 1, else 0.
- State 1 (`0x800527A4`): nothing while `0x800E4480` ≠ 0. The timer counts down; below 0 the
  script goes 2 bytes on. Duration 0 ends the script (state 2, `DeleteObject3`); a negative value
  −n jumps n entries back (a loop). Then the new source palette is copied.
- A script entry is `{source palette, updates}`.

| `id` | target palette | script (source: updates) | loop | what it lights [likely] |
|---|---|---|---|---|
| 0 | 225 (VRAM CLUT 16, 494; tile palette 129) | 17, 18, 19, 20, 19, 18: 14 each | 84 updates | small lights on the orange pillars (layer 0, screens 2–3 and 11–12) |
| 1 | 110 | 21, 22, 23, 24, 23, 22: 18 each | 108 updates | no record in either list |
| 2 | 135 (VRAM CLUT 112, 488; tile palette 39) | 25, 26, 27, 28, 27, 26: 18 each | 108 updates | small light panels in the last part (layer 0, screens 30–33) |

The tile cells that use each palette were found from the tile info words of member 101 and drawn
over `stage.png` (probe [`~/mmx-data/x6/re/yammark-turtloid/p8_paltiles.py`](../../work/yammark-turtloid/p8_paltiles.py)). So the cycles are not
the sea; the sea is on layer 2 (below).

## The background layers [certain for the code and the camera values]

The three layer cameras in `turtloid-1`, `turtloid-2_mid` and `turtloid-3_mid`:

| layer | mode | what the mode does | holds |
|---|---|---|---|
| 0 | 1 | follows the player | the stage (collision) |
| 1 | 5 (`0x800297B4`) | nothing: only other code moves it | the rain curtain: 5 frames of rain streaks (rows 0–4, columns 0–1) |
| 2 | 4 (`0x8002998C`) | x = layer 0 x / 2 + `+0x40`, y = layer 0 y + `+0x42` | the sky, the stormy sea, the sea wall, the pagoda and the boss room background (columns 0–18, rows 0–3) |

- Layer 2 values agree in the three states: layer 0 at (5576, 224) → layer 2 at (2788, 224); (7972,
  384) → (3986, 384). Layer 2 is 18 screens wide; at half speed it covers the whole stage (camera x
  up to 9040 → 4520 + 320). Its rows 0–1 are stage y 0–511; columns 17–18 of rows 2–3 are behind
  the boss room.
- A drawing of layer 2 at its scrolled place under layer 0 fills every black area over the bridge
  with sky and sea (probe `p5_compose.py`). So the converter must place layer 2 with the half-speed
  x parallax, not at the map origin.
- Layer 1 is the acid rain: the rain controller (stage logic 11) moves it to (0, 0) and changes its
  y by 256 every 4 updates (`docs/enemies/rain-effect.md`, "The other rain"). The checkpoint records
  put it at (320, 0), where its art has no frame.

## Facts from the mid-stage save states

- **Weather Analyze.** Machine 2 (`turtloid-2_mid`) and machine 3 (`turtloid-3_mid`) are at their
  table positions, not their record positions. Machine 3 has all four sensors live at the table
  positions; machine 2 has sensors 3 and 4 (sensors 1 and 2 were destroyed [likely: a sensor never
  deletes itself off screen]). `game+0x26` (rain) is 1
  in both, and X is in the zone each time. This agrees with `docs/enemies/weather-analyze.md`.
- **Rocket platforms.** In `turtloid-2_mid` all five are live while X is 850 px or more past them,
  four at one end of the path (record 46 at y + 80, records 47 and 49 at x + 96, record 48 at
  x − 96) and record 50 on its way (y + 75).
  This agrees with the sheet: the platform never deletes itself.
- **Items.** Records 128 (`id` 32) and 130 (`id` 33) are live; each fell onto the floor below its
  record point. The other item records of that group were already collected.
- **Death and respawn.** X died between the two mid states and respawned at checkpoint 4. In
  `turtloid-3_mid`, every record behind X has its "spawned" bit and its kind high digit cleared, and
  no object behind X is live, also the wreck of machine 2 (which never deletes itself). So a death
  clears all objects and resets the whole record list. The Rescue Reploid status (`game+0xD8`)
  survives the death: Reploids 80–85 are still "saved". The lives stay at 8, because saved Reploids
  gave lives back (the user).
- **Counters** (values in `turtloid-1`, `-2_mid`, `-3_mid`) [certain for the code unless marked]:
  - `game+0x98` (0, 108, 183), 32-bit: the **HP the player lost in this stage**. Every player
    damage path adds the damage (`0x80031000`, `0x800311C4`, `0x800311D8`, `0x80039270`,
    `0x800392A4`, `0x80039350`, `0x800393DC`, `0x80039434`); an instant death adds the HP that was
    left (`0x80028304`: `HP & 0x7F`). `StageStartReset` clears it; the report screen (overlay 20,
    `0x800F1D2C`) shows it.
  - `game+0x9C` (0, 15, 15), 16-bit: the **largest damage of one player hit on an enemy** in this
    stage. `ApplyWeaponHit` (`0x800318DC`) stores the damage (after the cap of 127) when it is
    larger. 15 is a full charge shot. Zero's states have 6.
  - `game+0x9E` (1 in all three), 16-bit: not part of `+0x9C`. A bit set per stage group (index
    `0x8007A124`[stage × 2 + area]); `0x800533F4(n)` returns bit n. Stage logic 14 (the event zones)
    reads it, so a set bit probably means "this event was seen" [likely].
  - `game+0xAA` (0, 4, 4): no reader or writer found with a search for game-base accesses; it is 4
    in every later state (also Shark, Sheldon, Scaravich) and 0 in the intro and Yammark states.
    Open.
  - The kill count `game+0x94` is 0, 32, 49 and the X souls `game+0xD2` are 228, 280, 312.

## The boss room: Rainy Turtloid (main type 57)

Save state `turtloid-7_boss` (the user, slot 2): Zero (the player) at (9032, 858) at the start of the
fight. Checkpoint 5, camera at (8944, 688), limits x 8944–9040, y 640–688. No boss overlay is
loaded: the boss code is in the stage overlay.

- Rainy Turtloid is main slot 0, type 57, state 1/5/2, at (9184, 816). `game+0x20` points to it,
  `game+0x24` = 1, `game+0x8C` = 56. Sprite source: data `0x8014EB48`, texture `0x140`, palette
  `0x7948` (no texture-table entry for bank 14, as for the other bosses).
- Behaviour sheet: `docs/enemies/rainy-turtloid.md`. Room (camera minimum 8944, 640): inner walls
  x 8960 and x 9344, ceiling y 656, floor y 880, middle x 9152; boss start (9184, 592), standing
  y 816. In the save state the boss is in pattern 0, step 5 (the missile volley), count 9.
- The ten visual objects (type 23) are his two shell crystals (`var` 0, 1) and 8 volley missiles
  (`var` 2, 3) [certain].
- Below the room, at y 976–1199, x 8944/8960 and 9344/9360 hold spare wall columns (collision 57 and
  the normal 59/56 columns). The boss copies them onto the walls when his shell hits a wall
  (`CopyTiles`).
- Boss gates `id` 23 (x 8944) and `id` 34 (x 8624) are live, both in state 2.
- Export: `~/mmx-data/x6/sprites/enemies/rainy-turtloid/` (`enemy-520fe16c6e`), 119 sprites and 34
  animations: the shell stance, the open shell, the curled shell, an arm cannon, small missiles,
  spinning green crystals and blue water bubbles. Its texture covers the cells of banks 4–9 and 12.

## Alternate section (stage 21, area 1)

The blue teleporter crystal before the boss room (record 133, main type 28, `id` 1, x 7736, y 808)
sets `game.exit_req` = `0x41`. It takes X to a section with its own stage number: `game.stage` = 21,
area 1 [certain from the save state `turtloid-4_alt`, taken at the start of the section; the user
used the crystal].

- Overlay 30, stage member 120, a layout of 8 × 5 screens (catalog entry "Rainy Turtloid,
  alternate section"; `~/mmx-data/x6/stages/m120-rainy-turtloid-alternate-section/`). Layer 0 uses
  about 5–7 × 4 screens: one tall room of grass platforms around a central shaft, and a corridor
  to the right at the bottom. The camera limits in the state are x 0–768 and y 16–704.
- List entry 21, area 1 of `enemy_set_tbl`: 41 records.

| class, type | records (kind 0 / 1 / 2) | object |
|---|---|---|
| main 9 | 8 / 2 / 2 | Monbando |
| main 18 | 11 / 1 / 3 | Batton Bone B81 (all `id` 0) |
| main 14 | 1 | Weather Analyze, `id` `0x14`: machine index 4 at (854, 864), rain zone x 0–1119 |
| main 47 | 2 | items, `id` 33 |
| stage object 4 | 4 | Rescue Reploids, global ids 92–95 |
| stage object 13 | 2 | health capsules, `id` 0, at (296, 168) and (928, 888) |
| stage object 8 | 2 | boss gates `id` 44 (x 1072) and 45 (x 1264), y 864 |
| stage logic 6 | 1 | checkpoint marker `id` 1, x 1088 |
| stage logic 14, 13 | 1, 1 | background setting `id` 132; type 13 at (1360, 864), not read |

- Boss gate `id` 44 needs `game+0x26` = 0 (`docs/enemies/boss-gate.md`), so the corridor opens only
  after Weather Analyze machine 4 is destroyed [likely; the same rule as gate 34 in section 1].
  Its four sensors are live at the table positions.
- The second list: stage logic 3 (`id` 0, 1), stage logic 17 (live, in its state 3; the rain
  controller of section 1 is type 11, so 17 is probably this section's one [guess]), the Nightmare
  shadow spawner `var` 4 (zone x 1–1055, which matches this room), and one kind-6 record.
- Sprite banks: the same as section 1, except bank 7 = type 0 (24 sprites; no object yet) and banks
  14–17 = types 53, 55, 55, 63. These banks have no texture-table entry; they are the three bosses
  that stage logic 13 can make (below).

### The boss room behind gate 45: Zero Nightmare (main type 53)

Save state `turtloid-5_zero` (the user, slot 1): X at (1344, 875) in the boss room, fighting Zero
Nightmare. Checkpoint 1, camera at (1264, 704), limits x 1264–1296, y 704. The user says that the fight
is almost the same as the Zero fight in X5, with a new palette and one or two new attacks.

- The record of stage logic 13 at (1360, 864) is the boss spawner (`0x80052E44`, `docs/engine.md`,
  "Boss overlay"). With `0x800CD30B` = 0 it loaded overlay 43 at `0x800FA000` and made main type 53.
  The two other choices are type 55 (overlay 44), High Max [certain: `turtloid-8_high-max`], and
  type 63 (overlay 45), Dynamo [likely]. `boss_choice` goes 0 → 1 → 2 with game progress, and all 8
  hidden areas share it (`docs/engine.md`, "Boss overlay").
- Behaviour sheet: `docs/enemies/zero-nightmare.md`. Room: camera minimum (1264, 704), floor y 896,
  boss start (1488, 864). In the save state the boss is in pattern 4, step 5 (dash).
- Zero Nightmare is main slot 0, state 1/5/1, at (1446, 872). `game+0x20` points to it, `game+0x24` =
  1 (HP bar shown), boss HP maximum `game+0x8C` = 48 [likely].
- Its sprite source: data `0x80159430`, texture `0x140`, palette `0x7948`. Three effect objects (type
  6) use the same sprite data with palette `0x794C`, and a visual object (type 19) uses the same
  source [likely: the after-images].
- Export: `~/mmx-data/x6/sprites/enemies/zero-nightmare/` (`enemy-71f965d1d8`), 124 sprites and 17
  animations: stand, dash, jump, saber combos, a buster charge and shots, saber waves, a large crescent
  wave and ground spikes. Its texture covers the cells of banks 4–13 in this state, so those banks
  are skipped here; they draw from the earlier states.

### The same room with High Max (main type 55, `var` 1)

Save state `turtloid-8_high-max` (the user, slot 4): Zero is the player, after the Zero Nightmare
clear. `boss_choice` = 1, and overlay 44 is at `0x800FA000` (1 of 33304 bytes differs). This
confirms the rule for value 1.

- Zero at (1299, 860), checkpoint 1, the same camera and limits as `turtloid-5_zero`.
- High Max is main slot 0, type 55, `var` 1, state 1/5/1, at (1552, 766). `game+0x20` points to it,
  `game+0x24` = 1, `game+0x8C` = 48. Sprite source: data `0x8014FCE8`, texture `0x140`, palette
  `0x7948`.
- Behaviour sheet: `docs/enemies/high-max.md`. In the save state he is in pattern 6 (VOLLEY, SHIELD
  RAM), step 5, 2 updates after the fourth shot. The four visual objects (type 21, `var` 0) are his
  energy balls: 5 px/frame, aimed at the player, 15 updates apart. They use their own sprite data
  (`0x801521BC`, 67 sprites) with the boss's texture and palette [certain].
- Room points (from the camera minimum): hover y 64, rush y 112 / 116, sides x 64 and 288, centre
  x 176.
- Export: `~/mmx-data/x6/sprites/enemies/high-max/` (`enemy-1b0293f642`), 116 sprites and all 23
  animations, with the `sets` rule for the shot set (`docs/enemies/high-max.md`, "Animations").

## Open questions

1. The single camera zones and event zones (stage logic 0 and 14) are not listed record by record.
2. `game+0xAA`: what writes it. Palette cycle `id` 1 (palette 110) has no record.
3. The darkness effect (Nightmare Effect 8): a save state after a stage-8 pick.
4. Rainy Turtloid: collision value 57 on the hit wall; re-export animations 28–33 from banks +1 and
   +2.
5. Alternate section: where the section leads after the boss. A hidden-area save state with
   `boss_choice` 2 is needed to export Dynamo.
