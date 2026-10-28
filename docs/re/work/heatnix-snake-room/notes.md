# Nightmare Snake `var` 10 (Heatnix top corridor, the lava room): working notes

State: `~/mmx-data/x6/dumps/heatnix-6_snake-room.state.gz`. The user: "fought the donut in again
another room". Stage 3, area 0, checkpoint 6, Zero (`mega` `var` 1) at (3673, 1162), HP 2, step 2.
The snake is dead; only its four lava kill boxes are left.

## Method

- Read `CLAUDE.md`, `README.md`, `docs/enemies/nightmare-snake.md`, `docs/stages/heatnix.md`,
  the first work folder `../heatnix-mini-boss/` (notes, `pseudo.c`, kit).
- `scripts/dump_state.py` (copy of the first folder's script): type 25 and visual 29 objects,
  stage objects, `game`, `mega`, the three layer cameras.
- Kit: `python3 -m mmxdemake.rekit --state heatnix-6_snake-room.state.gz --type 25 --name
  heatnix-snake-room --out .../heatnix-snake-room/kit` (60 functions, the same code as the first
  kit; the catalog names are already applied). `--at 0x8002820C`, `--at 0x800291A0`,
  `--at 0x800294F0` for the camera code.
- `scripts/find_field_refs.py <state> <lo16 hex>`: every load and store in RAM with that 16-bit
  offset (used for the layer 0 camera `+0x2E`).
- Record flags read from RAM (list `0x800F3280`, 8 bytes per record; fixed list `0x800F36D8`).
- Collision and art: crops of `stages/m097-blaze-heatnix-section-1/collision.png` and of
  `stages/m097-blaze-heatnix-second-texture/stage.png` and its ORA layer `art 2 background`
  (scratch only).
- The preview picture of the state (offset `0x25`) is black with a "0": it shows nothing.

## Which fight: `var` 10 [certain]

- Visual 29 `var` 7 is made only by `SnakeInit` case 10 (`0x800EAF9C`–`0x800EB044`): four objects
  at x = 3296 + 160 × i (i = 0–3), y 1304. The state has exactly these four: (3296, 1304),
  (3456, 1304), (3616, 1304), (3776, 1304), state 1 step 2.
- Record 26 (`a` = `0x01` "spawned", `b` 25, `id` 10, kind `0x10`, (3378, 1024)). In
  `heatnix-2_mini-boss` it is `0x00`, kind `0x00`. The body ends with `DeleteObject3`, which keeps
  the record "spawned".
- Layer 2: mode 2, offsets (−448, 608), as case 10 writes (mode 2, y offset 608; x offset from the
  lava cycle, below). Layer 1 is hidden (`+0x03` = 0, `SnakeEnd`), at (−3060, −580): the body wrote
  (512 − x, 512 − y) each update, so its last point was **(3572, 1092)** [certain for the
  arithmetic]: in the air above the platforms, not in the lava [likely: it died during a leap].
- Other records: 22 (`id` 6, upper route, x 1664) and 24 (`id` 9, upper route, (3021, 2224)) are
  also "spawned"; 23 (`id` 7) and 25 (`id` 8, lower route) are not. So the player took the upper
  route: snakes 6 and 9 before this one [likely]. Stage objects: gates 11, 12, 13 (2864, 2224) and
  14 are in state 2, gate 9 (lower) in state 1.

## Visual 29 `var` 7: the lava kill boxes [certain for the code; the state agrees]

Set-up (`SnakeVisualInit`, `0x800EBF1C`): contact damage **127**, attack box `0x800F4004` =
(−80, −64, 160, 128), no hurt box, no terrain box, no damage table, step 2, priority 4, the body's
graphics (never drawn: `display` stays 0). The state: contact 127, attack box `0x800F4004`,
hurt/terrain/damage 0, `display` 0.

Each box covers x − 80 … x + 80, y 1240 … 1368. The four boxes together: **x 3216–3856,
y 1240–1368**, one band with no gap (the room is x 3232–3808 between the walls). Touching it is
a hit of 127: death at once.

Each update (`SnakeVisualMain`, `0x800EC3D0`–`0x800EC518`, for `var` ≠ 6):

1. **Only the box at x 3296** (slot 0): when `frame_count` % 10 = 0, `+0x92` = (`+0x92` + 1) & 3,
   layer 2 offset x (`0x800972E0`) = `snake_lava_waves[+0x92]` − 3232. Every update it sets the
   layer 2 redraw request (`0x800972EC` = 1). Values: −2272, −1664, −1056, −448. The state: slot 0
   `+0x92` = 3, offset −448; the other boxes `+0x92` = 0.
2. When `mega.x` ≥ 3825: layer 0 camera `+0x2E` += 64, state 2 (deleted). Else `ContactMega`.

The box does not move (`var` 7 has no `+0x90` y copy; that is `var` 6).

**The lava picture.** The layer 2 map (ORA layer `art 2 background` of the second-texture drawing)
has four lava strips, each 576 × 192 px, at layer 2 x 960, 1568, 2176, 2784 (the values of
`snake_lava_waves`), y 1840–2032 [certain for the strip places, by eye on the crop]. With layer 2
mode 2 (layer 0 camera + offsets) and offset x = table − 3232, each strip lands at world x
3232–3808, y 1232–1424 (offset y 608): exactly the room between the two walls, from 8 px above the
top of the kill boxes down to the bottom of the view at camera max y 1184. So the four strips are
four animation frames of the lava, 10 updates each, a 40-update loop [likely: the colours in the
drawing are purple, from another state's palette; not checked].

**The camera follow point** (layer 0 camera `+0x2E`, `0x80097226`). The code at `0x800291E4`
(layer 0 camera, near `CameraLimitEase`) takes the player y − camera y − `+0x2E`. In every save
state where the camera y is between its limits, the player's screen y is 170–171 and `+0x2E` = 171
(heatnix-4, mijinion-3, scaravich-11/13, turtloid-3 …). Here `+0x2E` = **107** and Zero's
screen y = 1162 − 1055 = **107**. So `+0x2E` is the screen y where the camera keeps the player
[likely]. `SnakeInit` case 10 lowers it by 64 (`0x800EAFC0`), so the view shows 64 px more below
the player (the lava). The restore (+64 per box when `mega.x` ≥ 3825) runs four times (one per box,
+256 in all), but only when the player is past x 3824; gate 15 is at x 3824 and its trigger calls
`ClearEnemies`, which deletes every visual object first. So the restore code does not run in
normal play [likely], and `+0x2E` stays 107 until something else writes it (`0x80029518` writes
170 or 171 by `mega+0x02`; when it runs is not known). In the boss corridor after gate 15 the
camera y is fixed (944), so the value has no effect there [guess].

The first work folder's `pseudo.c` says "layer 0 min y + 64" for this code: that is wrong; it is
`+0x2E`, not a limit (`+0x2A`).

## The `var` 10 fight, from the code (`SnakeInit` case 10, `SnakeChase` `0x800EC740`, sub-steps `0x800F3FB0`)

Set-up (state 0) [certain for the code]:

- Once (`+0x90` = 1 after it): camera `+0x2E` −= 64; the four kill boxes; layer 2 mode 2, visible,
  offsets (−2272, 608), redraw; body at x 3616, y = camera target min y + 352 = **1296** (min y
  944 in this room).
- Then it waits each update until `mega.x` ≥ 3344; then `PlayerEvent(20, 64)` and the common
  body set-up (four gems, four jets, `game+0x26` = 0, layer 1 shown). `var` 10 gets **no terrain
  box** (only `var` < 8 do), step 9 (INTRO, as `var` 5; at the end step 7). `+0x8D` stays 0.
- Record 26 is at x 3378, inside the view as soon as gate 14 sets camera x 3200–3504, so the lava
  and the boxes are there from the room entry; the fight starts at x 3344 [likely].

SnakeChase sub-steps [certain for the code; the speeds use the `MoveObject2` sign checked for
`var` 5]:

| sub | function | what |
|---|---|---|
| 0 | `0x800ED190` | player right of the body: `speed_x` +1, `accel_x` −1/16, sub 1; else `speed_x` −1, `accel_x` +1/16, sub 3. `speed_y`, gravity 0 |
| 1 | `0x800ED1F8` | (wait `+0x7C` first) roll right, speed capped at 2 px/frame. When \|x − player x\| < 48 or x ≥ 3729: stop (`speed_x` 0), `speed_y` +1 (up), gravity −1/16, wait **50**, sub 2 |
| 3 | `0x800ED370` | the same to the left, cap −2, edge x < 3296 |
| 2 | `0x800ED2B8` | after the 50-update wait: rise, speed up 1/16 per update to **3 px/frame**; above y = min y + 192 (1136) the gravity turns over (slows down). When `speed_y` < 0: `speed_y` −1, gravity +1/16, wait **90**, sub 4 |
| 4 | `0x800ED424` | while the 90-update wait runs: no move, `+0x8D` = 1 (**the gems may shoot only now**); then `+0x8D` = 0, fall, speed to 2 px/frame; when y > min y + 352 (1296): sub 0 |

So: the ring swims in the lava (centre y 1296: its attack box top at 1229, the lava surface at
1232, the platform tops at 1184) toward the player, stops under the player, waits 50 updates,
leaps up through the platforms (no terrain box), hangs 90 updates at the top, falls back, and
repeats. Computed from the code: the rise takes about 32 updates from 1 to 3 px/frame (64 px),
32 at 3 px/frame (96 px, to 1136), 48 slowing down (72 px): the top is about **y 1064**; the fall
from 1064 to 1296 takes about 120 updates. One cycle is the roll + 50 + about 112 + 90 + 120
updates [likely: computed, not timed]. The death point (3572, 1092) is near that top [likely].

Gem shots need `frame_count` % 120 = 0 while `+0x8D` = 1; the hang is 90 updates, so **at most
one volley per leap** (none in about 1 of 4 leaps) [certain for the arithmetic].

Death (step 8) and end: the `var` 8/9 parts of `SnakeDeath` (`0x800EC7BC`, `0x800ED4BC`,
`0x800ED5F4`) do not run for `var` 10; it ends as `var` 5: state 3, `SnakeEnd`: layer 1 hidden,
**`game+0x26` = 1**, `DeleteObject3` (no wait: the stay-until-x-944 rule is `var` 5 only). The
kill boxes and the layer 2 lava are not removed by the death [certain for the code; the state
agrees: four boxes live, layer 2 mode 2].

No `CopyTiles` for `var` 10 (only `var` 4 in `SnakeIntroStart`), no checkpoint write anywhere in
the type 25 or visual 29 code (`game.checkpoint` is only read) [certain for the code].

## The state against the sheet and the stage page

| item | sheet / page | state | mark |
|---|---|---|---|
| record 26 = `var` 10 | row `var` 10, record 26 (3378, 1024) | record 26 spawned; the four `var` 7 boxes at the code places | certain |
| start x 3616 | "x 3616" | (the body is gone) | not shown |
| four kill boxes, `var` 7 | (−80, −64, 160, 128), "not in a state" | attack box `0x800F4004`, contact 127, step 2, not drawn | certain |
| layer 2 (lava) | "over lava (layer 2)" | mode 2, offsets (−448, 608): the 4th lava frame | certain |
| camera limits | checkpoint 6: x 3200–3504, y 944–1184; gate 14: x 3200–3504, max y 1184 | current and target limits x 3200–3504, y 944–1184 | certain |
| `game+0x26` | page: "probably set by each mini-boss fight [guess]" | 1; gate 15 (stage slot 13, `id` 15, (3824, 1024)) is in state 1 step 0 (WAIT), so it opens when the player walks into it | certain |
| checkpoint 6 | marker record 5 at (3216, 1024) | `game.checkpoint` = 6; the marker is gone (no logic object 6 `id` 6); logic slot 0 is marker `id` 7 at (3840, 1024) | certain; set by the marker, not by the snake |
| HP bar | – | `game+0x24` = 0, `game.boss` = 0 | certain |
| map cells | – | no tile change in this fight (code) | certain for the code; the RAM map was not compared |

Collision of the room (from `collision.png` of section 1; the same layout) [certain for the
collision]: ceiling y 960–992; left wall x 3168–3232 from y 1056 down, gate 14 cells at
x 3184–3216, y 992–1056; right wall x 3808–3840 from y 1056 down, gate 15 cells at x 3808–3840,
y 992–1056; three 64 × 32 platforms at x 3328–3392, 3488–3552, 3648–3712, y 1184–1216; floor
y 1376 (under the lava, x 3232–3808). Zero at (3673, 1162) stands on the third platform.

Other things the state shows:

- The five grey blocks `id` 3 (stage slots 0, 2, 3, 4, 6, in the first tower) are still live at
  checkpoint 6. The page says they "go away at checkpoint 3"; the block code does nothing while
  it is off screen (`docs/enemies/grey-block.md`, "Entry"), so they are deleted only when seen
  again [certain for the state].
- Boss gate `id` 7 (stage slot 1, (944, 608)) is still live in state 1.
- `stage_texture_swap` = 2 (the swap was done) [certain for the value; meaning of 2: not checked].

## Sprites

`spriteexport --state heatnix-6_snake-room.state.gz --root .../heatnix-snake-room/sprites-root`:
9 exports (3 live, 6 search), 2 skipped (banks 8–13 and 14/15 without texture, as before). All 9
IDs are the same as in `~/mmx-data/x6/sprites/enemies/` (boss-gate, explosion, flame-jet,
grey-block, nightmare-bug, nightmare-snake, nightmare-virus, nightmare-virus-orb,
teleport-crystal): **nothing new**. Banks 0–7 and `stage-8-live-0` are equal pixel for pixel to
the shared `heatnix-2_mini-boss` banks. Checked by eye: bank 6 (clean), `effect-4-live-0`
(explosion), `stage-12-live-0` (grey block). No wrong sheets. Visual 29 `var` 7 has no sprite (never
drawn); the lava is BG layer 2 art, not a sprite.

## What the state does not show

- The fight itself: no body, gem, jet or ball is live. The roll, the leap height and the timing
  are from the code only.
- Whether the lava (layer 2) is drawn in front of the ring (layer 1) while the ring swims.
- The lava colours (the preview is black; the ORA colours come from another state).
- Whether the `+0x2E` restore code ever runs, and what resets `+0x2E` after this room.

## Files

- `notes.md` (this file), `doc-changes.md`, `helpers-proposed.json`.
- `scripts/dump_state.py`, `scripts/find_field_refs.py`.
- `kit/` (rekit of type 25 from this state), `sprites-root/` (the export above).
