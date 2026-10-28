# Proposed doc changes (from `scaravich-12_boss-fight`)

Evidence: [`~/mmx-data/x6/re/scaravich-boss/notes.md`](notes.md). Nothing here is applied.

## `docs/stages/scaravich.md`

### "Save states and overlay", table: add a row after `scaravich-11_boss`

```
| `scaravich-12_boss-fight` | 1890, 1256 | 28 | 1664, 1155 | 4, 3, 2 | the boss fight: Zero hangs on the right upper bar (step 28). The boss is in step 5 (pellet throw), HP 17, with a size-3 ball (visual 14 `var` 3, HP 48) |
```

### "The boss corridor and the boss room": replace the first bullet

Old:

```
- The room walls are at x 1568–1599 and 1984–2015, with four small platforms at (1616–1663, 1280),
  (1664–1711, 1216), (1872–1919, 1216) and (1920–1967, 1280) [likely].
```

New:

```
- The room walls are at x 1568–1599 and 1984–2015 [certain, `scaravich-10`]. The four small
  "platforms" are **flat bars** (collision 45; the player hangs on them, step 28): (1616–1663,
  1280), (1664–1711, 1216), (1872–1919, 1216) and (1920–1967, 1280) [certain: the collision map
  and Zero on the bar in `scaravich-12`].
- The boss set-up removes both walls: its two `CopyTiles` copy the empty strip (1600, 1136, 2 × 15
  cells) over x 1568–1599 and 1984–2015, y 1136–1375. The wall (56) and the gate 20 door cells (59)
  become 0; the copy also puts one bar cell (45) at (1584, 1280) and (2000, 1280). All these cells are
  outside the view (limits x 1600–1664) [certain: 60 cells differ between `scaravich-10` and
  `scaravich-11`, none between `scaravich-11` and `scaravich-12`]. Why: the boss and the ball leave
  the room through the walls [guess].
```

### "The boss (main 50)": replace the bullet "Set-up" text "[the effect was not checked]" and "[guess: the ball]"

```
  a strip of 32 × 240 px that removes the side walls (see "The boss room"). ... It makes visual 14
  `var` 1 with the boss as parent (`+0x7C`): the ball, size 1 [certain].
```

### "The boss (main 50)": replace the "Entry" sub-steps 1–3

```
  1. When exit request = 0 and player step = 20: `PlayerEvent(29, right)` at once, timer 100 (stage
     12: 10). Camera: `cam+0x48` saved in `+0x86`, then 1; `cam+0x47` = 2; target min x = max x =
     1632. It makes `BossWarning`.
  2. At the end of the timer `PlayerEvent(20, right)`. When the warning is gone: `game+0x24` = 1,
     `game+0x20` = the boss, boss HP max (`game+0x8C`) = 40, `BossLevel`, `speed_x` = −3 px/frame.
  3. `0x800F09B4`: walk in at 3 px/frame with the rolling sounds (2, 52), (2, 34). Below x 1888 (not
     on sprites 4, 5): `TextOpen(0, 255, game+0x2E)`, then `game+0x2E` = 1. So the text shows only
     the first time; after a retry `TextOpen` waits 60 updates with no text. Stage 12: no text.
     Animation 0.
```

and in step 6 add: "`cam+0x48` is restored from `+0x86`."

### "The boss (main 50)": add after "Death"

````
- **Death end:** after `BossExplosion` the boss clears `game+0x24` and `game+0x20`. Stage 5: a
  Nightmare Virus soul `var` `0x10` when the fight took under 5401 updates (timer `0x800F6AF4`, which
  does not count step 9), else `0x11`. Stage 12: exit request `0x80`, `game[0x2C + checkpoint]` = 1,
  checkpoint += 8.

### The fight [certain for the code]

The boss pushes a ball (visual 14) of 4 sizes. Each time it leaves the room it gets a new ball, and
the ball grows with the count `+0x87` of new balls and run-offs: Easy/Normal 0–3 → size 1, 4–6 →
2, 7–8 → 3, 9 and more → 4; Hard 0–2, 3–4, 5, 6+. The first time the HP is below 20, a size-3 ball
count goes back to 0 and a size-4 count to 4 (Hard 3).

| step | what |
|---|---|
| 2 | walk out with the ball (3.0, 2.5, 2.0, 1.5 px/frame for sizes 1–4) until x 2176 or 1424 (off screen) |
| 3 | delete the ball; after size × 10 frames a new ball; then step 4 (HP ≥ 20, or 9 in 16) or step 11 |
| 4 | walk in to x 1728 (from the left) or 1856 (from the right); then 5 in 16 step 5, 5 in 16 step 6 (90 frames), 6 in 16 walk on (step 2) |
| 5 | wait 40, then kick a pellet (visual 14 `var` 0, 4 damage): boss level 0–1 straight at 3 px/frame, 2–3 aimed; 1, 1, 2, 3 pellets for boss level 0–3, 50 frames apart; then step 2 |
| 6 | stand (animation 0), then step 2 |
| 7 | the ball is destroyed: run off at 5 px/frame (animation 4), count + 1, step 3 |
| 8 | a hit in step 2 or 4 (once per ball): run on for a time (32–128 px / speed), turn, wait 40 |
| 10 | stun (weapon ids 1, 10, 54 in steps 2, 4, 6): hop (4.0 up, gravity 0.125), 180 frames of flashing and no contact damage, then step 8 |
| 11 | HP < 20 only: walk in and kick the ball (animation 3) boss level + 1 times. The kick picks one of three rolls: at the player at 3 px/frame, a lob that lands and rolls, or bounces. After each kick a new ball falls from the top in front of the boss (random size) and is kicked again; then step 7 |

- Hit flash: 30 frames of `dmg_tbl_deflect` after a hit, 2 frames after sabre ids 24 and 25.
  Damage table `0x80074C9C`: weapon ids 1 and 54 5, id 10 8, sabre ids 24 and 25 1, other sabre ids 2.
- The ball (visual 14, entry `0x800F32E8`): sizes 1–4 have HP 8, 16, 48, 127, contact damage 4, 6,
  8, 16 and boxes of 32, 48, 80, 112 px; table `0x800F6288`. It rolls only while the boss walks.
  A destroyed ball breaks (2 s of explosions, debris). Texture boss + `0x100`, sprites of bank 15
  (the second type-50 bank), animations `0x800F6234` (21). On Hard and up: a purple ball that
  deflects everything (animations 11–14), and a small gold ball that deflects shots and drops a ball
  on the player when hit (animation 10, `var` 6).
- Sprites of the ball: `~/mmx-data/x6/re/scaravich-boss/sprites-root/enemies/scaravich-ball/`
  (probe script; `spriteexport` does not export it).
````

### "Open questions": replace item 4

```
4. The boss: weapon id 54 (a stun id); step 10 sub-steps 2–4 (no writer found); stage logic 27.
```

## `docs/engine.md`

### "The damage tables" or weapon id table, row "1, 10, 54"

Old: `| 1, 10, 54 | the only ids that kill Nightmare Insect for good, and the only ids that hurt the
Nightmare Bug (4 damage) [...] |`

Add at the end of the cell: `; the stun ids of Ground Scaravich (5, 8, 5 damage; `docs/stages/scaravich.md`)`

### "Game state", `+0x0F` row

The text says for a boss death in stage 12 "checkpoint += 9". Ground Scaravich's death code adds
**8** (`0x800F0774`, overlay 08). Write "checkpoint += 9 (Rainy Turtloid, High Max) or 8 (Ground
Scaravich)" after a check of the other two.

## `docs/TODO.md`

### The `spriteexport` overwrite item (types 31, 37, stage 12)

Add: "Cause [likely, `scaravich-12_boss-fight`]: the Scaravich ball (visual 14) draws with texture
`0x240` (boss + `0x100`) and covers banks 7, 8 and 9; the overwrite check does not include live
visual objects whose sprite data is a main-type bank. Fix: add those objects to the takers of
`overwritten`."

### Add under the sprite export items

```
- [ ] Tool: export the Ground Scaravich ball (visual 14, bank 15 sprite data, texture boss +
  `0x100`, table `0x800F6234`, 21 animations). A probe export is in
  `~/mmx-data/x6/re/scaravich-boss/sprites-root/enemies/scaravich-ball/`.
```

## `docs/players/map.md` (or `air.md`, "Hang")

Add: "Step 28 is live in `scaravich-12_boss-fight` (Zero on a flat bar of the boss room): sub-step
1, animation 163, `speed_x` 2.0 [certain]."

## `catalog/helpers.json`

See `helpers-proposed.json`. Also rename the wrong data names that the kit shows: `0x800F5DE8`
(`rain_zones8+0x50`) and `0x800F5DEC` (`rain_zones8+0x54`) are the boss attack and hurt boxes, and
`0x80074C9C` (`dmg_tbl_yammark+0xa0`) is its own table.
