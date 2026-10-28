# Reconstructed C: player ground movement (part 2a)

A plausible reconstruction from the disassembly of the main program (`SLUS_013.95`). It is not
original code and it is not compiled. Each function carries its address; the order and the constants
follow the game.

## Files

- `include/object.h`, `game.h`, `sound.h`: copied from an earlier enemy.
- `include/player.h`: the `mega` object as far as part 2a reads it (`PLAYER`), button, wall and part
  bits, step numbers.
- `src/player/ground.c`: the physics set-up, the animation helpers, the tests, the step entries, and
  steps 2, 3, 4, 8 and 12 (with its three sub-steps).

## Certain

- Control flow, the order of the checks, constants, field offsets, animation and sound numbers.

## Guesses

- Names of fields and functions (`dash_a`, `dash_b`, `jump_work`, `ZeroTryA`…). The Zero tries are
  the overlay-1 functions at `0x801EC86C` (A), `0x801EA560` (B), `0x801EBE04` (C), `0x801EBE54` (D),
  `0x801EA254` (E), `0x801EBEDC` (F).
- `StartCrouch` reads `+0x8C` as a 32-bit word and tests its high half; the struct has a byte there.

## Not here

- The crouch step (26) body: see `../pseudocode.md`.
- The jump (`0x8003AF1C`) and the air steps: part 2b.
