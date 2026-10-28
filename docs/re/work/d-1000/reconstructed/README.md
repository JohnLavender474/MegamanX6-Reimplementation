# Reconstructed source (plausible, not original)

My guess at how the original C for D-1000 could have looked (Psy-Q SDK, PS1). Not compiled, not a
matching decompilation. Conventions as in `../../junkroid/reconstructed/`.

## Layout

| file | contents |
|---|---|
| `include/object.h` | as in the Junkroid copy |
| `include/game.h` | the Meta Dridler copy plus the `GAME` boss fields (`message` `+0x0F`, `boss` `+0x20`, `boss_bar` `+0x24`, `boss_f` `+0x25`, `boss_hp` `+0x8C`), `BLOCKCOPY`, `SPRTBANK`, and the engine calls used here |
| `include/sound.h` | plus the D-1000 sound ids (bank 2) and `SE_HP_FILL` (bank 0, id 14) |
| `src/st02/m48_d1000.c` | the body (main object 48, var 0), the core (var 1) and visual object 01 (var 0–5) |
| `src/st02/st02_tbl.c` | the table excerpt |

The code is in overlay `02` at `0x800EDD60`–`0x800F1720`, with the data at `0x800FAF08`–`0x800FB2E8`
(animations from `0x800FAF18`, animation table `0x800FB138`).

## What is certain (from the binary)

- Every constant in the fight: start position (6288, 160), standing y 304, crouch y 352, the core
  positions 6368 / 6160 and the side test at 6264, the wall columns 6064 / 6448 and the templates,
  HP 32, contact damage 6 / 6 / 4, the speeds and gravities, every timer (60, 120, 90, 60, 180,
  180, 65, 120, 24, 84, 160, 220, 100, 60, 6, 8, 124, 60, 220, 50, 30, 2), the THINK choice table,
  the stun damage 8, the weak-point weapon ids.
- The control flow of the body, the core and the six visual variants, and the order of calls.
- The boxes, the wall template blocks, the damage table addresses.

## What is invented or approximate

- All names. The `ANM_` names come from what each animation is used for in the code, not from the
  sprites.
- `SetAnimeBank`: the binary repeats the same four lines (sprite bank lookup through `*0x1F800020`,
  then `SetAnime`) at every call. A macro or inline helper is my guess.
- `D1000_Move`, `D1000_TurnMega`, `D1000_SetFx`, `D1000_Walls`, `D1000_Kill`, `Fx_Copy`, `Fx_Gone`,
  `Core_WeakHit`: the binary has this code inline.
- `Spark_Main`: the math (distance check, `ratan2` / `rcos` / `rsin` use) follows the binary in
  outline; the exact shifts and the compare are approximate.
- `Dead_Sink`: the camera limit code is written as `CameraClampLeft/Right`; in the binary it is
  inline compares on the layer-0 camera fields `+0x24` / `+0x26`.
- `Dead_Debris`: the palette expression is approximate.
- The engine function names `GetSysObject`, `RockFall`, `Rumble`, `PlayMusic`, `StartBossMusic`,
  `ShowBossBar`, `BossBarBusy`, `BossInit`, and the `GAME` field names.
- Engine objects 24 and 26 are not in this overlay; their meaning ("warning", "explosion sequence") is
  a guess.
- The comments and the includes.

Correction (process log section 64): `0x8003D308` is `PlayerEvent(step, facing)` and `0x8003D330` is `PlayerEventEnd()`, not music calls. The C keeps the old names `PlayMusic` / `MusicRestore` / `StartBossMusic`.
