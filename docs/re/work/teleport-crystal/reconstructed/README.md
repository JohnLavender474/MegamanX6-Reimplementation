# Reconstructed source (plausible, not original)

My guess at how the original C for the teleporter crystal (main object 28) could have looked
(Psy-Q SDK, PS1). It is not compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h` | copied from Monbando, no change |
| `include/game.h` | from Monbando; `GAME` gets `exit_f` (`+0x0F`), `stop[10]` (`+0x10`), `unk1c`, `checkpoint` (`+0x1D`), `char_state` (`+0x26`); `BGLAYER` is rewritten with the real offsets of the camera position (`+0x0A`, `+0x0E`), the limits (`+0x1C`–`+0x2A`), `follow` (`+0x44`) and `redraw` (`+0x4C`); new externs for the fade, the spawner and the player clean-up calls |
| `include/sound.h` | adds bank 1 and `SE_WARP` (bank 1, id 1) |
| `src/st03/m28_teleport_crystal.c` | the crystal |

In the binary the code is at `0x800EE5CC`–`0x800EF304` in overlay `03` (Yammark). The same code,
with the same data, is at `0x800EF6FC`–`0x800F0434` in overlay `09` (Turtloid).

## What is certain (from the binary)

- Every constant: HP 32, contact damage 4, the three boxes, priority 6, the split time 29, the delay
  90, the fade speed 8, the ±64 px start of the join, the spawn rectangle (−48, +368, −48, +288),
  the four destination entries, the checkpoint values, the `0x40` bit of the exit byte.
- The control flow of every function and the four dispatch tables, including the `var` switch at
  `0x800E98F4`.
- The field offsets (see `../pseudocode.md`).

## What is a guess

- All names. In particular `exit_f`, `stop[]`, `follow`, `redraw`, `fade_dir`, `bg_refresh`,
  `checkpoint_set` and the five `Mega*` clean-up functions.
- The loops over the three layers and over `stop[]`: the binary writes each field one by one.
- The player fields above `0x9C` are written through `MEGA_B()`, because `OBJECT` does not cover the
  player's larger struct.
