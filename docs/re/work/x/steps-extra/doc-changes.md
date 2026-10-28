# Proposed doc changes (steps 32–77 and the forms)

The main session owns these files. The facts are in `docs/players/steps-extra.md` and in
`notes.md` here. The most important change is the form numbering.

## 1. The form numbering (all sheets)

Old: "X 1–4, Zero 5", "1 in all X states", "form 1 in every X state read".
New: **`mega.form` (`mega+0xE7`) = `game+0x5E`: 0 no armour, 1 Falcon, 2 Shadow, 3 Blade, 4
Ultimate, 5 Zero.** Evidence [certain for the values]:

- The armour select (overlay 16, `0x800E9898`) always lists 0, then 1 for armour bit `0x01`, 5
  for `0x10` (Zero), 2 for `0x02`, 3 for `0x04`, 4 for `0x08`.
- The new game (overlay 19) sets form 1 with armour bit `0x01` for X, form 5 with bit `0x10` for
  Zero; the cheat sets form 4 with bit `0x08` (X) or bits `0x31` (Zero: Black Zero `0x20`).
- `sheldon-4_no-armor` has form 0; the Light capsule gives the Ultimate Armor only to X with
  form 0.
- The names: bits `0x02`/`0x04` are the full Shadow/Blade armour bits of the Light capsule sheet
  [likely]; form 3 has the Mach Dash and form 2 the spike immunity [certain for the code].

## 2. `docs/engine.md`

- Progress bytes bullet: replace "and `game+0x5E` goes 1 → 5 when the user picks Zero (so `+0x5E`
  is the selected character)" by "and `game+0x5E` (the form) goes 1 → 5 when the user picks Zero:
  form 5 is Zero. The character is `game+0x38` (0 X, 1 Zero); the armour select sets both".
- Game table: `+0x5E` row → "player form, copied to `mega+0xE7`: 0 no armour, 1 Falcon, 2 Shadow,
  3 Blade, 4 Ultimate, 5 Zero". Add a row `+0x38` | `0x800CCF08` | "character: 0 X, 1 Zero; picks
  the player member and overlay".
- "The player" intro: "`player_steps` (`0x80073ACC`, 32 entries)" → "78 entries (32–77:
  `docs/players/steps-extra.md`)".
- "Disc source" bullet: replace "Member 87 is X (…) and member 85 is Zero (…) [certain: byte
  match]. Members 86, 88 and 89 also have a 228-frame set [likely: X's armours]." by "The stage
  load (`0x80013BF0`) picks the member: Zero (`game+0x38` ≠ 0) 85; X by the form: 1 → 87, 2 → 88,
  3 → 89, 0 and 4 → 86 [certain for the code]. Byte match: 87 in the Falcon states, 86 in
  `sheldon-4_no-armor`, 85 in the Zero state [certain]. Each X member has one 228-frame set; the
  second set has 66 (86), 93 (87), 94 (88), 76 (89) frames."
- "Frame sets" table: "frames 10, sprites 18 (93)" → "(93 with member 87, 66 with member 86)".
- "More steps" bullet: replace by "32–47 and 74–77 are X's overlay steps, 48–63 Zero's, 64–71
  stand, 72 and 73 have no writer (`docs/players/steps-extra.md`)".
- "Form" bullet: replace by the new numbering (section 1) and "the form changes the air action,
  the special attack, special-weapon charge, weapon cost, hit class, spikes, the wall slide and
  the sprite member (`docs/players/steps-extra.md`)".
- Hit class line ("class 0 picks 4 for player forms 1–4"): no change needed.

## 3. `docs/players/map.md`

- "The 32 steps" stays, but the paragraph "More than 32 steps" → "See
  `docs/players/steps-extra.md`: X 32–47, 74–77; Zero 48–63; 64–71 stand; 72, 73 no writer."
- Step 13 row: "X only in form 4, Zero always" stays true.
- Sprite paths: `~/mmx-data/x6/sprites/players/x/` → `.../x-falcon/` (the export of
  `intro-1_start` is now named `x-falcon`; the old `x/` folder is still on disk).
- Open items: remove "The overlay steps 32–63 and 74–77 … read the overlays" (done). Add "Zero's
  step bodies 48–63 in detail".
- "Step 2 (movement core): done" list: add "4. Steps 32–77 and the forms:
  `docs/players/steps-extra.md`".

## 4. `docs/players/air.md`

- `mega+0xE7` row: → "form, from `game+0x5E`: 0 no armour, 1 Falcon, 2 Shadow, 3 Blade, 4
  Ultimate, 5 Zero [certain for the values; names likely]; 1 in the Falcon states, 0 in
  `sheldon-4_no-armor`, 5 in the Zero state".
- Paragraph "The form names: the X form is 1 and `game.armors` is 1 in all X states. With the armour
  bits … form = bit number + 1." → "The armour select lists form n + 1 for armour bit n (bits 0–3);
  form 0 (no armour) is always in the list (`docs/players/steps-extra.md`)."
- Table "X, forms 0 and 2 | nothing" stays. Step 33 row: "a hover [guess]" → "a hover of up to
  180 updates [certain for the code]".
- Open items: steps 33, 35, 74 are now in `steps-extra.md`.

## 5. `docs/players/ladder-hurt-events.md`

- `+0xE7` row (line 40): → "form: `game+0x5E`: 0 no armour, 1–4 armours, 5 Zero [certain]".
- Lines 137, 413: "(X: form 1 in every X state read)" → "(X: forms 1–4, so every armour; form 0
  and Zero use classes 1 and 2)".
- Line 162: "form 2 is likely the Shadow Armor" → keep "likely" for the name, the value is
  certain.
- Open item "The form numbers: what X's forms 1–4 are …": answered (section 1).

## 6. `docs/players/movement.md`

- Open item (line 212) "`+0xE7` = 2 (the armour of the super jump)": → "form 2 = Shadow Armor
  (spike-proof, no wall slide, super jump) [names likely]".

## 7. `catalog/helpers.json`

See `helpers-proposed.json` (same format). Notable: `mega.form` note (new numbering),
`game.form`, `game.character`, `game.zero_skills`, `SpecialAttackTry`, `WeaponPoseStart`,
`WeaponCost`, `StageLoadPlayer`. `mega+0xE0` is set by the special attacks; the old name
`mega.no_drain` may still fit (the rain code reads it) — keep or rename to `mega.special`.

## 8. `docs/TODO.md`

- Mark done: the overlay steps past 31; the form numbering; the members 86–89.
- Add: a save state in each armour form (Shadow, Blade, Ultimate) to export members 88, 89 and to
  check the frame-set rule of `playerexport` for them; a state of Zero in a technique step.
