# Notes: Rescue Reploid (stage object type 4, code in the main program)

Sheet: [`~/IdeaProjects/mmx-demake-tools/docs/enemies/rescue-reploid.md`](../../findings/enemies/rescue-reploid.md).

## Method

- Kit: `python3 -m mmxdemake.rekit --name rescue-reploid --state yammark-1_start.state.gz
  --type 4 --class stage` (output copied to `kit/` here; the original stays in
  `~/mmx-data/x6/re/yammark-stage-object-4/kit/`).
- Read all of `code.txt` (25 functions). Read with `--at`: the shot code `0x8004F2EC`, `0x8004F624`,
  `0x800500C0` (state table `0x80078CCC`, right after the Reploid's step table); the Nightmare Virus
  search `0x80042D98` and its callers `0x8004270C`, `0x800429D4`; the check `0x80042E68`.
- Decoded the animation table `0x80078BEC` (36 entries, ends where the state table `0x80078C7C`
  starts) from the executable with a small script (scratchpad, not kept).
- Searched the RAM of `yammark-1_start` (`0x80010000`–`0x80100000`) for every `lui/addiu` pair that
  builds `0x800C1220`: 15 hits, all in the engine; one is the virus search.
- Searched for `lw r,128(x)` followed by `sb ?,7(r)`: no hit. The `act4` = 2 write is documented in
  `docs/enemies/nightmare-virus.md` (GRAB step, written by another agent at the same time).

## Identification

- Sprite bank: the set-up calls `GetSpriteBank(141)`; `docs/objects-legend.md` names bank 141
  "savable Reploid".
- `GetReploidStatus` / `SetReploidStatus` = `0x8004FF4C` / `0x8004FF00`: the same addresses as
  `symbols/misc.txt` in `mmx-reference/MegaManX6_PS1_Modding` (facts only; comment there: 2 = rescued,
  3 = dead, 4 = zombie). `game.lives` at `+0x39`, `maxHPs[2]` at `+0x5B`, `tanksAmmo` at `+0x63`,
  `tanks` at `+0x6A`, `reploids[16]` at `+0xD8` match `include/common.h` there.

## Save-state checks

| state | `game+0x39` (lives) | max HP X / Zero | `game.reploids` words 0–1 | live type 4 / 26 |
|---|---|---|---|---|
| `yammark-1_start` | 1 | 32 / 32 | 0, 0 | none |
| `yammark-2_boss` | 8 | 34 / 32 | `0x00200222`, `0x02220222` | none |
| `yammark-3_caterpirade` | 8 | 36 / 32 | same | none |
| `turtloid-1` | 8 | 36 / 32 | same; all other words 0 | none |

- Status 2 for ids 0, 1, 2, 5, 8, 9, 10, 12, 13, 14 (10 rescued). No other word is set, so the
  Yammark Reploids are global ids 0–15.
- Lives 1 → 8 with 10 rescues: consistent with +1 per rescue and some deaths (not checked).
- No live object of type 4 or 26 in any state; the stage-object slots hold only type 8 (boss gates,
  `yammark-2_boss`) and type 1 (Caterpirade, `yammark-3_caterpirade`).

## Key facts and where they come from

- Rescue overlap: `0x8002DCF4` compares the terrain boxes of both objects (x with the facing,
  y plain): |dx| < w1 + w2 and |dy| < h1 + h2.
- Heal table `0x80078BE8` = `02 04 08 10`. The index (0–3) is also passed in `a2` to `0x8004ED4C`
  and added to the sub tank (`s4`).
- Hover: `y = +0x7C + (rsin((*(u32*)0x80097420 & 0x7F) << 5) >> 10)`.
- Infected HP 13 and contact 6 in `0x8004FC40`.
- Muzzle offsets `0x80011674`: (−14,−6 | 14,−6), (−8,0 | 8,0), (−8,−14 | 16,−14), (−12,−16 | 12,−16),
  (−14,−6 | 14,−6), (−10,−17 | 8,−17), (−10,−2 | 8,−2), (−14,−6 | 14,−6).
- Bomb x speed: `(dx << 16) * 0xEA0EA0EB` (signed, high word) + `(dx << 16)`, `>> 6`, minus the sign:
  the compiler's division by 70.

## Positions (record point vs the collision map)

Record points with a solid collision pixel at the point (standing): 40, 41, 45, 46, 50, 51.
Records 42 and 37 have no floor within 200 px (hovering). The others have a floor 16–152 px below
(hovering, except 49 where the box bottom touches the floor at 1888 [likely standing]).

## Open questions

See the sheet. Plus: `0x8004A2B4(0, 0, obj, 1, 0)` called by the bomb every 8 updates.
