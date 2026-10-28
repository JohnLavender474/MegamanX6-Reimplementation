# Nightmare Freeze (stage 07, main object type 35): behaviour sheet

Nightmare Freeze is a Nightmare Effect of section 2 of Metal Shark Player's stage (stage 4, area 1):
blocks of blue ice on the tops of the junk piles under the Nightmare Press. Each object is one ice
block. At its spawn it changes two map cells under it to ice cells (collision value 60). Two hits of
the right weapon break it; then the normal cells come back. Other weapons have no effect. The
objects exist only with Nightmare Effect value 2.

The name: "Nightmare Freeze" (ナイトメア・フリーズ) is in the Nightmare Phenomenon table of the
Fandom page "Nightmare": caused by the North Pole Area, seen in the Recycle Lab; "parts of compressed
debris are coated with slippery ice. It can be cleared off with Ice Burst/Hyoroga" [Fandom, read
through the MediaWiki API]. The code agrees: value 2 is the stage 2 pick (`docs/stages/shark.md`,
"Nightmare Effect"), and only the slot 2 weapon ids break the block (below).

Stage overlay `07` (`rock_bin/07.bin`, loaded at `0x800E9860`). Not live in any save state (all
states have `nightmare_effect` 0). Read with the RE kit (`python3 -m mmxdemake.rekit --state
shark-2_section-2.state.gz --type 35`); notes in [`~/mmx-data/x6/re/shark-open/`](../../work/shark-open/). Units, confidence
tags and the shared rules: `docs/engine.md`. Records: `docs/stages/shark-2.md` (44 records, all `id`
0, x 880–4080, y 336–400). All values are [certain for the code] unless a line says otherwise.

## Spawn and set-up (`0x800F01BC`)

- `nightmare_effect` (`0x800CD30A`) ≠ 2: `DeleteObject3`, nothing more.
- HP 2, contact damage 0, no attack box, no terrain box. It does not move and cannot hurt.
- Hurt box: `var` 0 `0x800F8DB0` (−16, −16, 32, 40): 32 × 40 px; `var` ≠ 0 `0x800F8DB4` (−16, −16,
  16, 40): 16 × 40 px.
- Damage table `0x80074ABC` (below). Draw priority 35, layer = the player's layer. Animation table
  `0x800F8DC8`; animation 0 (`var` 0) or 3 (`var` ≠ 0).
- **The ice cells.** `CopyTiles` on layer 0 (cells of 16 px): the cells at (x − 16, y + 464), 2 × 1
  cells (`var` ≠ 0: 1 × 1), go to (x − 16, y − 16). Then the same copy again with 2 × 1 cells. Then a
  layer 0 redraw. In member 99 the source cells have collision 60 and the target cells have 56: for
  the 44 records, 88 cells change from 56 to 60 [certain for the map]. So the block sits on the top
  of a junk pile and its two cells under the block become ice.
- State 1, step 2.

## The `id` parameter

`var` (the record `id`) 0: a block 32 px wide, 2 cells of ice. `var` ≠ 0: 16 px wide, 1 cell (and
the junk is made 8 px to the left). All 44 records of area 1 have `id` 0.

## State machine

States (table `0x800F8DE0`): 0 set-up, 1 main (`0x800F036C`), 2 `DeleteObject` (`0x800F04F0`), 3
`DeleteObject2` (`0x800F0510`). The entry (`0x800F04A4`) runs the state, then `CollideCheck`.

Main, each update:

1. Flags |= `0x10`; `AnimeTick`; `ContactWeapon`.
2. Killed (2 HP of damage):
   - `display` = 0;
   - `CopyTiles` of the cells at (x − 16, y + 496) (2 × 1, or 1 × 1) to (x − 16, y − 16): collision
     60 back to 56 (the plain cells, 32 px below the ice cells in the map); layer 0 redraw;
   - `SpawnJunk(5, 0x800F8DD8)` (`var` ≠ 0: 3, at x − 8): ice pieces, animations 1, 1, 2, 1, 2;
   - state 2: `DeleteObject`. The record is not marked "used", so the block can spawn again when
     the player comes back to that place [likely: `DeleteObject` clears the spawn bits only].
3. Else: more than 48 px off screen: state 3 (`DeleteObject2`). Else `DisplayCheck(32, 32)`.

The cells stay ice while the object is off screen and deleted by step 3: only the kill restores
them [certain for the code; a play check would show it].

## Animations (table `0x800F8DC8`, 4 animations)

| # | sprite | use |
|---|---|---|
| 0 | 0 | the block, `var` 0 (held) |
| 1 | 1 | ice piece (junk) |
| 2 | 2 | ice piece (junk) |
| 3 | 0 | the block, `var` ≠ 0 (held) |

Export: `enemy-4c32312ebb` (found by search; cell 32 × 62). Proposed catalog name: `nightmare-freeze`.

## Damage

`0x80074ABC` (named `dmg_tbl_freeze` here): class 2 (no effect) for all 80 ids except four, with
damage 2 (one hit kills):

| id | weapon |
|---|---|
| 2, 11 | slot 2, uncharged and charged (`docs/engine.md`, "the stun ids": slot n and n + 9). Slot 2 is the weapon of stage 2, Blizzard Wolfang's Ice Burst [likely: from the Fandom text and the stage numbers] |
| 33 | Zero's technique of stage 2 (Hyoroga) [likely: the id pattern of `docs/engine.md`, "the stun ids"; see `docs/enemies/metal-shark-player.md`, open questions] |
| 52 | runs the same weapon code as id 2 (`weapon_types[52]` = `weapon_types[2]` = `0x801EECE8`): a part of Ice Burst [likely] |

A shot of any other id passes through (class 2 has no reaction).

## What collision value 60 does

Value 60 is the ice. Stage logic 7 of area 1 (`0x800F0530`) makes it slippery: the same code as
the glass slide of Shield Sheldon's stage and the ice of Blizzard Wolfang's stage (`docs/engine.md`,
"Slippery floors and slopes (stage logic 7)") [certain for the code]. Without Nightmare Effect 2 no
cell of value 60 is in the playable part of area 1.

## Summary for a reimplementation

1. Only with Nightmare Effect 2: on each record, a 32 × 40 px ice block on top of a junk pile; the
   two 16 px cells under it become ice (slippery floor).
2. It does nothing else. Two damage of Ice Burst (ids 2, 11, 52) or Zero's ice technique (id 33)
   breaks it in one hit; other shots pass through.
3. On the kill: the cells become normal floor again, 5 ice pieces fly off.

## Open questions

1. (answered) Value 60 is the slippery floor of stage logic 7 (`docs/engine.md`).
2. Whether a block spawns again after a kill (the cells then become ice again).
3. Why the set-up copies the cells twice.
