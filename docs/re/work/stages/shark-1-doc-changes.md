# Proposed changes from the Metal Shark Player rip (`shark-1_ride`)

Files that this agent may not edit. The main session decides.

## `docs/engine.md`

### "Layout", the texture block paragraph

Add after "In Shield Sheldon's stage, entry `0x16` stays in RAM ...":

> Member 98 (Metal Shark Player, stage 4) also has entry `0x16`. In `shark-1_ride` (upper path),
> entry `0x10000` is in VRAM. Entry `0x16` draws the right shaft, the lower path and the bottom
> rooms correctly [observed; no state with it in VRAM yet].

### "Object classes", the boss overlay bullet (or a new bullet on the stage overlays)

> **Stage overlays.** Stage 4 has two overlays: 06 (area 0) and 07 (area 1). The record lists of
> stage 4 area 1 (`0x800F7BD8`, `0x800F7E58`) are past the end of overlay 06 (`0x800F4020`) and are
> valid lists in overlay 07 [likely]. This gives: intro 02, stage 1 = 03, stage 4 = 06 and 07, stage
> 5 = 08, stage 6 = 09, stage 7 = 10 [certain for 02, 03, 06, 09, 10].

### "Game state", the Nightmare Effect bullet "The value is the stage that caused it"

Replace "5 a stage 7 object (state table `0x800FC6B8`, not read)" with:

> 5 the falling crates of stage 4 (stage object 15, overlay 06, set-up `0x800ECBEC`), an object of
> stage 4 area 1 (overlay 07, near `0x800EEC54`) and a stage 7 object (state table `0x800FC6B8`, not
> read); 2 an object of stage 4 area 1 (overlay 07, set-up `0x800F01BC`)

### "Game state", "Values seen"

Add:

> `shark-1_ride` `0 6 7 4 0 4 7 1 4` (`3 5 8`): the stage 4 pick after the Sheldon pick; stage 4's
> byte is 0, and `nightmare_effect` is 0.

### "Camera"

Add after the zone bullet:

> Stage 4 area 0: `camera_zone_tbl` word 8 = `0x800795E0`, 11 entries; area 1 (word 9) =
> `0x80079690`, 4 entries.

## `docs/TODO.md`, collision legend

Add rows or extend rows:

| Value | Cells | Reading so far | Confirmed by |
|---|---|---|---|
| 34 | 6 (stage 4) | one cell on the left and one on the right of each ladder top (33), at all three ladders of Metal Shark Player's stage (x 5328, 1056, 432) | map (member 98) |
| 60 | 318 (stage 4) | also 10 blocks (4 × 6, 6 × 3, 6 × 6, 8 × 6 and 16 × 3 cells) on the lower path of Metal Shark Player's stage (y 624–719), each with stage objects of type 9 (`var` ≥ 64) at its x | map and records (member 98) |

## `docs/process/process-log.md`

Proposed section 81:

> ## 81. Metal Shark Player: section 1
>
> One state: X on the Ride Armor on the upper path of stage 4. `learn` passed at once (member 98,
> 22 × 8 screens, overlay 06, 118 records). The drawing showed garbled art in the lower rooms again,
> and member 98 has the second texture block (entry `0x16`) too; a second catalog entry draws the
> lower part with it, by hand, until a state on the lower path exists. The camera zone table of area 0
> had 11 entries, and the next 4 belonged to area 1; the area 1 record lists pointed past the end of
> overlay 06 and were valid in overlay 07. So stage 4 has two overlays, and that explains the overlay
> numbers of stages 5–7. A search for `nightmare_effect` reads found the stage's effect object at once:
> falling crates that delete themselves unless the value is 5. Stage logic 30 places the Ride Armor
> in `actor2`.
>
> Lessons:
>
> - Read the next entries of a per-area table. Their pointers tell how many entries an area has, and
>   whether an area 1 exists.
> - A record list address outside the loaded overlay is a pointer into another overlay.
> - To find a stage's Nightmare Effect object, search the overlay for loads from `0x800CD30A`.

## `catalog/enemies.json`, `names`

| ID | proposed name | why |
|---|---|---|
| `enemy-9782d63f15` | `rescue-reploid-shark` | stage object 4 of stage 4; the same table `0x80078BEC` and the same 36 animations as `rescue-reploid` (`enemy-61922ae859`), but another Reploid look (blue armour), cell 47 × 98. Another option: give it the same name with a suffix by look (`rescue-reploid-blue`) |

No other new clean export: `junkroid`, `mettaur-d2`, `nightmare-virus`, `nightmare-virus-orb` and
`teleport-crystal` already have names.

## `docs/enemies/` (other owners)

- `junkroid.md`: main type 1 of overlay 06 (entry `0x800EB8E0`), 10 records in stage 4 area 0, all
  `id` 1; the export fingerprint is the same as in the intro.
- `mettaur-d2.md`: main type 27 of overlay 06 (entry `0x800EAF94`), 19 records in area 0 (5 Hard
  only), all `id` 0, and 10 in area 1; the same fingerprint as in Shield Sheldon's stage.
- `teleport-crystal.md`: stage 4 has two (`id` 0 at (1216, 1104), `id` 1 at (1552, 1584)); main type
  28 of overlay 06, entry `0x800EC678`.
- `light-capsule.md`: stage 4 record 117 has `id` 4 (Shadow Armor, bit `0x10`), at (1424, 1592).
- Crusher sheet (crusher agent): stage object 9 of overlay 06, entry `0x800E9E98`, state table
  `0x800F35FC`, animation table `0x800F35A0`; the record table is in `docs/stages/shark.md`.
- `docs/players/ride-armor.md` (Ride Armor agent): stage logic 30 (`0x800EE158`) writes `actor2`
  from record 20 (`id` 0 at (1872, 395)) and then `DeleteObjectKeep`.
