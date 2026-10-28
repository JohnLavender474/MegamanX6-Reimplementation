# Stage: Blaze Heatnix (stage 3, overlay 05)

Section 1 of Blaze Heatnix's stage (the Magma Area): DAT member 97, 18 × 13 screens (4608 × 3328 px),
138 object records. Overlay `05` at RAM `0x800E9860` [certain: the object code and the record lists
of both states are in it]. The layout is in the overlay at offset `0x9760`, the record list at
`0x9A20` (RAM `0x800F3280`), the second list at RAM `0x800F36D8`. Stage 3 has one area in the tables
(`checkpoint_start_tbl` and `camera_zone_tbl` have no word for area 1) [certain].

Save states (in `~/mmx-data/x6/dumps/`; Zero, form 5; Normal (difficulty 1); lives 5 (4 in
`heatnix-4_cave`); armour parts `0x0C`; `nightmare_effect` 4):

| state | Zero at (x, y) | stage time | what |
|---|---|---|---|
| `heatnix-1_start` | 80, 394 | 664 | the start; area 0, checkpoint 0. Live: main type 25 `var` 5 and `var` 4 (second list, not set up yet), boss gate `id` 7 (second list), talk zones 129 and 130, stage logic 27 (`ReadyControl`) |
| `heatnix-2_mini-boss` | 366, 618 | 921 | the first mini boss, in the room after the start (camera zone 7: x 336–624, y 480). Main type 25 `var` 0–5 (Nightmare Snake: the ring `var` 5, its four gems `var` 0–3 and the next snake `var` 4) and visual type 29 `var` 0–3 (its jet flames) are live (`docs/enemies/nightmare-snake.md`), Rescue Reploid `id` 32 |
| `heatnix-4_cave` | 1012, 1898 | 3989 | the first tower, checkpoint 1, after the first snake (`game+0x26` = 1). Live: flame jets (main 36) records 35–38, 40, 41; grey blocks records 111–114; main type 25 `var` 4 (made by gate `id` 7, state 0); gate `id` 7 closed (state 2); talk zones 129, 130; stage logic 27. No virus is live; records 60, 62, 63, 65 were killed (soul bits `0x2D`). Reploids `id` 32 and 34 saved |
| `heatnix-5_snake-rising` | 2901, 2234 | 12880 | checkpoint 5, the upper route: the rising room of Nightmare Snake `var` 9 (camera x 2848–2880, y limits 2155 and going up 1 px per 4 updates). Main type 25 `var` 9 and its gems `var` 0–3, visual 29 `var` 0–3 (jets) and `var` 6 × 2 (the kill boxes of the flame); BG layer 2 mode 9 at (2784, 1712) is the purple flame; `stage_texture_swap` 2 (entry `0x16` loaded) |
| `heatnix-6_snake-room` | 3673, 1162 | 18913 | the top corridor just after the Nightmare Snake `var` 10 fight; checkpoint 6, `stage_texture_swap` 2. No main object; the four lava kill boxes (visual 29 `var` 7) are live, layer 2 shows the lava; `game+0x26` = 1, gate `id` 15 waits. Records 22 and 24 (upper route snakes `id` 6 and 9) are "spawned", 23 and 25 are not: the upper route |

Stage files: `~/mmx-data/x6/stages/m097-blaze-heatnix-section-1/` (catalog entry "Blaze Heatnix,
section 1", from `heatnix-1_start`) and `m097-blaze-heatnix-mini-boss/` (the same map from
`heatnix-2_mini-boss`, `tmx_skip`). TMX: `~/mmx-data/x6/tmx/m097-blaze-heatnix-section-1/`.
`learn` kept the layout although the screens join less well than the edge limit (horizontal 0.824
against 0.85; vertical 0.95): it is the layout on disc [certain for the place on disc].

The record format and the kinds are in `docs/objects-legend.md`; the shared rules in
`docs/engine.md`. Sheets in `docs/enemies/`: `flame-jet` (new), `grey-block`, `nightmare-bug`,
`nightmare-virus`, `rescue-reploid`, `teleport-crystal`, `light-capsule`, `boss-gate`.

## The map

The path, from the records, the camera zones and the gates [likely for the order]:

1. **Start** (x 0–624, y 224–480): a cave ledge. Checkpoint 0 respawn (80, 224), facing right.
2. **First mini-boss room** (camera zone 7: x 336–624, y 480; main type 25 `var` 5 of the second
   list at (448, 512)). Boss gate `id` 7 (second list, (944, 608)) opens only when `game+0x26` ≠ 0
   [certain for the gate code]; at its trigger it makes main type 25 `var` 4.
3. **The first tower** (camera x 944–976, y 448–2624): a tall shaft that goes down. 18 flame jets,
   5 grey blocks (`id` 3), 8 Nightmare Viruses, 2 Rescue Reploids. Checkpoint 1 at the top.
   In `heatnix-4_cave` the current max y is the camera y + 2, not 2624: `CameraLimitEase` keeps a
   target below the camera just ahead of it while the player stays on the follow line
   (`docs/engine.md`, "Camera") [certain for the code and the state]. Min y 448 is the gate value
   (checkpoint 1 restarts with 464).
4. **The tower floor** (checkpoint 2 at (1216, 2416); camera x 640–976, y 2400–2624; Rescue Reploid
   `id` 36 at (848, 2592)) and boss gate `id` 8 at (1296, 2784): camera x 1280, min y 1616.
5. **A shaft room** (x 1280–1600, y 1616 down to the floor): checkpoint 3 at (1328, 2768) and the
   **texture swap** (zone 1, `id` 129, x 1376–1391, y 2720–2799) at its floor. Rescue Reploids
   `id` 37–39, the item record (1328, 2192), 2 grey blocks (`id` 4). Two doors on its right side:
   gate `id` 11 at (1584, 2464) to the upper route (camera x 1584–2416, y 2080–2304) and gate
   `id` 9 at (1584, 2784) to the lower route (x 1584–2416, y 2528–2816). Each route has a main
   type 25 record at x 1664 (`id` 6 upper, 7 lower).
6. **A side corridor** (x 1280–2560, y 1600–1900) off the top of the shaft room (zone 8 opens max
   x 2240, zone 9 closes it to 1280): Light capsule `id` 6, Rescue Reploids `id` 40–44, 8 grey
   blocks (`id` 4), and the teleporter crystal (2512, 1824) at its far end [likely: from the record
   places and the zones; it draws correctly only with the second texture block, so it comes after
   the swap].
7. **The two routes** go right to x 2736: gate `id` 12 (2736, 2320) upper, gate `id` 10 (2736, 2976)
   lower; both lock the camera at x 2544 with push 3.84 px/frame. Checkpoints 5 (2800, 2224) and 4
   (2800, 2976); gates `id` 13 at x 2864 (both routes); main type 25 `id` 9 (3021, 2224) and `id` 8
   (3021, 2976).
8. **The second tower** (x 2848–3200, y 944–3072) goes up; Nightmare Bug and Nightmare Virus
   records. It is the **rising room** of Nightmare Snake `var` 9 (from checkpoint 5, y 2160) or
   `var` 8 (from checkpoint 4, y 2816): the view goes up 1 px every 4 updates to y 944 over a purple
   flame that kills (`docs/enemies/nightmare-snake.md`, MAGMA LEAP). Platforms: layer 0 solid cells
   (collision 56); no kill collision value in the tower.
9. **The top corridor** (y 944–1184): gate `id` 14 (3200, 1024; camera x 3200–3504, max y 1184),
   checkpoint 6, main type 25 `id` 10 (3378, 1024): a lava room (walls x 3232 and 3808, three
   platforms at x 3328–3392, 3488–3552, 3648–3712 with top y 1184, the BG layer 2 flame from
   y 1232, four kill boxes; floor y 1376 under the flame) with the Nightmare Snake `var` 10 fight
   (`docs/enemies/nightmare-snake.md`, CHASE; `heatnix-6_snake-room`), gate `id` 15 (3824, 1024;
   camera x 3808, y 944),
   checkpoint 7 (3840, 1024), gate `id` 16 (4112, 1104; camera x 4112–4144, y 944, time stamp), and
   the boss record (main type 52, (4240, 1088)).

Gates 7, 8, 10, 12, 14 and 15 open only when `game+0x26` ≠ 0 (`docs/enemies/boss-gate.md`). Each
Nightmare Snake body writes 0 at its set-up and 1 at its end (`SnakeInit`, `SnakeEnd`) [certain
for the code]; `heatnix-6_snake-room` has 1 after the `var` 10 fight, with gate 15 waiting.

BG layers (from the TMX): layer 1 mode 5 (fixed on the screen; code moves it), layer 2 mode 1 (half
speed). The layer-2 camera of checkpoint 6 is (928, 1664), not half of the layer-0 camera (3200,
1056), so code also moves layer 2 in the later parts [certain for the table]. In the rising room
Nightmare Snake `var` 8/9 set layer 2 to mode 9 and use it as the purple flame (four frames of
576 px at layer 2 x 960, 1568, 2176, 2784, y 1840–2255; ox stepped every 10 updates)
[certain; `heatnix-5_snake-rising`].

### The texture blocks [certain for VRAM and the code rule; the regions are by eye]

Member 97 has two texture blocks: entry `0x10000` and entry `0x16`. In both states the VRAM at
(320, 256) holds all 8 pages of entry `0x10000`; entry `0x16` agrees only on page 0 (and 192 of 256
rows of page 7) (`scripts/texpages.py`). `stage_texture_swap` (`0x8008EAB0`) is 0 in both states.
Camera zone 1 (record 8, `id` 129, bit 7) sets it to 1, so the game loads entry `0x16` when the
player enters x 1376–1391, y 2720–2799, in the shaft room after gate `id` 8 (`docs/engine.md`,
"The texture swap").

A drawing with entry `0x16` (scratch, `scripts/split_preview.py`) shows:

- **Layer 0:** the start, the first tower and the tower floor are correct only with `0x10000`.
  The shaft room, the side corridor, both routes, the second tower, the top corridor and the boss
  room are correct only with `0x16` (rounded cave edges, clean slopes, the blue tower). So the
  border is x 1280, the right wall of the first tower.
- **The purple screens at the edges are BG screens, not layer 0.** They are layer 1 screens
  (rows 5–6 and 10–11, x 2048–4608) and layer 2 screens (rows 7–8 and 11–12). With `0x10000` they
  are garbage; with `0x16` each is one even pattern (a lava or magma surface [guess]), still purple
  because the colours come from the start state. So they are not block stores and not unused
  screens [likely]: they are the backgrounds of the part after the swap. The layer 2 screens of
  rows 7–8 are the flame of the rising room: four animation frames (x 960, 1568, 2176, 2784), cell
  bits 14 and 15 set, all colours semi-transparent; in `heatnix-5_snake-rising` they are purple
  and drawn at layer 2 y 1712 (mode 9) [certain for the values]. Rows 11–12 of layer 2 and the
  layer 1 screens are not checked in a state yet.
- The layer-2 screens behind the first tower (rows 3–6, x 512–1600) draw a machine wall with
  `0x10000` and a starry sky with `0x16`; the tower is seen before the swap, so `0x10000` [likely].

Proposed `texture_split` for the catalog entry (preview checked by eye; layer 0 looks clean
everywhere with it):

```json
"texture_split": {"texture": 22, "regions": [
  [0, 1280, 1536, 2816, 3328], [0, 2816, 0, 4608, 3328],
  [1, 1280, 1536, 2816, 3328], [1, 2816, 0, 4608, 3328], [1, 2048, 1280, 2816, 1536],
  [2, 0, 1792, 4608, 3328]]}
```

## Nightmare Effect

`nightmare_effect` = 4 in both states; `game.stage_effect` = `0 6 3 4 2 4 7 5 3`, `game.effect_new`
= `2 8 8` (the pick of stage 3 itself, which wrote 3 to stages 2 and 8). Stage 3 got its 4 from
the earlier Metal Shark Player pick (targets `3 5 8`) [certain for the values]. Value 4 is
**Nightmare Iron** (`docs/stages/shark.md`, the Fandom table). `heatnix-3_rekkoha` and
`heatnix-4_cave` come from another run: `game.stage_effect` = `0 8 3 4 2 4 8 5 3` (a stage 8 pick,
targets `1 6 6`, before the stage 3 pick); stage 3 is still 4.

Overlay 05 reads `nightmare_effect` twice [certain for the reads with `0x800CD30A`]:

| reader | test | effect here |
|---|---|---|
| stage object 12 set-up (`0x800EA5C8`), the grey block | = 4 | the 15 grey blocks (records 111–125) exist. `id` 3 (5 blocks in the first tower) go away at checkpoint 3; `id` 4 (10 blocks in the shaft room and the side corridor) at checkpoint 4. The test runs only on screen: in `heatnix-6_snake-room` (checkpoint 6) the five `id` 3 blocks are still live, off screen |
| main type 22 set-up (`0x800EA07C`), the Nightmare Bug | = 1 | the 19 bug records (90–108) delete themselves at once |

With value 1 instead (a Commander Yammark pick after the Metal Shark Player pick writes 1 to stage
3), the bugs would appear and the grey blocks would not.

## Objects

All 138 records [certain for the counts]: records 0–21 are kind 3 (stage logic), records 22–44,
60–108, 110, 126 and 137 are kinds 0–2 (main objects), records 45–59, 109, 111–125 and 127–136 are
kind 4 (stage objects). No record has kind 5, 6 or 7.

Type list (`0x800733C4` word 6) and sprite banks in `heatnix-1_start`:

| bank | type | sprites | texture | object |
|---|---|---|---|---|
| 0 | 128 | 84 | 0 | Light capsule and boss gate (shared) |
| 1 | 15 | 43 | 160 | Nightmare Virus (shared) |
| 2 | 141 | 63 | 272 | Rescue Reploid |
| 3 | 36 | 28 | 320 | flame jet |
| 4 | 22 | 6 | 432 | Nightmare Bug |
| 5 | 142 | 8 | 448 | grey block (the same sheet as Turtloid bank 11 "Nightmare Iron") |
| 6 | 25 | 49 | 512 | main type 25, Nightmare Snake (`docs/enemies/nightmare-snake.md`) |
| 7 | 28 | 12 | 640 | teleporter crystal |
| 8–13 | 0 | – | none | no texture; the sprite data pointer is that of bank 14 [likely: filler] |
| 14, 15 | 52 | 61, 18 | none | Blaze Heatnix [likely]; not loaded at the start |

Update-table entries in overlay 05: main 22, 25, 28, 36, 52; visual 18 (`0x800F2B84`, near the
boss code), 29 (main 25's shot); stage 12. The entries logic 1 and 29, misc 1, 29 and 56, effect
7, 13 and 15 point into the middle of functions of main 36, 22 and 25 [certain for the addresses]:
no object of these types can work [likely: unused entries].

The table has kinds 0–2 and 4, from left to right. The type is the record `b`.

| record | kind | difficulty | type | object | id | x | y |
|---|---|---|---|---|---|---|---|
| 45 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 32 | 624 | 656 |
| 48 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 36 | 848 | 2592 |
| 30 | 1 | Normal and up | 36 | flame jet | 20 | 977 | 1088 |
| 34 | 2 | Hard and up | 36 | flame jet | 4 | 977 | 1930 |
| 41 | 1 | Normal and up | 36 | flame jet | 16 | 977 | 1984 |
| 115 | 4 | – | 12 | grey block (effect 4) | 3 | 992 | 2104 |
| 62 | 0 | all | 15 | Nightmare Virus | 1 | 1040 | 1152 |
| 28 | 1 | Normal and up | 36 | flame jet | 4 | 1041 | 1066 |
| 31 | 2 | Hard and up | 36 | flame jet | 4 | 1041 | 1322 |
| 111 | 4 | – | 12 | grey block (effect 4) | 3 | 1056 | 712 |
| 63 | 0 | all | 15 | Nightmare Virus | 1 | 1056 | 1536 |
| 66 | 0 | all | 15 | Nightmare Virus | 1 | 1056 | 2032 |
| 29 | 0 | all | 36 | flame jet | 0 | 1073 | 1194 |
| 42 | 1 | Normal and up | 36 | flame jet | 4 | 1073 | 2282 |
| 60 | 0 | all | 15 | Nightmare Virus | 1 | 1088 | 1024 |
| 65 | 0 | all | 15 | Nightmare Virus | 1 | 1088 | 1760 |
| 40 | 0 | all | 36 | flame jet | 24 | 1105 | 1984 |
| 27 | 1 | Normal and up | 36 | flame jet | 20 | 1137 | 688 |
| 32 | 0 | all | 36 | flame jet | 4 | 1137 | 1418 |
| 43 | 2 | Hard and up | 36 | flame jet | 4 | 1137 | 2282 |
| 112 | 4 | – | 12 | grey block (effect 4) | 3 | 1152 | 872 |
| 61 | 2 | Hard and up | 15 | Nightmare Virus | 0 | 1152 | 928 |
| 114 | 4 | – | 12 | grey block (effect 4) | 3 | 1152 | 1528 |
| 36 | 1 | Normal and up | 36 | flame jet | 16 | 1153 | 1632 |
| 35 | 0 | all | 36 | flame jet | 4 | 1153 | 1930 |
| 39 | 2 | Hard and up | 36 | flame jet | 20 | 1153 | 1984 |
| 67 | 1 | Normal and up | 15 | Nightmare Virus | 33 | 1168 | 2240 |
| 44 | 1 | Normal and up | 36 | flame jet | 20 | 1169 | 2112 |
| 113 | 4 | – | 12 | grey block (effect 4) | 3 | 1184 | 1240 |
| 38 | 1 | Normal and up | 36 | flame jet | 20 | 1185 | 1984 |
| 33 | 1 | Normal and up | 36 | flame jet | 4 | 1201 | 1482 |
| 46 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 34 | 1232 | 1296 |
| 64 | 2 | Hard and up | 15 | Nightmare Virus | 0 | 1232 | 1664 |
| 47 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 35 | 1232 | 2160 |
| 37 | 0 | all | 36 | flame jet | 20 | 1265 | 1632 |
| 127 | 4 | – | 8 | boss gate | 8 | 1296 | 2784 |
| 90 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 1312 | 528 |
| 50 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 38 | 1328 | 2080 |
| 49 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 37 | 1328 | 2192 |
| 110 | 0 | all | 47 | item object (`id` = item kind) [likely] | 2 | 1328 | 2192 |
| 109 | 4 | – | 26 | Light capsule | 6 | 1376 | 1720 |
| 68 | 0 | all | 15 | Nightmare Virus | 1 | 1392 | 2176 |
| 53 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 41 | 1456 | 1728 |
| 118 | 4 | – | 12 | grey block (effect 4) | 4 | 1472 | 1816 |
| 117 | 4 | – | 12 | grey block (effect 4) | 4 | 1472 | 1944 |
| 52 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 40 | 1520 | 1840 |
| 116 | 4 | – | 12 | grey block (effect 4) | 4 | 1520 | 2280 |
| 51 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 39 | 1544 | 1968 |
| 131 | 4 | – | 8 | boss gate | 11 | 1584 | 2464 |
| 128 | 4 | – | 8 | boss gate | 9 | 1584 | 2784 |
| 119 | 4 | – | 12 | grey block (effect 4) | 4 | 1664 | 1720 |
| 22 | 0 | all | 25 | main 25: Nightmare Snake | 6 | 1664 | 2464 |
| 23 | 0 | all | 25 | main 25: Nightmare Snake | 7 | 1664 | 2784 |
| 54 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 42 | 1712 | 1728 |
| 120 | 4 | – | 12 | grey block (effect 4) | 4 | 1808 | 1720 |
| 69 | 0 | all | 15 | Nightmare Virus | 1 | 1808 | 1752 |
| 55 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 43 | 1856 | 1728 |
| 70 | 0 | all | 15 | Nightmare Virus | 1 | 1944 | 1752 |
| 91 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 1944 | 2792 |
| 121 | 4 | – | 12 | grey block (effect 4) | 4 | 1952 | 1720 |
| 77 | 1 | Normal and up | 15 | Nightmare Virus | 1 | 1968 | 2672 |
| 80 | 1 | Normal and up | 15 | Nightmare Virus | 1 | 1984 | 2416 |
| 122 | 4 | – | 12 | grey block (effect 4) | 4 | 2016 | 1720 |
| 94 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2024 | 2376 |
| 71 | 2 | Hard and up | 15 | Nightmare Virus | 0 | 2048 | 1680 |
| 72 | 0 | all | 15 | Nightmare Virus | 0 | 2120 | 1808 |
| 92 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2168 | 2824 |
| 81 | 1 | Normal and up | 15 | Nightmare Virus | 1 | 2200 | 2376 |
| 123 | 4 | – | 12 | grey block (effect 4) | 4 | 2208 | 1704 |
| 78 | 1 | Normal and up | 15 | Nightmare Virus | 1 | 2240 | 2704 |
| 73 | 0 | all | 15 | Nightmare Virus | 0 | 2272 | 1712 |
| 95 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2312 | 2344 |
| 124 | 4 | – | 12 | grey block (effect 4) | 4 | 2336 | 1704 |
| 57 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 45 | 2352 | 2384 |
| 125 | 4 | – | 12 | grey block (effect 4) | 4 | 2400 | 1704 |
| 74 | 0 | all | 15 | Nightmare Virus | 1 | 2416 | 1808 |
| 82 | 1 | Normal and up | 15 | Nightmare Virus | 32 | 2424 | 2344 |
| 56 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 44 | 2448 | 1840 |
| 75 | 2 | Hard and up | 15 | Nightmare Virus | 1 | 2464 | 1760 |
| 76 | 2 | Hard and up | 15 | Nightmare Virus | 1 | 2496 | 1968 |
| 79 | 1 | Normal and up | 15 | Nightmare Virus | 1 | 2496 | 2784 |
| 93 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2504 | 2952 |
| 126 | 0 | all | 28 | teleporter crystal (area change) | 1 | 2512 | 1824 |
| 58 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 46 | 2624 | 2992 |
| 132 | 4 | – | 8 | boss gate | 12 | 2736 | 2320 |
| 129 | 4 | – | 8 | boss gate | 10 | 2736 | 2976 |
| 133 | 4 | – | 8 | boss gate | 13 | 2864 | 2224 |
| 130 | 4 | – | 8 | boss gate | 13 | 2864 | 2976 |
| 108 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2896 | 1040 |
| 88 | 0 | all | 15 | Nightmare Virus | 1 | 2904 | 1200 |
| 85 | 0 | all | 15 | Nightmare Virus | 1 | 2904 | 1544 |
| 102 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2904 | 1776 |
| 100 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2904 | 2000 |
| 98 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2912 | 2304 |
| 97 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2912 | 2448 |
| 106 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 2928 | 1264 |
| 84 | 0 | all | 15 | Nightmare Virus | 0 | 2992 | 1840 |
| 59 | 4 | – | 4 | Rescue Reploid (`id` = Reploid number) | 47 | 2992 | 2528 |
| 87 | 0 | all | 15 | Nightmare Virus | 0 | 3000 | 1400 |
| 24 | 0 | all | 25 | main 25: Nightmare Snake | 9 | 3021 | 2224 |
| 25 | 0 | all | 25 | main 25: Nightmare Snake | 8 | 3021 | 2976 |
| 104 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 3024 | 1584 |
| 99 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 3040 | 2176 |
| 83 | 0 | all | 15 | Nightmare Virus | 33 | 3040 | 2512 |
| 103 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 3048 | 1712 |
| 105 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 3056 | 1584 |
| 89 | 1 | Normal and up | 15 | Nightmare Virus | 1 | 3112 | 1152 |
| 96 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 3120 | 2752 |
| 86 | 1 | Normal and up | 15 | Nightmare Virus | 1 | 3128 | 1400 |
| 107 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 3136 | 1104 |
| 101 | 0 | all | 22 | Nightmare Bug (needs effect 1) | 0 | 3144 | 1872 |
| 134 | 4 | – | 8 | boss gate | 14 | 3200 | 1024 |
| 26 | 0 | all | 25 | main 25: Nightmare Snake | 10 | 3378 | 1024 |
| 135 | 4 | – | 8 | boss gate | 15 | 3824 | 1024 |
| 136 | 4 | – | 8 | boss gate | 16 | 4112 | 1104 |
| 137 | 0 | all | 52 | Blaze Heatnix [likely] | 0 | 4240 | 1088 |

Notes:

- Flame jet (main 36, 18 records): `id` 0 and 4 fire up, 16, 20 and 24 fire down; low nibble 0 =
  one long flame, 4 = four bursts, 8 = 1–4 random bursts (`docs/enemies/flame-jet.md`). Kinds 1
  and 2: 13 of the 18 need Normal (9) or Hard (4).
- Grey block (stage object 12, 15 records): the same code as overlays 08 and 29, instruction for
  instruction after relocation (344 words) [certain]; only under Nightmare Effect 4. The falls in
  the first tower (records 111–115) are 80, 160, 176, 48 and 144 px (`docs/enemies/grey-block.md`).
- Nightmare Bug (main 22, 19 records): the same code as overlay 10 (339 words) [certain]; only under
  Nightmare Effect 1, so none appear in these states.
- Teleporter crystal (record 126, `id` 1): the same code as overlay 03 (846 words) [certain];
  `id` 1 asks for an area change (`game.exit_req` = `0x41`). The destination is not known.
- Rescue Reploids (records 45–59): `id` 32–47, the status words `game+0xE8` and `+0xEC` (words 4
  and 5), both 0 in `heatnix-1` and `-2`. In `heatnix-4_cave` word 4 is `0x202`: `id` 32 and 34
  saved.
- Nightmare Virus: `id` 0 and 1 normal; `id` 32 (record 82) and 33 (records 67, 83) are snatchers
  next to Reploid records 57, 47 and 59. They spawn under Nightmare Effect 4 (no effect test).
  Soul row 2. In `heatnix-4_cave` records 60, 62, 63 and 65 were killed (soul bits `0x2D`); the
  next one to spawn below the save point is record 66 (1056, 2032), when the camera y passes about 1744 [likely: the band
  is camera y + 288; the strip bounds of `ScrollSpawn` were not read].
- Light capsule (record 109): `id` 6 = bit `0x20`, a Shadow Armor part (arms)
  (`docs/enemies/light-capsule.md`). `game.armor_parts` is `0x0C` in the states.
- Item (record 110, main 47, `id` 2) at the same point as Rescue Reploid record 49 (1328, 2192).
  What item kind 2 is, is not known.
- Main type 25 (records 22–26 with `id` 6–10, and second-list records `id` 5 and 4): the mini
  bosses; read in another work folder.
- Main type 52 (record 137): the boss [likely]; no texture at the start, not read.

## Stage logic records (kind 3, records 0–21)

All main-program types: 0 `CameraZone`, 6 `CheckpointMarker`, 14 `TalkZone`.

### Checkpoints [certain for the table and the markers]

`checkpoint_start_tbl` word 6 → list `0x80073168` (8 entries; the next list starts at
`0x80073188`).

| record | `id` | marker x, y | checkpoint | respawn player | camera 0 | camera 2 | limits x | limits y | facing |
|---|---|---|---|---|---|---|---|---|---|
| – | – | – | 0 (start) | 80, 224 | 0, 224 | 0, 112 | 0–624 | 224–480 | right |
| 0 | 1 | 1072, 560 | 1 | 1088, 464 | 944, 464 | 472, 232 | 944–976 | 464–2624 | right |
| 1 | 2 | 1216, 2416 | 2 | 1248, 2624 | 976, 2624 | 488, 1312 | 640–976 | 2400–2624 | left |
| 2 | 3 | 1328, 2768 | 3 | 1424, 2736 | 1280, 2624 | 640, 1312 | 1280 | 1616–2624 | right |
| 3 | 4 | 2800, 2976 | 4 | 2784, 2816 | 2544, 2816 | 1272, 1408 | 2544 | 2816 | right |
| 4 | 5 | 2800, 2224 | 5 | 2784, 2160 | 2544, 2160 | 1272, 1080 | 2544 | 2160 | right |
| 5 | 6 | 3216, 1024 | 6 | 3360, 1024 | 3200, 1056 | 928, 1664 | 3200–3504 | 944–1184 | right |
| 6 | 7 | 3840, 1024 | 7 | 3952, 992 | 3808, 944 | 1904, 472 | 3808 | 944 | right |

- Checkpoints 4 and 5 have layer-1 camera (0, 0) and `beam_thru` 16; the others have layer 1 =
  layer 0 and `beam_thru` 1. Respawn 3 is past the swap zone x (1424 > 1376); whether a restart
  at checkpoint 3 or later loads entry `0x16` is not known (`docs/engine.md`).

### Camera zones (type 0, 10 records) [certain for the table]

`camera_zone_tbl` word 6 → `0x80079540` (10 entries, all used; the next table starts at
`0x800795E0`).

| record | `id` | x, y | player rectangle | target limits |
|---|---|---|---|---|
| 14 | 7 | 352, 520 | x 352–591, y 448–703 | x 336–624, y 480 (first mini-boss room) |
| 16 | 9 | 1376, 1952 | x 1376–1407, y 1920–1983 | x max 1280 |
| 15 | 8 | 1424, 1824 | x 1424–1455, y 1792–1855 | x max 2240 (side corridor) |
| 7 | 0 | 1256, 2440 | x 1216–1279, y 2432–2447 | x min 640, y min 2400 |
| 11 | 4 | 1328, 2784 | x 1312–1343, y 2752–2815 | y min 1616 |
| 8 | 129 | 1384, 2760 | x 1376–1391, y 2720–2799 | none; `id` bit 7: the texture swap, then deleted |
| 12 | 5 | 1792, 2704 | x 1776–1791, y 2576–2815 | y max 2624 |
| 13 | 6 | 1792, 2704 | x 1792–1807, y 2576–2815 | y max 2816 |
| 10 | 3 | 2560, 2240 | x 2544–2559, y 2112–2351 | y max 2304 |
| 9 | 2 | 2560, 2240 | x 2560–2575, y 2112–2351 | y max 2160 |

### Talk zones (type 14, 5 records)

| record | `id` | x, y |
|---|---|---|
| 17 | 129 | 80, 256 |
| 18 | 130 | 304, 256 |
| 19 | 131 | 2896, 2896 |
| 20 | 132 | 3216, 1024 |
| 21 | 133 | 4096, 1024 |

All have bit 7: optional Alia calls ("!" and Select) (`docs/engine.md`, "Talk zones").

## The second record list (`fixed_set_tbl`, 3 records)

`fixed_set_tbl` word 6 → `0x800F36D8`:

| record | kind | type | `id` | x, y | object |
|---|---|---|---|---|---|
| 0 | 0 | 25 | 5 | 448, 512 | main type 25: Nightmare Snake, the first fight (live in both states) |
| 1 | 0 | 25 | 4 | 1248, 2624 | main type 25 `var` 4 (live in both states, not set up) |
| 2 | 4 | 8 | 7 | 944, 608 | boss gate `id` 7 |

No palette cycles, no tile animators, no ray records [certain for the list].

## Boss gates

| record | `id` | x, y | opens when | camera after |
|---|---|---|---|---|
| fixed 2 | 7 | 944, 608 | `game+0x26` ≠ 0 | x 944–976, y 448–2624; makes main 25 `var` 4 |
| 127 | 8 | 1296, 2784 | `game+0x26` ≠ 0 | x 1280, min y 1616 |
| 131 | 11 | 1584, 2464 | always | x 1584–2416, y 2080–2304 |
| 128 | 9 | 1584, 2784 | always | x 1584–2416, y 2528–2816 |
| 132 | 12 | 2736, 2320 | `game+0x26` ≠ 0 | x 2544, min y 2160, push 3.84 |
| 129 | 10 | 2736, 2976 | `game+0x26` ≠ 0 | x 2544, min y 944, push 3.84 |
| 133 | 13 | 2864, 2224 | always | x 2848–2880 |
| 130 | 13 | 2864, 2976 | always | x 2848–2880 |
| 134 | 14 | 3200, 1024 | `game+0x26` ≠ 0 | x 3200–3504, max y 1184 |
| 135 | 15 | 3824, 1024 | `game+0x26` ≠ 0 | x 3808, max y 944 |
| 136 | 16 | 4112, 1104 | always | x 4112–4144, y 944, time stamp (second gate before the boss) |

From the `id` table of `docs/enemies/boss-gate.md` [certain for the code]. Gates 15 and 16 are the
boss corridor.

## Nightmare Snake rooms

`docs/enemies/nightmare-snake.md` has the fight. The first snake room: floor y 640 with three 32 px
deep dips, walls x 320–352 (from y 432) and 928–960; camera limits x 336–624, y 480; Rescue Reploid
`id` 32 (record 45) at (624, 656). The fight starts when the player passes x 363 or y 464; the
player is kept at y ≥ 432 until it passes x 944 [certain for the code]. Records 22–26 are main 25
`id` 6, 7, 9, 8, 10 (more snake fights; `id` 9 is in `heatnix-5_snake-rising`, the end of `id` 10
in `heatnix-6_snake-room`, the others have no save state yet), and the second list has `id` 5 (the
first fight) and `id` 4 (the next snake, which gate 7 makes again). The rising room (`id` 8 or 9):
see the map, item 8. The lava room (`id` 10): the map, item 9. On the upper route the player meets
`id` 6 and 9, on the lower route `id` 7 and 8 [likely: from the record places; in
`heatnix-6_snake-room` records 22 and 24 are "spawned", 23 and 25 are not].

## Hidden parts and teleporters

The teleporter crystal (record 126) is the only teleporter. No record of this list makes a hidden
part; where the crystal leads (area change `0x41`) is open.

## Open questions

1. The colours and screen place of the BG screens after the texture swap, and the
   `texture_split` regions: `heatnix-5_snake-rising` has entry `0x16` in VRAM (`stage_texture_swap`
   2) and shows the layer 2 flame (rows 7–8); the layer 1 screens and layer 2 rows 11–12 are still
   open.
2. The destination of the teleporter crystal (`id` 1, area change).
3. Item kind 2 (record 110).
4. Why banks 8–13 are type 0 with no texture.
