# Reconstructed source (plausible, not original)

My guess at how the original C for the boss gate could have looked in 2001 (Psy-Q SDK, PS1). It is
not compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h`, `include/sound.h` | copied from Meta Wheel F; `sound.h` adds `SE_GATE_A` / `SE_GATE_B` (bank 0, ids 16, 17) and bank 5 ids 1, 3 |
| `include/game.h` | copied from Meta Wheel F; `GAME` gets the gate flags (`+0x10`, `+0x12`, `+0x1C`, `+0x26`, `+0x28`), `time` (`+0x90`) and `boss_time` (`+0x43C`); `BGLAYER` gets the camera x and the current and target limits (`+0x1C`..`+0x2A`), `event_f` (`+0x34`), `lim_spd` (`+0x47`) and two layer-1 fields; new engine prototypes `SolidObjectCollide`, `ClearEnemies`, `PlayerSetTerrainBox`, `Quake2` |
| `src/main/s08_gate.c` | the gate: stage object 08, in the main program (so `main`, not `stNN`; `s` = stage-object class) |

The earlier `BGLAYER` names `x` (`+0x1E`) and `dest_y` (`+0x20`) are kept. By this reading they are
the current min x and max y limits.

In the binary the gate code is at `0x8005011C`–`0x80051158`, the jump table of `gate_open` at
`0x80011694`, the tables at `0x80078DC8`–`0x80078DF7`.

## What is certain (from the binary)

- Every constant: flags `0x49`, priority 6, bank `0x80`, the terrain box, the push speeds, the
  camera limit values of every case, the limit speed 2, the quake values (10, 2, 2) and timer 10,
  the counts 112 and 31, the sounds.
- The control flow of every function, the two dispatch tables, the 51-entry jump table, and the
  writes to the player (`+0xD4`, `+0x68`, `+0x08`, `+0x71`).

## What is a guess

- All names of functions, fields and globals.
- `SetSpriteBank`: the binary has the same bank code inline (the stage spawner has a copy too).
- The `switch` of `gate_open` as one `switch`: the binary has the jump table and shared tails (the
  time stamp tail at `0x800509AC` / `0x80050AC4`); the order of the stores in a case may differ.
- `gate_wait`'s open conditions as one `switch`: the binary has a chain of `if`s.
