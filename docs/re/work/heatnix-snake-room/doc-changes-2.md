# Doc changes from `heatnix-6_snake-room.state.gz`, round 2

Written against the current files, after the `var` 8/9 changes. Only the parts that are still to
do. Each Old block occurs exactly once in its file. The `var` 8/9 facts are kept.

## `docs/enemies/nightmare-snake.md`

### 1. Save states (intro paragraph)

Old:
```
rising room, step 6 sub 4: the ring at (3024, 2380) falls back into the flame; Zero at (2901, 2234);
checkpoint 5). Written with the RE kit (`python3 -m mmxdemake.rekit
```

New:
```
rising room, step 6 sub 4: the ring at (3024, 2380) falls back into the flame; Zero at (2901, 2234);
checkpoint 5). `heatnix-6_snake-room.state.gz` is just after the `var` 10 fight (the four kill
boxes are live; see CHASE). Written with the RE kit (`python3 -m mmxdemake.rekit
```

### 2. The `var` table, row 10, and the line under the table

Old:
```
| 10 | record 26, `id` 10 (3378, 1024) | x 3616 | step 7 | over lava (layer 2), jumps when the player is within 48 px in x (four kill boxes, visual 29 `var` 7) [likely] |

`var` 9 is checked in `heatnix-5_snake-rising`. `var` 4, 6, 7, 8 and 10 are not in a save state
yet; their rows come from the code only.
```

New:
```
| 10 | record 26, `id` 10 (3378, 1024) | (3616, 1296); the lava and the four kill boxes at once, the intro when the player x ≥ 3344 | step 7 | the lava room of the top corridor: swims in the lava toward the player, leaps up through the platforms when the player is within 48 px in x (see CHASE) [certain for the code; the end of the fight in `heatnix-6_snake-room`] |

`var` 9 is checked in `heatnix-5_snake-rising`, the end of `var` 10 in `heatnix-6_snake-room`.
`var` 4, 6, 7 and 8 are not in a save state yet; their rows come from the code only.
```

### 5. Kill boxes

Old:
```
**Kill boxes** (`var` 6 for `var` 8/9, `var` 7 for `var` 10): contact **127**, attack box `var` 6
(−80, −56, 160, 112), `var` 7 (−80, −64, 160, 128); never drawn. Step 2: y = `+0x90` (the body
writes it); the box at x 2944 (`var` 6) or 3296 (`var` 7) also animates the BG layer 2 flame (ox =
`snake_lava_waves[n]` − 2848 or − 3232, n + 1 every 10 updates). `var` 7 ends when the player x ≥
3825 (layer 0 `+0x2E` + 64) [certain for the code; `var` 6 checked in `heatnix-5_snake-rising`].
```

New:
```
**Kill boxes** (`var` 6 for `var` 8/9, `var` 7 for `var` 10): contact **127**, attack box `var` 6
(−80, −56, 160, 112), `var` 7 (−80, −64, 160, 128); no hurt box, no terrain box; never drawn.
Step 2: `var` 6 only: y = `+0x90` (the body writes it); `var` 7 does not move. The box at x 2944
(`var` 6) or 3296 (`var` 7) also animates the BG layer 2 flame (ox = `snake_lava_waves[n]` − 2848
or − 3232, n + 1 every 10 updates, with a layer 2 redraw each update). `var` 7 ends when the player
x ≥ 3825 (layer 0 `+0x2E` + 64 per box, then deleted) [certain for the code; `var` 6 checked in
`heatnix-5_snake-rising`, `var` 7 in `heatnix-6_snake-room`].

`var` 7 in `heatnix-6_snake-room` [certain]: four boxes at (3296, 1304), (3456, 1304),
(3616, 1304), (3776, 1304), state 1 step 2, contact 127, box `0x800F4004`; the box at x 3296 has
n = 3 and layer 2 ox = −448. Together they cover x 3216–3856, y 1240–1368, one band over the
whole room. The layer 2 map holds the four flame strips (576 × 192 px) at x 960, 1568, 2176, 2784,
y 1840–2032; with ox = table − 3232 and oy 608 (layer 2 mode 2) each lands on world x 3232–3808,
y 1232–1424: 4 frames of 10 updates. Gate 15 (x 3824) clears all visual objects at its trigger
before the player can reach x 3825, so the `+0x2E` restore does not run in normal play [likely].
```

### 6. Open question 1

Old:
```
1. The other snakes (`var` 4, 6, 7, 8, 10): the rows above come from the code only; save states of
   these fights would check them. `var` 9 is checked. Open for `var` 8/9: the sounds (5, 1) and
   (5, 3), and the game's draw order and blend of the BG layer 2 flame.
```

New:
```
1. The other snakes (`var` 4, 6, 7, 8): the rows above come from the code only; save states of
   these fights would check them. `var` 9 is checked, and the end of `var` 10. Open for `var` 8/9:
   the sounds (5, 1) and (5, 3), and the game's draw order and blend of the BG layer 2 flame. Open
   for `var` 10: the leap height and timing in play (a state during the fight), and whether the
   flame (layer 2) is drawn in front of the ring (layer 1).
```

## `docs/stages/heatnix.md`

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
   platforms at x 3328–3392, 3488–3552, 3648–3712 with top y 1184, the BG layer 2 flame from
   y 1232, four kill boxes; floor y 1376 under the flame) with the Nightmare Snake `var` 10 fight
   (`docs/enemies/nightmare-snake.md`, CHASE; `heatnix-6_snake-room`), gate `id` 15 (3824, 1024;
   camera x 3808, y 944),
```

### 5. Nightmare Snake rooms

Old:
```
`id` 6, 7, 9, 8, 10 (more snake fights; `id` 9 is in `heatnix-5_snake-rising`, the others have no
save state yet), and the second list has `id` 5 (the first fight) and `id` 4 (the next snake,
which gate 7 makes again). The rising room (`id` 8 or 9): see the map, item 8.
```

New:
```
`id` 6, 7, 9, 8, 10 (more snake fights; `id` 9 is in `heatnix-5_snake-rising`, the end of `id` 10
in `heatnix-6_snake-room`, the others have no save state yet), and the second list has `id` 5 (the
first fight) and `id` 4 (the next snake, which gate 7 makes again). The rising room (`id` 8 or 9):
see the map, item 8. The lava room (`id` 10): the map, item 9. On the upper route the player meets
`id` 6 and 9, on the lower route `id` 7 and 8 [likely: from the record places; in
`heatnix-6_snake-room` records 22 and 24 are "spawned", 23 and 25 are not].
```

### 6. Open question 2

Old:
```
2. What sets `game+0x26` for the gates after the first one. The end of Nightmare Snake `var` 5 sets it to 1 and gate 7 opens [certain]; the other snakes probably open the other gates [likely].
3. The destination of the teleporter crystal (`id` 1, area change).
4. Item kind 2 (record 110).
5. Why banks 8–13 are type 0 with no texture.
```

New (item 2 is answered by the gate flag paragraph under the map; the next items are renumbered):
```
2. The destination of the teleporter crystal (`id` 1, area change).
3. Item kind 2 (record 110).
4. Why banks 8–13 are type 0 with no texture.
```
