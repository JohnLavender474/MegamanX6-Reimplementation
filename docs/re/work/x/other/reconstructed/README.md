# Reconstructed C: player ladder, hit reaction, death, beams, ride, events (part 2c)

A plausible reconstruction from the disassembly of the main program (`SLUS_013.95`) and of the two
player overlays. It is not original code and it is not compiled. Each function carries its address;
the order of the tests and the constants follow the game.

## Files

- `include/object.h`, `sound.h`: copied from part 2a; `sound.h` has the player sounds added.
- `include/game.h`: copied from part 2a, with byte accessors (`GAME_B`, `GAME_W`) for the game
  fields used here, and without the `extern OBJECT mega` (it clashes with `player.h`).
- `include/player.h`: the `mega` object as parts 2a and 2c read it. Part 2c adds `iframes`
  (`+0x61`), `hit_class` (`+0x63`), `spike` (`+0x79`), `hit_lock` (`+0xA4`), `hit_flip` (`+0xA5`),
  `energy[9]` (`+0xA8`), `grab` (`+0xCA`), `beam_thru` (`+0xCE`), the event bytes (`+0xD0`–`+0xD2`),
  `ride` (`+0xD5`, called `dash_box` in part 2a), the death bytes (`+0xD6`, `+0xD7`), `ride_anime`
  (`+0xD8`), `capsule` (`+0xE2`), the squeeze bytes (`+0xE5`, `+0xE6`), and the step and state
  numbers.
- `src/player/other.c`: the ladder (entries, step 14 sub-steps, steps 15 and 16), the hit check,
  `PlayerHurt`, step 17 and its handlers, the death (state 2), the set-up (state 0) and the beam-in
  (step 0), the beam-out (step 1), `PlayerExit`, step 25, the ride (step 18), the event set-up and
  steps 20–24, 29, 30, Zero's step 5 and idle.

## Certain

- Control flow, the order of the checks, the constants, field offsets, table addresses, animation
  and sound numbers.

## Guesses

- All names of fields and functions.
- `HurtMove` writes the sign test as one expression; the game tests the facing first, then the sign.
- `PlayerInit`'s stage test reads `game.stage` and `game.area` as bytes (`+0x0C`, `+0x0D`).
- `DeathParticle` is used for both the particles and the orbs: the game has one function
  (`0x8003DAF4`) for the particles and an inline copy in `0x8003DB44` for the orbs.

## Not here

- The ladder shot code of X (`0x8003E260`, step 32 in overlay 0) and Zero's ladder skills (overlay 1).
- The jump and the fall (`StartJump`, `0x8003AFF4`): part 2b.
- `ContactMega` (`0x80030ECC`): in `../pseudocode.md`.
