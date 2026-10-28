# Ice block (stage 04, main object type 17): behaviour sheet

Proposed as `docs/enemies/ice-block.md`.

The ice block is a blue ice boulder of Blizzard Wolfang's stage, large (64 px) or small (48 px),
picked at random. One object type does all the ice hazards of the stage's first part. The record
`id` (`var`) picks the mode: modes 0–3 are invisible spawners, modes 4–7 are the blocks that they
make. The stage logic object 12 (`docs/enemies/ice-fall-controller.md`) makes the spawners when
the player enters a trigger zone. No record of the main list has this type. The blocks have
8 HP and do 8 contact damage. Only the Metal Anchor (weapon slot 4) can break them; all other
shots bounce off. "Ice block" is a descriptive name; no official name was found.

| mode (`var`) | what | made by |
|---|---|---|
| 0 | avalanche spawner: 51 blocks in 6 s, to the left | the controller, triggers 0–5 |
| 1 | avalanche spawner, to the right | no trigger uses it |
| 2 | falling-ice spawner: 18 pieces in 3 s, a 2 s pause, again | the controller, triggers 6–11 |
| 3 | bouncing-block spawner: 4 blocks 101 frames apart, then a pause | the controller, triggers 12–14 |
| 4 | avalanche block: blinks 50 frames, then flies down-left at (−8, +4) px/frame | mode 0 |
| 5 | the same, down-right at (+8, +4) px/frame | mode 1 |
| 6 | falling ice: falls straight down at 9 px/frame, through the floor | mode 2 |
| 7 | bouncing block: blinks 20 frames, then hops toward the player | mode 3 |

Overlay `04` (stage 2, area 0). Entry `0x800EED40`, state table `0x800F6CE8`, step table
`0x800F6CF4`, animation table `0x800F6CBC`. Save state `wolfang-3_falling-ice` has one spawner
(mode 2) and four falling pieces (mode 6) live; `wolfang-1_x-sabre` and `wolfang-2_ice-slope`
have none. Kit: `python3 -m mmxdemake.rekit --state wolfang-3_falling-ice.state.gz --type 17`.
Export: `ice-block` (`enemy-3362eccf5a`). Units, tags and shared rules: `docs/engine.md`. No stage
page for stage 2 exists yet. All values are [certain for the code] unless a line says otherwise.

## Spawn and set-up (state 0, `0x800EE334`)

- The controller makes the object with `GetMainObject` (flags `0x41`, type 17, `var` = the mode)
  and gives it the sprite source of the type 17 bank (bank 5). A spawner copies its own sprite
  source, palette and animation table to each block that it makes. The object has no record
  (`+0x10` = 0), so `DeleteObject3` removes it.
- **HP 8**, damage table `dmg_tbl_ice_block` (`0x80075CDC`, below), contact damage **8**,
  `iframes` 0. Draw priority 6, layer = `mega.layer`, `flip` 0.
- **Size** `+0x80` = `GetRNG() & 1`: 0 large, 1 small. The boxes come from the size:

  | box | large (size 0) | small (size 1) |
  |---|---|---|
  | hurt (`0x800F6C58`) | (−32, −32, 64, 64) | (−24, −24, 48, 48) |
  | attack (`0x800F6C50`) | (−28, −28, 56, 56) | (−22, −22, 44, 44) |
  | terrain (`0x800F6C48`) | centre (0, 0), half 30 × 30 | centre (0, 0), half 22 × 22 |

- Then by mode (switch `0x800E9AB0`); every mode ends in state 1:

  | mode | set-up |
  |---|---|
  | 0 | timer `+0x7C` = 120; no boxes; not drawn; step 3 |
  | 1 | timer = 12; no boxes; not drawn; step 3 |
  | 2 | timer = 0; no boxes; step 3 |
  | 3 | timer = 0, count `+0x7E` = 0; no boxes; step 3 |
  | 4 | `speed_x` −8.0, `speed_y` −4.0 (4 px/frame down); no boxes; animation = size; no terrain box; timer 50; step 4 |
  | 5 | `speed_x` +8.0, `speed_y` −4.0; animation = size; no terrain box; timer 50; step 4 |
  | 6 | `speed_y` −9.0 (9 px/frame down); animation = size + 2; step 1 |
  | 7 | `speed_x` −1.125 when the player is to the left (`flip` 0), else +1.125 (`flip` `0x40`); `speed_y` +2.0625 (up); gravity `0x1080` (0.064453125); animation = size; timer 20; step 2 |

  A `var` of 8 or more has no case: the object stays in step 0 (`HitReactionDispatch`).

## The `id` parameter

The mode, from the controller's trigger table (`0x800F6E08`). See the table at the top.

## State machine

States (`0x800F6CE8`): 0 set-up, 1 main (`0x800EE5C4`), 2 delete (`0x800EEDB8`, `DeleteObject3`).
Steps (`0x800F6CF4`): 0 `HitReactionDispatch` (no mode uses it), 1 MOVE (`0x800EEDD8`), 2 BOUNCE
(`0x800EE78C`), 3 SPAWN (`0x800EE8D8`), 4 APPEAR (`0x800EEE14`).

**Entry** (`0x800EED40`): when the player is at x ≥ 4673 and y ≥ 1265 (past the ice part, at the
bottom of the stage), `DeleteObject3`. So every type 17 object goes away there.

**Main** (state 1), in order:

1. `prev_x`, `prev_y` = x, y. The step.
2. `ContactWeapon`. A kill: `game+0x27` = 120 when it is 0 (no reader found); modes 4, 5 and 7
   roll `DropItem` table 21; 3 debris (`SpawnJunk(3, {4, 5, 6, 7, 8})`: animations 4, 5, 6
   [likely: one per list entry]); state 2. No sound and no explosion.
3. Else `ContactMega` (8 damage).
4. The end rules:
   - mode 4: state 2 when its x < player x − 240, or when `game.checkpoint` > 0;
   - mode 5: state 2 when its x > player x + 240, or when the player is at y ≥ 529 and x ≥ 5025;
   - modes 6 and 7: state 2 when `OffScreenCheck2(240, 64)` = 1 (240 px left or right of the view,
     64 px above or below it); `latch` = 0 on a floor, else −1 (`speed_y` < 0) or 1;
   - modes 0–3: none (the spawner ends itself, below).

There is no `DisplayCheck` and no off-screen freeze. A spawner is not drawn (`display` 0).

### SPAWN (step 3, modes 0–3)

Each update: when the timer `+0x7C` is not 0, count it down and do nothing else. Then:

- **Mode 0 (avalanche, left):** state 2 when `game.checkpoint` > 0 or the count `+0x7E` is 0. Else,
  when count mod 7 = 0, a mode 4 block at (x, y + rand(−16 … 15)). Count − 1. The controller sets
  the count to 360. So: a wait of 120 frames, then 51 blocks, one every 7 frames (count 357 … 7),
  then the spawner ends at frame 480.
- **Mode 1:** the same with mode 5 blocks and no checkpoint test (timer 12).
- **Mode 2 (falling ice):** when the count is 0: timer = 120, count = 180. Else, when count mod 10
  = 0, a mode 6 piece at (x + rand(−8 … 7), y); count − 1. The controller sets the count to 180, so
  the first piece comes at once. Cycle: 18 pieces in 180 frames, one update at 0, then 120 frames
  of rest: **301 frames**. The spawner never ends by itself (only the entry rule above).
- **Mode 3 (bouncing blocks):** nothing while the player is less than 48 px away in x. Else, when
  the count is not 0, count − 1. At 0: a mode 7 block at (x + rand(−8 … 7), y); `+0x81` + 1, and
  after the 4th block `+0x81` = 0 and timer = 140; count = 100. So: 4 blocks 101 frames apart,
  then 241 frames to the next block. The count stops while the player is near; the timer does not.
  The spawner never ends by itself.

`GetMainObject` is called before the "mod" test, so a free slot must exist for a block; a frame
with no free slot makes no block and the count goes on.

### APPEAR (step 4, modes 4 and 5)

`AnimeTick`. For 50 updates: no boxes, `display` toggles (a blink), no move. Then the boxes of the
size, `display` 1, step 1. So the block appears at the spawner point and blinks there for 50
frames, turning, before it flies. It cannot hurt and cannot be hit while it blinks.

### MOVE (step 1, modes 4, 5 after APPEAR; mode 6)

`act4` + 1 (the age; the controller reads it), `AnimeTick`, `MoveObject` (straight line, no
gravity). **No terrain test**: the blocks pass through floors and walls.

- Modes 4 and 5 move (∓8, +4) px/frame: 2 px across for 1 px down. That is the angle of the ice
  slopes of this stage (collision 25, 26 and 27, 28: 8 px per 16 px). The spawner points of
  triggers 0, 1, 4 and 5 are 63 px above the slope surface, trigger 2 is 55 px above
  [certain for the map]. So the blocks run along the slope at 47–79 px (centre) above it,
  at 480 px per second [likely for the look in play].
- Mode 6 falls at 9 px/frame. In `wolfang-3_falling-ice` the pieces are 90 px apart (10 frames ×
  9 px), at `act4` 3, 13, 23, 33 [certain for the state]. The spawner of trigger 6 is at (3320, 211)
  under the ceiling (collision 57 at y 208); the column falls into the gap between the two ice
  pillars at x 3232–3279 and 3360–3407 [certain for the map]. A piece is removed at y ≥ 529
  (view bottom 464 + 64), so at most 4 are live.

### BOUNCE (step 2, mode 7)

For 20 updates: no boxes, blink, no move. Then each update:

1. When `collide_flags` bit `0x01` (right wall) is set and `speed_x` > 0, or bit `0x02` (left
   wall) and `speed_x` < 0: sound (2, 17), 3 debris, state 2 (the rest of this update still runs). No item.
2. `display` 1, boxes of the size, `AnimeTick`, `MoveObject2`.
3. On a floor (`collide_flags` bit `0x08`, from the last `CollideCheck`): `speed_y` = 2.0625,
   gravity `0x1080`, `Quake2(8, 2, 2)`, sound (2, 17).
4. `CollideCheck`.

So the block hops toward the side where the player was at its spawn, 1.125 px/frame, with the
same hop at each landing: about 64 frames and 33 px high, 72 px across per hop [likely: from the
numbers, with no speed limit in `MoveObject2`]. It shakes the screen at each landing and breaks
on the first wall in its way. It does not turn.

## Animations (table `0x800F6CBC`, 9 entries) [certain for the data]

| # | sprites | use |
|---|---|---|
| 0 | 0–7, 8 updates each, loop | large block, turning (modes 4, 5, 7 with size 0) |
| 1 | 8–15, 8 updates each, loop | small block, turning (size 1) |
| 2 | 0 (held) | large falling piece (mode 6) |
| 3 | 8 (held) | small falling piece (mode 6) |
| 4–8 | 16–20 (one each) | debris (`SpawnJunk` list `0x800F6CE0` = 4 5 6 7 8) |

Sprites: bank 5 (`~/mmx-data/x6/re/wolfang-ice/sprites-root/banks/wolfang/wolfang-3_falling-ice/bank-5.png`):
0–7 a 64 px boulder at 8 angles, 8–15 a 48 px boulder, 16–20 small chips. The export
`ice-block` (`enemy-3362eccf5a`) has all 9 animations; `wolfang-3_falling-ice` adds a live source
of the same ID.

## Damage

`dmg_tbl_ice_block` (`0x80075CDC`, new):

| ids | damage |
|---|---|
| 4, 13 | 3 |
| 55 | 127 |
| 7, 23 | – (no effect) |
| all other ids | D (deflected) |

HP 8: 3 hits of the Metal Anchor (slot 4, ids 4 and 13). The buster, the Z-sabre and Ice Burst
bounce off. Damage dealt: contact 8 (before the difficulty scaling), in every mode that has boxes
(4–7 after the blink, 6 at once). The spawners (0–3) have no boxes.

## Death

- A weapon kill: 3 debris, `DropItem(table 21)` for modes 4, 5, 7 (not for the falling pieces),
  `game+0x27` = 120, then state 2: `DeleteObject3`.
- A bouncing block that hits a wall: sound (2, 17), 3 debris, no drop.
- All other ends: `DeleteObject3` with no effect.

## Sounds [certain for the numbers]

| bank, id | when |
|---|---|
| 2, 17 | a bouncing block lands, or breaks on a wall (word `0x12200082`: `m096-e05` `vag-03.wav`) |

The rumbles of the avalanche and of the falling ice (5/8, 5/9) and their stop (5/3) are played by
the controller, not by this object.

## Summary for a reimplementation

1. One class with a size (64 or 48 px, random), 8 HP, 8 contact damage, hurt only by the Metal
   Anchor (3 per hit).
2. Avalanche: at a trigger, a point 63 px above an ice slope makes 51 blocks in 6 s. Each blinks
   50 frames in place, then flies down the slope at (−8, +4) px/frame, through the terrain. The
   screen shakes ±3 px for 8 s. Not after checkpoint 1.
3. Falling ice: a point under the ceiling drops one piece every 10 frames for 3 s (18 pieces),
   then rests 2 s, for ever. A piece falls at 9 px/frame through everything.
4. Bouncing blocks: a point drops 4 blocks 101 frames apart, then rests 4 s; only while the player
   is 48 px or more away. Each blinks 20 frames, then hops toward the player (1.125 px/frame,
   hop 2.0625 up, gravity 0.0645), shakes the screen at each landing and breaks on a wall.
5. Everything goes away when the player reaches the bottom of the stage (x ≥ 4673, y ≥ 1265).

## Open questions

1. What reads `game+0x27` (set to 120 by a kill; cleared by the controller set-up).
2. Whether the avalanche blocks hit a standing player: the block bottom is 19–51 px (large) or
   25–57 px (small) above the slope surface, by the random y. A play check is needed.
3. The look of a falling piece that passes through a floor (draw priority 6 against the tile
   layers).
4. Mode 1 / 5 (the right-hand avalanche) and its end rule (y ≥ 529, x ≥ 5025): no trigger of
   this stage uses it. Maybe a left-over.
5. The official name.
