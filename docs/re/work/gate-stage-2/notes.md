# Gate stage 2, section 1 (stage 17, area 0): notes

Save state: `gate-3_stage-2.state.gz` (SHA-1 `992d5d0609e736bba918340805dd9c642d889217`). User's
words: "New save state: slot 10, 2nd Gate stage". Coordinator note: the user's "Gate stage 2" is
stages 17 (section 1, this state) and 18 (section 2, [`~/mmx-data/x6/re/gate-stage-3/`](../gate-stage-3/)).

## What is new

Before this state nothing of stage 17 existed (catalog, docs, exports, work folders): only the
table values in `docs/stages/gate-1.md` and the guess in `docs/enemies/high-max.md` [certain].

- The first map of stage 17: member 113, 24 × 9 screens, overlay 23, 113 records, a texture swap.
- The boss is High Max `var` 2 (record 112), in the stage overlay (all 158 functions also in
  overlay 44). This closes the "where is `var` 2 made" part of `high-max.md` open question 3.
- Stage 17's clear goes straight to stage 18 (`StageExit`), and stages 16 and 18 end with scenes.
- Main 34 (new in Gate stage 1) read in full: the missile launcher. The first good sheet of it.
- Main 11 (Illumina's orbs) read in part: the first save state with live orbs and records of them.
- Known types in a new overlay: falling rock (main 31), totem pole (stage 3, the base from
  records), rocket platform (stage 5), Nightmare Virus (main 15), item (main 47), boss gates 28, 29.
- `0x80072DB0`, "the sixth pointer" of `gate-1.md`, is checkpoint 0 of stage 17.
- Zero: nothing new (step 2, animation 17 = the dash end; same parts and gauge maximum as `gate-1`).

## Method

Scripts in `scripts/` (copied from `gate-1` and changed: `ascii_map.py`, `cmpcode.py`,
`tmx_view.py`, `contact.py`; new: `tables.py` (stage tables and checkpoints), `ovsearch.py`
(searches each function of a kit in an overlay file, masked), `check_changes.py` (checks the old
blocks of `doc-changes.md`)). Throwaway files in the scratchpad folder `gate-stage-2/`.

1. `where.py` (the caller's) and [`~/mmx-data/x6/re/heatnix-stage/scripts/live.py`](../heatnix-stage/scripts/live.py) (state, banks,
   live objects; its "camera" line prints `+0x20`/`+0x22`, which are limits, not the camera).
2. `learn` on a copy of `catalog/`; `stages` and `tmx` with a one-entry catalog into `stages/` and
   `tmx/` of this folder; a second `stages` run with `"texture": 22` (scratchpad) to compare the
   two blocks; the final `tmx/` has the proposed `texture_split`.
3. Kits: `kits/main-11`, `main-15`, `main-31`, `main-34`, `main-47`, `main-55`, `stage-3`,
   `stage-5`, `stage-8` (`rekit --state gate-3_stage-2.state.gz --type N [--class stage] --out`).
4. `spriteexport --root sprites-root` on a copy of `~/mmx-data/x6/sprites/`, catalog copy with
   `"17": "gate-2"`. Every new sheet checked by eye.
5. `mmxdemake.text --member 225/226` into the scratchpad (the text banks of stages 17 and 18).
6. `rekit --at 0x8001E32C` (`StageExit`).

## Stage tables [certain for the values]

`stage_members[34]` = 113, `stage_overlays[34]` = 23 (area 1: 0, 0); `text_members` 225 (X) / 226
(Zero); `stage_music` 19 (all four entries); `boss_music` 23; `boss_pack_tbl[17]` = 136,
`boss_pack_loaded` = 0; `talk_stage_idx[34]` = 11, boxes of k 11 all 0; `checkpoint_start_tbl[34]`
= `0x8007329C`; `camera_zone_tbl[34]` = `0x80079B70`; `bg_mode_tbl` stage 17: layer 1 mode 2,
layer 2 mode 3.

## The state [certain]

Stage 17 area 0, checkpoint 1, time 1638. Zero, form 5, step 2 (animation 17), HP 28, (1216, 1290),
facing right. Camera (1056, 1189); current limits x 368–3200, y 5–1200; target min y 1040.
`boss_choice` 2, `nightmare_effect` 0, difficulty 1, lives 2, kills 6. Type list 128, 15, 131, 133,
34, 11, 130, 0 × 7, 55, 55. Live: main 11 × 2 (records 41, 42, both in step 11 = attack), main 31
`var` 2 (record 82, step 0), visual 10 (orb shot), `PaletteCycle` 0, 2, 3, stage logic 27.
The preview shows Zero on the corridor platforms with the two orbs [seen].

## Map and textures

`learn`: member 113, 24 × 9, overlay 23 at layout offset `0xE688`, edges 0.926 / 0.965, 0 cells
differ, 113 records at `0xE910` (`0x800F8170`), palettes at (0, 486) from 178 tiles (agree 1.0),
texture `0x10000` (no `texture` key) [certain for the run].

- Member 113 has blocks `0x10000` and `0x16`. VRAM = `0x10000`, `stage_texture_swap` = 0.
- Camera zone record 8 (`id` 133 = zone 5 + swap bit) at x 2384–2415, y 1248–1375.
- Drawings with both blocks (scratchpad `room_cmp.png`, `corr_cmp.png`, `shaft_cmp.png`): `0x10000`
  is right for the shaft and the corridor; `0x16` is right for the tall room and the boss room
  (ladders, spikes, the High Max machines). Cells of x 2224–2591 are equal in both blocks; the
  first differing cells right of the swap are at x 2592 (the tall room's left wall). Layer 2
  (x 0–1279, y 0–719) is equal in both. Layer 1 is empty.
- Proposal: `texture_split` `{"texture": 22, "regions": [[0, 2384, 0, 6144, 2304]]}`. Checked in
  `tmx/m113-gate-stage-2/` (drawn with `tmx_view.py`: the whole map joins) [likely]. The colours of
  the `0x16` part come from this state's palettes [guess that they are right].

### Catalog entries

Nothing blocks `learn`. Command (main session):

```
python3 -m mmxdemake.learn --state gate-3_stage-2.state.gz --name "Gate stage 2" \
  --note "Zero in Gate's lab stage 2 (stage 17 area 0), checkpoint 1, at (1216, 1290) on the machine platforms"
# then add texture_split to the new entry
```

The name "Gate stage 2" gives the folders `m113-gate-stage-2`. If the coordinator picks another
scheme (for example "Gate stage 2, section 1" → `m113-gate-stage-2-section-1`), only `--name`
changes.

`catalog/savestates.json` (after `gate-1_before-boss.state.gz`):

```json
  {
    "name": "gate-3_stage-2.state.gz",
    "note": "Zero in Gate's lab stage 2 (stage 17 area 0), checkpoint 1, at (1216, 1290) on the machine platforms",
    "sha1": "992d5d0609e736bba918340805dd9c642d889217"
  },
```

`catalog/stages.json` (after member 112 "Gate stage 1", before member 118), with `texture_split`
added by hand:

```json
  {
    "differing_cells": [],
    "edge": {
      "h": 0.926,
      "h_joins": 42,
      "v": 0.965,
      "v_joins": 18
    },
    "height": 9,
    "member": 113,
    "name": "Gate stage 2",
    "objects": {
      "count": 113,
      "offset": 59664,
      "overlay": 23,
      "table_area": 0,
      "table_entry": 17
    },
    "offset": 59016,
    "overlay": 23,
    "palettes": {
      "agree": 1.0,
      "cameras": [
        [
          1056,
          1189
        ],
        null,
        null
      ],
      "clut_x": 0,
      "clut_y": 486,
      "matches": 178
    },
    "save_state": "gate-3_stage-2.state.gz",
    "save_state_sha1": "992d5d0609e736bba918340805dd9c642d889217",
    "texture_split": {
      "regions": [
        [0, 2384, 0, 6144, 2304]
      ],
      "texture": 22
    },
    "width": 24
  },
```

## Object identification [certain for the code matches]

| type | evidence | result |
|---|---|---|
| main 11 | `cmpcode` with `mijinion-bot/kit-main11`: 28 = 28 functions, 24 of 24 distinct identical; export `illumina-orb` pixel-equal | orb robot |
| main 15 | main-program code; `cmpcode` with `nightmare-virus/kit`: 54 of 54 distinct functions found; bank 1 sheets pixel-equal | Nightmare Virus |
| main 31 | `ovsearch` in `rock_bin/08.bin`: 10 of 10 functions, at `FallingRockInit` / `Main` / entry | falling rock |
| main 34 | `cmpcode` with `gate-1/kits/main-34`: 42 = 42, 33 of 33 identical | missile launcher (new sheet) |
| main 47 | shared item type; record `id` 34 = `0x22` | item, kind not known |
| main 55 | `ovsearch`: 158 of 158 in `rock_bin/44.bin` and `rock_bin/02.bin` | High Max |
| stage 3 | `cmpcode` with `scaravich-crystals/kit-stage-3`: 28 = 28, 24 of 24 | totem pole |
| stage 5 | 5 of 5 functions in the `rocket-platform` kit | rocket platform |
| stage 8 | main program | boss gate |

The flying robots of the preview are the two live main 11 (orb robots).

## High Max `var` 2 and the stage end [certain for the code]

- Death: `0x800F0CE4` (overlay 23): stage 12 → `exit_req` `0x80`, `game[0x2C + checkpoint]` = 1,
  checkpoint + 9; else `exit_req` `0x10` (delay slot `addiu v1, zero, 12` at `0x800F0C88`).
- `0x800F52AC` (state table `0x800F9A48`[5]): `exit_req` `0x40` when no text is open; probably the
  fly-away end of `high-max.md` (the intro fight) [likely; not read].
- `StageExit` (`0x8001E32C`), exit request > 0, stage not 0 and not 1–8: stage 16 → `game+0x00` =
  19, `+0x1D` = 8, `+0x66` = 4; stage 17 → `game.stage` = 18, `game+0x00` = 8; stage 18 →
  `+0x00` = 19, `+0x1D` = 9, `+0x66` = 5. (`+0x00` = 8 is also the value of the area change and of
  the `0x41` crystal exit, so it is likely the stage load mode.)
- What this state cannot show: the fight itself (no boss object, boss pack not loaded, banks 14/15
  without texture), the boss HP, steps 13–17 and the `var` 2 shots. Text: member 226/225 message 0
  is the High Max talk.

## Main 34 and main 11

Read in full (main 34) and in part (main 11) from the kits. Results: `missile-launcher.md`,
`illumina-orb.md` (sheets for `docs/enemies/`). Main 34 cycle timing is from the code only.

## Sprites

Exports (`sprites-root/`):

- New: `enemies/enemy-eb6483d4fb/` (main 34, 14 animations, search): good (checked by eye; body,
  shells, missiles, debris, flash). `names`: `"enemy-eb6483d4fb": "missile-launcher"`.
- Changed: `illumina-orb`, `nightmare-virus`, `nightmare-virus-orb`: only `animations.json` (a new
  source line); the PNGs are byte-equal.
- `stages/gate-2.json`: 4 IDs, skipped types 0 and 55 (no texture). No wrong ID.
- Bank sheets `banks/gate-2/gate-3_stage-2/bank-0.png` … `bank-6.png`: correct (bank 0 type 128:
  Light capsule and boss gate parts; 2: rocket platform; 3: falling rock; 6: totem pole). The
  main 31, stage 3 and stage 5 types are not exported because their banks are misc types
  (`docs/TODO.md`).
- Gate stage 1 types: main 34 good here; main 15 good; main 6, 13, 18, 33 are not in this type list.

## Open questions

1. Item kind `0x22`.
2. Who starts `BossPackLoad` (no logic 4 record).
3. Colours of the `0x16` part; the texture after a restart at checkpoint 2/3.
4. Totem pole `a` = 48; whether the poles change `game+0x2F` here.
5. Orb rolling steps and visual 10; official names of the orb robot and the missile launcher.
