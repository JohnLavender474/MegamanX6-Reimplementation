# Notes: method, evidence, open questions

## Method

1. `python3 -m mmxdemake.rekit --name monbando`. It found the entry (`0x800EA3D0`, main table entry 9),
   15 functions, the 3 state tables, 3 neighbour entries (one real: visual 7), 21 engine helpers by
   name, the animation table (11 animations) and the fields used.
2. Read `summary.md`, `calls.txt`, then `code.txt` from the entry, function by function.
3. Read the one call outside the walk with `--at 0x8002943C` (and `Quake` at `0x80029478` to compare).
4. Read the boxes in `data.txt`, and the three damage tables (`0x800743DC`, `0x8007433C`, `0x800741FC`,
   80 ids each) from `SLUS_013.95`.
5. Looked at the export (`~/mmx-data/x6/sprites/enemies/monbando/`, `anim-0.png` to `anim-10.png`) to
   name the animations.
6. Read `mmx-reference` (facts only) for the meaning of `mega+0x02`: engine functions use it to pick the
   HP bar of the character, and the X4 code tests `player->unk2 == 0` before X armour rules.
7. Read the live objects in the save state.

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | all 15 functions, annotated |
| `kit/data.txt` | boxes `0x800F5A48`–`0x800F5A53`, `0x800F5B68`–`0x800F5B8B`, animation frames, the dispatch tables, the debris list `0x800F5B40` |
| `kit/summary.md` | helpers, tables, fields, animations |

## Key facts and where they come from

- HP 32, contact damage 6, the three boxes, priority 4, start in step 4: `monb_init` (`0x800EA41C`).
- Guard: `monb_main` sets `damage_table` = `0x8007433C` when `anime.sprite` = 0, else `0x800743DC`.
- Blocked hit resets the wait: `monb_main` stores 0 to `+0x7C` when `ContactWeapon` returns 127.
- Wait of 62 updates: `monb_wait` compares the old timer with 61 (`slti 61`).
- Attack choice: `GetRNG() & 1`, then `lb 0x800970A2` (`mega+0x02`).
- 3 waves with delays 0, 6, 12: `monb_fire` loop (`s1` = 3 when `step2` = 0, else 1; `s2 += 6`).
- Shot kinds and constants: `shot_init` (`0x800E9EF0`), three branches on `+0x02`.
- Missile homing: `shot_move` (`0x800EA178`) uses `+0x8C` (50 at set-up), `slti 40`, `mega.y.px`
  (`0x800970AE`), steps of 2 px, range 8 px.
- Punch box removal: `shot_move` reads the parent (`+0x7C`) `anime.flag` = 3 and `state` = 2 or 3.
- Missile table by difficulty: `shot_init` tests `game.difficulty < 2`.
- Death: `SpawnExplosion` once, `SpawnJunk(4, {7, 8, 9, 10})`, `DropItem(21)`.

## Damage tables (from `SLUS_013.95`, `class/damage` per weapon id)

`0x800743DC` (open Monbando):

```
0:00/3 1:00/5 2:00/4 3:00/6 4:00/6 5:00/6 6:00/5 7:02/0 8:00/4 9:00/7 10:00/6 11:00/6 12:00/6
13:00/6 14:00/8 15:00/6 16:00/8 17:00/8 18:00/15 19:00/15 20:00/16 21:00/1 22:00/32 23:00/1
24:00/4 25:00/6 26-31:00/4 32:00/3 33:00/3 34:00/6 35:00/8 36:00/8 37:00/6 38-42:00/4 43:00/5
44:00/8 45:00/4 46:00/15 47:00/8 48-51:00/4 52:00/3 53:00/6 54:00/4 55:00/127 56:00/3 57:00/8
58:00/4 59:00/4 60:00/15 61:00/5 62:00/15 63:00/3 64-78:ff/0 79:02/0
```

`0x8007433C` (guard, punch box, missile on difficulty 2+): every id `ff/0`, except 7, 23, 79 `02/0`,
55 `ff/127`, 56 and 57 `ff/1`.

`0x800741FC` (wave, missile on difficulty 0–1):

```
0:00/3 1-8:00/4 9:00/5 10:00/4 11:00/4 12-17:00/7 18:00/15 19:00/4 20:00/16 21:00/2 22:00/32
23:00/4 24:00/4 25:00/6 26-31:00/4 32:00/3 33:00/3 34:00/6 35:00/8 36:00/8 37-54:00/4 55:02/0
56:00/1 57:00/1 58:00/4 59-62:00/127 63:00/3 64-79:ff/0
```

## Verified against the save state

- No Monbando and no visual type 7 object is live in `turtloid-1.state.gz` (48 main slots and 32
  visual slots read). So the timings are not checked in RAM.
- The player in this state has `mega+0x02` = 0 (X) and `game.difficulty` = 1.
- The export shows the guard pose (sprite 0), the arm attacks (anims 2–4), the blue wave (5), the
  missile (6) and the debris (7–10), as the code uses them.

## What the kit saved, and what stayed manual

- Saved: the functions and their order, the dispatch tables, the helper names, the field names, the
  animation table, the boxes in hex.
- Manual: the meaning of each function; the shot code (`neighbour code`, offsets not named; it uses
  the main layout anyway); the call `0x8002943C`; the damage tables beyond the first 80 bytes in
  `data.txt`.
- Kit problem: two false neighbour entries. The misc table in RAM (`0x800F58FC`) has entry 1 =
  `0x800EA02C` and entry 29 = `0x800E9E98`. Both addresses are inside Monbando functions (a jump
  target in `shot_init` and a delay slot in `monb_main`), not function starts. The kit listed them as
  neighbours and printed their code a second time. Monbando never calls `GetMiscObject`, so I ignored
  them. A check "neighbour address inside an already walked function → skip" would stop this.
- Catalog note: `dmg_tbl_guard` (`0x8007429C`) holds class 0, damage 127 for every id in `data.txt`
  (the dump of `0x800741FC` runs into it). That is "everything kills", not "everything deflected".
  The table that deflects everything is `0x8007433C`. Check the catalog note.

## Open questions

1. Who sets step 0 (engine hit reaction)?
2. `mega+0x02` = character (0 X): confirm with a Zero save state; the punch is only seen with Zero.
3. The unused box `0x800F5B6C` (−8, −8, 16, 16).
4. What `Quake2` changes compared to `Quake`.
5. Exact update counts of the animation timings (±1).
6. The FALL lock: after a later fall, no animation frame has flag 2, so step 4 never ends.
7. Why misc table entries 1 and 29 point into Monbando's code (stale entries of another area or
   revision?).
