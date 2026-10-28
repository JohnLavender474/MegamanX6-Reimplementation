# Reconstructed source (plausible, not original)

My guess at how the original C for the Guardian could have looked in 2001 (Psy-Q SDK, PS1). It is
not original code, not a matching decompilation, and not compiled for the PS1. A syntax check with
`gcc -fsyntax-only` and empty stand-ins for `libgte.h` and `libgpu.h` is the only test.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/game.h`, `include/sound.h` | copied from `sheldon-lasers`; `game.h` adds `KILL_BY_WPN55` (−56), `bank_tpage` (the pointer at `0x80095098`), `SpawnHitEffect` (`0x80049D90`) and `dmg_tbl_one_hit` (`0x8007429C`); `sound.h` adds `SE_GUARDIAN_SHOT` (bank 2, id 79). The laser block that the copy had after `#endif` is moved in front of it |
| `src/st10/m30_guardian.c` | the Guardian (main object 30) and its shot (visual object 35) |

In the binary the code is at `0x800EFE48`–`0x800F0BB0` in overlay 10. The functions are in the
binary's order: init, main, fire, entry, death, delete, the step routines, `player_behind`, then the
shot (entry, init, move, delete). Stage 22 has the same code at `0x800EEC0C`.

## What is certain (from the binary)

- Every constant: HP 6 and 1, contact 6, 7 and 3, the timers 120, 20, 30, 20, 10, 32 and 6, the
  look mask 31, the near limit 188, the muzzle offset 6, the shot count 4, the speed factor 3, the
  direction limits (9…23, 8 and 24), the off-screen ranges 96 and 32 and the display ranges 64 and
  16, the sound (2, 79), the debris animations 8–11, drop table 20 and the −56 test.
- The control flow, the dispatch tables (states 0–4, steps 0–7 with step 0 = the engine hit
  reaction, every sub-step table, the shot states 0–2), and the box addresses.
- The four box pointers are overlay globals (`0x800FCFF0`…`0x800FCFFC`), not object fields.

## What is a guess

- All names of functions, macros, constants and work fields.
- The work fields: `timer2` = `+0x7E`, `aim_dir` = `+0x81`, `open_f` = `+0x8B`, `resume` = `+0x94`,
  `work[27]` = `+0x99`.
- The large-Guardian set-up: the binary calls `GetSpriteBank` four times; the C keeps the four calls
  but the order of the stores differs a little. `sprt_bank->ofs[bank + 1]` stands for the word at
  `sprite_bank_base + bank × 4 + 4`.
- The empty routines of steps 2, 6 and 7 are written as separate functions, as the binary has
  separate addresses.
