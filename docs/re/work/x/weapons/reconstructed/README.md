# Reconstructed C: X's special weapons (player RE step 4)

A plausible reconstruction from the disassembly of the main program (`SLUS_013.95`) and of
overlay 0 (X). It is not original code and it is not compiled. Each function carries its address.

## Files

- `include/object.h`, `game.h`, `sound.h`, `player.h`, `shot.h`: copied from step 3
  (`../attacks/reconstructed/include/`). `player.h` already has `wpn_state` (`+0xA6`), `energy[10]`
  (`+0xA8`) and `var_144` (`+0x144`).
- `include/weapons.h`: the weapon ids by slot, the `+0xA6` and `+0x144` values, the work-byte
  macros of the weapon objects, the cost and key tables.
- `src/player/weapons.c`: `PadToButtons`, `PlayerInput`, `WeaponCost`, `HasEnergy`,
  `SpendEnergy`, the gauge fill on a hurt, the timed item effect, and samples of the weapon
  objects: Ice Burst (block, flight), Metal Anchor (bounce), Ground Dash (throw, the freeze of
  step 75), Meteor Rain (the 10 meteors), Guard Shell (block and shard), Ray Arrow (turn at a
  wall). The other weapon objects are in `../pseudocode.md`.

## Certain

- Control flow, the order of the tests, constants, table addresses, field offsets, animation and
  sound numbers.

## Guesses

- All names, also the weapon names (from the stage-number rule and the official X6 weapon list).
- Some struct field names (`key`, `key_old`, `key_new`, `gate`, `special_timer`, `game.stop[]`,
  `player_lock_1c`) are names for the offsets in the comments of `../pseudocode.md`; they are not
  all in `player.h` and `game.h`.
