# Reconstructed source (plausible, not original)

My guess at how the original C for Batton Bone B81 could have looked in 2001 (Psy-Q SDK, PS1). It is
not compiled for the PS1 and it is not a matching decompilation. It passes `gcc -fsyntax-only` with
empty stand-ins for `libgte.h` and `libgpu.h`.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/sound.h` | copied from Meta Wheel F, no change (this object plays no sound) |
| `include/game.h` | copied from Meta Wheel F; adds `AimAtPoint` (`0x8002D354`) and `STAGE_AREA()` (the halfword at `game.stage`, `0x800CCEDC`) |
| `src/st09/m18_batton_bone.c` | the bat (main object 18) and its bomb (visual object 17) |

In the binary the bat code is at `0x800EBFA4`–`0x800ECB48` and the bomb at `0x800ECB48`–`0x800ECD20`
in overlay `09`. The functions are in the binary's order, with the entry `m18_batton_bone` after
`bat_main`, as in the binary.

## What is certain (from the binary)

- Every constant: HP 3 and 1, contact damage 3 and 5, the boxes, the speeds `0x30000`, `0x38000`,
  `0x5000`, `0x18000`, `0x20000`, `-0x10000`, the lift `0x100`, gravity `0x3000`, `accel_x` `-0x1000`,
  the chase offset `0x500000` (80 px) and margin `0x20000`, the timers 200, 5, 13, 0–63 and 300, the
  aim period 4, the off-screen ranges 32 and 16, the debris animations 4–7, drop table 20, the
  damage tables (`dmg_tbl_mecha_s` = `0x800743DC`, `dmg_tbl_shot` = `0x800741FC`, as named in the
  Junkroid reconstruction).
- The control flow of every function and the dispatch tables (states 0–3, steps 0–8 with step 0 = the
  engine hit reaction, every sub-step table).
- The struct offsets used (see `../pseudocode.md`).

## What is a guess

- The names of the functions, tables, macros and constants.
- The work fields as `long` at `work[2]`, `work[10]`, `work[18]`, `work[22]` (`+0x80`, `+0x88`, `+0x90`,
  `+0x94`): the binary uses 32-bit loads and stores there. The cleared words `+0x84` and `+0x8C` have
  no name.
- The bomb's parent pointer stored over `timer` (`+0x7C`, a 32-bit store in the binary).
- `FLIP_IDX`: the binary indexes the speed tables by the byte offset `(flip >> 4) & 4`.
- `AimAtPoint` taking 16.16 coordinates as `long`.
- The misc table entry 56 (`0x800ECBAC`, inside `bomb_init`) is not reconstructed: it is not a usable
  function.
