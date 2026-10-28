# Nightmare Cube (stage 06 and 07, stage object type 15): behaviour sheet

Nightmare Cube is a Nightmare Effect of Metal Shark Player's stage (stage 4): solid 48 × 48 px
boxes that stand on the floor and on each other. They fall when nothing is under them. There are
four kinds, by the record `id`: two kinds slide when the player pushes them with the right attack,
one kind breaks, and one kind explodes and hurts. Other shots stop on them. The objects exist only
with Nightmare Effect value 5.

The name: "Nightmare Cube" (ナイトメア・キューブ) is in the Nightmare Phenomenon table of the Fandom
page "Nightmare": caused by the Central Museum, seen in the Recycle Lab and the Laser Institute; "a
box obstacle. Its behavior when struck by Ground Dash/Sentsuizan depends on color: brown cubes are
destroyed, red cubes explode to cause damage, purple cubes slide across the floor until they hit
something, and black cubes slide only one space at a time" [Fandom, read through the MediaWiki API].
The code agrees: value 5 is the stage 5 pick (Ground Scaravich, the Central Museum), and the stage
5 pick also writes 5 to stage 7 (the Laser Institute, Shield Sheldon's stage). Earlier sheets called
these objects "falling crates".

Code in overlay `06` (area 0; entry `0x800EE050`, set-up `0x800ECBEC`) and in overlay `07` (area 1;
entry `0x800F00B4`, set-up `0x800EEC50`). The two copies have the same 1 448 instructions (the
mnemonics are the same in order; only the addresses differ) [certain]. The code tests stage 7 and
stage ≥ 19, so the same object is compiled into other stage overlays too [likely]. Not live in any
save state (`nightmare_effect` is 0 in all of them). Read with the RE kit (`python3 -m
mmxdemake.rekit --state shark-1_ride.state.gz --type 15 --class stage`, and the same with
`shark-2_section-2`); notes in [`~/mmx-data/x6/re/shark-open/`](../../work/shark-open/). Units, tags and the shared rules:
`docs/engine.md`. Records: `docs/stages/shark.md` (23 in area 0) and `docs/stages/shark-2.md` (1 in
area 1). All values below are [certain for the code] unless a line says otherwise. Addresses are
those of overlay 06.

## Spawn and set-up (`0x800ECBEC`)

- `nightmare_effect` (`0x800CD30A`) ≠ 5: `DeleteObject3`, nothing more.
- Flags `0x41`, state 1, step 2, sub-step (`+0x06`) 2 (resting). Sprite type 147 (bank 8 in
  `shark-1_ride`). Draw priority 32, layer = the player's layer, facing 0.
- HP 8, contact damage 16 (used only by the explosion of `id` 3). No attack box.
- Terrain box `0x800F3A14` (centre (0, 0), half 24 × 24): **48 × 48 px**. Hurt box `0x800F3A18` (−24,
  −24, 48, 48): the same square.
- `+0xA2` = 1: the "stackable block" mark. Junk blocks (stage object 11) have it too. The helper
  `0x800ECAB8` (`BlockAt`, proposed name) returns 56 when a point is inside the terrain box of
  another stage object with this mark, else 0. Cubes and junk blocks use it to stand on each other
  and to stop at each other.
- When its own point is inside another block at spawn: state 2 at once (deleted; `id` 3 shows one
  explosion).
- `var` 17: the object copies itself to stage object slot 79 (`0x800C44BC`) and deletes the
  original [certain for the code; why is not known; no record has `id` 17].

## The `id` parameter

| `id` | sprite | colour (bank 8 image) | damage table | pushed (below) | records |
|---|---|---|---|---|---|
| 0 | 6 | grey-black | `dmg_tbl_capsule` (no damage) | slides and slows down: one space | area 0: 4 |
| 1 | 5 | purple | `dmg_tbl_capsule` (no damage) | slides faster and faster until it hits something | area 0: 2; area 1: 1 |
| 2 | 4 | brown | `0x80075D7C` | breaks (junk, explosion) | area 0: 17 |
| 3 | 0 | red | `0x80075D7C` | explodes: a 128 × 128 px blast with contact damage 16 for 30 updates | none in stage 4 |

This agrees with the Fandom text (purple: until it hits something; black: one space) [certain for
the code and the sprite numbers; the colours come from the bank image of the export].

Another sheet, [`nightmare-crate.md`](nightmare-crate.md) (written from Shield Sheldon's stage,
overlay 10, where all records are `id` 3), covers the same object under a descriptive name. This
sheet adds the official name and the stage 4 records; the two should be merged under the official
name.

## State machine

States (table `0x800F3A20`): 0 set-up, 1 main (`0x800ECE74`), 2 end (`0x800EE0B0`). Sub-steps of
state 1 (table `0x800F3A2C`, field `+0x06`): 0 slide (`0x800ED09C`), 1 fall (`0x800EDAB0`), 2 rest
(`0x800EDC9C`), 3 nothing.

The entry (`0x800EE050`) does nothing while `mega+0xE0` = 2 (the player's screen-wide special
attack, `docs/enemies/nightmare-press.md`).

### Main (state 1)

1. Clear the hit flash (palette bit 15).
2. Only in stages ≥ 19 (the hidden areas): when no block and no wall is just left of it and the cell
   under it is 58 (the belt), x − 1 px: it rides the conveyor belt to the left (as the junk block).
3. The sub-step.
4. `SolidWeaponHit` (`0x80032B34`): the player's shots that touch its terrain box hit it with its
   damage table. On a kill (only `id` 2, 3): `SpawnJunk(3, …)`, one explosion; `id` 3 also gets the
   blast box (below). State 2.
5. More than 128 px off screen: `DeleteObject2` (it can spawn again), except in stage 7 at
   checkpoint 1. Else `DisplayCheck1`.
6. `0x80031F4C` (pushes and carries enemies), `0x80032BE8` (stops enemy shots), `SolidPushShots`.
7. `latch` (`+0x67`) = 0 on the floor, else the sign of `speed_y`.

### Rest (sub 2)

1. `CollideCheck`; `BlockAt` under its two bottom corners.
2. Nothing under it (no floor bit `0x08`, no block): gravity 0.0508 px/update² (`0x0D00`), sub 1.
   `SolidObject` when the player is within 48 px in x (and `mega+0xE5` = 0).
3. Else `SolidObject` (same test) and a **push test**. The cube keeps the player's push bits
   (`mega+0x71`) from before and after its `SolidObject`; the difference is the side that this cube
   pushes. The test passes only when:
   - X is in step 44, sub-step 2 (the weapon pose of shot type 5, slot 5: Ground Scaravich's
     weapon, Ground Dash [likely for the name]; `docs/players/steps-extra.md`), or
   - Zero is in step 57 (skill bit `0x10`, slot 5: Sentsuizan [likely for the name]).
4. On a push (push bit `0x02` of this cube with the player facing left: the cube moves left; bit
   `0x01` with the player facing right: it moves right), and only when the push test passed:
   - `id` 0, 1: `speed_x` = 4.0 px/update away from the player, and an acceleration of 0.14
     px/update² (`0x2400`). `MoveObject2` subtracts the acceleration from `speed_x` because the cube
     faces left (`flip` 0). So `id` 0 slows down (one space: the other sheet reads the stop at
     exactly 48 px), and `id` 1 speeds up until it hits something. Sub 0. The start x goes to
     `+0x7C`.
   - `id` 2 (X only): `SpawnJunk(3, 0x800F3A0C)`, an explosion, state 2 (gone).
   - `id` 3 (X only): `SpawnJunk(3, 0x800F3A10)`, timer 30, attack box `0x800F3A1C`, state 2 (the
     blast).

   Zero breaks `id` 2 and 3 with the damage table instead (id 36, below).

### Fall (sub 1)

- `BlockAt` under the two bottom corners. On the floor or on a block: y snapped to 16 n + 8, sound
  (2, 82), and when it fell for more than 16 updates and is on screen: `Quake2(16, 2, 1)`. Speeds and
  gravity 0, sub 2.
- Else: fall speed at most 2.0 px/update; `MoveObject2`; the fall timer `+0x85` + 1. `SolidObject`
  (the 48 px test); `CollideCheck`.

### Slide (sub 0)

`MoveObject2`, `CollideCheck`, `BlockAt` on both sides and under it; gravity `0x0D00` when it has
no floor; `SolidObject`. The speed stays within ±4 px/update. It stops (sub 2) at a wall or a block
[certain for the calls; the stop rules were not read line by line].

### End (state 2)

`display` = 0. `id` 3: `ContactMega` with the blast box (−64, −64, 128, 128): **128 × 128 px**,
contact damage 16; an explosion every 4 updates and junk every 8 updates, for 30 updates. Then
`DeleteObject` (all `id`).

## Damage

- `id` 0, 1: `dmg_tbl_capsule` (`0x80075B9C`): class 7 for the buster and most weapon ids, class 2
  for the sabre ids. Class 7 stops the shot as a wall does (`ApplyWeaponHit` sets the shot's
  collision flags `+0x70` |= `0x0F` and returns 127; no damage, no sound) [certain for the code].
- `id` 2, 3: `0x80075D7C` (named `dmg_tbl_cube` here). Only three ids do damage, 8 each (HP 8: one
  hit): **5, 14** (slot 5 uncharged and charged: Ground Dash [likely]) and **36** (Zero's technique of
  slot 5, made by Zero's overlay at `0x801ECFA8` and `0x801ED054`: Sentsuizan [likely]). Class 7
  (stopped) for most other ids; class 2 (pass) for ids 2–4, 7, 10, 23–35, 37–41, 44, 55–57,
  59–62; class `0xFF` (deflect) for ids 64–78.

## Summary for a reimplementation

1. Only with Nightmare Effect 5. A solid 48 × 48 px box; the player and enemies stand on it; shots
   stop on it. It falls (0.05 px/update², at most 2 px/update) when nothing is under it; a long fall
   shakes the screen.
2. Pushed by X's Ground Dash or Zero's Sentsuizan from one side: grey (`id` 0) slides one space
   (48 px); purple (`id` 1) slides faster and faster until it hits something; brown (`id` 2) breaks;
   red (`id` 3) explodes (128 × 128 px, 16 damage, 30 updates).
3. Brown and red also break from one hit of ids 5, 14, 36.

## Open questions

1. The stop rules of the slide (sub 0) in this sheet: see `nightmare-crate.md`.
2. (answered) The cubes of the hidden area stage 20 area 1 (records 22–24, brown, `id` 2): the
   overlay 28 code is the overlay 07 code after relocation, so they also need `nightmare_effect` 5
   and ride the belt there (the stage ≥ 19 test) [certain for the code;
   `docs/stages/shark-alt.md`]. (Stage 7 and stage 22: the same code, `nightmare-crate.md`.)
3. Merge this sheet and `nightmare-crate.md`.
