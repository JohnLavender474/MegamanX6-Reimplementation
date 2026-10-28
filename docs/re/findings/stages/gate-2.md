# Stage: Gate stage 2, section 1 (stage 17, area 0, overlay 23)

The second stage of Gate's lab has two sections: stage 17 (this page) and stage 18 (the way to
Gate; `docs/damage.md`, "Main type 59: Gate"). Stage 16 is Gate stage 1 ([`gate-1.md`](gate-1.md)).
Stage music 19, boss music 23 (`docs/sound.md`) [certain for the tables]. Section 1 ends with
**High Max** (main 55, record 112, `var` 2): the stage overlay holds the High Max code, and the
record list has main 55 `id` 2 in the boss room ([`high-max.md`](../enemies/high-max.md))
[certain for the code and the record]. His death goes straight on to stage 18 ("Stage end",
below). The stage is a
shaft with ropes between spike walls, a long corridor of machine platforms with falling rocks,
Nightmare Viruses, orb robots and missile launchers, a tall room with ladders, rocket platforms and
totem poles, a boss corridor and the boss room at the top right.

## Save states

| state | checkpoint | player (x, y), HP | camera (x, y), limits | what |
|---|---|---|---|---|
| `gate-3_stage-2` | 1 | Zero, 1216, 1290, 28 | 1056, 1189; x 368–3200, y 5–1200 (target y 1040–1200) | the corridor, on the machine platforms; two orb robots (main 11) live |

- Zero (`game+0x38` = 1, form 5), step 2 (animation 17, the end of a dash); difficulty 1; lives 2;
  `boss_choice` 2; `nightmare_effect` 0; stage time 1638 [certain].
- `game+0x60` = `0xFF`, `+0x61` = 56, `+0x62` = 52; the six gauges are 312 = `mega+0x14A` (Zero:
  6 × `game+0x62`, as in `gate-1_before-boss`). Owned and equipped parts are the same as in
  `gate-1_before-boss` [certain].
- Live: main 11 (records 41 and 42, at (1141, 1307) and (1288, 1364)), main 31 `var` 2 (record 82,
  at (1408, 1232), step 0: the blink before the attack), visual 10 (an orb shot), three
  `PaletteCycle` (`var` 0, 2, 3) and stage logic 27 (the stage start banner) [certain].
- The current limit min y (5) moves to its target 1040 (camera zone record 5) [certain for the
  values].

## Overlay, member and lists

- Overlay 23 at `0x800E9860` (66 444 bytes, up to `0x800F9BEC`; 30 bytes differ from
  `rock_bin/23.bin` in RAM); stage member 113, a layout of 24 × 9 screens (6 144 × 2 304 px; the
  art uses x 0–3551, y 208–2159), catalog entry "Gate stage 2"
  (`~/mmx-data/x6/stages/m113-gate-stage-2/`, TMX `~/mmx-data/x6/tmx/m113-gate-stage-2/`). No area 1
  [certain].
- `enemy_set_tbl` stage 17 area 0: `0x800F8170` (overlay offset `0xE910`), 113 records and the end
  record. `fixed_set_tbl`: 3 records [certain].
- Text bank: member 226 for Zero, 225 for X (`text_members`). Message 0 is the High Max talk
  (Zero: High Max "Just die! Die, original one!", Zero "What is he saying? Anyway, It's time to
  finish!"; X: High Max "Data-wise, I've surpassed you... There's no chance for me to lose!").
  Message 1 is Gate's talk (stage 18, `docs/damage.md`) [certain for the text; likely that the
  `var` 2 fight opens message 0].
- Boss pack: member 136 (`boss_pack_tbl[17]`); not loaded in `gate-3` (`boss_pack_loaded` = 0).
  Banks 14 and 15 (type 55) have no texture. No record makes `BossPackLoad` (stage logic 4); which
  object starts the load is not read [certain for the RAM; open for the loader].
- Talk zones: none. `talk_stage_idx`[34] = 11, and the boxes of k 11 are all 0 [certain].

### Texture blocks

Member 113 has two texture blocks: `0x10000` and `0x16`. VRAM holds block `0x10000` in `gate-3`
(`stage_texture_swap` = 0) [certain]. Camera zone 133 (record 8; zone 5 with the texture swap bit)
is at x 2384–2415, y 1248–1375, in the corridor just before checkpoint 2 [certain for the record].
So the shaft and the corridor (x < 2384) use block `0x10000`, and the tall room, the boss corridor
and the boss room use `0x16` [likely]:

- Drawn with `0x10000`, the corridor is correct and the tall room is broken (ladders cut, the boss
  room machines are noise). Drawn with `0x16`, the tall room is correct (ladders, spikes and the High
  Max machines join) and the corridor has wrong cells [seen in the two drawings].
- The cells of x 2224–2591 are the same in both blocks, so the swap does not show [certain for the
  drawings]. The background (layer 2, x 0–1279, y 0–719) is the same in both blocks. Layer 1 has no
  cell.
- The catalog entry has `texture_split` with block `0x16` for `[0, 2384, 0, 6144, 2304]`. The
  colours of the tall room come from the palettes of `gate-3`; a save state there is needed to check
  them [guess that they are right].

## Map

Collision of layer 0, one character per 64 × 32 px (4 × 2 cells; the most important value wins).
`#` 56 solid, `=` 57, `|` 59 boss gate, `^` 63 spikes, `H` ladder 32 and 33, `_` 34, `r` 46
rope. Markers: `C` checkpoint marker, `z` camera zone, `D` boss gate, `M` High Max, `f` Nightmare
Virus, `s` orb robot (main 11), `o` falling rock (main 31), `4` missile launcher (main 34), `p`
totem pole (stage 3), `e` rocket platform (stage 5), `i` item. Column 0 is x 0; each column is
64 px.

```
  192                                                  =======
  224                                         #########=     =
  256                                         ##########     #
  288                                         ##########     #
  320                                         #    |   | M   #
  352                                         #z   D C D     #
  384                                         H_########     #
  416                                         H###############
  448                                         H###############
  480                                         H##############
  512                                         H^^^^^^^^     #
  544                                         Hz  e  ^^ ff  #
  576                                         H      ^^  f  #
  608                                         #   ^^    f   #
  640                                         #####e e  =##_H
  672                                         #####^   ^####H
  704                                         ####=     =###H
  736                                         #  #      ##  H
  768                                         #  #      ##  H
  800                                         #  #  p ff## pH
  832                                         #             H
  864                                         #             H
  896                                         H_#^#######^^##
  928                                         H##############
  960                                         H             #
  992                                         H   f         #
 1024      ##############################     H   f      f  #
 1056      ##############################     H p e   p     #
 1088      #     ooo o ooo ooooo oo f  f#     H         e   #
 1120      #    ##### #  oo # #ofoo f ff#     ##=   s#=   =_H
 1152      #       z     ##   # # z  ff #     ##^  ^###^  ^#H
 1184 ############## ## #### #### ######i#######^   =#=   =#H
 1216 #############oo#oo####oo###oo#############^        f  H
 1248 #              #f                           f         H
 1280 #  r    z  zo  #   s    f sf  z      z    z f   p  fp H
 1312 #  r    C   o     ##      #          C       ###  ##  H
 1344 #  oz       o4  4 ##s 4 s # 4                ###s ## s#
 1376 ##^r^####### ######## # #   # #########################
 1408 ##^rz####### ######## # #   # #########################
 1440 ^  r^      = =      = = =   = =
 1472 ^ror^      = =      = = =   = =
 1504 ^o r^
 1536 ^r r^
 1568 ^ror^
 1600 ^r^^^
 1632 ^o^##
 1664 ^r=r^
 1696 ^ror^
 1728 ^r o^
 1760 ^  r^
 1792 ^  o^
 1824 ^   ^     #
 1856 #^r^#     #
 1888 #^r^#
 1920 ^ r ^
 1952 ^rrr^
 1984 ^rrr^
 2016 ^rrr^
 2048 ^   ^
 2080 ^###^
 2112 #####
 2144 #####
```

Collision values in layer 0 (cells): 0 (51 688), 56 (3 224), 63 (157), 57 (83), 46 (65), 32
(51), 59 (16), 34 (8), 33 (4) [certain]. Value 46 (rope) is only in the shaft (x 96–223,
y 1296–2031); the ladders (32, 33) are only in the tall room (x 2608 and x 3471).

| part | x (px) | y (px) | what |
|---|---|---|---|
| shaft | 0–319 | 1376–2143 | the start (checkpoint 0 at the bottom); a climb up on ropes (46) between two spike walls (63); falling rocks |
| corridor | 0–2559 | 1024–1439 | checkpoint 1 (x 528); a floor with gaps at y 1344, machine platforms at y 1088–1232 and a row of falling rocks under the ceiling; Nightmare Viruses, orb robots, four missile launchers on the floor; an item at (2192, 1184); the texture swap at x 2384 and checkpoint 2 (x 2416) |
| tall room | 2560–3551 | 384–1439 | a climb up to y 384 on ladders at the left and right walls; spike rows (63); five rocket platforms, six totem poles, Nightmare Viruses, an orb robot |
| boss corridor | 2880–3183 | 224–415 | between boss gates 28 (x 2896) and 29 (x 3184); checkpoint 3 (x 3040) |
| boss room | 3184–3535 | 208–415 | High Max (record 112, (3312, 336)); floor at y 416; camera x 3184–3216, y 224 |

The boss room agrees with the High Max room of `docs/enemies/high-max.md`: room origin (3184, 224)
(the camera min x and min y), 352 px wide, floor at room y 192 = world y 416 [certain for the
values].

## Objects

113 records (list `0x800F8170`). `kind` low digit 0–2 main (the minimum difficulty), 3 stage
logic, 4 stage object. No Rescue Reploid (stage 4), Light capsule (stage 26), talk zone or grey
block record [certain].

| # | kind | b | `id` | object |
|---|---|---|---|---|
| 0–2 | 3 | 6 | 1, 130, 3 | checkpoint markers (below); `id` 130 = checkpoint 2, set by the player's y |
| 3–13 | 3 | 0 | 0–4, 133, 6–10 | camera zones (below) |
| 14–40 | 0–2 | 15 | 0, 2 | 27 Nightmare Viruses ([`nightmare-virus.md`](../enemies/nightmare-virus.md)): `id` 2 (row 2: IDLE, DASH, SHOOT, DASH) ×15, `id` 0 (row 0: IDLE, SHOOT) ×12; no snatcher. In the corridor x 1048–2144 (records 14–27) and in the tall room (28–40). Kinds: 0 ×14, 1 ×8, 2 ×5 |
| 41–47 | 0–1 | 11 | 0 | 7 orb robots ([`illumina-orb.md`](../enemies/illumina-orb.md), main 11): (1264, 1296), (1328, 1360), (1584, 1360), (1696, 1296), (3104, 1360), (3424, 1360), (2960, 1120). Kinds: 0 ×5, 1 ×2 |
| 48–53 | 4 | 3 | 0 (`a` 48) | 6 totem poles ([`totem-pole.md`](../enemies/totem-pole.md), the base, `var` 0): (3120, 1296), (3376, 1296), (3120, 1056), (2688, 1056), (2976, 816), (3408, 816) |
| 54–57 | 0–2 | 34 | 0 | 4 missile launchers ([`missile-launcher.md`](../enemies/missile-launcher.md), main 34) on the corridor floor: (880, 1344), (1056, 1344), (1440, 1344), (1824, 1344) |
| 58–62 | 4 | 5 | 0, 1 | 5 rocket platforms ([`rocket-platform.md`](../enemies/rocket-platform.md)): `id` 0 (horizontal) at (3256, 1088), (3056, 664), (2816, 560); `id` 1 (vertical) at (2824, 1072), (2936, 640) |
| 63–108 | 0–1 | 31 | 0 | 46 falling rocks ([`falling-rock.md`](../enemies/falling-rock.md)): 10 in the shaft (x 80–240, y 1344–1808), 36 in the corridor (x 736–1792, y 1104–1344) |
| 109 | 0 | 47 | 34 | item (`var` `0x22`) at (2192, 1184) |
| 110, 111 | 4 | 8 | 28, 29 | boss gates ([`boss-gate.md`](../enemies/boss-gate.md)) at (2896, 352) and (3184, 352) |
| 112 | 0 | 55 | 2 | High Max, `var` 2, at (3312, 336) |

The type of each object [certain for the code match unless marked]; the masked compare is
[`~/mmx-data/x6/re/gate-stage-2/scripts/ovsearch.py`](../../work/gate-stage-2/scripts/ovsearch.py) and `cmpcode.py` (immediates and jump targets
masked):

- Main 11: the 28 functions of the overlay 23 kit are identical to the 28 of the overlay 11 kit of
  main 11 (Illumina's orbs, [`~/mmx-data/x6/re/mijinion-bot/kit-main11/`](../../work/mijinion-bot/kit-main11/)). The live objects use
  the animation table of the `illumina-orb` export (pixel-equal sheets).
- Main 15: main-program code (Nightmare Virus); the sheets of bank 1 are pixel-equal to the
  shared `nightmare-virus` and `nightmare-virus-orb` sheets.
- Main 31: all 10 functions are in overlay 08 at the addresses of `FallingRockInit`
  (`0x800EF13C`), `FallingRockMain` (`0x800EF340`), `FallingRock` (`0x800EF6B8`) and the steps.
  Sprite type 133 (bank 3), as in overlay 08.
- Main 34: the 42 functions are identical to the main 34 kit of overlay 22 (Gate stage 1).
- Main 55: all 158 functions of the overlay 23 kit are in overlay 44 (High Max) and in overlay 02
  (the intro).
- Stage 3: the 28 functions are identical to the overlay 08 kit (totem pole). Here the records
  make the base directly (`id` 0 = `var` 0) [likely: the record `id` is the `var`]; in stage 5 the
  section goal makes it. `a` = 48 is not read.
- Stage 5: the 5 functions are identical to the overlay 09 kit (rocket platform).
- Stage 8: boss gate (main program; 12 of 14 walked functions identical to the boss-gate kit).

The second list [certain]:

| # | kind | b | `id` | object |
|---|---|---|---|---|
| 0–2 | 3 | 3 | 0, 2, 3 | `PaletteCycle`; all three live in `gate-3` |

## Camera and checkpoints

### Checkpoints

`checkpoint_start_tbl` entry 34 → `0x8007329C` [certain]:

| checkpoint | record | player (x, y) | camera (x, y) | limits x, y | marker |
|---|---|---|---|---|---|
| 0 | `0x80072DB0` | 152, 1904 | 0, 1904 | 0–0, 1200–1904 | – (start, the bottom of the shaft) |
| 1 | `0x80072DD4` | 528, 1200 | 368, 1200 | 368–3200, 1200–1200 | record 0 (528, 1312) |
| 2 | `0x80072DF8` | 2400, 1200 | 2240, 1200 | 2240–3200, 1200–1200 | record 1 (2416, 1312), `id` 130: set when the player's y reaches 1312 |
| 3 | `0x80072E1C` | 3040, 224 | 2880, 224 | 2880–2880, 224–224 | record 2 (3040, 352), the boss corridor |

All face right. Layer 1 camera = layer 0 camera for 0–3; layer 2 (0, 476), (92, 300), (560, 300),
(720, 56). `mega+0xCE` (`beam_thru`) 8, 4, 4, 13. The next pointers of the list (`0x80072E40` …)
are the records of stage 18 [likely]. `gate-1.md` calls `0x80072DB0` "a sixth pointer past the
map": it is checkpoint 0 of this stage [certain].

A restart at checkpoint 2 or 3 is after the texture swap. Whether the restart loads block `0x16` is
not known (the same question as in `docs/stages/sheldon.md`).

### Camera zones (stage logic 0)

From the TMX `camera zones` layer: the trigger rectangle and the new limits (−1 = keep) [certain
for the table].

| record | `id` | trigger x, y, w × h | min x | max x | min y | max y |
|---|---|---|---|---|---|---|
| 4 | 1 | 144, 1408, 144 × 32 | – | 0 | – | 1904 |
| 3 | 0 | 256, 1248, 32 × 128 | – | 3200 | – | 1200 |
| 5 | 2 | 528, 1248, 32 × 128 | 368 | 3200 | 1040 | 1200 |
| 11 | 8 | 720, 1248, 32 × 128 | 368 | – | – | – |
| 10 | 7 | 832, 1072, 32 × 128 | 320 | – | – | – |
| 6 | 3 | 1808, 1136, 32 × 64 | – | 1920 | – | – |
| 7 | 4 | 1920, 1248, 32 × 128 | – | 3200 | – | – |
| 8 | 133 | 2384, 1248, 32 × 128 | 2240 | – | 1200 | 1200 (texture swap) |
| 9 | 6 | 2720, 1248, 32 × 128 | 2560 | – | 240 | – |
| 12 | 9 | 2640, 320, 32 × 64 | – | 2576 | – | – |
| 13 | 10 | 2640, 528, 32 × 112 | – | 3200 | – | – |

### Boss gates [certain for the code of `docs/enemies/boss-gate.md`]

| record | `id` | x | after the pass |
|---|---|---|---|
| 110 | 28 | 2896 | camera x 2880 fixed (the boss corridor) |
| 111 | 29 | 3184 | camera x 3184–3216, y 224 (the boss room) |

## Stage end [certain for the code]

- High Max's death code (`0x800F0CE4` in overlay 23, the same code as overlay 44) sets
  `game.exit_req` = `0x10` in every stage but 12 (stage 12: `0x80` and checkpoint + 9).
- `StageExit` (`0x8001E32C`) with an exit request above 0: stage 16 → game mode 19 with
  `game+0x1D` = 8 and `game+0x66` = 4 (a scene); **stage 17 → `game.stage` = 18 and game mode 8**
  (the same mode that the intro and area changes use; that it loads the next stage at once, with
  no scene, is likely); stage 18 → game mode 19 with `game+0x1D` = 9 and
  `game+0x66` = 5. So stages 17 and 18 play as one stage with two sections (section 2:
  [`gate-2b.md`](gate-2b.md)), and only stages 16 and
  18 end with a scene.
- In the record list nothing else ends the stage: no exit object, no teleporter crystal [certain
  for the records].

## Sprites

`spriteexport` on `gate-3` (on a copy of the shared folder, with `"17": "gate-2"`):

| export | type | what | check |
|---|---|---|---|
| `enemy-eb6483d4fb` (new) | main 34 | the missile launcher: 14 animations | good; the first good sheet of main 34 (the `gate-1` sheet `enemy-4c7523f990` was covered by the boss pack) |
| `enemy-c3301f6267` (`illumina-orb`) | main 11 (live) | the orb robot | pixel-equal to the shared sheets; a new source line |
| `enemy-363dce0a1a` (`nightmare-virus`), `enemy-a96f0b9d66` (`nightmare-virus-orb`) | main 15 | | pixel-equal; new source lines |

- Not exported (no texture): types 0 and 55 (banks 7–15; the boss pack is not loaded).
- Not exported (misc-type banks, `docs/TODO.md`): the falling rock (main 31, bank 3, type 133),
  the rocket platform (stage 5, bank 2, type 131) and the totem pole (stage 3, bank 6, type 130).
  Their bank sheets (`banks/gate-2/gate-3_stage-2/bank-2.png`, `bank-3.png`, `bank-6.png`) are
  correct. Bank 0 (type 128) holds the Light capsule and boss gate sprites.
- Of the Gate stage 1 enemies, only main 34 and main 15 are in the stage 17 type list (128, 15,
  131, 133, 34, 11, 130, 55, 55). The Wolfloid (main 6), Batton Bone B81 (main 18), the laser
  beamers (main 13) and main 33 need a Gate stage 1 save state at checkpoints 0–3.

## Open questions

1. The kind of item `0x22` (record 109). `docs/engine.md` lists kinds `0x20`–`0x26` as a guess.
2. Which object starts `BossPackLoad` in this stage (no stage logic 4 record).
3. The colours of the tall room (block `0x16`) and the texture after a restart at checkpoint 2.
4. The meaning of `a` = 48 on the totem pole records, and whether the six poles count for
   `game+0x2F` here.
5. The official names of the orb robot and the missile launcher.
