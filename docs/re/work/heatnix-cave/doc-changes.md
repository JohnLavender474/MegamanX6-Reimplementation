# Doc changes from `heatnix-4_cave`

Exact old and new text per doc file. The facts and their checks are in `notes.md`. Another agent
works on `heatnix-3_rekkoha`; where both change the same lines of `docs/stages/heatnix.md`
(the save-state table, the "lives" line), merge by hand.

## `docs/enemies/flame-jet.md`

### 1. The header

Old:

````text
Overlay `05` (Blaze Heatnix, stage 3), code at `0x800E98C0`–`0x800EA07C`. Save state
`heatnix-1_start` (stage 3, area 0, Zero, Normal). No flame jet is live in the state; the animation
table was found by the code search. Kit: `python3 -m mmxdemake.rekit --name flame-jet --state
````

New:

````text
Overlay `05` (Blaze Heatnix, stage 3), code at `0x800E98C0`–`0x800EA07C`. Save states
`heatnix-1_start` (stage 3, area 0, Zero, Normal; no flame jet is live, the animation table was
found by the code search) and `heatnix-4_cave` (six jets live in the first tower; see "Checked in a
save state"). Kit: `python3 -m mmxdemake.rekit --name flame-jet --state
````

### 2. A new section before "## Summary for a reimplementation"

Old:

````text
## Summary for a reimplementation

- A fixed fire nozzle; `id` bits 4–7: 0 = up, else down; bits 0–3: 0 = one 180-frame flame, 1–7 =
````

New:

````text
## Checked in a save state

`heatnix-4_cave` (checkpoint 1, Normal; camera (944, 1727)) has six live jets [certain for the
values]:

| slot | record | `var` | x, y | state | sprite | box `+0x80` (x, y, w, h) | `+0x7C` | `+0x85` |
|---|---|---|---|---|---|---|---|---|
| main 1 | 35 | 4 | 1153, 1930 | FIRE sub 1 | 0 | −8, −16, 16, 16 | 14 | 1 |
| main 2 | 38 | 20 | 1185, 1984 | FIRE sub 3 | 23 | −8, 0, 16, 56 | 0 | 3 |
| main 3 | 40 | 24 | 1105, 1984 | PAUSE sub 1 | 13 | −8, 0, 16, 1 (no box) | 27 | 0 |
| main 4 | 36 | 16 | 1153, 1632 | FIRE sub 2 | 18 | −8, 0, 16, 88 | 83 | 1 |
| main 5 | 41 | 16 | 977, 1984 | FIRE sub 2 | 19 | −8, 0, 16, 88 | 108 | 1 |
| main 6 | 37 | 20 | 1265, 1632 | FIRE sub 2 | 19 | −8, 0, 16, 88 | 9 | 2 |

This confirms: the direction bits (`id` 4 up with the box above the point; 16, 20, 24 down with
y offset 0; `flip` 0 on all), the box height by sprite (`0x800F3CD0` in RAM), the attack box
pointer (`+0x50` = the object's `+0x80` while it burns, 0 on sprite 13), the hold times (≤ 14 for
bursts, ≤ 180 for `id` 16), the burst counter `+0x85` that counts down, the PAUSE wait, HP 12,
contact 16 and no hurt box. Records 34 and 39 (kind 2, Hard) are in the view and not spawned on
Normal. Jets 95 px above the view are live with `display` 0; records 27–33 (245 px and more above)
are deleted (`OffScreenCheck2(160, 160)`). The state does not show the sound, the random burst
count of `id` 24 or `+0x84` (0 on all six).

## Summary for a reimplementation

- A fixed fire nozzle; `id` bits 4–7: 0 = up, else down; bits 0–3: 0 = one 180-frame flame, 1–7 =
````

### 3. Open question 3

Old:

````text
3. The look in play (up or down nozzle, fire colour): the export `enemy-7f9bbc9a47` was drawn from
   the bank of the start state; no flame jet was live, so the palette is the bank palette [likely
   correct].
````

New:

````text
3. The look of the nozzle in play. The colours are answered: the export `enemy-7f9bbc9a47` from the
   live jets of `heatnix-4_cave` (palette `0x788C`, the bank 3 CLUT) is byte-identical to the export
   from the bank [certain].
````

## `docs/enemies/grey-block.md`

### 1. The "Stage 3" paragraph

Old:

````text
Records: 5 with `id` 3 in the first tower and 10 with `id` 4 in the shaft room and the side
corridor (`docs/stages/heatnix.md`). Sprite type 142 is bank 5 there; the bank sheet is the same
as that of stage 5.
````

New:

````text
Records: 5 with `id` 3 in the first tower and 10 with `id` 4 in the shaft room and the side
corridor (`docs/stages/heatnix.md`). Sprite type 142 is bank 5 there; the bank sheet is the same
as that of stage 5. In `heatnix-4_cave` (checkpoint 1, camera (944, 1727)) records 111–114 are live
(`var` 3, `work.7E` = the record y) [certain for the values]:

| record | start y | live y | step | timer | floor | fall |
|---|---|---|---|---|---|---|
| 111 (1056) | 712 | 750 | RISE, `speed_y` +3 | −1 | 816 | 80 |
| 112 (1152) | 872 | 894 | RISE | −1 | 1056 | 160 |
| 113 (1184) | 1240 | 1293 | RISE | −1 | 1440 | 176 |
| 114 (1152) | 1528 | 1576 | REST, `speed_y` −4 | 119 | 1600 | 48 |
| 115 (992) | 2104 | not reached | – | – | 2272 | 144 |

The floors are from the layer-0 collision. Block 114 rests at the floor − 24 [certain]. Blocks
111–113 are stopped in the rise, 434–977 px above the view, as `OffScreenCheck2(64, 240)` predicts.
All four hold sprite 4 with flag 2.
````

### 2. Animation 0

Old:

````text
Animation 0 (table `0x800EC5F8`): sprites 7, 0, 1, 2, 3, 4, 5, then 4, 6, 4, 5 repeated; 2, 2, then
3 updates per frame; it holds on the last frame.
````

New:

````text
Animation 0 (table `0x800EC5F8`): sprites 7, 0, 1, 2, 3, 4, 5, 4, 6 (flag 0), then 4, 5, 4, 6, 4,
5, 4, 6, 4 (flag 1), then 4 (flag 2, held); 2, 2, then 3 updates per frame [certain: the stage 3
copy `0x800F3DB0` in `heatnix-4_cave`].
````

### 3. Open question 4

Old:

````text
4. Sprite 7 (the first frame) and the look of each frame (the export `grey-block`).
````

New:

````text
4. The look of each frame in play. The live sheet of `heatnix-4_cave` shows sprite 7 empty,
   sprites 0–3 the block that grows down from its top edge, 4–6 the full block (5 darker)
   [likely].
````

## `docs/enemies/nightmare-virus.md`

### 1. Stage 3, after the Shield Sheldon paragraph

Old:

````text
they are a different object (stage-object type 15), not read.
````

New:

````text
they are a different object (stage-object type 15), not read.

In Blaze Heatnix's stage (stage 3, area 0; `docs/stages/heatnix.md`) the records use `id` 0, 1,
32 and 33; the soul row is 2. In `heatnix-4_cave` (Normal, checkpoint 1, `nightmare_effect` 4) no
virus is live, but the soul bits of row 2 are `0x2D`: the four Normal records of the first tower
above the player (60, 62, 63, 65) were made and killed; the Hard records 61 and 64 were not made.
Zero's souls are 32 more than 3632 frames before (4 × 8) [certain for the values].
````

### 2. Death, the soul bits

Old:

````text
  `turtloid-1` the Yammark row is 0 again, so the bits are probably cleared when a stage starts
  [likely].
````

New:

````text
  `turtloid-1` the Yammark row is 0 again, so the bits are probably cleared when a stage starts
  [likely]. Stage 3 uses row 2 (`0x2D` in `heatnix-4_cave`).
- A killed virus is deleted with `DeleteObject` (state 2), an off-screen one with `DeleteObject2`
  (state 3). So its record keeps kind high digit `0x10` after a kill and comes back only on the
  third entry into the spawn band (`docs/engine.md`, "Spawning and despawning") [certain for the
  code; seen in `heatnix-4_cave`: records 60, 62, 63 `0x20`, record 65 `0x10` in the view, not live].
````

### 3. Nightmare Effect

Old:

````text
without the Nightmare Effect. Whether the effect changes which virus records spawn is not known.
````

New:

````text
without the Nightmare Effect. The spawner (`SpawnInRect`, `RecordSpawnFilter`) does not read
`nightmare_effect` either; in the main program the only access found is the store of
`StageStartReset` [certain for the direct addresses]. So the virus records spawn for every effect
value [likely]; in stage 3 they spawn with value 4 [certain: `heatnix-4_cave`].
````

### 4. Open questions 3 and 4

Old:

````text
3. Whether the spawner skips a virus record whose "soul taken" bit is set.
4. The Nightmare Effect: which code makes the stage different, and whether it changes virus records.
````

New:

````text
3. Answered: the spawner reads only the record (flags, kind), not the "soul taken" bits. A killed
   virus record waits for its third entry into the spawn band (kind high digit).
4. The Nightmare Effect: it does not change the virus records (see "Nightmare Effect"); which
   objects it changes is per stage (`docs/engine.md`).
````

## `docs/stages/heatnix.md`

### 1. The save-state lines

Old:

````text
Save states (in `~/mmx-data/x6/dumps/`; Zero, form 5; Normal (difficulty 1); lives 5; armour parts
`0x0C`; `nightmare_effect` 4):
````

New:

````text
Save states (in `~/mmx-data/x6/dumps/`; Zero, form 5; Normal (difficulty 1); lives 5 (4 in
`heatnix-4_cave`); armour parts `0x0C`; `nightmare_effect` 4):
````

Add this row after the last row of the table (after `heatnix-2_mini-boss`, or after the
`heatnix-3_rekkoha` row when the other agent adds one):

````text
| `heatnix-4_cave` | 1012, 1898 | 3989 | the first tower, checkpoint 1, after the first snake (`game+0x26` = 1). Live: flame jets (main 36) records 35–38, 40, 41; grey blocks records 111–114; main type 25 `var` 4 (made by gate `id` 7, state 0); gate `id` 7 closed (state 2); talk zones 129, 130; stage logic 27. No virus is live; records 60, 62, 63, 65 were killed (soul bits `0x2D`). Reploids `id` 32 and 34 saved |
````

### 2. The first tower in "The map"

Old:

````text
3. **The first tower** (camera x 944–976, y 448–2624): a tall shaft that goes down. 18 flame jets,
   5 grey blocks (`id` 3), 8 Nightmare Viruses, 2 Rescue Reploids. Checkpoint 1 at the top.
````

New:

````text
3. **The first tower** (camera x 944–976, y 448–2624): a tall shaft that goes down. 18 flame jets,
   5 grey blocks (`id` 3), 8 Nightmare Viruses, 2 Rescue Reploids. Checkpoint 1 at the top.
   In `heatnix-4_cave` the current max y is the camera y + 2, not 2624: `CameraLimitEase` keeps a
   target below the camera just ahead of it while the player stays on the follow line
   (`docs/engine.md`, "Camera") [certain for the code and the state]. Min y 448 is the gate value
   (checkpoint 1 restarts with 464).
````

### 3. The notes on the objects

Old:

````text
- Grey block (stage object 12, 15 records): the same code as overlays 08 and 29, instruction for
  instruction after relocation (344 words) [certain]; only under Nightmare Effect 4.
````

New:

````text
- Grey block (stage object 12, 15 records): the same code as overlays 08 and 29, instruction for
  instruction after relocation (344 words) [certain]; only under Nightmare Effect 4. The falls in
  the first tower (records 111–115) are 80, 160, 176, 48 and 144 px (`docs/enemies/grey-block.md`).
````

Old:

````text
- Rescue Reploids (records 45–59): `id` 32–47, the status words `game+0xE8` and `+0xEC` (words 4
  and 5), both 0 in the states (no Reploid of this stage met yet).
````

New:

````text
- Rescue Reploids (records 45–59): `id` 32–47, the status words `game+0xE8` and `+0xEC` (words 4
  and 5), both 0 in `heatnix-1` and `-2`. In `heatnix-4_cave` word 4 is `0x202`: `id` 32 and 34
  saved.
````

Old:

````text
- Nightmare Virus: `id` 0 and 1 normal; `id` 32 (record 82) and 33 (records 67, 83) are snatchers
  next to Reploid records 57, 47 and 59.
````

New:

````text
- Nightmare Virus: `id` 0 and 1 normal; `id` 32 (record 82) and 33 (records 67, 83) are snatchers
  next to Reploid records 57, 47 and 59. They spawn under Nightmare Effect 4 (no effect test).
  Soul row 2. In `heatnix-4_cave` records 60, 62, 63 and 65 were killed (soul bits `0x2D`); the
  next one to spawn below the save point is record 66 (1056, 2032), when the camera y passes about 1744 [likely: the band
  is camera y + 288; the strip bounds of `ScrollSpawn` were not read].
````

### 4. The Nightmare Effect section

Old:

````text
the earlier Metal Shark Player pick (targets `3 5 8`) [certain for the values]. Value 4 is
**Nightmare Iron** (`docs/stages/shark.md`, the Fandom table).
````

New:

````text
the earlier Metal Shark Player pick (targets `3 5 8`) [certain for the values]. Value 4 is
**Nightmare Iron** (`docs/stages/shark.md`, the Fandom table). `heatnix-3_rekkoha` and
`heatnix-4_cave` come from another run: `game.stage_effect` = `0 8 3 4 2 4 8 5 3` (a stage 8 pick,
targets `1 6 6`, before the stage 3 pick); stage 3 is still 4.
````

## `docs/engine.md`

### 1. Despawn (in "Spawning and despawning")

Old:

````text
- **Despawn.** `DeleteObject` (`0x8002CA14`) clears the record's "spawned" bit (`flags &= 0x70`) and
  the object. So the object comes back when its record leaves the spawn band and enters it again,
  also after it was killed [likely]. It skips the record when `+0x10` is 0 (an object that no record
  made). `DeleteObject2` (`0x8002CA54`) also clears the high nibble of the record kind when it is
  `0x10` [certain for the code; the effect is not known]. `DeleteObject3` (`0x8002C9B0`) only clears
````

New:

````text
- **Despawn.** `DeleteObject` (`0x8002CA14`) clears the record's "spawned" bit (`flags &= 0x70`) and
  the object. It skips the record when `+0x10` is 0 (an object that no record made).
  `DeleteObject2` (`0x8002CA54`) also changes the record kind high digit `0x10` to 0. Objects use
  `DeleteObject2` when they go off screen and `DeleteObject` when they are killed (Nightmare Virus
  states 3 and 2). `DeleteObject3` (`0x8002C9B0`) only clears
````

Then add after the Despawn bullet (before "- **Death.**" is not the order in the file; put it
directly after the Despawn bullet, which ends with "[certain for the code]."):

````text
- **The kind high digit** (records of kind 0–2) [certain for the code]. `SpawnInRect` calls
  `RecordSpawnFilter` (`0x80029E7C`) for a record whose point is in the spawn rectangle. On
  difficulty 3 it spawns and keeps the digit. Else, after the difficulty test: digit 0 → `0x10`,
  spawn; `0x10`, `0x30`, `0x50` → no spawn; `0x20` → `0x30`, no spawn; `0x40` → `0x10`, spawn (not on
  Easy); `0x60` → `0x30`, no spawn (not on Easy). For a record whose point is outside the band
  (camera x − 48 … + 368, y − 48 … + 288): `0x10` → `0x20`, `0x30` → `0x40`, `0x50` → `0x60`.
  So an object that went off screen (`DeleteObject2`, digit 0) comes back at the next entry; a
  killed object (`DeleteObject`, digit stays `0x10`) comes back only when its point leaves the band,
  enters it (no spawn), leaves it again and enters a third time, and never on Easy. `RecordsReset`
  clears the digit at a restart. No writer of `0x50` was found. Seen in `heatnix-4_cave`: killed
  virus records with `0x20` (left the band) and `0x10` (in the view, not live).
- **Spawn margin.** When the record flags have bits `0x70`, `SpawnInRect` widens the rectangle on
  the side of the strip by `spawn_margin_tbl` (`0x800733B8`)[(flags >> 4) − 1] = 32, 64, 96, 128,
  160, 480 px [certain for the code; which records use it was not checked].
````

### 2. Camera, the limit ease

Old:

````text
limits; the camera then eases to them. The teleporter sets the cameras of all three layers
directly. The limits are the top-left position of the 320 × 240 view.
````

New:

````text
limits; the camera then eases to them. The teleporter sets the cameras of all three layers
directly. The limits are the top-left position of the 320 × 240 view.

`CameraLimitEase` moves a limit relative to the camera, not on its own. Max y (`0x80029180`), when
the target is below the camera: if (player y − camera y − `cam+0x2E` + 2) ≥ 4 (the player is more
than 1 px below the follow line `cam+0x2E`), max y = the target at once; else max y = camera y +
max(0, player y − `mega+0x1E`) + `cam+0x47`, not past the target. When the target is above the
camera: max y = max(camera y − `cam+0x47`, target). The other limits use the same pattern [likely;
min x and min y read in part]. In `heatnix-4_cave` (`cam+0x2E` = 171, Zero standing on the line)
max y = 1727 + 0 + 2 = 1729 while the target is 2624 [certain].
````

## `docs/objects-legend.md`

Old:

````text
High digit (0x10–0x60): a respawn counter that the game changes at run time
````

New:

````text
High digit (0x10–0x60): a respawn counter that the game changes at run time; a killed object comes back on the third entry into the spawn band (`docs/engine.md`, "The kind high digit")
````
