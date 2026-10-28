# Reconstructed source (plausible, not original)

My guess at how the original C for Commander Yammark could have looked (Psy-Q SDK, PS1). Not
compiled, not a matching decompilation. Conventions as in `../../d-1000/reconstructed/`.

## Layout

| file | contents |
|---|---|
| `include/object.h` | as in the D-1000 copy |
| `include/game.h` | the D-1000 copy, plus the `GAME` fields `stop` `+0x10`, `player_lock` `+0x1C`, `checkpoint` `+0x1D`, `clear_f` `+0x2C`, `talk_f` `+0x2E`, `boss_count` `+0x8D`, `boss_level` `+0x8F`; the `BGLAYER` target limits and `limit_spd` `+0x47`; `PlayerEvent` / `PlayerEventEnd` (correct names; the D-1000 copy still has the wrong music names) |
| `include/sound.h` | the Yammark sound ids (bank 2) and `SE_HP_FILL` (bank 0, id 14) |
| `src/st03/m49_yammark.c` | the boss (main object 49, `var` 0), the options / drones (`var` 1) and visual object 15 (`var` 0–6) |
| `src/st03/st03_tbl.c` | the table excerpt |

The code is in overlay `03` at `0x800EF810`–`0x800F5EC4`, with the data at `0x800F7950`–`0x800F7E40`
(the animation table at `0x800F7B44`) and the switch tables at `0x800E9910`–`0x800E99D4`.

## What is certain (from the binary)

- Every constant: HP 32 and the level bonus {0, 6, 12, 16}, the contact damages 4 / 2 / 2 / 2 / 4,
  the room points (+16, +336, +192, +240/−32, +48, +304, +96, +150, +32/+176/+320, +48/+144/+96), the
  drone pairs (3,1) (4,2) (6,3) and the Hard rule, the pattern table, the timers (30, 2, 90, 500,
  600, 120, 260, 50, 3 walls, 500, 38, 60, 75, 60, 30 per drone in the trail, 210 trail entries,
  127, 25, 5, 5401), the speeds (2, 2.81, 2.5, 3.75, 3, 6, 4, 1, 0.0625), the ring radius and spin
  546, the angle table, the shot rates, the wall and column offsets, the boxes, the stun ids.
- The control flow of the boss, the drone and the six visual variants, and the order of calls.

## What is invented or approximate

- All names. The drone is called an "option" in the C (`OPT_`), after the X-series name for such
  helpers; that is a guess.
- The binary has five copies of the "go to the side point" and "arrive and summon" code, one per
  attack (`0x800EFF18`/`0x800F008C`, `0x800F0360`/`0x800F04D4`, `0x800F09C8`/`0x800F0B0C`,
  `0x800F0F1C`/`0x800F1060`, `0x800F1334`/`0x800F1478`), and five copies of the phase, gather and
  ready subs. Here they call shared helpers (`Ymk_FlyTo`, `Ymk_SideX`, `Ymk_Arrive`,
  `Ymk_AttackPhase`, `Ymk_AttackGather`, `Ymk_AttackReady`). The wrapper functions keep the binary's
  function list.
- `Opt_Main` (`0x800F1B54`) is one function with switch statements, as in the binary. The helpers
  `Opt_Ring`, `Opt_Spin`, `Opt_TurnTo`, `Opt_Arm`, `Opt_Step`, `Opt_Fx` are inline in the binary.
  `Opt_TurnTo` follows the binary's compare in outline; the exact signed compares are approximate.
- The flicker / display tail of `Opt_Main` (the `goto` labels) follows the binary's jumps to
  `0x800F3A60`, `0x800F3A78`, `0x800F3A90` and `0x800F3AAC` in outline.
- The ring arithmetic is written as `/ 128` and `/ 4096`; the binary uses `sra 7` / `sra 12` with
  the round-toward-zero fix-up, which is the same for these values.
- `hp_bonus_unused`: the set-up copies 4 bytes `{0, 1, 1, 2}` from `0x800E9908` to the stack and never
  reads them.
- `text_busy` is the byte `0x8008EAFC`; the catalog calls it `no_rain_drain`, but here it is read as
  "the text box is open".
- `stage_sprt_bank` stands for the stage-12 graphics path through `stage_object_table+0x8`.
- The comments and the includes.
