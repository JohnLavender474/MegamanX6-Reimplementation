# Killer Moth (stage 03, main object type 4): behaviour sheet

Killer Moth is a small flying robot moth. It hovers in place. When the player comes near, it dives to
a point above the player, flies one small circle there and drops 8 slow-falling powder shots. Then it
hovers again. A second object (misc type 4) follows it and draws its exhaust flame. A variant (record
`id` ≠ 0) is hidden at the start, comes in from a screen edge and flies to its spawn point.

Overlay `03` (Commander Yammark), save state `yammark-1_start.state.gz`. Written with the RE kit
(`python3 -m mmxdemake.rekit --name killer-moth`) and `docs/re-guide.md`. Units, confidence tags and
the shared engine rules: `docs/engine.md`. Records: `docs/stages/yammark.md`.

## Spawn and set-up [certain]

- **HP 8**, contact damage **4**.
- Boxes:
  - contact box `0x800F7180` = (−8, −8, 16, 16): 16 × 16 px around the origin;
  - hurt box `0x800F7184` = (−8, −8, 16, 16): the same size;
  - no terrain box. It flies through walls and never tests the terrain.
- No gravity, no speed at the start. Draw priority 5.
- It makes the exhaust flame overlay (misc type 4, below).
- Each update, except in ENTER, it faces the player (equal x → left).

## The `id` parameter [certain]

| id | start step | boxes | flame | what |
|---|---|---|---|---|
| 0 | HOVER (2) | contact and hurt box on | mode 1 | visible at its spawn point |
| ≠ 0 | ENTER (5) | both off until it appears | mode 0 (off) | hidden; comes in from a screen edge |

All 8 Killer Moth records of this stage have id 0. No user of id ≠ 0 was found (see open questions).

## State machine

Standard states (table `0x800F7254`): 0 `0x800EB37C`, 1 `0x800EB490`, 2 `0x800EB57C`, 3 `0x800EB59C`.

### Main (state 1) [certain]

1. Saves the previous position. Faces the player (not in ENTER).
2. Runs the animation (`AnimeTick`).
3. Runs its step (table `0x800F7264`): 0 engine hit reaction, 1 nothing, 2 HOVER, 3 SWOOP, 4 CIRCLE,
   5 ENTER.
4. **Death:** when a weapon kills it (see Death), then state 2.
5. Otherwise: contact test. Off screen 80 (x) / 160 (y) → state 3. Else (not in ENTER) visible
   48 (x) / 64 (y).

The flame mode (`+0x88`) tells the overlay which flame to show: 0 off, 1 down, 2 diagonal,
3 sideways.

### HOVER (step 2) [certain]

The moth does not move.

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | animation 0 (wings open), flame mode 1, timer 180 |
| 1 | up to 180 updates | waits. It goes on when the timer is 0 **or** when the player is less than 48 px away in x (y is not tested) |
| 2 | 1 update | animation 2 (wing-up wind-up), flame mode 2 |
| 3 | about 7 updates | waits for the event flag 1 of animation 2 (on its second frame, after 7 updates), then SWOOP |

So the moth attacks at once when the player is near in x. Without the player near, it attacks every
180 updates (3 s) anyway.

Quirk [certain for the code, likely for the effect]: in sub-step 1, the timer and the distance test
can both add 1 in the same update. Then sub-step 2 is skipped: animation 0 stays on, it has no event
flag, and sub-step 3 waits for ever. The moth then hovers until it is destroyed or deleted off screen.
This happens only when the player comes within 48 px in x in the same update that the timer ends.

### SWOOP (step 3) [certain]

| sub-step | what |
|---|---|
| 0 | animation 4 (side-on dive pose), flame mode 3 (sideways) |
| 1 | each update: aims at the target point and moves |

- Target point: the player's x; the player's y **minus 48 px** (48 px above the player) when the player
  is X (`mega.var` = 0), **minus 64 px** when the player is Zero [certain for the code; X/Zero likely].
- Each update: angle `a` = `ratan2` toward the target; `speed_x` = 2.5 × cos(a) px/frame,
  `speed_y` = 1.25 × sin(a) px/frame (`rcos(a) × 160 / 4` and `rsin(a) × 160 / 8`). So it moves
  twice as fast in x as in y, and it follows the player as the player moves.
- No time limit. When it is less than 3 px from the target in x and in y, it snaps onto the target,
  stores the circle direction (−32 per update when it faces left, +32 when it faces right), and goes
  to CIRCLE.

### CIRCLE (step 4) [certain]

| sub-step | what |
|---|---|
| 0 | circle centre = its position minus 16 px in y (16 px above it); angle 3072 (bottom of the circle, so no jump); animation 6 (spin); flame mode 1; shot timer 20; shot count 8 |
| 1 | each update: angle += ±32; x = centre x + 16 × cos, y = centre y − 16 × sin (radius 16 px) |

- One full turn takes 4096 / 32 = 128 updates. It moves first to the side that it faces.
- Every 20 updates: sound (bank 2, id 5) and one powder shot at its position. After 8 shots
  (160 updates, 1.25 turns) it goes back to HOVER.
- At the end it is 16 px to the facing side and 16 px above the point where the circle started.

### ENTER (step 5, id ≠ 0 only) [certain]

| sub-step | time | what |
|---|---|---|
| 0 | until the spawn point is on screen | waits while its spawn point is outside the 320 × 240 px view (camera layer 0). Then the timer = one of {0, 8, 16, 32, 48, 64, 80, 96} (random, table `0x800F729C`) |
| 1 | timer + 1 updates | waits (hidden, no boxes). Then: home = spawn point; flame mode 3; animation 4; position = camera + one of 16 random points (table `0x800F72AC`); faces the player; contact and hurt boxes on |
| 2 | until it is home | flies straight to home at 1.25 px/frame (`rcos/rsin × 160 / 8` in both axes); visible 48 × 64. Less than 3 px from home in x and y: snaps onto it, then HOVER |

The 16 start points (screen coordinates, relative to the camera) [certain]:

| index | x | y | edge |
|---|---|---|---|
| 0–3 | −16 | 30, 90, 150, 210 | left, outside |
| 4–7 | 336 | 30, 90, 150, 210 | right, outside |
| 8–11 | 40, 120, 200, 280 | −16 | top, outside |
| 12–15 | 40, 120, 200, 280 | 256 | bottom, outside |

## Animations (table `0x800F7234`, 8 entries)

| # | what | frames | user |
|---|---|---|---|
| 0 | hover: front view, wings open | sprite 0, hold | HOVER 0 (and the set-up) |
| 1 | exhaust flame, downward (overlay) | sprites 11–15, 37 (blank); 1 2 2 1 1 2 updates = 9, loop | flame mode 1 |
| 2 | wind-up: wings raised | sprite 1, 7 updates, then sprite 1 with **flag 1**, hold | HOVER 2 |
| 3 | exhaust flame, diagonal (overlay) | sprites 16–20, 37; 9 updates, loop | flame mode 2 |
| 4 | dive pose, side view | sprite 2, hold | SWOOP 0, ENTER 1 |
| 5 | exhaust flame, sideways (overlay) | sprites 21–25, 37; 9 updates, loop | flame mode 3 |
| 6 | spin round its vertical axis | sprites 0, 26–36; 3 updates each (36 per turn), loop | CIRCLE 0 |
| 7 | powder shot: purple sparkles | sprites 3–10, 37; 4 updates each (36), loop | powder shot |

## The exhaust flame (misc object type 4) [certain]

- Made in the set-up (`0x800EBABC`): `id` 0; copies the parent (`docs/engine.md`). Draw priority 6.
- Each update it copies the moth's position and flip, and runs its animation. When the moth's flame
  mode changes, it restarts: mode 0 → hidden; 1 → animation 1; 2 → animation 3; 3 → animation 5
  (all visible). The last mode is kept at `+0x54` (`0xFF` at the start).
- It does no box test. It deletes itself when the moth is no longer in state 1 (dead or deleted).

## Powder shot (visual object type 2, `0x800EB304`) [certain]

- Made in CIRCLE every 20 updates (`0x800EBB70`), `id` 0, at the moth's position; copies the parent.
  Its parent pointer is not used by its code.
- Animation 7. Contact damage **4**. Contact box `0x800F72F8` = (−6, −6, 12, 12): 12 × 12 px. Terrain
  box `0x800F72FC` = (0, 0, 5, 5). No hurt box: weapons cannot destroy it. Draw priority 4.
- Falls straight down at **0.25 px/frame** (`speed_y` = −`0x3FFF`), with no gravity and no x speed.
- Each update: animation, move, contact test. It is **not** removed when it touches the player.
  It is removed on terrain and 32 px off screen. Visible within 16 px.

## Damage

- Taken: `dmg_tbl_normal`, 8 HP: 3 lemons, 2 half charges, 1 full charge, 2 sabre hits.
- Dealt: moth 4; powder shot 4 (the shot stays).

## Death

One explosion (`SpawnExplosion`), drop table 18, then state 2. The flame overlay sees that the moth
left state 1 and deletes itself. Powder shots that are already in the air stay.

## Sounds

Bank 2, id 5: each powder shot (8 per circle).

## Summary for a reimplementation

- 8 HP, contact damage 4, 16 × 16 px body; flies, ignores terrain; always faces the player.
- HOVER: stays still with the downward flame. Attacks when the player is within 48 px in x, or after
  180 updates.
- Wind-up of about 7 updates (wings raised), then SWOOP: homes at 2.5 px/frame (x) and 1.25 px/frame
  (y) onto the point 48 px above X (64 px above Zero) until it is within 3 px.
- CIRCLE: radius 16 px round a centre 16 px above it, 32/4096 turn per update, toward the facing side.
  Drops a powder shot every 20 updates, 8 in all (160 updates), then HOVER again.
- Powder: 12 × 12 px, 4 damage, falls at 0.25 px/frame, not destroyed by weapons or by the player,
  removed by terrain or off screen.
- Variant id ≠ 0: hidden; after the spawn point comes on screen and a random 1–97 update delay, enters
  from one of 16 points just outside the screen edges and flies at 1.25 px/frame to its spawn point.

## Open questions

1. Which object or record uses id ≠ 0 (the ENTER variant)? All 8 records in this stage have id 0.
2. The stuck-hover quirk (HOVER sub-step 2 skipped) is not seen in play.
3. The meaning of `latch` = 1 in the powder shot (a visual object); it may block the shot from
   gravity or the hit test.
4. Whether `ContactMega` on a visual object really damages the player (assumed, as for other shots).
