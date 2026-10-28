# Proposed changes from the Shield Sheldon stage rip (2026-10-21)

These are for files that this work stream does not edit. The facts are in
`docs/stages/sheldon.md`.

## `docs/TODO.md`, collision legend

Add rows:

| Value | Cells (Sheldon) | Reading so far | Confirmed by |
|---|---|---|---|
| 60 | 24 | solid floor, used only under the hidden glass floors (main type 32) of Shield Sheldon's stage; X stands on it at the same height as on 56 | save state (`sheldon-6_tile-shown`: step 2, air latch 0, `mega+0x72` bit `0x08` clear) |
| 59 | 68 (Sheldon) | also the laser gates of Shield Sheldon's stage: 2 × 4 or 4 × 2 cells, replaced by empty cells (0) when the gate opens | code (`0x800EA650`, `CopyTiles`) |

Note for value 59: `TileAt` reads 59 as 56 (`catalog/helpers.json`), so 59 is solid until a gate
copies empty cells over it.

## `docs/TODO.md`, other items

- [x] Shield Sheldon stage map and records (`learn`, `stages`); `docs/stages/sheldon.md`.
- [ ] Shield Sheldon: save states of the boss room, the lower rooms (y 512–1023), the hidden area
  behind the teleporter crystal (record 110), and each laser gate after it opens.
- [ ] Weapon id 44: the only weapon that shows a hidden glass floor (damage table `0x80074A1C`).
- [ ] What `0x8008EAB0` does (camera zone with `id` bit 7 sets it to 1).
- [ ] Stage logic 7 (`0x800F0BB0`, overlay 10) and 27 (`0x800541D4`), stage object 15 (overlay 10,
  `0x800F3B98`), the kind-7 record of the second list.
- [ ] `spriteexport`: an object can store its state table in `+0x30` (the animation table field;
  Sheldon main type 32). The exporter then reads the state table as animations and skips the type.
  Skip a table whose words are code addresses, and draw the sprite in `+0x47` instead.
- [ ] Re-learn the older catalog entries once, so that `edge.v` and `edge.v_joins` use the new
  vertical score (joins with a uniform edge row are not scored). The old entries keep the old
  numbers; `learn` and `check_layout` pass for all of them with the new score.

## `docs/engine.md`

Under "Camera", add:

- **Camera zones** (stage logic 0, `CameraZone`, `0x80052258`, states `0x800791E8`: set-up
  `0x80052294`, update `0x8005211C`) [certain for the code]. The set-up takes a 16-byte entry from
  `camera_zone_tbl` (`0x80079D70`, word index stage × 2 + area) at `(id & 0x7F) × 16`:
  `u16 x0, x1, y0, y1` (a rectangle that the player position must be in), then
  `u16 min_x, max_x, min_y, max_y` (`0xFFFF` = keep). While the player is in the rectangle, the
  zone writes the target limits (`cam+0x26`, `+0x24`, `+0x2A`, `+0x28`) and sets `cam+0x47` = 2.
  With `id` bit 7 it sets `0x8008EAB0` = 1 and deletes itself with `DeleteObject3` (the record
  stays "spawned", so it does not come back). Off screen (`OffScreenCheck`, 64 px) it deletes
  itself with `DeleteObject`. Table pointers seen: stage 0 `0x800791F0`, 1 `0x800792C0`, 2
  `0x80079450`, 3 `0x80079540`, 4 `0x800795E0` / `0x80079690` (areas 0 / 1), 5 `0x800796D0`, 6
  `0x800797A0`, 7 `0x800798C0`, 12 `0x80079A20`.
- In Shield Sheldon's stage the laser gates (main 13, `id` `0x50`–`0x55`) also set target limits
  when they open (`0x800EA650`).

Under "The player", add:

- The player position is `mega+0x1A` (x) and `mega+0x1E` (y), as for the other objects.
- Form `mega+0xE7` / `game+0x5E` = 0 is X with no armour (`sheldon-4` to `sheldon-7`).

Under "Layout", add:

- Shield Sheldon (member 102): row 0 of layer 0 holds tile templates (empty cells that the laser
  gates copy); the camera never shows it.

## `docs/process/process-log.md`

Suggested section "The Shield Sheldon stage map":

The user saved seven states in Shield Sheldon's stage (stage 7, area 0). `learn` refused the map:
the vertical edge score was 0.282, under the threshold of 0.70. We did not trust or distrust the
score; we tested the map with a fact from outside the tool: X stands on solid floor. That held, so
the address was right and the test was wrong for this stage. Many floors here end exactly at a
screen edge, so a full solid edge row faces an empty one, which is correct. The edge test now skips
a vertical join when one of its two edge rows is uniform, and still needs three scored joins. The
wrong-member case of section 68 (member 101 against the alternate Turtloid layout) still fails: it
has only one scored join.

Two new states asked about hidden tiles. A full diff of RAM and VRAM between "hidden" and "shown"
found no tile, layout or collision change, and only colour cycling in the palettes. The live main
objects showed it: type 32 objects sit on the floor at the hidden cells, and the one under X had its
`display` byte set and a timer of 130. The collision (new value 60) is always in the map; the
object only draws the floor for 150 updates after a touch. The note from the coordinator named type
32 as a wasp; the state data showed that it is the floor.

A third new state (the laser puzzle) had wider camera limits than the start. No camera zone record
was near, so we searched the overlay for the new limit value (3488) and found the gate routine. It
sets one limit and copies empty cells over the gate per gate `id`. The six gate limits line up: each
one opens the view exactly to the next gate.

Lessons:

- A threshold that worked on four maps is a guess for the fifth. Check the map with an outside fact
  before you change the threshold, then change the test, not the threshold.
- Diff two states that differ in one visible thing. When nothing in the map changes, look at the
  live objects next to the change.
- To find who sets a value, search the code for the value itself.

## `catalog/helpers.json`

Proposed names:

| address | name | note |
|---|---|---|
| `0x80052258` | `CameraZone` | stage logic 0: camera limits from `camera_zone_tbl` when the player is in a rectangle |
| `0x8005211C` | `CameraZoneUpdate` | the rectangle test and the limit writes |
| `0x80079D70` | `camera_zone_tbl` | word per stage × 2 + area: 16-byte entries {x0, x1, y0, y1, min_x, max_x, min_y, max_y} |
| `0x800F3E2C` | `HiddenFloor` | Sheldon main 32 (overlay 10): draws a glass floor or brick block for 150 updates after a touch or weapon id 44 |
| `0x800EA650` | `LaserGateOpen` | Sheldon main 13 state 3 (overlay 10): per `id & 0x0F` one camera limit and a `CopyTiles` |
