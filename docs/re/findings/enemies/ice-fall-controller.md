# Ice fall controller (stage 04, stage logic type 12): behaviour sheet

Proposed as `docs/enemies/ice-fall-controller.md`.

An invisible stage logic object of Blizzard Wolfang's stage. It has a table of 15 trigger zones.
When the player enters a zone for the first time, it makes one ice block spawner (main type 17,
`docs/enemies/ice-block.md`) at a fixed point. It also shakes the camera during an avalanche,
starts and stops the rumble sounds, and at the bottom of the ice part changes 3 map cells and
ends. "Ice fall controller" is a descriptive name.

Overlay `04` (stage 2, area 0). Entry `0x800F049C`, states `0x800F6DFC` (0 set-up `0x800F058C`,
1 main `0x800EFFD8`, 2 `DeleteObject3` `0x800F05D0`), trigger table `0x800F6E08`. One record in
the fixed list (index 13, kind 3, `b` 12, `id` 0). Live in all three Wolfang states. Kit:
`python3 -m mmxdemake.rekit --state wolfang-3_falling-ice.state.gz --type 12 --class logic`. All
values are [certain for the code] unless a line says otherwise.

## Fields

| field | use |
|---|---|
| `+0x05` | 1 while the falling-ice rumble (5/9) plays |
| `+0x06` | 1 while the avalanche rumble (5/8) plays |
| `+0x07` | sound timer: 240 at the avalanche rumble start, −1 per test |
| `+0x14` (s16) | camera shake counter |
| `+0x16` … `+0x24` | one "done" byte per trigger |

## Set-up (state 0)

Clears the 15 "done" bytes, `game+0x27`, the shake counter, `+0x05`, `+0x06`, `+0x07`; state 1.
The set-up runs again after a death (the fixed-list objects are made again): in
`wolfang-3_falling-ice` (checkpoint 1) only trigger 6 is done, while in `wolfang-2_ice-slope`
trigger 0 is done [certain for the states].

## Entry (each update, `0x800F049C`)

1. When the player is at x ≥ 4833 and y ≥ 1217: `CopyTiles(0, 4672, 1056, {4672, 912, 3, 1})`,
   `PlaySound(5, 3)`, `DeleteObject3`. The copy puts the 3 cells at (4672, 912) to (4672, 1056)
   [likely for the meaning of the 4 values; the effect on the map is not checked; maybe it closes
   the shaft above the player].
2. When the shake counter is not 0: `camera_layer0` y + 3 (counter even) or − 3 (odd); counter − 1.
3. The state function.

## Main (state 1)

Nothing when `game.checkpoint` ≥ 3.

**Sounds.** It looks at the 48 main objects. For each falling piece (type 17, `var` 6) with an age
(`act4`) of 7 or more: when `+0x05` is 0, `PlaySound(5, 9)`, `Rumble(5)`, `+0x05` = 1. For each
avalanche block (`var` 4 or 5): when `+0x06` and `+0x07` are 0, `PlaySound(5, 8)`, `+0x06` = 1,
`+0x07` = 240. Then the stop rules, in order (5/3 is a 56-sample tone on the same SPU voice 14 as
5/8 and 5/9, so it cuts them [likely]):

1. `game.checkpoint` > 0 and player y ≥ 625: 5/3 (each update).
2. No falling piece and `+0x05` = 0: 5/3, `+0x05` = 0.
3. `+0x07` − 1 when not 0.
4. No falling piece and `+0x06` = 0: 5/3, `+0x05` = 0. Else, when no avalanche block and
   `+0x05` = 0: `+0x07` − 1 when not 0, else 5/3 and `+0x06` = 0.
5. `+0x07` − 1 when not 0 (not on the second branch of rule 4).

As read, rule 2 plays 5/3 in each update with no falling piece, also during an avalanche. So it
would stop the avalanche rumble in the update that starts it [certain for the code; the sound in
play is open].

**Triggers.** For each row i = 0 … 14 of the table that is not done: when trigger x0 < player x <
x0 + 96 (and, for rows 12, 13, 14, player y ≥ 448, 688, 848; else the loop ends for this update),
`GetMainObject` → type 17, `var` = the row's mode, the type 17 sprite source, x = the row's x, and:

- mode 2: `PlaySound(5, 9)`, `Rumble(5)`, count `+0x7E` = 180, y = row y + rand(−16 … 15);
- mode 3: y = row y;
- modes 0, 1: `Rumble(5)`, `PlaySound(5, 8)`, count = 360, shake counter = 480 (when lower),
  y = row y + rand(−16 … 15).

The row is marked done also when no main slot was free.

## Trigger table (`0x800F6E08`, 8 bytes per row: x0, x, y, mode, pad) [certain for the data]

| row | player x | extra | spawner (x, y) | mode | what |
|---|---|---|---|---|---|
| 0 | 497–591 | | (864, 1024) | 0 | avalanche |
| 1 | 769–863 | | (1104, 944) | 0 | avalanche |
| 2 | 1281–1375 | | (1600, 752) | 0 | avalanche |
| 3 | 1809–1903 | | (2224, 496) | 0 | avalanche |
| 4 | 2225–2319 | | (2384, 480) | 0 | avalanche |
| 5 | 2513–2607 | | (2656, 400) | 0 | avalanche |
| 6 | 3185–3279 | | (3320, 224) | 2 | falling ice |
| 7 | 3425–3519 | | (3568, 224) | 2 | falling ice |
| 8 | 3713–3807 | | (3856, 224) | 2 | falling ice |
| 9 | 3857–3951 | | (4000, 224) | 2 | falling ice |
| 10 | 4593–4687 | | (4720, 224) | 2 | falling ice |
| 11 | 4977–5071 | | (5080, 224) | 2 | falling ice |
| 12 | 4753–4847 | y ≥ 448 | (4720, 432) | 3 | bouncing blocks |
| 13 | 4833–4927 | y ≥ 688 | (4720, 648) | 3 | bouncing blocks |
| 14 | 4753–4847 | y ≥ 848 | (4720, 848) | 3 | bouncing blocks |

A 16th row (x0 1580; mode 15) is in RAM after the table; no code reads it (the loop ends at 14;
the test for row 15, y ≥ 976, is dead code).

Points on the map (`m096` collision) [certain for the map]: the avalanche points of rows 0, 1, 4,
5 are 63 px above an ice slope (collision 25, 26), row 2 55 px; row 3 is 128 px above flat ice.
The falling-ice points are at y 224, just under a ceiling row at y 208. The bouncing points are
32–48 px above a floor, at x 4720, at three heights of a descent.

## Summary for a reimplementation

- Keep one flag per trigger; the first entry into an x band of 96 px makes the spawner.
- An avalanche also shakes the camera ±3 px for 480 frames and plays a looping rumble.
- The falling ice plays a second looping rumble while pieces older than 6 frames exist.
- All stops when `game.checkpoint` ≥ 3 or at the bottom of the ice part.

## Open questions

1. The rumble sounds in play (rule 2 above).
2. What the `CopyTiles` at the end changes on the map.
3. `game+0x27`: cleared here, set to 120 by an ice block kill; no reader found.
