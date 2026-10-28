# Reconstructed source (plausible, not original)

My guess at how the original C for the intro's small jet could have looked (Psy-Q SDK, PS1). It is not
compiled and it is not a matching decompilation. The conventions follow
`../../junkroid/reconstructed/` so that the two read as parts of one code base.

## Layout

| file | contents |
|---|---|
| `include/object.h` | the same as the robot's copy |
| `include/game.h` | the robot's copy plus `OffScreenCheck`, `DisplayCheck1`, `AimAt`, `SetSpeedDir`, `BoxOverlap` |
| `include/sound.h` | the robot's copy plus `SE_JET_SHOT` |
| `src/st02/m05_jet.c` | the jet (main object 05), its shot (visual object 04) and its muzzle flash (misc object 06) |
| `src/st02/st02_tbl.c` | the excerpt of the stage 02 object tables that point to the three update functions |

The jet, shot and flash code is adjacent in overlay `02` (`0x800EC11C`–`0x800EDD60`), and their data
is adjacent at `0x800FAD54`–`0x800FAF08`. That is why they are in one file.

## What is certain (from the binary)

- Every constant: HP 5 and 1, contact damage 3 and 3, speeds `0x40000`, `0x8000`, `0x20000`, the
  timers 48, 60, 20, 8, 20, 90, 60, 40, 32, the shot counts 3 and 9, the burst count 2, the wake
  distance 176, the range table 64 / 80 / 96 / 112, the drop table 18, the debris count 5, the
  bob gravity `0x1000`, the screen margins.
- The control flow of every function, including the sub-step tables, the direction switches, and
  the order in which work bytes are saved and restored for `var` 3.
- The animation, box, offset and `turret_dir` data, byte for byte.
- The work-area offsets. `+0x7C` is the 16-bit `timer` on the jet, and a parent pointer on the shot.
  The misc object stores its parent in the field at `+0x50`, which is the attack box pointer on main
  objects.

## What is invented

- All names, including `M05_` / `V04_` / `X06_`, the `MODE_` and `ANM_` names, and the `*_W` macros
  for the work bytes. The binary only shows offsets.
- The split of the direction switches into `Jet_SetPose`, `Jet_Ofs` and `Fan_Side`. The binary has a
  separate compiled jump table for each use (BURST and FAN poses are two copies, shot and flash
  offsets are two copies). The original may have had copies of the same `switch` in each function.
- `tama_ofs` and `fx_ofs` are two identical tables in the binary (`0x800FAECC` and `0x800FAEEC`), so I
  kept two.
- The `goto end` in `Tama_Main`. The binary has several branches to one shared tail; a `goto` is one
  way to write that, a chain of `else if` is another.
- The comments and the Psy-Q includes.
- The pose names ("up", "down", "diagonal") are from the direction cases, not from the sprites.
