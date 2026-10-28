# Gate stage 2, section 2 (stage 18, area 0) and Gate: notes

Save states:

- `gate-4_before-gate.state.gz` (SHA-1 `f51fcc1d7dd4d15662dc4a3ecd18beb124cd7419`). User: "Slot 10:
  new save state, in room right before fight against Gate himself (2nd section of Gate stage 2)".
- `gate-5_gate-fight.state.gz` (SHA-1 `68a442ae9239e06c8539d8eea50087a0bfce575b`). User: "fight
  against Gate has started".

Names (from the main session): stage 17 = `gate-2`, stage 18 = `gate-2b` (page
`docs/stages/gate-2b.md`); catalog stage name "Gate stage 2, section 2" (slug
`gate-stage-2-section-2`).

## What is new

Before these states nothing of stage 18 existed (no catalog entry, page, map, export or work
folder); `docs/damage.md` section 7 had Gate's main state and set-up only [certain].

- The map of stage 18: member 114, 25 × 10 screens, overlay 24, three parts joined by teleporter
  crystals (`id` 2–4, sequence B), with different paths for X and Zero.
- Gate's full code (main 59, 74 functions; visual 26 with 35 `var` cases): the behaviour sheet.
- Known objects in a new overlay: Meta Wheel F, Illumina jet (main 8), Monbando, Weather Analyze
  (rows 5 and 6), teleporter crystal, health capsule, press blocks, the Nightmare Press, acid rain
  (stage logic 17), boss gates 30 and 31.
- Answers: teleporter `id` 2–4 users and destination table (`teleport-crystal.md` open question
  2); Weather Analyze rows 5–6 users; boss gate ids 30 (the Y − 48 max x seen in a state) and 31
  (the FADE and its undo by Gate); `docs/damage.md` open question 2 (visual 26).
- A `sets` rule for Gate (animations 31–33 from bank + 1).

## Method

Scripts in `scripts/`; throwaway files in the scratchpad folder `gate-stage-3/`.

1. `where.py` (caller's) and [`~/mmx-data/x6/re/heatnix-stage/scripts/live.py`](../heatnix-stage/scripts/live.py) for both states.
2. `scripts/tables.py <state> 18`: `stage_members`, `stage_overlays`, `text_members`,
   `stage_music`, `boss_music`, `boss_pack_tbl`, `talk_stage_idx`, `talk_boxes`,
   `checkpoint_start_tbl` (5 records: the stage 19 list starts at `0x800732C0`).
3. `learn` on a copy of `catalog/` (scratchpad), then `stages` and `tmx` with a one-entry catalog
   into `stages/` and `tmx/` of this folder. A second drawing with block `0x10000` (scratchpad
   `st2/`) for the texture comparison; the ORA layers compared one by one.
4. `rekit` for main 59 (on `gate-5`), visual 26, main 2, 8, 9, 14, 28, stage 9, 13, logic 17,
   kind7 2 (on `gate-4`) into `kits/`. `scripts/condense.py` prints a kit without the
   tex/clut/sprite boilerplate. `scripts/findcopies.py` compares a kit with every kit under
   [`~/mmx-data/x6/re/`](../) (masked code); `scripts/ovsearch.py` searches overlays on disc.
5. `spriteexport --root sprites-root` (a copy of `~/mmx-data/x6/sprites/`) with a catalog copy that
   has `"18": "gate-2b"`, on `gate-4`, then on `gate-5` twice (the second run with the `sets` rule
   for type 59). Every sheet compared with the shared one (`cmp`) or checked by eye.
6. `mmxdemake.text --member 225/226` into the scratchpad.
7. `StageExit` (`0x8001E32C`) and scene overlay 55 disassembled for the stage end.
8. `scripts/ascii_map.py 6` → `ascii-map.txt`.

## The states [certain]

`gate-4`: stage 18 area 0, checkpoint 4, time 16 621; Zero step 2 at (2989, 1082), HP 34; layer 0
camera (2880, 960); limits current (max x 2878, min x 2880, max y 960, min y 960), target (1024,
2880, 960, 960). `boss_pack_loaded` 0. Type list: 128, 2, 8, 9, 143, 28, 14, 0 × 7, 59, 59; banks
14 and 15 (type 59) have no texture entry. Live: stage objects 0–22 = press blocks (stage 9 `var`
2, state 1.2), 23 = boss gate 30 (state 2), 24 = boss gate 31 (state 1); logic 1–4 PaletteCycle,
5 = logic 17 `var` 1, 6 = logic 27; kind7 0 = type 2. `game+0x8C` = 56, `+0x8D` = 3, `+0x8F` = 2.

`gate-5`: time 17 238; Zero step 2 at (3264, 1114); camera (3184, 960); limits current (3216, 3184,
960, 58), target (3216, 3184, 960, 928). `boss_pack_loaded` 1. Main 0 = Gate: state 1, step 6,
sub 1, HP 56, contact 6, `dmg_tbl_gate`, boxes `0x800F6F2C`/`0x800F6F30`, (3456, 1112), animation
2, `+0x86` 0, `+0x87` 1, texture `0x0140`, palette `0x7948`. Visual 0–5 = `var` 15–20 at the
platform table positions; visual 6 = `var` 32 at Gate (`+0x8C` 27); visual 7 = `var` 2 (HP 8,
contact 4, `dmg_tbl_deflect`, step 0, `+0x8C` 6). Globals: `gate_orbs` 1, `gate_obj` = Gate,
`gate_frame` 1473, all others 0. `game+0x20` = Gate, `game+0x24` (bar) 1, `game+0x2F` 1.

What `gate-5` confirms in the sheet: HP = `boss_hp_max` = 56 at level 2, contact 6, the damage
table, the boxes, the start position after the fall (y 1112), the platforms and their positions,
the summon step with an orb and its charge effect, the orb set-up values. Everything else is code
only.

## Map and catalog

`learn`: member 114, 25 × 10, overlay 24 at layout offset `0xC138`, edges 0.962 / 0.940, 0 cells
differ, 65 records at `0xC428` (`0x800F5C88`), palettes at (0, 486) from 223 tiles (agree 1.0),
texture entry `0x16` [certain for the run].

Texture: `stage_texture_swap` = 2; block `0x16` is in VRAM. The two drawings differ in the top
corridor (layer 0), the X path, the Zero path and layer 1; layer 2 is identical; Gate's room is
identical. The spikes of the top corridor (collision 63 at x 384, y 176) are drawn right only with
`0x10000`; the X path spikes (x 1300, y 1632) only with `0x16`. So `texture_split` with block
`0x10000` for `[0, 0, 0, 1920, 240]` [likely].

### Catalog entries

`catalog/savestates.json` (sorted by name; after `gate-3_stage-2` if present, else after
`gate-2_nightmare-mother`):

```json
  {
    "name": "gate-4_before-gate.state.gz",
    "note": "Zero in Gate stage 2, section 2 (stage 18 area 0), checkpoint 4, at (2989, 1082) in the corridor between boss gates 30 and 31, before Gate's room",
    "sha1": "f51fcc1d7dd4d15662dc4a3ecd18beb124cd7419"
  },
  {
    "name": "gate-5_gate-fight.state.gz",
    "note": "Zero in Gate's room (stage 18 area 0, checkpoint 4) at (3264, 1114), the fight has started: Gate (main 59) HP 56, step 6 (summon), one orb and six platforms live; boss pack 137 loaded",
    "sha1": "68a442ae9239e06c8539d8eea50087a0bfce575b"
  },
```

`learn` writes only the first one; add `gate-5` by hand (or with `learn` under a second name and
`tmx_skip`, which is not needed: it adds no map data).

`catalog/stages.json` (sorted by member, after member 113), with `texture_split` added by hand:

```json
  {
    "differing_cells": [],
    "edge": {
      "h": 0.962,
      "h_joins": 106,
      "v": 0.94,
      "v_joins": 21
    },
    "height": 10,
    "member": 114,
    "name": "Gate stage 2, section 2",
    "objects": {
      "count": 65,
      "offset": 50216,
      "overlay": 24,
      "table_area": 0,
      "table_entry": 18
    },
    "offset": 49464,
    "overlay": 24,
    "palettes": {
      "agree": 1.0,
      "cameras": [
        [
          2880,
          960
        ],
        null,
        null
      ],
      "clut_x": 0,
      "clut_y": 486,
      "matches": 223
    },
    "save_state": "gate-4_before-gate.state.gz",
    "save_state_sha1": "f51fcc1d7dd4d15662dc4a3ecd18beb124cd7419",
    "texture": 22,
    "texture_split": {
      "regions": [
        [0, 0, 0, 1920, 240]
      ],
      "texture": 65536
    },
    "width": 25
  },
```

Commands (main session):

```
python3 -m mmxdemake.learn --state gate-4_before-gate.state.gz --name "Gate stage 2, section 2" \
  --note "Zero in Gate stage 2, section 2 (stage 18 area 0), checkpoint 4, at (2989, 1082) in the corridor between boss gates 30 and 31, before Gate's room"
# then add texture_split to the new entry and the gate-5 entry to savestates.json
```

Nothing blocks `learn`. Output folders: `m114-gate-stage-2-section-2` (stages and TMX). My
drawings: `stages/m114-gate-stage-2-section-2/` and `tmx/m114-gate-stage-2-section-2/` (without
the `texture_split`).

## Object identification

| type | evidence | result |
|---|---|---|
| main 2 | 17 of 17 functions = `meta-wheel-f` kit (masked); animation table 11 of 11; export ID `enemy-43b4084bcf`, sheets byte-equal | Meta Wheel F [certain for the code] |
| main 8 | 28 of 28 functions in overlay 11 and in overlay 32 (`ovsearch`); animation table = `illumina-jet` (7 of 7); export ID `enemy-304e0899c6`, sheets byte-equal | Illumina jet [certain for the code] |
| main 9 | 13 of 13 = `monbando` kit; ID `enemy-c11dd314d5` | Monbando [certain for the code] |
| main 14 | 14 of 14 = `weather-analyze` kit; tables in overlay 24 equal to the sheet's rows (positions `0x800F6CE0`, sensors `0x800F6CFC`, camera `0x800F6D6C`); records `id` `0x15`, `0x16` | Weather Analyze, rows 5 and 6 [certain] |
| main 28 | 17 of 17 = `teleport-crystal` kit; destination table `0x800F6E80` = the four entries in the sheet | teleporter crystal [certain] |
| stage 13 | 18 of 19 = `health-capsule` kit | health capsule [certain for the code] |
| stage 9 | 4 of 4 functions in overlays 06 and 07 (stage 4); `a` = 48 as in `shark-2` | `PressBlock` [certain for the code] |
| kind7 2 | entry `0x80046E14` (main program) | `PressUpdate`, the Nightmare Press [certain] |
| logic 17 | 6 of 6 functions in overlay 30 | acid rain controller [certain for the code; meaning likely] |
| main 59 | the boss (both states) | Gate |

## Gate: evidence for the sheet

Kit `kits/main-59/` (`condensed.txt` is the short form). The key facts and where they are:

- HP: `0x800EF4DC`… `game+0x8C` = 48 + `0x800E98B8`[`game+0x8F`]; `0x800E98B8` = 04 06 08 0a.
- Weapon hits: `0x800EF5AC`–`0x800EF5D4` (HP written back, `gate_damage`).
- Fragment hit: `0x800EF5D8`–`0x800EF710` (HP − 4, death or step 8); `0x800F36E8`–`0x800F3788`
  (the fragment test, step 7 excluded).
- Choice: `0x800EFDCC` with `0x800E98BC` = 01 02 02 03 and patterns `0x800F7528`.
- Orbs: `0x800F0B90`… (set-up), `0x800F0D4C`… (yellow), switch `0x800E9958`; the purple, green,
  red and blue copies; `+0x98` vs 16 at `0x800F0E10` etc.
- Column: `0x800F407C`, `0x800F4240`, box `0x800F76C8` = f0 b8 20 90.
- Platforms: `0x800F43B8`, `0x800F4560`, positions `0x800F7768`.
- Death: `0x800F5120`, `0x800F51CC`, `0x800F0884`, `0x800F52CC`, `0x800F5344`, `0x800F53C8`.
- `StageExit` stage 18 branch: `0x8001E4C0`–`0x8001E4E0` (game mode 19, `+0x1D` = 9, `+0x66` = 5).
- Scene overlay 55 (2 416 bytes): 15 state functions (table `0x800FA934`); text line ids 431–447
  (X) / 452–470 (Zero) by `game+0x38`; the last state writes 11 to byte 0 of its argument and
  clears its bytes 1–3 (the game mode pattern) [likely that the argument is `game`].

### Hazards and edge cases (for a reimplementation)

- `gate_hit` is one flag for all fragments: two fragments in one update give one hit [certain].
- A fragment spawned within 24 × 32 px of Gate hits him at once (no 5-update delay for the Gate
  test; the delay is only for weapons) [certain for the code].
- `GateChoose` treats k = 3 and 4 differently only by `gate_orbs`; a red and a blue orb can never
  be live together with another orb [certain for the code].
- The slow motion writes the stop flags every update while the red orb works; when `gate_slow`
  goes to 0 the flags are written 0, which also clears a stop flag that another object set
  [certain for the code; a hazard for any object that uses `game+0x10`, `+0x11`, `+0x13` during
  the fight].
- Boss gate 30 writes a max x (1024) below the min x (2880); the camera code keeps the camera at
  the min x (seen in `gate-4`).

## Sprites [certain for the runs]

- `gate-4`: 6 exports, all with the shared IDs; every PNG byte-equal to the shared copy:
  `enemy-43b4084bcf` (meta-wheel-f), `enemy-304e0899c6` (illumina-jet), `enemy-c11dd314d5`
  (monbando), `enemy-efd9270cec` (teleport-crystal), `enemy-9b0ee535de` (weather-analyze),
  `enemy-30419fd669` (boss-gate). Only `animations.json` changes (a new source line).
- `gate-5`: banks 3–6 skipped by the tool ("cells overwritten by type 59"); meta-wheel-f,
  illumina-jet, boss-gate byte-equal again. New: `enemy-db764876a4` (Gate, 34 animations, live
  source bank 14, table `0x800F74A0`). Without a `sets` rule animations 31–33 show Gate sprites
  (wrong); with `{"stage": 18, "type": 59, "set": 1, "animations": [31, 32, 33], "texture":
  "0x0540", "palette": 1}` they show the small creature (31, 32) and the platform block (33). The
  ID stays `enemy-db764876a4` [seen by eye].
- Good IDs to keep: all of the above. Wrong IDs: none. `names` entry to add:
  `"enemy-db764876a4": "gate"`.
- Kept in `sprites-root/`: `enemies/enemy-db764876a4/` (with the set rule), the new source lines,
  `stages/gate-2b.json`, `banks/gate-2b/`. The main session runs `spriteexport` on both states
  after the catalog changes (the `sets` rule first, then `gate-5`).

## Talk [certain for the decode]

Member 226 message 1 (Gate and Zero, 21 pages; in the sheet). Member 225 does not decode
(`mmxdemake.text --member 225` writes nothing for it), as member 223.

## Open questions

See the sheet and the page. In short: a state in each Gate phase (hurt, red orb, blue orb,
purple creatures, column and platforms), the death and what follows, X's version, the boss pack
loader, the top corridor colours, collision 57.
