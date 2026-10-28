# Metal Shark Player: reconstructed C

A plausible reconstruction of the overlay 07 code for main object type 56 (Metal Shark Player and
the three side kicks) and visual object type 22 (anchor, debris, forming pieces, beam, sparks,
bees). It is not original code and it was not compiled. It follows the disassembly in
`../kit/code.txt` function by function, with the same constants; each function has its address.

- Certain: the control flow, the constants, the table addresses, the timings and the speeds.
- A guess: all names, the field layout of `GAME` and `BGLAYER` beyond the fields used, the clut
  formula in `SharkSprtSet0/1` (simplified), and where small helpers were merged
  (`SharkFlyTo` stands for the three fly routines `0x800F25F0`, `0x800F2738`, `0x800F2C38`).

Files:

- `include/object.h`, `include/game.h` (from Shield Sheldon, with the helpers this boss adds),
  `include/sound.h` (this stage's sound ids);
- `src/st07/m56_metal_shark.c`: the boss, his steps, the side kicks;
- `src/st07/v22_metal_shark.c`: visual object 22.
