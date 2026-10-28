# Proposed doc changes (conveyor belt agent)

I did not edit these files. The main session or the owner agent should apply them.

## `docs/engine.md`

### "Game state" table, `+0x26`

Add: Stage 4 area 1: not 0 ends the forced scroll of the mini-boss room (`AutoScroll`, kind-7 type 3
of overlay 07): the walk speed goes back to 2.0 and the object is deleted. Set after the mini boss
(`shark-5`) [certain for the read; the writer is not read].

### Object classes, the kind-7 row

Change "kind-7 objects (stage 7 background switch, stage 4 Nightmare Press)" to "kind-7 objects
(stage 7 background switch, stage 4 Nightmare Press, stage 4 area 1 forced scroll)".

### The player (fields)

- `mega+0xF4`: walk and air speed, 2.0. Stage 4 area 1 adds 0.5 while the forced scroll runs
  (`docs/enemies/conveyor-belt.md`).
- `mega+0x14C` (halfword): the player code writes 2 at `0x80034D30` (when `0x800E4430` ≠ `mega+0x93`)
  and 0 at `0x80034DA4` (before `FadeIn`). The belt restarts its sound when it is not 0 [guess:
  "back from the pause menu"].

### Camera

Add after the camera zone rules: stage logic 9 of stage 4 area 1 (the conveyor belt) sets min x and
target min x = camera x in each update at checkpoint 0 (one-way scroll). Kind-7 type 3 of the same
area scrolls the camera 0.5 px per update to the right and holds all four x limits at the camera x.

### Animated tiles (new)

Stage 4 area 1 animates tiles in two ways (`docs/enemies/conveyor-belt.md`, "The art"): it turns the
4-byte tile definitions (scratchpad `+0x0C`) of 8 tile blocks by one place every 5 updates, and it
copies cells with `CopyTiles` from a frame store in the map. Byte 0 of a definition is the collision
value (`TileCollisionAt` reads it).

## `docs/stages/shark.md` (stage agent)

Area 1 (overlay 07) facts from this work [certain]:

- Layer 0: 26 × 5 screens (6656 × 1280 px).
- Second list (`0x800F7E58`, 47 records + end): 0 logic 9 (conveyor belt), 1 kind-7 type 3 (forced
  scroll), 2 logic 10 (scene move to the mini-boss room), 3–5 stage 11 `id` 128 (junk blocks at x
  1320, 1368, 1416, y 392), 6 kind-7 type 2 `id` 16 (Nightmare Press), 7–25 stage 9 `var` 0, 26–42
  stage 9 `var` 66–70 and 99, 43–46 stage 9 `var` 112, 47 logic 7.
- Overlay 07 entries: main 16 `0x800EB4C8`, 27 `0x800F1958`, 35 `0x800F04A4`, 56 `0x800F4230`;
  visual 13 `0x800EB504`, 22 `0x800F6FA0`, 32 `0x800F1DA4`; misc 1 `0x800EA02C`, 29 `0x800E9E98`, 56
  `0x800ECBAC`; stage 9 `0x800EE3F8`, 10 `0x800ED494`, 11 `0x800EEA90`, 15 `0x800F00B4`; logic 1
  `0x800E9AE0`, 7 `0x800F0530`, 9 `0x800ED998`, 10 `0x800EDD00`, 29 `0x800EA2EC`; kind-7 0
  `0x800E9B90`, 1 `0x800E9E4C`, 3 `0x800E9C98`, 4 `0x800E9864`.
- Collision value 58 = the conveyor belt surface (cells in `docs/enemies/conveyor-belt.md`). The belt
  frame store is at y 544–847, x 32–159, 672–1087, 2000–2127 (not in the camera view).
- Logic 10 (`0x800EDD00`): at camera x ≥ 4672, X x = 4832 and `PlayerEvent(20)`; then the scene moves
  by −4672 in x to y 944, camera limits x 0–6080, y 944, checkpoint 1, layer 1 mode 5,
  `stage_texture_swap` = 1, `SpawnInRect`. In `shark-2` the camera max x is 4672 (the end of the
  upper path).
- Checkpoint 0 camera limits in `shark-2`: x 0–4672, y 224.

## `docs/enemies/junkroid.md` or a new junk block sheet

Stage object 11 (junk block) in overlay 07 rides the belt: 1 px per update to the left on value 58
when its left side is free; `var` bit 7 makes it wait for player x ≥ 1088 (`docs/enemies/conveyor-belt.md`,
"Other riders").
