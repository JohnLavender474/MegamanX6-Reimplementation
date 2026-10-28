# Wolfloid (purple wolf): working notes

Stage 2 (Blizzard Wolfang, overlay 04), main type 6. Export `purple-wolf` (`enemy-bd435676eb`).
Proposed name `wolfloid`.

## Files

| file | what |
|---|---|
| `sheet.md` | the proposed `docs/enemies/wolfloid.md` |
| `pseudo.c` | reconstructed C (plausible, not original, not compiled), with hazard comments |
| `include/` | `object.h`, `game.h`, `sound.h` from `worm-spider`; `game.h` adds `dmg_tbl_normal`, `PLAYER_IS_ZERO`, `SmokePuff` |
| `doc-changes.md` | the other doc edits, exact text |
| `helpers-proposed.json` | names for `catalog/helpers.json` (overlay 4) |
| `kit/` | output of `python3 -m mmxdemake.rekit --state wolfang-2_ice-slope.state.gz --type 6 --name purple-wolf` |
| `at/` | `--at` listings of the 10 wolf functions that the kit did not walk |
| `scripts/peek.py` | prints the live type-6 main objects and type-5 visual objects of a state |
| `scripts/records.py` | the type-6 records of section 1 with the collision column under each |
| `scripts/find_spawns.py` | finds `addiu rX,zero,6` + `sb rX,1(..)` in the overlay (who makes a type-6 object) |

Run the scripts from the repo root with `PYTHONPATH=.`.

## Method

1. Checked for earlier work: no `docs/enemies/` sheet, no `catalog/helpers.json` name, no
   [`~/mmx-data/x6/re/`](../) folder. `docs/enemies/nightmare-freeze.md` names Ice Burst (stage 2 weapon)
   only. `catalog/enemies.json` has the placeholder `purple-wolf`. Nothing to extend.
2. Kit run. The kit took the entry `0x800EB5A0` and walked only the visual type 5 code. The entry
   loads the state table `0x800F6A84` with an `lb state` / `sll` / `addu` sequence that is
   interleaved with the `prev_x` stores; the kit listed the table under "Globals and data" and did
   not walk it. The table has 4 states and, directly after it, the 8-entry step table
   `0x800F6A94`. The kit also merged the visual `var` 1 table `0x800F6AD8` and the `var` 2 table
   `0x800F6AE4` into one 6-entry table. Both are kit issues (see `doc-changes.md`).
3. Read the 10 functions with `--at` (`at/*.txt`). Steps 2/3, 4/5 and 6/7 are pairs; `diff` of the
   listings shows that 4/5 and 6/7 differ only in the swapped animation numbers, and 2/3 differ in
   range, animations and the fire timing.
4. Looked at the sprites (`anim-*.png` of the export) to name the animations: 0–5 flat floor,
   6–11 and 24–29 the tilted bodies for the two slope directions, 12–17 and 30–32 breath clouds,
   18 the ball, 19–23 debris.
5. Found the carrier with `scripts/find_spawns.py`: `0x800EBE14` in stage object 7 state 0
   (`0x800EBAA0`, `var` case 12). Read only the parts that touch the wolf.

## Key facts and where they come from

- Set-up `0x800E9B30`: HP 12 (`a0 = 12` → `+0x5C`), contact 4 (`+0x60`), gravity `0x40000`,
  boxes `0x800F6720`/`24`/`28`, priority 5, the `var` switch `0x800E9864` (6 cases), the
  character test `lb mega+0x02`.
- Main `0x800E9C84`: order ContactWeapon, squeeze (`& 0x0C == 0x0C`, `& 3 == 3`), ContactMega,
  carrier or screen tests (64, 48), MoveObject2, AnimeTick, step table `0x800F6A94`.
- `timer % 200`: `0x51EB851F` multiply, `>> 6` (= / 200), then `× 25 × 8`.
- Ranges: `slti 129` (step 2), `slti 181` (steps 3, 6, 7), `slti 97` (steps 4, 5).
- Ball `speed_x`: multiply by `0xEA0EA0EB`, add, `>> 6`: division by 70 of `dx << 16`.
- Junk list `0x800F69F8` = `13 14 15 16 17` (animations 19–23). Drop table 17.
- Breath sound (2, 16) in `0x800EB750` and `0x800EB110`; ball sound (2, 15) in `0x800EB3C8`.

## Check against the save state (`wolfang-2_ice-slope`) [certain]

`scripts/peek.py`:

```
main 3 8008f11c flags 41 type 6 var 0 disp 1 state 1 step 6 step2 3 act4 0 pos (768, 1119) flip 00
  prio 5 anime 11 next 0 spd (0.000, -6.500) grav 4.000 tbl 800f6a00 atk 800f6724 hurt 800f6728
  dmg 800743dc hp 4 cd 4 terr 800f6720 coll 08 7C 63 rec 800f5fec
   record 01 06 00 11 00 03 40 04
visual 0 80093c98 type 5 var 2 state 1 step 0 step2 3 pos (670, 1073) flip 00 prio 4 anime 18
  spd (-1.600, -1.781) grav 0.094 atk 800f6ac4 hurt 800f6ac8 dmg 800743dc hp 2 cd 3 terr 800f6ac0
```

- Record 29 (id 0, kind 1). The player is X (`mega.var` 0), so step 6 (cannon on the slope that
  rises to the right) [matches].
- Sub-step 3 (fire), timer 63, animation 11 (fire, facing left) [matches].
- `speed_y` −6.5: gravity 4 clamped by `fall_max` [matches]. Floor flag 8. y 1119 + 16 = 1135 = the
  surface of cell value 25 (h 15 at the left edge of the cell at x 768) [matches].
- HP 4: it took 8 damage before the save.
- The ball: made at timer 12, so 51 updates old. Start (752, 1107) (step 6 facing left: x − 16,
  y − 12). `speed_x` = (640 − 752) / 70 = −1.6 [matches]. `speed_y` = 3 − 51 × 0.09375 = −1.78
  [matches]. x = 752 − 51 × 1.6 = 670.4 [matches]. y = 1107 − (3 × 51 − 0.09375 × 51 × 50 / 2) =
  1073.5 [matches]. Its counter (`+0x06`) is 3 = 51 mod 8 (smoke every 8) [matches].
- `wolfang-1_x-sabre` and `wolfang-3_falling-ice` have no live wolf and no live stage object 7.

## The ice

The wolf code has no ice test. Records 29, 31–36 stand on values 25/26 (the 2-cell slope that
rises to the right, 8 px per 16 px, on glass/ice: values 9/10 + 16); records 40–42 on 27/28 (falls
to the right). The id (0 or 1) chooses the tilted sprite set, not the terrain. The wolf never moves
in x, so it does not slide. Only the player slides on the ice (not part of this object).

## The carrier (stage object 7) [likely, read in part]

- Stage object 7 `var` 0 (records 58, 59 at (4992, 1376) and (5344, 1376)): `0x800ED150` waits
  until the player is at x ≥ 5137 and y ≥ 1473 (first, object x < 5312) or x ≥ 5345 and y ≥ 1489
  (second); then it locks the layer-0 camera x (4976 or 5328) and goes to step2 9.
- Step2 9 (`0x800EC7DC`): a block every 128 updates (64 when `game.difficulty` ≥ 2), up to 18,
  column `GetRNG % 6` at x = 5016 + 48 × column (first generator), at most 3 per column; each block
  is stage object 7 `var` 12.
- `var` 12 set-up: 50 % a wolf (`var` 4 or 5, 50 % each) with `+0x80` = the block; the block gets
  step2 6 and `speed_y` × 2 (−2 px/frame: falls 2 px per update).
- Step2 6 (`0x800EC628`): falls; collision only for y ≥ 1296; lands on a floor → step2 7.
- The block's state 4 comes from the crush code in its main (`0x800EBF18`: the player under it,
  the block lands → `SpawnJunk(3, 0x800F6B8C)`, state 4). Its state 2 comes from state 3
  (`0x800ECC10`). The wolf: block state 4 → dies with the drop; block state 2 → released.
- Not checked in play: no state of the lower room.

## Hazards

- **Missing step for `var` ≥ 6** (`0x800E9BC0`): `sltiu 6` skips the switch; the step stays 0
  (`HitReactionDispatch`). Safe: no record uses it. A reimplementation should reject the id.
- **Carrier pointer** (`+0x80`, `0x800E9D14` and the steps 2/3): never tested for 0 or for a reused
  slot. Safe while the block sets state 2 or 4 before it deletes itself and the slot is not reused
  in between [likely]. A reimplementation should hold a handle and release the wolf when the
  carrier is gone.
- **Released wolf with gravity 0**: after `var` −= 2 it keeps gravity 0 and no speed; it stands
  where the block left it. It falls only if something else moves it [likely: no code sets gravity
  again].
- **Shared y in steps 2/3 for ids 2/3**: the carrier test is `var == 4` (`var == 5`) and
  `act4 == 0`; after the release `var` is 2 or 3, so the follow stops. Order-safe.
- **Breath clouds read the wolf's state** each update (`+0x7C` owner pointer). The wolf deletes
  itself in its state 2 the update after death; the cloud sees state 2 in the visual loop of the
  death update (the main loop runs first) [likely for the order]. If the wolf slot were cleared
  first, the cloud would read a free slot (state 0) and keep running until its own end.
- No recursion (no self call; `scripts` not needed: the reconstruction has no call cycle).

## Open questions

1. The kit issues above (state table at the entry; merged `var` tables).
2. The block (stage object 7): its own sheet, and its states 2 and 4.
3. `0x8004A2B4`: called by the ball and by the Rescue Reploid bomb; makes `n` effect objects of
   type 9 at random points of the terrain box (mode 0). "Smoke" is a guess.
4. Official name: "Wolfloid" from a web search summary (Mega Man Knowledge Base text); the wiki
   page was not readable (HTTP 402 / bot block). The game text was not searched.
5. The breath has no freeze effect in this code (only contact damage 4). Whether `ContactMega`
   applies an ice effect by some other field is not checked.
