# Conveyor belt and the section-2 Nightmare Press (stage 4 area 1, overlay 07): notes

## Method

- Save states (`~/mmx-data/x6/dumps/`): `shark-2_section-2`, `shark-3_press-low`, `shark-4_mini-boss`,
  `shark-5_after-mini-boss` (stage 4, area 1, X form 1, difficulty 1). `shark-1_ride` (area 0) for
  comparison.
- Overlay 07 is in RAM at `0x800E9860`: the first 58 232 bytes are the same as `rock_bin/07.bin`
  (the rest is data that the game changes).
- Disassembly from the state RAM: `probes/ovl7.txt` (`0x800E9860`–`0x800F7C00`), `probes/main.txt`
  (main program), `probes/press.txt` (`0x80046E14`–`0x800477A0`, from `shark-3`).
- `rekit` for stage type 9 of overlay 07: `python3 -m mmxdemake.rekit --state shark-2_section-2.state.gz
  --type 9 --class stage --name shark-conveyor --out ~/mmx-data/x6/re/shark-conveyor/kit-type9`
  (the kit has no class for stage logic or kind-7 objects, so these were read from `ovl7.txt`).
- Probes (`probes/`, run from that folder with `python3 <probe>.py <state>`): `common.py` (state
  loader), `slots.py` (player, layer cameras, kind-7 slots), `live2.py` (stage objects and stage
  logic, with type-table entries), `k7tab.py` (kind-7 table), `belt.py` (collision value 58 cells,
  belt frame and roller tables), `tiles.py` (tile ids and tile definitions), `vals.py` (walk speed,
  `mega+0x14C`, camera limits), `press3.py` (press, X box, type 9 bodies), `floor2.py` (floor profile
  of section 2 and body bottoms), `recs.py` (record list), `types.py` (overlay entries of all type
  tables), `lst.py` (linear disassembly).

## Which object is the belt

The belt is **stage logic type 9** of overlay 07 (`BeltUpdate`, `0x800ED998`), not kind-7 type 3
[certain]:

- Kind-7 type 3 (`0x800E9C98`) is the forced camera scroll of the checkpoint-1 room (the mini-boss
  room). It waits for checkpoint 1. In `shark-2` and `shark-3` (checkpoint 0) it is still in state 0
  and does nothing.
- Stage logic 9 reads the collision value under X's feet and moves X 1 px left on value 58. It also
  animates the belt tiles.
- Stage logic 7 (`0x800F0530`) reads the foot tile too, but its jump table (`0x800E98B0`, 61 entries)
  sends value 58 to the default case. It handles values 0, 16–28 and 60 (slopes or other floors;
  not read further).

Records of area 1, second list (`fixed_set_tbl`, `0x800F7E58`, 47 records + end):

| record | kind, type | `id` | object |
|---|---|---|---|
| 0 | logic 9 | 0 | the belt (`BeltUpdate`) |
| 1 | kind-7, 3 | 0 | the forced scroll (`AutoScroll`) |
| 2 | logic 10 | 0 | the scene move to the mini-boss room (`0x800EDD00`) |
| 3–5 | stage 11 | 128 | junk blocks at x 1320, 1368, 1416, y 392 |
| 6 | kind-7, 2 | 16 | the Nightmare Press |
| 7–25 | stage 9 | 0 | press bodies, x −120 + 240 n (n 0–18), y 176 |
| 26–42 | stage 9 | 66–70, 99 | press "teeth" (lower bodies), y 256–288 |
| 43–46 | stage 9 | 112 | press bodies with a contact attack |
| 47 | logic 7 | 0 | foot-tile logic (not the belt) |

## Live values

| state | checkpoint | X (x, y) | X step | camera x, y | belt (logic 9) | `+0x14` (sound on) | kind-7 type 3 | X walk speed `mega+0xF4` |
|---|---|---|---|---|---|---|---|---|
| `shark-2` | 0 | 66, 395 | 2 | 0, 224 | live | 1 | state 0 | 2.0 |
| `shark-3` | 0 | 1044, 347 | 2 | 885, 226 | live | 1 | state 0 | 2.0 |
| `shark-4` | 1 | 137, 1115 | 2 | 10, 944 | live | 1 | state 2, target x 6080 | 2.5 |
| `shark-5` | 2 | 5604, 395 | 2 | 5456, 224 | **gone** | — | gone | 2.0 |

- `shark-2`: X is at x 66; the user saw the spawn at x 67. X stands on a belt cell (feet y 416). The
  1 px is one belt update [likely].
- `shark-4`: the junk blocks are at x 1176 and 1224 (they were at 1320 and 1368): they rode the belt
  to the wall at x 1152 (the third block is gone) [certain for the positions].
- `game+0x26` = 1 only in `shark-5`; this ends the forced scroll and gives back the walk speed.
- `mega+0x14C` = 0 in all states.

## The belt cells (collision value 58)

Layer 0 is 26 × 5 screens (6656 × 1280 px). Value-58 cells from `belt.py` [certain]:

| y (px) | x (px) | what |
|---|---|---|
| 416–431 | 0–319, 384–479, 544–783, 1136–1599, 1792–2015, 2080–2207, 2272–2351, 2528–2671, 2768–3007, 3952–4047, 4096–5439 | the belt surface of the upper path (checkpoint 0) |
| 1136–1151 | 0–6399 | the floor of the mini-boss room (checkpoint 1) |
| 560–847 (blocks) | 32–159, 672–1087, 2000–2127 | the frame store of the belt art, outside the camera limits |

The belt is two cells high: row y 400 (tiles 56 and 57, collision 0) is the upper part of the art and
row y 416 (tiles 58 and 59, collision 58) is the surface. Row y 432 (tiles 60–63) is the base. Between
the belt runs, the floor is normal (value 56) at y 320–384.

## Movement [certain for the code]

`BeltUpdate` (each update, in `StageLogicLoop`, after the player step and before `CollideCheck(mega)`):

- returns when `mega+0xE0` = 2, player state ≠ 1, HP = 0;
- step 20 (event hold): stops the belt sound (`PlaySound(5, 3)`) once and returns;
- step 0 (beam-in): returns. This is the start rule: nothing (no push, no animation, no sound) until X
  leaves the beam-in step. The beam-in step lasts until the landing animation ends (87 updates after
  the landing for X, `docs/players/ladder-hurt-events.md`);
- checkpoint 0: camera min x (current and target) = camera x. The camera cannot go back to the left;
- no terrain box (`mega+0x68` = 0, for example in a Ride Armor): returns;
- checkpoint ≥ 2: `PlaySound(5, 3)`, delete. This is the end rule;
- the sound `PlaySound(5, 6)` when `+0x14` = 0 (first time) or `mega+0x14C` ≠ 0; `+0x14` = 1;
- `BeltRotateTiles` and, when `frame_count % 5` = 0, `BeltDrawRollers` with the next frame;
- `TileCollisionAt(mega, x, y + box.oy + box.h)` (the tile under the bottom of the terrain box) =
  58: **`mega.x −= 1`** (whole pixels, the fraction is not changed, no wall test).

So the belt moves only the player, always to the left, at 1 px per update (60 px per second). It
does not change speed or direction. In the air the box bottom is not in a value-58 cell, so only a
player on the belt moves. A wall is handled later by `CollideCheck(mega)` [likely].

Other bodies:

- Junk block (stage type 11, `0x800EE790`, area ≠ 0 part): when its own foot tile is 58 and its left
  side is free (`0x800EE64C` and a tile test 1 px left of its box): `x −= 1`. With `var` bit 7 it
  first waits until the player x ≥ 1088 (`+0x07`). It does not test `BeltUpdate`, so it moves also
  during the beam-in [certain for the code].
- Stage type 15 (`0x800EEED8`, the Nightmare Effect object of area 1) has the same test, but only when
  the stage ≥ 19. So in stage 4 it does not ride the belt [certain for the code].
- Enemies: no other code in overlay 07 compares a tile with 58. The main-program compares with 58 are
  object types (`ShotSpark`, `0x800383F8`), not tiles [certain for the search].
- Ride Armor: no record makes one in area 1 (no stage logic 30). While riding, `mega+0x68` = 0 and the
  belt does nothing [certain for the code].

## The art [certain for the code; the frame order is likely]

- `BeltSaveFrames` (`0x800ED4D0`, in state 0): for the 8 places in `0x800F8C08` (x 32–128, y 560 and
  592, 32 px apart) it reads the 4 tile ids of each 2 × 2 cell block into `0x800F981C`. Block 0 is
  tiles 56, 57, 58, 59 (the tiles of the visible belt); blocks 1–7 hold tiles 208–235.
- `BeltRotateTiles` (`0x800ED764`): when `frame_count` (`0x80097420`) % 5 = 0, the 4-byte tile
  definitions (table at scratchpad `+0x0C`) of the 8 blocks turn by one place: block i gets the
  definition of block i + 1, block 7 gets the old block 0. So the visible tiles 56–59 show a new
  frame every 5 updates; 8 frames = 40 updates. `0x800E3142` (main RAM, below the overlay) counts
  the turns. The collision byte of each definition is the same in all 8 blocks (0 for the upper row,
  58 for the lower row), so the collision does not change.
- `BeltDrawRollers` (`0x800ED8AC`): frame = `0x800F985C` & 7 (`0x800F985C` starts at
  `0x800E3142` and goes up by 1 every 5 updates); for the 20 places in `0x800F88E8` (at y 400; 1 × 2
  or 2 × 2 cells: the belt ends) it calls `CopyTiles(0, dx, dy, src)` with the source cell from
  `0x800F8988` + frame × 4 + i × 32 (the frame store at y 544–847). Then layer 0 `+0x4C` = 1
  (redraw).
- No sprite, no texture of its own: the belt is layer-0 tiles. So `spriteexport` has nothing to
  export.

## Kind-7 type 3: the forced scroll (`AutoScroll`, `0x800E9C98`) [certain for the code]

Not the belt, but it acts on the same floor in the mini-boss room:

- does nothing while `mega+0xCC` ≠ 0 or `mega+0xE0` = 2;
- state 0: waits for checkpoint 1; then layer 1 mode 5 and target x (`+0x14`) = camera max x;
- state 1: waits for X in step 2 (stand); then `mega+0xF4` (walk and air speed) += 0.5 (2.0 → 2.5);
- state 2: when `game+0x26` ≠ 0: `mega+0xF4` −= 0.5, delete. Else, while camera x < target:
  camera x += 0.5 px per update and min x = max x = camera x (current and target).

In the mini-boss room the floor is belt for x 0–6399. So X goes left at 1 px per update when X
stands, and right at 2.5 − 1 = 1.5 px per update when X walks right; the camera goes right at 0.5 px
per update, and `CameraClamp` keeps X at camera x + 8 or more.

## Stage logic 10: the scene move (`0x800EDD00`) [certain for the code; owned by the stage agents]

State table `0x800F8C28`. State 0: checkpoint ≠ 0 → delete. State 1: when camera x ≥ 4672: X x =
4832, `PlayerEvent(20)` (X stands still). State 2 (`0x800EDBB8`), when X is in step 20: X and camera
move by −4672 in x and to y 944, camera limits x 0–6080 and y 944, checkpoint = 1, layer 1 mode 5,
`stage_texture_swap` = 1, `SpawnInRect`, delete. During step 20 the belt sound stops; the belt starts
again when X leaves step 20.

## The press in section 2

| field | `shark-2` | `shark-3` |
|---|---|---|
| state, sub-step | 1, 1 (wait) | 1, 1 (wait) |
| timer | 94 | 117 |
| speed | +1.0 (the next move is down) | −1.0 (the next move is up) |
| offset | 16 (top) | 96 (bottom) |
| saved | 0 | 96 |
| layer-1 y | 224 = 224 + 16 − 16 | 144 = 224 + 16 − 96 |

- `shark-2`: the intro (8 × 2.0 px) is done; the first top wait (180) has 94 updates left.
- `shark-3`: 43 updates after the slam (40 at the bottom, then timer 120 → 117). Timer 120 is the
  checkpoint-0 value [certain]. The quake moves layer-0 y to 226 in RAM; the press used 224 (the press
  runs before the layer cameras), so layer 1 = 144.
- Kill line in area ≠ 0: `L0.y − L1.y + 240` = `offset − var + 240` = 96 − 16 + 240 = **320**. Since
  `L1.y` follows `L0.y`, the quake does not move the kill line.
- X: y 347, stand box (0, 2, 11, 19): top = 347 + 2 − 19 = **330**, bottom = 368. 320 < 330: no crush.
  X is 10 px below the kill line.
- X stands on a floor at y 368 (x 864–1055), between the tooth body at x 928–991 (`var` 67) and the
  step to floor 320 at x 1056. X's box is x 1033–1055: the 64 px gap x 992–1055 is safe.
- Type 9 `var` 0 bodies: y = 176 + L0.y − L1.y = 256, box (0, −1, 120, 64): bottom 319 [certain].

So the rule holds. The section-2 press is the same object with `var` 16. `var` and the constant of
the kill line both are 16 less than in area 0, so the world kill line is the same:
**offset + 224** (240 at the top, 320 at the bottom) [certain]. The press sheet had "offset + 208"
for area ≠ 0; that was wrong (it used `var` 32).

### Floor tops under the section-2 press (`floor2.py`, first cell with value ≥ 56 below y 240)

| x | floor y | x | floor y |
|---|---|---|---|
| 0–319 | 416 (belt) | 2208–2271 | 384 |
| 320–383 | 368 | 2272–2335 | 416 (belt) |
| 384–479 | 416 (belt) | 2336–2399 | 352 |
| 480–543 | 368 | 2400–2463 | 368 (value 63) |
| 544–767 | 416 (belt) | 2464–2527 | 352 |
| 768–863 | 320 | 2528–2655 | 416 (belt) |
| 864–1055 | 368 | 2656–2783 | 320 |
| 1056–1151 | 320 | 2784–3007 | 416 (belt) |
| 1152–1599 | 416 (belt) | 3008–3503 | 368 |
| 1600–1663 | 368 | 3504–3711 | 320 |
| 1664–1727 | 384 (value 63) | 3712–3839 | 384 (value 63) |
| 1728–1791 | 368 | 3840–3967 | 352 |
| 1792–1999 | 416 (belt) | 3968–4031 | 416 (belt) |
| 2000–2079 | 352 | 4032–4095 | 352 |
| 2080–2207 | 416 (belt) | 4096–4479 | 416 (belt) |

### The teeth (type 9 `var` ≥ 64, follow layer 1)

Bottom at the press bottom = record y + 80 + box oy + box h. X standing on floor F has the box top at
F − 38; a tooth touches it at offset ≥ F − 22 − (record y + oy + h) [likely ±1 px, from the
`SolidObject` rule in `docs/engine.md`].

| `var` | x | box (px) | bottom at the top / at the bottom | floor under it | standing X touched at offset |
|---|---|---|---|---|---|
| 67 | 256–319 | 64 × 48 | 288 / 368 | 416 | never |
| 69 | 608–671 | 64 × 80 | 320 / 400 | 416 | ≥ 74 |
| 67 | 928–991 | 64 × 48 | 288 / 368 | 368 | ≥ 58 |
| 99 | 1280–1471 (2) | 96 × 48 | 288 / 368 | 416 | never |
| 67 | 1536–1599 | 64 × 48 | 288 / 368 | 416 | never |
| 67 | 1728–1791 | 64 × 48 | 288 / 368 | 368 | ≥ 58 |
| 70 | 1856–1919 | 64 × 96 | 336 / 416 | 416 | ≥ 58 (the tooth meets the floor) |
| 68 | 2272–2335 | 64 × 64 | 304 / 384 | 416 | ≥ 90 |
| 66 | 2464–2527 | 64 × 32 | 272 / 352 | 352 | ≥ 58 |
| 99 | 2848–3135 (3) | 96 × 48 | 288 / 368 | 416, 368 | never on 416; ≥ 58 on 368 |
| 66 | 3344–3407 | 64 × 32 | 272 / 352 | 368 | ≥ 74 |
| 67 | 3776–3839 | 64 × 48 | 288 / 368 | 384 | ≥ 74 |
| 68 | 3968–4031, 4096–4159 | 64 × 64 | 304 / 384 | 416 | ≥ 90 |
| 112 | 1536–1599, 2880–3007 | 64 × 16 | 304 / 384 | 416 | ≥ 90; also `ContactMega` when the player is under it |
| 112 | 3344–3407 | 64 × 16 | 288 / 368 | 368 | ≥ 58; also `ContactMega` |

The `var` 0 bodies over floors 320 and 352 kill as in section 1 (floor 320: standing X at offset
≥ 58; floor 352: at offset ≥ 90).

### Code differences from area 0

- Press, area ≠ 0 (`0x80047018`–`0x800470BC`): needs a terrain box; delete when player x ≥ 4480 or
  checkpoint **= 1** (the code compares with 1); the test runs only when player x < 4257; kill line
  `L0.y − L1.y + 240` [certain].
- Type 9 in overlay 07 (entry `0x800EE3F8`, states `0x800F8CD8`: `0x800EDDD8` set-up, `0x800EDF90`
  run, `0x800EE444` delete, sub-step `0x800EE2BC` follow): the same logic as overlay 06. In area ≠ 0:
  `SolidObject` only within 160 px of the player in x and when on screen; `var` 112 calls
  `ContactMega` when the player is under it (touch `0x04`); delete when player x ≥ 4480 or
  checkpoint > 0 [certain for the code].
- Kind-7 table in overlay 07: 0 `0x800E9B90`, 1 `0x800E9E4C`, 2 `PressUpdate`, 3 `AutoScroll`
  `0x800E9C98`, 4 `0x800E9864`. Types 0, 1 and 4 are not used by the area-1 records (not read).

## Open questions

- `mega+0x14C`: the belt restarts its sound when it is not 0. The player code writes 2 at
  `0x80034D30` (when `0x800E4430` ≠ `mega+0x93`) and 0 at `0x80034DA4` (before a `FadeIn`). Probably
  "back from the pause menu" [guess].
- Sound bank 5, ids 3 and 6: the belt stop and the belt loop [likely from the code; not heard].
- The visible direction of the art (the frames turn by one block every 5 updates): check in play.
- What sets checkpoint 2 after the mini boss (the end of the belt): the mini-boss agent.
- The 1 px between the body bottom and the kill line (from the section-1 sheet): still not checked.
- The tooth kill offsets are from the shared `SolidObject` rule, not from a state.
- Stage logic 7: values 16–28 and 60 (not read).
