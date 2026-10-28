# Reconstructed source (plausible, not original)

My guess at how the original C for the Nightmare shadow could have looked in 2001 (Psy-Q SDK, PS1).
It is not compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/game.h`, `include/sound.h` | copied from Monbando; `game.h` adds `nightmare_no` (`0x800CD30A`), `NIGHTMARE_SHADOW` (7), `frame_count` (`0x80097420`), `bank_tpage` (the pointer at `0x80095098`) and `dmg_tbl_nightmare_shadow` (`0x8007497C`) |
| `src/st09/m26_nightmare-shadow.c` | the spawner (`var` 0–5) and the shadows (`var` 10, 11), main object 26 |

In the binary the code is at `0x800EE9E4`–`0x800EF284` in overlay `09`. The functions in the C file
are in the same order as in the binary: `shadow_init`, `shadow_spawn`, `appear_wait`,
`m26_nightmare_shadow` (entry), `shadow_main`, `shadow_split`, `shadow_delete`, `shadow_dash`,
`shadow_appear`, `appear_blink`, `appear_stand`. `in_zone` and `spawn_time` are inlined twice in the
binary (the zone tests are copied in `shadow_init` and `shadow_spawn`).

## What is certain (from the binary)

- Every constant: the flag value 7, the zone limits, the timer `(rng % 3 + 1) × 100 + 200`, HP 4,
  contact damage 6, priority 4, blink 40, stand 50, death 60, offset 32, off-screen range 128, the
  shift 6 on `rcos`/`rsin`, `var` 10/11, the four boxes and the damage table address.
- The control flow of every function and the five dispatch tables.

## What is a guess

- All names (functions, constants, the global `nightmare_no`, `frame_count`).
- `split_r` / `split_l` as macros over `work[]` (`+0x80`, `+0x82`).
- The C form of the sprite-bank set-up (the same arithmetic as the binary).
