# Reconstructed C: player steps 32–77 and the forms

A plausible reconstruction from the disassembly (main program, overlay 0, overlays 16 and 19). It
is not original code and it is not compiled. Each function carries its address.

## Files

- `include/object.h`, `game.h`, `sound.h`, `player.h`: copied from the air part (2b), not changed.
- `src/player/steps_extra.c`: `LoadPlayerMember` (`0x80013BF0`), `PlayerLoadForm` (`0x8003D718`),
  `ArmourSelectList` (overlay 16 `0x800E9898`), `WeaponCost` (`0x8003EB0C`), `SpecialAttackTry`
  (`0x8003E344`), `UltimateHoverTry` (`0x8003F200`) and step 33, `NovaStep` (step 34),
  `BladeMachTry` (`0x8003F32C`) and step 74, `WeaponPoseStart` (`0x8003F864`), step 73.

## Certain

- Control flow, constants, field offsets, animation, sound and object numbers.
- The form values (0 none … 4 Ultimate, 5 Zero) and the member per form.

## Guesses

- The names. "Nova" for step 34 and "special attack" for `0x8003E344` are names from the
  game's public documents, not from its code.
- `U8`/`S16` accesses stand in for fields that `player.h` does not name.

## Not here

- Steps 35–47, 75–77 beyond their entries (see `../notes.md` and `../pseudocode.md`).
- The Zero steps 48–63 (described in `../notes.md`).
