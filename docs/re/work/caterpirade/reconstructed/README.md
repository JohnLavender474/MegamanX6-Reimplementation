# Reconstructed source (plausible, not original)

My guess at how the original C for the Caterpirade and its spawner could have looked (Psy-Q SDK,
PS1). It is not compiled and it is not a matching decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h` | from Meta Wheel F; names `+0x66` (`push_f`), `+0x71` (`touch_all`), `+0x72..+0x74` (`touch[]`), `+0x75` (`side_kind`), `+0x76..+0x78` (`ride[]`) |
| `include/game.h` | adds `GetStageObject`, `SolidObjectMega`, `TerrainPointCheck`, `dmg_tbl_normal`, `dmg_tbl_deflect`, and two macros for the player bytes `mega+0xE5` and `mega+0x72` |
| `include/sound.h` | adds `SE_CATER_HIT` (bank 2, id 8) and `SE_CATER_OPEN` (bank 2, id 9) |
| `src/st03/m07_caterpirade.c` | the spawner (main object 7) and the Caterpirade (stage object 1) |

The functions are in binary order: `cater_main` `0x800EBF6C`, `cater_walk` `0x800EC144`,
`cater_spawner` `0x800EC25C`, `cater_update` `0x800EC300`, `cater_init` `0x800EC368`, the two deletes,
then the CURL, FALL, TURN and BALL routines up to `0x800EC8F0`. `cater_platform` is inlined in
`cater_main` in the binary.

Stage objects are `0xA4` bytes; the `OBJECT` struct here covers only the first `0x9C` bytes, which
have the same layout as a main object.

## What is certain (from the binary)

- Every constant: HP 12, contact 4, priority 7, gravity `0x2000`, speeds `0x8000` and `0x10000`,
  brake `-0x200`, timers 40 / 40 / 128 / 90, probe 8 px, off-screen 64 and visible 32, drop table
  16, the sounds, the six boxes, the two damage tables.
- The control flow of every function, the four dispatch tables, the double `DisplayCheck`, and the
  platform code around `SolidObjectMega`.

## What is a guess

- All names.
- `cater_platform` as a separate function.
- The macros for the player bytes (the real code surely had a larger player struct).
