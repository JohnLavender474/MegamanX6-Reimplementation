# Proposed doc changes: Mettaur D2 and Guardian

The sheets `docs/enemies/mettaur-d2.md` and `docs/enemies/guardian.md` are written. These changes go
to files that this task did not own. Helper names: `mettaur-guardian-helpers-proposed.json` (same
folder; same format as `catalog/helpers.json`).

## `docs/engine.md`

1. Drop table rows (section "Death, debris and drops"):
   - row 18, user column: add "Mettaur D2" →
     `| 18 | 192 | 30 | 7 | 16 | 10 | 1 | Hover Gunner, Killer Moth, Worm Spider, Mettaur D2 |`
   - row 20, user column: add "Guardian" →
     `| 20 | 128 | 40 | 56 | 24 | 8 | 0 | Meta Wheel F, Batton Bone B81, Guardian |`
2. Under "Drops", a new bullet:
   - "**No drop for weapon id 55.** Mettaur D2 and the Guardian skip `DropItem` when the kill
     result of `ContactWeapon` is −56 (weapon id 55). `dmg_tbl_normal` gives id 55 127 damage.
     Which weapon id 55 is, is not known [certain for the code]."
3. Under "Damage to an object", a new bullet after the `ContactWeapon` bullet:
   - "**Hit tag.** `ApplyWeaponHit` returns 126 when the object's `+0x65` equals the shot's tag
     (`+0x64`), and else stores the tag. `ContactWeapon` clears `+0x65` when no shot touches the
     hurt box. So an object that calls `ContactWeapon` twice in one update with two boxes (the
     Guardian: shield with `dmg_tbl_deflect`, then body with `dmg_tbl_normal`) cannot be hit twice
     by one shot in that update [certain for the code]."
4. Under "Animations" (the "More than one sprite set" bullet) or under "Object classes":
   - "**A type can have two sprite banks.** The stage type list (`0x800733C4`, pointer per stage ×
     8 + area × 4; byte list, `0xFF` ends) can hold a type twice. `GetSpriteBank` returns the first.
     Stage 7 and stage 22 list main type 30 twice; the Guardian with `var` ≥ 16 takes the texture
     (`bank_tpage[bank + 1] >> 7`, table pointer at `0x80095098`), the palette (x = ((bank + 1) × 4
     + 32) mod 16 in 16 px units, y = 480 + (bank + 9) / 4) and the sprite data of bank + 1. With
     bank + 0 the same formulas give the palette and texture that the spawner sets (checked on the
     live Guardian and Mettaur D2) [certain for the code]."
5. Weapon ids table: no change. Add to the open questions: "Which weapon is id 55 (no drop for
   Mettaur D2 and the Guardian; 127 damage in `dmg_tbl_normal`)."

## `docs/stages/sheldon.md`

1. Objects table, the two rows:
   - `| main 27 | 4 | Mettaur D2 (`docs/enemies/mettaur-d2.md`); all `id` 1, at y 432, on the hidden floors 73, 75, 76 and 78: it paces on one hidden floor and turns at its ends |`
   - `| main 30 | 12 / 6 | Guardian (`docs/enemies/guardian.md`); `id` 0 faces left (12), `id` 1 faces right (5), `id` 16 is the large Guardian (sprite bank 8) at (3928, 664) |`
2. In "The hidden glass floors", a bullet: "The four Mettaur D2 records (88–91) stand on floors 73,
   75, 76 and 78. With `id` 1 a Mettaur turns at a floor end (its ledge probe is 11 px in front and
   21 px down), so it walks to and fro on one floor while the floor is not shown, and shows where
   the floor is [certain for the code; likely in play]."
3. Records 92–109 (type 30) with `id` and kind: the table is in
   [`~/mmx-data/x6/re/guardian/notes.md`](guardian/notes.md) ("Records").
4. The two live objects: `sheldon-6` main slot 4 = Mettaur D2 from record 89; `sheldon-8` main slot
   10 = Guardian from record 108.
5. Stage 22 (`sheldon-11_laser-shot`): records 26–28 are Guardians (type 30, `id` 0; record 27 kind
   1). The code is the same as in overlay 10. Its stage type list also holds type 30 twice (banks 6
   and 7) and type 27 (bank 8).

## `docs/TODO.md`

- The item "Shield Sheldon: read main type 27 (Mettaur D2) and main type 30 (Guardian) …": mark the
  two objects done (sheets `mettaur-d2.md`, `guardian.md`); keep the rest of that item.
- New items:
  - `[ ]` Play checks: Mettaur D2 guard timing (9 / 60 / 80 updates), the shot when the player is
    behind it, the "never shoots again" lock after a guard that interrupts a shot, and the pace on
    the hidden floors; Guardian cycle (297 updates, 4 shots), open window (updates 22–149), the
    back-half rule and the 188 px rule.
  - `[ ]` `spriteexport`: a type listed twice in the stage type list (main 30 in stages 7 and 22)
    gets an export only for its first bank. The large Guardian (`id` 16, bank 8) needs an export.
  - `[ ]` A save state with the large Guardian (`id` 16, (3928, 664)) live.
  - `[ ]` Which weapon is id 55.

## `docs/process/process-log.md`

One line, for example:

- "2026-10-21: Mettaur D2 (main 27) and Guardian (main 30) of Shield Sheldon's stage read from
  overlay 10, checked against `sheldon-6`, `sheldon-8` and `sheldon-11` (stage 22 has the same
  Guardian code). Sheets `docs/enemies/mettaur-d2.md` and `guardian.md`; notes, pseudo code and C
  in [`~/mmx-data/x6/re/mettaur-d2/`](mettaur-d2/) and `guardian/`. `spriteexport` re-run on `sheldon-8` (the
  folders are now `mettaur-d2` and `guardian`; the Guardian export has a live source)."

## Title convention (for the main session)

The guide (`docs/re-guide.md`) puts the overlay number in the title ("stage 10"). The two other
Sheldon sheets (`nightmare-bug.md`, `laser-reflector-puzzle.md`) use "stage 07", the stage number.
The two new sheets follow the guide. One of the two conventions should be applied to all four.
