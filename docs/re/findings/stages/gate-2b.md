# Stage: Gate stage 2, section 2 (stage 18, area 0, overlay 24)

The second section of Gate stage 2 and the room of Gate himself. Stage 17 is section 1
([`gate-2.md`](gate-2.md)); `StageExit` sends a stage 17 clear straight to stage 18, so the two play
as one stage [certain for the code]. Stage music 19, boss music 26 (`docs/sound.md`) [certain for
the tables]. The boss is **Gate** (main 59, record 64; [`gate.md`](../enemies/gate.md)). The
section has three parts that the teleporter crystals join: a top corridor, then one path for X
(a dark room and an acid-rain room with Weather Analyze machines) and another for Zero (a long
corridor under a Nightmare Press), then a common corridor, the boss corridor and Gate's room. The
parts copy the themes of the Maverick stages: Meta Wheel F, Illumina jets, Monbando, the Weather
Analyze rain (Rainy Turtloid) and the Nightmare Press (Metal Shark Player).

## Save states

| state | checkpoint | player (x, y), HP | camera (x, y), limits | what |
|---|---|---|---|---|
| `gate-4_before-gate` | 4 | Zero, 2989, 1082, 34 | 2880, 960; x 2880 (min) / 2878 (max), y 960 | the corridor between boss gates 30 and 31 |
| `gate-5_gate-fight` | 4 | Zero, 3264, 1114, 34 | 3184, 960; x 3184–3216, y 58 → 928 (target), 960 | the fight has started: Gate (main 59) in step 6, one orb and six platforms live |

- Zero (`game+0x38` = 1, form 5), step 2; difficulty 1; lives 2; boss level (`game+0x8F`) 2;
  `nightmare_effect` 0; stage time 16 621 and 17 238 [certain].
- `game+0x60` = `0xFF`, `+0x61` = 56, `+0x62` = 52; the six gauges are 312 = `mega+0x14A`
  [certain].
- `stage_texture_swap` (`0x8008EAB0`) = 2 in both: the swap to block `0x16` has been done
  [certain].
- Live in `gate-4`: boss gates 30 (state 2, closed) and 31 (state 1); the 23 press blocks of the
  second list (stage object 9, state 1, far below at y 1890–1986); the Nightmare Press (kind-7
  type 2); four `PaletteCycle`; stage logic 17 `var` 1; stage logic 27 (the stage start banner).
  No main, visual or misc object [certain].
- In `gate-4` the target max x is 1024 and the current max x is 2878, below the min x 2880: the
  camera stays at x 2880. Boss gate `id` 30 writes Y − 48 = 1024 to the max x
  (`docs/enemies/boss-gate.md`, the push table); this stage shows its effect [certain for the
  values].

## Overlay, member and lists

- Overlay 24 at `0x800E9860` (57 258 bytes, up to `0x800F780A`; 48 bytes differ from
  `rock_bin/24.bin` in `gate-4`); stage member 114, a layout of 25 × 10 screens (6 400 × 2 560 px),
  catalog entry "Gate stage 2, section 2" (`~/mmx-data/x6/stages/m114-gate-stage-2-section-2/`,
  TMX `~/mmx-data/x6/tmx/m114-gate-stage-2-section-2/`). No area 1 [certain].
- `enemy_set_tbl` stage 18 area 0: `0x800F5C88` (overlay offset `0xC428`), 65 records and the end
  record. `fixed_set_tbl`: `0x800F5E98`, 29 records [certain].
- Live main types in the main table `0x800F5F88`: 2, 8, 9, 14, 28, 59 (overlay code) and 15, 47
  (main program); all others are stubs [certain].
- Text bank: member 226 for Zero, 225 for X (`text_members`), the same as stage 17. Message 1 is
  Gate's talk (Gate's sheet). Member 225 does not decode with `mmxdemake.text` [certain for the
  run].
- Boss pack: member 137 (`boss_pack_tbl[18]`); `boss_pack_loaded` = 0 in `gate-4`, 1 in `gate-5`
  [certain]. The loader is not read.
- Talk zones: none (`talk_stage_idx`[36] = 12, all boxes of k 12 are 0; no talk zone record)
  [certain].

### Texture blocks

Member 114 has two texture blocks: `0x10000` and `0x16`. Both save states have block `0x16` in
video memory (`stage_texture_swap` = 2) [certain]. Three things write the swap request: the
teleporter crystal `id` 2 at the end of the top corridor (`docs/enemies/teleport-crystal.md`,
B2), camera zone 128 (record 1, x 0–319, y 992–1135, the start of X's path) and camera zone 137
(record 10, x 0–5039, y 1904–2095, Zero's path) [certain for the records]. So the top corridor
(checkpoint 0) uses block `0x10000` and everything after the teleporter uses `0x16` [likely].

- Drawn with `0x16`, the top corridor has rain noise and a broken cell at x 384, y 176; drawn
  with `0x10000` it shows a row of spikes there, which is where collision 63 is [seen in the two
  drawings]. The X path, the Zero path (layer 1 too) and the corridors are correct with `0x16`.
  Gate's room is the same in both blocks [certain for the drawings].
- Layer 2 (the background) is the same in both blocks; layer 1 differs only in parts that use
  `0x16` [certain for the drawings].
- The catalog entry has `texture_split` with `0x10000` for `[0, 0, 0, 1920, 240]` (layer 0 of the
  top corridor), so the TMX shows the top with its own block. The colours of the top come from
  the palettes of `gate-4`; a save state at checkpoint 0 is needed to check them [guess that they
  are right].

## Map

Collision of layer 0, one character per 96 × 32 px (6 × 2 cells; the most important value wins).
`#` 56 solid, `=` 57, `|` 59 boss gate, `^` 63 spikes. Markers: `C` checkpoint marker, `z` camera
zone, `D` boss gate, `G` Gate, `w` Meta Wheel F, `8` Illumina jet, `m` Monbando, `A` Weather
Analyze record, `t` teleporter crystal, `h` health capsule. Column 0 is x 0; each column is 96 px.

```
    0 ####################
   32 #################  #
   64 #################  #
   96     8   w 8 w8 ww t#
  128 #################  #
  160 ####^###############
  192 ####################
  224 ####################
  928                                  =========
  960 ########            ##############  ##  ##
  992       ##            ##### # ######  #### #
 1024       ##               ## 88##|##|  #|   #
 1056  z  z ##                 # #8 D  DG #| ###
 1088       ##            ##########|##|  #| ###
 1120 ####  ##            ############### ### ##
 1152 ####  #==========   ##############  ######
 1184 ####  ##        =   #############=  ==   =
 1216    #  ##       z#                =  ==   =
 1248    #  ##     m  #                =  ==   =
 1280    #  z   m######
 1312    #  m   #######                  #
 1344    # m## #####  #                  #
 1376    ###########  #
 1408    ###########m ============================
 1440              ####          ##      A###
 1472              ####          ##       ### m
 1504              #   A     m  m##   ##  ##t #
 1536              #z m      #  #8## 8##h ###
 1568              # ^^#   ###  #  #mm### m##
 1600              ##=####h######  ######## 8 m ##
 1632              ^^#######  ##    ####### # # ##
 1664              #########          ##### # # ##
 1888 ===================================================================
 1984       ## w  ww    ##
 2016  z    w#########  ##                                             t
 2048      ##########w ww#      m#w#w# wm###m m
 2080 ###############################################8###8####8###8######
 2112 ###################################################################
 2144 ###################################################################
```

(The checkpoint marker, record 0 at (2944, 1072), is under boss gate 30's `D`.) Collision values
in layer 0 (cells): 0 (58 170), 56 (5 068), 57 (724: lines at y 928, 1408 and 1888, and the
walls of the Gate rooms), 59 (24, x 2912–3647), 63 (14, x 384–1551) [certain].

| part | x (px) | y (px) | what |
|---|---|---|---|
| top corridor | 0–1919 | 0–239 | checkpoint 0; Meta Wheel F and Illumina jets; spikes at x 384; teleporter `id` 2 at x 1760 |
| X: dark room | 0–1599 | 960–1679 | checkpoint 1 (teleporter entry 0); camera zones 1–8; steps down with Monbando; acid rain (layer 1 frames at x 0–319) |
| X: rain room | 1920–4207 | 1408–1679 | small platforms over a pit under a ceiling line (57 at y 1408); Weather Analyze machines `id` `0x15`, `0x16`; two health capsules; teleporter `id` 3 at x 3712 |
| Zero: press corridor | 0–6399 | 1888–2159 | checkpoint 2 (teleporter entry 1); a floor under the Nightmare Press (BG layer 1, mode 9) with 23 press blocks; Meta Wheel F, Monbando, Illumina jets; teleporter `id` 4 at x 6256 |
| common corridor | 1920–2927 | 960–1199 | checkpoint 3 (teleporter entries 2 and 3); three Illumina jets |
| boss corridor | 2928–3183 | 960–1199 | between boss gates 30 and 31; checkpoint 4 (record 0) |
| Gate's room | 3184–3535 | 928–1199 | the fight; six platforms that Gate's code makes (`gate.md`) |
| room copies | 3536–4447 | 928–1279 | two more copies of the room drawing, and loose cells at y 1280–1300; tile stores for `CopyTiles` [likely: the intro copies from x 4096] |

Layers: layer 0 has cells at y 0–239, 928–1679 and 1904–2159; layer 1 has five copies of the rain
(x 0–319, 256 px apart in y, the frames that stage logic 17 shows) and the press band (x 0–6079,
y 1888–2095); layer 2 (the background) x 0–1919, y 0–719 [certain for the cells].

## Objects

65 records (list `0x800F5C88`). `kind` low digit 0–2 main (the minimum difficulty), 3 stage logic,
4 stage object. No item (main 47), Rescue Reploid (stage 4), Light capsule (stage 26) or talk
zone record [certain].

| # | kind | b | `id` | object |
|---|---|---|---|---|
| 0 | 3 | 6 | 4 | checkpoint marker (2944, 1072) |
| 1–10 | 3 | 0 | 128, 1–8, 137 | camera zones (below) |
| 11–24 | 0–2 | 2 | 1 | 14 Meta Wheel F ([`meta-wheel-f.md`](../enemies/meta-wheel-f.md)): top (800, 112), (1216, 112), (1440, 112), (1552, 112); press corridor (592, 2016), (928, 2000), (1184, 2000), (1312, 2000), (1520, 2064), (1744, 2064), (1664, 2064), (2688, 2064), (2880, 2064), (3248, 2064). Kinds: 0 ×9, 1 ×3, 2 ×2 |
| 25–37 | 0–2 | 8 | 0 | 13 Illumina jets (the main 8 of overlay 11): top (456, 104), (1032, 104), (1320, 104); rain room (2664, 1560), (3032, 1544), (3656, 1624); press corridor (4552, 2088), (4920, 2088), (5448, 2088), (5816, 2088); common corridor (2504, 1040), (2648, 1048), (2760, 1080) |
| 38–54 | 0–2 | 9 | 0 | 17 Monbando ([`monbando.md`](../enemies/monbando.md)): dark room (544, 1352), (640, 1320), (1016, 1288), (1248, 1256), (1424, 1416), (1600, 1544); rain room (2272, 1528), (2528, 1528), (2928, 1592), (3056, 1592), (3520, 1592), (3904, 1480), (3904, 1608); press corridor (2528, 2072), (3296, 2072), (3648, 2072), (3888, 2072) |
| 55–56 | 4 | 13 | 1 | health capsules ([`health-capsule.md`](../enemies/health-capsule.md)), `id` 1 (breaks after one use): (1944, 1608), (3296, 1544) |
| 57–59 | 0 | 28 | 2, 3, 4 | teleporter crystals ([`teleport-crystal.md`](../enemies/teleport-crystal.md)): (1760, 104), (3712, 1520), (6256, 2032) (below) |
| 60–61 | 0 | 14 | `0x15`, `0x16` | Weather Analyze machines ([`weather-analyze.md`](../enemies/weather-analyze.md)), index 5 and 6 (below) |
| 62–63 | 4 | 8 | 30, 31 | boss gates ([`boss-gate.md`](../enemies/boss-gate.md)) at (2928, 1072), (3184, 1072) |
| 64 | 0 | 59 | 0 | Gate at (3312, 1056) ([`gate.md`](../enemies/gate.md)) |

The type of each object [certain for the code match unless marked]: main 2 = all 17 functions of
the `meta-wheel-f` kit, identical after masking; main 8 = all 28 functions are in overlay 11 (and
32), and its animation table is the `illumina-jet` table (7 of 7); main 9 = all 13 functions of the
`monbando` kit; main 14 = all 14 functions of the `weather-analyze` kit; main 28 = all 17 functions
of the `teleport-crystal` kit; stage 13 = 18 of the 19 functions of the `health-capsule` kit. The
`spriteexport` IDs are the same as in the shared exports (below). See
[`~/mmx-data/x6/re/gate-stage-3/notes.md`](../../work/gate-stage-3/notes.md).

The second list (`0x800F5E98`) [certain]:

| # | kind | b | `id` | object |
|---|---|---|---|---|
| 0–3 | 3 | 3 | 0–3 | `PaletteCycle`; all four live |
| 4–26 | 4 | 9 | 2 (`a` = 48) | 23 press blocks (`PressBlock`, the stage object 9 code of overlays 06 and 07, 4 of 4 functions): x 120–1800 every 240 px at y 1904; x 2360–3720 at y 1968; x 4280–5960 at y 2000. All live in `gate-4` |
| 27 | 7 | 2 | 0 | the Nightmare Press (`PressUpdate`, `0x80046E14`, main program; `docs/enemies/nightmare-press.md`); live |
| 28 | 3 | 17 | 1 | stage logic 17: the acid rain controller (below); live |

### Weather Analyze in this stage

The two machines use rows 5 and 6 of the Weather Analyze tables (`id & 0x0F`), which are equal in
overlay 24 (positions at `0x800F6CE0`) [certain for the data]. The machine does not use its record
position. Row 5: machine (3168, 1488), rain zone x 1920–3199, on its death the camera max x
becomes 3840 (and `cam+0x26` the old max x). Row 6: machine (4144, 1584), zone x 3328–4207, no
camera change. Sensor positions: row 5 (2143, 1567), (2688, 1584), (3040, 1544), (3095, 1583); row
6 (3584, 1488), (3632, 1648), (3792, 1616), (3984, 1472). These are the rain room of X's path
[certain for the tables; `docs/enemies/weather-analyze.md` lists the rows].

### Stage logic 17: acid rain [certain for the code]

The 6 functions are identical after masking to stage logic 17 of overlay 30 (stage 21 area 1,
`docs/stages/turtloid.md`, the alternate section). Set-up: copies 32 bytes of the extra palettes into
`0x800F7798`. While `game+0x26` ≠ 0 (the rain is on): a 60-update palette fade
(`0x8002B410`, `PaletteCopy`), sound (5, 4); every 4 updates the layer 1 camera y (`0x8009725A`)
steps through 0, 256, 512, 768, 1024 (the five rain frames of layer 1) with a redraw; every 180
updates the player loses 1 HP (not while a text box is open; at HP 1 it writes `0x80`); every 51 updates `mega+0xE8` = 1. When
`game+0x26` is 0 again it fades back. So this is the acid rain of the X path [likely for the
meaning].

## Teleporter crystals [certain for the code and the table]

The crystals of this stage use sequence B (`docs/enemies/teleport-crystal.md`); the destination
table in overlay 24 is at `0x800F6E80` and has the four entries listed in that sheet:

| record | `id` | at | goes to |
|---|---|---|---|
| 57 | 2 | top corridor, x 1760 | X: entry 0, (72, 1115), checkpoint 1 (the dark room); Zero: entry 1, (120, 2074), checkpoint 2 (the press corridor); both: `stage_texture_swap` = 1 |
| 58 | 3 | rain room, x 3712 | entry 2, (1952, 1115), checkpoint 3 |
| 59 | 4 | press corridor, x 6256 | entry 3, (1952, 1114), checkpoint 3 |

So X and Zero take different paths after the top corridor and meet again in the common corridor.

## Camera and checkpoints

### Checkpoints

`checkpoint_start_tbl` entry 36 → `0x800732AC`, five records (the next stage's list starts at
`0x800732C0`) [certain]:

| checkpoint | record | player (x, y) | camera 0 | camera 1 | camera 2 | limits x, y | set by |
|---|---|---|---|---|---|---|---|
| 0 | `0x80072E40` | 152, 80 | 0, 0 | 1664, 0 | 0, 0 | 0–1600, 0–0 | the start |
| 1 | `0x80072E64` | 72, 1024 | 0, 960 | 1664, 0 | 0, 240 | 0–368, 960–960 | teleporter `id` 2 (X) |
| 2 | `0x80072E88` | 120, 1920 | 0, 1904 | 0, 1904 | 0, 476 | 0–6080, 1904–1904 | teleporter `id` 2 (Zero) |
| 3 | `0x80072EAC` | 1952, 1008 | 1920, 960 | 1664, 0 | 480, 240 | 1920–2608, 960–960 | teleporters `id` 3, 4 |
| 4 | `0x80072ED0` | 2992, 1056 | 2880, 960 | 1664, 0 | 720, 240 | 2880–2880, 960–960 | marker record 0 |

All face right, beam byte 2. The layer modes: layer 1 mode 9, layer 2 mode 3 (`gate-4`).

### Camera zones (stage logic 0)

From the TMX `camera zones` layer [certain for the table]:

| record | `id` | trigger x, y, w × h | min x | max x | min y | max y |
|---|---|---|---|---|---|---|
| 1 | 128 | 0, 992, 320 × 144 | – | – | – | – (texture swap) |
| 2 | 1 | 384, 992, 16 × 144 | 320 | – | – | 1184 |
| 3 | 2 | 368, 992, 16 × 144 | 0 | – | – | 960 |
| 4 | 3 | 592, 1264, 16 × 80 | – | 1280 | 1184 | – |
| 5 | 4 | 576, 1264, 16 × 80 | – | 368 | 960 | – |
| 6 | 5 | 1472, 1184, 16 × 96 | 1280 | – | – | 1440 |
| 7 | 6 | 1456, 1184, 16 × 96 | 320 | – | – | 1184 |
| 8 | 7 | 1392, 1488, 16 × 128 | – | 2880 | 1440 | – |
| 9 | 8 | 1376, 1488, 16 × 128 | – | 1280 | 1184 | – |
| 10 | 137 | 0, 1904, 5040 × 192 | – | – | – | – (texture swap) |

The zones come in pairs one cell apart (forward and back) at three places on the X path.

### Boss gates [certain for the code of `docs/enemies/boss-gate.md`]

| record | `id` | x | after the pass | special |
|---|---|---|---|---|
| 62 | 30 | 2928 | min x 2880, max x Y − 48 = 1024 (the camera stays at x 2880) | – |
| 63 | 31 | 3184 | camera x 3184–3216, min y 928 | the FADE step before it opens; Gate's intro undoes the fade |

`gate-5` has the target limits x 3184–3216, y 928–960 [certain].

## Sprites

`spriteexport` on `gate-4` (stage name `gate-2b`): Meta Wheel F, Illumina jet, Monbando, teleporter
crystal, Weather Analyze and the boss gate get the same IDs as the shared exports, and every sheet
is byte-equal to the shared sheet [certain]. Gate's banks (14, 15) have no texture in `gate-4`.
Bank 4 is misc type 143 (misc 15; beam and capsule frames [guess]), which has no export of its own.

On `gate-5` the boss pack covers the cells of banks 3–6 (Monbando, misc 143, teleporter, Weather
Analyze); the tool skips them ("cells overwritten by type 59"). Banks 1 and 2 are still right
(byte-equal). Gate is exported from the live object as `enemy-db764876a4` (34 animations); with
the `sets` rule `{"stage": 18, "type": 59, "set": 1, "animations": [31, 32, 33], "texture":
"0x0540", "palette": 1}` animations 31–33 (the small creature and the platform) come out right
[seen by eye].

## Stage end

Gate's death sets `game.exit_req` = 1; `StageExit` sends a stage 18 clear to game mode 19, scene 9,
with `game+0x66` = 5 [certain for the code; Gate's sheet, "Death"].

## Open questions

1. A save state at checkpoint 0 to check the top corridor's block and colours, and states on the X
   path (dark room, rain room) and the Zero path (press corridor) for the colours of `0x16` there.
2. Which object loads the boss pack (member 137) before the fight.
3. What collision 57 does here (the lines at y 928, 1408 and 1888).
4. Member 225 (X's text bank of stages 17 and 18) does not decode.
5. The Nightmare Press of this stage: its `var`, the count of bodies, and the layer 1 band.
