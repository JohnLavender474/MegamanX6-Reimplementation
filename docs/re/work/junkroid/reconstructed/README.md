# Reconstructed source (plausible, not original)

This is my guess at how the original C for the intro robot could have looked in 2001 (Psy-Q SDK, PS1).
It is not compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h` | object work struct `OBJECT`, `HITBOX`, `ANIME`, `ENEMYSET`, flag `#define`s |
| `include/game.h` | `GAME` work (difficulty at `+0x68`), engine prototypes, damage tables |
| `include/sound.h` | sound effect numbers used here |
| `src/st02/m01_mecha_s.c` | the robot (main object 01) and its shot (visual object 00) |
| `src/st02/st02_tbl.c` | the excerpt of the stage 02 object tables that point to the two update functions |

In the binary, the robot and the shot code are adjacent in overlay `02` (`0x800EAC8C`–`0x800EB44C`), and
their data is adjacent at `0x800FAADC`–`0x800FAB7C`. This is why I put both in one source file.

## What is certain (from the binary)

- Every constant: HP 12, contact damage 4 and 1, the 112 / 113 px range test, the timer values 1 and 120,
  gravity `0x4200`, shot speed `0x20000`, the offsets +16 / −12, the screen ranges 64, 32, 16, the drop
  table numbers 21 and 14, the sound numbers (bank 2, 26 / 33), the debris count 4.
- The control flow of every function and the order of the calls in it. The routine boundaries and the
  dispatch tables (states 0–3 with 2 and 3 sharing one routine; steps 0–3 with step 0 = an engine
  routine; shot states 0–2).
- The animation data and the box data, byte for byte.
- The struct offsets that the code uses (see `pseudocode.md`).
- The double copy of `flip` into the new shot. The binary writes it twice.

## What is invented

- All names: files, functions, enums, `#define`s, struct fields, the `M01_` / `V00_` prefixes. The X4
  decompilation suggested the pattern "per-object file, state table, sub-state table, init / main /
  delete", but the names here are mine.
- The use of `enum`s and `#define`s instead of literals, and the static helper functions
  `Mecha_TurnMega` and `Mecha_InRange`. The binary has this code inline at each use; the original could
  have used a macro or written it out.
- The file names, header split and the Psy-Q `#include`s (`<libgte.h>` and `<libgpu.h>` are not needed by
  this code; I included them because an object file of that time usually did).
- The comments.
- The `GAME` struct layout outside the two known fields, and the `OBJECT` padding names.
- The exact types (`char` versus `u_char`) of some fields. The loads in the binary show the signedness
  that the compiler used, and I followed that where I saw it.
