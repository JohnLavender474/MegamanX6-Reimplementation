# Reconstructed source (plausible, not original)

My guess at how the original C for Sea Attacker ST could have looked in 2001 (Psy-Q SDK, PS1). It is
not compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/game.h`, `include/sound.h` | copied from Meta Wheel F; `game.h` adds `current_main`, `GetEffectObject` (`0x8002C398`) and `SpawnShotBurst` (`0x80049D90`); `sound.h` adds `SE_SEA_SHOT` (= `SE_E_SHOT`, bank 2, id 26) |
| `src/st09/m10_sea_attacker.c` | the seahorse (main object 10) and its bubble shot (visual object 9) |

In the binary the code is at `0x800EA880`–`0x800EB2D4` in overlay `09`. The functions in the C file
are in the binary order: shot set-up and move first, then the two entries, then the seahorse, and the
shot delete last.

## What is certain (from the binary)

- Every constant: HP 6 and 1, contact damage 4 and 4, priorities 7, 18 and 5, the range 96, the
  timers 30, 20, 40, 40 and 8, the speeds `0x3C000` and `0x40000`, the window `0x18000`, the random
  mask `0x1F`, the shot offsets 6 and 20, the off-screen ranges (64/32, 32/16), the debris list
  4–8, drop table 19, the sound (bank 2, id 26), the damage tables `0x800743DC` and `0x8007429C`.
- The control flow of every function and the five dispatch tables.
- The struct offsets used (see `../pseudocode.md`).
- The unused `ratan2` call in the shot set-up.

## What is a guess

- The names of the functions, tables, macros and constants.
- `face_tm` and `rise_ofs` are written through `current_main` in the binary (the scratchpad pointer
  `0x1F80004C`), not through `op`. The C keeps that. It may have been a macro in the original.
- `dmg_tbl_mecha_s` for `0x800743DC` and `dmg_tbl_shot` for `0x8007429C`, as in the Junkroid
  reconstruction.
- `tama_sub` at `+0x07` and `tama_tm` at `+0x88` of the visual object.
