# Kit fixes: proposed changes to files this agent does not own

## `docs/TODO.md`

Mark done:

- `catalog/helpers.json` applies overlay addresses in every stage: done. Entries take an optional
  `"overlay": <n>` or `"overlay": [<n>, ...]` and an optional `"alt": [{"overlay", "name", "note"}]`
  for the same address in other overlays. All 148 entries at `0x800E9860` and up are tagged.
- `rekit --name` on a sprite-only misc or visual export fails with a `KeyError` (both bullets, the
  Enemy RE one and the `spriteexport` one): done. `resolve` uses only sources with a `table`;
  the kit knows the `visual`, `effect`, `logic`, `misc`, `kind7` and `actor2` classes; an export
  without sources or with an unknown class stops with exit 2 and a message.
- Kit: show `jr reg` switch tables as code, not data, and walk their targets: done (boss gate
  `0x80011694`: 51 cases; shadow `0x800E9864`: 12 cases, `0x800E9894`: 6 cases). The other parts of
  that bullet (a copy of self in another register; player fields through a base register;
  `a1` = `game`, `a2` = `mega` in stage-object steps) stay open.
- Kit: a step table is read past its end (rocket platform `0x800F60A0`): done. A state table now
  ends at an address that any code of the same region uses (`0x800F60AC` is used by the rain
  controller at `0x800ED53C`). The "highest step value that the code stores" rule is not made.
- Kit, from batch 2: the class tables at scratchpad `+0x88` and `+0x8C`: done (effect = neighbour
  class, 40 entries; stage logic = owner class, 45 entries; both are `--class` values). The other
  parts of that bullet stay open.
- Kit minors from the final review: all done: `found_tables` searches only the region of
  the entry function (same result; `turtloid-1` main 26: 23 s to 0.3 s) and marks a table that no
  live object uses ("Found by the code search"); `self_fields` counts a store through `a0` in the
  delay slot of a call; a test for an export with several tables; a type out of range gives exit 2;
  the box line of `docs/re-guide.md` says that terrain boxes use half sizes.

New open items:

- Kit: a switch case number is the table index; show the case value when the code subtracts a
  bias before the `sltiu` (`SharkBoxBySprite` `0x800F66FC`: index = sprite − 10).
- Kit: the `weapon` class (`weapon_types` `0x80071220`, 83 entries) is not a `--class` value and not
  a neighbour class (the stage laser probes are weapons made by the laser puzzle).
- Kit: the stage-logic table at scratchpad `+0x8C` is followed by 32 more words of stubs before
  the misc table (`0x800F82A8` + 45 × 4 .. `0x800F83DC` in `shark-4`); check whether stage logic has
  more than 45 types.

## `docs/engine.md`, "Object classes"

- Effect row: "scratchpad `+0x88`, 40 entries" (the visual table is 58 entries, the effect table
  starts `0xE8` after it, the stage-logic table `0xA0` after the effect table, in `intro-1`,
  `turtloid-1`, `sheldon-17`, `shark-7`, `yammark-2`).
- The current-object pointer of each update loop (scratchpad): main `+0x4C` (`0x8002000C`), weapon
  and visual `+0x50` (`WeaponLoop`, `0x80020258`), effect `+0x54` (`EffectLoop`), stage logic `+0x5C`
  (`StageLogicLoop`), stage objects `+0x60` (`0x800205A0`), misc `+0x64` (`0x800206C0`), the class of
  table `+0x98` (`0x800207D4`, likely the ray objects) `+0x68`, kind-7 `+0x6C` (`Kind7Loop`)
  [certain for the stores; the kit reads self through all of them].
- `kind7_types` and `actor2_types` are in the main program and hold overlay addresses of one stage
  each (`actor2` type 0 `0x800F2414` is `RideUpdate` only in overlay 06); in other overlays these
  entries point into unrelated code.
- Stage logic type 29 in `shark-4` (overlay 07) is `0x800EA2EC`, inside `PressureDeathWarp`, not a
  function start: a stale entry.

## `catalog/helpers.json`: open questions and proposed additions (no rename made)

- `0x800F765C` in overlay 07 is the state 0 function of the Metal Shark Player shot (visual 22,
  state table `0x800F97F8[0]`); propose an `alt` name, for example
  `"alt": [{"overlay": 7, "name": "SharkVisLaunch", "note": "visual 22 state ... (to read)"}]`.
- `boss_overlay` (`0x800FA000`) is tagged `[43, 44]`: it names the base only while a boss overlay
  is loaded. Before, in stage 7 (overlay 10 reaches `0x800FD008`) the kit named Shield Sheldon's
  data `boss_overlay+0x..`.
- The Ride Armor entries are tagged 06. `docs/engine.md` says that overlay 28 has an `actor2` type 1
  Ride Armor; none of the tagged addresses is a function start in overlay 28.
- Guardian (`0x800F03F8`) is tagged 10; the note says the same code is at `0x800EF1BC` in stage 22:
  that address has no entry yet.
- Unplaced entries: none.

## Effect of the fixes on known kits (for the main session)

- Overlay-scoped names now label the object's own functions and the kit walks them (before, a name
  in `helpers.json` stopped the walk). Kits get more complete: `shield-sheldon` 121 to 132
  functions, Metal Shark Player (`shark-7`, main 56) 100 to 147, Nightmare Pressure (`shark-4`,
  main 16) 90 to 95, player X 393 to 395. No function was lost and no function end changed.
- `commander-yammark` (overlay 03) loses the wrong names `GuardianMain`, `MettaurGuardTrigger`,
  `SharkSprtSet1`, `SheldonThink`.
- `rocket-platform`: 17 to 7 functions; table `0x800F60A0` has 3 entries; the rain controller's code
  and the misc type 20 neighbour are gone.
