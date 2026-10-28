# Reconstructed source (plausible, not original)

My guess at how the original C for Mettaur D2 could have looked in 2001 (Psy-Q SDK, PS1). It is not
original code, not a matching decompilation, and not compiled for the PS1. A syntax check with
`gcc -fsyntax-only` and empty stand-ins for `libgte.h` and `libgpu.h` is the only test.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/game.h`, `include/sound.h` | copied from `sheldon-lasers`; `game.h` adds the weapon area (`0x800950A0`, 32 × `0xA0`) with `WEAPON(i)`, `WPN_LEMON` (0), `KILL_BY_WPN55` (−56), `TerrainPointCheck` (`0x80030CCC`), `SpawnHitEffect` (`0x80049D90`) and `dmg_tbl_one_hit` (`0x8007429C`); `sound.h` adds `SE_MET_SHOT` (bank 2, id 46). The laser block that the copy had after `#endif` is moved in front of it |
| `src/st10/m27_mettaur_d2.c` | the Mettaur (main object 27) and its shot (visual object 32) |

In the binary the code is at `0x800F1748`–`0x800F2600` in overlay 10. The functions are in the
binary's order: shot trigger, edge check, guard trigger, guard, init, main, entry, hidden wait, jump,
fall, shoot, walk, delete, then the shot (entry, init, move, delete).

## What is certain (from the binary)

- Every constant: HP 3, contact 1 and 2, the speeds `0x6000`, `0x38000`, `0x10000`, `0x28000`,
  gravity `0x4200`, the zones 40 × 80 px (shot) and 24 × 128 px (guard), the counts 61, 60 and 120,
  the probe offset 8, the grid snap (+8, +20), the shot offsets (16, 3), the off-screen ranges 64 and
  32 and the display ranges 37 and 16, the sound (2, 46), the debris animations 9–12, drop table 18
  and the −56 test.
- The control flow and the dispatch tables (states 0–4, steps 0–5 with step 0 = the engine hit
  reaction, the shot states 0–2).
- The struct offsets used (see `../pseudocode.md`).

## What is a guess

- All names of functions, macros, constants and work fields.
- The work bytes as `work[2]`…`work[5]` (`+0x80`…`+0x83`).
- The unsigned `(u_char)(var − 2) < 2` test: the binary does `addiu −2` then `sltiu 2` on the byte.
- The 16-bit compares in the zone tests are written with `POS_X`/`POS_Y`; the player-side tests of
  the guard trigger use the 32-bit `x.l`, as the binary does.
