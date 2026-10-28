# Reconstructed source (plausible, not original)

My guess at how the original C for the Nightmare Press of Metal Shark Player's stage could have looked
(Psy-Q SDK, PS1). It is not compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h` | copied from the boss gate, unchanged |
| `include/sound.h` | copied from the boss gate; adds `SE_PRESS_RISE` (bank 2, 34) and `SE_PRESS_SLAM` (bank 2, 35) |
| `include/game.h` | copied from the boss gate; `GAME` gets `checkpoint` (`+0x1D`) and `flag27` (`+0x27`); `BGLAYER` gets `mode` (`+0x04`) and `old_y` (`+0x1A`); new: the player field offsets, `SYSOBJ` (the kind-7 object, `0x30` bytes), `stage_object[]`, prototypes of the solid helpers |
| `src/main/k02_press.c` | the press: kind-7 object type 2, main program (`0x80046E14`–`0x800477E0`) |
| `src/st06/s09_press_block.c` | stage object 09 of overlay 06: the solid body that follows layer 1 (`0x800E9878`–`0x800E9F00`) |
| `src/main/player_squeeze.c` | the flag set-up in `PlayerUpdate`, the crush and squeeze part of `PlayerHitCheck`, and the Ride Armor flag (`0x800F2414`, overlay 06) |

`k` = kind-7 class, `s` = stage-object class, as in the earlier file names.

## What is certain (from the binary)

- Every constant: the speeds (2.0, 1.0, 1.25), the counts (8, 16), the travel `0x500000` (80 px),
  the timers (180, 120, 50, 40), the kill-line constants (240, 256, 736, 2001, 2065, 2097), the x and y
  limits, the damage 127, the squeeze timer 75, the sounds (bank 2, 34 and 35), `Quake(60, 2, 2)`,
  `Rumble(5)`, the type 9 box tables and the box rule for `var` ≥ 10.
- The control flow of every function and the order of the tests.

## What is a guess

- All names of functions, fields, structs and globals (`SYSOBJ`, `PressUpdate`, `flag27`, …).
- The C types of the fields (signed or unsigned), where the binary does not show it.
- `game.max_damage` stands for the 32-bit damage total at `game+0x98`; the earlier header has a short
  at a different offset under that name.
- The bodies of `SolidPre`, `SolidSideFix1`, `SolidSideFix2` and `SolidOthers` (`0x80032B34`,
  `0x80032BE8`, `0x8003208C`, `0x80031F4C`) were not read.
