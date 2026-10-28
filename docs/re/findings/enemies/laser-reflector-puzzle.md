# Laser reflector puzzle (stage 10, main object type 13): behaviour sheet

The latter part of Shield Sheldon's stage is a chain of rooms. Each room ends in a closed door. In
the room, beamers fire lasers. A laser that hits the front of a reflector makes a new laser in a new
direction. When a laser hits the sensor of the door, the door opens. Some beamers turn one step
toward the player before each shot. Some reflectors turn one step when X or Zero hits them with a
weapon. One main type (13) makes all the parts: the `var` (record `id`) picks the part. The laser is
a visual object (type 11). It has a probe box in the weapon pool (type 79), and it is drawn each
frame with misc objects (type 7).

No official names were found: the Fandom enemy list returned HTTP 402 and the Mega Man Wiki returned
HTTP 403 when this sheet was written. The names here are descriptive: **beamer** (fixed, aiming,
relay), **reflector** (fixed, turning), **door** (the part that opens), **laser**.

Overlay `07` (Shield Sheldon), save state `sheldon-7_lasers` (stage 7, area 0; X, no armour; the
first room is solved and its door is open). Written with the RE kit
(`python3 -m mmxdemake.rekit --name sheldon-lasers --state sheldon-7_lasers.state.gz --type 13`)
and `docs/re-guide.md`. The kit walks the laser (visual 11) and the draw pieces (misc 7) as
neighbours. The probe (weapon type 79) is main-program code; it was read with `--at`. Units,
confidence tags and the shared rules: `docs/engine.md`. Records: `docs/stages/sheldon.md`.

Gate stage 1 (stage 16, overlay 22) has the same object as main type 13: the same 34 animations,
42 functions of the same sizes in the same order (34 identical after the addresses are masked), the
damage tables `dmg_tbl_reflector` and `dmg_tbl_reflector_wait`, and the neighbours visual 11 and
misc 7 [likely: the same code]. Its 24 records have `id` 0–6 only: fixed beamers (`0x0n`) with no
reflector and no door, so there they are laser traps (`docs/stages/gate-1.md`).

## Which object is which [certain]

| part | class, type | `var` | live in `sheldon-7_lasers` |
|---|---|---|---|
| fixed beamer | main 13 | `0x0n` | – |
| aiming beamer | main 13 | `0x1n` | slots 0, 3, 4 (`0x15`, `0x16`, `0x16`) |
| relay beamer | main 13 | `0x2n` | – |
| fixed reflector | main 13 | `0x3n` | slot 1 (`0x3F`) |
| turning reflector | main 13 | `0x4n` | – |
| door | main 13 | `0x5n` | slot 2 (`0x50`), already open |
| laser | visual 11 | 0 | slots 0–3, one per live laser |
| laser probe | weapon 79 | 0 | slots 0–3, one per laser, at the laser's head |
| laser draw pieces | misc 7 | 0 = body, 1 = end sprite | 3 per laser, made again each frame |

`n` is `var & 15`. The other objects in the state are not part of the puzzle: misc type 14 is one
scenery sprite near the stage start (it deletes itself at checkpoint 2 or more), and the stage-logic
objects (types 3, 7, 14, 27) are not made or read by this code.

## Spawn and set-up (state 0, `0x800EA13C`) [certain]

All parts:

- HP 127, contact damage 1, no attack box, no terrain box, no `SolidObject` call. So no part
  hurts the player, and no part is solid. The closed door is solid only through its tiles.
- Damage table `0x8007451C` (`dmg_tbl_reflector`, below); `flip` 0 (boxes not mirrored); draw
  priority 7; layer = `mega.layer`; speeds 0.
- `work.84` = the next **shot id** from the 16-bit counter `0x800FCFEC`, and the counter + 1. Each
  laser also takes a shot id. A part never reacts to a laser with its own id (below).
- `+0x86` (the direction of the last laser that hit it) = 64 ("none").

By part:

| `var` | direction (`work.88`) | animation | hurt box (laser sensor) | first state, step |
|---|---|---|---|---|
| `0x0n` | `n` (0–7) | `8 + n` | none | 1, 3 |
| `0x1n` | `n` | `8 + n` | none | 1, 4 |
| `0x2n` | `n` | `n` | (−8, −8, 16, 16) | 1, 2 |
| `0x3n` | `n` (0–15) | `16 + n` | `0x800FB968[n]` | 2, 2 |
| `0x4n` | first entry of its turn list (below) | `16 + r` | `0x800FB968[r]` | 2, 2 |
| `0x5n`, `n` = 0–3 | – | 33 | (−16, −40, 16, 16) | 3, 2 |
| `0x5n`, `n` = 4, 5 | – | 32 | (24, −16, 16, 16) | 3, 2 |

A door also spawns the records of the room after it (next section, "How the objects find each
other").

## Directions [certain]

- **Laser and beamer directions** are 8-way, 0–7, counter-clockwise from right: 0 right, 1 up-right,
  2 up, 3 up-left, 4 left, 5 down-left, 6 down, 7 down-right.
- **Reflector directions** `r` are 16-way, 0–15. The mirror line is at `r × 22.5°`. `r` and `r + 8`
  have the same line, with the front on opposite sides. In the sprites, the bulb is on the back.
  `r` = 0 is a horizontal mirror with its front up; `r` = 4 is a vertical mirror with its front to
  the left.

## The laser (visual type 11)

### Set-up (`0x800EAF28`) [certain]

The laser is made by its parent (a beamer or a reflector) with `GetVisualObject`. It copies the
parent ("copies the parent"), keeps the parent at `+0x7C`, its direction at `+0x84`, and its shot id
at `+0x90`. The parent's `+0x89` = 1 (a laser is live). At set-up:

- life `+0x8A` = **182 updates**; contact damage 1; attack box `0x800FBF9C` = (−8, −8, 16, 16), a
  16 × 16 box at the head; no hurt box (weapons cannot hit it); draw priority 6;
- the start point (`+0x92`, `+0x94`) = the spawn point;
- speed: **16 px per move** in the direction; for the diagonals 16 px on each axis;
- one **probe** (weapon type 79, `GetWeaponObject` `0x8002C32C`): its parent is the laser, it has
  the same direction, and its `+0x8C` = the laser's shot id. The laser keeps it at `+0x8C`.

When the parent has no live link at that moment (see "Life"), the laser goes to its end state at
once.

### Each update (state 1, `0x800EB0F0`) [certain]

1. **Life test.** The laser lives while its parent's `+0x89` is not 0 and the parent is not in
   state 4. Else it goes to the end state.
2. **Pin.** It reads `+0x98` of its probe into its own `+0x98` and clears it in the probe. When
   the value is 2, the head moves to the probe's position (a part has caught the laser; below).
3. `ContactMega`: **1 damage** to the player when the head box touches the player.
4. **Timer.** `+0x8A` − 1. At 0: parent `+0x89` = 2 ("laser timed out") and the end state.
5. **Move.** The head moves (`MoveObject`) only when all are true:
   - the tile at the head is not a wall: collision value 0 or 37–47 (`0x80030234`); any other value
     stops the head there;
   - the move counter `+0x88` is 0 (it counts 0, 1, 2, 0, … each update), so the head moves on 1
     update of 3;
   - `+0x98` ≠ 2 (not pinned in this update).

   Then x and y are kept at 0 or more.
6. **Draw.** Three misc type 7 objects for this frame only: an end sprite at the start point
   (sprites `0x800FBCB0`[direction] + `+0x96`, priority 5), the body from the start point to the
   head (`var` 0, priority 6), and an end sprite at the head (`0x800FBCA0`[direction] + `+0x88`,
   priority 4). `+0x96` counts 0–4. So the two end sprites cycle through 5 and 3 frames.

Speed: 16 px per 3 updates, **5⅓ px/frame** on average. The head moves on update 1, 4, 7, …, 181
of its life: at most **61 moves = 976 px** (on each axis for a diagonal). The laser is a line from
its start point to its head: its length grows until the head stops.

### End (state 2, `0x800EB734`) [certain]

No damage and no movement. The laser is drawn only on odd frames (bit 0 of `frame_count`), so it
flickers, for **14 updates**. Then state 3 deletes it (`DeleteObject3`).

### The probe (weapon type 79, `0x80044554`) [certain]

Set-up `0x800444B0`: attack box `0x80076148` = (−8, −8, 16, 16), no damage. Each update it copies
the position of its laser while the laser is in state 1. When the laser leaves state 1: box 0,
`+0x98` = 0, then `DeleteObject3`. Its weapon id is its type, 79 (`ApplyWeaponHit` reads byte 1).
`dmg_tbl_reflector` and the standard tables give id 79 class 2 ("no effect"), and class 2 makes
`ApplyWeaponHit` return 0. So the probe does not turn reflectors and does not hurt enemies with the
standard tables [certain for those tables].

### Can the laser hurt the player? [certain]

Yes, but only the **head** box (16 × 16 px). The body and the end sprites have no contact test.
Base damage **1**, hit class 0 (the light hit reaction). After `PlayerDamageScale` it is 1 on all
difficulties (the minimum is 1). The player's invincibility after a hit stops more hits for that
time. A head that a part has caught stays at the catch point, so that point stays dangerous while
the laser lives.

## How the parts catch a laser (`0x800EAA00`) [certain]

The relay beamer, both reflectors and the door call this function each update. It scans the 32
weapon slots for type 79. It skips a probe whose shot id is the part's own `work.84` (a reflector
does not catch its own laser). It tests `BoxOverlap(probe, part)`: the probe box against the part's
hurt box. For the first probe that overlaps:

- When the probe's `+0x98` is already set (another part caught it in this update), it gives "no
  hit".
- When the part's `work.8A` is set (see "Re-fire"), it writes `+0x98` = 1 (no pin) and clears
  `work.8A`.
- Else it writes `+0x98` = 2 and moves the probe to the **catch point**:
  - beamer: its centre;
  - reflector: the offset from the reflection table (below); the centre when the laser comes from
    the back;
  - door 0–3: door + (−8, −30); door 4–5: door + (30, −8) (the centres of the sensor boxes).
- It keeps the laser's direction in `+0x86` and gives "hit".

## The reflection rule [certain]

Table `0x800FBCC0` (16 reflector directions × 8 laser directions) gives an index into
`0x800FBD40` (6 bytes: out direction, 0, s16 dx, s16 dy). The rule in the table is:

- **out = (r − in) mod 8** when the laser comes onto the front of the mirror. The new laser starts
  at reflector + (dx, dy). The old head is caught at the same point.
- When out = in + 4 (the laser comes straight onto the front): the head is caught at the offset,
  and the table's out direction is 255: **no new laser**.
- When the laser comes onto the back, or along the mirror line: the head is caught at the
  reflector's centre; no new laser.

Example (the save state): reflector `0x3F` (`r` 15) at (3080, 408). The beamer above fires down
(in 6). out = (15 − 6) mod 8 = 1 (up-right), offset (0, −14). The new laser starts at (3080, 394) and
goes up-right to the sensor of door 0.

| `r` | lasers it reflects (in → out) | in, caught with no new laser |
|---|---|---|
| 0 | 5 → 3, 7 → 1 | 6 |
| 1 | 0 → 1, 5 → 4, 6 → 3, 7 → 2 | – |
| 2 | 0 → 2, 6 → 4 | 7 |
| 3 | 0 → 3, 1 → 2, 6 → 5, 7 → 4 | – |
| 4 | 1 → 3, 7 → 5 | 0 |
| 5 | 0 → 5, 1 → 4, 2 → 3, 7 → 6 | – |
| 6 | 0 → 6, 2 → 4 | 1 |
| 7 | 0 → 7, 1 → 6, 2 → 5, 3 → 4 | – |
| 8 | 1 → 7, 3 → 5 | 2 |
| 9 | 1 → 0, 2 → 7, 3 → 6, 4 → 5 | – |
| 10 | 2 → 0, 4 → 6 | 3 |
| 11 | 2 → 1, 3 → 0, 4 → 7, 5 → 6 | – |
| 12 | 3 → 1, 5 → 7 | 4 |
| 13 | 3 → 2, 4 → 1, 5 → 0, 6 → 7 | – |
| 14 | 4 → 2, 6 → 0 | 5 |
| 15 | 4 → 3, 5 → 2, 6 → 1, 7 → 0 | – |

All other (`r`, in) pairs: caught at the centre, no new laser. The offsets (6 to 24 px from the
centre) are in the table; most put the start 13–16 px from the centre on the out side.

## The beamers (state 1, `0x800EC9B8`)

Each update: `AnimeTick`, the step (table `0x800FBF34`), then for `var` `0x2n` only the catch test
(a catch sets step 4), then `DisplayCheck(16, 16)`. Shots: `0x800EABD8`, sound bank 2 id 19. The laser
starts 16 px from the centre in the direction (12 px on each axis for a diagonal) [certain].

### Fixed beamer (`0x0n`, step 3, sub-steps `0x800FBF68`) [certain]

| sub | what |
|---|---|
| 0 | timer = 3 |
| 1 | after 3 updates: fire in direction `n` |
| 2 | wait until the laser timed out (`+0x89` = 2); then `+0x89` = 0, timer = 120 |
| 3 | after 120 updates: sub 0 |

So the laser is on for 182 updates and off for about 125 [likely ±2 for the update order]. The
direction never changes.

### Aiming beamer (`0x1n`, step 4, sub-steps `0x800FBF78`) [certain]

| sub | what |
|---|---|
| 0 | `+0x89` = 2 → 0. While a laser is live (`+0x89` = 1) it waits here. Else: when the player's x is within **144 px** of the beamer's x, **turn one step** toward the player; animation `8 + dir`; next sub. |
| 1 | Fire in the current direction (unless the relay rule below stops it); timer = 302 |
| 2 | `+0x89` = 2 → 0. After 302 updates: sub 0 |

The turn: `AimAt(beamer, player)` gives a 32-way direction; table `0x800E987C` rounds it to 8 ways
(the straight directions take 5 of the 32 values, the diagonals 3). When it differs from the
current direction, the beamer turns by 1 (45°) on the shorter side; at exactly 180° it turns −1
(clockwise). So the beamer **turns and fires once every 304 updates** (about 5 s), and the laser is
on for 182 of them. The player controls it by where they stand when it turns. When the player is
farther than 144 px in x, it fires without a turn.

### Relay beamer (`0x2n`) [certain]

Step 2: it does nothing until its sensor (−8, −8, 16, 16) catches a laser (the laser stops at its
centre). Then step 4, for ever: the same cycle as the aiming beamer, but with animations 0–7 (the
other colour). In sub 1 it **does not fire when it points straight back** along the last laser that
hit it (`+0x86` = direction + 4, mod 8 by the two tests of `0x800ECD40`). The fixed and aiming beamers
have no sensor: lasers pass through them.

## The reflectors (state 2, `0x800EA4CC`)

Each update, in this order:

1. `AnimeTick`; the step (table `0x800FBF48`): step 2 = `0x800ECDF4` (**re-fire**, below).
2. `DisplayCheck(16, 16)`.
3. HP = 127, then `ContactWeapon` (the **turn**, below).
4. The catch test: on a catch while no laser of its own is live, `work.8C` = 1 (fire in the next
   update); when nothing is caught, `+0x89` = 0, so its own laser ends.
5. The damage table timer.

### Re-fire (step 2) [certain]

- `+0x89` = 0 and `work.8C` = 1: fire a new laser (`0x800EADA8`) with the out direction and the offset
  of the reflection table for (`r`, `+0x86`); no laser when the table gives "no new laser". Sound
  bank 2 id 18. `work.8C` = 0.
- `+0x89` = 2 (its laser timed out): `+0x89` = 0, `work.8A` = 1. When the incoming laser is still
  there, the reflector fires a new laser one update later.

So a reflected laser **lives only while the incoming laser touches the reflector** (and at most
182 updates). When the incoming laser ends or moves away, the reflected laser ends in the next
update. A chain of reflectors goes out from the end, one link per update.

### The turn (`0x4n` only) [certain]

`ContactWeapon` gives a non-zero result for a player weapon that hits the hurt box with class 0
(damage) or class `0xFF` (deflected) in the current table. Then:

- the damage table becomes `0x800745BC` (`dmg_tbl_reflector_wait`) for **33 updates**
  (`+0x8B`; every new hit sets it to 33 again);
- `0x4n` only: when its own laser is live, `work.8C` = 1 and `+0x89` = 0 (the old reflected laser
  ends; the new one starts in the next update, in the new direction); sound bank 2 id 78; the list
  index `+0x87` + 1 (mod 8); `r` = the new list entry; new animation `16 + r` and hurt box
  `0x800FB968[r]`.

Which hits count:

| table | when | ids that turn the reflector |
|---|---|---|
| `dmg_tbl_reflector` (`0x8007451C`) | normal | 0–63 except 7 and 23 (class 2); ids 64–78 too (deflected); not 79 (the laser) |
| `dmg_tbl_reflector_wait` (`0x800745BC`) | 33 updates after a counted hit | only 0 (lemon), 9 (half charge), 18 (full charge) |

So each X-Buster shot turns it one step. A sabre swing turns it once, and the next sabre hit counts
only after 33 updates. Because one shot hits an object only once (`docs/engine.md`, "Damage to an
object"), a multi-part charge shot turns it once per part that the table accepts [likely]. HP is set
to 127 again each update, so a reflector cannot be destroyed. A fixed reflector (`0x3n`) changes
its damage table but does not turn.

### Turn lists [certain]

`0x800FBAE0[n]` gives the list number and the first index; the lists are at `0x800FBB60` (8 entries,
32 bytes each). The order of `r` after each hit, from the first entry:

| `n` | list | `r` in order (repeats) |
|---|---|---|
| 0 | 3 | 9, 5, 1, 13 |
| 1 | 1 | 15, 13, 11, 9, 7, 5, 3, 1 |
| 2 | 2 | 12, 8, 4, 0 |
| 3 | 6 | 11, 10, 9, 8, 7, 8, 9, 10 |
| 4 | 2 | 0, 12, 8, 4 |
| 5 | 1 | 9, 7, 5, 3, 1, 15, 13, 11 |
| 6 | 2 | 0, 12, 8, 4 |
| 7 | 0 | 8, 6, 4, 2, 0, 14, 12, 10 |
| 8 | 0 | 0, 14, 12, 10, 8, 6, 4, 2 |
| 9 | 5 | 3, 15, 11, 7 |
| 10 | 7 | 0, 14, 10, 9, 8, 6, 4, 2 |
| 11 | 8 | 12, 7, 3, 15, 10, 7, 3, 15 |
| 12 | 0 | 0, 14, 12, 10, 8, 6, 4, 2 |

Most lists turn the mirror 45° (−2) or 90° (−4) clockwise per hit; list 6 swings back and forth.
`n` 13–15 give list 0 from index 0 (not used by a record).

## The door (state 3, `0x800ECA5C`)

Each update: the step (table `0x800FBF54`), `DisplayCheck(48, 48)`, the catch test. The animation
does not run before the door opens.

| step | what |
|---|---|
| 2 | Closed. When the sensor catches a laser: sound bank 2 id 32, step 3. **One catch opens it.** |
| 3, sub 0 (`0x800EA650`) | `AnimeTick` until the frame with flag 1 (19 updates): animation 33 (`n` 0–3) shows the door going up, animation 32 (`n` 4–5) going to the side. Then it copies tiles (below) and raises a camera limit. |
| 3, sub 1 | step 4 |
| 4, sub 0 (`0x800EA7C8`) | Wait until the camera has passed the door: layer-0 camera x ≥ door x + 16 (`n` 0–3), or y ≥ door y + 16 (`n` 4–5). Then it deletes the puzzle behind it and locks the camera (below). |
| 4, sub 1 | nothing, for ever |

**Opening** (`CopyTiles(0, dx, dy, src)`, cells of 16 px [likely]): the door's tiles are replaced by
tiles from a store area of the map at y 144–207 (`n` 0–3) or another place; the door is then open in
the terrain, and it stays open while the area is loaded [certain for the copy; the content of the
store cells is likely, not drawn]. The camera limit that it raises is layer-0 `+0x24` (x maximum,
`n` 0–3) or `+0x28` (y maximum, `n` 4–5), as named in `docs/enemies/teleport-crystal.md`.

| `n` | door (record) | tiles: to (dx, dy) ← from, cells | camera limit after opening |
|---|---|---|---|
| 0 | 3152, 360 | 3136, 336 ← 3136, 144, 2 × 4 | x max 3488 |
| 1 | 3808, 408 | 3792, 384 ← 3792, 144, 2 × 4 | x max 3824 |
| 2 | 4144, 360 | 4128, 336 ← 4128, 144, 2 × 4 | x max 4480 |
| 3 | 4800, 376 | 4784, 352 ← 4784, 144, 2 × 4 | x max 4832 |
| 4 | 4728, 464 | 4688, 448 ← 4688, 176, 4 × 2 | y max 512 |
| 5 | 3928, 720 | 3888, 704 ← 3584, 704, 4 × 2 | y max 768 |

**After the camera passes** (step 4): every main type 13 object whose origin is in the rectangle
below goes to state 4 (`work.7E` = 2: after 2 updates `DeleteObject3`). `DeleteObject3` leaves the
records' "spawned" bit set, so these parts do not come back while the area is loaded [likely]. The
camera minimum is set so that the player cannot go back.

| `n` | delete rectangle (x0–x1, y0–y1) | camera lock |
|---|---|---|
| 0 | 2896–3167, 256–447 (room 0 and this door) | x min = door x + 16 |
| 1 | 3168–3823, 256–447 | x min = door x + 16 |
| 2 | 3824–4159, 256–447 | x min = door x + 16 |
| 3 | 4160–4815, 256–447 | x min = door x + 16 |
| 4 | 4160–4783, 256–479 | x min 3840; y min = y max = door y + 32; also copies 5 × 2 cells from (4592, 448) to (4688, 448) |
| 5 | 3856–4783, 480–735 | y min = y max = door y + 32; also copies 5 × 2 cells from (4592, 448) to (3888, 704) |

Doors 4 and 5 copy tiles again after the pass and close the hole behind the player [certain: in
`sheldon-15` all 10 cells of the door 4 gate are collision 56 after the pass, and the camera is
locked at y 496]. The gate on the disc is 5 × 2 cells (59 at x 4688–4767, y 448–479; 57 at x 4672
and 4768), but the opening copy is 4 × 2, so x 4752–4767 stays 59 (solid) and the hole is 64 px
wide (`sheldon-14`). The same for door 5 (59 at x 3888–3967, y 704–735). `sheldon-13_door-open`:
door `0x50` in state 3 step 4, camera x 2961 (< 3168), max x 3488; the laser is still on the sensor
at (3144, 330).

## How the objects find each other [certain]

- **Records.** All the parts are records of the stage area (type 13, `id` = `var`).
- **The door spawns the next room.** At set-up, door `n` calls `SpawnInRect(x0, x1, y0, y1, 0)` with
  rectangle `0x800FBEF8`: door 0 → 3168–3791 × 256–447, door 1 → 3824–4127 × 256–447, door 2 →
  4160–4783 × 256–447, door 4 → 3856–4783 × 480–703, door 5 → 3856–4767 × 736–991; door 3 spawns
  nothing. So the parts of the next room are live (and firing) before the player sees them. In the
  save state the two aiming beamers at x 3360 and 3600 are live for this reason, 300 px off screen.
- **Parent link.** A laser keeps its parent at `+0x7C`; the parent's `+0x89` is the link: 0 none,
  1 laser live, 2 laser timed out. The laser keeps its probe at `+0x8C`.
- **Catch.** The parts find a laser only through its probe in the weapon pool (type 79), with
  `BoxOverlap` and the shot id test. They write the result into the probe (`+0x98`, position); the
  laser reads it in its next update.
- **Turn.** A player weapon reaches a reflector through the standard `ContactWeapon`.

## Animations

Table `0x800FBA58` (34 animations; export `enemy-07d032a495`).

| # | sprites | use |
|---|---|---|
| 0–7 | 2 1 0 7 6 5 4 3 (one each, held) | relay beamer, direction 0–7 |
| 8–15 | 10 9 8 15 14 13 12 11 | fixed and aiming beamer, direction 0–7 |
| 16–31 | 20 19 18 17 16 31 30 … 21 | reflector, `r` 0–15 |
| 32 | 32 33 34 35 36 (1, 6, 6, 6, 1 ticks; flag 1 on the last) | door 4–5 opening |
| 33 | 37 38 39 40 41 (same ticks and flag) | door 0–3 opening |

## Laser sprites [certain]

The laser does not use the table above. Its sprites are in a second sprite source: **entry 16** of
`sprite_bank_base` (the word at `+0x40`), after the 16 banks of the type list. It has **250
sprites**. The beamer or reflector that fires the laser (`0x800EABD8`, `0x800EADA8`) gives the
laser this sprite data (`+0x3C`), its own texture (`+0x40`, the texture of the type 13 bank) and
its own palette (`+0x42` without bit 15). Each draw piece (misc 7) copies the three fields from the
laser. So the laser is drawn with the texture and the palette of the puzzle parts.

Each piece sets its sprite byte (`+0x47`) and its `flip` (`+0x15`) from a table by laser
direction `d` (0–7). Each table entry is 2 bytes: {first sprite, flip}.

| piece | table | first sprite for `d` = 0 … 7 | sprite |
|---|---|---|---|
| head (`var` 1, priority 4) | `0x800FBCA0` | 210 213 222 213 210 216 219 216 | first + `+0x88` (0–2) |
| start (`var` 1, priority 5) | `0x800FBCB0` | 225 230 245 230 225 235 240 235 | first + `+0x96` (0–4) |
| body, a full piece of 224 px | `0x800FBC90` | 39 81 207 81 39 123 165 123 | first + phase |
| body, the last piece | `0x800FBC80` | 0 42 168 42 0 84 126 84 | first + phase + 3 × (⌈length / 16⌉ − 1) |

- `flip` is `0x40` for `d` 0, 1 and 7, else 0. So the right-facing directions are the mirrored
  left-facing sprites: 0 and 4 use the same sprites, as do 1 and 3, 5 and 7. In the draw, an object
  with `flip` `0x40` is mirrored about its x.
- The body (`0x800EBCD8`): the phase is the laser's `+0x88` (0–2, copied to the piece `+0x56`).
  The line from the start point to the head is cut into full pieces of 224 px (each one a new misc
  7 object, `0x800ECEE4`) and one last piece. The last piece has a length of 1–14 cells of 16 px
  (the rounding of the length was not read in full [likely]).
- So the sprites are: horizontal bodies 0–41, diagonal bodies 42–83 (up) and 84–125 (down),
  vertical bodies 126–167 (down) and 168–209 (up) (14 lengths × 3 phases each), head 210–224,
  start 225–249.

**Export.** `spriteexport` finds this source through the live misc 7 and visual 11 objects (see
`README.md`, "extra sprite source") and writes a sprite-only export: `enemy-e5920c31ed`,
`sprites.png` with all 250 sprites (cell 230 × 230, origin 115, 115, 8 columns), from
`sheldon-7_lasers`, `sheldon-8_lower-1` and `sheldon-11_laser-shot` (stage 22; the same ID, so the
same sprites). The numbered sheet is `banks/<stage>/<state>/entry-16-live-0.png`. All 250 sprites
draw from one save state. A drawing of every live piece at its position, with its flip and palette,
gives one closed line from each beamer to its head and from each reflector to the next part, in all
three states.

## Sounds

Bank 2: id 19 beamer shot, id 18 reflected laser, id 78 reflector turn, id 32 door sensor hit.

## Summary for a reimplementation

1. Lasers: 8 directions; the head moves 16 px every 3 frames; it stops at a wall tile; life 182
   frames, then 14 frames of flicker; only the 16 × 16 head hurts the player, 1 damage.
2. Fixed beamer: 182 frames on, about 125 off, one direction. Aiming beamer: every 304 frames,
   turn 45° toward the player (when within 144 px in x), then fire. Relay beamer: like the aiming
   beamer after a laser hits it once; it does not fire straight back.
3. Reflector (16 directions, mirror line at `r × 22.5°`): a laser on the front makes a new laser
   with out = (r − in) mod 8; head-on, back or edge hits stop the laser. The new laser lives while
   the incoming laser touches the reflector.
4. Turning reflector: each player weapon hit moves it to the next entry of its 8-entry list;
   after a hit, for 33 frames only the X-Buster ids 0, 9, 18 count. It cannot be destroyed.
5. Door: one laser on its 16 × 16 sensor opens it (19 frames of animation, then the tiles change
   and the camera limit opens). When the camera passes it, the room behind is deleted and the
   camera is locked.
6. Each door spawns the next room's parts when it spawns.

## Open questions

1. The update order of the main, visual and weapon pools, which changes the exact on/off timing by
   1–2 frames, and which part wins when two parts catch one probe in one update.
2. What the store cells at y 144 (and the other sources) look like. The copied cells make the
   passage collision 0 in all 8 cells (doors 0–3) and in 4 of 5 columns (doors 4, 5) [certain].
3. Whether the opened doors stay open after a death and respawn. The copies are in the screen data
   in RAM (`CopyCell`); within one life they stay (in `sheldon-14` and `sheldon-15` all earlier
   doors are collision 0).
4. The rounding of the last body piece length (`0x800EBCD8`, `0x800EC76C`) was not read in full.
   The sprites are exported (section "Laser sprites").
5. Whether the laser probes count against X's shot limit (they are in the weapon pool).
6. Whether enemy damage tables other than the standard ones (for example a table with id 79 = 127,
   as `dmg_tbl_yammark`) make the laser hurt an enemy in this stage.
7. Play checks: the 304-update turn cycle of the aiming beamer; the 33-update sabre wait; the
   on/off time of a fixed beamer; 1 damage from the head.
