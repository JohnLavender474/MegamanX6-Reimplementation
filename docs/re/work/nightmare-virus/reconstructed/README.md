# Reconstructed source (plausible, not original)

My guess at how the original C for the Nightmare Virus could have looked (Psy-Q SDK, PS1). It is not
compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/game.h`, `include/sound.h` | copied from Nightmare Insect; `game.h` adds `soul[2]` (`+0xD2`), `reploid[2]` (`+0xD8`), `soul_done[][32]` (`+0x178`), the Reploid status values, `stage_object`, `GetRayObject`, `TerrainBoxOverlap`; `sound.h` adds the virus and soul sounds |
| `src/main/m15_nightmare_virus.c` | the virus (main object 15), its orb (visual 12), its after-images and grab overlay (misc 11), and an outline of the rays of the boss soul (object 13 of the class at `0x8009C9B0`) |

The code is in the main program (`0x800404D8`–`0x800434BC`), so it is shared by every stage.
There is no overlay number; the file is in `src/main/`.

## What is certain (from the binary)

- Every constant: HP 10 / 2 (Easy) / 18 (Hard and up), contact damage 4, the boxes, the speeds
  (`<<4`, `*96`, `*80`, `>>1`), the timers (60, 40, 30/60/90, 180, 90, 8), the shot counts 3/4/5, the
  soul values 8, 4, 100, 200, the pattern table, the sound ids.
- The control flow of every function listed in `../pseudocode.md`, and all dispatch tables.
- The struct offsets used.

## What is a guess

- All names.
- `alt_body` (`0x800CD340`, size `0xB0`): the code uses it in place of the player when
  `mega+0xD5` ≠ 0. What it is, is not known.
- `game.message` (`+0x0F`): the name comes from earlier headers; the boss soul writes 16 there.
- The camera layout: this code reads the camera x at `bg_layer[0] + 0x0A`, not at `+0x1E` as the
  shared header has it. The reconstruction writes `bg_layer[0].x` for both.
- The rays are only outlined.
