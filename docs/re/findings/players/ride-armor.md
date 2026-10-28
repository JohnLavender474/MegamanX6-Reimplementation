# Ride Armor (Metal Shark Player's stage): behaviour sheet

The mech suit that X (or Zero) can get into in Metal Shark Player's stage (stage 4, area 0). The
code is in the stage overlay (06); overlay 28 has a copy of the same code. The player side (step
18) is in `docs/players/ladder-hurt-events.md`.

Source: the save state `shark-1_ride` (X in Falcon Armor rides; ride step 1, HP 32), the X kit of
that state (`python3 -m mmxdemake.rekit --player --state shark-1_ride.state.gz`), a rekit walk
from the ride entry, and a disassembly of the main-program users. Notes, pseudo code,
reconstructed C, kits, probes and the ride's animation strips are in [`~/mmx-data/x6/re/ride-armor/`](../../work/ride-armor/).
Units and tags as in `docs/players/movement.md`: speeds in px per update (16.16), times in
updates (60 per second), a positive `speed_y` is up. All values are [certain for the code] unless a
line says otherwise; the feel needs a play check.

## Is the mech a "fixture" attached to X?

No. The Ride Armor is its own object with its own body: one fixed slot, `actor2` (`0x800CD340`,
`0xB0` bytes), with its own position, speeds, terrain box, hurt box and HP. While X rides, **the
ride is the body and X is a passenger**: the ride reads the pad, moves, collides and takes the hits,
and at the end of its update it copies its x, y and facing into X. X keeps running its own update,
but in step 18 X only plays a head-only animation that the ride chooses; X has no terrain box, no
hurt box, and does not shoot.

In engine terms, it is closer to "the player controls a second entity, and the player entity is
parented to it with physics off" than to a fixture on X's body. The game links the two by two
bytes in X (`mega+0xD5` ride state, `mega+0xD8` ride animation) and one bit in the ride (`+0x97`
bit `0x40`, "X inside").

## Objects and memory

| what | where | note |
|---|---|---|
| the ride | `actor2`, `0x800CD340`, `0xB0` bytes | one fixed slot, not in a pool. Head in the state: `41 00 00 01 01 01 01 00` (flags `0x41`, type 0, var 0, display, state 1, step 1, sub-step 1) |
| type table | `actor2_types`, `0x80071380` | 2 words: type 0 → `0x800F2414` (overlay 06), type 1 → `0x800F01A4` (overlay 28). The 3 words after them (`0x80020F24`, `0x80020F8C`, `0x8002103C`) are another table, `0x80071388`: `Actor2Update` (`0x80020A5C`) indexes `0x80071380` by the type, but `0x80020EF0` and `0x80021354` index `0x80071388` by an object's sub-step (`+0x05`) and call it with (object, parent). These three functions copy the parent's previous position (`+0x18`, `+0x1C`) after a delay and read the parent's `+0x8C` [certain for the users; the object that uses them is not known: an afterimage or trail is a guess]. So actor2 has 2 types, both a Ride Armor |
| update | `Actor2Update`, `0x80020A08` | each frame **after the input and before the player** (`0x80020C40`: … actor2, `0x80034DCC` player, `WeaponLoop` …) |
| spawner | stage logic type 30, `RideSpawner` `0x800EE158` | record at `0x800F2B1C`: id 0, (1872, 395). Writes actor2 flags `0x41`, type = var = record id, x, y; then `DeleteObjectKeep`: the record never spawns again while the area is loaded |
| sprites | sprite type 225 (bank 3 in this state, 77 sprites) | animation table `0x800F3D4C` (31 animations) |
| main type 1 in the state | Junkroid | not the ride |

Overlay 28 is **stage 20 area 1, the hidden area ("Another Route") of this stage** [certain for the
code and the records]: the teleporter crystal `id` 1 of area 0 asks for exit `0x41`, and the stage
exit code turns `0x41` in stage 4 into stage 20, area 1 (`docs/stages/shark.md`, "The teleporter
crystals"); `overlay = 25 + 2 × (stage − 19) + area` = 28, and the record lists of stage 20 area 1
(`enemy_set_tbl`, `fixed_set_tbl` + 164) are valid lists in overlay 28. Its first list has the Ride
Armor place (stage logic 30, `id` 1, at (32, 354)), so `actor2` type 1. It also has the Nightmare
Press (`docs/enemies/nightmare-press.md`, "Other users"). The Fandom page of the stage says that the
Ride Armor is the safe way through the press of the Another Route [Fandom, "Recycle Lab"]. Overlay
28 has the same 35 functions with the same sizes. The code does not name the model.
X6 has one Ride Armor model, called "Rabbit" in other sources [guess: not in the code].

## Who owns what while X rides

| | the ride (`actor2`) | X (`mega`) |
|---|---|---|
| pad input | copies `mega+0x7C` (held) and `+0x80` (pressed) into `+0x88`, `+0x8A` each update (0 during an event) | its own requests only (exit, events) |
| position | moves, then `CollideCheck` with its terrain box (centre (0, 3), half 15 × 29) | x, y, `flip` = the ride's, written by the ride each update |
| terrain box | `0x800F3A44` | none (`PlayerSetTerrainBox` gives 0 while `+0xD5` < 0) |
| enemy contact | hurt box (−8, −14, 16, 46); `ContactMega` tests it instead of X | not tested |
| solid objects | pushed and carried (`SolidObject`, `SolidPush(solid, actor2)`) | not (no terrain box) [likely] |
| items, souls | the overlap test uses the ride | the refill goes to X |
| attacks | its own punches (weapon objects 61, 62) | no shot, no charge, no weapon select (the tries test `+0xD5`) |
| drawing | its own sprite (priority 3), blinks after a hit | step 18: animation `+0xD8` (41–57, head only, priority 2) |
| HP | 32 | not changed by hits; dies only with the ride (crush, pit) |

## Each update (`RideEntry` `0x800F2414`, `RideMain` `0x800F1EC4`)

1. Input copy; `prev` = position; `+0xA8` = `collide_flags` | object push (`+0x71`, then
   cleared).
2. Off screen by more than 64 px: nothing more (frozen, not drawn).
3. Crush test (below), then a waiting hit (below).
4. Leave request: exit request (`game.exit_req`) with X inside → step 11.
5. Double-tap dash, dash button, charge (below).
6. The step (table `0x800F3A74`), the knockback move, `CollideCheck`.
7. **X inside (`+0x97` bit `0x40`): `mega.x = ride.x`, `mega.y = ride.y`, `mega.flip =
   ride.flip`.** The blink, the 0-HP countdown, display.
8. Camera limits: x stays within `cam.min_x + 32` … `cam.max_x + 288` (the left limit only when
   not blocked on the right). Below `cam.max_y + 272` the ride is removed and X dies.
9. HP bit 7 cleared; `game.stage_flag` = 1 while the ride touches a ceiling and a floor at once.

Then the player runs: step 18 calls only `AnimeTick` (no `PlayerMove`), so X stays at the copied
position. X is never more than the ride's update behind.

## Boarding (step 0 → 10 → 1)

The empty ride stands (animation 17; it falls with step 12 when there is no floor). Each update it
tests, in order: no event (`mega+0xD0` = 0), no exit request, Zero only: `mega+0x8E` = 0,
`mega+0x7A` = 0, `mega+0xE0` = 0, `+0x96` & `0x8200` = 0, X's terrain box overlaps the
**boarding box** (centre (0, 22), half 15 × 10: the lower part of the ride), X HP > 0, `mega+0xA4`
= 0 (no hit reaction or blink), `mega+0x79` = 0 (no spike). Then:

- ride step 10 (animation 18, X animation 57, sound (2, 36); a flag frame makes effect 12 var 7 and
  sound (2, 38)), `+0x97` |= `0x40`;
- `mega+0xD5` = 1 (2 when the object is not actor2).

The next player update (`PlayerRideCheck`): dash and shot flags cleared, charge stopped, no
terrain box, latch 1, step 18, `+0xD5` = −1. At the end of animation 18 (59 updates) the ride
goes to step 1 with its hurt box on.

So X boards by touching the low part of the ride, from either side or from above; no button is
needed. X and Zero can both board.

## Controls (X inside)

| input | step | what | numbers |
|---|---|---|---|
| left / right | 2 walk | faces and walks | 1.375 px/update; footstep sound on flag frames |
| jump | 4 jump | dust effect, sound (2, 39) | `speed_y` 5.5, gravity 0.2578. Rises while jump is held: up to 22 updates, about 61 px. Release, ceiling or the top → step 12 with `speed_y` = 0 |
| (air) | 12 fall | | gravity 0.2578, fall speed max 5.875 |
| left / right in the air | 4, 6, 12 | air control | 1.375 px/update, or 4.125 after a dash (`+0x98`) until the landing |
| dash, or double tap (12-update window) | 7 dash | from stand, walk or land; ground only | 4.125 px/update for up to 32 updates (about 132 px); stops at a wall, when the forward direction is let go (with the dash button: when both are let go), or at the end. Dash effect 12 var 5 |
| jump in the dash | 4 | dash jump | air speed 4.125 |
| buster or weapon (press) | 3 punch | 2-punch combo | punch 1: weapon 61, animation 8, 23 updates; press again in its flag frames → punch 2: weapon 62, animation 10, 37 updates. Punch 2 has no queue frame, so the combo stops at 2 |
| attack in the air | 6 air punch | | weapon 62 var 3, animation 11; air control and fall go on |
| attack in the dash | 9 dash punch | | weapon 62 var 1, animation 9; the dash goes on to its end |
| hold buster or weapon 60 updates | 13 charge | wind-up while held (animation 12), from stand or walk | on release: charged dash punch, weapon 62 var 4, 4.125 px/update for 30 updates (about 124 px), stops early when the opposite direction is held |
| up + jump | 11 leave | from stand, walk, jump, land, dash, dash end, fall, charge | sound (2, 37) |

Damage of the punches (`dmg_tbl_normal`, the standard enemy table): id 61 = 5, id 62 = 15. The
weapon-62 punches hit again every 6 updates while their box is live. The boxes come from the
ride's frame flags (`0x80076254`; the largest, the charged punch, reaches 106 px forwards).

Odd case [certain for the code; play check]: in the dash end (step 8), a held left or right starts
a punch (step 3), not a walk.

What X cannot do inside: shoot, charge the buster, change weapon, dash on foot, climb, wall-kick,
take hits, pick up items with its own box.

## Leaving (step 11)

Up + jump, or X's death, or an exit request. The ride: `+0x61` = 1 (the empty ride can no longer
be hit), `mega+0xD5` = 0, bit `0x40` off, X's terrain box back. Animation 19 on the floor (step 12
in the air), then step 0: X can board again. The next player update (`PlayerRide`): terrain box on,
`StartJump` (a normal jump out of the cockpit, `docs/players/air.md`), `+0x86` = 1 (no air dash),
the lock off.

## Damage, HP and destruction

| rule | what |
|---|---|
| HP | 32. Only enemy contact (`ContactMega`) hurts it; X's shots pass through it (no damage table) |
| a hit | full contact damage (no difficulty or armour scaling); hit class 1 (damage < 5) or 2 |
| after a hit | knockback: 4.0 px/update backwards (opposite to the facing), slowing by 0.34375 per update, about 11 updates and 25 px; then 121 updates of blink, no new contact hit (`+0x85`) |
| X | not hurt by any contact while inside |
| HP 0 | HP back to 32 at once; a countdown of 494 updates (about 8.2 s) starts: one-update flashes, faster and faster (30, 30, 29, … 2 updates apart). Riding goes on. Hits still knock back but cannot end it sooner |
| end of countdown | X is ejected (`+0xD5` = 0: X jumps out), 7 debris pieces (animations 24–30), 30 updates of explosions, then the slot is cleared (`RideRemove`; the record stays used: no new ride in this area load) |

## Crush, spikes and pits while riding

- **Screen-edge crush:** X inside, the ride blocked on its right side (terrain or an object push)
  and less than 32 px from the left edge of the screen → X dies at once (`mega.hp` = `0x80`), the
  ride explodes (8 explosions) and is cleared. This is the case of a wall or object that pushes
  the ride against the left screen edge.
- **X's own crush and squeeze** (`PlayerHitCheck`) need X's terrain box, which is off: they do not
  act while riding [likely].
- **Ceiling and floor at once:** no damage; only `game.stage_flag` = 1 (read by the teleport
  crystal, main type 28).
- **Spikes:** X's spike death needs `mega+0x79`, set by X's own tile test, which has no box while
  riding: the ride walks on spikes [likely; play check].
- **Pit:** the ride below the camera limit + 272 px → removed, X dies.
- Solid objects push and carry the ride like the player (`SolidObject`).

## Steps

| step | routine | name | ride animation | X animation |
|---|---|---|---|---|
| 0 | `0x800EE924` | empty, board test | 17 | — |
| 1 | `0x800EEC50` | stand | 0 | 41 |
| 2 | `0x800EEE00` | walk | 1 | 42 |
| 3 | `0x800EEFF0` | punch 1 / punch 2 | 8 / 10 | 49 / 50 |
| 4 | `0x800EF288` | jump | 2 | 43 |
| 5 | `0x800EF524` | land | 4 | 45 |
| 6 | `0x800EF758` | air punch | 11 | 51 |
| 7 | `0x800EF950` | dash | 5 | 47 |
| 8 | `0x800EFC4C` | dash end | 7 | 48 |
| 9 | `0x800EFDB0` | dash punch | 9 | 55 |
| 10 | `0x800EEACC` | board | 18 | 57 |
| 11 | `0x800EFFCC` | leave | 19 | — |
| 12 | `0x800F011C` | fall | 3 | 44 |
| 13 | `0x800F26E4` | charge, charged punch (sub table `0x800F3A5C`) | 12, 15, 16 | 52, 54, 41 |

A second table (`0x800F3AAC`, 18 steps) is used when the ride's `var` ≠ 0: it fires shots (weapon
types 59 and 60), and has an air dash (15) and steps 14, 16, 17. The init always sets `var` = 0, so
this path is not reached in stages 4 and 20 [certain for the code]. In X6, `weapon_types[59]` and
`[60]` point to `0x801EE05C`, the code of the lemon (id 0) in X's overlay (in Zero's overlay that
address is inside another function), so the shots have no code of their own [certain for the table].
In Mega Man X4 the same ids are a Ride Armor shot (59) and a Ride Armor missile (60), and 61 / 62 the
punches [the X4 decompilation's names, `mmx4`; facts only]. So the `var` ≠ 0 path is a second Ride
Armor model left from X4 (a shooting model with an air dash) that X6 does not use [likely].

## Sprites

- Ride: `~/mmx-data/x6/re/ride-armor/sprites/anim-<k>.png` and `animations.json` (31 animations,
  77 sprites of type 225, cell 165 × 125, origin (111, 80)), made by a probe that uses the
  `spriteexport` functions. The bank sheet is
  `~/mmx-data/x6/sprites/banks/stage-4/shark-1_ride/bank-3.png`.
- `spriteexport` exports actor2 (`0x800CD340`) as a live source with `class` `actor2`:
  `spriteexport --state shark-1_ride.state.gz` gives `~/mmx-data/x6/sprites/enemies/enemy-e32b3dd423/`
  (31 animations, the same bytes as the probe) and the sheet
  `~/mmx-data/x6/sprites/banks/shark/shark-1_ride/actor2-0-live-0.png`. `shark-10_alt` (stage 20
  area 1) has the empty ride of type 1 at (32, 352): the same export ID and the same pixels
  (`banks/shark-alt/shark-10_alt/actor2-1-live-0.png`), texture `0x0140`, CLUT `0x788C`
  (`docs/stages/shark-alt.md`).
- X's ride animations 41–64 (head only) are in `~/mmx-data/x6/sprites/players/x-falcon/`
  (`playerexport`).

## Open items

- Bits `0x01` and `0x10` of `+0x97` [certain for the code; a search of all stores to `+0x96` and
  `+0x97` in the main program, overlay 06, X's overlay and Zero's overlay]:
  - `0x01`: the only writer is `xori 0x01` at `0x800F2154` in the ride update. The leave test
    (`0x800F2128`) goes to step 11 when `+0x97` & `0x41` = `0x41`, or when `game.exit_req` ≠ 0 and
    bit `0x40` is set; both paths flip bit `0x01`. So an exit-request leave sets it, and a leave
    with it set clears it. `0x800F2114` also reads it: when the 0-HP countdown `+0x8C` reaches 0
    while bit `0x01` is set, `mega+0x03` = 1. No other code sets it, so in play only an exit
    request sets it. Its purpose is open.
  - `0x10`: read at `0x800F05C0` and `0x800F0804`, just before the start of the charged dash
    punch (weapon 62 `var` 4, 4.125 px/update for 30 updates); no writer at all. So the charged
    punch is never blocked in X6.
- (answered) `game.stage_flag` (`game+0x26`) and the teleporter crystal: only the crystal `id` 3
  reads it, and stage 4 has only `id` 0 and 1 (`docs/stages/shark.md`). So it has no effect on the
  crystals of this stage.
- (answered, likely) The `var` ≠ 0 model: an unused X4 shooting model (see "Steps"). (answered)
  `actor2` types 2–4 do not exist: those words are another table ("Objects and memory").
- (answered) Overlay 28 is stage 20 area 1, the Another Route of this stage.

## Play checks

1. Board by walking into the ride; board by falling onto it.
2. Jump height (about 61 px) and the dash length (about 132 px); dash jump distance.
3. The 2-punch combo; a third press does nothing.
4. Hold the punch button about 1 s, release: the charged dash punch.
5. During the dash end, hold a direction: a punch starts (odd case above).
6. Walk on spikes in the ride; take a hit (X's HP must not change).
7. Bring the ride to 0 HP: about 8 s of flashes, then X is thrown out.
8. Up + jump to leave; X jumps out with a normal jump.

## States that would help

- In the ride during a jump, a dash and a punch (box and speed check).
- The ride at 0 HP (countdown running), and just after the ejection.
- The empty ride before boarding (step 0) and X just after leaving.
- Overlay 28's area with its ride (stage 20 area 1: through teleporter crystal `id` 1 of area 0).
