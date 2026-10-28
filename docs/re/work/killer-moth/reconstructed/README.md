# Reconstructed source (plausible, not original)

My guess at how the original C for Killer Moth could have looked in 2001 (Psy-Q SDK, PS1). It is not
compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/game.h`, `include/sound.h` | copied from Meta Wheel F. `game.h` adds `CUR_MAIN` and `CUR_MISC` (the scratchpad pointers `0x1F80004C` and `0x1F800064`) and the field `scr_x` (`+0x0A`) in `BGLAYER`; `sound.h` adds `SE_MOTH_POWDER` (bank 2, id 5) |
| `src/st03/m04_killer_moth.c` | the moth (main object 04), its powder shot (visual object 02) and its exhaust flame (misc object 04) |

In the binary the code is at `0x800EAE94`–`0x800EBF6C` in overlay `03`. The functions in the file are
in the same order as in the binary: `swoop_move`, `enter_wait_view` and `enter_fly_home` come first,
then the three entries, then the rest.

## What is certain (from the binary)

- Every constant: HP 8, contact damage 4 (moth and powder), the four boxes, priority 5 / 4 / 6, the
  HOVER timer 180, the distance 48, the SWOOP target offsets 48 and 64, the speed factors
  `160 / 4` and `160 / 8`, the arrival limit 3, the circle start angle 3072, radius 16, step ±32, the
  shot timer 20, the shot count 8, the powder speed `-0x3FFF`, the off-screen ranges (80 / 160,
  48 / 64, 32 / 32, 16 / 16), drop table 18, the sound (bank 2, id 5).
- The control flow of every function and the eight dispatch tables.
- The struct offsets used (see `../pseudocode.md`). The accesses through `CUR_MAIN` and `CUR_MISC`
  are where the binary loads that pointer instead of using `self`.

## What is a guess

- The names of the functions, tables, macros and constants.
- The C types of the work fields (`shots`, `angle`, `spin`, `flame_mode`, `centre_x`, `centre_y`
  in `work[]`; the flame's last mode in the `dmg_box` slot; the powder's parent in the `timer` slot).
- `scr_x` as the name of the camera field at `+0x0A`.
- The data tables (`moth_anime`, the boxes, `moth_delay`, `moth_point`) are declared `extern`; their
  contents are in `../kit/data.txt`.
- `flame_follow` writes `op->disp = mode` and `op->disp = act`: in the binary the value comes from the
  register that holds the mode or the parent state, both 1 there.
