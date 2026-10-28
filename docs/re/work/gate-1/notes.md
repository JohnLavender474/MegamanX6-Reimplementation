# Gate stage 1 (stage 16, area 0): notes

Save states: `gate-1_before-boss.state.gz` (SHA-1 `585948ebe43e9241ae0828aff8654b2e9b98ea04`), and for
a check `gate-2_nightmare-mother.state.gz` (the boss fight; the boss is the work of
[`~/mmx-data/x6/re/nightmare-mother/`](../nightmare-mother/)). User's words: "slot 1, Zero in Gate stage 1, in the room
before the boss".

## What is new

Before this state no dump, catalog entry, stage page, map or sprite export of stage 16 existed
(`catalog/stages.json`, `catalog/savestates.json`, `docs/stages/`, [`~/mmx-data/x6/stages`](../stages/), `tmx`,
`sprites/stages/`, [`~/mmx-data/x6/re/`](../): no `stage 16`, `member 112` or `overlay 22`) [certain].

- The first map of Gate's lab: member 112, 40 × 6 screens, overlay 22.
- Stage 16 = Gate stage 1; boss Nightmare Mother (main 60).
- New main types (stage 16 only so far): main 29 (the lava controller, no sprites), main 33, main 34.
- Known objects in a new overlay: Wolfloid (main 6), Batton Bone B81 (main 18), the laser puzzle
  (main 13, fixed beamers only), `IceSlide` code (stage logic 7), boss gates 24, 25, 39, 26.
- Boss gate ids 25 and 39 (the layer-1 lowering) now have a user: this stage.
- Zero's gauge maximum is 312 here: `mega+0x14A` = 6 × `game+0x62` for Zero, 6 × `game+0x61` for X
  (80 save states agree; `docs/players/weapons.md` gives only `game+0x61`).
- No sprite export is good: the boss pack covers every enemy bank (below).

## Method

Scripts are in `scripts/`; throwaway files in the scratchpad folder `gate-1/`.

1. `where.py` (the caller's) and [`~/mmx-data/x6/re/heatnix-stage/scripts/live.py`](../heatnix-stage/scripts/live.py) for the state
   and the live objects.
2. Tables read from the executable: `stage_members` `0x8006D9FC`, `stage_overlays` `0x8006DB78`,
   `text_members` `0x8006DBD4`, `stage_music` `0x8007093C`, `boss_music` `0x800709F4`,
   `boss_pack_tbl` `0x8007A0E0`, `talk_stage_idx` `0x8007A124`, `talk_boxes` `0x8007A154`,
   `checkpoint_start_tbl` `0x80073300`.
3. `learn` on a copy of `catalog/` (scratchpad), then `stages` and `tmx` with a one-entry catalog
   into `stages/` and `tmx/` of this folder.
4. `rekit --state gate-1_before-boss.state.gz --type N --out kits/main-N` for main 6, 13, 18, 29,
   33, 34 and stage logic 7. `scripts/cmpcode.py` compares two kits (instructions with the
   immediates and jump targets masked).
5. `spriteexport --root sprites-root` on a copy of `~/mmx-data/x6/sprites/`, with a catalog copy
   that has `"16": "gate-1"`. Every new sheet checked by eye.
6. `scripts/tmx_view.py` draws TMX layers to PNG; `scripts/contact.py` makes one picture of an
   export folder; `scripts/ascii_map.py` makes the collision map of the stage page.

## Stage numbers and tables [certain for the table values]

| stage | member | overlay | stage music | boss music | text X / Zero | boss pack |
|---|---|---|---|---|---|---|
| 16 | 112 | 22 | 19 | 21 | 223 / 224 | 135 |
| 17 | 113 | 23 | 19 | 23 | 225 / 226 | 136 |
| 18 | 114 | 24 | 19 | 26 | 225 / 226 (as 17) | 137 |

Area 1 of 16, 17, 18: member 0, overlay 0 (no area 1). `docs/sound.md` already lists track 19 for
stages 16–18 and the boss tracks 21 (16), 23 (17), 26 (18, Gate). `docs/damage.md` has stage 18 =
overlay 24 = Gate. The order of the three Gate stages [likely]: stage 16 (Nightmare Mother, this
state), 17 (High Max: boss pack 136 has the High Max data, `docs/enemies/high-max.md`), 18 (Gate).
Recomp's boss HP table lists "Nightmare Mother", "High Max (Secret Lab)", "Gate", "Sigma" in that
order (`MegaManX6Recomp/tools/tweaks_domain_psxmod.py`, names only).

- `text_members[16 × 2 + 1]` = 224. Member 224 decodes (3 messages, below). Member 223 does not
  decode with `mmxdemake.text --member 223` (exit 1, not read further) [certain for the run].
- `boss_pack_loaded` (`0x800CC868`) = 1 in both states [certain].

## The state [certain]

- Stage 16 area 0, checkpoint 4, stage time 11 236, exit request 0. Zero (`game+0x38` = 1, form 5),
  difficulty 1, lives 1, `boss_choice` 2, `nightmare_effect` 0.
- Player step 2 at (7824, 122), facing right, HP 42. Layer 0 camera (7664, 0); limits min x = max
  x = 7664, min y = max y = 0: the corridor between boss gates 39 and 26.
- Live: stage objects 0 and 1 = boss gates `id` 39 (7664, 112; state 1 step 0) and `id` 26 (7984,
  112; state 1 step 0); stage logic: 5 `PaletteCycle` (`var` 0–4), stage logic 7, stage logic 27.
  No main, visual or misc object.
- Layer 1: shown (`+0x03` = 1), mode 5, `+0x40`/`+0x42` = (0, 0). Layer 2: not shown.
- The preview picture is black. So is the frame buffer: VRAM pages 0–4 (x 0–319, y 0–255) are all
  zero in both states.
- `gate-2_nightmare-mother`: checkpoint 4, time 12 995, Zero at (8152, 330), camera (7992, 144),
  limits x 7984–8096, y 16–144; main 60 `var` 0–4 live (the boss and four parts); gate 26 in
  state 2 (closed for good).

## Map: what `learn` gives, and what the state can show

`learn`: member 112, 40 × 6 screens, overlay 22 at layout offset `0x112D0`, edges 0.956 / 0.969,
0 cells differ from the disc, 110 records at offset `0x115A0` (`0x800FAE00`), palettes at (0, 486)
from 247 tiles, texture entry `0x16` [certain for the run].

- **Layout:** the whole layout is in RAM and equal to the disc (0 differing cells). So checkpoint 4
  does not limit the layout or the record list [certain].
- **Textures:** member 112 has two 256 KB blocks, `0x10000` and `0x16`; only the first 8 KB of the
  two are equal. VRAM holds block `0x16`. Camera zone record 5 (`id` 130 = zone 2 with the texture
  swap bit) is the drop at x 1280–1599, y 1168–1199. So the start (x 0–1919 above y 1168) uses block
  `0x10000` and the rest uses `0x16` [likely]. `stage.png` draws everything with `0x16`, so the
  start is wrong there. The TMX has `texture_split` with block `0x10000` for the regions
  `[0, 0, 0, 1920, 1168]`, `[1, 0, 0, 1920, 1536]`, `[2, 0, 0, 1920, 1536]` (set by eye from the two
  drawings, as for the other stages): the start then shows ice, rock and machines that join the
  rest [likely]. The colours of the start come from the palettes in this state; they look correct
  but are not checked against a state of the start [guess].
- **Layers:** layer 0 art x 0–8431, y 0–1439; layer 1 is four copies of a lava band (x 0–3935,
  y 640–943; copies 992 px apart, the frames of main 29); layer 2 (the background) x 0–2383,
  y 0–543 only [certain for the cells].

### Catalog entries that `learn` adds (from the run on the copy)

`catalog/savestates.json` (sorted by name: the new entry is the first one):

```json
  {
    "name": "gate-1_before-boss.state.gz",
    "note": "Zero in Gate's lab stage 1 (stage 16 area 0), checkpoint 4, at (7824, 122) in the room before the Nightmare Mother's gate",
    "sha1": "585948ebe43e9241ae0828aff8654b2e9b98ea04"
  },
```

`catalog/stages.json` (sorted by member, after member 103 and before member 119), with the
`texture_split` added by hand (`learn` keeps it on a later run):

```json
  {
    "differing_cells": [],
    "edge": {
      "h": 0.956,
      "h_joins": 83,
      "v": 0.969,
      "v_joins": 36
    },
    "height": 6,
    "member": 112,
    "name": "Gate stage 1",
    "objects": {
      "count": 110,
      "offset": 71072,
      "overlay": 22,
      "table_area": 0,
      "table_entry": 16
    },
    "offset": 70352,
    "overlay": 22,
    "palettes": {
      "agree": 0.996,
      "cameras": [
        [
          7664,
          0
        ],
        null,
        null
      ],
      "clut_x": 0,
      "clut_y": 486,
      "matches": 247
    },
    "save_state": "gate-1_before-boss.state.gz",
    "save_state_sha1": "585948ebe43e9241ae0828aff8654b2e9b98ea04",
    "texture": 22,
    "texture_split": {
      "regions": [
        [0, 0, 0, 1920, 1168],
        [1, 0, 0, 1920, 1536],
        [2, 0, 0, 1920, 1536]
      ],
      "texture": 65536
    },
    "width": 40
  },
```

Commands (main session):

```
python3 -m mmxdemake.learn --state gate-1_before-boss.state.gz --name "Gate stage 1" \
  --note "Zero in Gate's lab stage 1 (stage 16 area 0), checkpoint 4, at (7824, 122) in the room before the Nightmare Mother's gate"
# then add texture_split to the new entry
```

Nothing blocks `learn`. The output folders are `m112-gate-stage-1` (stages and TMX). The name
"Gate stage 1" gives the slug `gate-stage-1`; a name with an apostrophe ("Gate's lab stage 1")
gives `gate-s-lab-stage-1`, so it is not used.

The drawings in this folder: `stages/m112-gate-stage-1/` (`stage.png`, `collision.png`,
`stage.ora`, `objects.json`, `layout.json`) and `tmx/m112-gate-stage-1/gate-stage-1.tmx`.

A state of the start (checkpoint 0 or 1, before the drop) would give the real colours of the
start and a check of the texture regions. `gate-2` does not add map data (same layout, same VRAM
block; the boss room is in this map).

## Records (list `0x800FAE00`, 110 records; second list `0x800FB178`, 6 records) [certain]

Full list: `stages/m112-gate-stage-1/summary.txt`. Grouped in `area-page.md`.

- Main 47 (items), stage 4 (Rescue Reploids), stage 26 (Light capsules), stage 12 (grey blocks):
  no record.
- Kind low digit 0–2 = minimum difficulty (`docs/objects-legend.md`).

## Object identification

| type | evidence | result |
|---|---|---|
| main 6 | animation table `0x800FBA80`: all 33 animations equal to the Wolfloid export; set-up `0x800EC670`: HP 12, contact 4, gravity `0x40000`, `dmg_tbl_normal`, step by `var` and `mega.var` (as `WolfloidInit` of overlay 4); `cmpcode`: all 9 functions of the overlay 4 kit are in the overlay 22 kit (which walks 21) | Wolfloid [likely, close to certain] |
| main 13 | animation table `0x800FBD9C`: 34 of 34 equal to `laser-reflector-puzzle`; `cmpcode` against `sheldon-16` type 13: 42 functions of the same sizes in the same order, 34 identical after masking; `dmg_tbl_reflector`, `dmg_tbl_reflector_wait`, `CopyTiles`, `SpawnInRect`, `GetWeaponObject`; neighbours visual 11 and misc 7, as the laser and its draw pieces in stage 7 | the laser puzzle code [likely]. Record ids 0–6 only: `var` `0x0n` = fixed beamer, direction n |
| main 18 | 33 functions of the same sizes as the `turtloid-1` type 18 kit, all 26 distinct ones identical after masking; animation table equal (8 of 8) | Batton Bone B81 [certain for the code match] |
| logic 7 | the 3 functions of the overlay 22 kit (281, 216, 8 instructions) are identical after masking to 3 of the 4 of the overlay 4 kit (`IceSlide`) | `IceSlide` code [likely] |
| main 29 | below | lava controller [likely for the meaning] |
| main 33 | set-up `0x800F2410` (`var` 0–2): HP 4, contact 3, `dmg_tbl_normal`; `var` 0: speed x +8 px/frame, faces right; 1: −8, faces left; 2: speed y +8 (up); then step 2 if `TerrainTouchFlags` finds a wall, else 5. `var` ≥ 3 (state table `0x800FC680`, set-up `0x800F32B0`): HP 31, contact 3, `dmg_tbl_deflect`; no record has it. Neighbour visual 36 (kit); 27 animations, no match in the exports | new, not named |
| main 34 | set-up `0x800F43A0`: HP 30, contact 4, `dmg_tbl_normal`; reads `game.character`; neighbours visual 37 and 38; uses `GetEffectObject`, `SpawnExplosionRand`, `DropItem`; 14 animations, no match | new, not named |
| main 60 | the boss (gate-2: 5 live objects) | Nightmare Mother, other agent |

### Main 29: the lava controller (overlay 22, entry `0x800EBBF8`) [certain for the code]

- State table `0x800FB708`: set-up `0x800EA770`, main `0x800EAA2C`, delete `0x800EBC44`.
- Set-up by `var`:
  - `var` 0 (record 106, (5591, 608)), 1 (107, (6080, 608)), 2 (108, (7184, 608)): layer 1 x
    (`0x8009728C`) = −5440 / −6080 / −7024, layer 1 y (`0x8009728E`) = −80, layer 1 `+0x04`
    (mode) = 2, `+0x03` (shown) = 0, `camera_layer0+0xA0` = 1; then 5 main objects type 29 `var` 10
    at the record y and x = 5408 + 160 k / 6032 + 176 k / 6992 + 160 k (k = 1–5); step 2 / 3 / 4.
    Only `var` 0 calls `PlayerEvent(20, mega.flip)` (the player holds still).
  - `var` 10: contact damage 127, attack box `0x800FB6F8` = (−120, −120, 240, 240), y = 760 −
    layer 1 y, step 5.
- Every 10 updates (`frame_count` % 10 = 0) the controller moves layer 1 x to base + the next
  entry of `0x800FB700` = {0, 992, 1984, 2976}: it shows the 4 lava copies in turn (the animation).
- Step `0x800EAB68` (one rise step): layer 1 y += 1 every 2 updates until y ≥ −31, then y = −32,
  `PlaySound(5, 3)`, next sub-step; `Quake2(10, 2, 2)` every 10 updates. Other sub-steps
  (`0x800EACFC` …) use `Rumble`, `PlayerEvent`, `PlayerEventEnd`, `cam.target_min_x`,
  `mega.x` (not read in full).
- A lava cell of map y 640 is drawn at world y 640 − layer 1 y (the mode-5 rule of
  `docs/engine.md`: layer px + (−layer position) = world px) [likely]. With y = −80 the lava top is
  at world y 720, and the kill boxes reach from y 840 − 120 = 720 down. So the kill line is the
  lava top, and the lava rises when layer 1 y grows [likely].
- Boss gates 25 and 39 move layer 1 y down to −80 (1 px per 2 frames, and 1 px per frame for 112
  frames), with quakes (`docs/enemies/boss-gate.md`): they put the lava back down [likely].
- Meaning [likely]: three rising-lava rooms (x 5440–5759 between gates 24 and 25, x 6080–6703,
  x 7024–7663) with an instant-death line at the lava top.

## Sprites: why every new sheet is wrong [certain for the data]

`spriteexport` made 7 search exports (types 15 ×2, 6, 18, 13, 33, 34) and the live boss gate. All
7 search sheets are noise (checked by eye; the Nightmare Virus sheet next to the shared
`nightmare-virus` sheet shows other pixels in the same frame shapes).

- The cells of banks 1–6 are in VRAM pages 7–13 (`sprites.cells`).
- About 60 % of the non-empty 32-byte pieces of pages 7–14 are pieces of member 135 entry
  `0x10411` (163 840 bytes, the stage 16 boss pack, `boss_pack_tbl[16]` = 135); pages 5, 6 and 15:
  0 %. So the boss pack covers all the enemy banks.
- The boss gate (bank 0, page 6) is correct: its `anim-0.png` and `anim-1.png` are byte-equal to
  the shared `boss-gate` sheets.

Exports kept in `sprites-root/`: only `enemies/boss-gate/` (a new source line) and
`stages/gate-1.json` (the 7 wrong IDs removed and listed as skipped, as in `mijinion-alt.json`).
The banks sheets `banks/gate-1/gate-1_before-boss/bank-1.png` … `bank-6.png` show the covered
cells; do not copy them as enemy art.

Wrong IDs (do not add): `enemy-9b63092464`, `enemy-71ce84488a` (type 15), `enemy-6d90265515` (6),
`enemy-51c2def92c` (18), `enemy-80ea7f5439` (13), `enemy-5e5c1e01f6` (33), `enemy-4c7523f990`
(34). The real stage 16 sheets of types 6, 13, 15 and 18 will probably have IDs of their own (the
sprite data differs from the other stages: the type 13 table is equal to the Sheldon table, but the
fingerprint is not) [likely]. A state earlier in the stage (checkpoint 0–3) is needed.

Catalog for sprites: `"16": "gate-1"` in `stages` of `catalog/enemies.json`. No `names` entry.

## Talk zones [certain]

k = `talk_stage_idx[32]` = 10. Boxes `talk_boxes` + 10 × 128: `id` 0 x 0–240, y 544–656; `id` 1
x 1296–1328, y 464–656; `id` 2 x 1280–1600, y 1200–1376; box 3 empty. All three are forced zones.
`talk_seen` k 10 = `0x0007` (all three seen). Messages of member 224 (Zero): 0 Alia "The secret
laboratory of Gate exists in a place like this? Let's move in!"; 1 Alia and Zero: the radar is
bad, "There's a big hole ... made for a big oaf to fit through", the lab is below, transmission
will be lost; 2 Zero alone: "It's dark here ... Alia, Alia? I've already lost the transmission".

## Checkpoints (`checkpoint_start_tbl[32]` → `0x80073288`) [certain]

| cp | record | player | camera 0 | camera 1 | camera 2 | limits (min x, max x, min y, max y) |
|---|---|---|---|---|---|---|
| 0 | `0x80072CFC` | 144, 464 | 0, 464 | 1920, 0 | 0, 116 | 0, 1296, 464, 1200 |
| 1 | `0x80072D20` | 1424, 1184 | 1264, 1184 | 1920, 0 | 1264, 296 | 1264, 1920, 1184, 1184 |
| 2 | `0x80072D44` | 3376, 1184 | 3200, 1184 | 1920, 0 | 800, 296 | 3200, 3200, 480, 1184 |
| 3 | `0x80072D68` | 5904, 480 | 5760, 480 | 1920, 0 | 1440, 120 | 5760, 7360, 480, 480 |
| 4 | `0x80072D8C` | 7824, 0 | 7664, 0 | 1920, 0 | 1916, 0 | 7664, 7664, 0, 0 |

All face right, layer `+0x40`/`+0x42` 0. A sixth pointer (`0x80072DB0`: player (152, 1904)) is
past the map height (1536); no marker sets checkpoint 5, so it is probably the next table's data
[guess]. The state's player y 122 vs the record y 0: the player has fallen to the floor.

## Zero [certain for the bytes]

- `game+0x60` = `0xFF` (all 8 techniques; also in `mijinion-4_dynamo`: not new).
- `game+0x5F` = `0x17`; owned parts `game+0x70` = `0x03F5BFD0`; equipped X `0x00000410`, Zero
  `0x00C00000` (= `mega+0xEC`; the two sabre parts of `docs/players/zero.md`, as in
  `yammark-4_blade-armor`).
- HP 42. The 6 gauges `mega+0xA8`… are 312 = `mega+0x14A`.
- **The gauge maximum per character** [certain for 80 save states; the code not read]:
  `mega+0x14A` = 6 × `game+0x61` for X and 6 × `game+0x62` for Zero. Every X state agrees with
  `+0x61` (288, 300, 312, 324, 336); every Zero state agrees with `+0x62` (48 → 288 in all the
  older Zero states, 52 → 312 here). Example of the difference: `heatnix-1` has `+0x61` = 56 and
  `+0x62` = 48, max 288 (Zero). So `docs/players/weapons.md` ("6 × `game+0x61`") is right only for
  X, and `docs/players/zero.md` ("full = 288 in all five states") has its first exception.
- No new player step or form: step 2 (stand) only.

## Open questions

1. Official names of main 33 and main 34 (and whether main 34, HP 30, is a mid-boss).
2. What the other main 29 sub-steps do (the camera and player events, when the lava stops), and
   the meaning of layer 1 mode 2.
3. Whether collision 58 (344 cells, walls x 2528–7679) has an effect in this stage: no conveyor
   code is loaded (no record of the stage 4 belt type).
4. The code that writes `mega+0x14A` from `game+0x61 + character`.
5. Member 223 (X's text bank of stage 16) does not decode.
6. A state of the start (checkpoint 0–1) to check the texture regions and the colours, and a
   state in checkpoints 1–3 for the enemy sprites.
