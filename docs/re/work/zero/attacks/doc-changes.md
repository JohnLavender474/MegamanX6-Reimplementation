# Proposed doc changes from the Zero attacks work

The main session decides; this file does not change the repo. The new sheet is
`docs/players/zero.md`. New names: `helpers-proposed.json` (57 functions, 10 globals; overlay 1
entries carry `"overlay": 1`).

## `catalog/helpers.json` (corrections to existing entries)

- `WeaponCost` (`0x8003EB0C`): note → "WeaponCost(m, slot): X `0x80073D74`[slot] × 6 (× 3 part
  `0x20`, × 4 in forms 1, 3, 4); Zero `0x80073DF4`[slot] × 6 (× 3 part `0x20`, × 4 Black Zero);
  at least 1". The argument is the gauge slot, not the weapon id.
- `SpendEnergy` (`0x8003F740`): add "for Zero the second argument is the gauge slot, so the
  44/47/49 exception never applies to Zero".
- `mega.var` (`0x800970A2`): remove "(likely)": every Zero-only path tests it.
- `weapon_types` (`0x80071220`): add "Zero: 24–31, 35–42, 48 → `SabreObject` `0x801ED2A8`; 33
  icicle, 34 fire wave, 44 Rekkoha beam, 58 Z-Buster shot".

## `docs/engine.md`

- Game fields table, `+0x60`: "beaten Mavericks: bit n = the Maverick of stage n + 1; copied to
  `mega+0xC9` by `PlayerLoadForm` (0 in form 2): X's owned weapons and Zero's techniques are this
  same byte (`docs/players/zero.md`)". The present text "Zero's skill bits" is right but partial.
- Progress bytes paragraph: add the stage numbers 2 Blizzard Wolfang, 3 Blaze Heatnix, 8 Infinity
  Mijinion [likely: the Zero technique ids in the stun lists and the code]. Bits `0x02`, `0x04`,
  `0x80` then follow.
- Weapon ids table:
  - row "2, 11, 33 / 4, 13, 35 / 6, 15, 42 / 8, 17, 44": the third id is the Zero technique of
    the same Maverick: 33 Hyoroga icicle, 35 Rakukojin, 42 Ensuizan, 44 Rekkoha beam. Remove
    "which weapon makes them is open". (35 and 42 run `0x801ED2A8` in Zero's overlay; the old
    text read the address in X's RAM.)
  - row "24–31": "Z-Saber hits: 24–26 the ground combo, 27 air, 28 landing, 29 spin (after the
    double or super jump), 30 ladder, 31 wall"; add rows 36 Sentsuizan, 37 crouch slash, 38–41
    hang slash, 48 ceiling slash, 34 Shoenzan fire wave, 58 Z-Buster shot.
- `ApplyWeaponHit` bullet: "+1 for ids 24–31 and 37–41 with part `0x400000` or as Black Zero";
  "+1 for all ids when `mega+0x144` = 2"; "ids 26–31, 35, 46, 51, 57, 61, 62 set `mega+0xCD` = 1".
- The player section: "Black Zero (`game+0x5F` bit `0x20`, Zero only): damage taken / 2
  (`PlayerDamageScale`), no knock-back, energy × 4, black palette, sabre +1".

## `docs/players/steps-extra.md`

- Zero table: 53 and 54 run after the double jump too (`+0x86` = 6 is set by both the super jump
  and `TryDoubleJump` `0x801EC298`).
- 55: "skill `0x04`, up held, attack: Shoenzan"; 56 "Rakukojin (falls only)"; 57 "Sentsuizan";
  58 "Ensuizan"; 59 "Rekkoha"; 62 "Z-Buster"; 63 "Hyoroga attack"; link `docs/players/zero.md`.
- "Skill bits that start no step": `0x01` is Yammar Option (gauge 3), `0x02` Hyoroga (super jump),
  `0x40` Guard Shell (weapon type 7, started from the pause-menu close at `0x80034D78`). Remove
  "no caller found in the walk" and "The game's names … [open]".
- The open item "Zero's step bodies 48–63 in detail": done.

## `docs/players/map.md`

- Open items: remove "Zero's step bodies 48–63 in detail, and the official names of Zero's
  techniques" (done, `docs/players/zero.md`).

## `docs/players/air.md`

- `+0x86` row: "6 Zero's super jump **or double jump**".
- Air dash table, Zero row: "(overlay try `0x801EC298` first: the double jump with the jump
  button)".

## `docs/players/movement.md`

- Open item "Zero's idle (`0x801EC9C4`) and step 5": done (`ladder-hurt-events.md` and
  `zero.md`).

## `docs/TODO.md`

- Add: a Zero state during a technique (steps 48, 55–59) to test the timings; a state with Black
  Zero; what sets `mega+0x144`.
