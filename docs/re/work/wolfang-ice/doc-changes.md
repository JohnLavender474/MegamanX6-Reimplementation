# Doc changes from the Wolfang ice work

Exact text per file. "Replace" quotes the current text; "Add" says where. New sheets:
`sheet.md` → `docs/enemies/ice-block.md`, `sheet-ice-fall-controller.md` →
`docs/enemies/ice-fall-controller.md`. Helper names: `helpers-proposed.json`.

## 1. `docs/engine.md`, "Tile collision": the slope note

Replace:

```
  Values 17–28 are in the maps only on glass (60, members 96 and 116) and belt (58, member 112)
  floors [certain for the maps; likely that they are "slopes on a special floor"]. The glass slide
  of Shield Sheldon's stage reads them as slopes (`docs/stages/sheldon.md`).
```

with:

```
  Values 17–28 are in the maps only on slippery floors (60: the ice of Blizzard Wolfang's stage,
  member 96, and member 116) and belt (58, member 112) floors [certain for the maps]. Stage logic 7
  makes them slippery slopes ("Slippery floors and slopes (stage logic 7)" below). Member 96 has
  25, 26 and 27, 28 only (2-cell slopes, 8 px per 16 px).
```

## 2. `docs/engine.md`, "Tile collision": new sub-section

Add after the line `- No handler: 13–15, 29–31, 48–55, 61.`:

```
### Slippery floors and slopes (stage logic 7) [certain for the code]

Stage logic 7 makes collision 60 slippery and pushes the player down the slopes 17–28. The same
code is in overlay 04 (Blizzard Wolfang, `0x800ED3A4`), 07 (stage 4 area 1, `0x800F0530`) and 10
(Shield Sheldon, `0x800F0BB0`): every differing word is an address part, and the two jump tables
have the same cases. One record per area, in the fixed list. It acts only while `mega.state` = 1.
It reads the collision value under the middle of the player's feet each update.

- **Flat 60 (and 0 on an object):** the slide. When the player stops, lands or dashes on 60, the
  object keeps a slide speed and adds it to the player's own x move until it falls to 0, the
  player jumps or hits a wall, or the floor is not 60 (or 20, 24, 26, 28). Start speeds and the
  slow-down: `docs/stages/sheldon.md`, "The glass slide (stage logic 7)".
- **Slopes** (value under the feet; 16 reads the cell 16 px higher). `dx` = the player's x move of
  this update (`mega.x` − `mega+0x13C`), `s` = `mega+0x104` (the dash start speed, 6.5; 8.125
  with the dash part):

  | value | slope | the player's x move becomes | y |
  |---|---|---|---|
  | 17–20, 25, 26 | rises to the right | max(`dx` − 3.5, −`s`) | + abs(move) (down; the player's `CollideCheck` puts the player back on the slope) |
  | 21–24, 27, 28 | falls to the right | min(`dx` + 3.5, `s`) | + abs(move) |

  So with no input the player goes down the slope at 3.5 px/update. A walk uphill (2.0) still goes
  down at 1.5. A dash uphill goes up at the dash speed − 3.5.
- **The foot of a slope:** when the value of the update before was 60 and the player faces up the
  slope (right for 17–20, 25, 26; left for 21–24, 27, 28) and the step is not 12 (dash), the x
  move of this update is undone (`mega.x` = `mega+0x13C`). So from flat ice the player cannot walk
  onto an up-slope; only a dash goes on.
- After the slope move the object reads the value again. When the player is now on 60, the slide
  start (state 1) runs in the same update, with the slope move as the "last x move": a player that
  slides off a slope at 3.5 px/update keeps about 4.7 px/update on the flat ice and slows down at
  0.09375 per update, about 115 px [likely: from the numbers, not seen in play].
- Every slope value puts the slide back to state 1 (armed): the slide and the slope push do not
  add up.

The object also keeps `+0x1E` = `0x40` when the player moved right in this update, 0 when left.

For the conversion: collision 60 = ice floor, 25–28 on ice = ice slope; push the player down an
ice slope at 3.5 px/frame against its own move (cap: the dash speed); stop a walk at the foot of an
up-slope; keep the run-out on the flat ice.
```

## 3. `docs/engine.md`, "The damage tables"

Add after the `dmg_tbl_capsule` table (before `## Damage to the player`):

```
`dmg_tbl_ice_block` (`0x80075CDC`, the ice blocks of Blizzard Wolfang's stage): D for every id
except ids 4 and 13 (3), id 55 (127), ids 7 and 23 (–). Only the Metal Anchor (slot 4) hurts it.
`dmg_tbl_grey_block` (`0x80075E1C`) is the same pattern with 4 damage and id 35.
```

## 4. `docs/engine.md`, Nightmare Effect 3

Replace:

```
    read), 3 main type 19 of stage 2 (set-up `0x800EEEA0`, a fixed-list record; not read further),
```

with:

```
    read), 3 main type 19 of stage 2 (set-up `0x800EEEA0`, a fixed-list record, `id` 1: a spawner;
    `var` 0 is an orange fire ring, export `enemy-78c8f556f5`, HP 8, contact 4, damage table
    `0x8007465C`, 3 px/update down, sound (2, 80); the spawner works only in zones by stage and
    checkpoint; not read further),
```

## 5. `docs/stages/sheldon.md`, "The glass slide (stage logic 7)"

Replace:

```
   state 1 (no slide); 16 reads the cell 16 px higher; 17–28 (slopes) limit the player's x move and
   change y with it (not used in this stage: the map has no slope values).
```

with:

```
   state 1 (no slide); 16 reads the cell 16 px higher; 17–28 (slopes) push the player down the
   slope at 3.5 px/update (not used in this stage: the map has no slope values; see
   `docs/engine.md`, "Slippery floors and slopes (stage logic 7)", and Blizzard Wolfang's stage,
   which uses them).
```

And add after the first paragraph of the section (after "The object is never deleted."):

```
The same code is stage logic 7 of overlay 04 (Blizzard Wolfang, ice) and 07 (stage 4 area 1)
[certain: the words differ only in address parts].
```

## 6. `docs/stages/shark-2.md`

Replace the row:

```
| 47 | stage logic 7 | 0 | 0, 0 | overlay 07 `0x800F0530`: each update it reads the collision value under the player and branches on it (a jump table of 61 values); not read further. Value 60 is the ice of Nightmare Freeze, so this is probably the ice slide [guess] |
```

with:

```
| 47 | stage logic 7 | 0 | 0, 0 | overlay 07 `0x800F0530`: the slide on collision 60 (the ice of Nightmare Freeze) and on slopes 17–28; the same code as overlays 04 and 10 [certain]; `docs/engine.md`, "Slippery floors and slopes (stage logic 7)" |
```

Replace the open question `1. What stage logic 7 does with the floor collision value.` with:

```
1. (answered) Stage logic 7: the ice slide (`docs/engine.md`, "Slippery floors and slopes").
```

## 7. `docs/enemies/nightmare-freeze.md`

Replace:

```
Value 60 is the ice. Stage logic 7 of area 1 (`0x800F0530`) reads the collision value under the
player each update and branches on it (`docs/stages/shark-2.md`); that is probably the slide on ice
[guess: not read]. Without Nightmare Effect 2 no cell of value 60 is in the playable part of area 1.
```

with:

```
Value 60 is the ice. Stage logic 7 of area 1 (`0x800F0530`) makes it slippery: the same code as
the glass slide of Shield Sheldon's stage and the ice of Blizzard Wolfang's stage (`docs/engine.md`,
"Slippery floors and slopes (stage logic 7)") [certain for the code]. Without Nightmare Effect 2 no
cell of value 60 is in the playable part of area 1.
```

Replace open question `1. What value 60 does to the player (stage logic 7); play check: walk on the ice.` with:

```
1. (answered) Value 60 is the slippery floor of stage logic 7 (`docs/engine.md`).
```

## 8. `docs/players/movement.md`

Add before `## Summary for a reimplementation`:

```
## Ice and glass floors

Collision 60 with stage logic 7 (Shield Sheldon's glass, Blizzard Wolfang's ice, stage 4 area 1
under Nightmare Effect 2) adds a slide to the steps above, and the ice slopes (17–28) push the
player down at 3.5 px/update. This is stage code, not player code: `docs/engine.md`, "Slippery
floors and slopes (stage logic 7)".
```

## 9. `docs/objects-legend.md`, "Object types by sprite bank"

Add these rows after the Turtloid rows:

```
| wolfang | 17 | 5 | ice block: avalanche blocks, falling ice and bouncing blocks (modes by `var`), made by spawner modes of the same type; the spawners come from stage logic 12 (`docs/enemies/ice-block.md`) | `ice-block` | code reading; user (falling ice) |
| wolfang | 19 | 6 | fire ring of Nightmare Effect 3 (only when `nightmare_effect` = 3) | `enemy-78c8f556f5` | code reading |
| wolfang | 144 | 8 | scenery: snow mounds and ice wall edges, misc type 23 (7 fixed-list records, x 5120–5888; sprite = `var`; `var` 1 is always drawn) | `enemy-cd98e86270` (sprite-only, misc type) | code reading; by eye |
```

## 10. `catalog/object-names.json`, `members`

Add:

```
"96": {"logic:7": "IceSlide", "logic:12": "IceFallController", "main:17": "IceBlock", "main:19": "NightmareFireRing", "misc:23": "SnowScenery"}
```

(`main:6` is the export `purple-wolf`; no code name was checked here.)

## 11. `docs/TODO.md`

Add:

```
- [ ] Blizzard Wolfang ice: a play check of the avalanche (do the blocks hit a standing X?), of the
  rumble stop rule of stage logic 12 (5/3 each update with no falling ice) and of the `CopyTiles`
  at the end of the ice part; the reader of `game+0x27`; a stage page `docs/stages/wolfang.md`
  (records, the trigger table of `docs/enemies/ice-fall-controller.md`).
```

## 12. Process log (`docs/process/process-log.md`)

```
- 2026-10-23: Blizzard Wolfang ice objects (main 17, stage logic 12) and the slope part of stage
  logic 7, from `wolfang-1`…`wolfang-3`; agent work folder `~/mmx-data/x6/re/wolfang-ice/`.
```
