# Reconstructed C: Zero's attacks and techniques

A plausible reconstruction from the disassembly of overlay 1 (Zero) in `turtloid-6_zero-player`.
It is not original code and it is not compiled. Each function carries its address.

## Files

- `include/object.h`, `game.h`, `sound.h`, `player.h`: copied without change from
  [`~/mmx-data/x6/re/x/attacks/reconstructed/include/`](../../../x/attacks/reconstructed/include/).
- `include/zero.h`: Zero's use of the `PLAYER` fields (`+0x8E` busy, `+0xCB` combo, `+0xC9`
  skills, `+0x93` energy slot, `+0xA6`, `+0xE1`), the skill bits, energy slots, steps 48–63,
  weapon ids, frame flags and part bits.
- `src/player/zero.c`: the sabre helpers, the ground combo (step 48 and its combo try), the
  landing of the air slash, Shoenzan, Sentsuizan, Ensuizan, Rekkoha (start and beams), the
  Z-Buster start, the double jump, and the sabre hit object (`0x801ED2A8`, `0x801ED6F0`).
  The other steps and objects are in `../pseudocode.md`.

## Certain

Control flow, the order of the tests, constants, table addresses, field offsets, animation,
sound and weapon numbers.

## Guesses

All names. The OBJECT field names `hit_tag` (`+0x64`), `shot_hit` (`+0x98`), `work8c`, `work90`
are placeholders for the weapon-object fields. The technique names come from fan wikis
(`docs/players/zero.md`, "Sources").
