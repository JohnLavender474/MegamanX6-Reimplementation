# Weather Analyze (stage 09, main object type 14): behaviour sheet

Weather Analyze is the weather machine of Rainy Turtloid's stage. While it is alive and X is inside
its zone, it sets the stage's "rain" flag, and a stage-logic object makes the acid rain: the screen
palette changes, and X loses 1 HP every 180 frames. The machine has four small sensors around the
area. They protect it: the machine can be hit only after all four are destroyed. When it is
destroyed, the rain stops and some camera limits (and, for one machine, some map tiles) change. The
name is the one the user gave; the Fandom list could not be read (HTTP 402), so the spelling was not
checked.

Overlay `09` (`0x800E9860`). Save state `turtloid-1` (area 0; no Weather Analyze is live in it). Kit:
`python3 -m mmxdemake.rekit --name weather-analyze --state turtloid-1.state.gz --type 14`. Units,
confidence tags and the shared rules: `docs/engine.md`. Records: `docs/stages/turtloid.md`.

## One type, four kinds of object [certain]

The entry `0x800EB5F8` selects the routine by `var` (byte 2), read as a signed byte:

| `var` | object | routine |
|---|---|---|
| ≤ 0 or ≥ 7 | the machine (made by a record) | standard states, table `0x800F5DE4` |
| 1–4 | sensor (made by the machine) | `0x800EB914`, table `0x800F5EC8` |
| 5 | display on the machine (made by the machine) | `0x800EBA90`, table `0x800F5EE4` |
| 6 | barrier on the machine (made by the machine) | `0x800EBD14`, table `0x800F5EF4` |

The records use ids `0x10`–`0x13`, so the signed byte is ≥ 7 and the machine routine runs.

## The `id` parameter [certain]

The machine stores `id & 0x0F` in the overlay global `wa_index` (`0x800F6BA0`, 1 byte) and sets
`var` to 0. The index selects one row of each table below. There are 7 rows (0–6); the area-0
records use 0–3. Stage 18 (section 2 of Gate stage 2, overlay 24, the same tables) uses rows 5 and
6 (records `id` `0x15`, `0x16`; `docs/stages/gate-2b.md`). All machines share the one global, so only one machine can work at a time
[certain for the code; the level is built so that only one is live, likely].

| index | machine position (x, y) | rain zone (X's x, inclusive) | camera on death |
|---|---|---|---|
| 0 | 1574, 592 | 1248–1631 | `+0x24` = 2272 |
| 1 | 2848, 384 | 1872–2879 | `+0x24` = 7360; `+0x2A` = 224; 2 tile copies |
| 2 | 7648, 384 | 3584–7695 | `+0x24` = 8288 |
| 3 | 8576, 672 | 7792–8655 | none; sets `game+0x28` = 1 |
| 4 | 854, 864 | 0–1119 | none |
| 5 | 3168, 1488 | 1920–3199 | `+0x26` = old `+0x24`, then `+0x24` = 3840 |
| 6 | 4144, 1584 | 3328–4207 | none |

Tables: positions `0x800F5DF4` (4 bytes per row), zones `0x800F5E90`, camera values `0x800F5E80`
(2 bytes per row; 0 = no change), sensor positions `0x800F5E10` (16 bytes per row). The machine does
**not** use its record position: it jumps to the table position at set-up.

Sensor positions (x, y) by index [certain]:

| index | sensor 1 | sensor 2 | sensor 3 | sensor 4 |
|---|---|---|---|---|
| 0 | 1296, 448 | 1584, 448 | 1392, 576 | 1472, 640 |
| 1 | 2536, 528 | 1928, 384 | 2528, 64 | 2816, 256 |
| 2 | 4144, 384 | 4992, 384 | 6064, 384 | 7520, 400 |
| 3 | 7888, 704 | 7888, 848 | 8560, 528 | 8560, 832 |
| 4 | 1037, 299 | 1037, 595 | 47, 413 | 47, 816 |
| 5 | 2143, 1567 | 2688, 1584 | 3040, 1544 | 3095, 1583 |
| 6 | 3584, 1488 | 3632, 1648 | 3792, 1616 | 3984, 1472 |

So the sensors are spread over the zone, up to 3500 px from the machine (index 2).

## The machine

### Set-up (state 0, `0x800EB6C0`) [certain]

- **HP 32**, contact damage 0, **no attack box**, **no hurt box** (it cannot be hit yet).
- Terrain box `0x800F5CB0` = (0, 0, 20, 32): 40 × 64 px.
- Gravity 4 px/frame² (`0x40000`), so it falls at `fall_max` at once. Draw priority 6.
- `dmg_tbl_normal`; animation 0; position from the table.
- Sets the rain flag `game+0x26` (`0x800CCEF6`) = 1.
- Step 2, `act4` (`+0x07`) = 5, then state 1.

### Main (state 1, `0x800EB2D4`)

Each update, in this order [certain]:

1. Weapon test (`ContactWeapon`).
2. **Rain zone:** `game+0x26` = 1 when X's x (`mega.x.px`) is inside the zone of `wa_index`, else 0.
3. When killed: see Death.
4. Else: contact test (no box, so no effect); off screen 40 / 64 only selects the visibility test
   (visible 40 / 64). **It never despawns**: no code sets state 3.
5. The step (table `0x800F5EBC`): 0 engine hit reaction, 1 nothing, 2 WAIT (below); then
   `MoveObject2` and `AnimeTick`.

**WAIT (step 2):**

- On the first update with the floor flag while `act4` = 5: it makes 6 main objects with `var` 1–6.
  Each copies the machine's flags and type and keeps the machine at `+0x80`. Then `act4` = 4.
- `act4` is the count of live sensors. When it is 0 and there is no hurt box yet, the hurt box
  `0x800F5CB4` = (−16, −32, 32, 64) is set: 32 × 64 px. From then on weapons hurt it.

### Death (in state 1) [certain]

1. Index 5: camera `+0x26` = camera `+0x24`. Then, when the table value is not 0: camera `+0x24` =
   the value (see the table). Camera = `camera_layer0` (`0x800971F8`).
2. Index 1: camera `+0x2A` = 224, and two tile copies on layer 0 (`CopyTiles`, source rectangles at
   `0x800F5EAC`): {x 2832, y 512, w 1, h 20} to (2592, 96), and {x 2944, y 512, w 1, h 22} to (2832, 0).
   Probably this opens a passage [likely; the map was not looked at].
3. `game+0x26` = 0 (the rain stops).
4. One explosion, 4 debris pieces (animations 14–17), animation 13 (the wreck), hit flash off,
   state 2. **No item drop.**

The camera fields: `CheckpointStart` (`0x80029B8C`, store at `0x80029C20`) writes the same source value
to `+0x1C` and `+0x24`, `+0x1E` and `+0x26`, `+0x20` and `+0x28`, `+0x22` and `+0x2A`. These
are the target limits (`docs/engine.md`, "Camera"): `+0x24` max x, `+0x26` min x. In the save state they are 1296, 0, 464, 464. Index 0 lifts the right limit from 1296 to
2272, which suggests that the camera cannot pass the first machine until it is destroyed [guess].

### After death (state 2 `0x800EB89C`, state 3 `0x800EB8F0`) [certain]

- State 2: index 3 sets `game+0x28` (`0x800CCEF8`) = 1. Then state 3.
- State 3: only the visibility test. The wreck stays for ever; it is never deleted, so its record stays
  "spawned" and the machine does not come back while the area is loaded.

## Sensor (`var` 1–4) [certain]

- Copies the machine's graphics (texture page, palette, animation table, sprite data).
- **HP 16**, contact damage **4**, attack box `0x800F5CB8` = hurt box `0x800F5CBC` = (−6, −6, 12, 12).
- `dmg_tbl_normal`, priority 6, no terrain box, no gravity, no speed: it does not move.
- Position: row `wa_index`, column `var − 1` of the sensor table. Animation 9 (one still frame).
- Main (`0x800EB974`): weapon test; killed → the machine's `act4` − 1, one explosion, 3 debris pieces
  (animations 10–12), state 2 (`DeleteObject`). Else contact test, visible 16 / 16. The step table
  `0x800F5ED8` has only empty routines. No drop.

## Display (`var` 5) [certain for the code]

- Copies the machine's graphics, priority 4, no boxes, HP 0, at the machine's position.
- Shows the count of live sensors: 4 → animation 1, 3 → 2, 2 → 3, 1 → 4. At 0 it deletes itself.
- Probably a lamp or screen on the machine [guess].

## Barrier (`var` 6) [certain for the code]

- Copies the machine's graphics, priority 5, attack box `0x800F5CC0` = (−24, −32, 48, 64), contact
  damage **0**, no hurt box, at the machine's position.
- Animation by the live sensors: 4 → 5, 3 → 6, 2 → 7, 1 → 8 (each flickers: a picture and sprite 18,
  2 updates each). At 0 it deletes itself.
- Probably a shield picture over the machine [guess]. What a contact with damage 0 does to X is not
  read (`PlayerDamageScale` has a minimum of 1) [open].

## Animations (table `0x800F5D9C`)

| # | what | frames |
|---|---|---|
| 0 | machine | sprites 0, 1, 2, 4 updates each, loop |
| 1–4 | display, 4 → 1 sensors left | 2 sprites, 14 updates each, loop (4/5, 19/20, 21/22, 23/24) |
| 5–8 | barrier, 4 → 1 sensors left | 4 sprites alternating with sprite 18, 2 updates each, loop |
| 9 | sensor | sprite 3, hold |
| 10–12 | sensor debris | sprites 34, 35, 36 |
| 13 | machine wreck | sprite 29, hold |
| 14–17 | machine debris | sprites 30–33 |

No event flags are used.

## The acid rain (stage-logic object, type 11)

The rain itself is not part of this object. A stage-logic object (class at `0x8009B7B0`, 96 slots of
`0x30` bytes, update table at scratchpad `+0x8C`; a kind-3 record class [likely]) of type 11, entry
`0x800ED510` in overlay `09`, reads the flag. It is live in the save state (slot 3, state 1).
States (table `0x800F60AC`) [certain for the code]:

| state | routine | what |
|---|---|---|
| 0 | `0x800ED5C4` | set-up: runs even during a pause (flags bit 3); saves 32 bytes of palette |
| 1 | `0x800ED69C` | dry: waits for `game+0x26` ≠ 0; then sound (bank 5, id 4), state 2 |
| 2 | `0x800ED774` | palette fades to the rain palette over 30 updates; flag off → state 4 |
| 3 | `0x800ED3E4` | **rain**: the HP drain (below); flag off → state 4 |
| 4 | `0x800ED850` | fades back over the frames faded in; flag on → state 2; at 0: sound (bank 5, id 3), state 1 |

**HP drain (state 3)** [certain for the code]:

- Only while X's HP (`mega+0x5C`) ≠ 0 and `mega+0xE0` = 0.
- A timer (`+0x16`) counts each update. At **180 updates (3 s)** it restarts and X loses **1 HP**, unless
  the byte `0x8008EAFC` (`text_open`: a text box is open [likely]) ≠ 0. HP 1 becomes `0x80` (HP 0 with bit 7, which `ContactMega` treats as
  "no contact": probably the death flag) [likely]. Bit 7 of the HP is kept.
- The drain writes the HP directly: no difficulty scaling, no armour halving, no invincibility test.
- A second timer (`+0x18`): every **51 updates** it sets `mega+0xE8` = 1. The life-bar drawing
  (`0x80025064`) uses it for one frame, so the life bar blinks [likely].
- While `game+0x27` (`0x800CCEF7`) ≠ 0, both timers are held at 0 (the dispatcher `0x800ED510`).
  `game+0x27` is set by the health capsule (stage-object type 13, `0x800EE5A8`) while it holds X in
  place and refills the HP (`docs/enemies/health-capsule.md`) [certain].
- When `mega+0x14C` ≠ 0, sound (bank 5, id 4) is played again [certain; why is open].
- Both timers restart only on the dry → fade-in change. So the first HP loss comes 180 updates after
  the fade-in ends.

## Other readers of the flags [certain for the code]

- `game+0x26`: the Turtloid teleporter crystal (main type 28, `0x800F0174`) waits until it is 0;
  the boss gate (stage-object type 8, WAIT step `0x80050250`) needs it ≠ 0 for `id` 7, 8, 10, 12,
  14, 15 and = 0 for `id` 44 (`docs/enemies/boss-gate.md`); main-program code at
  `0x80047460`, `0x8004755C` and `0x800529C8` also tests it. So `game+0x26` is a general stage flag
  that each stage uses in its own way.
- `game+0x28`: the boss gate `id` 34 (the Turtloid corridor gate, x 8624) needs it ≠ 0. So the
  corridor opens only after machine 3 is destroyed. No other reader found.
- `0x8001E81C` clears `game+0x26`…`+0x35` (16 bytes); when it runs was not read.

## Damage

- Machine and sensors: `dmg_tbl_normal`. Machine 32 HP: 11 lemons, 5 half charges, 3 full charges, 8
  sabre hits. Sensor 16 HP: 6 lemons, 3 half charges, 2 full charges, 4 sabre hits.
- Dealt: sensor contact 4. Machine: none. Barrier: 0 (see above). Acid rain: 1 HP per 180 updates.

## Sounds

The object plays none. The rain object: bank 5 id 4 (rain starts), bank 5 id 3 (rain ends).

## Summary for a reimplementation

- A record (id = machine index) makes the machine; it jumps to its fixed position and falls.
- On landing it makes 4 static sensors (16 HP, contact 4, 12 × 12 px) across the area, a display and
  a barrier picture.
- The machine is invulnerable until all 4 sensors are destroyed; then it has 32 HP (32 × 64 px hurt
  box) and no attack.
- While it lives, X inside its x zone turns the rain on; outside, the rain stops. Fade 30 frames.
- Rain: −1 HP every 180 frames, not scaled, can kill.
- Destroyed: rain off for good in this area, a wreck stays, camera limits change, no drop.

## Open questions

1. What `mega+0xE0` (no drain) is; and why `mega+0x14C` replays the rain sound. (`0x8008EAFC` is
   very probably "a text box is open": the Light capsule and Commander Yammark wait on it after
   `TextOpen`.)
2. Whether a destroyed machine stays destroyed after a death or a checkpoint restart (no flag is saved
   for index 0–2; index 3 sets `game+0x28`, and it is not known what clears it).
3. Where rows 4–6 are used (no record in area 0), and the meaning of the camera fields `+0x24`–`+0x2A`.
4. What the barrier's contact (damage 0) does to X.
5. The tile copies of index 1: the cell size of `CopyTiles` and what the copy changes on the map.
