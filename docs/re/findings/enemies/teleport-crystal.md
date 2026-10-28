# Teleporter crystal (stage 03, main object type 28): behaviour sheet

The teleporter crystal is a glowing crystal that moves the player to another part of the stage.
When the player touches it, the player splits into two flickering images, the screen fades to black,
and the player appears at a new place. There is no official name for it in the enemy list; this
sheet uses the descriptive name `teleport-crystal`. The same record type also makes two invisible
variants that trigger when the player walks past an x position.

Overlay `03` (Yammark), save state `yammark-1_start`. Written with the RE kit
(`python3 -m mmxdemake.rekit --name teleport-crystal --state yammark-1_start.state.gz --type 28`)
and `docs/re-guide.md`. Units, confidence tags and the shared rules: `docs/engine.md`. Records:
`docs/stages/yammark.md` (record 34).

**Turtloid.** Overlay `09` has the same object as main type 28 (`--name teleport-crystal-turtloid
--state turtloid-1.state.gz --type 28`, entry `0x800EFFF4`). The code is the same instruction for
instruction, and the data (boxes, animations, destination table) is the same byte for byte
[certain]. Only the addresses differ: code `+0x1130`, data `−0x13B8` (the destination table is at
`0x800F6474` in overlay 09). The blue/green colour is the
palette of sprite bank 7, not the code. The reference project lists the same functions in overlays
04, 05, 06, 08, 10, 11 and 24 too [likely]. So the object is a copy compiled into each stage overlay,
not code in the main program.

**Heatnix.** Overlay `05` has the same code as main type 28 (entry `0x800EE488`; all 846 words
of `0x800EDB90`–`0x800EE8C8` equal overlay 03 after relocation; `heatnix-1_start`) [certain for
the code; the data was not compared]. Record 126 has `id` 1 (sequence A, area change).

## Spawn and set-up [certain]

- HP 32 and contact damage 4 are set, but **no code uses them**: there is no weapon test and no
  contact test. The crystal cannot hurt the player and cannot be destroyed.
- Boxes:
  - attack box `0x800F77B0` = (0, 0, 16, 24): the trigger box, 16 × 24 px, from the origin down. It
    is mirrored by `flip` (standard), so it is on the left of the origin when the crystal faces right;
  - hurt box `0x800F77B4` and terrain box `0x800F77B8` = (−18, −28, 28, 40): not used for anything that
    matters (the crystal does not move).
- Facing: standard "face the player" at spawn (`flip` = `0x40` when the player's x ≥ the crystal's x).
- Draw priority 6; layer = `mega.layer`.
- Visible margin 64 × 64 (`DisplayCheck`) for `id` 0–2. `id` 3 and 4 are never drawn.
- No off-screen test: the object stays until it deletes itself.

## The `id` parameter [certain]

| id | look | trigger | sequence |
|---|---|---|---|
| 0, 1 | crystal | touch | A: area change, request `0x40 \| id` |
| 2 | crystal | touch | B: jump inside the area; destination by character |
| 3 | invisible | player x ≥ crystal x, `game+0x26` = 0, then 90 frames | B: destination entry 2 |
| 4 | invisible | player x ≥ crystal x | B: destination entry 3 |
| ≥ 5 | – | none: step 0 is `HitReactionDispatch`, which runs every update; no effect [likely] | – |

Yammark record 34 has `id` 1, so it uses sequence A.

## State machine

Standard states (table `0x800F77F8`): 0 set-up `0x800EE5CC`, 1 main `0x800EEF10`, 2/3 delete
`0x800EEF8C` (never reached).

Main routine: `AnimeTick`, the step (table `0x800F7808`), then `DisplayCheck(64, 64)` for `id` 0–2.
`CollideCheck` runs after each update.

| step | routine | what |
|---|---|---|
| 0 | engine | hit reaction (not used) |
| 1 | `0x800EEFCC` | wait for touch (`id` 0–2) |
| 2 | `0x800EF0CC` | sequence A (sub-steps, table `0x800F7820`) |
| 3 | `0x800EF108` | sequence B (sub-steps, table `0x800F78BC`) |
| 4 | `0x800EF044` | wait for the x line, then a delay of 90 (`id` 3) |
| 5 | `0x800EF090` | wait for the x line (`id` 4) |

### Trigger (step 1) [certain]

`BoxOverlap(crystal, player)`: the crystal's attack box against the player's hurt box. It fails
while the player's invincibility (`mega+0x61`) is not 0, so a player who flashes after a hit passes
through. The code does **not** test the player's state, the floor, or a button. "Jump into it" is
only because of where the crystals are placed [guess]. The player's x at the trigger goes to
`work.84`.

### Start of a teleport (A0, B0) [certain]

- A0 waits while the player's HP (`mega+0x5C`) is 0. B0 first counts down `work.7C` (90 for `id` 3,
  else 0).
- Sound bank 1, id 1.
- Camera follow off: `+0x44` = 0 in the three camera layers (`0x800971F8`, stride `0x54`).
- Stop flags: `game+0x10`–`+0x17` and `+0x19` (`0x800CCEE0`–`0x800CCEE9`) = 2. `game+0x10` stops the
  player's control [likely]; `game+0x19` stops an object loop [likely].
- Crystal `flags` bit 3 set.
- The player: terrain box (`mega+0x68`) saved in `work.88` and set to 0 (no terrain collision);
  `mega.step` = 2; the engine clears the charge and weapon state (`0x8003F3DC`, `0x8003CDF0`,
  `0x8003CE30`, `0x8003C4EC`) and all player shots (`0x8002B6B0`).

### Split (A1, B1, and during the fades) [certain]

Two x values start at the player's x. Each update one goes +1 px and the other −1 px. The player's
x is set to one value on even frames and the other on odd frames (bit 0 of the frame counter
`0x80097420`). So the player shows as two images that move apart at 1 px/frame each. After
**29 frames**: fade-out at speed 8 (`0x800139F4`).

### Sequence A: area change (`id` 0, 1) [certain for the writes]

- With the fade-out, A1 writes `0x800CCEDF` (`game.stage+3`, `game+0x0F`) = `id | 0x40`.
- The player code sees a non-zero `0x800CCEDF` and, for bit `0x40`, puts the player in state 3,
  step 0 [certain for the code].
- A2 keeps the split going until the fade-out ends. Then it puts back the player's x (`work.84`),
  the terrain box, the camera follow, and clears the stop flags. It stays in A2.
- Which code then loads the new area, and where the player appears, was not found [open]. The crystal
  does not change the camera or `game.stage`/area itself.

### Sequence B: jump inside the area (`id` 2–4) [certain]

B2 runs when the fade-out ends:

| id | entry | checkpoint (`game+0x1D`) | other |
|---|---|---|---|
| 2, X | 0 | 1 | `0x8008EAB0` = 1 |
| 2, Zero | 1 | 2 | `0x8008EAB0` = 1 |
| 3 | 2 | 3 | |
| 4 | 3 | 3 | |

Destination table `0x800F782C` (32 bytes per entry; stage pixels):

| entry | player (x, y) | camera layer 0 | layer 1 | layer 2 | layer-0 limits x | y |
|---|---|---|---|---|---|---|
| 0 | 72, 1115 | 0, 960 | 1664, 0 | 0, 240 | 0–368 | 960–960 |
| 1 | 120, 2074 | 0, 1904 | 0, 1904 | 0, 476 | 0–6080 | 1904–1904 |
| 2 | 1952, 1115 | 1920, 960 | 1664, 0 | 480, 240 | 1920–2608 | 960–960 |
| 3 | 1952, 1114 | 1920, 960 | 1664, 0 | 480, 240 | 1920–2608 | 960–960 |

Then B2:

- sets the player's x, y (Zero: y − 1), `prev_x`/`prev_y`; clears `mega+0x7A` (no_hit), `+0x67`
  (latch), `+0x61` (invincibility), `+0xA4`, `+0xE0`, `+0xE4`, and `game+0x1C`;
- sets the three camera positions (`+0x0A`, `+0x0E` of each layer) and the layer-0 limits:
  x min `+0x1E`/`+0x26`, x max `+0x1C`/`+0x24`, y min `+0x22`/`+0x2A`, y max `+0x20`/`+0x28`.
  The y limits are equal, so the camera cannot scroll up or down there;
- requests a background redraw: tile cache reset of layer 0 (`0x8002B36C`), `0x800C4560` = 1,
  camera `+0x4C` = 1 in the three layers;
- clears the player shots again;
- spawns the records in the new view: `0x80029F38(cx − 48, cx + 368, cy − 48, cy + 288)`. Old
  objects are not removed; they leave by their own off-screen tests [likely];
- fade-in at speed 8 (`0x800139A0`); sound bank 1, id 1.

B3: the two images start at ±64 px and come together at 1 px/frame until the fade-in ends. B4: 29
more frames, then the player's x snaps to the table x, the terrain box and the camera follow come
back, the player gets a normal step (`0x8003AD18`), and the stop flags are cleared. B5:
`DeleteObject3`.

## Use more than once [likely]

- Sequence B ends with `DeleteObject3`, which leaves the record's "spawned" bit set. So the record
  does not make a new crystal until the record list is reset (death, a new area).
- Sequence A stays in A2. The area change replaces it.

## Animations

Table `0x800F77EC`.

| # | sprites | ticks | loop to | use |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 | 3 each | 0 | the crystal (the only one started) |
| 1 | 6 7 8 9 10 11 | 3 each | 0 | not started by this code |

No event flags.

## Sounds

Bank 1, id 1: at the start (A0/B0) and at the arrival (B2). Probably the teleport sound [guess].

## Summary for a reimplementation

1. A static, indestructible crystal; animation 0 loops (6 frames × 3 updates). No damage.
2. Trigger: its 16 × 24 box touches the player and the player is not invincible.
3. Freeze the player and the objects, turn off the player's terrain collision and the camera follow,
   clear the player's shots. Sound.
4. 29 frames of "split": the player flickers between two x values that move apart 1 px/frame.
5. Fade out (about 32 frames [guess]).
6. `id` 0/1: go to another stage area (`0x40 | id`). `id` 2–4: move the player and the camera to the
   table entry, set the camera limits and the checkpoint, spawn the new view's objects, fade in.
7. The images come back together from ±64 px; 29 frames after the fade-in, unfreeze.
8. The crystal is used once per record spawn.

## Open questions

1. Which code reads `0x800CCEDF` = `0x40 | id` and loads the area, and whether `id` is the area
   number. So the destination of Yammark record 34 is not known.
2. (answered) `id` 2–4 are used in stage 18 (section 2 of Gate stage 2, overlay 24, records
   57–59; `docs/stages/gate-2b.md`). Overlay 24 has the same destination table at `0x800F6E80`,
   and its four entries are the places of that stage: `id` 2 at the end of the top corridor sends
   X to checkpoint 1 and Zero to checkpoint 2, `id` 3 and `id` 4 end those paths at checkpoint 3
   [certain for the table and the records].
3. (`0x8008EAB0` is `stage_texture_swap`: B2 with `id` 2 writes 1, so a jump inside the area loads
   the second texture block of the stage when it has one; `docs/engine.md`, "The texture swap".)
   The meaning of `game+0x26` (tested by `id` 3; in stage 4 the Ride Armor writes it), `game+0x1C`, `mega+0xA4`,
   `+0xE0`, `+0xE4`, and camera `+0x4C`.
4. The fade length at speed 8, and which sound bank 1 id 1 is.
5. What the player's step 2 and state 3 look like during the teleport.
