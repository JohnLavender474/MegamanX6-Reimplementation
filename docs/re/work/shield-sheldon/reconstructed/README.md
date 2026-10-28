# Reconstructed source (plausible, not original)

My guess at how the original C for Shield Sheldon could have looked (Psy-Q SDK, PS1). Not
compiled, not a matching decompilation. Conventions as in `../../commander-yammark/reconstructed/`.

## Layout

| file | contents |
|---|---|
| `include/object.h` | as in the Yammark copy |
| `include/game.h` | the Yammark copy, plus `MoveObject`, `OffScreenCheck2`, `GetEffectObject`, `GetVisualObjectAfter` (overlay 10, `0x800F77A0`), `Rumble`, `Quake`, `SpawnHitEffect`, the GTE functions and the tables `dmg_tbl_one_hit`, `dmg_tbl_sheldon`, `dmg_tbl_sheldon_guard` |
| `include/sound.h` | the Sheldon sound ids (bank 2) and `SE_HP_FILL` (bank 0, id 14) |
| `src/st10/m51_sheldon.c` | main object 51: the boss (`var` 0), the plates (16–19), the image (32), the turrets (48–51) |
| `src/st10/v16_sheldon.c` | visual object 16 (shields and shots) and effect object 5 (warp flashes) |

The code is in overlay `10` at `0x800F44A0`–`0x800FA8FC`, with the data at `0x800FC734`–`0x800FCFF8`
(the animation table at `0x800FCBBC`), the switch tables at `0x800E9E08` and `0x800E9E30`, the HP
bonus at `0x800E9E28`, and the globals at `0x800FD000` (fight timer) and `0x800FD004` (plate
freeze).

## What is certain (from the binary)

- Every constant: HP 32 and the bonus {0, 4, 8, 16}; contact 10, 8, 4; the stun results 5, 14, 36;
  the invulnerability 30 / 2 / 80; the room points, routes and corners; the speeds (1, 2, 3, 4, 5,
  6–9, 12 with 0x4CCC, gravity 0x4200); the timers (30, 40, 10, 600, 20, 30, 50, 7, 127, 25, 5401);
  the pattern tables; the boxes; the sounds.
- The control flow of every function and the `act4` messages between the boss and his shots and
  children.

## What is invented or approximate

- All names.
- The binary has one function per sub-step. Here the identical sub 0 of steps 4–7 is one function
  (`Shd_Throw0`), and the three "warp in at room x 160" subs share `Shd_Warp160`. The boss's step
  routines are written with a `SUBSTEP` macro; in the binary each is its own dispatcher.
- `Shd_PlateMain`, `Shd_ImageMain`, `Shd_TurretMain` put the binary's step tables (`0x800FCDE4`,
  `0x800FCDFC`, `0x800FCE10`) in switch statements.
- `Shd_SprtSet` (`0x800F5F04`) and the copies in `0x800F6F44`, `0x800F71F4`, `0x800F74D8`,
  `0x800F765C`: the palette arithmetic is written in outline. The binary computes the palette from
  the sprite bank number with the round-toward-zero fix-ups for `/ 16` and `/ 4`.
- `GetVisualObjectAfter` is the overlay copy of the visual-slot search; `a1` = 0 searches forward.
- The plates, the image and the turrets have no damage table (the binary never writes `+0x58`);
  the C keeps that.
- The comments and the includes.
