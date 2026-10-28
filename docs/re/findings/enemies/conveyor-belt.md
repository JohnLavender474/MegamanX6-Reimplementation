# Conveyor belt (stage 4 area 1, stage logic type 9): behaviour sheet

The conveyor belts of section 2 of Metal Shark Player's stage (the Recycle Lab) carry the player to
the left. They are layer-0 tiles with collision value 58 and one stage logic object that moves the
player and animates the tiles. The belt has no official name in the enemy list; this sheet uses the
descriptive name `conveyor-belt`.

Overlay `07` (stage 4, area 1), loaded at `0x800E9860`. Save states `shark-2_section-2`,
`shark-3_press-low`, `shark-4_mini-boss` and `shark-5_after-mini-boss` (X form 1, difficulty 1). The
kit has no class for stage logic; the code was read from the state RAM. Notes, probes, pseudo code
and C: [`~/mmx-data/x6/re/shark-conveyor/`](../../work/shark-conveyor/). Units, tags and the shared rules: `docs/engine.md`. The
press over the belt: `docs/enemies/nightmare-press.md`.

The belt is three parts [certain]:

- the **tiles**: collision value 58 in layer 0 (the floor surface), with an upper row of art above it;
- the **belt logic**: stage logic type 9, `BeltUpdate` (`0x800ED998`). It moves the player and
  animates the tiles;
- other **riders**: the junk block (stage object type 11) has its own belt test.

Kind-7 object type 3 (`AutoScroll`, `0x800E9C98`) is not the belt. It is the forced camera scroll of
the checkpoint-1 room (the mini-boss room), which also has a belt floor. It is in this sheet because
the two act together.

## Spawn and set-up

- Record: `fixed_set_tbl` of stage 4 area 1 (`0x800F7E58`), record 0: kind 3 (stage logic), type 9,
  `id` 0, at (0, 0). It is made when the area loads [certain].
- No sprite, no box, no HP. It cannot be hit or destroyed.
- State 0 (once): `belt_frame` (`0x800F985C`) = `belt_turns` (`0x800E3142`, main RAM; it keeps its
  value when the area loads again [likely]), `BeltSaveFrames`, `BeltDrawRollers` (see "The art"),
  state 1.

| field | meaning |
|---|---|
| `+0x04` | state: 0 set-up, 1 run |
| `+0x14` | 1 = the belt sound is on |

## Where the belt is [certain for the cells]

Layer 0 of area 1 is 26 × 5 screens (6656 × 1280 px). The cells with value 58:

| y (px) | x (px) | part |
|---|---|---|
| 416–431 | 0–319, 384–479, 544–783, 1136–1599, 1792–2015, 2080–2207, 2272–2351, 2528–2671, 2768–3007, 3952–4047, 4096–5439 | the upper path (checkpoint 0), under the Nightmare Press |
| 1136–1151 | 0–6399 | the floor of the mini-boss room (checkpoint 1) |
| 560–847 (small blocks) | 32–159, 672–1087, 2000–2127 | the frame store of the belt art; outside the camera limits |

The belt is two rows of 16 px cells: tiles 56 and 57 at y 400 (collision 0, the upper part of the
art) and tiles 58 and 59 at y 416 (collision 58, the surface). The base row at y 432 has tiles 60–63.
Between the belt runs, the floor is normal (value 56, steps at y 320–384). The side-wall test treats 58
as a wall that allows the wall slide (`docs/engine.md`, "The side-wall test").

## The update (`BeltUpdate`, `0x800ED998`) [certain for the code]

It runs in `StageLogicLoop`, after the player step and before `CollideCheck(mega)`. In order:

1. Return when `mega+0xE0` = 2, the player state (`mega+0x04`) ≠ 1, or HP = 0.
2. Player step 20 (event hold): when the sound is on, `PlaySound(5, 3)` (the sound stops) and
   `+0x14` = 0. Return.
3. Player step 0 (beam-in): return. **This is the start rule**: the belt does not move, animate or
   play its sound until X leaves the beam-in step (for X, 87 updates after the landing; see
   `docs/players/ladder-hurt-events.md`, "Beam-in").
4. Checkpoint 0: camera min x and target min x = camera x. So on the upper path the camera cannot go
   back to the left.
5. No player terrain box (`mega+0x68` = 0, for example while the player rides): return.
6. Checkpoint ≥ 2: `PlaySound(5, 3)`, delete. **This is the end rule** (after the mini boss).
7. When `+0x14` = 0 or `mega+0x14C` ≠ 0: `PlaySound(5, 6)` (the belt sound), `+0x14` = 1.
8. `BeltRotateTiles`; when `frame_count` % 5 = 0, `belt_frame` += 1 and `BeltDrawRollers`.
9. **The push**: when `TileCollisionAt(mega, x, y + box oy + box h)` = 58 (the cell under the bottom
   of the terrain box), `mega.x` −= 1. Only the whole-pixel half is written; there is no wall test.

So:

- **Speed and direction**: 1 px per update (60 px per second), always to the left. It never changes
  speed or direction. All belts in the area move together.
- **Who moves**: only the player, and only while the bottom of the terrain box is in a value-58 cell
  (on the belt; not in the air). A wall is handled by `CollideCheck(mega)` later in the frame
  [likely].
- While the player rides (no terrain box) nothing happens, also no animation. Area 1 has no Ride
  Armor record [certain for the records].
- X walking right on the belt moves at 2.0 − 1 = 1.0 px per update; walking left at 3.0; standing at
  1.0 to the left. A dash adds its speed in the same way [likely].

## Other riders [certain for the code]

- **Junk block** (stage object type 11, run state `0x800EE790`, the area ≠ 0 part): when its left
  side is free (no block from `0x800EE64C` and no tile 1 px left of its box) and the cell under its
  box is 58: x −= 1. With `var` bit 7 (records 3–5 have `id` 128) it first waits until the player
  x ≥ 1088. It does not depend on `BeltUpdate`. In `shark-4` the blocks of records 3 and 4 had moved
  from x 1320 and 1368 to 1176 and 1224: they stop at the step at x 1152.
- **Stage object type 15** (`0x800EEED8`, the Nightmare Effect object of area 1): the same test, but
  only when the stage number ≥ 19. In stage 4 it does not ride the belt.
- **Enemies**: no other code in overlay 07 compares a tile with 58, and the main program has no such
  tile test. Enemies are not carried.

## The art [certain for the code; the frame order is likely]

The belt has no sprites. It is layer-0 tiles in two parts, both changed every 5 updates (8 frames, a
cycle of 40 updates):

- **The surface** (`BeltRotateTiles`, `0x800ED764`): the frame store has 8 blocks of 2 × 2 cells at
  x 32, 64, 96, 128 and y 560, 592 (table `0x800F8C08`). `BeltSaveFrames` (`0x800ED4D0`) reads their
  tile ids into `0x800F981C`; block 0 is tiles 56, 57, 58, 59 (the tiles of the visible belt), blocks
  1–7 are tiles 208–235. When `frame_count` (`0x80097420`) % 5 = 0, the 4-byte tile definitions
  (scratchpad `+0x0C`) of the 8 blocks turn by one place (block i gets the definition of block i + 1)
  and `belt_turns` += 1. So every belt cell on the map shows the next frame at once. The collision
  byte is the same in the 8 blocks (0 for the upper row, 58 for the lower row), so the collision does
  not change.
- **The ends** (`BeltDrawRollers`, `0x800ED8AC`): 20 places at y 400, 1 × 2 or 2 × 2 cells (table
  `0x800F88E8`: dx, dy, w, h). For each, `CopyTiles(0, dx, dy, src)` with the source cell of frame
  `belt_frame & 7` from `0x800F8988` + i × 32 + frame × 4 (the frame store at x 672–1087 and
  2000–2127, y 544–847). Then layer 0 is drawn again (`+0x4C` = 1).

For a conversion: an animated tile set of 8 frames at 12 frames per second, and a push of 60 px per
second to the left on the surface tiles.

## Sounds

| bank, id | when |
|---|---|
| 5, 6 | the belt starts (first update after the beam-in), and again when `mega+0x14C` ≠ 0 |
| 5, 3 | the belt stops: player step 20, and the delete at checkpoint 2 |

Bank 5 is not described yet; "start" and "stop" come from the code [likely].

## The forced scroll (kind-7 type 3, `AutoScroll`, `0x800E9C98`) [certain for the code]

Record 1 of the same list (kind 7, type 3, `id` 0). Nothing while `mega+0xCC` ≠ 0 or `mega+0xE0` = 2.

| state | does | next |
|---|---|---|
| 0 | waits for checkpoint 1 | layer 1 mode 5, target x (`+0x14`) = camera max x; state 1 |
| 1 | waits for the player in step 2 (stand) | walk speed (`mega+0xF4`) += 0.5 (2.0 → 2.5); state 2 |
| 2 | while camera x < target: camera x += 0.5, and min x = max x = camera x (current and target) | `game+0x26` ≠ 0: walk speed −= 0.5, delete |

Stage logic 10 (`0x800EDD00`) sets checkpoint 1 when it moves the scene to the mini-boss room (camera
x 0, y 944, target x 6080). In `shark-4`: target 6080, camera x 10, walk speed 2.5. In `shark-5`:
`game+0x26` = 1, walk speed 2.0, the object is gone.

In that room the floor is belt for x 0–6399. X standing goes left at 1 px per update until the left
edge of the view (camera x + 8, `CameraClamp`), which moves right at 0.5 px per update. X walking
right goes at 2.5 − 1 = 1.5 px per update.

## Why the belt is gone in `shark-5`

`shark-5` has checkpoint 2. `BeltUpdate` deletes itself at checkpoint ≥ 2 (step 6), so the push, the
animation and the sound end. The value-58 cells stay in the map, but nothing reads them. What sets
checkpoint 2 after the mini boss is not read here.

## States

| state | checkpoint | X (x, y), step | belt | forced scroll |
|---|---|---|---|---|
| `shark-2_section-2` | 0 | (66, 395), 2 | live, sound on | state 0 |
| `shark-3_press-low` | 0 | (1044, 347), 2 | live, sound on | state 0 |
| `shark-4_mini-boss` | 1 | (137, 1115), 2 | live, sound on | state 2 |
| `shark-5_after-mini-boss` | 2 | (5604, 395), 2 | gone | gone |

## Stage 20 area 1 (overlay 28) [certain for the code]

The hidden area of the stage ([`docs/stages/shark-alt.md`](../stages/shark-alt.md), save state
`shark-10_alt`) has a belt and a forced scroll of its own. Both are variants of the code above.

**The belt** (stage logic 15, `BeltUpdateAlt`, `0x800F0984`; record 25 of `fixed_set_tbl`
`0x800F2598`). Cells 58 at y 416, x 96–2751 (one run); the frame store is at x 96–223, y 512–575
(table `0x800F3524`, tile ids saved to `0x800F35AC` by `0x800F04C0`); one end roller (table
`0x800F34FC`: x 2736, y 400, 1 × 2 cells; sources `0x800F3504`, x 2736–2863, y 560). The tile turn
(`0x800F0754`) and the roller draw (`0x800F089C`, frame `0x800F35EC`) are as in section 2. The
differences from `BeltUpdate`:

1. Only `mega+0xE0` = 2 returns at the top. There is no test of the beam-in step (0) and no camera
   lock (the forced scroll locks the camera).
2. **End rule:** camera x ≥ 2752 → `PlaySound(5, 3)`, delete (not the checkpoint). The camera max x
   is 2576 before boss gate 37, so the belt ends when gate 37 moves the camera to x 2848 [likely].
3. The push needs player step ≠ 20, state 1 and HP ≠ 0, then the same foot-cell test (X x −= 1).
4. **The Ride Armor rides the belt:** when `actor2` has a terrain box (`+0x68` ≠ 0), no left-wall
   flag (`+0x70` & 2 = 0) and its foot cell is 58, `actor2.x` −= 1. In section 2 no `actor2`
   exists.

**The forced scroll** (kind-7 type 4, `AutoScrollAlt`, `0x800E9864`, states `0x800F2678`; record
26). Nothing while `mega+0xCC` ≠ 0 or `mega+0xE0` = 2.

| state | does | next |
|---|---|---|
| 0 | checkpoint 1: delete | target (`+0x14`) = camera max x (2576); state 1 |
| 1 | waits for X's x ≥ 128 (not for a step) | walk speed `mega+0xF4` += 0.5 (2.0 → 2.5); state 2 |
| 2 | while camera x < target: camera x += 0.5, min x = max x = camera x (current and target) | camera x ≥ target: walk speed −= 0.5, delete |

So the scroll takes 2576 / 0.5 = 5 152 updates (86 s), and it ends by itself at the target (in
section 2, `game+0x26` ends it). In `shark-10_alt` it waits in state 1 (X at x 79, target 2576).

Other riders in overlay 28: the junk block (records 25, 26, `id` 128) has the overlay 06 code with
the belt test, and the Nightmare Cube (stage object 15) uses its stage ≥ 19 belt test here. The
Mettaurs D2 on the belt are not carried.

## Summary for a reimplementation

1. Mark the belt surface tiles. Each update, when the player stands on one, move the player 1 px
   left. Do nothing while the player is in the spawn (beam-in) or in a scripted pause.
2. Animate all belt tiles together: 8 frames, a new frame every 5 updates.
3. Junk blocks on a belt also move 1 px left per update until something blocks their left side.
4. On the upper path, lock the camera min x to the camera x (no scroll back).
5. The belt ends at checkpoint 2 (after the mini boss).
6. Mini-boss room: forced scroll right at 0.5 px per update and player walk speed 2.5 until the
   room flag (`game+0x26`) is set.

## Open questions

- `mega+0x14C`: the player code writes 2 at `0x80034D30` and 0 at `0x80034DA4` (before a `FadeIn`).
  Probably "back from the pause menu", so the belt sound starts again [guess].
- The direction in which the art seems to move: check in play.
- What sets checkpoint 2 after the mini boss.
- Stage logic 7 (`0x800F0530`) reads the foot cell too (values 0, 16–28 and 60); not read.
