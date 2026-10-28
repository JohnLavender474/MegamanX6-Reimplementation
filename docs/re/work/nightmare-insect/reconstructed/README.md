# Reconstructed source (plausible, not original)

My guess at how the original C for Nightmare Insect could have looked in 2001 (Psy-Q SDK, PS1). It
is not compiled for the PS1 and it is not a matching decompilation. It passes `gcc -fsyntax-only`
with stub `libgte.h` / `libgpu.h` headers.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/sound.h`, `include/game.h` | copied from Meta Wheel F; `game.h` adds `VISUAL_OBJECT_MAX`, `visual_object[]`, `TerrainPointCheck` (`0x80030CCC`) and the damage tables `dmg_tbl_small` (`0x800741FC`), `dmg_tbl_deflect` (`0x8007433C`), `dmg_tbl_insect` (`0x8007479C`); `sound.h` adds `SE_INSECT_BLADE` (bank 2, id 62) |
| `src/st03/m23_nightmare_insect.c` | the insect (main object 23) and its shots and hatchlings (visual object 27) |

In the binary the code is at `0x800EC8F0`–`0x800EE154` in overlay `03`. The functions in the file
are in the binary order, from `insect_main` (`0x800EC8F0`) to `kid_hatch` (`0x800EE118`). The small
helpers at the top (`count_hatch`, `spawn_kid`, `face_mega`, `aim_mega`) are inlined in the binary.

## What is certain (from the binary)

- Every constant: HP 32 / 3, contact damage 8 / 6 / 3, the boxes, the speeds (`0x30000`,
  `0x18000`, `rcos * 80`, `rcos * 48`), the timers 61, 91, 161, 20, 241, 51, the distances 64, 8 /
  32, 32 / 16, 16, the counts 4 and 8, the offsets ±16, ±32, −16, the off-screen ranges 64 and 48,
  the bottom limit 301, the debris lists, drop table 21, the sound (bank 2, id 62), the kill
  results −2, −11, −55.
- The control flow of every function and the ten dispatch tables.
- The struct offsets used (see `../pseudocode.md`), including the visual object's work fields
  `+0x7C` (parent), `+0x8C` (timer), `+0x8E` / `+0x90` (target).

## What is a guess

- The names of the functions, tables, macros and constants.
- The use of `timer` (`+0x7C`) as the parent pointer in the visual object: the binary stores a
  32-bit pointer there.
- `visual_object[]` as a byte array: the slots are `0xA0` bytes, larger than `OBJECT` (`0x9C`).
- The order of the inlined `count_hatch` test in `insect_idle` (the binary has it once per branch).
