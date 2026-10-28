# Shield Sheldon (main 51, stage overlay 10): notes

## Method

- Kit: `python3 -m mmxdemake.rekit --state sheldon-17_boss.state.gz --type 51 --name shield-sheldon`
  (132 functions, 19 state tables, 1 neighbour: visual type 16). Output in `kit/`.
- Read `kit/summary.md`, `kit/calls.txt`, all of `kit/code.txt`, `kit/data.txt`.
- Extra code read with `--at`: `ContactWeapon` `0x80031474`, `ApplyWeaponHit` `0x800315CC` (the
  null-table case), `ContactMega` `0x80030ECC` (no zero-damage test), effect type 5 `0x800FA8C4`
  and its states `0x800FA644`, `0x800FA710`, `0x800FA798`, `0x800FA7B8`, `0x800FA81C`,
  `0x800FA89C` (found through the effect class table of stage 7: `0x8006DDDC + 7 × 8` →
  `0x800FB644`, entry 5).
- Scripts (run from the repo root with `PYTHONPATH=.`): `scripts/peek.py` (RAM reader),
  `scripts/probe_overlay.py` (overlay 10 against RAM; boss overlay search), `scripts/dmgtables.py`
  (tables, and the check against the executable file), `scripts/live.py` (live objects).

## Overlay

- Overlay 10 is 79 784 bytes, `0x800E9860`–`0x800FD008`. The boss code (`0x800F44A0`–`0x800FA8FC`)
  matches the file byte for byte. 115 bytes differ in all: the record lists (`0x800FAEF8`–
  `0x800FB26B`, the "spawned" bits) and the variables at `0x800FCFEC` (21 bytes).
- `0x800FA000` is inside overlay 10, so no boss overlay is loaded. No overlay 25–58 matches RAM at
  `0x800FA000`.

## Key facts and where they come from

- HP table `0x800E9E28` = `00 04 08 10`; `boss_hp_max = 32 + tbl[level]` (`0x800F5880`).
- Contact 10 at the set-up `0x800F44F8`; also at stun (`0x800F987C`) and the lunge (`0x800F8E68`,
  `0x800F90A0`).
- Stun test `0x800F4990`: results 5, 14, 36 → weapon ids 4, 13, 35. Invulnerability 30 (2 for
  results 25, 26), stun 80.
- Guard flag `+0x8E`, rule at `0x800F4A24`.
- THINK `0x800F5D58`; tables `0x800FCE24` (high) and `0x800FCEA4` (low), 16 entries of 8 bytes.
  First low pattern `GetRNG() & 7`.
- Step 3 route `0x800FCCE4` and points `0x800FCCF4`, jump table `0x800E9E08` (indices 1 and 4 can
  follow the player, nibbles of `+0x7E`). Speed `(act4 / 2 + 6) × 16 × rcos`.
- Step 4 shield logic: visual 16 `var` 0 `0x800FA128`/`0x800FA1B4`/`0x800FA230`/`0x800F65DC`/
  `0x800FA2B0`; `var` 1 `0x800F6794`/`0x800FA37C`/`0x800FA3D4`/`0x800F6978`/`0x800FA408`. They talk
  to the boss through his `act4` (1, 2, 3, 4).
- Step 5: plates `var` 16–19 made by `0x800F9E3C`; boss chase `0x800F50A0` (1 px/frame); freeze
  counter `0x800FD004` (= 8 at `0x800F9AB8`).
- Step 6: image made by `0x800F9EE4` (`var` 32); mirror point arithmetic at `0x800F5208`;
  shot pair `0x800F71F4` (`var` 32, then `var` 33 in the next free visual slot after it:
  `0x800F77A0(a0, 0)` searches forward from `a0 + 0xA0`; with `a1` ≠ 0 it searches backward). The
  pair link is `+0x8C` of `var` 33.
- Step 7: turrets made by `0x800F765C` (`var` 48–51, set 1); dash `0x800F55A0`, speed `rcos << 6`
  = 4 px/frame, × 5 / 4 on difficulty ≥ 2.
- Stun `0x800F9848`: speed 12 px/frame (`0x000C0000`), accel `0x4CCC`, accel off at |speed| ≤ 4,
  wall flag by facing, `Quake(10, 4, 2)`, 40 updates.
- Death `0x800F7968`, `0x800F79F4`, `0x800F4A74`; soul `var` 0x10 below 5401 updates of the fight
  timer `0x800FD000`.
- Damage tables (`scripts/dmgtables.py`, both match the executable file at RAM − `0x80010000` +
  `0x800`): `0x80074E7C` (own), `0x80074F1C` (step 2 only).
- Null damage table for main 51 `var` ≥ 16: their set-up never writes `+0x58`, and
  `GetMainObject` clears the slot. `ApplyWeaponHit` reads `0 + id × 2`: in the state, RAM
  `0x0`–`0x9F` is zero except bytes 3 (`0xEC`, id 1 damage −20) and `0x28`–`0x33` (ids 20–25:
  classes 1, 2, `0xAC`, `0x42`, 8, `0x40`). Class 0 with damage 0 goes to the damage path: HP 0 − 0
  ≤ 0 → kill result. So the objects react, never die, and add to the kill count.

## Checked against the save state (`scripts/live.py`)

| field | state | reading |
|---|---|---|
| main slot 0 | type 51, `var` 0, state 1, step 4, sub 3 | SHIELD THROW, waiting for the first release |
| position | (5424, 400) = room (272, 144), facing left | step 4 sub 1 with the player at room x 70 (≤ 160) |
| animation | 21, frame `02 00 01 0b` | sub 2 started animation 21; frame 1 (sprite 11) |
| HP / `boss_hp_max` | 32 / 32 | level 0 (`game+0x8D` = 5 → table value 0) |
| table | `dmg_tbl_deflect`, `+0x8E` 0 | the guard from the set-up; the second throw not yet |
| boxes | `0x800FC734` / `0x800FC738`, terrain 0 | set in sub 2 |
| contact | 10 | set-up |
| `+0x8A`, `+0x8B`, `+0x8C` | 0, 3, 1 | high mode, pattern 3 (4, 3), at entry 1 |
| fight timer `0x800FD000` | 105 | the intro ended 105 updates before |
| freeze `0x800FD004` | 0 | |
| `game+0x20`, `+0x24`, `+0x2E` | boss slot, 1, 1 | intro writes |
| visual / effect pools | empty | no shield yet |
| camera | x 5152, y 256, all limits fixed | |
| player | (5222, 427), step 2, HP 34, form 1 | |

Note: the brief says form 0; the state has `game+0x5E` = `mega+0xE7` = 1.

## Sprite export

`python3 -m mmxdemake.spriteexport --state sheldon-17_boss.state.gz` (no error):
`enemy-0d386ba53d`, 41 animations, type 51, live, bank 14 (162 sprites). Bank 15 (type 51 again)
has "no texture" in the state. The code uses set 1 (bank + 1, texture `0x540`) for animations 0, 1
(boss), 2, 35, 36 (turrets) and 3 (shields): these six strips are drawn from the wrong bank. Bank
sheet: `~/mmx-data/x6/sprites/banks/sheldon/sheldon-17_boss/bank-14-live-0.png`. Other exports of the
run: nightmare-virus, nightmare-virus-orb, `enemy-bff67c81a6` (type 13, the laser puzzle, 34
animations, search), boss-gate.

## Open questions

See the sheet.
