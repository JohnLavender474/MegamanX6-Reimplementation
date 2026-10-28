# Doc changes from `heatnix-6_snake-room.state.gz`

Exact old / new text per file. The agents for `heatnix-4_cave` and `heatnix-5_snake-rising` can
change the same lines (the save-state lists, the `var` 8/9 rows, open questions). Apply their
changes first and merge where the old text no longer matches.

## `docs/enemies/nightmare-snake.md`

### 1. Save states (intro paragraph)

Old:

```
state 0) and `heatnix-2_mini-boss.state.gz` (the fight runs: Zero at (366, 618), the ring at
(617, 584), rolling right at full speed). Written with the RE kit (`python3 -m mmxdemake.rekit
```

New:

```
state 0) and `heatnix-2_mini-boss.state.gz` (the fight runs: Zero at (366, 618), the ring at
(617, 584), rolling right at full speed). `heatnix-6_snake-room.state.gz` is just after the
`var` 10 fight (see "CHASE"). Written with the RE kit (`python3 -m mmxdemake.rekit
```

### 2. The `var` table, row 10, and the line under the table

Old:

```
| 10 | record 26, `id` 10 (3378, 1024) | x 3616 | step 7 | over lava (layer 2), jumps when the player is within 48 px in x (four kill boxes, visual 29 `var` 7) [likely] |

`var` 4 and 6–10 are not in a save state yet; their rows come from the code only.
```

New:

```
| 10 | record 26, `id` 10 (3378, 1024) | (3616, 1296); the lava and the four kill boxes at once, the intro when the player x ≥ 3344 | step 7 | swims in the lava toward the player, leaps up through the platforms when the player is within 48 px in x (see "CHASE") |

`var` 4 and 6–9 are not in a save state yet; their rows come from the code only. `var` 10: the
code, and the end of the fight in `heatnix-6_snake-room`.
```

### 3. New section after ROLL (before "GEM SHOTS")

Old:

```
### GEM SHOTS (gem step 1) [certain]
```

New:

```
### CHASE (step 7, `var` 10) [certain for the code]

The room: the top corridor between gate `id` 14 (x 3200) and gate `id` 15 (x 3824); walls x 3232
and 3808 (inner faces), three 64 × 32 platforms at x 3328–3392, 3488–3552, 3648–3712 (top y 1184),
lava from y 1232 down (floor y 1376 under it); camera x 3200–3504, y 944–1184 [certain for the
collision and the state].

**Set-up** (state 0, once, then it waits): camera follow point `cam+0x2E` − 64 (the view shows
64 px more below the player); four lava kill boxes (visual 29 `var` 7, below) at x 3296, 3456,
3616, 3776, y 1304; layer 2 mode 2 (layer 0 + offsets), offsets (−2272, 608), shown; body at
x 3616, y = camera target min y + 352 = 1296 (in the lava). Then it waits until the player x ≥
3344: `PlayerEvent(20, 64)` and the common set-up, with **no terrain box** (the ring passes
through the platforms) and `+0x8D` = 0. The record is in view as soon as gate 14 opens, so the
lava and the boxes are there from the room entry [likely].

Sub-steps (`0x800F3FB0`):

| sub | what |
|---|---|
| 0 | Toward the player: `speed_x` ±1, `accel_x` ∓1/16; sub 1 (right) or 3 (left). |
| 1, 3 | Roll at up to 2 px/frame. When \|x − player x\| < 48, or x ≥ 3729 (right) / x < 3296 (left): stop, `speed_y` +1 (up), gravity −1/16, **wait 50**, sub 2. |
| 2 | Rise, up to 3 px/frame; above y = min y + 192 (1136) the gravity turns over. When it starts to fall: `speed_y` −1, gravity +1/16, **wait 90**, sub 4. |
| 4 | During the 90 updates: no move, `+0x8D` = 1 (the gems may shoot). Then `+0x8D` = 0, fall at up to 2 px/frame; below y = min y + 352 (1296): sub 0. |

Computed from the code: the top of the leap is about y 1064 (rise about 112 updates, fall about
120) [likely: not timed in play]. The gems shoot only during the hang: 90 updates against the
120-update shot timer, so at most one volley per leap. In `heatnix-6_snake-room` layer 1 was last
at (−3060, −580): the body died at (3572, 1092), in the air.

Death as `var` 5 (the `var` 8/9 lava parts do not run); state 3: `game+0x26` = 1 (gate 15 can
open), `DeleteObject3` at once. The kill boxes and the lava stay after the death [certain: four
boxes live and layer 2 mode 2 in the state].

### GEM SHOTS (gem step 1) [certain]
```

### 4. State 3 (end)

Old:

```
**State 3 (end):** `game+0x26` = **1**: boss gate `id` 7 at (944, 608) can now open. While the player
```

New:

```
**State 3 (end):** `game+0x26` = **1** for every body (`var` 4–10; each body set-up writes 0), so
the next locked gate can open: for `var` 5 boss gate `id` 7 at (944, 608); for `var` 10 gate `id`
15 at (3824, 1024) (`heatnix-6_snake-room`: flag 1, gate 15 waits in step 0). While the player
```

### 5. Kill boxes

Old:

```
**Kill boxes** (`var` 6, 7; the lava fights of `var` 8–10): contact **127**, attack box `var` 6
(−80, −56, 160, 112), `var` 7 (−80, −64, 160, 128); never drawn [certain for the code; not in a state].
```

New:

```
**Kill boxes** (`var` 6 for `var` 8/9, `var` 7 for `var` 10): contact **127** (death at once),
attack box `var` 6 (−80, −56, 160, 112), `var` 7 (−80, −64, 160, 128); no hurt box, no terrain
box; never drawn [certain for the code].

`var` 7 (`heatnix-6_snake-room`: four live, state 1 step 2, contact 127, box `0x800F4004`)
[certain]: they do not move. The four together cover x 3216–3856, y 1240–1368, one band over the
whole lava. The box at x 3296 alone animates the lava: every 10 updates (`frame_count` % 10 = 0)
`+0x92` = (`+0x92` + 1) & 3 and layer 2 offset x = `snake_lava_waves[+0x92]` − 3232 (−2272,
−1664, −1056, −448), with a layer 2 redraw each update. The layer 2 map holds four lava strips,
576 × 192 px, at x 960, 1568, 2176, 2784, y 1840–2032; each lands on world x 3232–3808, y 1232–1424.
So the lava has 4 frames of 10 updates. When the player x ≥ 3825 each box adds 64 to
`cam+0x2E` and deletes itself; gate 15 (x 3824) clears all visual objects at its trigger first,
so this restore does not run in normal play [likely].
```

### 6. Open question 1

Old:

```
1. The other snakes (`var` 4, 6–10): the rows above come from the code only; save states of these
   fights would check them (the lava of `var` 8–10 is on BG layer 2).
```

New:

```
1. The other snakes (`var` 4, 6–9): the rows above come from the code only; save states of these
   fights would check them. `var` 10: a state during the fight would check the leap height and
   timing, and whether the lava (layer 2) is drawn in front of the ring (layer 1).
```

## `docs/stages/heatnix.md`

### 1. Save-state table: a new row after `heatnix-2_mini-boss`

Old (the last row of the table now):

```
| `heatnix-2_mini-boss` | 366, 618 | 921 | the first mini boss, in the room after the start (camera zone 7: x 336–624, y 480). Main type 25 `var` 0–5 (Nightmare Snake: the ring `var` 5, its four gems `var` 0–3 and the next snake `var` 4) and visual type 29 `var` 0–3 (its jet flames) are live (`docs/enemies/nightmare-snake.md`), Rescue Reploid `id` 32 |
```

New (add this row after it, or after the rows of `heatnix-3` to `heatnix-5` if they are there
by then):

```
| `heatnix-6_snake-room` | 3673, 1162 | 18913 | the top corridor just after the Nightmare Snake `var` 10 fight; checkpoint 6, `stage_texture_swap` 2. No main object; the four lava kill boxes (visual 29 `var` 7) are live, layer 2 shows the lava; `game+0x26` = 1, gate `id` 15 waits. Records 22 and 24 (upper route snakes `id` 6 and 9) are "spawned", 23 and 25 are not: the upper route |
```

### 2. Map, item 9

Old:

```
9. **The top corridor** (y 944–1184): gate `id` 14 (3200, 1024; camera x 3200–3504, max y 1184),
   checkpoint 6, main type 25 `id` 10 (3378, 1024), gate `id` 15 (3824, 1024; camera x 3808, y 944),
```

New:

```
9. **The top corridor** (y 944–1184): gate `id` 14 (3200, 1024; camera x 3200–3504, max y 1184),
   checkpoint 6, main type 25 `id` 10 (3378, 1024): a lava room (walls x 3232 and 3808, three
   platforms at x 3328–3392, 3488–3552, 3648–3712 with top y 1184, lava from y 1232 on BG layer 2,
   four kill boxes) with the fifth snake fight (`docs/enemies/nightmare-snake.md`, "CHASE"),
   gate `id` 15 (3824, 1024; camera x 3808, y 944),
```

### 3. The gate flag

Old:

```
Gates 7, 8, 10, 12, 14 and 15 open only when `game+0x26` ≠ 0 (`docs/enemies/boss-gate.md`). In this
stage the flag is probably set by each mini-boss fight (main type 25) [guess; open].
```

New:

```
Gates 7, 8, 10, 12, 14 and 15 open only when `game+0x26` ≠ 0 (`docs/enemies/boss-gate.md`). Each
Nightmare Snake body writes 0 at its set-up and 1 at its end (`SnakeInit`, `SnakeEnd`) [certain
for the code]; `heatnix-6_snake-room` has 1 after the `var` 10 fight, with gate 15 waiting.
```

### 4. Nightmare Effect table, grey block row

Old:

```
`id` 3 (5 blocks in the first tower) go away at checkpoint 3; `id` 4 (10 blocks in the shaft room and the side corridor) at checkpoint 4 |
```

New:

```
`id` 3 (5 blocks in the first tower) go away at checkpoint 3; `id` 4 (10 blocks in the shaft room and the side corridor) at checkpoint 4. The test runs only on screen: in `heatnix-6_snake-room` (checkpoint 6) the five `id` 3 blocks are still live, off screen |
```

### 5. Nightmare Snake rooms

Old:

```
player is kept at y ≥ 432 until it passes x 944 [certain for the code]. Records 22–26 are main 25
`id` 6, 7, 9, 8, 10 (more snake fights; no save state yet), and the second list has `id` 5 (the
first fight) and `id` 4 (the next snake, which gate 7 makes again).
```

New:

```
player is kept at y ≥ 432 until it passes x 944 [certain for the code]. Records 22–26 are main 25
`id` 6, 7, 9, 8, 10 (more snake fights), and the second list has `id` 5 (the first fight) and
`id` 4 (the next snake, which gate 7 makes again). Record 26 (`id` 10) is the lava room of the top
corridor (`heatnix-6_snake-room`, map item 9). On the upper route the player meets `id` 6 and 9,
on the lower route `id` 7 and 8 [likely: from the record places and the spawned flags in
`heatnix-6_snake-room`].
```

### 6. Open question 2

Old:

```
2. What sets `game+0x26` for the gates after the first one. The end of Nightmare Snake `var` 5 sets it to 1 and gate 7 opens [certain]; the other snakes probably open the other gates [likely].
```

New: delete the item (answered in "The gate flag" above) and renumber the next items.

## `docs/engine.md`

### Camera table: a new row after `+0x24`…`+0x2A`

Old:

```
| `+0x24`…`+0x2A` | `0x8009721C` | target limits, same order (`cam.target_max_x`, `cam.target_min_x`, `cam.target_max_y`, `cam.target_min_y`) |
```

New:

```
| `+0x24`…`+0x2A` | `0x8009721C` | target limits, same order (`cam.target_max_x`, `cam.target_min_x`, `cam.target_max_y`, `cam.target_min_y`) |
| `+0x2E` | `0x80097226` | the screen y where the camera keeps the player (read at `0x800291E4`). 171 in every other save state with a player; Nightmare Snake `var` 10 lowers it by 64 (107 in `heatnix-6_snake-room`, where Zero is at screen y 107) [likely] |
```

## `catalog/helpers.json` (notes of existing entries)

- `0x800EC2D0` `SnakeVisualMain`, new note: `overlay 05 visual 29 state 1: ball ends on the
  player, terrain, kill (explosion) or 96 px off screen; jets end at body state 3; lava box var 6
  y = +0x90; the box at x 2944 (var 6) or 3296 (var 7) steps the layer 2 x offset every 10
  updates (snake_lava_waves - 2848 / - 3232); var 7: player x >= 3825 -> cam+0x2E += 64, delete`
- `0x800F3F0C` `snake_lava_waves`, new note: `4 x u16 (960, 1568, 2176, 2784): layer 2 x of the 4
  lava frames (576 x 192 px strips at layer 2 y 1840); layer 2 offset x = value - 3232 (var 10
  room) or - 2848 (var 8/9)`
- `0x800EC740` `SnakeChase`, new note: `overlay 05 var 10 step 7 (sub-steps 0x800F3FB0): swim in
  the lava toward the player at up to 2 px/frame; within 48 px in x (or x >= 3729 / < 3296): stop
  50, leap up to 3 px/frame, turn over above min y + 192, hang 90 (gems may shoot), fall back below
  min y + 352`

New names are in `helpers-proposed.json`. No name or address clashes with `catalog/helpers.json`.
`../heatnix-snake-9/helpers-proposed.json` also proposes `0x800F4004` `snake_lava_box7` (the same
name; take one of the two notes) and `0x800F4000` `snake_lava_box6` (left out here).

## Work folder `heatnix-mini-boss/pseudo.c` (not in the repo)

The comment "when mega.x >= 3825: layer 0 min y + 64" is wrong: the code adds 64 to camera
`+0x2E` (the follow point), not to a limit.
