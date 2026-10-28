# Reconstructed source (plausible, not original)

My guess at how the original C for Meta Dridler could have looked (Psy-Q SDK, PS1). Not compiled, not a
matching decompilation. Conventions as in `../../junkroid/reconstructed/`.

## Layout

| file | contents |
|---|---|
| `include/object.h` | as in the Junkroid copy |
| `include/game.h` | the Hover Gunner copy plus `BGLAYER` (layer camera work, stride `0x54`), `CopyBlock`, `Quake`, `SpawnExplosionRand` |
| `include/sound.h` | plus `SE_DRL_DROP`, `SE_DRL_DRILL`, `SE_DRL_RISE` (bank 2, ids 1–3) |
| `src/st02/m00_dridler.c` | the drill (main object 00) and its two effects (misc object 00) |
| `src/st02/st02_tbl.c` | the table excerpt |

The code is adjacent in overlay `02` (`0x800E9F1C`–`0x800EAC8C`, with the update at `0x800EA1D8`); the
data is at `0x800FA99C`–`0x800FAADC`.

## What is certain (from the binary)

- Every constant: HP 1, contact damage 8, priorities 33 / 32 / 34, the drop speed `0x1000`, gravity
  `0x2000`, rise speed `0x8000`, the 128 px limit, the timers 90 / 45, 30 / 15, 60, the floor-break
  coordinates and the tile-copy source blocks, the explosion counts 4 and 3 and their masks 7 / 15,
  the quake arguments, the screen margins 80 / 160 and 48 / 64.
- The control flow, the sub-step tables, the `var` switch for the floor break, and how the effects
  watch the drill's `+0x82` / `+0x83`.
- The animation, box and tile-copy data, byte for byte.

## What is invented

- All names, including `BGLAYER` and its field names `y`, `dest_y`, `redraw` (only the offsets
  `+0x0E`, `+0x20`, `+0x4C` are known; their meaning is a guess).
- `Drl_Explode` and `Drl_CameraStill`: the binary has the code inline. `Drl_Explode` leaves `y`
  raised by 32 so that the callers can do more work at that height, as the binary does before
  `SpawnJunk`.
- Using `atk_box` / `dmg_box` of the misc object for its parent pointer, last value and y offset: the
  offsets (`+0x50`, `+0x54`, `+0x56`) are certain; writing them as casts of those fields is my guess.
- The comments and the includes.
