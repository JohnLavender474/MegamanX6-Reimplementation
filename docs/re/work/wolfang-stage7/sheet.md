# Ice stack block (stage 04, stage object type 7): behaviour sheet

Proposed as `docs/enemies/ice-stack-block.md`.

The ice stack block is a square block of dark blue ice (48 × 48 px, with icicles on its top
edge) in the lower part of Blizzard Wolfang's stage (North Pole Area). The blocks fall from the
ceiling, stop on the floor or on other blocks, and then become map tiles. So they pile up, and the
player climbs on them. One object type does all of it; the `var` picks the job. Two jobs are used:

- **The ice pits** (x 4992–5632, y 1248–1616): two generators (`var` 0, records 58 and 59) drop
  single blocks (`var` 12) into a random column of a 6-column pit. One block in two carries a
  Wolfloid (`docs/enemies/wolfloid.md`, ids 4 and 5). The blocks fill each pit to a fixed height
  (3 or 4 blocks), so the player can climb out on them.
- **The ice shaft** (x 5760–6000, y 312–1264): stage logic type 8 (the "ice shaft controller",
  below) drops rows of 5 block places with **one gap** (`var` 1–5, 4 visible blocks each). The
  player climbs up the shaft: the player must jump up through the gap of each falling row. When a
  row lands, a block appears in its gap (`var` 11), and that block kills a player who stands in the
  gap.

A block that lands on the player while the player stands on the floor breaks, and the player
takes the block's contact damage. "Ice stack block" is a descriptive name; no official name was
found. This object is not the ice block of main type 17 (`docs/enemies/ice-block.md`).

Overlay `04` (stage 2, area 0). Entry `0x800ECDE8`, state table `0x800F6B9C`, step2 table
`0x800F6BD8`, animation table `0x800F6B6C`, sprite type 134 (bank 4). Save state
`wolfang-5_ice-pit` has the first generator and two pit blocks live (one with a Wolfloid). No save
state has the ice shaft. Kit: `python3 -m mmxdemake.rekit --state wolfang-5_ice-pit.state.gz
--type 7 --class stage`; the stage logic 8 code and two main-program helpers were read with
`--at`. Export: `enemy-583d456ecb` (proposed name `ice-stack-block`). Units, tags and shared rules:
`docs/engine.md`. No stage page for stage 2 exists yet. All values are [certain for the code]
unless a line says otherwise.

## The `var` parameter

| `var` | what | made by |
|---|---|---|
| 0 | pit generator (invisible) | records 58 (x 4992) and 59 (x 5344) |
| 1–5 | row: an invisible 240 × 48 px body with 4 visible blocks; the gap is at place 5 − `var` (0 = left) | stage logic 8 |
| 6–9 | a block that falls when nothing is under it (step2 0); no user found [likely: unused] | nothing |
| 10 | one visible block of a row | a row (`var` 1–5) |
| 11 | gap filler: a block that appears in place (contact damage 127) | a row, when it lands |
| 12 | pit block: falls 2 px/frame; 50 % with a Wolfloid | a pit generator |

## Spawn and set-up (state 0, `0x800EBAA0`)

For all `var` values:

- State 1, `step` 2 (not used), flags `0x51`, `flip` 0, layer = `mega.layer`.
- Sprite source: the bank of sprite type 134 (texture, sprite data, palette).
- **HP 32**, damage table `dmg_tbl_capsule`, contact damage **6**, `iframes` 0, attack box none.
- Terrain box `+0x80` = (0, 0, 24, 24): a 48 × 48 px body centred on the origin. Hurt box
  `0x800F6B94` = (−24, −24, 48, 48).
- `+0xA2` = 1: the body is a solid rectangle for `TerrainPointCheck` (value 56).
- Draw priority 6, animation 0, `speed_y` −1.0 (1 px/frame down), `speed_x` 0, `+0x84` = 0,
  the six column counters `+0x86`…`+0x8B` = 0.

Then by `var` (switch `0x800E98C0`):

| `var` | set-up |
|---|---|
| 0 | `act4` (block count) 0, no terrain box, step2 8 |
| 1–5 | 4 blocks (`var` 10, `+0x7C` = the row) at x − offset, same y (offsets: table below); terrain box (0, 0, 120, 24) (240 × 48 px), no hurt box, not drawn, `act4` 0, **state 3** |
| 6–9 | nothing more: step2 0 |
| 10 | contact damage **16**, `act4` 0, step2 1 |
| 11 | contact damage **127**, no terrain box, no hurt box, not drawn, `act4` 0, step2 4; animation 1 when `TerrainPointCheck(x, y − 26)` is solid (a block with no icicle edge), else 0 |
| 12 | 50 % (`GetRNG & 1` = 0): a Wolfloid (main type 6, flags `0x41`, `var` = 4 + (`GetRNG & 1`), at the block position, `+0x80` = the block, sprite source of type 6); then `speed_y` × 2 (−2.0: 2 px/frame down), step2 6 |

Row offsets (`0x800F6BB0`; block x = row x − offset) and the gap:

| `var` | block offsets | gap at (row x +) | gap place (0 = left) |
|---|---|---|---|
| 1 | 96, 48, 0, −48 | 96 | 4 |
| 2 | 96, 48, 0, −96 | 48 | 3 |
| 3 | 96, 48, −48, −96 | 0 | 2 |
| 4 | 96, 0, −48, −96 | −48 | 1 |
| 5 | 48, 0, −48, −96 | −96 | 0 |

## State machine

States (`0x800F6B9C`): 0 set-up, 1 main `0x800EBF18`, 2 `DeleteObject3`, 3 row `0x800ECC10`
(`var` 1–5 only), 4 `DeleteObject3` (a broken block).

The entry (`0x800ECDE8`) saves `prev_x`/`prev_y` and runs the state. It does nothing while
`mega.special` (`mega+0xE0`) = 2 [certain for the code; the meaning of value 2 is not known].

### Main (state 1)

1. Run the step2 function (table `0x800F6BD8`, below).
2. Step2 4 or ≥ 8: not drawn; end. Step2 5: `display` toggles (blink); end.
3. Clear the flash bit of the palette. `SolidWeaponHit`: a shot that touches the block hits it,
   but **the result is not used**: shots cannot destroy a block.
4. Off screen by more than 128 px (`OffScreenCheck2(128, 128)`): `display` is not changed. Else
   `DisplayCheck1` (32 px).
5. `var` 12 only: `SolidMainObjects` (`0x80031F4C`): the block is solid for the main objects (the
   Wolfloid). It pushes them out, carries the ones on top, and puts back a main object that the
   push squeezes into the terrain [likely for the names of the effects].
6. `SolidVisualHit` (`0x80032BE8`): a visual object (for example a Wolfloid cannon ball) that
   touches the block gets all 4 terrain flags, so it ends on the block [likely for the effect].
7. `SolidPushShots`.
8. `SolidObject` (solid for the player) only when `mega.squeeze_timer` = 0 and the player is less
   than 48 px away in x.
9. The crush test (below).

### The crush test [certain for the code; likely for the player side]

The test uses the touch flags of `SolidObject` (`+0x72` bit `0x04`: the player is under the block)
and a copy of the player (`0x800F76EC`, `0x154` bytes): `CollideCheck` on the copy tells if the
player stands on the floor tiles.

- **Row block (`var` 10):** when the row's `act4` = 4 (the row broke): 2 debris (animations 5, 6),
  delete. Else, player under the block and on the floor: the row's `act4` = 4, so all 4 blocks of
  the row break in this or the next update.
- **Other blocks:** player under the block, on the floor, and the block falls (step2 6, so in
  practice a pit block, `var` 12): 3 debris (animations 5, 6, 7), the generator's block count and
  column count − 1, state 4. A Wolfloid on the block dies with its drop. A gap filler never falls,
  so it never breaks.

At the same time the player gets the squeeze damage of `docs/engine.md` ("Squeeze"): the block
touches the head (`mega+0x71` bit 4) while the feet are on the floor, so HP −= the block's contact
damage: **6** for a pit block, **16** for a row block, **127** (death) for a gap filler or for a
pit block that falls between two walls (step2 6, below). After a squeeze, `mega.squeeze_timer` is
75, and for 75 updates no block is solid for the player.

So a block does not kill the player in most cases: it breaks and does 6 or 16 damage.

### Step2 functions (table `0x800F6BD8`)

| step2 | address | used by | what |
|---|---|---|---|
| 0 | `0x800EC1DC` | `var` 6–9 | `MoveObject`; with `speed_x` ≠ 0, stop at a wall or a block (step2 7, x to 16n + 8); with no block or floor under it, fall (`speed_y` −1, step2 6). No code sets `speed_x` |
| 1 | `0x800ECE88` | `var` 10 | fall with the row (below) |
| 2 | `0x800ECF20` | `var` 10, 11 | sparkle (misc type 9), priority 7, `+0x84` = 31, step2 3 |
| 3 | `0x800EC4DC` | landed blocks | freeze into map tiles (below) |
| 4 | `0x800ECFE8` | `var` 11 | not drawn; `TerrainPointCheck(x, y)` solid → state 2; else drawn, step2 5 |
| 5 | `0x800ED044` | `var` 11 | damage table, hurt box, terrain box (0, 0, 24, 24); step2 2 |
| 6 | `0x800EC628` | `var` 12, 6–9 | fall (below) |
| 7 | `0x800ED08C` | `var` 12, 6–9 | sparkle (misc type 9), priority 7, `+0x84` = 0, step2 3 |
| 8 | `0x800ED150` | `var` 0 | wait for the player (below) |
| 9 | `0x800EC7DC` | `var` 0 | make blocks (below) |

### Pit generator (`var` 0)

**Wait (step2 8).** The generator with x < 5312 (record 58) waits until the player is at
x ≥ 5137 and y ≥ 1473 (in the first pit). The other one (record 59) waits until x ≥ 5345 and
y ≥ 1489. Then it sets both target x limits of the layer-0 camera (`cam.target_max_x`,
`cam.target_min_x`) to **4976** (first) or **5328** (second): the view locks on the pit. Step2 9.

**Make blocks (step2 9).** Each update:

1. `c` = `GetRNG % 6` (a column, 0 = left).
2. When the counter `+0x84` (u8, + 1 each update, 0 at the start) is a multiple of the period
   (**128 updates**; **64** when `game.difficulty` ≥ 2) and the block count `act4` is below the
   total (**18** first pit, **24** second pit):
   - if column `c` is full (**3** blocks first pit, **4** second pit), take the next column to the
     right (`(c + 1) % 6`), up to 5 times;
   - a block: `var` 12, x = **5016 + 48 × c** (first) or **5368 + 48 × c** (second), y **1248**,
     `act4` = c, `+0x7C` = the generator, the generator's sprite source;
   - column count + 1, block count + 1 (also when no free slot was found).
3. End: when the player is at x ≥ **5281** and the camera target is 4976 (first pit), or x ≥
   **5633** and 5328 (second pit): both target x limits = **5328** or **5712** (the view moves on),
   state 2 (deleted). The blocks stay.

So the first block falls at once when the view locks, then one every 128 updates (2.13 s). The
column is random; **there is no fixed opening and no test of the player position**. Only a full
column is skipped. 18 blocks in 6 columns of 3 fill the first pit to 144 px above its floor
(y 1472). The player leaves the pit on the right (x ≥ 5281) [likely: the top of the pillar between
the two pits is at about y 1424, from the stage drawing].

### Pit block fall (step2 6)

- `MoveObject`: **2 px down per update** (no gravity). Above y 1296 (the spawn y is 1248, in the
  ceiling) nothing else happens.
- From y 1296: `CollideCheck`. Then:
  - **Another block under it** (`StageBlockAt`, `0x800EB96C`, at its two bottom corners,
    (x − 24, y + 23) and (x + 23, y + 23)): it goes back to `prev_y`. It waits on top of a block
    that has landed but is not a tile yet.
  - **Between two walls:** when the tiles 8 px left and right of the block (`TileCollisionAt` at
    (x − 32, y) and (x + 32, y)) are both not 0, contact damage = **127**. The value stays. So a
    block that falls in a one-column well (the pit wall or a higher stack on each side) kills a
    player under it. Blocks that have not become tiles yet do not count (they are objects).
  - **Floor** (`collide_flags` bit `0x08`): step2 7, y to 16n + 8, speeds 0, sound (2, 17) when
    drawn.

### Freeze into tiles (step2 7 / 2, then 3)

- Step2 7 (pit block) or 2 (row block, gap filler): one sparkle (misc type 9) on the block, draw
  priority 7, step2 3.
- Step2 3: `AnimeTick`; `+0x84` + 1. At `+0x84` = 32: animation 2 (the block flashes light blue;
  `var` 11 with a solid cell above it: animation 3). At the last frame of that animation (event
  flag 1, 15 updates later): `CopyTiles` of the block picture into layer 0, `cam.redraw` = 1,
  state 2 (deleted next update).
  - Source `0x800F6C00` = {6016, 1568, 3, 4}: 3 × 4 cells to (x − 24, y − 32): the block and the
    cell above it (the icicle edge).
  - `var` 11 with a solid cell above it (`TerrainPointCheck(x, y − 26)`): source `0x800F6C08` =
    {6016, 1584, 3, 3} to (x − 24, y − 24): the block without the edge.
  - The source cells (x 6016, y 1568–1632) are a part of the map that the player does not see
    [likely].

Times from the landing: a pit block becomes tiles after about **48 updates** (1 + 32 + 15); a row
block after about **17 updates** (1 + 1 + 15). Then the block is part of the map: the next block
lands on it, and `TileCollisionAt` sees it.

### Row (`var` 1–5, state 3) and row blocks (`var` 10, step2 1)

- Each row block falls **1 px per update** (`speed_y` −1.0) and writes its y into the row before
  it moves. So the row's y follows its blocks.
- The row (state 3), from y ≥ 369: `CollideCheck` with its 240 × 48 px body. On a floor (the shaft
  floor, or the tiles of the row before):
  - sound (2, 17) when drawn; `act4` = 1; no terrain box;
  - a **gap filler** (`var` 11) at (row x + `0x800F6C10`[`var` − 1], row y): the gap closes;
  - at row y 977–1022 or 737–782: one more gap filler at row x − 144; at row y 833–878 or
    689–734: one at row x + 144 [likely: they close side niches in the shaft walls; not checked];
  - state 2 (the row is deleted).
- Each row block then sees `act4` = 1: y to 16n + 8, speeds 0, `Quake2(16, 3, 1)` when drawn (the
  screen shakes; 4 calls in one update), step2 2 (freeze).
- When the row breaks (`act4` = 4): the shaft controller's row count − 1, state 2.

### Gap filler (`var` 11)

Not drawn for one update (step2 4). If the point (x, y) is solid (a tile or a solid object), it is
deleted. Else it blinks once (step2 5), gets its boxes and freezes into tiles (step2 2, 3), with
contact damage **127**. When it appears around the player, `SolidObject` finds the player's centre
in its box and sets the push flags (`mega+0x71` = 15) and `mega+0xE6` = 127: the player dies
(`docs/engine.md`, "Solid objects" and "Squeeze") [likely for the death; the engine rule is
certain]. This is the crush of the shaft: **a player who did not jump up through the gap dies when
the row lands**.

## Ice shaft controller (stage logic type 8, entry `0x800EE208`)

Proposed name "ice shaft controller". One record in the main list (index 0, kind 3, `b` 8, `id` 0,
at (5888, 1360)). States `0x800F6C34`. It does nothing while `mega.special` = 2.

| state | address | what |
|---|---|---|
| 0 | `0x800EDF3C` | wait for the player at x ≥ 5953 and y < 1244 (at the shaft bottom). Then: `CopyTiles(0, 5952, 1264, {6144, 1264, 3, 3})` (closes the entrance in the floor [likely]); `cam.target_max_y` = 1072, `cam.target_min_y` = 320, `cam.redraw` = 1; **delete every stage object of type 7** (the pit generators and blocks); state 1 |
| 1 | `0x800EE258` | 5 rows at x **5880**, y 1032, 888, 744, 600, 456 (144 px apart), each `var` = `GetRNG % 5 + 1`, `+0x7C` = the controller; row count `+0x05` = 5; timer `+0x18` = 0; state 2 |
| 2 | `0x800EE038` | while the row count < **16** and the timer is a multiple of **144**: a row at (5880, **312**), random `var`; count + 1. Timer + 1 each update (not while `mega.special` = 2). Camera field `+0x2C` + 48 once when the current max y < 1073, − 48 once when the player is at x ≥ 5761 and y < 448 [guess: a vertical view offset] |

So the gap of each row is random (`GetRNG % 5`; `var` 1 is a little more likely, 52 in 256
against 51); it does not depend on the player. Rows fall at 1 px per update and come every 144
updates (2.4 s), so they are 144 px apart. Each landed row adds a 48 px floor, so a row lands about
every 96 updates and the floor rises by 48 px each time. 16 rows (if none breaks) stack 768 px,
from the shaft floor (y 1264) to y 496.

## Animations (table `0x800F6B6C`, 8 animations)

| # | what | frames |
|---|---|---|
| 0 | block with the icicle edge on top (sprite 0) | hold |
| 1 | block without the edge (sprite 4) | hold |
| 2 | freeze flash: light blue, dark, light blue (sprites 15, 16, 0) | 8 frames, 16 updates; flag 1 on the last |
| 3 | freeze flash without the edge (sprites 17, 18, 4) | 8 frames, 16 updates; flag 1 on the last |
| 4 | sparkle: a star that grows into a ring and fades (sprites 5–14; 14 is empty) | 10 frames, 13 updates, hold; flag 1 on the last |
| 5–7 | debris: three ice pieces (sprites 1, 2, 3) | hold |

## Sparkle (misc type 9, entry `0x800ED1EC`)

States `0x800F6C1C`. Set-up (`0x800ED238`): copies the block (texture, palette without the flash
bit, animation table, sprite data, flip, layer); draw priority 0; `DisplayCheck(48, 16)`; sound
(2, 21) when drawn; animation 4; counter 60. Main (`0x800ED2F4`): `AnimeTick`; after the last frame
(empty) it waits 60 updates, then state 2 (`DeleteObject3`); also state 2 when it is off screen
(`OffScreenCheck`). No boxes: it cannot hurt.

## Damage

- Taken: `SolidWeaponHit` with `dmg_tbl_capsule`, HP 32, but the code ignores the result: a block
  cannot be destroyed by a weapon [certain for the code]. Whether the hit flash shows is not
  checked (the main clears the flash bit each update).
- Dealt: only by the squeeze (see "The crush test"). The blocks have no attack box, so there is no
  contact damage on a side touch.

## Death

A block breaks only by the crush test: 3 debris (pit block) or 2 debris per block (row), no
drop, no explosion. A broken pit block frees its place in the generator's counts, so the
generator can make one more block.

## Sounds

| bank, id | when |
|---|---|
| 2, 17 | a pit block lands; a row lands (only when drawn) |
| 2, 21 | a sparkle starts (when drawn): each landed block |

## Summary for a reimplementation

- 48 × 48 px solid ice block. Solid for the player and for the Wolfloid. Shots do not hurt it.
- Pit: when the player drops in, lock the camera; drop a block into a random column (skip full
  columns) at once and then every 128 frames (64 on Hard), 2 px/frame. 18 blocks, 3 per column
  (second pit 24 and 4). 50 %: a Wolfloid rides the block (cannon or breath, 50 % each).
- A landed block sparkles, flashes after 32 frames and becomes map tiles 16 frames later.
- Shaft: 5 rows at once, then one every 144 frames, up to 16; each row has 4 blocks and one
  random gap, falls 1 px/frame, lands on the floor or the last row, fills its gap with a block that
  kills a player in it, and shakes the screen.
- A falling block that hits the player's head while the player stands on the floor breaks and
  does 6 (pit) or 16 (row) damage; 127 when the pit block falls in a one-column well.

## Open questions

1. The player side of the crush (the squeeze in `PlayerHitCheck`) is from `docs/engine.md`, not
   seen in play. No save state shows a crush.
2. No save state has the ice shaft (stage logic 8 and the rows). Its geometry (shaft x 5760–6000,
   floor y 1264, entrance at (5952, 1264)) is read from the stage drawing [likely].
3. The meaning of `mega.special` = 2 (both entries do nothing then) and of the camera field
   `+0x2C` that the shaft controller changes.
4. `var` 6–9 and step2 0 have no user in overlay 04 [likely: unused].
5. The extra gap fillers at row x ± 144 (side niches) are not checked on the map.
6. Whether the player can climb out of the first pit without blocks (wall jump) was not tested.
