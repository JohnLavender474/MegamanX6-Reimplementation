# Stage 4 open items: proposals for files of other owners

Work dir: [`~/mmx-data/x6/re/shark-open/`](./) (kits `kit-main35`, `kit-st15-a0`, `kit-st15-a1`,
`kit-st11-a0`, `kit-st11-a1`; probes in `probes/`; Fandom wikitext in `web/`). Names for
`catalog/helpers.json` are in `helpers-proposed.json`.

Fandom access: `https://megaman.fandom.com/wiki/...` gives HTTP 402/403, but the MediaWiki API
works: `https://megaman.fandom.com/api.php?action=parse&page=<Page>&prop=wikitext&format=json&redirects=1`.
Pages read: `Recycle_Lab`, `Metal_Shark_Player`, `Nightmare` (redirect target of
`Nightmare_Phenomenon`, `Nightmare_Cube`, `Nightmare_Freeze`, `Nightmare_Pressure`).

## Files changed in the repo (mine)

- New: `docs/enemies/nightmare-freeze.md` (main 35, value 2), `docs/enemies/nightmare-cube.md`
  (stage object 15, value 5; was "falling crate"), `docs/enemies/junk-block.md` (stage object 11).
- `docs/enemies/nightmare-press.md`: the four solid helpers, `mega+0xE0`, the other users of
  `PressUpdate` (stage 18 area 0, stage 20 area 1), open questions.
- `docs/enemies/nightmare-pressure.md`: name source; open questions 5 and 7.
- `docs/stages/shark.md`: junk block and Nightmare Cube records (the old crate list was incomplete:
  17 / 2 / 4 records over the whole map, not only two grids), the teleporter crystals section,
  Nightmare Effect names, open question 3.
- `docs/stages/shark-2.md`: Nightmare Effect objects, names, open question 3.
- `docs/enemies/metal-shark-player.md`: the side-kick section (names) and open questions 1 and 2
  (ids 35 and 42) only.
- `docs/players/ride-armor.md`: "Objects and memory" (actor2 table, overlay 28), "Steps" (the
  `var` ≠ 0 model), "Open items", "States that would help". Not the sprite section.

## Proposals: `docs/engine.md`

1. **Nightmare Effect list** ("Nightmare Effect", "The value is the stage that caused it"): replace
   the value 2 and 5 items by:
   - 2 **Nightmare Freeze** (main type 35 of stage 4 area 1, set-up `0x800F01BC`; ice blocks that
     turn two map cells to collision 60; `docs/enemies/nightmare-freeze.md`);
   - 5 **Nightmare Cube** (stage object 15 of stage 4 areas 0 and 1, set-ups `0x800ECBEC` and
     `0x800EEC50`, the same code; `docs/enemies/nightmare-cube.md`) and a stage 7 object (likely
     the same cube).
   - Add: Fandom's Nightmare Phenomenon table (page "Nightmare") names the effects by the stage that
     causes them: North Pole Area → Nightmare Freeze (value 2), Central Museum → Nightmare Cube (5),
     Recycle Lab → Nightmare Iron (4, in stages 3, 5, 8). So stage 2 = North Pole Area (Blizzard
     Wolfang), stage 3 = Magma Area, stage 5 = Central Museum (Ground Scaravich), stage 8 = Weapon
     Center [the stage names from Fandom; the values certain].
2. **The stun ids** row (weapon ids table, "2, 11, 33 / 4, 13, 35 / 6, 15, 42 / 8, 17, 44"): the third
   id is **Zero's technique of the same slot**. Replace "ids 35 and 42 run the same weapon code,
   `0x801ED2A8`; which weapon makes them is open" by: "the third id is Zero's technique of slot n
   (id 33 slot 2, 35 slot 4, 42 slot 6, 44 slot 8). `0x801ED2A8` (ids 24–31, 35–42, 48) is a
   function only in Zero's overlay 1. Zero's overlay makes id 35 in step 56 (skill bit `0x08`) and
   id 42 in step 58 (skill bit `0x20`); skill bit 2^(n−1) is the technique of slot n" [certain for
   the code]. Names [likely, Fandom + techno-science weakness table]: 33 Hyoroga (Wolfang), 35
   Rakukojin (Metal Shark Player), 36 Sentsuizan (Scaravich, slot 5, step 57: the Nightmare Cube
   takes it), 42 Ensuizan (Rainy Turtloid), 44 Rekkoha (Infinity Mijinion, skill `0x80`, step 59).
3. **Damage class 7** ("Damage to an object"): replace "Class 7 also exists (`dmg_tbl_capsule`);
   its effect was not read" by: "Class 7 = blocked as by a wall: `ApplyWeaponHit` sets the shot's
   `+0x70` |= `0x0F` and returns 127; no damage, no sound, no hit reaction" [certain for the code,
   `0x800316B4`]. Also: class 8 sets the shot's `+0x98` = 1 and returns 127 (seen in the code, not
   used here).
4. **`actor2`** row of the class table and the `actor2` paragraph: `actor2_types` has 2 entries
   (types 0 and 1, both a Ride Armor: overlay 06 and overlay 28); the next 3 words (`0x80071388`) are
   a separate sub-step table used by `0x80020EF0` and `0x80021354`.
5. **Stage overlays / hidden areas**: overlay 28 = stage 20 area 1, the Another Route of stage 4
   (the record lists of stage 20 area 1 parse in overlay 28; the `0x41` exit of stage 4 goes there).
   The 0x41 rule: `HiddenAreaOf` (`0x80042F7C`): stage 19 + (s − 1) / 2, area (s − 1) & 1. The 0x40
   rule for stage 4: `0x80071190`[stage] = 0 and area 0 → area 1.
6. **Game state**: a row for `mega+0xE0` in the player fields (if engine.md lists them), as in
   `docs/enemies/nightmare-press.md` ("`mega+0xE0`").
7. **Solid objects** ("Some objects are solid for the player"): add the four helpers
   (`SolidWeaponHit`, `SolidStopShots`, `SolidPushShots`, `SolidPushEnemies`) and the `+0xA2`
   stackable mark with `BlockAt`.

## Proposals: other docs

- `docs/players/attacks.md` ("Weapon ids and shots"): add rows "35–42 (24–31, 48): Zero's sabre and
  technique hit objects, code `0x801ED2A8` in Zero's overlay; X never makes them" and "59, 60:
  `0x801EE05C` (the lemon code), used only by the unused Ride Armor model `var` ≠ 0".
- `docs/players/steps-extra.md` ("Zero steps 48–63"): step 56 makes weapon id 35 (Rakukojin
  [likely]), step 58 makes id 42 (Ensuizan [likely]); the skill bits follow the weapon slots (bit
  2^(n−1) = slot n): `0x01` Yammar Option (slot 1), `0x02` Hyoroga (2), `0x04` Shoenzan? (3, step
  55) [guess], `0x08` Rakukojin (4), `0x10` Sentsuizan (5, step 57), `0x20` Ensuizan (6), `0x40`
  Guard Shell (7) [guess], `0x80` Rekkoha (8, step 59). The names are from the Fandom / techno-science
  boss tables; only the slot of each bit and the ids are from the code.
- `docs/enemies/teleport-crystal.md` (open question 1): `StageExit` (`0x8001E32C`) reads `0x40 | id`.
  `0x41` → the hidden area of the stage (`HiddenAreaOf`); `0x40` in stage 4 (byte
  `0x80071190`[4] = 0, area 0) → area 1. Stage 4: `id` 0 → stage 4 area 1, `id` 1 → stage 20 area
  1. (The Scaravich agent studies the other branches; this is only the stage 4 result.)
- `docs/enemies/conveyor-belt.md`, `docs/enemies/rocket-platform.md` (open question 2),
  `docs/enemies/weather-analyze.md` (open question 1): `mega+0xE0` = 2 is the screen-wide special
  attack (X steps 36, 37, 41, 77; Zero step 59); 1 is X step 34. See
  `docs/enemies/nightmare-press.md`.
- `docs/objects-legend.md` ("Object types by sprite bank"): stage 4 rows for main 35 (Nightmare
  Freeze, `nightmare-freeze`, Fandom), stage 11 (junk block, `junk-block`, descriptive), stage 15
  (Nightmare Cube, Fandom; no export yet).
- `docs/TODO.md`: the items "Nightmare Effect objects of stage 4", "junk block", "names", "weapon ids
  35 and 42", "Nightmare Press items", "Ride Armor items" and "crystal destinations of area 0" can be
  closed, with the open points below.

## Catalog names (`catalog/enemies.json`, `names`)

| export | name | source |
|---|---|---|
| `enemy-4c32312ebb` | `nightmare-freeze` | official (Fandom "Nightmare" page, ナイトメア・フリーズ) |
| `enemy-4185141b5a` | `junk-block` | descriptive (already set) |
| `enemy-74e4e39589` | `nightmare-pressure` | official (already set) |
| (no export yet: stage object 15, sprite type 147) | `nightmare-cube` | official (Fandom, ナイトメア・キューブ) |

## Side-kick names (for the record)

`var` 1 (Zero) Sting Chameleon, `var` 2 (X) Magna Centipede, `var` 3 (Xtreme, difficulty ≥ 2)
Blast Hornet [Fandom "Metal Shark Player": "If the player is X, Player summons Magna Centipede ... If
the player is Zero, Player summons Sting Chameleon ... In Xtreme mode, Player may summon Blast
Hornet"; the Atrocious Gameplay wiki page of the stage gives the same]. The earlier guess Spark
Mandrill (`var` 2) was wrong.

## Weapon ids 35 and 42 (task 4 finding, for the X-weapons agent)

- `weapon_types` (`0x80071220`) is static (the same in an X state and a Zero state). Entry
  `0x801ED2A8` (ids 24–31, 35–42, 48) is `addiu sp,sp,-32` (a function start) in Zero's overlay 1
  (`turtloid-6_zero-player`) and mid-function data/code in X's overlay 0 (`shark-8_boss-stun`:
  `00000000 0e0005a6 …`). [certain]
- X: `ShotIds` (`0x8003DF58`) only makes ids slot, slot + 9, 45, 46, 50 and the buster ids. No code
  in X's overlay makes 35 or 42 (all immediates 35/42 there are animations, sounds or compares).
  [certain]
- Zero: `0x801ECD90(player, id)` (`ZeroSpawnAttack`) makes the weapon object. id 42 at `0x801EA60C`
  (the skill try `0x801EA560`, called from the ground and air steps) and `0x801EC5E0` (step 58), both
  after `SetAnime(103)`: skill bit `0x20`, down + special. id 35 at `0x801ECEE4`
  (`ZeroSpawnAttackOnFlag`), sub-step 0 of step 56 (table `0x801EE3C4`): skill bit `0x08`, down +
  attack in the air. [certain]
- Names [likely]: 35 = Rakukojin (Metal Shark Player's technique), 42 = Ensuizan (Rainy Turtloid's
  technique). Fandom: Metal Shark Player weak to "Meteor Rain / Ensuizan" (= ids 6/15 and 42), Shield
  Sheldon to "Metal Anchor / Rakukojin" (= ids 4/13 and 35).
- So the answer to "which weapon fires them and when": **Zero**, with the techniques of slot 4 and
  slot 6, not a third form of an X weapon and not an armour part.

## Open points left

1. Save states: after each crystal of area 0; Nightmare Effect 2 and 5 in stage 4; the hidden area
   (stage 20 area 1); a Zero fight with Metal Shark Player (Rakukojin into BURROW, Ensuizan stun).
2. Nightmare Cube: `docs/enemies/nightmare-crate.md` (the Sheldon agent, descriptive name) and my
   `docs/enemies/nightmare-cube.md` cover the same object (stage object 15, the same code in
   overlays 06, 07, 10, 31). Proposal: keep one sheet under the official name `nightmare-cube.md`
   and merge the other into it (the owner of `nightmare-crate.md` decides). Both agree on the kinds:
   `id` 0 grey one space, 1 purple until it hits, 2 brown breaks, 3 red explodes.
3. Nightmare Freeze: what collision 60 does (stage logic 7 `0x800F0530`, likely the ice slide).
4. Ride Armor `+0x97` bit `0x01`: written only by the ride's own `xori`; purpose open.
5. Stage 18 area 0 (overlay 24) also has a press controller (`var` 0, checkpoint 2 case): which
   stage that is (Fandom lists the press in "Secret Lab 2 (as Zero)") [guess].
6. Trail table `0x80071388`: which object uses it.
