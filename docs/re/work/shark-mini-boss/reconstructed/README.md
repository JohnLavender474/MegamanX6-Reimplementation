# Reconstructed source (plausible, not original)

A guess at how the original C for Nightmare Pressure could have looked (Psy-Q SDK, PS1). Not
compiled, not a matching decompilation. The conventions are as in `../../shield-sheldon/reconstructed/`.

## Layout

| file | contents |
|---|---|
| `include/object.h` | the Sheldon copy, not changed |
| `include/game.h` | the Sheldon copy, plus `BGCAM` / `bg_cam[3]` (the camera records, with the 16.16 `y`), `GetMainObjectAfter` (`0x8002BCCC`), `GetStageObject`, `SpawnInRect`, `SolidObject`, `SolidWeaponHit` (`0x80032B34`), `SolidPushShots` (`0x8003208C`), `SpawnExplosionRand`, the `mega` and `game` byte macros, and `dmg_tbl_normal`, `dmg_tbl_minor`, `dmg_tbl_capsule` |
| `include/sound.h` | the sound ids of this fight (bank 2) and `SE_HP_FILL` (bank 0, id 14) |
| `src/st07/m16_nightmare_pressure.c` | main object 16 (the body `var` 0, the pods 16–18, the turrets 32–34) and visual object 13 (bombs, shots, missiles, beams) |
| `src/st07/s10_nightmare_pressure.c` | stage object 10 (side walls 0/1, press plates 16/17, head block 32) |

The code is in overlay `07` at `0x800E9E08`–`0x800ED4D0`, with the data at `0x800F852C`–`0x800F88E8`
(the animation table at `0x800F8638`) and the switch table at `0x800E9864`.

## What is certain (from the binary)

- Every constant: HP 48 and 16, contact 8, 127, 6, 4, 2; the machine offsets (160, 48, 208, −64,
  56, 16, 304, 80, 240, −80, 64, ±16, ±32, ±44, ±164); the camera values (872, 824, 800, 776,
  952, 936, 912, 5440, 5456, 2728, 224, 5604, 395) and the rectangle (5408, 5824, 224, 512); the
  speeds (8, 0.8 = `0xCCCC`, 1.5, 2, 4, 5 = 80 / 4096 × 65536, gravities `0x1800`, `0xB60`, bounce
  `0x15555`); the timers (300, 180, 60, 20, 30, 50, 90, 127, 25, 5, 50 + rnd % 40); the patterns;
  the boxes; the sounds.
- The control flow of every function and the `act4` messages between the body and its parts.

## What is invented or approximate

- All names.
- The binary has one dispatcher per step; here a `SUBSTEP` macro makes them. The three "head out"
  subs share `NP_HeadOut`, and the three "head in" subs share `NP_HeadIn`.
- `NP_Death1`: the blink-gap arithmetic (`gap -= 5`; set to 0 only when it is ≥ 26) is written as
  the binary does it, which looks like a bug or a signed-byte trick.
- `NP_MakeStagePart` writes the whole-pixel x and y into the 32-bit fields, as the binary does.
- The field macros (`OFS_X`, `HEAD`, …) index `work[]` from `+0x7E`; the binary uses raw offsets.
- The comments and the includes.
