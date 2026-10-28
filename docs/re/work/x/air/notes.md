# Notes: player air movement (part 2b)

## Method

- Kit: `../kit/` (X, `intro-1_start`), no rebuild. Functions printed with `../movement/fn.sh`
  (run from [`~/mmx-data/x6/re/x`](../); the address must be in lower case).
- Code and tables outside the kit: `scripts/rangedis.py <state> <lo> <hi> [words]` (disassembly or
  words from the RAM of a save state), `scripts/fieldscan.py <state> <hex offset> s|l` (all loads
  or stores with that immediate offset, to find who reads a constant or writes a field).
- Animation timings: `scripts/anim_show.py ~/mmx-data/x6/sprites/players/x/animations.json <n…>`.
- Numbers: `scripts/airsim.py` (jump, kick, fall, dash, with the game's order: move, then
  gravity, then the fall clamp).
- Read in order: steps 6, 7, the entries `0x8003AF1C`, `0x8003AFF4`, the air tries (`0x8003A2CC`,
  `…A33C`, `…A394`, `0x8003906C`, `0x80039984`, `0x8003A488`), steps 9, 10, 11, 13 with their
  entries, `0x80039DFC`, steps 27, 28, 31, the armour tries `0x8003F200`, `…F28C`, `…F32C`, the
  Falcon step 35 (overlay 0) and `0x8003A0E8`.

## Key facts and where

| fact | where |
|---|---|
| jump release sets `speed_y` = 0 | step 6 `0x80035670`, kick sub 1 `0x80036D9C` |
| jump to fall keeps the jump gravity | step 6 `0x80035770`: no gravity write |
| dash jump by `+0x84` or dash held | `0x8003AF50` |
| no wall grab for 10 updates after a jump | step 6 `0x80035714` (`+0x8B`) |
| wall tiles 56–63, `+0x4A`, spikes `+0x79` | `0x80030C58`, table `0x80010F18` |
| wall slide speed `+0x110`, 0 in form 2 | `0x8003B57C`; `PlayerPhysicsParts` |
| wall kick 5.625, 10 updates fixed | `0x8003B50C`, `0x8003B51C` |
| air dash: X only in form 4; Zero always | `0x80039138` |
| air dash timer 18 (10) | `0x8003B2D8` |
| shooting ends X's air dash | step 13 `0x800371C4` → `0x8003B360` |
| step 27/28 are rope and bar hangs, not wall steps | `0x80039984`: up held + tile 46 / 37–45 |
| step table has more than 32 entries | `player_steps` words 32–77; stores of 33, 35, 74 to `mega+0x05` |
| super jump rises 144 px or more, then 5 updates | `0x80036584`, `0x800365B4` |
| super jump holds the ceiling 180 updates (Zero 120) | `0x80036544` |

The form names: `game.armors` (`game+0x5F`) bit `0x01` is set in all states and the X form is 1
everywhere; `docs/enemies/light-capsule.md` gives bit `0x02` Shadow, `0x04` Blade, `0x08`
Ultimate, so form = bit number + 1 [likely]. A public mod tool (mmx-reference, facts only) names
the instruction at `0x8003F29C` (in `0x8003F28C`) "Falcon Armor jump air dash" and the one at
`0x8003F39C` (in `0x8003F32C`, `+0x86` = 7) "Blade Mach Dash repetitions". Form 2 has the spike
walls (`+0x79`), no wall slide and the super jump with the ceiling hold: Shadow Armor [likely].

## Checks against the save states

- `+0xE7`: 1 in `intro-1_start`, `intro-*`, `yammark-1_start`, `yammark-2_boss`, `turtloid-1`
  (X); 5 in `turtloid-6_zero-player`. `game+0x5F` = 1 (X states), `0x11` in `turtloid-8`.
- `player_steps` words 32–86 are the same in `intro-1_start`, `yammark-1_start` and
  `turtloid-6_zero-player`.
- The constants `+0x100` … `+0x130` are the 2a values in all three states.
- No state is in the air, so the numbers are from the code only.

## Numbers (from `scripts/airsim.py`)

- Jump: 23 updates up, 68.46 px; with part bit `0x8`: 31 updates, 90.28 px.
- Fall from rest: `fall_max` after 26 updates; 16 px in 12, 32 px in 17, 64 px in 23 updates.
- Wall kick: 44.65 px in the 10 fixed updates, 64.20 px in 22 updates with jump held.
- Air dash: 18 moves, 90.5 px (6.5 down to 4.125); with part bit `0x10`: 10 moves, 74.92 px.
- Super jump: 13 moves of 11.625 = 151.125 px.

## Corrections to the map (`docs/players/map.md`)

- 9 wall grab, 10 wall kick, 11 wall slide (and push-off), 27 rope hang, 28 bar hang.
- 13 is the generic air dash (X only in form 4, Zero always); X form 1 uses step 35, form 3
  step 74 (overlay).
- The stores of 33 and 74 are to `mega`: steps 32–77 exist and are overlay steps.
- Step 31 has six sub-steps (`0x80073C58`), not "anims 84, 85, 140 … sub-step 4" only.

## Open

- `0x80038594` (bar dash step), `0x8003F700` (X on the ceiling), steps 33, 35 (beyond sub 1), 74.
- Who sets `+0x87` and `+0x92`; `+0xA4` (walk-off is off while > 0).
- Tile 47 (accepted by `HangExit`, moved by `BarSnapY`), tile 57 above the super jump.
- The effects of type 33 (var 0, 1, 3), 65 (splash), the objects of `MakeObject`.
- Animations 83, 84, 169, 189 show wrong sprites in the X export (frame set 1 of the loaded
  member); animation 85 is not in the export.
- Play checks: jump height, the wall grab time (8 updates), the air dash hover (about 4 updates),
  the shot ending the air dash, the Falcon dash with the jump button.

## Recursion and re-entry

- **`RopeDashStep` ↔ `RopeClimb`** (reconstruction only). The game has the climb code inline in the
  rope dash (`0x80038788`); the reconstruction calls `RopeClimb` instead. `RopeClimb` calls
  `RopeDashStep` only while `dash_a` (`+0x84`) is not 0, and `RopeDashStep` calls `RopeClimb` only
  after `ClearDashFlags` (`0x8003CD9C`: `+0x84` = `+0x86` = 0). So the cycle ends after one call. A
  change that leaves `dash_a` set on that path would loop.
