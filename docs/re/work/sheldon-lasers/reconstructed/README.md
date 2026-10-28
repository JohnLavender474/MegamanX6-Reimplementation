# Reconstructed C: stage 07 laser puzzle (main type 13, visual type 11, misc type 7)

A plausible reconstruction from the disassembly of overlay 07 (save state `sheldon-7_lasers`). It is not
original code and it was not compiled.

- `src/st07/m13_laser_puzzle.c`: one function per game function, in the same order as the kit, with the
  same constants. The three misc type 7 spawns are written once (`Laser_Draw`); the game has them inline
  three times in state 1 and twice in state 2.
- `include/`: copied from `teleport-crystal`; `game.h` has the new declarations at the end.
  `bg_layer[0].min_x/max_x/min_y/max_y/x/y` stand for layer-0 camera `+0x26/+0x24/+0x2A/+0x28/+0x0A/+0x0E`
  (not fields of the copied struct).

Certain: control flow, constants, tables, field offsets. Guess: the names, the struct field mapping of the
work bytes, and the type of `LaserBodyDraw` (`0x800EBCD8`, not reconstructed: it draws the body in pieces
of up to 224 px).
