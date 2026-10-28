# Notes: method, evidence, open questions

## Method

1. `python3 -m mmxdemake.rekit --name worm-spider`. It found the entry (`0x800EA264`, main table entry
   3), 43 functions, 10 state tables, 4 neighbour entries, 21 engine helpers by name, the animation
   table (24 animations) and the fields used.
2. Read `summary.md`, `calls.txt`, then `code.txt` from the entry, function by function.
3. Read the boxes, the height table `0x800F70C4` and the two jump tables of the effect object
   (`0x800E9864`, `0x800E98AC`) in `data.txt`.
4. Read the damage tables `0x800743DC` (worm) and `0x8007429C` (shot) from `SLUS_013.95`
   (file offset = RAM − `0x80010000` + `0x800`).
5. Read the code outside the walk with `--at`: `0x80049D90` (the shot's wall effect) and the slot
   allocator it calls, `0x8002C398`. Looked at `ContactWeapon`, `ContactMega` and `CornerCheck` only to
   see which object bytes they write.
6. Looked at the export (`~/mmx-data/x6/sprites/enemies/worm-spider/`, 24 PNGs) to name the animations.
7. Computed the jump arcs and the animation times with a small script (MoveObject2 order: `y -= vy`,
   then `vy -= gravity`, then the clamp to −6.5 px/frame).

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | all 43 functions, annotated |
| `kit/data.txt` | boxes `0x800F6CBC`, `0x800F6CC0`, `0x800F7154`–`0x800F715F`; height table `0x800F70C4`; dispatch tables `0x800F70E0`–`0x800F7178`; effect jump tables `0x800E9864`, `0x800E98AC` |
| `kit/summary.md` | helpers, tables, fields, animations |
| `SLUS_013.95` | damage tables `0x800743DC`, `0x8007429C` |

## Key facts and where they come from

- HP 16, contact 4, damage table, priority 7, the facing at spawn: `worm_init` (`0x800E99D4`) and
  `worm_face` (`0x800EACB0`).
- The three forms by `var`: `worm_init` (y − 32 / y + 32 / y + 26; anims 0, 14, 7).
- The own box at `+0x80`: `worm_init` copies the 4 bytes at `0x800F6CC0` with `lwl`/`lwr` to
  `current_main + 0x80`; `emerge_start` points both boxes at `current_main + 0x80`; `emerge_grow`
  writes `+0x81` (oy) and `+0x83` (h) from `0x800F70C4[flag − 1]`.
- Proximity 96 × 96 px: `wait_near` (`slti …, 97` on |dx| and |dy|).
- Timers: 30/15 (`emerge_grow`), 30 and 20 (`attack_start`, `attack_fire`), 90/45 (`sway_start`),
  120 (`retract_wait`). The short values are for `game.difficulty >= 2`.
- The hit flag `+0x87`: written by `worm_main` from the `ContactWeapon` result, read only by
  `sway_wait`.
- Jumps: `jump_start` (`0x88888`, `0x48D1`), `jump_lob` (`0x80000`, `0x8000`, `(mega.x.px − x.px) << 11`),
  counter `+0x86` = 3 in `worm_init`, decremented in `jump_air` at flag 3.
- Shot: `shot_init` (`0x800E9DCC`): HP 1, contact 4, boxes, `ratan2` + `rcos`/`rsin` × 80.
- Spark position, ripple blink (60 updates, `display = timer & 1`): `fx_init`, `fx_main`, `fx_blink`.

## Kit observations

- The neighbours "misc type 1" (`0x800EA02C`) and "misc type 29" (`0x800E9E98`) are false. Both
  addresses are inside other functions (`fx_init`, `shot_init`), and the code never makes those types.
  The real neighbours are visual type 3 (the shot) and misc type 5 (the effects).
- The fields table does not list `+0x80`–`+0x87`. The code reaches them through `current_main`
  (`0x1F80004C`), not through the object register. The same holds for the misc object's timer at
  `current_misc + 0x54`.
- `mega.x.px` (`0x800970AA`) and `mega.layer` (`0x800970B4`) show as unnamed globals.
- The catalog note of `dmg_tbl_guard` (`0x8007429C`, "everything deflected") is not correct: every id
  is class 0 with 127 damage.
- Junkroid uses the same table `0x800743DC`. Its sheet gives id 23 as class 2; the bytes give class 0,
  damage 1 (only id 7 is class 2).

## Verified against the save state

- The export of `yammark-1_start` shows the floor and ceiling nests, the worm frames, the water
  ripple/splash/spray, the spark and the energy ball, as the code uses them.
- Not run in an emulator: the timing and the arcs are from the code only.

## Open questions

1. What `0x80049D90` shows (engine object type 3, var 1, animation table `0x8007789C`).
2. Which code sets the shot's `+0x70` before its own `CornerCheck`.
3. Animations 3, 6, 17, 20 and some event flags have no user.
4. Records with `var` > 2.
5. The ceiling spark side (+12 px when facing left).
6. Weapon ids 32–63 and the real table length.
