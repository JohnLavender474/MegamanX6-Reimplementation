# Reconstructed source (plausible, not original)

My guess at how the original C for Monbando could have looked in 2001 (Psy-Q SDK, PS1). It is not
compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/game.h`, `include/sound.h` | copied from Meta Wheel F; `object.h` adds `ANM_BACK` (event flag 3) and `HIT_BLOCKED` (127); `game.h` adds `Quake2`, `dmg_tbl_normal` (`0x800743DC`), `dmg_tbl_deflect` (`0x8007433C`), `dmg_tbl_e_shot` (`0x800741FC`) and `PLAYER_CHAR()` (`mega.var`); `sound.h` adds `SE_MONB_PUNCH` 27, `SE_MONB_MISSILE` 28, `SE_MONB_SLAM` 29 (bank 2) |
| `src/st09/m09_monbando.c` | Monbando (main object 09) and its shots (visual object 07) |

In the binary the code is at `0x800E9D64`–`0x800EA880` in overlay `09`. The functions in the C file are
in the same order as in the binary: `monb_main`, `shot_init`, `shot_move`, `m09_monbando` (entry),
`monb_init`, `monb_delete`, `monb_wait`, `monb_fire`, `monb_recover`, `monb_fall`, `monb_fall_check`,
`shot_delete`, `e07_monbando_shot`.

## What is certain (from the binary)

- Every constant: HP 32 and 6, contact damage 6, 4 and 8, gravity `0x4200`, the wait 61, the wave count
  3 and gap 6, the speeds `0x28000` and `0x30000`, the homing timer 50 and limit 40, the homing step
  2 px and range 8 px, the offsets 36/8, 32/16, 48/16, the off-screen ranges 64/32 and 32/16, the
  debris animations 7–10, drop table 21, the sounds (bank 2, ids 27–29), `Rumble(5)`, `Quake2(8, 2, 2)`.
- The control flow of every function and the three dispatch tables.
- The struct offsets used (see `../pseudocode.md`).

## What is a guess

- The names of the functions, tables, constants and the three damage tables.
- `Quake2` as a name: the function is a copy of `Quake` that writes other bytes of the camera work.
- `PLAYER_CHAR()`: that `mega.var` is the character number (0 = X).
- `parent` at `+0x7C` and `aim_t` at `+0x8C` as macros over `timer` and `work[]`.
- Death and squeeze death as two copies of the same block: the binary has the code twice.
