# Reconstructed source (plausible, not original)

My guess at how the original C for Meta Wheel F could have looked in 2001 (Psy-Q SDK, PS1). It is not
compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/game.h`, `include/sound.h` | shared with the earlier intro enemies; `game.h` adds `main_object`, `MAIN_OBJECT_MAX` and `HitReactionDispatch`; `sound.h` adds `SE_WHEEL_HIT` (bank 2, id 4, the same id as `SE_D1000_RISE`) |
| `src/st02/m02_wheel.c` | the wheel (main object 02) and its stud lights (misc object 02) |

In the binary the wheel code is at `0x800EB450`–`0x800EBFEC` and the studs at `0x800EBFEC`–`0x800EC11C`
in overlay `02`, right after the Junkroid's shot.

## What is certain (from the binary)

- Every constant: HP 32, contact damage 4, the three boxes, acceleration `0xC00`, the speed cap
  `0xC000`, gravity `0x2000`, the timers 90 and 20, the off-screen ranges 128 and 96, the explosion
  count 5 and range 16, the debris animations 8–10, drop table 20, the sound (bank 2, id 4).
- The control flow of every function, the dispatch tables (states 0–3, steps 0–3 with step 0 = the
  engine hit reaction, TURN sub-steps 0–2), and the two switch tables of `turn_flip`.
- The struct offsets used (see `../pseudocode.md`).

## What is a guess

- The names of the functions, tables and constants.
- `wheel_move` as a separate function: the binary has the same code inlined twice (ROLL and FALL).
- The misc object's use of `atk_box` as the parent pointer (the binary stores the pointer at `+0x50`).
