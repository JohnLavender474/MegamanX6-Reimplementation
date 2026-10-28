# Proposed doc changes from step 4 (X's special weapons)

The main session decides; this file does not change other repo files. The new sheet is
`docs/players/weapons.md`. New names: `helpers-proposed.json`.

## `docs/engine.md`

### "Weapon ids": proposed table (replaces the current one)

| id | weapon | maker |
|---|---|---|
| 0 | X-Buster lemon [certain] | X shot try |
| 1, 10 | Yammar Option (slot 1), uncharged / charged; the controller and its 3 options; 10 var 7 is also the charged option shot [likely name] | X shot try (main-program type `0x800434E0`, also used by Zero) |
| 2, 11 | Ice Burst (slot 2) [likely name] | X shot try / pose step 39 |
| 3, 12 | Magma Blade (slot 3): the fireballs; the slash is 50/51 [likely name] | X shot try |
| 4, 13 | Metal Anchor (slot 4) [likely name] | X shot try (main program, also Zero) |
| 5, 14 | Ground Dash (slot 5); with 36 the only ids that break the brown and red Nightmare crates [likely name] | X shot try |
| 6, 15 | Meteor Rain (slot 6) [likely name] | X shot try |
| 7, 16 | Guard Shell (slot 7) [likely name] | X shot try (main program, also Zero) |
| 8, 17 | Ray Arrow (slot 8) [likely name] | X shot try |
| 9 | X-Buster half charge [certain] | muzzle flash |
| 18 | X-Buster full charge, forms 0, 2 [certain] | muzzle flash |
| 19 | X-Buster full charge, Falcon (form 1) [certain] | muzzle flash |
| 20, 21 | X-Buster full charge, Ultimate (form 4), and the bursts it leaves at each hit (it does not stop at a hit) [certain for the code] | muzzle flash / type 20 |
| 22 | X's body in the Ultimate Giga attack (step 34) [certain] | step 34 |
| 23 | X's body in the Falcon dash (step 35) [certain] | step 35 |
| 24–31 | Z-Saber and its combo hits (the sabre bonus ids) [likely] | Zero |
| 32 | X's body in the Mach Dash (step 74) [certain] | step 74 |
| 33–42, 48, 58 | Zero's objects: the entries are function starts in overlay 1 only [certain]; 35 and 42 run Zero's sabre code `0x801ED2A8`. Names from `docs/players/zero.md`: 33 Hyoroga, 34 Shoenzan, 35 Rakukojin, 36 Sentsuizan, 42 Ensuizan, 48 Hyoroga attack | Zero |
| 43 | X-Buster full charge, Blade (form 3) [certain] | X shot try |
| 44 | Zero's step 59 (Rekkoha) waves (Sheldon agent, `zero.md`) [certain] | Zero |
| 45, 46 | Shadow Armor buster button: kunai, and the charged slash [certain for the code] | X shot try / pose step 38 |
| 47 | the Shadow Giga parts (step 37) [certain] | step 37 |
| 49 | the Falcon Giga parts (step 36) [certain] | step 36 |
| 50, 51 | the sabre slash, floor and air/wall; the weapon button with the buster selected gives 50 [certain for the code; "X's sabre in all forms", likely] | sabre poses 45–47, 37, 38, 76, 77 |
| 52 | Ice Burst shard [certain] | type 2 |
| 53 | Guard Shell shard (X and Zero) [certain] | type 7 |
| 54 | Yammar Option shot [certain] | the options |
| 55 | (open) | |
| 56 | Blade up-slash (step 76) [certain] | X shot try (id 43 with up held) |
| 57 | the Blade Giga wave (step 77) [certain] | slash 50/51 |
| 59, 60, 63 | (open) | |
| 61, 62 | Ride Armor punches [certain for the code] | ride |
| 64–78 | not met | |
| 79 | the laser probe of Shield Sheldon's stage [certain] | stage 7 |

Slot rule: slot n (1–8) is the weapon of the stage n Maverick: 1 Commander Yammark, 2 Blizzard
Wolfang, 3 Blaze Heatnix, 4 Metal Shark Player, 5 Ground Scaravich, 6 Rainy Turtloid, 7 Shield
Sheldon, 8 Infinity Mijinion [likely; stages 1, 4, 5, 6, 7 are known; 2, 3, 8 follow from the
weakness chain].

### Stun rows

Replace "which weapon makes them is open" with: the third id of each boss is a Zero technique (33
Turtloid, 35 Sheldon, 42 Shark (and 35 for the buried pop-up), 44 Yammark). The X ids are slot 2
(Ice Burst) → Turtloid, slot 4 (Metal Anchor) → Sheldon, slot 6 (Meteor Rain) → Shark, slot 8 (Ray
Arrow) → Yammark.

### "The player"

- Button bits: `0x10` weapon button (Triangle for X, after the X-only exchange of `0x10` and
  `0x20` in `PadToButtons` `0x8003D7AC`), `0x20` buster (Square), `0x40` Giga attack (R2), `0x80`
  jump (Cross), `0x100` dash (Circle), `0x200` next weapon (R1), `0x400` previous weapon (L1);
  key table `0x8006D600` [certain for the table; button names likely].
- `mega+0xA6` = the weapon lock: 1 charged Ice Burst, 2 charged Guard Shell, 3 charged Meteor
  Rain. `mega+0x144` = the timed item effect (1 shot guard, 2 attack + 1, 3 free weapons, 6
  form 2 plain buster), timer `mega+0x148` (300; 480 for 6). `mega+0x14A` = energy maximum = 6 ×
  `game+0x61`.
- Weapon energy: `mega+0xA8 + 2 × slot`; slot 0 = the Giga gauge [certain]; the gauge fills by
  maximum / 8 per hurt.

### "Damage to an object", the shot side

Add: "Only the weapon objects that read `+0x98` stop. The lemon, Ice Burst, the shards 52/53, the
Yammar Option options, the charge shots and the sabre read it; most special weapons ignore it and
go through (also after a deflect). Type 20 makes a part and goes on." Replace "0, 19, 20 end at
the first hit" with "0 and 19 end at the first hit; 20 goes on".

### Open questions

Item 2 (meaning of most weapon ids): answered for X (above).

## `docs/players/attacks.md`

- "Buttons and fields": `0x10` = Triangle, `0x20` = Square, `0x40` = R2, `0x200` = R1 (next),
  `0x400` = L1 (previous), with the default keys; remove the open item.
- "Ultimate full charge (id 20)": "it makes parts of type 21" → "a hit does not stop it: it makes a
  type-21 part at the hit (up to 3 live in form 4, else 1) and goes on; a deflect, a wall or 42/34
  px off screen end it".
- "Weapon ids and shots" table: 1–8 and 10–17 → link `docs/players/weapons.md`; 50, 51 "[guess:
  X's saber]" → "the sabre slash (`weapons.md`)".
- Open questions: remove the buttons, `+0xA6`, `+0x144`, effect type 27 (a weapon graphics loader)
  and "ids 20/21, 43, 45, 46, 50, 51 in detail" (in `weapons.md`).

## `docs/players/steps-extra.md`

- Step 75: "60 updates after the weapon object signals (`act4`), they move again" → "the timer
  `+0xE1` counts from the start of the step; at 60 (and not before the weapon object's `act4`)
  everything moves again and the object is thrown".
- Step 41: "[guess: they rise]" → "they rise at 16.0 px/update (the charged Ray Arrow)".
- Form 2 at the beam-in: "effect object (type 27, var 5)" → "effect type 27 var 5 loads the
  form's sprite frames to VRAM (it is not visible)".
- Weapon poses table: add the weapon names (5/14 Ground Dash, 11 Ice Burst, 12 Magma Blade, 13
  Metal Anchor, 16 Guard Shell, 17 Ray Arrow).
- Open item "Who sets `act4` for the poses of steps 44 and 75": the weapon object (types 5 and 14)
  at the end of its first animation [certain].

## `docs/players/map.md`

- "Weapon objects": types 24–31, 33–42, 44, 48, 58 are Zero's (overlay 1 only) [certain]; add
  "Step 4 (special weapons): done — `docs/players/weapons.md`".

## `catalog/helpers.json` (corrections to existing entries)

- `WeaponCost` (`0x8003EB0C`): note → "6 × cost_x[id] (0x80073D74, bar units); forms 1, 3, 4:
  2/3 (min 1); Energy Saver (part 0x20): 1/2. Zero: cost_z (0x80073DF4) × 6, × 3 (part 0x20), × 4
  (Black Zero)". The current note names Zero's table.
- `SetSpeedDir` (`0x8002D3B4`): note → "speed 1 px/update in direction dir of 32 (0 = right, 8 =
  up, counter-clockwise; tables 0x80073760, 0x80073784)".
- `WeaponPose` (`0x8003F13C`): add "TryGigaAttack first".
- `mega.weapon_energy` (`0x80097148`): remove "(likely)": slot 0 = the Giga gauge [certain].
- `mega.held` / `mega.pressed`: add "0x10 weapon (X: Triangle), 0x20 buster (X: Square), 0x40
  Giga (R2), 0x200 next (R1), 0x400 previous (L1)".

## `docs/TODO.md`

Close: "which pad buttons are bits `0x10` … `0x400`", "`+0xA6`, `+0x144`", "effect type 27", the
special-weapon ids. New items: the names of the pause-menu items 1, 2, 3, 6; the Guard Shell block
flag writer; part bit `0x800`; the look of each weapon in play.
