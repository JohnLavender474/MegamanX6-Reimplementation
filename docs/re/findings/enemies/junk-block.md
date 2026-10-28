# Junk block (stage 06 and 07, stage object type 11): behaviour sheet

A block of grey scrap in Metal Shark Player's stage (stage 4). It is solid: the player stands on it
and is pushed by it. The player's shots can destroy it (16 HP); then it breaks into junk, explodes
and can drop an item. In section 2 the blocks ride the conveyor belt to the left. "Junk block" is a
descriptive name: the Fandom pages of the stage and of the Nightmare effects give no name for it
[checked: "Recycle Lab" and "Nightmare" pages, through the MediaWiki API]. It is not Nightmare Iron
(the Recycle Lab causes that effect in other stages; it is not in stage 4).

Code in overlay `06` (area 0; entry `0x800EA530`, state table `0x800F3664`, set-up `0x800E9F04`)
and overlay `07` (area 1; entry `0x800EEA90`, state table `0x800F8D40`, set-up `0x800EE464`, run
`JunkBlockRun7` `0x800EE790`). Live in `shark-2_section-2`, `shark-3_press-low` and
`shark-4_mini-boss` (second list, records 3–5). Read with the RE kit (`python3 -m mmxdemake.rekit
--state shark-1_ride.state.gz --type 11 --class stage`, and with `shark-2_section-2`); notes in
[`~/mmx-data/x6/re/shark-open/`](../../work/shark-open/). Units, tags and the shared rules: `docs/engine.md`. Records:
`docs/stages/shark.md`, `docs/stages/shark-2.md`. All values are [certain for the code] unless a
line says otherwise; the addresses are those of overlay 06.

## Spawn and set-up

- `var` bit 7 (record `id` ≥ 128): `act4` = 1 (wait) and draw priority 34; else `act4` = 0 and
  priority 35. Then bit 7 is cleared.
- Flags `0x41`, state 1. Sprite type 138 (bank 4). Sprite = `var`.
- **HP 16**, `dmg_tbl_normal`, contact damage 127 (not used: the block calls no `ContactMega`).
- Terrain box (a copy at `+0x80`): `var` 0 `0x800F365C` (centre (0, 4), half 24 × 28): **48 × 56
  px**; `var` ≠ 0 `0x800F3660` (centre (0, 4), half 16 × 20): 32 × 40 px. Hurt box `0x800F3654` +
  4 × `var`: `var` 0 (−24, −24, 48, 56), `var` 1 (−16, −16, 32, 40).
- `+0xA2` = 1: the stackable-block mark (as the Nightmare Cube, `docs/enemies/nightmare-cube.md`).

## Main (state 1, `0x800EA230`)

1. Area 0:
   - a block above y 512 (the upper path) is deleted at checkpoint 1;
   - a block at y 824 (the upper block of each pair on the lower path) tests the point 16 px under
     its terrain box for a block or a solid cell. When there is none, it breaks as if killed (step
     3). So when the lower block of a pair is destroyed, the upper one breaks too.
2. Area ≠ 0 (`JunkBlockRun7` in overlay 07): when no block and no wall is just left of it:
   - `act4` = 0: when the cell under it is 58 (the belt), x − 1 px per update: it rides the belt;
   - `act4` = 1: it waits until the player's x is ≥ 1088, then `act4` = 0.

   The blocks of the second list of area 1 (`id` 128, x 1320, 1368, 1416) wait; in `shark-4` they
   have moved 144 px (`docs/stages/shark-2.md`).
3. `ContactWeapon`. Killed: `var` ≠ 0: `SpawnJunk(3, 0x800F3650)` (animations 5–7) and 2
   explosions (`SpawnExplosionRand`: offsets of 0 or ±16 px in x and y, not a range); `var` 0: `SpawnJunk(3, 0x800F364C)` (animations 2–4)
   and 3 explosions (±32 px). Then `DropItem(14)` (difficulty < 2 only) and state 2
   (`DeleteObjectKeep`: it does not come back while the area is loaded).
4. Else: the hit flash when the HP changed; `0x80032BE8` (stops enemy shots), `SolidPushShots`;
   `DisplayCheck(64, 64)`; `SolidObject` when the player is within 48 px in x; when it is drawn,
   `0x80031F4C` (pushes and carries enemies).

States 4 and 5 (area 0 only) make a block that follows a parent object (`+0x50`) and ends with it;
state 1 never goes there and no record starts there [certain for the code; the user is not known].

## Damage

`dmg_tbl_normal`, 16 HP: 6 lemons (3 each), or 2 full charges (`docs/players/attacks.md`). It
takes damage from any normal weapon.

## Animations

Area 1 table `0x800F8D08`, 8 animations of one sprite each (12 ticks, held). Export:
`enemy-4185141b5a` (`shark-2` to `shark-6`, cell 48 × 64; the export of `shark-7` is garbage, see
`docs/stages/shark-2.md`). Proposed catalog name: `junk-block`.

## Summary for a reimplementation

1. A solid 48 × 56 px block (or 32 × 40 px); 16 HP; breaks into junk with an item drop.
2. Lower path of section 1: pairs; the upper block falls apart when the lower one is gone.
3. Section 2: it moves left with the belt at 1 px/update until a wall or another block stops it;
   some wait until the player is at x 1088.

## Open questions

1. The user of states 4 and 5.
2. The y 824 test runs only for that exact y; the other blocks use a support value that the
   function does not set [certain for the code; the effect is not known].
