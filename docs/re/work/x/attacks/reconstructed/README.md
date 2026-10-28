# Reconstructed C: X's attacks (player RE step 3)

A plausible reconstruction from the disassembly of the main program (`SLUS_013.95`) and of overlay
0 (X). It is not original code and it is not compiled. Each function carries its address.

## Files

- `include/object.h`, `game.h`, `sound.h`: copied from part 2b (`../air/reconstructed/`).
- `include/player.h`: the part 2b `PLAYER` struct, extended with the shot and charge fields
  `+0x7E` (`key_old`) and `+0x8F` … `+0xC9` (`wpn_slot`, `wpn_id`, `bst_id`, `weapon`,
  `last_shot`, `shot_count`, `wpn_count`, `shot_cycle`, `level[2]`, `charge[2]`, the blink and
  palette timers, `giga`, `wpn_state`, `energy[10]`), `+0xDD`, `+0xE0`, `+0x138`, `+0x144`, the
  button bits `0x10`, `0x20`, `0x40`, `0x200`, `0x400`, the release bits and the part bits.
  `ladder_timer` (`+0x92`) of part 2b is renamed `shot_wait` (catalog name `mega.shot_wait`).
- `include/shot.h`: field macros for the weapon objects and the muzzle flash, the weapon ids,
  sound ids and charge thresholds.
- `src/player/attacks.c`: weapon select and the shot ids, the charge, the shot try, the spawn
  functions, the lemon (type 0), the half charge and plain full charge (types 9, 18), the Falcon
  full charge (type 19), the muzzle flash (effect type 2).

## Certain

- Control flow, the order of the tests, constants, table addresses, field offsets, animation and
  sound numbers.

## Guesses

- All names. The part names (`PART_*`) match the effects in the code and the part bits that a
  public mod tool gives (facts only, `mmx-reference`).
- `game_c9` bits 0 and 1 as options "auto charge" and "auto fire": the code effect is certain,
  the option names are guesses.
- `WeaponSelect` loops are written as `for` loops; the game uses a byte table walk with the same
  result.
- `ShotSpark` (`0x800383F8`) is called, not written out (sound 1/9 for type 58, none for 56–57,
  else 1/12; effect type 3 var 4 for 18, 20, 46, else var 3).

## Not here

- Type 20 beyond its speed (`0x801ED1B8`, parts of type 21 in `0x801ED36C`), types 43, 45, 46,
  50, 51 and the special weapons 1–8, 10–17.
- The pose weapons (`WeaponPoseStep` `0x8003F864`, `TryGigaAttack` `0x8003E344`).
- `ZeroWeaponUpdate` (`0x8003DBDC`).
