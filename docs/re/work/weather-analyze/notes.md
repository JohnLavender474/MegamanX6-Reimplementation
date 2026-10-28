# Weather Analyze: notes

## Method

1. Kit output in `kit/` (from `turtloid-1.state.gz`, main type 14, entry `0x800EB5F8`, 21 functions).
2. Read `summary.md`, `calls.txt`, all of `code.txt`, `data.txt`.
3. Wrote small helpers in `tools/` (`dis.py` = raw disassembly of RAM; `scan.py` = finds loads and
   stores whose `lui`+offset address is in a list; `recs.py` = type-14 records of list entry 6).
4. Scanned the main program (`0x80010000`–`0x80074000`) and overlay 09 (`0x800E9860`–`0x800F5500`)
   for `0x800CCEF6`, `0x800CCEF7`, `0x800CCEF8`. Followed the readers.
5. Found the class table at scratchpad `+0x8C` (`0x800F57C8`) and its object array `0x8009B7B0`
   (`0x30` bytes, 96 slots, update loop `0x8002048C`). Live slot 3 is type 11 = entry `0x800ED510`
   (the rain controller), state 1 (dry).
6. Fandom: HTTP 402, not read. The name is the user's.

## Key facts and sources

- `var` dispatch: `0x800EB608`–`0x800EB670` (signed byte; 1–4, 5, 6; else machine).
- `wa_index` = `var & 0xF` at `0x800EB6E8`; global `0x800F6BA0`.
- Tables (7 rows): `0x800F5DF4` pos, `0x800F5E10` sensors, `0x800F5E80` camera, `0x800F5E90` zones,
  `0x800F5EAC` 2 tile rects {x, y, w, h} = {2832, 512, 1, 20}, {2944, 512, 1, 22}.
- Zone test `0x800EB2F0`–`0x800EB358`: `x < lo` or `hi < x` → 0, else 1 (inclusive).
- Death `0x800EB35C`–`0x800EB450`. No DropItem. State 2 at `0x800EB89C` sets `0x800CCEF8` for index 3.
- `OffScreenCheck2` (`0x8002CBFC`) only returns a value; it does not set state 3. No code in the
  object sets state 3, so nothing despawns.
- `DeleteObject` (`0x8002CA14`) skips the record when `+0x10` is 0 (the children have no record).
- Sensor kill decrements `parent+0x07` (`0x800EB990`–`0x800EB9A8`).
- Hurt box set at `0x800EB860`–`0x800EB884` when `act4` = 0.

## Rain controller (stage-logic type 11, overlay 09)

- Dispatcher `0x800ED510`: if `game+0x27` → clear `+0x16`, `+0x18`; call state (table `0x800F60AC`, 5
  entries); then if `+0x1E` ≠ 0 a counter `+0x14` drives `camera_layer0+0xA0` = 1 and `+0x62` =
  `(cnt >> 2) << 8` every 4 updates (cycles 0..4) [effect not known; maybe a layer scroll or blend].
- State 0 `0x800ED5C4`: if `game+0x1D` = 3 → `0x8008EAB0` = 1; flags |= 8; copy 32 bytes from
  `scratch[0x30]+0x200` to `0x800F6BA4`; `+0x20` = `scratch[0x30]+0x460`.
- State 1 `0x800ED69C`, 2 `0x800ED774`, 3 `0x800ED3E4`, 4 `0x800ED850`. Fade helpers `0x8002B410`,
  `0x8002B3D4`, `0x800ED18C` (palette blend by 30 steps).
- Drain `0x800ED454`–`0x800ED4F4`: needs `mega+0x5C` ≠ 0 (signed) and `mega+0xE0` = 0; every 180
  updates `hp &= 0x7F`; if `0x8008EAFC` = 0: `hp = hp < 2 ? 0x80 : hp − 1`; old bit 7 restored. Every
  51 updates `mega+0xE8` = 1 (read and decremented by the HUD code at `0x80025064`).
- `mega+0x14C` ≠ 0 → `PlaySound(5, 4, 0)` (`0x800ED424`). Writers of `+0x14C`: `0x80034A1C` (1),
  `0x80034D30` (2), player code.
- `ContactMega` returns at once when `mega+0x5C` has bit 7 (`0x80031064`).

## Other users of the flags

- `0x800F0174` (teleport crystal type 28): needs `game+0x26` = 0 and X at or right of it.
- `0x80050250` (main-program visual type 24 step, table `0x80078DD4`): var 7, 8, 10, 12, 14, 15 need
  `+0x26`; var 34 needs `+0x28`; var 44 needs `+0x26` = 0.
- `0x80047460`, `0x8004755C` (main program, Quake and bank-2 sounds 34/35) test `+0x26`.
- `0x8001E7C0` copies `game+0x0C/+0x0D` to `+0x26/+0x27` (another game mode uses the bytes);
  `0x8001E81C` clears `+0x26`..`+0x35`.
- `game+0x27`: set at `0x800EE5D8` by an overlay object (class `+0x88`, entry `0x800EE488`) that puts X
  at its position and calls `0x8003D308(20, flip)`; cleared at `0x800EE660`, `0x800EE710`.
- `0x800F9B50` writes `+0x26` = 1, but it is outside overlay 09 (ends `0x800F6C10`): probably stale RAM.

## Records (list entry 6)

| id | kind | x | y |
|---|---|---|---|
| `0x10` | 0 | 1376 | 544 |
| `0x11` | 0 | 1888 | 624 |
| `0x12` | 0 | 3552 | 368 |
| `0x13` | 0 | 8000 | 384 |

The kind-3 records with b = 14 (ids `0x81`–`0x87`, `0x02`) are stage-logic type 14 (live object
slot 0 at `0x8009B7B0`, entry `0x8005335C`), not this object.

## Checks

- SetAnime numbers 0, 1–4, 5–8, 9, 13 and junk animations 10–12, 14–17 are all in the 18-entry table.
  No event flags tested.
- All boxes quoted are in `data.txt`.
- Fields match `summary.md` (+0x80 = parent pointer in the children).
- Not checked in play: everything about timing and the rain look.

## Open questions

See the sheet. Also: the meaning of `camera_layer0+0x62/+0xA0` in the rain dispatcher.
