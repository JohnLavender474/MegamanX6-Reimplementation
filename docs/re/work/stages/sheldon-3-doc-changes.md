# Proposed changes from the third Shield Sheldon rip (2026-10-21)

These are for files that this work stream does not edit. The facts are in
`docs/stages/sheldon.md` ("The texture swap", "The background change", "The door opening in the
live state", "Why X cannot climb back up"). Probes: [`~/mmx-data/x6/stages/probes-sheldon-3/`](probes-sheldon-3/).

## `catalog/helpers.json`

See `~/mmx-data/x6/stages/sheldon-3-helpers-proposed.json` (6 functions, 6 globals).

- Rename `0x8008EAB0` from `zone_flag_eab0` to `stage_texture_swap` (0 none, 1 request, 2 done).
- Change the note of `0x8005335C` (`BackgroundSetting`): it is not the background switch. Its state
  0 (`0x80052F64`) tests the player against a rectangle from `0x8007A154` (per stage, `id & 0x0F`)
  and a per-stage bit in `game+0x9E`, then sets `mega+0x14E` = `id & 0x0F` and plays sound 42 (or
  calls `PlayerEvent(20)`). The name is a guess; a neutral name (`StageLogic14`) is better until the
  purpose is known (maybe a navigator message trigger).

## `docs/engine.md`

Under "Layout", replace "The code that loads it is not known." in the item on two texture blocks
with:

- The stage load keeps entry `0x16` in RAM (the only pointer is scratchpad word `+0x44`); it does
  not keep entry `0x10000`. `StageTextureSwap` (`0x80027A50`, called each frame from the main loop
  at `0x8001226C`) uploads it with `LoadImage` (`0x800663FC`) to the rectangle (320, 256, 512, 256)
  when byte `0x8008EAB0` (`stage_texture_swap`) is 1, then sets the byte to 2. A camera zone with
  `id` bit 7 writes 1 (in stage 7: record 3, `id` 128, trigger x 4352–4383, y 256–335, in laser
  room 3). The layer set-up of the stage start writes 0. Entry `0x16` is row by row because one
  `LoadImage` of 512 × 256 needs that order [certain].

Under "Camera", add:

- **BG layer modes.** Each BG layer runs a mode function by `cam+0x04` (layer 1 table
  `0x80071E88`, layer 2 table `0x80071EB0`; `0x8002935C` and `0x800293CC`). Layer 2 mode 3
  (`0x80028900`) sets x = layer 0 x / 4 + `+0x40` and y = layer 0 y / 4 + `+0x42`; mode 5
  (`0x800299C4`) does nothing (another object sets the position). Layer 1 mode 1 gives x = layer 0
  x / 2, y = layer 0 y / 2 + `+0x42` [certain for the numbers in the Sheldon states].
- **BG effect pool** (`0x800C4778`, 4 slots of `0x30`, type at `+0x01`, type table `0x8007136C`,
  update loop `0x800208F0`). In stage 7 slot 0 is type 1, `SheldonBgSwitch` (`0x800E9E4C`, overlay
  10): when the checkpoint is not 0, layer 1 gets `+0x42` = 320 and layer 2 goes to mode 5 at y 448,
  x = layer 1 x / 4. Stage 22 has no object in the pool.
- **The side-wall test** `0x80030C58` (jump table `0x80010F18`, values 56–63): `obj+0x4A` = 1 (wall
  slide and kick) only for 56 and 58 (and 62, 63 when byte `0x80097187` is 2). 57 and 60 are solid
  with `+0x4A` = 0; 59 and 61 return 0 here.
- `CameraClamp` (`0x8002820C`) clamps the player only in x; in y it sets y = 0 above the map top
  and starts the fall death 8 px below camera max y + 256.

Under "Camera zones": "With `id` bit 7 it sets `0x8008EAB0` = 1" → add "(the texture swap request,
`stage_texture_swap`)".

## `docs/enemies/laser-reflector-puzzle.md`

- Opening table, door 4: the gate on the disc is 5 × 2 cells (59 at x 4688–4767, y 448–479; 57 at
  x 4672 and 4768). The opening copy is 4 × 2, so x 4752–4767 stays 59 (solid) and the hole is 64
  px wide (`sheldon-14`). The same for door 5 (59 at x 3888–3967, y 704–735).
- "Doors 4 and 5 copy tiles again after the pass, probably to close the hole behind the player
  [guess]" → [certain]: in `sheldon-15` all 10 cells of the door 4 gate are collision 56 after the
  pass; the hole is closed and the camera is locked at y 496–496.
- Open question 3 ("Whether the opened doors stay open ..."): the copies are in the screen data in
  RAM (`CopyCell` writes to scratchpad `+0x08` screens). Within one life they stay: in
  `sheldon-14` and `sheldon-15` all earlier doors are still collision 0. After a death: not known.
- Open question 2: the copied cells make the passage collision 0 in all 8 cells (doors 0–3) and in
  4 of 5 columns (doors 4, 5) [certain for the collision].
- Add `sheldon-13_door-open`: door `0x50` in state 3 step 4, camera x 2961 (< 3168), max x 3488;
  the laser is still live at the sensor (3144, 330).

## `docs/enemies/teleport-crystal.md`

Open question 3: `0x8008EAB0` is the texture swap request (`stage_texture_swap`). B2 with `id` 2
writes 1, so a jump inside the area loads the second texture block of the stage when it has one.

## `docs/objects-legend.md`

Kind 3, b 14 is called "background setting". In stage 7 these objects do not change the background
(see the helpers note above). Mark the name as not confirmed.

## `docs/TODO.md`

- [x] Shield Sheldon: the code that copies texture entry `0x16` into VRAM (`StageTextureSwap`,
  trigger camera zone record 3 at x 4352); the camera y limits 496 and 752 (the locks of doors
  `0x54` and `0x55`); the background change at checkpoint 1 (`SheldonBgSwitch`); why X cannot climb
  back up from the first lower room (the door `0x54` hole is closed with 56; section 81).
- Replace the save-state item with: [ ] Shield Sheldon: a state after a death and restart at
  checkpoint 1, 2 or 3 (does the restart load entry `0x10000` again?); the laser puzzle just after
  each of gates `0x51`–`0x53`; a hidden brick block while shown; a floor after a hit by weapon id
  44.
- [ ] Shield Sheldon: `game+0x5E` is 1 in `sheldon-13` to `sheldon-17`; the user called them "no
  armour". Check which is correct (the player sprite member in VRAM would tell).
- [ ] Stage logic 14 (`0x8005335C`): what its state 0 does (`mega+0x14E`, sound 42,
  `PlayerEvent(20)`).
- Collision legend, row 57: add "no wall slide or wall kick (`SideWallTest` sets `obj+0x4A` = 0);
  the Sheldon lower room right wall". Row 56 (and 58): "`obj+0x4A` = 1: wall slide and kick".

## `docs/process/process-log.md`, a new section

```
## 81. Shield Sheldon: the texture swap, the background switch and a closed hole

The user saved five more states in Shield Sheldon's stage: a door just opened (`sheldon-13`), the
corridor before the lower rooms (`sheldon-14`), the first lower room (`sheldon-15`, where they saw
a new background and could not climb back up), the Death Guardian (`sheldon-16`) and the boss room
(`sheldon-17`).

**The texture swap.** A page count gave the first surprise: `sheldon-14`, "before" the change,
already had the second texture block. So the change was earlier, between x 3121 and 4655. The stage
load packs the kept entries one after the other; entry `0x16` was there, but no RAM word pointed to
it. The scratchpad did (word `+0x44`). A search of the code for the rectangle (320, 256, 512, 256)
found it at `0x80071E70`, and its only user was a 10-line function: when byte `0x8008EAB0` is 1,
LoadImage that rectangle from scratchpad `+0x44`, then set the byte to 2. The byte was the "zone
flag" that we had noted twice as open: camera zone `id` 128 writes it. So the zone in laser room 3
(x 4352) is the trigger. The row-by-row storage is now explained too: one LoadImage of 512 × 256
needs it.

**The background.** VRAM hardly changed between 14 and 15. The BG layer cameras did: between 13
and 14 layer 1 moved 320 px down and layer 2 changed its scroll rule. A search for stores to the
layer fields found a small object in a pool that we did not know: it waits for checkpoint 1, then
sets the new BG rule. With the second texture, the BG screens show a hall instead of the sky. In
state 14 the walls cover the view; in state 15 the room is open, so the user saw it there.

**The wall.** The main session's answer was "the opening is too high and the walls are 57". The
live collision showed more: in state 15 the hole is not open at all. Door `0x54` copies solid
cells over it when the camera passes. The other parts of the answer held.

Lessons:

- Check the "before" state first. One page count showed that the change was earlier than the
  user thought.
- A pointer that is not in RAM can be in the scratchpad.
- A rectangle in the data is a short way to the upload code.
- Read the live collision before you explain a "cannot reach" question. The map from the disc
  showed a hole that the game had closed.
```

## Test expectation changes

None. `learn` fit tests on states 13–17 pass with no tool change; no tool code was changed.
