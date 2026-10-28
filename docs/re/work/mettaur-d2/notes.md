# Mettaur D2 (stage 7, overlay 10, main type 27): notes

## Method

- Kit: `python3 -m mmxdemake.rekit --name mettaur-d2` (export `enemy-46d9e2249f`, now in the folder
  `mettaur-d2`). The first kit came from `--state sheldon-6_tile-shown.state.gz --type 27`; the walk
  is the same (17 functions, 3 state tables).
- Read: `kit/summary.md`, `kit/calls.txt`, all of `kit/code.txt`, the boxes and tables in
  `kit/data.txt`. Engine rules from `docs/engine.md`.
- Live check: `probe_live.py` (this folder):
  `python3 probe_live.py 27,30 32 <states...>` prints every live main object of the given types and
  every live visual object of the given types, with all fields used here.

## Code map

| address | what |
|---|---|
| `0x800F1FD8` | entry: `state_tbl[state]` (`0x800FC64C`) |
| `0x800F1D4C` | state 0, set-up |
| `0x800F1E8C` | state 1, main |
| `0x800F23E4` | states 2 and 4: `DeleteObject` (2) / `DeleteObject2` (4) |
| `0x800F2014` | state 3: wait inside a solid tile (`var` 2 and 3 only) |
| `0x800F1884` | edge and fall check (each main update, before the step) |
| `0x800F1748` | shot trigger (each main update) |
| `0x800F1A24` | guard trigger: looks for a lemon (each main update) |
| step table `0x800FC634` | 0 `HitReactionDispatch`, 1 WALK `0x800F22FC`, 2 FALL `0x800F2150`, 3 GUARD `0x800F1C10`, 4 SHOOT `0x800F21F8`, 5 JUMP `0x800F20B4` |
| `0x800F2424` | visual type 32 (the shot): state table `0x800FC668` = init `0x800F2460`, move `0x800F2518`, delete `0x800F25E0` |

## Key facts and where they come from

- Set-up `0x800F1D4C`: HP 3 (`0x800F1DE8`), contact 1 (`0x800F1DEC`), priority 6, `iframes` 0,
  `dmg_tbl_normal`, terrain `0x800FC4F4` (0, 5, 11, 8), attack `0x800FC4F8` (−11, −16, 21, 29),
  hurt `0x800FC500` (−12, −6, 21, 20). Facing: `flip` = `0x40` when `mega.x >= self.x` (32-bit
  compare, so an equal x faces right). Animation 1, step 1 (WALK).
- `var` 2 or 3 (`var − 2 < 2` unsigned): state 3, no attack or hurt box, x = (x & ~15) + 8,
  y = (y & ~15) + 20, animation 0. State 3: when `TerrainPointCheck(x, y)` is 0, state 1, step 1,
  `var` = 0, the normal boxes. Off screen 64 → state 2 (`DeleteObject`). `var` 2 calls
  `DisplayCheck(37, 37)` while it waits; `var` 3 does not, so it stays hidden. No record of stage 7
  area 0 uses `var` 2 or 3 (all four records have `id` 1).
- Main `0x800F1E8C`: prev pos; edge check; shot trigger; guard trigger; step; `ContactMega`;
  `ContactWeapon`. Killed (< 0): `SpawnExplosion`, `SpawnJunk(4, {9, 10, 11, 12})`
  (`0x800FC630`), `DropItem(18)` unless the result is −56 (weapon id 55), state 2. Else off screen 64
  → state 4; else `DisplayCheck(37, 37)`; then the squeeze test (`collide & 0xC == 0xC` or
  `collide & 3 == 3`): explosion and debris, `display` 0, state 2, no drop; else `CollideCheck`.
- Edge check `0x800F1884` (only while `latch` = 0):
  - no floor flag (`collide & 8` = 0): animation 3, step 2 (FALL), `gravity` `0x4200`
    (0.2578 px/update²), `speed_y` 0, `accel_x` 0, `latch` −1. `speed_x` is kept.
  - on the floor: probe point = (x ± (box.ox + box.w), y + box.oy + box.h + 8) = (x ± 11, y + 21),
    + in the facing direction. When the probe is solid: nothing. Else:
    - `var` 1: turn: `flip` = (flip == 0) ? `0x40` : 0, `speed_x` = −`speed_x`;
    - other `var`: when no wall flag (`collide & 3` = 0): animation 2, step 5 (JUMP), `speed_y` =
      `0x38000` (3.5 px/update up), `gravity` `0x4200`, `speed_x` = ±`0x10000` (1 px/update, by
      facing), `latch` 1.
- Shot trigger `0x800F1748` (only while `latch` = 0, `work.80 & 3` = 0 and the cooldown `+0x83` =
  0; else the cooldown goes down by 1): `|self.y − mega.y| < 40` and, facing right,
  `mega.x < self.x + 80`, facing left, `mega.x > self.x − 80` (16-bit pixel compares). True: the
  counter `+0x81` += 1, else 0. When the counter is 61 or more: step 4 (SHOOT), animation 7,
  counter 0, `work.80 |= 2`. Note: the test has no "in front" side, so the player **behind** the
  Mettaur at any distance also counts.
- Guard trigger `0x800F1A24` (only while `latch` = 0 and `work.80 & 1` = 0; the cooldown `+0x82`
  counts down first): for each of the 32 weapon objects with flags ≠ 0 and type (weapon id) 0, the
  lemon: the player is in front of the Mettaur and faces it (Mettaur faces right: `mega.flip` = 0
  and `self.x < mega.x`; faces left: `mega.flip` ≠ 0 and `mega.x < self.x`, 32-bit compares),
  `|shot.y − self.y| < 24`, and the shot x is less than 128 px in front (facing right:
  `shot.x < self.x + 128`; left: `shot.x > self.x − 128`). Then `work.80 |= 1`, step 3 (GUARD),
  step2 0, animation 5.
- GUARD `0x800F1C10`: step2 0: animation 5 again, timer `+0x7C` = 60. step2 1: on the frame with
  flag 1 (frame 3 of animation 5): `dmg_tbl_deflect`, hurt box = attack box = `0x800FC4FC`
  (−12, −4, 21, 18); the timer goes down; at 0: animation 6, step2 2. step2 2: when the animation
  holds (`anime.next` = 0): animation 1, `dmg_tbl_normal`, the normal boxes, step 1, guard cooldown
  `+0x82` = 120, `work.80 &= ~1`. `AnimeTick` runs at the end of every GUARD update.
- WALK `0x800F22FC`: step2 0: normal table and boxes, animation 1, `speed_x` = ±`0x6000`
  (0.375 px/update) by facing, the rest 0, step2 1. Each update: `AnimeTick`, `MoveObject`; facing
  right and `collide & 1`: `flip` = 0 and `speed_x` negated; facing left and `collide & 2`: `flip` =
  `0x40` and negated.
- FALL `0x800F2150`: step2 0: normal table, `MoveObject2`; on the floor: animation 4, all speeds
  and gravity 0, step2 1. step2 1: when animation 4 holds: animation 1, step 1, `latch` 0.
- JUMP `0x800F20B4`: step2 0: waits for the flag of animation 2 (frame 2, after 16 updates), no
  move; step2 1: `MoveObject2`; when `speed_y` < 0: animation 3, step 2 (FALL), `speed_y` 0,
  `gravity` `0x4200`, `latch` −1. `speed_x` (±1) is kept for the fall.
- SHOOT `0x800F21F8`: `AnimeTick`; on flag 1 (frame 3 of animation 7, after 20 updates) the flag is
  cleared and a visual object is made: flags `0x41`, type 32, the parent's x, y, sprite data,
  texture, palette, layer, animation table and `flip`; `PlaySound(2, 46, self)`. When the
  animation holds (frame 7, after 36 updates): step 1, animation 1, shot cooldown `+0x83` = 120,
  `work.80 &= ~2`. No move during SHOOT (no `MoveObject`; `speed_x` is kept for the walk).
- Shot init `0x800F2460`: `display` 1, `speed_x` = ±`0x28000` (2.5 px/update, by `flip`), x ± 16
  (in front), y + 3, priority 6, terrain `0x800FC664` (0, 0, 9, 7), `dmg_tbl_one_hit`, hurt =
  attack = `0x800FC660` (−5, −4, 9, 7), contact 2, animation 8. The HP is not set (0).
- Shot move `0x800F2518`: `AnimeTick`, `MoveObject`; `ContactMega` ≠ 0 → state 2; `ContactWeapon`
  < 0 → state 2 (no explosion); `+0x70` ≠ 0 or `CornerCheck` sets it → `SpawnHitEffect`, state 2;
  off screen 32 → state 2; else `DisplayCheck(16, 16)`. State 2: `DeleteObject3`.

## Timing (hand simulation, ±1 update)

- SetAnime loads frame 0; each `AnimeTick` takes 1 from the duration; frame k loads after the sum of
  the durations of frames 0 … k−1.
- SHOOT: trigger at update 0 (the trigger and the first SHOOT update are the same update). Shot at
  about update 20. End at update 36.
- GUARD: trigger at update 0. Deflect on from about update 9 (6 + 1 + 2), raise (animation 6) at
  update 60, normal again at update 80 (60 + 20). So the guard lasts 80 updates, deflecting for
  about 70.
- Cooldowns: guard 120 updates after a guard; shot 120 updates after a shot, then 61 updates in the
  zone. Shortest time from one shot to the next: 36 + 120 + 61 ≈ 217 updates (3.6 s), when no
  guard comes between.
- Jump (`var` 0 only): 16 updates crouch, then up 3.5 px/update with gravity 0.2578: 14 updates up,
  25.5 px high, 14 px forward; then a fall at 1 px/update forward (maximum fall 6.5 px/update).

## A possible lock (code fact, not checked in play)

The guard trigger does not test the SHOOT bit (`work.80 & 2`). A lemon during SHOOT starts GUARD,
and the SHOOT end (which clears bit 2 and sets the shot cooldown) never runs. GUARD clears only
bit 0. With bit 2 set, the shot trigger returns at once. So after a guard that interrupts a shot,
this Mettaur never shoots again until it is deleted and spawned again [certain for the code; the
effect in play is likely].

## Records and the hidden floors

- Records 88–91 (list entry 7, area 0): type 27, `id` 1, at (1656, 432), (1824, 432), (2080, 432),
  (2256, 432); kind 0 (in RAM, record 88 had kind `0x20` in `sheldon-8`).
- They stand on the hidden glass floors 73, 75, 76 and 78 (`docs/stages/sheldon.md`): each floor is
  64 px (4 tiles) wide, y 448–463, collision value 60, solid also while not shown. With `id` 1 the
  Mettaur turns at the floor ends, so it paces on one hidden floor.
- Example, floor 75 (x 1792–1855): facing left it turns when x − 11 < 1792 (x < 1803); facing right
  when x + 11 ≥ 1856 (x ≥ 1845). So it walks x 1803 … 1844 [likely: `TerrainPointCheck` reads value
  60 as solid; X stands on value 60 in `sheldon-6`].

## Checked against the save states

- `sheldon-6_tile-shown`, main slot 4: type 27 `var` 1, state 1, step 1 (WALK), step2 1, x 1804.125,
  y 435, `flip` 0 (left), `speed_x` −0.375, accel and gravity 0, HP 3, contact 1, `iframes` 0,
  `latch` 0, `collide_flags` 8 (floor), priority 6, animation table `0x800FC5F8`, boxes `0x800FC4F8`
  / `0x800FC500`, table `0x800743DC`, terrain `0x800FC4F4`, all work bytes 0. Record 89 (1824, 432).
  - y 435 = floor 448 − 13 (terrain centre +5, half height 8): it fell 3 px to the floor after the
    spawn.
  - 1824 − 1804.125 = 19.875 = 53 × 0.375: 53 walk updates after the land.
  - It faces left: the player (x 1661) is to its left at spawn.
  - Shot counter 0: `self.x − 80` = 1724 is not less than the player's x 1661, so the player is not
    in the zone (also |dy| = 8 < 40).
- No other state has a live Mettaur D2 (`sheldon-1` … `sheldon-11`). No shot (visual type 32) is live.

## Open questions

1. Which weapon id 55 is (no drop when it kills).
2. Whether Zero's attacks can start the guard: the trigger tests weapon id 0 only (the X-Buster
   lemon).
3. Not checked in play: the guard timing, the shot lock above, the shot when the player is behind,
   the pace on the hidden floors.
4. `var` 0 (jump off ledges) and `var` 2, 3 (wait in a solid tile, then come out) are not used in
   stage 7 area 0. Other stages were not searched.
5. Animation 13 (sprite 37) is not started by this code.
