# Reconstructed source (plausible, not original)

My guess at how the original C for Weather Analyze could have looked (Psy-Q SDK, PS1). It is not
compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/sound.h` | copied from Monbando |
| `include/game.h` | copied from Monbando; adds `stage_f`/`stage_f2`/`stage_f3` (`game+0x26`..`+0x28`) and a `CAMERA` struct for `camera_layer0` limits `+0x24`..`+0x2A` (names are guesses) |
| `src/st09/m14_weather_analyze.c` | the machine, its sensors, display and barrier (all main type 14) |

The code is at `0x800EB2D4`–`0x800EBFA4` in overlay `09`. The functions are in binary order.

## Certain

- Every constant (HP 32/16, contact 4, gravity `0x40000`, priorities 6/6/4/5, margins 40/64, 16/16,
  48/64, animation numbers, junk counts 4/3, index 1 and 5 and 3 special cases, camera value 224, the
  tile-copy destinations), the control flow and the six dispatch tables.

## Guesses

- All names. The camera field names (`limit_r`, `limit_l`, `limit_b`). `CopyBlock` = `CopyTiles`.
- The parent pointer is at `+0x80` (`work[2]` in this `object.h`).
- The rain object (stage-logic type 11, `0x800ED510`) is not reconstructed here; see `../pseudocode.md`.
