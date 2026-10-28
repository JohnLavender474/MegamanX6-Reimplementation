# Caterpirade (stage 03, stage object type 1; placed as main type 7): behaviour sheet

Caterpirade is a slow armoured roller in Commander Yammark's stage. It crawls along the floor at
0.5 px/frame and turns round at walls and at ledges. Its back is a solid block: the player can stand
on it and ride it. When a weapon hurts it, it curls into a ball, waits, then rolls fast towards the
player. Weapons bounce off the ball. After the roll it opens again and crawls towards the player.

Overlay `03` (Commander Yammark), save state `yammark-3_caterpirade.state.gz` (stage 1, area 0; a
live Caterpirade in stage-object slot 0 at (467, 374), with the player standing on it). Written with
the RE kit (`python3 -m mmxdemake.rekit --name caterpirade --state yammark-3_caterpirade.state.gz
--type 1 --class stage`, and `--name caterpirade-main-7 --type 7` for the spawner) and
`docs/re-guide.md`. Units, confidence tags and the shared engine rules: `docs/engine.md`. Records:
`docs/stages/yammark.md`.

## Two objects: the spawner (main type 7) and the Caterpirade (stage type 1) [certain]

The level records place **main type 7** (kind 0, 10 records). Main type 7 (`0x800EC25C`) is only a
spawner. On its first update it:

1. gets a free stage-object slot (`0x8002C4C4`: the first slot of the 80 × `0xA4` array at
   `0x800C1220` with byte 0 = 0; it clears all `0xA4` bytes; 0 when all slots are used);
2. sets the new object's type to **1** and copies from itself: byte 0 (flags), `var` (the record
   `id`), `x.px`, `y.px`, the record pointer (`+0x10`), `tex_cord`, `clut_cord` and `sprite_data`;
3. deletes itself with `DeleteObject3`. The record keeps its "spawned" mark, so the stage object now
   owns the record.

When no stage slot is free, it calls `DeleteObject2` instead: the record is released and the spawner
comes back when the record enters the spawn band again.

So no kind-4 record has type 1: all Caterpirades come from main type 7. The live object confirms it
(its record pointer `0x800F6540` is record 59, main type 7, at (544, 380)).

## Spawn and set-up (state 0, `0x800EC368`) [certain]

- **HP 12**, contact damage **4**, `iframes` = 0, draw priority 7, layer = `mega.layer`.
- Gravity 0.125 px/frame² (`0x2000`); speeds and `accel_x` 0; `flip` 0 (left).
- Boxes, normal form (walk, turn, fall, uncurl):
  - terrain box `0x800F73EC` = (0, 0, 14, 14): a 28 × 28 px body; this is also the **platform**;
  - contact box `0x800F73F4` = (−18, −7, 36, 20): 36 × 20 px;
  - hurt box `0x800F73FC` = (−12, −8, 24, 18): 24 × 18 px;
  - damage table `dmg_tbl_normal`.
- Boxes, ball form (curl and roll):
  - terrain box `0x800F73F0` = (0, 0, 14, 14) (the same size);
  - contact box `0x800F73F8` = (−10, −8, 20, 16): 20 × 16 px;
  - hurt box `0x800F7400` = (−8, −8, 16, 18): 16 × 18 px;
  - damage table `dmg_tbl_deflect` (every weapon bounces off).
- It starts in the FALL step with animation 0, so it drops onto the floor first.

## The `id` parameter [certain]

The spawner copies the record `id` to `var`, but the Caterpirade code never reads `var`. All ten
records have `id` 0. All Caterpirades act the same.

## State machine

States (table `0x800F7404`): 0 `0x800EC368` (set-up), 1 `0x800EBF6C` (main), 2 `0x800EC454`
(`DeleteObject`), 3 `0x800EC434` (`DeleteObject2`).

Each update, before the state routine (`0x800EC300`): when the floor flag is set, `speed_y` = 0;
then `prev_x`/`prev_y` are saved. The update function does **not** call `CollideCheck`; the main
routine does (below).

The step number is at `+0x06` (the kit names it `step2`) and the sub-step at `+0x07` (`act4`). The
set-up writes 2 to both `+0x05` and `+0x06`; only `+0x06` is read.

### Main (state 1), in this order [certain]

1. Clears the hit-flash bit, keeps the HP, calls `ContactWeapon`.
2. **Killed** (result < 0): death (below), state 2, end of the update.
3. **Hurt** (the HP changed): sound (bank 2, id 8), `speed_x` = `accel_x` = 0, sprite 0, step CURL
   (sub-step 0), hit-flash bit on. This is how a hit starts the curl.
4. Runs its step (table `0x800F7414`): 0 WALK, 1 CURL, 2 FALL, 3 TURN, 4 BALL. There is **no**
   engine hit-reaction step in this table.
5. Screen tests: off screen 64 → state 3; visible 32. (`DisplayCheck` runs twice when the object is
   on screen; this has no effect.)
6. `CollideCheck` (its own terrain).
7. **Platform** (below), only when `mega+0xE5` = 0 and the step is not CURL or BALL.
8. `ContactMega` (contact damage 4).
9. `latch`: 0 on the floor, else −1 when `speed_y` < 0 (falling), else 1. No code of this object
   reads it [certain]; its user is not known.

### WALK (step 0, `0x800EC144`)

- `AnimeTick`, `MoveObject2`. The speed is the one set on landing: **0.5 px/frame** (`0x8000`), no
  acceleration.
- **Ledge probe:** `TerrainPointCheck` at the front bottom corner of the terrain box plus 8 px down:
  (x ± 14, y + 22), + when facing right. No solid ground there → TURN.
- **Wall:** facing left and `collide_flags` bit 2, or facing right and bit 1 → TURN.
- TURN starts with a timer of 40.

### TURN (step 3, `0x800EC66C`) [certain]

| phase | time | what |
|---|---|---|
| wait | 40 updates | no move, no animation |
| turn animation | animation 1, about 5 updates | front view |

When the animation flag comes: animation 0, `flip` toggles, `speed_x` = −`speed_x`, back to WALK.
So it stops for about **45 updates** at each wall or ledge, then crawls the other way.

### FALL (step 2, `0x800EC5F4`)

- In the air: `MoveObject2` (gravity 0.125 px/frame², limited by `fall_max`).
- On the floor: **faces the player** (`flip` = right when `mega.x` ≥ its x), `speed_x` = ±0.5
  px/frame, `latch` = 0, back to WALK. So after every landing it crawls towards the player.

### CURL (step 1, `0x800EC474`) [certain]

`MoveObject2` every update (speed 0; gravity applies), then the sub-step (table `0x800F7428`):

| sub | time | what |
|---|---|---|
| 0 | 1 update | animation 2 (curl), ball boxes, `dmg_tbl_deflect` |
| 1 | 24 updates | animation 2 runs to its flag; then timer 40 |
| 2 | 40 updates | holds the ball picture |

At the end of sub-step 2: animation 3 (roll), **faces the player**, `speed_x` = **±1 px/frame**
(`0x10000`), timer 128, step BALL.

### BALL (step 4, `0x800EC6FC`) [certain for the code]

`AnimeTick` and `MoveObject2` every update (sub-step table `0x800F7434`):

| sub | time | what |
|---|---|---|
| 0 | 128 updates | rolls at 1 px/frame (128 px); then `accel_x` = −`0x200`, animation 5, timer 90 |
| 1 | 90 updates | slows by 0.0078 px/frame² (from 1 to 0.30 px/frame, about 59 px); then `speed_x` = `accel_x` = 0 |
| 2 | 1 update | when the sprite is 23 (end of animation 5) or 16: sound (bank 2, id 9), normal boxes and `dmg_tbl_normal`, animation 4 (uncurl) |
| 3 | 21 updates | animation 4 runs to its flag; then animation 0, step FALL |

- The roll goes about **187 px** in about **219 updates**. It has no wall or ledge test: at a wall
  `CollideCheck` stops it [likely]; at a ledge it falls with gravity [likely].
- The whole cycle from the hit to the end of the uncurl is about **305 updates (5 s)**. Weapons
  bounce off for the first 284 updates. During the 21 updates of the uncurl it can be hurt again,
  and a hit starts a new curl.

## The platform (the player rides on its back) [certain for the code]

The Caterpirade does **not** add a rectangle to `solid_rects` (`0x800E41B0`), and
`TerrainPointCheck` does not see it. The platform is an object-to-object test of the two terrain
boxes, in the main program (shared code). The Caterpirade calls it in step 7 of the main routine:

```
old72 = mega+0x72 ; mega+0x72 = 8          // temporary: "on a platform"
was   = self+0x72
SolidObject(self)                          // 0x80031DA8
mega+0x72 = old72 | (self+0x72 & 8)
if (was & 8) and not (self+0x72 & 8): mega+0x72 |= 8   // one update of grace
```

`SolidObject` (`0x80031DA8`) then pushes the player out, carries the player and sets the touch
flags, as in `docs/engine.md` ("Solid objects"). Particular to the Caterpirade [certain for the
code]:

- The contact damage that a centre-inside push or a touch writes to `mega+0xE6` is 4.
- While `mega+0xE5` ≠ 0 (75 updates after the player is hurt) the Caterpirade skips the platform, so
  for 75 updates after a hit the player falls through it [likely].

Save-state check: the player's terrain box (`0x80073D50` = (0, 2, 11, 19)) bottom is at
339 + 2 + 19 = 360, and the Caterpirade's top is at 374 − 14 = 360. `self+0x72` = 8,
`self+0x76` = 1, `mega+0x71` = 8, `mega+0x72` = 8, `mega+0xE6` = 4 [certain].

Consequences for play:

- Standing on top is safe: the contact box starts 7 px below the top of the body (y − 7 against
  y − 14) [certain for the boxes].
- Touching a side hurts (the contact box is 4 px wider than the body on each side) [likely].
- The platform is off during CURL and BALL. A Caterpirade that is hit under the player drops the
  player onto the ball (contact damage 4) [likely].

## Spikes [likely]

The object code has no spike or tile-damage test. Its `CollideCheck` uses the tiles only as floor and
walls, so it crawls over spike tiles unharmed. A player who rides it is on the object, not on the
tiles (the floor-tile test is skipped, `docs/engine.md`), so the spikes below do not touch the player. Whether
the spike tiles of this stage are under its paths was not checked.

## Animations (table `0x800F73D4`)

| # | what | frames |
|---|---|---|
| 0 | crawl | sprites 0, 1, 2, 12 updates each, loop |
| 1 | turn (front view) | sprites 3, 4, 4 (4, 1, 1 updates), flag 1 on the last, hold |
| 2 | curl into a ball | sprites 5–16 (2 updates each, 15: 3, 16: 1 + 1), flag 1 on the last, hold |
| 3 | ball roll | sprites 16–23, 8 updates each, loop |
| 4 | uncurl | sprites 15–5 (2 updates each, 5: 1 + 1), flag 1 on the last, hold |
| 5 | ball roll, slowing | sprites 16–23 (8, 8, 8, 8, 8, 10, 12, 1), hold on 23 |

Flags tested: animations 1, 2 and 4 (flag 1). Sprites tested: 23 and 16 (BALL sub-step 2).

## Damage

- Taken: `dmg_tbl_normal` (normal form), `dmg_tbl_deflect` (ball form). With 12 HP: lemon (3) 4
  hits, half charge (7) 2 hits, full charge (15) 1 hit, sabre (4) 3 hits. Each hit that does not
  kill starts the curl, and the next 284 updates are immune, so the hits must come in separate
  cycles (or during the 21-update uncurl).
- Dealt: contact 4 in both forms.

## Death

One `SpawnExplosion`, drop table 16, state 2 (`DeleteObject`). No debris.

## Sounds

Bank 2: id 8 when hurt; id 9 when the ball opens.

## Summary for a reimplementation

- 12 HP, contact 4, 28 × 28 px body; contact box 36 × 20, hurt box 24 × 18 (ball: 20 × 16, 16 × 18).
- Crawls at 0.5 px/frame; at a wall or a ledge (probe 8 px below the front corner) it stops for
  40 + 5 updates and turns. After any landing it faces the player.
- The body is a solid block for the player: push out on all sides, carry on top by its own move.
  Off while curled; off for 75 updates after the player is hurt.
- Hurt → curl (25 updates) → wait 40 → roll at 1 px/frame towards the player for 128 updates → slow
  down over 90 updates → open (21 updates) → crawl towards the player. Weapons bounce off from the
  curl to the end of the roll.
- Placed by main type 7, which only makes stage object type 1 and deletes itself.

## Open questions

1. The meaning of `mega+0x71` = 15 and `mega+0xE6` = 4 when the player's centre gets inside the
   body, and whether this hurts the player.
2. The second actor at `0x800CD340` (and player step 18) and the global `0x800C4568`.
3. Whether the spike tiles of this stage are under the Caterpirade paths (the claim "immune to
   spikes" is not tied to code here).
4. Who reads `latch` (`+0x67`) of a stage object.
5. The roll at a wall: whether it only stops, as assumed.
