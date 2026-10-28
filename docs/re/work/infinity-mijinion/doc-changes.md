# Proposed doc changes (Infinity Mijinion)

## 1. New file `docs/enemies/infinity-mijinion.md`

Copy [`~/mmx-data/x6/re/infinity-mijinion/sheet.md`](sheet.md) as it is.

## 2. `docs/engine.md`

### "The damage tables": add after the `dmg_tbl_wolfang` table

````
`dmg_tbl_mijinion` (`0x8007569C`, Infinity Mijinion, his clones and bubbles,
`docs/enemies/infinity-mijinion.md`):

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 2 2 2 – D 1 2 2 2 2 2 2 |
| 16–31 | 2 D 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 2 2 2 2 2 2 2 2 2 D 1 4 8 |
| 48–63 | 2 D 2 2 1 8 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |
````

### "Weapon ids": add a row after the row `| 3, 12, 34 | ...`

````
| 16, 53 | the stun ids of Infinity Mijinion (2 and 8 damage; `docs/enemies/infinity-mijinion.md`): Guard Shell (slot 7) charged and its shards. Id 7 (Guard Shell uncharged) is class 2 in his table, so his id 7 stun case is never reached [certain for the tables] |
````

### "Boss sequence": add at the end of the paragraph

````
Blizzard Wolfang, Ground Scaravich and Infinity Mijinion (overlay 11, fight timer `0x800FDA84`)
use the same death and soul code.
````

### "Tile collision": add to the item about 37–47

````
Infinity Mijinion's bar emitters write collision 45 cells at run time (`CopyTiles` from the hidden
cell (5632, 1200) of member 103), so a temporary hang bar appears in the boss room
(`docs/enemies/infinity-mijinion.md`).
````

## 3. `catalog/enemies.json`, `names`

````
"enemy-b75a43c056": "infinity-mijinion",
````

## 4. `catalog/helpers.json`

Add the entries of `~/mmx-data/x6/re/infinity-mijinion/helpers-proposed.json` (no name or address
clash with the catalog of today).

## 5. A stage page for stage 8 (when it is made, `docs/stages/mijinion.md`)

````
| 52 | 4 | 8 | 0 | 5488, 1088 | boss gate: camera fixed at x 5488 |
| 53 | 4 | 8 | 27 | 5808, 1088 | boss gate: camera x 5808–5824, y 832–928, time stamp |
| 54 | 0 | 58 | 0 | 5968, 1032 | Infinity Mijinion (`docs/enemies/infinity-mijinion.md`) |
````

## 6. `docs/TODO.md`

Add to the item on wrong sheets in boss states:

````
- `mijinion-3_boss-fight`: bank 3 (main type 8 of stage 8, a jet enemy) is drawn from overwritten
  cells, but the tool does not mark it "overwritten"; it makes the wrong export `enemy-08bc9efa49`
  and a wrong `bank-3.png`. In `mijinion-1_start` the same bank is clean. The overwrite test misses
  this bank [likely: the boss pack covers its cells but the test uses another cell range].
````

## 7. Process log (`docs/process/process-log.md`)

````
- Infinity Mijinion (main 58, overlay 11): boss sheet from `mijinion-3_boss-fight`; damage table
  `dmg_tbl_mijinion`; the bar emitters write collision 45 (hang bars) with `CopyTiles`.
````
