# Reconstructed C: player air movement (part 2b)

A plausible reconstruction from the disassembly of the main program (`SLUS_013.95`) and a few
functions of overlay 0 (X). It is not original code and it is not compiled. Each function carries
its address; the order of the tests and the constants follow the game.

## Files

- `include/object.h`, `game.h`, `sound.h`: copied from part 2a.
- `include/player.h`: the part 2a `PLAYER` struct, extended with the air fields (`wall_ok` `+0x4A`,
  `spike` `+0x79`, `wall_timer` `+0x8A`, `voice_timer` `+0x8D`, `hang_snd` `+0xDE`, `splash`
  `+0xDF`, `hang_wait` `+0xE3`), names for the constants `+0x100`, `+0x110` … `+0x130`, the air
  step numbers, the forms (`+0xE7`) and the hang tile types.
- `src/player/air.c`: steps 6, 7, 9, 10, 11, 13, 27, 28, 31 with their sub-steps, the entries
  (`StartJump`, `ToFall`, `StartWallGrab`, `StartWallKick`, `StartWallSlide`, `WallRelease`,
  `StartAirDash`, `AirDashStop`, `AirDashShot`, `LandOrFall`), the tries (`TryWall`,
  `TryWallKick`, `WallHeld`, `TryAirDash`, `TryHang`, `TryLadderAir`, `HangExit`), the armour air
  tries (`FalconTry`, `BladeTry`, `UltimateTry`), `AirControl`, `CheckWalkOff`, `FalconDashStep`,
  the rope and bar movers.

## Certain

- Control flow, the order of the checks, constants, field offsets, animation and sound numbers.

## Guesses

- Names of fields and functions. The form names (1 Falcon, 2 Shadow, 3 Blade, 4 Ultimate) are
  likely, not read from the game's own text.
- `RopeClimb` and `BarMove` leave out the animation choice (151, 154, 145 + n, 160 + n, 163 + n);
  see `../pseudocode.md`.
- `RopeDashStep` (`0x80038788`) repeats the climb code inline in the game; here it calls
  `RopeClimb` after `ClearDashFlags`.
- `CheckWalkOff` reads `+0xA4` through a pad array.

## Not here

- The overlay steps 33 (Ultimate), 35 (Falcon), 74 (Blade) beyond their entry; `BarDashStep`
  (`0x80038594`); `XCeilingShot` (`0x8003F700`); the Zero overlay tries.
- The ladder (`LadderFromAir` `0x8003B7B4`): part 2c.
