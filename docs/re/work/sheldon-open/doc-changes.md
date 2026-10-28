# Sheldon open items: proposed changes to files that this task does not own

Source: the code-reading round of Shield Sheldon's stage (stage 7, overlay 10; stage 22, overlay 31).
Probes: [`~/mmx-data/x6/re/sheldon-open/`](./) (`s7probe.py`, `fulldis.py`, `rawdis.py`, `find_state3*.py`,
the kits `stage15-kit`, `stage15-s4-kit`, `stage15-s22-kit`). Files changed by this task:
`docs/stages/sheldon.md`, `docs/stages/sheldon-alt.md`, `docs/enemies/light-capsule.md`,
`docs/enemies/nightmare-bug.md`, new `docs/enemies/nightmare-crate.md`.

## For the X weapons agent: weapon id 44

Weapon id 44 is **Zero's**, not X's [certain for the code]:

- `weapon_types[44]` (`0x80071220` + 176) = `0x801EDFC8`. It is a function start only in overlay 1
  (Zero); in overlay 0 (X) it is in the middle of a function.
- The only code that writes type 44 into a weapon object is `0x801EAA24` (overlay 1). Zero's step 59
  (`0x801EC614`, sub-step table `0x801EE39C`), sub-step 4 (`0x801ED198`), calls it every 10 updates
  (count `mega+0xE1`, n = count / 10): one object at Zero's x first, then pairs at x ± 32 × n, each
  with an effect object type 26. Step 59 is Zero's special attack (skill `0x80`, button `0x40`).
- So the "slot 8 → ids 8, 17, 44" stun rule is: X's slot 8 weapon (8, 17) and Zero's skill `0x80`
  technique (44). Zero's skill bit n is the technique of the stage n + 1 Maverick [likely: bits
  `0x01` and `0x20` after the stage 1 and 6 clears].
- More from the `weapon_types` table: the entries that are function starts in overlay 1 (Zero) are
  ids 24–31, 33–42, 44, 48 and 58 (`0x801ED2A8`, `0x801ED928`, `0x801EDAF4`, `0x801EDFC8`,
  `0x801EDCC8`); ids 45, 46, 47, 49, 50 and 51 (`0x801EE2D4`, `0x801ED898`, `0x801EDA64`,
  `0x801EE310`, `0x801EDCB0`) start functions in overlay 0 (X) [certain for the prologue test].

## `docs/engine.md`

1. "Weapon ids" table: replace the row for id 44 with "Zero's step 59 (skill `0x80`, the special
   attack): waves of objects that spread from Zero by 32 px every 10 updates (`0x801EAA24`, overlay
   1). The only id that shows a hidden glass floor of stage 7 [certain]". Add: "5, 14: slot 5 (the
   stage 5 weapon); with 36, the only ids that break the brown and red Nightmare crates".
2. "Nightmare Effect", value 5: replace "a stage 7 object (state table `0x800FC6B8`, not read)" with
   "the Nightmare crates of stages 4 and 7 and of stage 22 (stage object 15, the same code in
   overlays 06, 10 and 31; `docs/enemies/nightmare-crate.md`)". Value 5 is the stage 5 pick
   (Ground Scaravich: `scaravich-1_start` is stage 5).
3. Game state table: add
   - `+0x9E`[k] (halfwords): navigator message "seen" bits, one word per block k of
     `navcall_index` (`0x8007A124`[stage × 2 + area]), bit = message number.
   - `+0xC8`: a copy of `+0x69` made by `StageStartReset`; overlay 20 compares the two to show the
     part found in the stage and to give the full armour.
   - `+0x69`: bits `0x01` Blade head, `0x02` Blade arms, `0x04` Blade body, `0x08` Blade legs,
     `0x10` Shadow head, `0x20` Shadow arms, `0x40` Shadow body, `0x80` Shadow legs; 4 parts give
     `game.armors` bit `0x04` (Blade) or `0x02` (Shadow) in overlay 20 (`0x800F0668`).
4. Player fields: `mega+0x02` = the character (0 X, 1 Zero) [certain: the Light capsule and the
   crates test it; 1 in `turtloid-6_zero-player`]. `mega+0x14E` = the number of the optional
   navigator message of the zone the player is in (0 = none); the HUD (`0x80024528`) draws an icon
   while it is set.
5. Stage logic: add the two engine-wide types:
   - **14 `NavigatorCall`** (`0x8005335C`): the text-message zones (forced when `id` bit 7 is clear,
     optional with a button press when it is set). Full rule in `docs/stages/sheldon.md`,
     "Navigator call points".
   - **27 `StageStartBanner`** (`0x800541D4`): made by `0x8001E730` at every stage start while
     `game+0x1E` is 0; ray type 7 waves and misc type 18, which writes `game+0x1E` = 1.
   - `GetStageLogic` = `0x8002C3F8`. Ray objects: the type table is at scratchpad `+0x98`; the loop
     is `0x800207D4`.
6. "Open questions" 2 (meaning of weapon ids): id 44 is answered.

## `docs/objects-legend.md`

Stage logic 14 is not "BackgroundSetting": it is the navigator call point (`NavigatorCall`). The
record `id`: bits 0–3 = the message number (also the rectangle index), bit 7 = optional.

## `catalog/helpers.json`

See `helpers-proposed.json`: rename `0x8005335C` to `NavigatorCall`; new names for stage logic 7,
14, 27, the crate, the Zero id-44 spawner, the armour completion in overlay 20 and their tables.

## `docs/stages/shark.md` (owner: Shark objects agent)

"Falling crate (stage object 15)": the code is the same as overlay 10's; the records `id` 0 ×4,
1 ×2, 2 ×17 are the grey (push 48 px), purple (slides) and brown (breaks) kinds. Link to
`docs/enemies/nightmare-crate.md`.

## `docs/players/steps-extra.md`

- Step 44 (X, the pose of shot type 5, slot 5): in sub-step 2 it pushes the grey and purple
  Nightmare crates and breaks the brown and red ones.
- Step 57 (Zero, skill `0x10`): it also pushes the grey and purple crates.
- Step 59 (Zero's special attack) makes the id-44 waves (`0x801ED198`, `0x801EAA24`).
- On collision 60 (stage 7 glass floors) stage logic 7 adds a slide after steps 2, 26, 5, 8 and
  12 (`docs/stages/sheldon.md`, "The glass slide").

## `docs/TODO.md`

Close the six Sheldon items: stage logic 7 (glass slide), 27 (stage start banner), stage object 15
(Nightmare crate), the kind-7 record (`SheldonBgSwitch`), the value-5 object (the same crate),
the Nightmare Bug state 3 (no writer; probably unused), bit `0x04` of `game+0x69` (Blade body),
stage logic 14 (navigator call points), weapon id 44 (Zero step 59). New items:

- A save state with Nightmare Effect 5 in stage 4 or 7 (crates live).
- A Zero state after step 59 near a hidden floor; the game name of that technique.
- What sets `0x80097428` (it turns the navigator calls off).
- The pad bit `0x100` of `0x800C4570` (the optional call button).
