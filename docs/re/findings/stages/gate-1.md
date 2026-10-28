# Stage: Gate stage 1 (stage 16, area 0, overlay 22)

The first stage of Gate's lab. The game opens the lab after the eight Mavericks (`docs/engine.md`,
"`boss_choice`"). Stages 16, 17 and 18 are the three Gate stages: stage music 19 for all three,
boss music 21, 23 and 26 (`docs/sound.md`) [certain for the tables]. Stage 16 ends with the
Nightmare Mother ([`nightmare-mother.md`](../enemies/nightmare-mother.md)); stage 18 is Gate
(`docs/damage.md`, "Main type 59: Gate"); stage 17 ends with High Max `var` 2 (record 112;
[`gate-2.md`](gate-2.md)) and goes straight on to stage 18 [certain for the record and the code of
`StageExit`]. The stage is a lab entrance in the snow, a
deep hole down into a dark room, two climbs with spikes and lasers, a long corridor, three rooms
where lava rises, a climb up to the boss corridor and the boss room.

## Save states

| state | checkpoint | player (x, y), HP | camera (x, y), limits | what |
|---|---|---|---|---|
| `gate-1_before-boss` | 4 | Zero, 7824, 122, 42 | 7664, 0; x 7664–7664, y 0–0 | the corridor between boss gates 39 and 26 |
| `gate-2_nightmare-mother` | 4 | Zero, 8152, 330, 42 | 7992, 144; x 7984–8096, y 16–144 | the boss fight (main 60, `var` 0–4 live) |

- Zero (`game+0x38` = 1, form 5); difficulty 1; lives 1; `boss_choice` 2; `nightmare_effect` 0;
  stage time 11 236 and 12 995 [certain].
- `game+0x60` = `0xFF` (all techniques). The gauges are 312 = `mega+0x14A` = 6 × `game+0x62`: the
  gauge maximum of Zero is `game+0x62`, of X `game+0x61` (`docs/players/weapons.md`) [certain for
  the save states].
- In `gate-1` no main, visual or misc object is live. Live: boss gates 39 and 26 (stage objects 0,
  1, both in state 1, step 0), five `PaletteCycle`, stage logic 7 and stage logic 27 [certain].
- The preview picture and the frame buffer (VRAM x 0–319, y 0–255) are black [certain].

## Overlay, member and lists

- Overlay 22 at `0x800E9860`; stage member 112, a layout of 40 × 6 screens (10 240 × 1 536 px),
  catalog entry "Gate stage 1" (`~/mmx-data/x6/stages/m112-gate-stage-1/`, TMX
  `~/mmx-data/x6/tmx/m112-gate-stage-1/`). `stage_members` and `stage_overlays` give stage 16 =
  member 112, overlay 22; stage 17 = 113, 23; stage 18 = 114, 24; none of the three has an area 1
  [certain].
- `enemy_set_tbl` stage 16 area 0: `0x800FAE00`, 110 records and the end record.
  `fixed_set_tbl`: `0x800FB178`, 6 records [certain].
- Text bank: member 224 for Zero, 223 for X (`text_members`) [certain for the table].
- Boss pack: member 135 (`boss_pack_tbl[16]`); it is loaded in both save states
  (`boss_pack_loaded` = 1) [certain].

### Texture blocks

Member 112 has two texture blocks: `0x10000` and `0x16`. They differ in all but the first 8 KB.
Camera zone 130 (record 5, below) has the texture swap bit: the swap happens in the drop at x
1280–1599, y 1168–1199 [certain for the record]. So the start (above y 1168, x < 1920) uses block
`0x10000` and the rest of the stage uses `0x16` [likely]. Both save states have block `0x16` in
video memory [certain]. `stage.png` draws all the stage with `0x16`, so its start is wrong. The
catalog entry has `texture_split` with `0x10000` for `[0, 0, 0, 1920, 1168]`, `[1, 0, 0, 1920,
1536]` and `[2, 0, 0, 1920, 1536]` (set by eye), so the TMX shows the start with its own block. The
colours of the start come from the palettes of `gate-1`; a save state of the start is needed to
check them [guess that they are right].

## Map

Collision of layer 0, one character per 96 × 32 px (6 × 2 cells; the most important value wins).
`#` 56 solid, `=` 57, `%` 58, `|` 59 boss gate, `~` 60, `^` 63 spikes, `H` ladder 32 and 33, `_`
34, `/` and `\` slopes. Markers: `C` checkpoint marker, `z` camera zone, `T` talk zone, `D` boss
gate, `L` lava controller (main 29), `M` Nightmare Mother, `6` Wolfloid, `f` Batton Bone B81, `b`
laser beamer (main 13), `3` main 33, `4` main 34. Column 0 is x 0; each column is 96 px.

```
    0                                                                          ##%############
   32                                                                          ##%^  #   #   #
   64                                                                          ##%^  |   |M  #
   96                                                                          ##%^  D C D   #
  128                                                                          #%=  %|###|   #
  160                                                                          #%  6^%####   #
  192                                                                          #%H%%^#####   #
  224                                                                ############H% ^#####   ##
  256                                                                ############6  ^#   #   ##
  288                                                                #3   33 3 ##%6 ^=   #   ##
  320                                                                ##        ##%%% 3   #   ##
  352                                                                ##        ##%^% %   #####
  384                                                                ##  6   6 ###^%\%   #####
  416                                                                #########H###^^%H
  448 =================                                              #########H###^=%H
  480              T  =                ########b#####b######################  H###^  H
  512                 =                # z    3   f3 ##########  ##  #######6zH###^ \%
  544 T      ##       =                #  b 6  f##  f ## 3#####  ##    #####H####=^\6#
  576        ##  /##  =                ##b# # # 4#  #b 36  3  |  ||C  z#####Hz     ###
  608     ## ## /###  =                f#zf##3b### 4######b4  D L|D  L #####H   L \###
  640 ##############z#=                ###########b###66##6###|%%||#########H#6#######
  672 ################=                #33####################=%%%=###################
  704 #############=##=   #########    b##b###################=   =###################
  736              =  =   ###=#####    # f#
  768              =  =   ## b=####    3  3
  800              =  =   ^z  ==###    ####
  832              =  =   ^   fb###    ##f#
  864              =  =   ^^##  =##    #ff#
  896              =  =   ^z#^\fb=#    #33#
  928              =  =   6  ^#\ =##   b##b
  960              =  =   #^ ^## z######  #
  992              =  =   ^= ^##~ =#####  #
 1024              =  =   ^  ^##%bb=#### f#
 1056              =  =   ^^6^##%~ =#######
 1088              =  =   ^^####%%\ =###f3#
 1120              =  =   ^  ^##%%#fb=#####
 1152              =  =   6^ ^##z%#~ f##  #
 1184              T z====#^ ^##%%#%z =b#_b
 1216              =         ^##%%#%~\b=##H
 1248              =        6^  %%#%%#fb##H
 1280              =       z##   %#%z#~ z H
 1312              =       ^##   ##%%#%~C H
 1344              =       ^##   ##%%#%%  H
 1376              =##########     %%#%%%%%
 1408              =##########     %%#%%%%%
```

Collision values in layer 0 (cells): 0 (54 671), 56 (5 798), 58 (344), 57 (283), 16 (86), 63 (82),
59 (32), 32 (31), 27 and 28 (24 each), 60 (12), 34 (10), 25 and 26 (9 each), 33 (5), 1–4 (5 each)
[certain].

| part | x (px) | y (px) | what |
|---|---|---|---|
| start | 0–1279 | 448–719 | the lab entrance: snow, rocks and machines; floor at y 640 with small steps and a slope (values 1–4 at x 896–1215). Talk zones 0 and 1 |
| the hole | 1280–1599 | 640–1199 | the drop into the lab ("a big hole ... made for a big oaf"); the texture swap at y 1168 |
| dark room | 1264–1919 | 1184–1439 | checkpoint 1; talk zone 2 ("It's dark here") |
| first climb | 1920–3199 | 720–1439 | a climb up with spikes (63) on the walls; four Wolfloids on ledges, Batton Bones, fixed laser beamers; a glass floor (60) at x 2528–3199, y 1008–1343 |
| second climb | 3200–3519 | 480–1375 | checkpoint 2; a shaft with ladders (32, 33; the first at x 3456) and pairs of crossed lasers (beamers `id` 1 and 3 at x 3240 and 3480) |
| corridor | 3520–5439 | 480–719 | lasers, Wolfloids, Batton Bones, main 33 and the three main 34 |
| lava room 1 | 5440–5759 | 480–655 | between boss gates 24 and 25; lava controller `var` 0 |
| lava rooms 2 and 3 | 5760–7663 | 240–719 | checkpoint 3; lava controllers `var` 1 (x 6080) and `var` 2 (x 7184); then a climb on the 2-cell slopes 25–28 (x 7296–7583) up to y 0 |
| boss corridor | 7664–7983 | 0–159 | between boss gates 39 and 26; checkpoint 4 (this save state) |
| boss room | 7984–8431 | 0–431 | the Nightmare Mother |

The layer 1 map is four copies of a lava band (x 0–3935, y 640–943, 992 px apart): the frames of
the lava that main 29 moves. Layer 2 (the background) has cells only at x 0–2383, y 0–543 [certain
for the cells].

## Objects

110 records (list `0x800FAE00`). `kind` low digit 0–2 main (the minimum difficulty), 3 stage logic,
4 stage object. No item (main 47), Rescue Reploid (stage 4), Light capsule (stage 26) or grey
block record [certain].

| # | kind | b | `id` | object |
|---|---|---|---|---|
| 0–3 | 3 | 6 | 1–4 | checkpoint markers (below) |
| 4–19 | 3 | 0 | 1, 130, 3–16 | camera zones (below) |
| 20–22 | 3 | 14 | 0, 1, 2 | talk zones (below) |
| 23–39 | 0–2 | 6 | 0–3 | 17 Wolfloids ([`wolfloid.md`](../enemies/wolfloid.md)): `id` 2 (flat floor, breath) at (2179, 1264), (1983, 1152), (2179, 1056), (1983, 944), (7248, 272); `id` 3 (flat floor, cannon) at (3696, 544), (4640, 640), (4768, 640), (4864, 576), (5024, 640), (6432, 400), (6768, 512), (6944, 640), (6864, 400), (7440, 160); `id` 0 (slope up to the right) at (7488, 544); `id` 1 (slope up to the left) at (7344, 296). Kinds: 0 ×10, 1 ×1, 2 ×6 |
| 40–56 | 0–2 | 18 | 0 | 17 Batton Bone B81 ([`batton-bone-b81.md`](../enemies/batton-bone-b81.md)), all `id` 0 (with a bomb): (2384, 856), (2480, 904), (2656, 1024), (2848, 1120), (2992, 1168), (3080, 1248), (3312, 1104), (3360, 1032), (3272, 888), (3448, 888), (3360, 840), (3360, 736), (3256, 624), (3464, 624), (3944, 544), (4264, 528), (4424, 544). Kinds: 0 ×1, 1 ×10, 2 ×6 |
| 57–80 | 0–2 | 13 | 0–6 | 24 fixed laser beamers ([`laser-reflector-puzzle.md`](../enemies/laser-reflector-puzzle.md), `var` `0x0n`: direction n; 0 right, 1 up-right, 2 up, 3 up-left, 4 left, 5 down-left, 6 down): `id` 5 at (2296, 792), (2456, 856), (2568, 920), (2744, 1032), (2776, 1048), (2936, 1128), (3192, 1256), (4520, 504); `id` 1 at (3240, 1192), (3240, 952), (3240, 712); `id` 3 at (3480, 1192), (3480, 952), (3480, 712), (4288, 648); `id` 4 at (3392, 592), (3848, 616), (4936, 552), (5080, 624); `id` 6 at (3088, 1216), (4016, 504); `id` 2 at (2592, 1040), (3512, 568); `id` 0 at (4600, 600). No reflector and no door record |
| 81–98 | 0–2 | 33 | 0, 1, 2 | 18 main 33 (new, no sheet): `id` 0 at (3328, 912), (3472, 768), (3328, 672); `id` 1 at (3392, 912), (3248, 768), (3392, 672), (3744, 608), (6192, 312); `id` 2 at (3448, 1104), (3920, 512), (4384, 512), (4768, 576), (4912, 544), (5152, 576), (6528, 288), (6672, 288), (6896, 288), (7624, 336) |
| 99–101 | 0 | 34 | 0 | 3 missile launchers ([`missile-launcher.md`](../enemies/missile-launcher.md), main 34) at (4080, 600), (4336, 632), (5152, 632) |
| 102–105 | 4 | 8 | 24, 25, 39, 26 | boss gates ([`boss-gate.md`](../enemies/boss-gate.md)) at (5440, 624), (5760, 624), (7664, 112), (7984, 112) (below) |
| 106–108 | 0 | 29 | 0, 1, 2 | the lava controllers (main 29, below) at (5591, 608), (6080, 608), (7184, 608) |
| 109 | 0 | 60 | 0 | the Nightmare Mother at (8082, 93) ([`nightmare-mother.md`](../enemies/nightmare-mother.md)) |

The type of each object [likely unless marked]: main 6 has the Wolfloid animation table (33 of 33
animations equal) and set-up (HP 12, contact 4, gravity `0x40000`); main 13 has the laser-puzzle
animation table (34 of 34) and the same 42 functions as overlay 10 (equal sizes; 34 identical after
the addresses are masked), with the same neighbours (visual 11, misc 7); main 18 has the same 33
functions as the Batton Bone B81 of overlay 09 (all identical after masking) [certain for the code
match]. See [`~/mmx-data/x6/re/gate-1/notes.md`](../../work/gate-1/notes.md).

The second list (`0x800FB178`) [certain]:

| # | kind | b | `id` | object |
|---|---|---|---|---|
| 0–4 | 3 | 3 | 0–4 | `PaletteCycle`; all five live in `gate-1` |
| 5 | 3 | 7 | 0 | stage logic 7: the `IceSlide` code (the slide on 60 and the push down slopes 17–28; its 3 walked functions are identical to overlay 4 after masking) [likely]; live |

Stage logic 27 (the stage start banner) is live; it has no record.

### Main 33 and main 34 (new types; main 33 not read in full)

Kits: [`~/mmx-data/x6/re/gate-1/kits/main-33/`](../../work/gate-1/kits/main-33/), `main-34/` [certain for the values quoted].

- **Main 33** (entry `0x800F3B38`): `var` 0–2 run state table `0x800FC674` (set-up `0x800F2410`:
  HP 4, contact 3, `dmg_tbl_normal`; `var` 0 starts with speed x +8 px/frame facing right, `var` 1
  with −8 facing left, `var` 2 with speed y +8 (up); then step 2 when a wall or floor touches it,
  else step 5). `var` ≥ 3 run `0x800FC680` (set-up `0x800F32B0`: HP 31, contact 3,
  `dmg_tbl_deflect`); no record has it, so another object makes it [guess]. Neighbour: visual 36.
- **Main 34** (entry `0x800F4BA4`, set-up `0x800F43A0`): the missile launcher
  ([`missile-launcher.md`](../enemies/missile-launcher.md)); the same 42 functions as main 34 of
  overlay 23 (Gate stage 2) [certain for the code match]. Three records, all on the corridor floor.

### Lava controller (main 29, entry `0x800EBBF8`) [certain for the code]

Main 29 has no sprites and no type-list bank. It moves BG layer 1 (the lava band) and makes the
kill line:

- Set-up (`0x800EA770`): layer 1 x (`0x8009728C`) = −5440 (`var` 0), −6080 (`var` 1) or −7024
  (`var` 2); layer 1 y (`0x8009728E`) = −80; layer 1 mode (`+0x04`) = 2 and shown (`+0x03`) = 0.
  Then five main objects type 29 `var` 10 at the record y, 160 or 176 px apart (`var` 0: x 5568–
  6208; `var` 1: 6208–6912; `var` 2: 7152–7792). Only `var` 0 also holds the player
  (`PlayerEvent(20, facing)`).
- `var` 10: contact damage 127 (death), attack box `0x800FB6F8` = (−120, −120, 240, 240), y = 760
  − layer 1 y.
- Every 10 updates layer 1 x = base + {0, 992, 1984, 2976}[n] (`0x800FB700`, n = 0–3 in turn):
  the four lava copies are the animation frames.
- A rise step (`0x800EAB68`): layer 1 y + 1 every 2 updates until −32, `Quake2(10, 2, 2)` every 10
  updates, then sound (bank 5, id 3) and the next step. The other steps (`0x800EACFC` …) also use
  `Rumble`, `PlayerEvent`, `PlayerEventEnd` and the camera target (not read in full).
- With layer 1 y = −80 the lava top (map y 640) is at world y 720, and the kill boxes reach up to
  y 840 − 120 = 720: the kill line follows the lava top [likely]. So the lava rises while layer 1 y
  grows, and touching it kills [likely].

Boss gates 25 and 39 move layer 1 y back down to −80 (`docs/enemies/boss-gate.md`, "special
behaviour"): they lower the lava after lava room 1 and before the boss corridor [likely].

## Talk zones [certain]

k = `talk_stage_idx`[32] = 10; all three zones are forced. `talk_seen` k 10 = `0x0007`: all three
were seen. Messages of member 224 (Zero):

| record | `id` | box (x, y) | message |
|---|---|---|---|
| 20 | 0 | 0–240, 544–656 | 0: Alia: "The secret laboratory of Gate exists in a place like this? Let's move in!" |
| 21 | 1 | 1296–1328, 464–656 | 1: Alia: the radar is bad. Zero: "There's a big hole. This hole seems to be made for a big oaf to fit through..." Alia: the lab is below; transmission will be lost. Zero: "Don't worry." |
| 22 | 2 | 1280–1600, 1200–1376 | 2: Zero alone: "It's dark here, but I can vaguely see something... Alia, Alia? I've already lost the transmission... Let's see what is here." |

## Camera and checkpoints

### Checkpoints

`checkpoint_start_tbl` entry 32 → `0x80073288` [certain]:

| checkpoint | record | player (x, y) | camera (x, y) | limits x, y | marker |
|---|---|---|---|---|---|
| 0 | `0x80072CFC` | 144, 464 | 0, 464 | 0–1296, 464–1200 | – (start) |
| 1 | `0x80072D20` | 1424, 1184 | 1264, 1184 | 1264–1920, 1184–1184 | record 0 (1440, 1200), the dark room |
| 2 | `0x80072D44` | 3376, 1184 | 3200, 1184 | 3200–3200, 480–1184 | record 1 (3344, 1312), the second climb |
| 3 | `0x80072D68` | 5904, 480 | 5760, 480 | 5760–7360, 480–480 | record 2 (5936, 592), after gate 25 |
| 4 | `0x80072D8C` | 7824, 0 | 7664, 0 | 7664–7664, 0–0 | record 3 (7792, 112), the boss corridor |

All face right. Layer 1 camera (1920, 0) in all five; layer 2 (0, 116), (1264, 296), (800, 296),
(1440, 120), (1916, 0). A sixth pointer (`0x80072DB0`, player (152, 1904)) is past the map; no
marker sets checkpoint 5. It is checkpoint 0 of stage 17 (`docs/stages/gate-2.md`) [certain].

### Camera zones (stage logic 0)

From the TMX `camera zones` layer: the trigger rectangle and the new limits (−1 = keep) [certain
for the table].

| record | `id` | trigger x, y, w × h | min x | max x | min y | max y |
|---|---|---|---|---|---|---|
| 4 | 1 | 1360, 624, 160 × 32 | 1264 | – | – | 1184 |
| 5 | 130 | 1280, 1168, 320 × 32 | – | 1920 | 1184 | – (texture swap) |
| 6 | 3 | 2064, 1216, 32 × 160 | 1920 | – | 720 | – |
| 7 | 4 | 2064, 752, 32 × 96 | – | 3200 | – | – |
| 8 | 5 | 2064, 896, 32 × 64 | – | 1920 | – | – |
| 17 | 14 | 2528, 896, 32 × 112 | – | – | – | 1184 |
| 15 | 12 | 2560, 1120, 64 × 80 | – | – | – | 976 |
| 16 | 13 | 2624, 960, 32 × 112 | – | – | – | 1184 |
| 18 | 15 | 2880, 1232, 64 × 96 | – | – | – | 1088 |
| 19 | 16 | 2944, 1136, 32 × 112 | – | – | – | 1184 |
| 14 | 11 | 3296, 592, 128 × 32 | 3200 | 3200 | – | – |
| 9 | 6 | 3344, 1040, 32 × 336 | 3200 | – | 480 | – |
| 10 | 7 | 3424, 496, 32 × 80 | – | 5120 | – | – |
| 11 | 8 | 6176, 512, 96 × 128 | 6080 | 6704 | 240 | – |
| 12 | 9 | 6848, 464, 32 × 64 | 6704 | 6704 | 240 | – |
| 13 | 10 | 6848, 576, 32 × 80 | – | – | 0 | – |

### Boss gates [certain for the code of `docs/enemies/boss-gate.md`]

| record | `id` | x | after the pass | special |
|---|---|---|---|---|
| 102 | 24 | 5440 | camera x 5440 fixed, max y 480 | – |
| 103 | 25 | 5760 | camera x 5760–7360 | at the trigger: layer 1 y down to −80 (1 px per 2 frames), quakes |
| 104 | 39 | 7664 | camera fixed at (7664, 0) | layer 1 y down 1 px per frame for 112 frames |
| 105 | 26 | 7984 | camera x 7984–8096, max y 160 | – (the boss room) |

So gates 39 and 26 are the boss corridor pair. `gate-2` has limits x 7984–8096, y 16–144 during
the fight.

## Sprites

`spriteexport` on `gate-1` gives no good enemy sheet: the boss pack (member 135, entry `0x10411`)
is in VRAM pages 7–14, where the cells of banks 1–6 (main 15, 6, 18, 13, 33, 34) are [certain:
about 60 % of the non-empty 32-byte pieces of pages 7–14 are pieces of that entry; 0 % in pages 5,
6 and 15]. The seven search exports were removed and are listed as skipped in
`sprites/stages/gate-1.json`. The boss gate (bank 0, page 6) is correct. The Nightmare Mother's
own sprite source is in `docs/enemies/nightmare-mother.md`. A save state at checkpoint 0–3 is
needed for the enemy sprites. `gate-3_stage-2` (stage 17) gives the main 34 sheet
(`missile-launcher`, `enemy-eb6483d4fb`) and the main 15 sheets; the other types are not in the
stage 17 type list.

## Open questions

1. The official name of main 33 and what it does (only the set-up is read); the official name of
   the missile launcher (main 34).
2. The other steps of main 29: when each lava room starts and stops, and layer 1 mode 2.
3. Whether collision 58 (344 cells, walls of x 2528–7679) does anything in this stage.
4. The colours and texture regions of the start: a save state at checkpoint 0 is needed.
5. Member 223 (the X text bank of stage 16) does not decode with `mmxdemake.text`.
