# Proposed changes outside `docs/enemies/shield-sheldon.md`

The main session applies these (the files have other owners).

## `catalog/enemies.json`

- `names`: `"enemy-0d386ba53d": "shield-sheldon"` (main type 51, stage 7; export from
  `sheldon-17_boss.state.gz`, bank 14, 41 animations).

## `catalog/helpers.json`

- Add the entries of `helpers-proposed.json`.
- The kit names `0x800FD000` and `0x800FD004` "guardian_box_bar+0x4 / +0x8". They are the
  Sheldon globals above (`guardian_box_bar` is `0x800FCFFC`, 4 bytes). The kit also names
  `0x800E9E08`, `0x800E9E28`, `0x800E9E30` "bug_yofs_tbl+n"; with the new entries the names are
  correct.

## `docs/engine.md`

- "The damage tables": add `dmg_tbl_sheldon` (`0x80074E7C`):

  | ids | damage |
  |---|---|
  | 0–15 | 1 2 2 2 5 2 2 – 2 1 2 2 2 8 2 2 |
  | 16–31 | D 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
  | 32–47 | 2 2 2 8 2 2 2 2 2 2 2 2 D 1 4 8 |
  | 48–63 | 2 1 2 2 1 D 1 127 3 8 4 4 15 5 15 3 |
  | 64–79 | D × 15, then 127 (id 79) |

  and `dmg_tbl_sheldon_guard` (`0x80074F1C`): D for every id except id 1 (2), id 54 (1), id 55
  (127), ids 7 and 23 (–).
- "Weapon ids": the boss stun ids so far: Rainy Turtloid 2, 11, 33; Shield Sheldon 4, 13, 35;
  Commander Yammark 8, 17, 44. Each set is n, n + 9 and one id of 33–44: probably the uncharged and
  charged special weapon of one slot, and one more form [guess].
- "Damage to an object": a new rule: **an object with no damage table** (`+0x58` = 0). Shield
  Sheldon's plates, image and turrets (main 51 `var` ≥ 16) never set `+0x58`.
  `ApplyWeaponHit` then reads the table at RAM address 0; in the save state most ids there are
  class 0 with damage 0, so a touching shot is a hit, the HP (0) stays 0, the result is a kill
  (`−(id + 1)`), the shot stops, and `game+0x94` (kill count) gets + 1. A damage of 0 goes to the
  same path as 127 in `ApplyWeaponHit` (`0x800318D8`) [certain for the code; the low RAM bytes
  depend on the BIOS].
- "Damage to the player": `ContactMega` has no test for contact damage 0; the base 0 goes to
  `PlayerDamageScale` (minimum 1) [certain for the code that was read].
- "Animations", "More than one sprite set": add Shield Sheldon: `0x800F5F04(obj, set)` set 1 =
  bank + 1, texture `0x540`; used for boss animations 0, 1, turret animations 2, 35, 36 and shield
  animation 3. Effect class: stage 7 entry 5 of the effect table (`0x800FB644`) is in overlay 10
  (`0x800FA8C4`, the boss's warp flashes).
- "Shots and attached objects": engine objects 24 and 26: Shield Sheldon also uses them (intro and
  death), as Turtloid and Yammark do.

## `docs/stages/sheldon.md`

- "Objects": main 51: Shield Sheldon (`docs/enemies/shield-sheldon.md`); the record is confirmed by
  the live object in `sheldon-17_boss` (record pointer `0x800FB268`, the last record of the list).
- Add `sheldon-17_boss` to the state table: X form 1 (`game+0x5E` = 1, `mega+0xE7` = 1), at
  (5222, 427), checkpoint 3, camera x 5152, y 256, all limits fixed at x 5152 and y 256 (the boss
  room is one screen: x 5152–5471, y 256–495). The boss is in step 4 (shield throw), HP 32.
- "Sprite exports": `enemy-0d386ba53d` (Shield Sheldon, 41 animations; strips 0–3, 35, 36 are from
  the wrong bank). Bank 15 (type 51 again) has no texture entry; the boss uses texture `0x540` for
  it.
- "Save states that are still needed", item 1: done (`sheldon-17_boss`).

## `docs/TODO.md`

- Re-export Shield Sheldon's set-1 animations (0–3, 35, 36) from bank 15 / texture `0x540`. The
  exporter must know the "bank + 1 by animation" rule for type 51 (as for Rainy Turtloid). A save
  state with the turrets out or in the bounce would have that texture in use.

## `docs/process/process-log.md`

- Shield Sheldon (main 51, overlay 10): behaviour sheet, pseudo code and reconstructed C from
  `sheldon-17_boss`; kit `--type 51`; export `enemy-0d386ba53d`.
