# Notes: method, evidence, open questions

## Method

1. `python3 -m mmxdemake.rekit --name sea-attacker-st`. It found the entry (`0x800EAB54`, main table
   entry 10), 22 functions, 5 state tables, the bubble shot (visual entry 9, a neighbour), 19 engine
   helpers by name, the animation table (10 animations) and the fields used.
2. Read `summary.md`, `calls.txt`, then `code.txt` from the entry, function by function.
3. Read the one call outside the walk with `--at`: `0x80049D90` (and its getter `0x8002C398`). Also
   read `ContactMega` (`0x80030ECC`, to see whether it writes the shot), `CornerCheck`
   (`0x8002EE10`, it writes `+0x70`), `BoxOverlap` (`0x8002D654`, box format) and `DropItemRoll`
   (`0x8004E680`, drop table base).
4. Read the damage tables `0x800743DC` and `0x8007429C` (80 ids each) and drop table 19 from the
   executable `SLUS_013.95` (RAM − `0x80010000` + `0x800`).
5. Listed the type-10 records of the stage list (list entry 6 in the save state) with
   `mmxdemake.objects`.
6. Looked at the export (`~/mmx-data/x6/sprites/enemies/sea-attacker-st/`, 10 animations) to name
   the animations.

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | all 22 functions, annotated |
| `kit/data.txt` | boxes `0x800F5B98`–`0x800F5BA7`, `0x800F5C98`–`0x800F5CA3`, animation frames, debris list `0x800F5C50`, the dispatch tables |
| `kit/summary.md` | helpers, tables, fields, animations |
| executable `SLUS_013.95` | damage tables `0x800743DC`, `0x8007429C`; drop weights `0x80078860`, items `0x800788F0` |

## Key facts and where they come from

- HP 6, contact damage 4, priority 7, boxes, damage table: `sea_init` (`0x800EABCC`).
- Mode by `var`: `sea_init` (`var == 0` → step 2, animation 0; else step 3, animation 3).
- 96 px trigger: `shooter_wait` (`slti v0, v0, 97` on `|mega.x.px − x.px|`).
- FIRE events and the 20-frame rest: `shooter_fire` (flags 1 and 2), `shooter_rest`.
- Face timer 30 at `+0x80`: written through `current_main` (`0x1F80004C`) in `sea_init`,
  `shooter_rest`, `diver_hover`. `current_main` is the object being updated, so this is `self+0x80`
  [likely].
- Diver: `0x3C000` (3.75 px/frame) up, random `& 0x1F` at `+0x82`, rise test `mega.y.px − y.px < r`,
  timer 40, dash `±0x3C000`, window `0x18000`, drop `0xFFFC4000`.
- Shot: spawn offsets +6/−6 and −20, speed `0x40000` up, wait 40/8 frames by `game.difficulty < 2`,
  then `±0x40000`.
- The shot ignores the `ContactMega` result. `ContactMega` writes nothing to the shot (no store
  through `s1`). `+0x70` is written only by `CornerCheck` (`0x8002F048`); the test before
  `CornerCheck` reads the value of the previous frame (0 after `GetVisualObject` clears the slot).
- Shot damage table `0x8007429C`: all 80 entries are class 0, damage 127.

## Hand-simulated timing

Each main update: `AnimeTick`, then the step. `SetAnime` loads frame 0. A frame with duration d is
replaced in the d-th `AnimeTick` after it was loaded.

- FIRE: frame 3 (flag 1) loads 6 + 15 + 2 = 23 updates after `SetAnime(1)`; frame 7 (flag 2) 47
  updates after. REST ends 20 updates later. WAIT runs again 1 update after that: period 68 updates.
- CURL: frame 3 of animation 3 loads 24 updates after the set-up.

## Checked against the save state

- 18 type-10 records in list entry 6: 13 with id 1, 5 with id 0 (kinds 0, 1, 2; see the sheet).
- No Sea Attacker ST is live in the save state, so the run-time fields were not seen.
- The export shows all 10 animations of the table; the sprites match the use in the code.

## What the kit saved, and what stayed manual

- Saved: the function list, the dispatch tables, the helper names, the field names, the animation
  table, the neighbour (visual 9), the boxes in hex.
- Manual: `0x80049D90` and `0x8002C398` (not in the catalog); the player fields (the kit shows
  `0x800970A8`, `0x800970AA`, `0x800970AE`, `0x800970B4` as unnamed globals; they are `mega+0x08`
  `x`, `+0x0A` `x.px`, `+0x0E` `y.px`, `+0x14` `layer`); the fields through `current_main` (`+0x80`,
  `+0x82` are missing from the fields table); the record list; drop table 19.
- Catalog problem: `0x8007429C` is named `dmg_tbl_guard` ("everything deflected"). The data are
  class 0, damage 127 for all 80 ids. The deflect table is `0x8007433C` (class `0xFF`), as the
  Meta Dridler and D-1000 notes say. The D-1000 notes call `0x8007429C` the shot table, and the
  reconstructed code calls it `dmg_tbl_shot`.

## Open questions

1. The bubble and `ContactMega`: does it pass through the player in play?
2. How often DASH misses the ±1.5 px window.
3. Draw priority 18 in the diver.
4. The effect of `0x80049D90` (not seen).
5. Steps 0 and 1 have no user.
6. Weapon ids 21–23 and 32–79.
7. The player's origin point (sets the bubble and diver heights).
