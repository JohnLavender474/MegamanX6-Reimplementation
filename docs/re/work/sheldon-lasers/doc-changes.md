# Proposed changes to shared docs (laser puzzle, stage 07)

The new sheet is `docs/enemies/laser-reflector-puzzle.md`. These changes are for files that other agents
own. Apply them when you merge.

## `docs/engine.md`

### "Damage tables": add two tables

````markdown
`dmg_tbl_reflector` (`0x8007451C`, the laser puzzle parts of stage 07; a counted hit turns a turning
reflector, see `docs/enemies/laser-reflector-puzzle.md`):

| ids | damage |
|---|---|
| 0–15 | 3 3 3 4 6 5 7 – 3 7 6 6 6 8 8 8 |
| 16–31 | 8 8 15 10 16 1 32 – 4 6 4 4 4 4 4 4 |
| 32–47 | 3 3 6 8 8 6 4 4 4 4 4 5 8 4 7 8 |
| 48–63 | 4 4 2 3 6 4 4 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then – (id 79) |

`dmg_tbl_reflector_wait` (`0x800745BC`, for 33 updates after a reflector turn): – for every id, except
id 0 (3), id 9 (7) and id 18 (15).
````

### "Weapon ids": add a row

````markdown
| 79 | the stage 07 laser probe (weapon type 79, `0x80044554`); class 2 in the standard tables, so it hurts nothing [certain for the tables read] |
````

### "Damage to an object": add one line

````markdown
- The weapon id is the weapon object's type (byte 1): `ApplyWeaponHit` reads it [certain]. Class 2
  makes `ApplyWeaponHit` return 0, and `ContactWeapon` returns 0 for 126 ("already hit by this shot")
  too [certain].
````

### "Object classes": add the weapon class row

````markdown
| weapon (player shots, stage laser probes) | 32 | `0xA0` | `0x800950A0` | `0x80071220` (loop `0x8002012C`) | `GetWeaponObject` (`0x8002C32C`) |
````

### "Camera" (or "Layout"): one line

````markdown
- `TileCollisionAt(obj, x, y)` (`0x80030234`) returns the collision value of the tile at (x, y) on the
  object's layer (59 reads as 56). The stage 07 laser stops on any value except 0 and 37–47.
````

## `catalog/helpers.json`

- Add the entries in `helpers-proposed.json`.
- `0x8009721C` is named `cam.target` and `0x80097220` `cam.target+0x4`. The teleporter sheet names
  layer-0 `+0x24` the x maximum and `+0x28` the y maximum, and the laser doors raise them to open the
  next room. Proposed: rename to `cam0.max_x` and add `0x80097220` `cam0.max_y`; add `0x8009721E`
  `cam0.min_x` (`+0x26`), `0x80097222` `cam0.min_y` (`+0x2A`) [likely].
- Proposed name for the 16-bit counter `0x800FCFEC` (overlay 07 data): `laser_shot_id` (stage-local, so
  maybe only in the sheet).

## `docs/players/map.md`, "Weapon objects"

````markdown
Type 79 is not a player shot: it is the probe box of the stage 07 laser (`0x80044554`, main program;
`docs/enemies/laser-reflector-puzzle.md`). It follows a visual type 11 laser and has no damage.
````

## `docs/stages/sheldon.md`: the puzzle records (stage 7, area 0)

````markdown
### Laser puzzle (main type 13)

Roles from `docs/enemies/laser-reflector-puzzle.md`. Kind 2 = Hard and above only.

| record | x, y | `id` | kind | part |
|---|---|---|---|---|
| 43 | 3080, 304 | `0x15` | 0 | aiming beamer, starts down-left |
| 44 | 3080, 408 | `0x3F` | 0 | fixed reflector, r 15 |
| 82 | 3152, 360 | `0x50` | 0 | door 0 |
| 45 | 3360, 288 | `0x16` | 0 | aiming beamer, starts down |
| 46 | 3600, 288 | `0x16` | 0 | aiming beamer, starts down |
| 47 | 3760, 448 | `0x30` | 0 | fixed reflector, r 0 |
| 83 | 3808, 408 | `0x51` | 0 | door 1 |
| 48 | 3904, 296 | `0x06` | 0 | fixed beamer, down |
| 49 | 3904, 440 | `0x3E` | 0 | fixed reflector, r 14 |
| 50 | 4064, 424 | `0x40` | 0 | turning reflector: r 9, 5, 1, 13 |
| 84 | 4144, 360 | `0x52` | 0 | door 2 |
| 51 | 4216, 280 | `0x00` | 0 | fixed beamer, right |
| 52 | 4440, 280 | `0x41` | 0 | turning reflector: r 15, 13, 11, 9, 7, 5, 3, 1 |
| 53 | 4528, 432 | `0x02` | 2 | fixed beamer, up (Hard) |
| 54 | 4576, 440 | `0x42` | 0 | turning reflector: r 12, 8, 4, 0 |
| 55 | 4656, 328 | `0x43` | 0 | turning reflector: r 11, 10, 9, 8, 7, 8, 9, 10 |
| 85 | 4800, 376 | `0x53` | 0 | door 3 |
| 86 | 4728, 464 | `0x54` | 0 | door 4 (opens downward) |
| 56 | 4776, 520 | `0x04` | 0 | fixed beamer, left |
| 57 | 4664, 520 | `0x3B` | 0 | fixed reflector, r 11 |
| 58 | 4776, 616 | `0x44` | 0 | turning reflector: r 0, 12, 8, 4 |
| 59 | 4664, 696 | `0x45` | 0 | turning reflector: r 9, 7, 5, 3, 1, 15, 13, 11 |
| 60 | 4584, 680 | `0x22` | 0 | relay beamer, starts up |
| 61 | 4488, 584 | `0x46` | 0 | turning reflector: r 0, 12, 8, 4 |
| 62 | 4400, 696 | `0x47` | 0 | turning reflector: r 8, 6, 4, 2, 0, 14, 12, 10 |
| 63 | 4344, 608 | `0x48` | 0 | turning reflector: r 0, 14, 12, 10, 8, 6, 4, 2 |
| 64 | 4280, 696 | `0x49` | 0 | turning reflector: r 3, 15, 11, 7 |
| 65 | 4288, 528 | `0x26` | 0 | relay beamer, starts down |
| 66 | 4200, 616 | `0x4A` | 0 | turning reflector: r 0, 14, 10, 9, 8, 6, 4, 2 |
| 67 | 4168, 552 | `0x4B` | 0 | turning reflector: r 12, 7, 3, 15, 10, 7, 3, 15 |
| 68 | 4096, 528 | `0x06` | 2 | fixed beamer, down (Hard) |
| 69 | 4008, 528 | `0x06` | 2 | fixed beamer, down (Hard) |
| 70 | 3944, 568 | `0x4C` | 0 | turning reflector: r 0, 14, 12, 10, 8, 6, 4, 2 |
| 71 | 4240, 700 | `0x02` | 0 | fixed beamer, up |
| 72 | 4160, 696 | `0x04` | 0 | fixed beamer, left |
| 87 | 3928, 720 | `0x55` | 0 | door 5 (opens downward) |

Rooms: door 0 spawns 3168–3791 × 256–447 (records 45, 46; 47 is just below and spawns normally), door 1
spawns 3824–4127 × 256–447, door 2 spawns 4160–4783 × 256–447, door 4 spawns 3856–4783 × 480–703, door 5
spawns 3856–4767 × 736–991. The route is: room 0 (x 2896–3167) → door 0 → room 1 → door 1 → room 2 →
door 2 → room 3 (x 4160–4815) → door 3 or door 4 (down) → the lower rooms (y 480–735) → door 5 (down)
[likely for the route, from the rectangles].
````

## `docs/TODO.md`

````markdown
- `spriteexport`: the stage 07 laser (visual 11, misc 7) sets sprite numbers 210–245 directly from the
  second sprite source (`sprite_bank_base` word `+0x40`); export them for `laser-reflector-puzzle`.
- Play checks for the laser puzzle: aiming beamer cycle 304 updates; sabre wait 33 updates; fixed beamer
  on/off; 1 damage from the laser head only; whether doors stay open after a death.
````

## `docs/process/process-log.md`

````markdown
- Laser puzzle of Shield Sheldon's stage (stage 07 main type 13, visual 11, weapon 79, misc 7):
  `docs/enemies/laser-reflector-puzzle.md`. Notes, pseudo code, C and probes in
  `~/mmx-data/x6/re/sheldon-lasers/`. Export `enemy-07d032a495`.
````
