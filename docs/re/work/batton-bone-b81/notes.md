# Notes: method, evidence, open questions

## Method

1. `python3 -m mmxdemake.rekit --name batton-bone-b81`. It found the entry (`0x800EC29C`, main table
   entry 18), 34 functions, 9 state tables, 2 neighbour entries (visual 17, misc 56), 19 engine helpers
   by name, the animation table (8 animations) and the fields used.
2. Read `summary.md`, `calls.txt`, then `code.txt` from the entry, function by function.
3. Read with `--at`: `0x8002D354` (the one unnamed call), `0x8002D380` (`AimAt`, to get the sign
   convention), `0x8002D0D8` (`Dir32`), `0x8002D2B0` (`MoveObject`), `0x8002D230` (`MoveObject2`),
   `0x8002D2D4` (`GetRNG`), the spawner part `0x8002A0F0`–`0x8002A260` and `GetMainObject`
   `0x8002BBF4` (for the value of `flip` at spawn), `HitReactionDispatch` `0x80040290`, and the start of
   `ApplyWeaponHit` `0x800315CC`.
4. Read the boxes, speed tables and animation frames in `data.txt`; the two damage tables from
   `SLUS_013.95` (file offset = RAM − `0x80010000` + `0x800`).
5. Read the update tables from the save state (scratchpad `0x1F800080`, `0x84`, `0x90`) to check the
   neighbours: visual 17 → `0x800ECB48`, misc 56 → `0x800ECBAC`.
6. Looked at the export (`~/mmx-data/x6/sprites/enemies/batton-bone-b81/anim-0..7.png`) to name the
   animations.
7. `~/IdeaProjects/mmx-reference/`: facts only. The X4 port has a "bomb bat" (main 32) whose functions
   match several functions here by signature (`THIRD_PARTY.md`), so this code is shared with X4, with
   steps 7 and 8 (HOMING, RISE) added. No code was copied.

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | all 34 functions, annotated |
| `kit/data.txt` | boxes `0x800F5F04`–`0x800F5F13`, speed tables `0x800F5F14`, `0x800F5F1C`, animation frames, dispatch tables, the bomb boxes `0x800F6024`, `0x800F6028`, the two damage tables |
| `kit/summary.md` | helpers, tables, fields, animations |
| `SLUS_013.95` | damage tables `0x800743DC` (bat) and `0x800741FC` (bomb), ids 0–63 |

## Key facts and where they come from

- HP 3, contact damage 3, priority 6, layer = `mega.layer`, boxes: `bat_init` (`0x800EBFA4`).
- The id dispatch (0 → CHASE with bomb; 1 → STRAIGHT; 4 → HOMING; others → SCATTER): `bat_init`.
- Two hurt boxes while the bomb is held (`2 − +0x88` boxes): `bat_main` loop at `0x800EC1B8`.
- Bomb release at death only through box 0 and only while `+0x88` = 0: `0x800EC1D8`–`0x800EC1F0`.
- Chase target X.y − 80 px (`lui 0xFFB0`), speed × 1.5, drop test |dx| ≤ |vx| + 2 px, timeout 200:
  `chase_move` (`0x800EC3B8`).
- DROP timing 1 + 5 + 13 updates: `drop_start`, `drop_release`, `drop_end`.
- FLY AWAY ±3 px/frame, up 0.3125 (`0x5000`): `flyaway_start`; table `0x800F5F14`.
- SCATTER ±3.5 px/frame ± random 0..`0xFFFF`, up 1.5 (`0x18000`), +`0x100` per update, wait 0–63 for
  id 2: `scatter_start`, `scatter_wait`, `scatter_move`; table `0x800F5F1C`. `GetRNG` returns 0–255.
- HOMING 300 updates, aim every 4th update: `homing_start`, `homing_move`. RISE 2 px/frame: `bat_rise`.
- `0x8002D354` = `Dir32(obj.x − x, obj.y − y)`, the same form as `AimAt`: the direction from the
  object to the point (x, y).
- `flip` at spawn = 0: the spawner (`0x8002A150`–`0x8002A188`) writes flags, type, var, x, y and the
  record, not `flip`; `GetMainObject` clears all `0x9C` bytes of the slot [likely: the spawner reaches
  the allocator through a table by record kind at `0x8007347C`, not read].
- Bomb: HP 1, contact 5, `speed_y` −1, gravity `0x3000`, boxes, y + 32 px before the floor explosion:
  `bomb_init` (`0x800ECB84`), `bomb_main` (`0x800ECC10`).
- Misc entry 56 = `0x800ECBAC`: inside `bomb_init`, 10 instructions after its prologue (it skips
  `addiu sp,sp,-24` and `lui v1,0x800F`), and its end pops 24 bytes. So it is not a usable entry.
- `0x800CCEDC` (halfword) = `game.stage` | `game.area` << 8. The state has 6 / 0. The delete routine
  compares the halfword with 2 (stage 2, area 0).

## Verified against the save state

- The update tables: main 18 → `0x800EC29C`, visual 17 → `0x800ECB48`, misc 56 → `0x800ECBAC`.
- `game.stage` = 6, area 0.
- No main object of type 18 and no visual object of type 17 is live in the state (scan of the 48 main
  slots and 32 visual slots). So no live values (id, `flip`) could be checked.
- The export shows animations 0–7 as the code uses them: bat with bomb, release, bomb alone, bat
  without bomb, 4 debris pieces.

## Kit problems

1. **False neighbour.** The kit lists misc type 56 (`0x800ECBAC`) as a neighbour and prints it as a
   separate function. It is a table entry into the middle of `bomb_init`. Worked around: checked the
   table in the save state and read the overlap by hand. A test "entry inside a walked function" would
   flag it.
2. **Offsets into named globals.** `0x800970A8` (`mega.x`), `0x800970B4` (`mega.layer`) and
   `0x800CCEDC` (`game.stage`) show with an empty name, though `mega` (`0x800970A0`) and `game`
   (`0x800CCED0`) are in the catalog. Worked around by hand.
3. Neighbour code has raw offsets (as documented); `+0x70` in `bomb_main` is `collide_flags`.

## Open questions

1. Which ids the Rainy Turtloid records use.
2. `flip` at spawn (assumed 0, left).
3. The `game.stage` = 2 test: which stage is 2, and why the Turtloid copy has it.
4. Which engine code sets step 0 and step 1.
5. The box at `0x800F5F10` (no reader) and misc entry 56.
6. Whether `SpawnExplosion` hurts X or plays a sound.
7. Weapon ids 21–63.
