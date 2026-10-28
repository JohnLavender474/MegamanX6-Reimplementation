# Notes: rain effect (Yammark, main type 37)

Short sheet: `docs/enemies/rain-effect.md` in mmx-demake-tools. It is a pure effect, so there is no
pseudo code and no C.

## Method

1. Kit output in `kit/` (8 functions, entry `0x800EF578`, `yammark-1_start.state.gz`). Read
   `summary.md`, `calls.txt`, `code.txt`, `data.txt`.
2. Looked at the export (`~/mmx-data/x6/sprites/enemies/rain-effect/anim-0.png`, scaled ×4): five
   32 × 32 cells of blue/white diagonal streaks, top right to bottom left. It is rain.
3. Read RAM of all save states with a scratch script (`probe.py` in the session scratchpad):
   `0x800CD30A` = 0 everywhere; `game.stage` = 1 (Yammark), 6 (Turtloid), 0 (intro).
4. Searched overlay 03 and the main program for loads of `0x800CD30A` (imm `0xD30A`): overlay 03
   `0x800EE164` (tests 8; another object, `0x800EE154`) and `0x800EF314` (this one, tests 6).
   Overlay 09: `0x800EDAEC`, `0x800EE9FC` (Nightmare shadow, tests 7), `0x800EF294`. No writer found
   by this search (it may use a base register).
5. Searched for the flag `0x800971F0` (`mega+0x150`): only reader `0x8002D5D0` (lb `0x71F0`). Read it
   with `rekit --at`: a move function with the 3/4 rule. 30 `jal 0x8002D5D0` in `0x80035190`–
   `0x8003AEF8` (player code; uses offsets `0x89`, `0xCE`).
6. `GetRNG` (`0x8002D2D4`) read with `--at`: returns `& 0xFF`.
7. Searched for `addiu rX, zero, 37`: in overlay 03 only the rain's own spawn (`0x800EF794`); in the
   main program 6 hits, none makes a main object of type 37. Yammark section 1 records
   (`objects.json`) have no `b` = 37.

## Key facts

- `0x800EF304` set-up: `0x800CD30A` ≠ 6 → `DeleteObject3`. No boxes, damage 0, priority 0, timer
  (`+0x7C`) 5, animation 0. `var` 0: graphics from the bank, step 0. `var` ≠ 0: x = cam.x +
  2·rng + (rng & 31), y = cam.y − 16, speed (−7, −7) px/frame (`0xFFF90000`), step 1.
- `0x800EF620` generator step: loop i = 0..7 over `0x800EF4D4(i)` (rect table `0x800F7900`, 10 bytes:
  x0, y0, x1, y1, stage). First match → `mega+0x150` = 1, timer−1, at 0 timer = 5 and spawn
  (`0x800EF76C`). No match → `mega+0x150` = 0.
- `0x800EF6D4` drop step: `AnimeTick`, `MoveObject`; state 2 when x < cam.x − 16, y > cam.y + 256 or
  `OffScreenCheck2(336, 48)`; else `DisplayCheck(32, 32)`.
- `0x8002D5D0`: if `mega+0x150` ≠ 0 and `flip` ≠ 0: `x += (3·speed_x)/4` (round to 0); else
  `x += speed_x`; always `y -= speed_y`.

## Rect table `0x800F7900` (decoded)

| # | x0 | y0 | x1 | y1 | stage |
|---|---|---|---|---|---|
| 0 | 3200 | 1648 | 4800 | 2160 | 1 |
| 1 | 0 | 224 | 1616 | 544 | 0x13 |
| 2 | 2256 | 0 | 2592 | 896 | 5 |
| 3 | 2896 | 1392 | 3856 | 1744 | 5 |
| 4 | 960 | 2336 | 1920 | 2688 | 5 |
| 5 | 384 | 2800 | 1312 | 3376 | 5 |
| 6 | 2400 | 2672 | 3440 | 3472 | 5 |
| 7 | 16 | 240 | 608 | 912 | 0x15 |

## Open questions

1. What makes the generator (`var` 0).
2. The writer of `0x800CD30A` and the meaning of its values.
3. The stage values 5, `0x13`, `0x15` in the table.
4. The 30 callers of `0x8002D5D0` (which player moves are slowed).
