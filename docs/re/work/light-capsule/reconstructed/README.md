# Reconstructed source (plausible, not original)

My guess at how the original C for the Dr. Light capsule (stage object 26) could have looked
(Psy-Q SDK, PS1). It is not compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h` | copied from Meta Wheel F |
| `include/game.h` | the same extended `game.h` as the rescue Reploid (`armors`, `armor_parts`, `pause[]`, the stream and text calls) |
| `include/sound.h` | adds `SE_CAPSULE_OPEN` (20) and `SE_CAPSULE_IN` (24), the arguments of `0x8003D308` |
| `src/st03/m26_light_capsule.c` | the capsule and its lid |

In the binary the code is in the main program at `0x80051158`–`0x80051FE8` (no code between
`0x80051490` and `0x800515B4` belongs to it [not checked]). `st03` only says where it was studied.

## What is certain (from the binary)

- The constants: bank 128, the boxes, the lid offset 78, the approach box (64, 104, 16), stream 28 at
  volume 127, the sound arguments 20 and 24, the effect types 35/36/37 and their `var`, the timer 40,
  the margin 136, the part-bit, picture and message tables.
- The control flow of all 12 steps and the player handshake through `mega+0xE2`.

## What is a guess

- All names, in particular `SolidPlayer`, `SolidVisual`, `SolidMain`, `SolidWeaponHit`, `SolidLink`
  for `0x80031DA8`, `0x8003208C`, `0x80031F4C`, `0x80032B34`, `0x80032BE8`; `SoundPan` for
  `0x8003D308`; `MusicRestore` for `0x8003D330`.
- `SetSpriteBank` stands for the inline tex/clut set-up (the same code as in the Reploid).
- The use of `timer` (`+0x7C`) as the lid's parent pointer and as the 40-update counter.

Correction (process log section 64): `0x8003D308` is `PlayerEvent(step, facing)` and `0x8003D330` is `PlayerEventEnd()`, not music calls. The C keeps the old names `PlayMusic` / `MusicRestore` / `StartBossMusic`.
