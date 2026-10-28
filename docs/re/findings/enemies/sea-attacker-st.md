# Sea Attacker ST (stage 09, main object type 10): behaviour sheet

Sea Attacker ST is a small seahorse mechaniloid. It has two modes, set by the record `id`:

- **id 0, shooter:** it hovers in place. When the player is near, it fires a bubble shot. The bubble
  rises to the player's height, stops, then flies horizontally to the player's side.
- **id ≠ 0, diver:** it curls into a spinning ball, rises above the player, waits, dashes
  horizontally to the player's x, then drops straight down and leaves the screen.

Overlay `09` (Rainy Turtloid), save state `turtloid-1.state.gz`. Written with the RE kit
(`python3 -m mmxdemake.rekit --name sea-attacker-st`) and `docs/re-guide.md`. Units, confidence tags
and the shared engine rules: `docs/engine.md`. Records: `docs/stages/turtloid.md`. "Above the
player" means a smaller y than the player's origin (`mega.y.px`).

## Spawn and set-up [certain]

- **HP 6**, contact damage **4**, `iframes` = 0.
- Draw priority 7.
- Speeds, acceleration and gravity are 0. **No terrain box** and no terrain check: the body goes
  through walls and floors.
- It faces the player at spawn. A "face the player" timer (`+0x80`) is set to 30.
- Boxes [certain for the numbers, likely for the reading]:

  | box | address | data | facing left | in blocks |
  |---|---|---|---|---|
  | contact box (body) | `0x800F5B98` | (−1, −16, 10, 28) | X − 1 … X + 9, Y − 16 … Y + 12 | 0.6 × 1.75 |
  | hurt box (body) | `0x800F5B9C` | (−2, −16, 11, 30) | X − 2 … X + 9, Y − 16 … Y + 14 | 0.7 × 1.9 |
  | contact box (ball) | `0x800F5BA0` | (−8, −8, 16, 16) | X − 8 … X + 8, Y − 8 … Y + 8 | 1 × 1 |
  | hurt box (ball) | `0x800F5BA4` | (−8, −8, 16, 16) | as the contact box | 1 × 1 |

  The ball boxes are used only by the diver after it curls up.

## The `id` parameter [certain]

| id | mode | first animation | step |
|---|---|---|---|
| 0 | shooter | 0 (hover) | 2 |
| ≠ 0 | diver | 3 (curl into a ball) | 3 |

## State machine

Standard states (table `0x800F5C58`): 0 `0x800EABCC`, 1 `0x800EACD0`, 2 `0x800EADA0`, 3 `0x800EADC0`.

### Main (state 1) [certain]

1. Saves the previous position. Runs `AnimeTick`.
2. Runs its step (table `0x800F5C68`): 0 engine hit reaction, 1 does nothing, 2 SHOOTER, 3 DIVER.
3. **Death:** when a weapon kills it (see Death), then state 2.
4. Otherwise: contact test; off screen 64 → state 3; visible 32.

### SHOOTER (step 2, id 0)

Sub-steps (table `0x800F5C78`) [certain for the code; timing is a hand simulation, likely ±1 frame]:

| sub-step | routine | what |
|---|---|---|
| 0 WAIT | `0x800EAE1C` | animation 0. Waits until the horizontal distance to the player is **≤ 96 px** (6 blocks). Then it faces the player and starts animation 1 (fire). |
| 1 FIRE | `0x800EAE98` | On the frame with flag 1 (23 frames after the start): sound (bank 2, id 26) and one bubble shot. On the frame with flag 2 (47 frames after the start): animation 0, rest timer 20 frames. |
| 2 REST | `0x800EAF0C` | 20 frames. Each frame the "face" timer (`+0x80`) counts down; at 0 it faces the player and the timer is set to 30 again. Then back to WAIT. |

So, while the player is within 96 px, it fires **one bubble every 68 frames (1.13 s)**: 47 frames of
animation, 20 frames of rest, 1 frame of WAIT [likely]. It does not move. It turns to the player
only at the start of FIRE and every 30 frames of REST time.

### DIVER (step 3, id ≠ 0)

Sub-steps (table `0x800F5C84`). After the sub-step, `MoveObject` moves it by its speed [certain]:

| sub-step | routine | what |
|---|---|---|
| 0 CURL | `0x800EAFD4` | Animation 3 plays: the seahorse stretches and curls into a ball (16 + 6 + 2 = 24 frames). On the frame with flag 1: the ball boxes are set, speed up **3.75 px/frame** (`0x3C000`), and a random margin `r` = `GetRNG() & 31` (0–31 px) is stored at `+0x82`. |
| 1 RISE | `0x800EB048` | Rises at 3.75 px/frame until it is **at least `r` px above the player's origin** (`mega.y.px − y.px ≥ r`). Then: draw priority 18, it faces the player, wait timer 40 frames, speed 0. |
| 2 HOVER | `0x800EB0B8` | 40 frames in place (the "face" timer turns it to the player every 30 frames). At the end it faces the player and gets speed **3.75 px/frame** horizontally in the facing direction. |
| 3 DASH | `0x800EB16C` | Flies horizontally. When the player's x is within **±1.5 px** (`0x18000`) of its x: horizontal speed 0, speed **3.75 px/frame down**. |
| 4 DROP | `0x800EB1D4` | Does nothing; it keeps falling until it is 64 px off screen and is deleted (state 3). |

The ball animation (sprites 4, 5, 6, 2 frames each) runs in all sub-steps after CURL.

Note on DASH [certain for the code; likely for the effect]: the window is 3 px wide, but the ball
moves 3.75 px per frame. It can pass the player's x without being inside the window. Then it keeps
flying horizontally until it is 64 px off screen. For a player who stands still, this happens on
about 1 dash in 5 (the chance that a 3.75 px step lands outside a 3 px window).

The diver never climbs back. Each diver attacks once and then leaves the screen.

## Animations (table `0x800F5C28`)

Frames as (sprite, frames, flag) [certain for the data; the descriptions are from the export]:

| # | what | frames |
|---|---|---|
| 0 | hover, wings beat | sprites 0, 15; 20 frames each; loop |
| 1 | fire: bends forward and back | sprites 1, 7, 8, 8 (flag 1), 8, 7, 1, 1 (flag 2); 6 15 2 1 3 15 5 1 frames = 48; hold |
| 2 | bubble shot, grows and spins | sprites 9–14; 2 frames each; loop |
| 3 | diver: stretches, curls, spinning ball | sprites 1, 2, 3 (16, 6, 2 frames), then 4 (flag 1), 5, 6 (2 each); loop to frame 3 |
| 4–8 | debris pieces (wing, fin, head, body, tail) | 1 frame each; hold |
| 9 | bubble shot, stopped and pulsing | sprites 9, 10, 11, 21, 22; 2 frames each; loop |

## The bubble shot (visual object type 9, `0x800EAB90`) [certain]

- Made by `0x800EB1E4` in FIRE, when a visual slot is free; copies the parent (`docs/engine.md`;
  position in whole pixels).
- **Set-up** (`0x800EA880`): HP 1, contact damage **4**, `dmg_tbl_one_hit` (one hit destroys it,
  but only from a weapon with `+0x9C` ≠ 0; plain buster shots pass through it, `docs/damage.md`,
  "Shots that can hit enemy shots" [certain for the code]), draw priority 5,
  animation 2.
- Spawn point: **6 px to the back** of the parent (x + 6 when the parent faces left, x − 6 when it
  faces right) and **20 px up**.
- Boxes: terrain box `0x800F5C98` = (0, 0, 2, 2) (a 4 × 4 px body); contact box `0x800F5C9C` and hurt
  box `0x800F5CA0` = (−2, −2, 4, 4) (4 × 4 px).
- The set-up calls `ratan2` with the offset to the player, but the result is not used.

Movement (sub-step at `+0x07`, routine `0x800EA9B0`; it moves first, then tests):

| sub-step | what |
|---|---|
| 0 RISE | **4 px/frame up** until its y is at or above the player's origin (`y.px ≤ mega.y.px`). Then speed 0, animation 9, and a wait timer (`+0x88`). |
| 1 WAIT | Waits **40 frames** on Easy and Normal, **8 frames** on Hard. Then animation 2 and **4 px/frame horizontally toward the player's side** (left when the player's x is smaller, else right). |
| 2 FLY | Flies straight; no more aiming. |

When the bubble spawns at or above the player's origin, RISE ends in its first update.

End of the shot: a weapon kill (explosion); terrain (`SpawnHitEffect`); 32 px off screen. Visible
within 16 px. **Touching the player does not end it**: `ContactMega` hurts the player, but its result
is not used. The bubble flies on through the player [certain for the code].

## Damage

- Taken: `dmg_tbl_normal`, 6 HP: 2 lemons, 1 half charge, 1 full charge, 2 sabre hits (id 25: 1).
- Dealt: body 4 (also the ball), bubble 4.

## Death [certain]

5 debris pieces (`SpawnJunk`, animations 4–8 from `0x800F5C50`), one explosion (`SpawnExplosion`),
drop table 19, then state 2.

## Sounds [certain]

Bank 2, id 26: the bubble shot.

## Summary for a reimplementation

- 6 HP, contact damage 4, no terrain collision. Two modes by `id`.
- Shooter (id 0): hovers. When the player is within 96 px horizontally, it turns to the player and
  fires a bubble every 68 frames.
- Bubble: 4 px/frame up to the player's height, waits 40 frames (8 on Hard), then 4 px/frame
  horizontally toward the player. 4 × 4 px, damage 4, 1 HP (one hit from a weapon with `+0x9C` ≠ 0; the buster passes through), bursts on walls, passes
  through the player.
- Diver (id ≠ 0): after 24 frames it curls into a 16 × 16 px ball, rises at 3.75 px/frame to 0–31 px
  above the player, waits 40 frames, dashes horizontally at 3.75 px/frame to the player's x (±1.5
  px), then drops at 3.75 px/frame off the screen.
- Death: 5 debris pieces, one explosion, drop table 19.

## Open questions

1. Whether the bubble really passes through the player (the code ignores the `ContactMega` result).
   Check in play.
2. DASH can miss the ±1.5 px window and fly on to the edge of the screen. Check how often this
   happens in play.
3. The meaning of draw priority 18, set when the diver stops rising (7 before).
4. Where the player's origin `mega.y.px` is on the player sprite. It sets the bubble's stop height
   and the diver's hover height.
