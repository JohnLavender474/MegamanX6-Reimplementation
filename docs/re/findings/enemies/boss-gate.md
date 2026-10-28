# Boss gate (main program, stage object type 8): behaviour sheet

The boss gate is the shutter door that X walks through before a boss. It is a solid wall until the
player pushes into it from the left. Then it opens, moves the player through while the camera
scrolls, closes behind the player, and stays a solid wall. On the pass it also sets the new camera
limits, deletes the enemies, and can mark the time when the boss was reached. There is no official
enemy name for it; "boss gate" is a descriptive name.

The code is in the **main program** (entry `0x80050DFC`), so all stages share it. Record kind 4,
`b` 8 (stage-object class). Save state `intro-3_boss.state.gz`; kit:
`python3 -m mmxdemake.rekit --name boss-gate --state intro-3_boss.state.gz --type 8 --class stage`.
Also checked in `yammark-2_boss` and `turtloid-1`. Units, confidence tags and the shared engine
rules: `docs/engine.md`. Records: `docs/stages/intro.md`, `docs/stages/yammark.md`,
`docs/stages/turtloid.md`.

## Spawn and set-up [certain]

- Flags `0x49`, draw priority 6, layer = `mega.layer`, `flip` 0. Sprite bank `0x80` (the shared
  bank of the Dr. Light capsule and the gate).
- **Terrain box** `0x80078DC8` = (0, 0, 16, 32): a **32 × 64 px** solid body, centred on the record
  point. The record y is 32 px above the floor, so the box fills the corridor from the floor up
  64 px (intro gates: y 384, box y 352–416; the player stands at y 395, feet at 416) [likely].
- No attack box, no hurt box, no HP, contact damage 0. Weapons do nothing to it.
- No off-screen delete: only `DisplayCheck1` (visible within 32 px). The gate does not delete itself.
- Animation 0 frame 0 (sprite 58, closed) is loaded but does not run until the gate opens.

## Collision: a solid object, not tiles [certain for the code]

The gate does not copy tiles. Each update, while it is closed (step 0 and the final state), it calls
`SolidObject` (`0x80031DA8`) with its terrain box: the standard solid object (`docs/engine.md`,
"Solid objects"). Particular to the gate: after the call it clears the bits `0x04` and `0x08` again
on the player (`0x80051104`), so the player does not get a "floor" or "ceiling" from the gate
[likely for the effect]. So the gate is a wall, not a platform.

During steps 1–4 (opening, pass, closing) the gate has **no** collision.

## The `id` parameter [certain for the code]

`var` = record `id` & `0x3F`; bits 6–7 are dropped (Yammark's id 67 is case 3). The id does three
things:

1. **Open condition** (tested every update in step 0; while it fails, the gate is a locked wall):

   | id | the gate opens only when |
   |---|---|
   | 7, 8, 10, 12, 14, 15 | byte `0x800CCEF6` (`game+0x26`) ≠ 0 |
   | 34 | byte `0x800CCEF8` (`game+0x28`) ≠ 0 |
   | 44 | byte `0x800CCEF6` (`game+0x26`) = 0 |
   | others (also 20) | always |

   The test is the `bne` chain at `0x8005027C`–`0x80050358` [certain].

2. **The new camera limits** after the gate is open (jump table `0x80011694`, 51 cases for ids
   1–51). First every case sets min x = max x = X (the gate x) and the push speed 0.640625 px/frame
   (`0xA400`); then the case changes some values. Y = the gate y. Blank = not changed (it keeps the
   old target):

   | id | min x | max x | max y | min y | push (px/frame) | time stamp |
   |---|---|---|---|---|---|---|
   | 1 | X | X+80 | | | 0.64 | |
   | 2 | X | X | 224 | 224 | 0.64 | |
   | 3, 40 | X−48 | X−48 | Y−64 | Y−64 | 0.83 | |
   | 4, 22 | X | X+32 | | | 0.64 | yes |
   | 5 | 6016 | 6016 | 304 | 304 | 0.83 | |
   | 6 | X | 6384 | 320 | 304 | 0.64 | yes |
   | 7 | 944 | 976 | 2624 | Y−160 | 0.64 | |
   | 8 | X−16 | X−16 | | 1616 | 0.64 | |
   | 9 | X | 2416 | 2816 | 2528 | 0.64 | |
   | 10 | 2544 | 2544 | | 944 | 3.84 | |
   | 11 | X | 2416 | 2304 | 2080 | 0.64 | |
   | 12 | 2544 | 2544 | | 2160 | 3.84 | |
   | 13 | X−16 | X+16 | | | 0.64 | |
   | 14 | X | 3504 | 1184 | | 0.64 | |
   | 15 | X−16 | X−16 | 944 | | 0.64 | |
   | 16 | X | X+32 | 944 | 944 | 0.64 | yes |
   | 17, 21, 32, 42 | X−16 | X−16 | | | 0.64 | |
   | 18, 34, 48 | X | X | | | 0.64 | |
   | 19 | 1264 | 1264 | 1184 | 1104 | 0.64 | |
   | 20 | X | 1680 | 1184 | 1136 | 0.64 | yes |
   | 23 | X | 9040 | 688 | 640 | 0.64 | yes |
   | 24 | X | X | 480 | | 0.64 | |
   | 25 | X | 7360 | | | 0.64 | |
   | 26 | X | 8096 | 160 | | 0.64 | |
   | 27 | X | X+16 | 928 | 832 | 0.64 | yes |
   | 28 | 2880 | 2880 | | | 0.64 | |
   | 29 | X | 3216 | 224 | 224 | 0.64 | |
   | 30 | X−48 | **Y−48** | | | 0.64 | |
   | 31 | X | X+32 | | Y−144 | 0.64 | |
   | 33 | X | X+32 | | | 0.64 | |
   | 35, 38, 45, 47, 49, 51 | X | X+32 | Y−160 | Y−160 | 0.64 | |
   | 36 | X | X | | | 0.64 | yes |
   | 37 | 2848 | 2848 | | | 0.83 | |
   | 39 | X | X | 0 | 0 | 0.64 | |
   | 41, 43 | X | X+32 | Y−112 | Y−112 | 0.64 | |
   | 44 | X−112 | X−112 | | | 1.77 | |
   | 46 | X−48 | X−48 | | | 0.83 | |
   | 50 | X | X | Y−160 | Y−160 | 0.64 | |
   | 0, > 51 | X | X | | | 0.64 | |

   Push values: `0xA400` = 0.640625, `0xD400` = 0.828125, `0x1C400` = 1.765625, `0x3D800` =
   3.84375 px/frame. Id 30 writes Y−48 to the max x; this is what the code does (maybe an error in
   the game) [certain for the code]. Id 30 is the first gate of Gate's corridor (stage 18, x 2928,
   y 1072): in `gate-4_before-gate` the target max x is 1024 and the current max x 2878, below the
   min x 2880, and the camera stays at x 2880 (`docs/stages/gate-2b.md`) [certain for the values].

   **Time stamp:** when the word `game+0x43C` (`0x800CD30C`) is 0, it gets `game+0x90`
   (`0x800CCF60`, the stage time in frames [likely]). In `yammark-2_boss` it holds `0x4B62`, 161
   frames before the save-state time [certain]. What reads it is not known.

3. **Special behaviour** of single ids:

   | id | what |
   |---|---|
   | 7 | at the trigger: `ClearEnemies`, then a main object type 25 with `var` 4 (no null test): the next Nightmare Snake (stage 3 area 0, `docs/enemies/nightmare-snake.md`) |
   | 18 | at the end of the pass: byte `0x8008EAB0` = 1, min x = 3440 |
   | 20 | after closing it has no collision (a one-way gate) |
   | 25 | at the trigger: sound (bank 5, id 1), `Rumble(4)`; then it holds the player while the halfword `0x8009728E` goes down 1 px every 2 frames to −80, with `Quake2(10, 2, 2)` every 11 frames; then sound (bank 5, id 3), byte `0x8009724F` = 0 |
   | 39 | as 25, but `0x8009728E` goes down 1 px per frame for 112 frames |
   | 31 | before the opening: a fade step (below) |

**Which gate is "before the boss":** each boss has two gates with a one-screen corridor between them.
The first gate locks the camera on the corridor (min x = max x); the second opens the boss room. The
time stamp ids (4, 6, 16, 20, 22, 23, 27, 36) are second gates [likely]. Read from the records:

| stage | first gate | second gate (boss room) |
|---|---|---|
| intro | id 2, x 5744: camera fixed at x 5744, y 224 | id 1, x 6064: camera x 6064–6144 |
| Yammark | id 67 (case 3), x 4848: camera fixed at (4800, 1904), push 0.83 | id 4, x 5104: camera x 5104–5136, time stamp |
| Turtloid | id 34, x 8624: opens only when `game+0x28` ≠ 0 (Weather Analyze machine 3 destroyed); camera fixed at x 8624 | id 23, x 8944: camera x 8944–9040, y 640–688, time stamp |
| Gate stage 1 | id 39, x 7664: camera fixed at (7664, 0); lowers BG layer 1 (the lava) | id 26, x 7984: camera x 7984–8096, max y 160 |
| Gate stage 2 (stage 17) | id 28, x 2896: camera fixed at x 2880 | id 29, x 3184: camera x 3184–3216, y 224 (High Max) |

Checked against the save states: `intro-3_boss` has max x 6144, min x 6064, y 224 / 224;
`yammark-2_boss` has max x 5136, min x 5104, y 1904 / 1904, and the Yammark gate 1 has push
`0xD400` [certain].

## State machine

States (table `0x80078DD4`): 0 set-up `0x8005011C`, 1 main `0x80050E58`, 2 closed for good
`0x800510C4`. There is no delete state. Steps (table `0x80078DE0`): 0 WAIT, 1 START, 2 OPEN, 3 PASS,
4 CLOSE, 5 FADE. Step 0 is the gate's own routine, not `HitReactionDispatch`. The step routines get
`game` and `mega` as arguments.

### WAIT (step 0)

1. Solid collision (above).
2. The open condition by id (above). If it fails: wait.
3. **Trigger test** (`0x80050C88`). All must be true:
   - the player is not in a blocked state: `mega+0xA4` = 0, bit 7 of `mega+0x5C` clear,
     `mega.step` ≠ 3, and `mega+0x7A` = 0 (X) or `mega+0x8E` = 0 (Zero) [certain for the fields;
     their meaning is not known];
   - the player's x ≤ the gate's x (**the gate opens only from the left**);
   - the two terrain boxes touch or overlap in x (distance of the centres ≤ 16 + the player's half
     width; the push-out keeps the player exactly touching, so walking into the gate is enough);
   - the player's box is fully inside the gate's height (top ≥ gate top, bottom ≤ gate bottom). So
     the player must stand on the floor in front of the gate, not jump against it [likely].
4. On the trigger: `game+0x1C` (`0x800CCEEC`) = 1, `mega+0xD4` = 1 (ids 25 and 39 first do their
   special part). Then `ClearEnemies` (`0x8002B79C`): **every main object except items (type 47)
   and every visual object (enemy shot) is cleared** [certain]. `cam+0x34` &= `0xF`. Step 1.

### START (step 1)

The player code (`0x8003A1C8`) sees `mega+0xD4` > 0, turns the player to the right, sets
`mega+0xD4` = −1 and puts the player in its step 19 [certain for the code]. The gate waits for that
(−1), then:

- `mega+0xD4` = −2 (passing). While it is −2, the camera update skips its event handlers
  (`cam+0x34` bits `0x10` and `0x01`).
- `game+0x10` and `game+0x12` (`0x800CCEE0`, `0x800CCEE2`) = 1 [meaning not known].
- **The player's terrain box is set to none** (`mega+0x68` = 0): the player has no wall or floor
  collision during the pass.
- The camera limit speed `cam+0x47` is saved and set to 2 px/frame.
- Sound (bank 0, id 16); step 2. (Id 31: step 5 instead.)

### OPEN (step 2)

Animation 0 runs. At frame 27 (after 67 updates): sound (bank 0, id 17). At frame 29 (after 75
updates, sprite 70, open): `game+0x10` = `game+0x12` = 0, the new camera limits by id (above), then
step 3. So the gate opens in **75 updates (1.25 s)**.

### PASS (step 3)

Each update the gate adds the push speed to the player's x (to the right). The camera's min-x limit
moves toward the new min x at 2 px/frame (`CameraLimitEase`, `0x80028EBC`), so the view scrolls right
at 2 px/frame [likely]. When the camera x equals the new min x:

- `mega+0xD4` = 0; the player's terrain box comes back (`PlayerSetTerrainBox`, `0x8003CCBC`: X
  `0x80073D50`, Zero `0x80073D54`);
- `cam+0x47` is restored;
- sound (bank 0, id 17); animation 1 (close); step 4.

Example, the intro boss gate: the camera goes from 5744 to 6064, 320 px at 2 px/frame = **160
updates**; the player moves 160 × 0.64 ≈ **103 px** to the right [likely].

### CLOSE (step 4)

Animation 1 runs. At frame 2 (after 8 updates): sound (bank 0, id 16). At frame 29 (after 76 updates,
sprite 58, closed): state 2. Then, when `mega+0xD0`, `mega+0xD4` and `mega+0xE0` are all 0,
`game+0x1C` (`0x800CCEEC`) = 0. The player code reads this byte and blocks some input while it is
not 0 [likely], so the lock lasts from the trigger to the closed gate.

### FADE (step 5, id 31 only)

Every 3 updates: halfword `0x800CF854` + 1, halfword `0x80097430` + 32, halfword `0x80090D5C` +
`0x400`, `0x800C4560` bit 0 set. At `0x800CF854` = 31 (93 updates): sound (bank 0, id 16), step 2.
Before it, `gate_start` calls `0x8002B2F8` and fills arrays at `0x80090C98`. This is probably a
palette or screen fade [guess].

### Closed (state 2)

Solid collision again (except id 20). The gate never opens again, so the player cannot go back.

## Every global that the gate writes

| address | what |
|---|---|
| `0x800971F8+0x24` / `+0x26` | camera target max x / min x [likely for the names] |
| `0x800971F8+0x28` / `+0x2A` | camera target max y / min y [likely] |
| `0x800971F8+0x34` | camera event flags: `&= 0xF` on every update of steps 0–4 |
| `0x800971F8+0x47` | camera limit speed: 2 during the pass [likely] |
| `0x8009724F`, `0x8009728E` | ids 25, 39 only: fields `+0x03` and `+0x42` of the layer-1 camera (`0x8009724C`, the next `0x54`-byte layer record); probably a background offset [guess] |
| `0x800CCEE0`, `0x800CCEE2` | 1 during the opening |
| `0x800CCEEC` | player lock: 1 from the trigger to the closed gate |
| `0x800CD30C` | boss-gate time stamp (some ids) |
| `0x8008EAB0` | id 18 only |
| main and visual object arrays | cleared at the trigger |
| `mega+0xD4` | gate state of the player: 1, −2, 0 |
| `mega+0x68` | the player's terrain box: none during the pass, then restored |
| `mega+0x08` | the player's x, + push speed per update |
| `mega+0x71`, `mega+0x4A`, `mega+0xE6` | written by `SolidObject` |

It does **not** start the boss, the boss music or the boss HP bar, and it does not change the stage
area. The boss starts by itself when its record comes into view [guess].

## Animations (table `0x80078DCC`)

| # | what | frames |
|---|---|---|
| 0 | open: the closed cylinder flashes and its lock turns (sprites 58–67), then the top and bottom halves pull back (68, 69, 70) | 30 frames; flag 1 on frame 27, flag 2 on frame 29 (sprite 70, held); 75 updates to the end |
| 1 | close: the reverse (70, 69, 68, then 58–67) | 30 frames; flag 1 on frame 2, flag 2 on frame 29 (sprite 58, held); 76 updates to the end |

Both animations and both flags are used by the code.

## Sounds

| bank, id | when |
|---|---|
| 0, 16 | the opening starts; the closing, after 8 updates; the end of FADE |
| 0, 17 | the opening, after 67 updates; the closing starts |
| 5, 1 | the trigger of ids 25, 39 (with `Rumble(4)`) |
| 5, 3 | the end of the special part of ids 25, 39 |

## Summary for a reimplementation

- A 32 × 64 px solid block at the record point. It cannot be hurt.
- It opens when the player stands on the floor and walks into it from the left, and is not in a
  blocked state. Some ids open only when a stage flag is set.
- On the trigger: delete all enemies and enemy shots; lock the player's input; the player faces right.
- Opening: 75 frames. Then set the camera limits of the id (usually: the next screen, from the gate
  x).
- Pass: the player has no collision and is moved right at 0.64 px/frame (some ids 0.83, 1.77, 3.84);
  the camera scrolls right at 2 px/frame until it reaches the new left limit.
- Closing: 76 frames. Then the gate is a wall for good.
- In Maverick terms: a two-door boss corridor; door 1 locks the camera on the corridor, door 2 on the
  boss room.

## Open questions

1. The meaning of `game+0x26` / `game+0x28` in the stages other than Turtloid (the open conditions
   of ids 7, 8, 10, 12, 14, 15 and 44). In Turtloid, `+0x26` is "rain on" and `+0x28` is "Weather
   Analyze machine 3 destroyed" (`docs/enemies/weather-analyze.md`). Ids 7, 8, 10, 12, 14 and 15
   are all gates of Blaze Heatnix's stage (`docs/stages/heatnix.md`). There the end of Nightmare
   Snake `var` 5 sets `game+0x26` = 1 and gate 7 opens [certain for the code,
   `docs/enemies/nightmare-snake.md`]; the other snakes probably open the other gates [likely].
2. The player fields of the trigger test (`mega+0xA4`, step 3, `+0x7A`, `+0x8E`; `+0x5C` bit 7 is
   the dead flag of the HP byte, `docs/engine.md`) and
   what player step 19 does (does the player walk or only stand while the gate pushes it?).
3. What reads the time stamp `0x800CD30C`, and what `game+0x10` / `game+0x12` do.
4. Bits 6–7 of the record id (Yammark's 67): the gate drops them; whether other code reads them.
5. Whether a gate in state 2 is ever deleted (it has no delete), for example when the player dies
   and the stage restarts at a checkpoint.
6. What the layer-1 fields `+0x42` / `+0x03` and id 31's arrays are (main type 25 of id 7 is
   answered: Nightmare Snake); in which stages ids 7 and
   18 are used. Id 31 is the second gate of Gate's room (stage 18, record 63, x 3184); Gate's
   intro undoes its FADE with the reverse steps (`docs/enemies/gate.md`, intro sub-step 5), so
   the FADE darkens the room before Gate's talk [likely]. Ids 25 and 39 are gates of Gate stage 1 (stage 16, records 103 and 104):
   there BG layer 1 is a lava band and `0x8009728E` is its y; the lava controller (main 29) raises
   it from −80, and the two gates put it back to −80 (`docs/stages/gate-1.md`) [likely].
