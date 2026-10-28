# heatnix-4_cave: notes

Save state `~/mmx-data/x6/dumps/heatnix-4_cave.state.gz`. The user: "Zero has beaten first round of
mini boss, now in a 'cave-like' area with flame throwers, gray nightmare-effect blocks, and
nightmare viruses."

Scripts (run from the repo root with `PYTHONPATH=.`):

- `scripts/cave_dump.py <state>`: game, player, the three layer cameras, every main and stage
  object with the fields that the flame jet and the grey block use, the record flags and kinds of
  the area, the fixed list, the virus soul bits.
- `scripts/anim_tables.py <state>`: the flame jet and grey block animation tables and the flame
  box height table, from RAM.
- `scripts/block_floors.py`: the layer-0 collision under each grey block of the first tower (from
  the TMX), for the fall heights.
- `scripts/dis.py <state> <start> <end>`: a plain disassembly of RAM.
- `scripts/find_kind_writes.py <state>`: the stores to byte 3 of a record (the kind) in the main
  program and overlay 05.

[`~/mmx-data/x6/re/heatnix-stage/scripts/live.py`](../heatnix-stage/scripts/live.py) was also used (all classes). Note: its "camera"
line reads `cam+0x20`/`+0x22` (max y, min y), not the camera x, y (`+0x0A`, `+0x0E`).

## The state [certain for every value in this section]

- Stage 3, area 0, checkpoint 1. Zero (form 5), Normal (difficulty 1), **lives 4**, HP 8, stage
  time 3989, kill count 10. Zero at (1012, 1898), state 1, step 2, `speed_y` 0 (on a floor).
- `nightmare_effect` 4. `game.stage_effect` (stages 0–8) = `0 8 3 4 2 4 8 5 3`, `game.effect_new`
  = `2 8 8`. The start states (`heatnix-1`, `-2`) have `0 6 3 4 2 4 7 5 3`: stages 1 and 6 hold 8
  here, so this run had a stage 8 pick (targets `1 6 6`) before the stage 3 pick. Stage 3's own
  value is 4 in both runs. `heatnix-3_rekkoha` has the same bytes as this state.
- Same run as `heatnix-3_rekkoha`: `frame_count` 3885291 → 3888923 (3632 frames), stage time 482 →
  3989 (3507). Lives 3 → 4. Zero's souls (`game+0xD4`) 1176 → 1208 (+32).
- `game+0x26` = 1 (the first Nightmare Snake is dead; `docs/enemies/nightmare-snake.md`).
- Rescue Reploid status word 4 (`game+0xE8`) = `0x202`: Reploid `id` 32 (the snake room) and `id`
  34 (record 46, (1232, 1296), the first tower) are saved (status 2). In `heatnix-3_rekkoha` it is
  `0x2` (only `id` 32). Records 45 and 46 have flags `0x01`.
- `stage_texture_swap` = 0.

### Cameras

| layer | mode | shown | x, y | current limits | target limits |
|---|---|---|---|---|---|
| 0 | 1 | 1 | 944, 1727 | x 944–976, y 448–**1729** | x 944–976, y 448–2624 |
| 1 | 5 | **0** | −140, −72 | – | – |
| 2 | 1 | 1 | 472, 863 | – | – |

- Layer 0: `cam+0x47` (ease) = 2, `cam+0x2E` = 171. Zero's screen y = 1898 − 1727 = 171.
- The current max y 1729 is not a room limit. `CameraLimitEase` (`0x80028EBC`, the max-y part at
  `0x80029180`) sets, while the target max y is below the camera: when (player y − camera y −
  `cam+0x2E` + 2) ≥ 4, max y = the target at once; else max y = camera y + max(0, player y −
  player old y (`mega+0x1E`)) + `cam+0x47`, but not past the target. Zero stands on the follow
  line (171), so max y = 1727 + 0 + 2 = 1729 [certain for the code and the state]. So the camera
  follows Zero down the shaft while the limit leads it by 2 px; it is not stuck.
- Min y 448 is the gate `id` 7 value, not the checkpoint 1 record (464): there was no respawn at
  checkpoint 1 since the gate [likely].
- Layer 2 = half of layer 0 (mode 1) [certain].
- Layer 1 (mode 5) is hidden (`cam1+0x03` = 0) and keeps the last ring position of the snake:
  512 − (−140) = 652, 512 − (−72) = 584, so the first snake died at body (652, 584) [certain for the
  formula of `docs/enemies/nightmare-snake.md`].

### Live objects (all classes checked: main, stage, visual, misc, misc2, effect, weapon, logic, ray, kind7, actor2)

| slot | object | `var` | state | x, y | notes |
|---|---|---|---|---|---|
| main 0 | main 25 (Nightmare Snake) | 4 | 0.0.0 | 0, 0 | record pointer 0: made by gate `id` 7, not by fixed record 1 (its flags are 0). Waits for player y ≥ 2384 |
| main 1–6 | main 36, flame jet | 4, 20, 24, 16, 16, 20 | below | below | |
| stage 0 | boss gate | 7 | 2.0.0 | 944, 608 | **state 2 = closed for good** (sprite 58), not open. Live 1100 px above the view |
| stage 1–4 | stage 12, grey block | 3 | below | below | |
| logic 0, 1 | talk zones | 129, 130 | 0 | 80, 256 / 304, 256 | optional zones stay live far off screen (agrees with `docs/engine.md`) |
| logic 2 | stage logic 27 (`ReadyControl`) | 0 | 1.3.0 | – | |

No visual, misc, effect, weapon, ray, kind-7 or `actor2` object is live. **No Nightmare Virus**
in any class.

## 1. Flame jet (main 36) [certain for the values]

| slot | record | kind | `var` | x, y | state | sprite | box `+0x80` (x, y, w, h) | `+0x50` | `+0x7C` | `+0x85` | display |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 1 | 35 | 0 | 4 (up, 4 bursts) | 1153, 1930 | FIRE sub 1 (grow) | 0 | −8, −16, 16, 16 | `+0x80` | 14 | 1 | 1 |
| 2 | 38 | 1 | 20 (down, 4 bursts) | 1185, 1984 | FIRE sub 3 (die out) | 23 | −8, 0, 16, 56 | `+0x80` | 0 | 3 | 1 |
| 3 | 40 | 0 | 24 (down, random) | 1105, 1984 | PAUSE sub 1 | 13 | −8, 0, 16, 1 | 0 | 27 | 0 | 0 |
| 4 | 36 | 1 | 16 (down, long) | 1153, 1632 | FIRE sub 2 (hold) | 18 | −8, 0, 16, 88 | `+0x80` | 83 | 1 | 0 |
| 5 | 41 | 1 | 16 (down, long) | 977, 1984 | FIRE sub 2 (hold) | 19 | −8, 0, 16, 88 | `+0x80` | 108 | 1 | 1 |
| 6 | 37 | 0 | 20 (down, 4 bursts) | 1265, 1632 | FIRE sub 2 (hold) | 19 | −8, 0, 16, 88 | `+0x80` | 9 | 2 | 0 |

What this confirms in `docs/enemies/flame-jet.md`:

- `var` = record `id`; positions = the records of `docs/stages/heatnix.md` (records 35–38, 40, 41).
- Direction: `id` 4 uses sprite 0 (animation 0, up) with the box above the point (y offset −h);
  `id` 16, 20, 24 use sprites 13–27 (animations 2, 3, 4, down) with y offset 0. `flip` 0 on all.
- Box height by sprite: sprite 0 → 16, 18 and 19 → 88, 23 → 56; the RAM table `0x800F3CD0` = `16 40
  56 72 88 88 88 88 72 56 24 8 8 1` twice. `+0x50` (attack box) = the object's `+0x80` while it
  burns and 0 on sprite 13 (PAUSE). Hurt box 0, HP 12, contact 16.
- Hold time: 14 or less for the burst jets (`id` 4, 20); 83 and 108 for the long jets (`id` 16, ≤
  180). Burst counter `+0x85` counts down (1–3 seen). PAUSE: `+0x7C` 27 (of 60), `+0x7E` 0, sprite 13,
  display 0.
- Display: on sprites 13/27 display 0; slots 4 and 6 (point 95 px above the view) display 0 by
  `DisplayCheck(48, 48)`; slots 1, 2, 5 display 1.
- `OffScreenCheck2(160, 160)`: the live jets are at most 95 px outside the view; records 27–33 (≥
  245 px above) are not live, flags 0, kind high digit 0 (deleted by `DeleteObject2`).
- Difficulty filter: records 34 (977, 1930) and 39 (1153, 1984), kind 2 (Hard), are in the view and
  not spawned (Normal) [certain]. The kind-1 records 36, 38, 41 are live.
- The animation table `0x800F3CBC` in RAM = the sheet (animation 0 loops from frame 9 to frame 6 with
  `next` −3).
- Not shown by a state: the sound (bank 2, id 22), the random count of `id` 24 (slot 3 is in PAUSE),
  `+0x84` ("put out", 0 on all six).
- Corrections: none to the behaviour. The export from the live objects (palette `0x788C`, the bank 3
  CLUT) is byte-identical to the earlier export from the bank: open question 3 (the look) is closed.

## 2. Grey block (stage 12) [certain for the values]

| slot | record | start y (`+0x7E`) | x, y | step | `speed_y` | timer `+0x7C` | floor (TMX) | fall d | frozen? |
|---|---|---|---|---|---|---|---|---|---|
| 1 | 111 | 712 | 1056, 750 | 2 RISE | +3 | −1 | 816 | 80 | yes (977 px above the view) |
| 2 | 112 | 872 | 1152, 894 | 2 RISE | +3 | −1 | 1056 | 160 | yes |
| 3 | 113 | 1240 | 1184, 1293 | 2 RISE | +3 | −1 | 1440 | 176 | yes (434 px above) |
| 4 | 114 | 1528 | 1152, 1576 | 1 REST | −4 | 119 | 1600 | **48** | no (151 px above; inside the 240 px band) |
| – | 115 | 2104 | not live | – | – | – | 2272 | 144 | record not reached (point 137 px below the view) |

- `var` 3, HP 16, contact 8, terrain box `0x800F3DB4`, `display` 0 (all outside the view by more
  than 32 px). Checkpoint 1 < 3, so they stay.
- `+0x7E` = the record y [certain]. `anime.flag` = 2 and sprite 4 on all four: the appearance
  animation has ended and holds (only the set-up calls `SetAnime`).
- Block 114 rests at y 1576: the floor at y 1600 minus the half height 24 [certain]. Its timer is
  119 (61 updates after the landing). `speed_y` keeps −4 from the fall during REST; RISE uses +3
  (positive is up in this code) and timer −1 at the end of REST: all as the sheet says.
- Blocks 111–113 are stopped in the middle of the rise, 434–977 px above the view: this agrees with
  "an off-screen block stops where it is" (`OffScreenCheck2(64, 240)`) [certain for the state].
- Cycle of each block (fall d / 4 + 181 + d / 3 updates): 111: 20 + 181 + 27 = 228; 112: 40 + 181 +
  54 = 275; 113: 44 + 181 + 59 = 284; 114: 12 + 181 + 16 = 209; 115: 36 + 181 + 48 = 265 [likely:
  one update per frame; the floors are from the layer-0 collision, rounded up for d / 3].
- Block 113's floor is only under its left half (x 1152–1183 at y 1440, the full width at 1472);
  `CollideCheck` with the 64 px terrain box stops it at the first one [likely].
- The animation table `0x800F3DB0` (animation 0) in RAM: sprites 7, 0, 1, 2, 3, 4, 5, 4, 6 (flag
  0; 2, 2, then 3 updates), 4, 5, 4, 6, 4, 5, 4, 6, 4 (flag 1, 3 updates each), then 4 (flag 2,
  held). 25 + 27 updates, as the sheet says; the sprite list in the sheet is shorter than the table.
- The live source sheet `stage-12-live-0.png` (new, first live grey block in stage 3): sprite 7 is
  empty; sprites 0–3 show the block that grows down from its top edge; 4–6 the full block (5
  darker). Correct by eye [likely for the meaning]. This answers open question 4 in part.
- The records 111–125 in RAM match the record list of the stage page (x, y, `id`).

## 3. Nightmare Viruses

**Answer:** the viruses of the first tower spawn under Nightmare Effect 4; four were made and
killed before this save. None is live at the save point because of the record kind rule (below)
and because the next record is not in the spawn band yet.

- The first-tower records (type 15, kind 0–2): 60 (1088, 1024), 61 Hard (1152, 928), 62 (1040,
  1152), 63 (1056, 1536), 64 Hard (1232, 1664), 65 (1088, 1760), 66 (1056, 2032), 67 Normal,
  snatcher `id` 33 (1168, 2240), next to Rescue Reploid `id` 35 (1232, 2160).
- **Soul bits** [certain]: `soul_row_tbl[3 × 2 + 0]` = row 2; `game+0x178 + 2 × 32` byte 0 = `0x2D`
  = bits 0, 2, 3, 5. The bit is the ordinal among the type-15 records: records 60, 62, 63, 65 — all
  four Normal virus records above Zero. Records 61 and 64 (Hard) were never made. The row is 0 in
  `heatnix-1`, `-2`, `-3`.
- Zero's souls +32 since `heatnix-3_rekkoha` = 4 × 8: each soul was taken in its first 60 updates
  (worth 8) [likely: the arithmetic fits only so].
- **Record kinds** [certain]: 60, 62, 63 have kind `0x20`, 65 has `0x10`, flags 0 on all. The
  start states have kind `0x00` on all. See "The record kind high digit" below: `0x10` = killed,
  still in the band; `0x20` = killed, then left the band.
- Record 65 is inside the view (1088, 1760) but not live: a killed record does not come back when
  its point is still in the band (kind `0x10`).
- **Nightmare Effect:** the spawner (`SpawnInRect`, the filter `0x80029E7C`) and the virus code
  do not read `nightmare_effect` (`0x800CD30A`). In the main program the only access to it with
  `lui 0x800D` + `0xD30A` or `game` + `0x43A` is the store in `StageStartReset` (`0x8001E9D0`)
  [certain for these two address forms]. Overlay 05 reads it only for the grey block and the
  Nightmare Bug (stage page). So the virus records spawn for every effect value [likely]; the state
  shows them spawn with value 4 [certain].
- **The sheet `docs/enemies/nightmare-virus.md` covers stage 3:** the code is in the main program;
  the `id` values here are 0, 1, 32, 33 (all in the sheet's table). Only the stage 3 record list,
  the soul row (2) and the kind rule are new.
- **What a state needs to show a live virus here:** the camera must bring a record point into the
  spawn band (48 px outside the 320 × 240 view) for the first time, or for the third time after a
  kill. From this save: record 66 (1056, 2032) enters the band when the camera y passes about 1744 [likely: band = camera y + 288;
  the strip bounds of `ScrollSpawn` were not read] (17 px
  lower; Zero goes down about 17 px more), then 67 (the snatcher `id` 33) at camera y ≥ 1953. A
  state while the snatcher flies to Reploid 35 (if that Reploid is not saved yet) would show the
  SNATCH step in stage 3. A state just after Zero drops lower in the shaft is enough for record 66.

### The record kind high digit (spawner, main program) [certain for the code]

`SpawnInRect` (`0x80029F38`) walks the area's records. For a record of kind 0–2 whose flags have
bits `0x81` clear:

- Point **inside** the spawn rectangle → `RecordSpawnFilter` (`0x80029E7C`, new name; 0 = spawn,
  1 = no spawn). Kind low digit ≥ 3: 0 (no filter). Kinds 0–2:
  - difficulty 3: spawn, kind not changed;
  - difficulty < kind low digit: no spawn;
  - high digit 0: → `0x10`, spawn;
  - `0x10`, `0x30`, `0x50`, above `0x60`: no spawn;
  - `0x20`: → `0x30`, no spawn;
  - `0x40`: Easy: no spawn (no change); else → `0x10`, spawn;
  - `0x60`: Easy: no spawn; else → `0x30`, no spawn.
- Point **outside** the band (camera x − 48 … + 368, y − 48 … + 288): `0x10` → `0x20`, `0x30` →
  `0x40`, `0x50` → `0x60`.
- `DeleteObject2` (off-screen despawn) changes `0x10` to `0x00`; `DeleteObject` (a kill) keeps
  `0x10`; `RecordsReset` (a restart) clears the digit.

So an object of kind 0–2 that leaves the screen comes back at its next entry. An object that was
**killed** (`DeleteObject`) comes back only after its point leaves the band, enters it (no spawn),
leaves it again and enters a third time, and never on Easy. On difficulty 3 it comes back at each
entry. Nothing found writes `0x50` (`scripts/find_kind_writes.py`); no record of stage 3 has a high
digit on disc. The virus uses `DeleteObject` in state 2 (death, the end of a soul) and
`DeleteObject2` in state 3 (off screen) [certain].

`spawn_margin_tbl` (`0x800733B8`, new name): 6 halfwords `32 64 96 128 160 480`; `SpawnInRect`
widens the strip by entry (record flags >> 4) − 1 on the side of the strip when the record flags
have bits `0x70` [certain for the read; which records use it was not checked].

## 4. The stage page (`docs/stages/heatnix.md`), the cave part

- The cave is the first tower: Zero at x 1012, camera x 944 [certain].
- Checkpoint 1 [certain]; marker record 0 is not live (it deleted itself).
- Camera target limits x 944–976, y 448–2624 = the gate `id` 7 values in the page [certain].
- Gate `id` 7: state 2, closed behind the player, sprite 58 [certain]. The caller's "open" is
  wrong: the gate opened (the snake set `game+0x26`), the player passed, and it closed.
- Main 25 `var` 4 waits in state 0 at (0, 0), made by the gate (record pointer 0) [certain].
- The record list of the page (flame jets, grey blocks, viruses, Reploids) matches RAM [certain].
- New for the page: the save state row; lives 4; the Reploid status; the virus soul bits; the
  camera ease note; the grey block fall heights; the kind rule (engine).
- Not shown: the floor of the tower (checkpoint 2), main 25 `var` 4 after its set-up, the sound of
  any object.

## 5. Sprites

`spriteexport --state heatnix-4_cave.state.gz --root ~/mmx-data/x6/re/heatnix-cave/sprites-root`:
8 exports (3 live: flame jet, boss gate, grey block; 5 by search), 2 skipped (type 15 table
`0x8007789C` and type 22, as in the earlier states). All 8 IDs are the same as the shared exports,
and every `anim-*.png` is byte-identical to `~/mmx-data/x6/sprites/enemies/<name>/`. Bank sheets
0–7 and `stage-8-live-0.png` are byte-identical to `banks/heatnix/heatnix-1_start/`.

New: `banks/heatnix/heatnix-4_cave/stage-12-live-0.png` (the live grey block source, 8 sprites).
Checked by eye: correct. Flame jet strips `anim-0` (up) and `anim-2` (down) checked by eye: correct.
No wrong sheets.

Nothing new for the shared outputs except that bank sheet folder, if the main session wants the
state listed in `sprites/stages/heatnix.json`.
