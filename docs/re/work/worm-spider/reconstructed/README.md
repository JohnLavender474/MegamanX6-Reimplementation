# Reconstructed source (plausible, not original)

My guess at how the original C for the Worm Spider could have looked in 2001 (Psy-Q SDK, PS1). It is
not compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h` | copied from Meta Wheel F, no change |
| `include/game.h` | copied from Meta Wheel F; adds `cur_main`, `cur_misc` (the scratchpad pointers `0x1F80004C`, `0x1F800064`), `GetEffectObject` (`0x8002C398`) and `SpawnHitEffect` (`0x80049D90`) |
| `include/sound.h` | copied from Meta Wheel F; adds `SE_WORM_MOVE` (bank 2, id 6) and `SE_WORM_SHOT` (bank 2, id 7) |
| `src/st03/m03_worm_spider.c` | the worm (main object 03), its effects (misc object 05) and its energy ball (visual object 03) |

In the binary the code is at `0x800E99D4`–`0x800EAE94` in overlay `03`. The functions in the C file are
in the same order as in the binary.

## What is certain (from the binary)

- Every constant: HP 16, contact damage 4 (worm and shot), the boxes `0x800F6CBC`, `0x800F6CC0`,
  `0x800F7154`–`0x800F715F`, the own-box offsets 26 and −30, the height table `0x800F70C4`, the
  proximity 96, the timers 30/15, 30, 20, 90/45, 120, the jump values `0x88888`, `0x48D1`, `0x80000`,
  `0x8000`, the shift 11, the shot speed factor 80, the screen ranges (80/160, 48/64, 32/32, 16/16),
  the ripple blink 60, the spark offsets ±12 and ±30, drop table 18, the sounds (bank 2, ids 6 and 7).
- The control flow of every function, the dispatch tables (states 0–3, steps 0–7 and their sub-steps,
  shot states 0–2, effect states 0–3) and the two `var` switches of the effect object.
- The struct offsets used (see `../pseudocode.md`).

## What is a guess

- The names of the functions, tables, macros and constants.
- The use of `work[2..9]` for the own box, the base y, the jump counter and the hit flag. The binary
  reaches these bytes (`+0x80`–`+0x87`) through `current_main`, so the source likely did the same.
- The effect object's use of `atk_box` as the parent pointer and of `dmg_box` as its timer (the binary
  uses `+0x50` and `+0x54`); the shot's use of `timer` as the owner pointer (`+0x7C`).
- `dmg_tbl_mecha_s` for `0x800743DC` (the name from the Junkroid reconstruction; the two enemies use the
  same table) and `dmg_tbl_guard` for `0x8007429C`.
- The `switch` in `fx_init` and `fx_main`: the binary has jump tables of 18 entries (`0x800E9864`,
  `0x800E98AC`), which a `switch` on `var` with cases 0–3, 16 and 17 gives.
