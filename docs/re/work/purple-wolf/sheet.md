# Wolfloid (stage 04, main object type 6): behaviour sheet

The Wolfloid is a purple robot wolf of Blizzard Wolfang's stage (North Pole Area). It does not
walk. It stands on the floor or on an ice slope, faces the player, and attacks when the player is
near in x. It has two weapons: ice breath (a short cloud in front of the head) or a cannon on its
back that fires a ball in an arc at the player. On the slopes the weapon depends on the character:
X meets the cannon, Zero meets the breath. Out of range, it howls every 200 updates.

Overlay `04` (Blizzard Wolfang, member 96), save state `wolfang-2_ice-slope.state.gz`. Written with
the RE kit (`python3 -m mmxdemake.rekit --state wolfang-2_ice-slope.state.gz --type 6`) and
`docs/re-guide.md`; the kit did not walk the state and step functions, so they were read with
`--at` (see "Open questions"). Units, confidence tags and the shared engine rules:
`docs/engine.md`. Records: `docs/stages/wolfang.md` (not written yet; the records are listed at the
end of this sheet until then).

Name: "Wolfloid" [likely: the Mega Man Knowledge Base text, as a search summary gives it: "infected
ones attack the player by either launching bombs or by breathing ice"; the page itself was not
read]. The export folder is `purple-wolf` (`enemy-bd435676eb`).

## Spawn and set-up [certain]

- **HP 12**, contact damage **4**, `dmg_tbl_normal`.
- Draw priority 5. Layer = `mega.layer`.
- Boxes:
  - terrain box `0x800F6720` = (0, 0, 16, 16): a 32 × 32 px body centred on the origin (the feet are
    16 px below the origin);
  - attack box `0x800F6724` = (−16, −16, 32, 32) and hurt box `0x800F6728` = (−16, −16, 32, 32):
    the same 32 × 32 px square.
- `gravity` = `0x40000` (4 px/frame²); speeds 0. `MoveObject2` then clamps `speed_y` to
  `fall_max` (−6.5 px/frame) from the second update on, so the wolf is pressed onto the floor each
  update and `CollideCheck` puts it back on the surface. The live wolf of `wolfang-2` has
  `speed_y` = −6.5 [certain]. **The wolf never moves in x** (no code writes `speed_x` or
  `accel_x` other than 0).
- Nothing in the code tests the ice, the Nightmare Effect or the difficulty [certain].

## The `id` parameter [certain]

The step depends on `var` and, for ids 0 and 1, on the player character (`mega.var`: 0 = X).

| id | X | Zero | what |
|---|---|---|---|
| 0 | step 6 (cannon) | step 4 (breath) | on a slope that rises to the right (records stand on values 25, 26: the 2-cell ice slope) |
| 1 | step 7 (cannon) | step 5 (breath) | on a slope that rises to the left (values 27, 28) |
| 2 | step 2 (breath) | step 2 | flat floor |
| 3 | step 3 (cannon) | step 3 | flat floor |
| 4 | step 2 | step 2 | carried by a falling ice block (no record; see "The carried wolf") |
| 5 | step 3 | step 3 | as 4, with the cannon |
| ≥ 6 | – | – | no step is set: the object runs step 0 (`HitReactionDispatch`). No record uses it [likely] |

Ids 4 and 5 also get no terrain box and no gravity.

## State machine

States (table `0x800F6A84`): 0 `0x800E9B30` set-up, 1 `0x800E9C84` main, 2 and 3 `0x800EB608`
(ids 4, 5: `DeleteObject3`; else state 2 `DeleteObject`, state 3 `DeleteObject2`). The step table
`0x800F6A94` follows at once: 0 `HitReactionDispatch`, 1 nothing (`0x800EB600`), 2 BREATH-FLAT,
3 CANNON-FLAT, 4 BREATH-SLOPE-R, 5 BREATH-SLOPE-L, 6 CANNON-SLOPE-R, 7 CANNON-SLOPE-L.

The update function (`0x800EB5A0`) saves `prev_x`/`prev_y`, clears `display`, runs the state, then
always calls `CollideCheck`.

### Main (state 1) [certain]

1. `ContactWeapon` < 0 (killed): death with drop (see Death), state 2.
2. Squeeze: `collide_flags` has floor and ceiling (`& 0x0C` = `0x0C`) or both walls (`& 3` = 3):
   one explosion, 5 debris, **no drop**, state 2.
3. `ContactMega`.
4. Ids 4 and 5 (carried): `DisplayCheck(64, 48)` only (no off-screen delete); then the carrier
   test (see "The carried wolf"). Other ids: off screen 64 (x) / 48 (y) → state 3; else visible
   64 / 48.
5. `MoveObject2`, `AnimeTick`, then the step.

A step that ends in 1, 2 or 3 skips the move and the step in that update.

### The step timer

All six steps add 1 to the halfword `+0x7C` each update before the sub-step switch. Sub-step 0
sets it to 0. "Every 200 updates" is the test `timer % 200 == 0`.

### BREATH-FLAT (step 2; ids 2 and 4)

It faces the player **every update** (`flip` = right when x < player x; equal x → left).

| sub-step | time | what |
|---|---|---|
| 0 | 1 update | animation 0 (stand); timer 0 |
| 1 | until a test passes | player within **128 px in x** (`|dx| ≤ 128`; no y test) → BREATH (below). Else, at timer 200 → animation 1 (howl), sub-step 2 |
| 2 | about 30 updates | in range → BREATH. Else, when the howl reaches its last frame (`anime.next` = 0) → sub-step 0 |
| 3 | 90 updates | the breath: at timer 90, animation 0, timer 0, sub-step 4 |
| 4 | 40 updates | rest; at timer 40 → sub-step 0 |

BREATH: one breath object (visual type 5, `var` 0, `+0x7C` = the wolf), animation 2 (crouch, open
mouth; frames 2–3 loop), timer 0, sub-step 3.

So, while the player stays in range, a breath starts every **132 updates (2.2 s)**: 90 + 40 + 1
(sub-step 0) + 1 (sub-step 1) [certain].

### CANNON-FLAT (step 3; ids 3 and 5)

As BREATH-FLAT, with these changes [certain]:

- range **180 px in x** (`|dx| ≤ 180`);
- animations 3 (stand), 4 (howl), 5 (fire: recoil, 24 updates, hold);
- sub-step 3: at timer **12** one cannon ball (visual type 5, `var` 2); at timer 90 → sub-step 0.
  There is no rest sub-step.

So the wolf fires one ball every **92 updates (1.53 s)** while the player is in range, 12 updates
after the fire animation starts [certain].

### BREATH-SLOPE (steps 4 and 5; Zero, ids 0 and 1)

As BREATH-FLAT, with these changes [certain]:

- range **96 px in x**;
- the wolf faces the player **only** when a sub-step starts an animation (sub-step 0, the howl,
  the breath, the end of the breath), not every update;
- the breath object is `var` 1 (it does not follow the wolf);
- the animation depends on the step and on the facing. The sprites are drawn tilted for the slope;
  the "right" set is the mirror image of the other set:

  | step | facing left | facing right |
  |---|---|---|
  | 4 (id 0) | 6 stand, 7 howl, 8 breath | 24, 25, 26 |
  | 5 (id 1) | 24, 25, 26 | 6, 7, 8 |

### CANNON-SLOPE (steps 6 and 7; X, ids 0 and 1)

As CANNON-FLAT (range 180, ball at timer 12, sub-step 0 at timer 90), with the facing rule of
BREATH-SLOPE and these animations [certain]:

| step | facing left | facing right |
|---|---|---|
| 6 (id 0) | 9 stand, 10 howl, 11 fire | 27, 28, 29 |
| 7 (id 1) | 27, 28, 29 | 9, 10, 11 |

### The carried wolf (ids 4 and 5)

Stage object 7 with `var` 12 (a falling ice block) makes a wolf at its own position in **one case
of two** (`GetRNG & 1` = 0): `GetMainObject`, flags `0x41`, type 6, `var` = 4 + (`GetRNG & 1`),
`+0x80` = the block. Then the block falls at 2 px/frame. Each update, while `var` = 4 or 5
[certain for the wolf code]:

- BREATH-FLAT / CANNON-FLAT first set y = block y − 39 (only while `act4` = 0). When the block is in
  its sub-step 6 (falling) and y ≥ **1313**, the wolf gets its terrain box `0x800F6720`. Gravity
  stays 0.
- In state 1: block state 4 (the block breaks) and `act4` = 0 → the wolf dies with the drop.
  Block state 2 → `act4` = 1 and `var` −= 2: from then on it is a normal flat wolf (ids 2, 3) that
  stays where it is.

The blocks come from stage object 7 `var` 0 (records 58 and 59 of section 1), which starts when the
player is in the lower room (x ≥ 5137 and y ≥ 1473, or x ≥ 5345 and y ≥ 1489) [likely: read in
part]. What the block's states 2 and 4 mean exactly is in that object's code [guess: 2 = it
stopped, 4 = it broke].

## Animations (table `0x800F6A00`, 33 animations)

| # | what | frames |
|---|---|---|
| 0 | stand, flat (breath wolf) | sprite 0, hold |
| 1 | howl: head up, shakes | 13 frames, 8 then 2 updates each (32), hold |
| 2 | breath: crouch, mouth open | 4 frames (5 4 2 2), loops to frame 2 |
| 3 | stand, flat (cannon wolf) | sprite 7, hold |
| 4 | howl (cannon wolf) | as 1 |
| 5 | fire: the back cannon recoils | 4 frames, 3 9 2 10 (24 updates), hold |
| 6–8 | breath wolf on a slope that rises to the right (stand, howl, breath) | as 0–2 |
| 9–11 | cannon wolf on that slope (stand, howl, fire) | as 3–5 |
| 12 | breath cloud grows (flat) | 9 frames, 25 updates, hold |
| 13 | breath cloud (flat) | 8 frames of 3 updates, loop |
| 14 | breath cloud fades (flat) | 3 frames, 15 updates, hold |
| 15–17 | breath cloud on a slope, aimed down-left: grow, loop, fade | as 12–14 |
| 18 | cannon ball | 6 frames (5 4 4 5 4 4), loop |
| 19–23 | debris: head, body, leg, tail, part | one sprite each, hold |
| 24–26 | breath wolf on a slope that rises to the left | as 0–2 |
| 27–29 | cannon wolf on that slope | as 3–5 |
| 30–32 | breath cloud on a slope, aimed up-left: grow, loop, fade | as 12–14 |

All 33 animations are used by this code [certain]. No animation has an event flag, and the code
tests none; it tests only `anime.next` = 0 (the last frame of a hold animation).

## Shots and effects (visual type 5, dispatcher `0x800EB670`)

The dispatcher clears `display`, calls `CornerCheck` when `+0x84` = 0 and `collide_flags` = 0, and
runs the state table of the `var`: `var` 0 `0x800F6ACC`, `var` 1 `0x800F6AD8`, `var` 2
`0x800F6AE4`. Each one copies the parent (`docs/engine.md`): animation table, texture, sprite data,
palette without the flash bit, layer. Draw priority 4.

### Breath, flat (`var` 0: `0x800EB750`, `0x800EAF68`) [certain]

- HP 1, contact damage **4**; no hurt box, no damage table (it cannot be shot). Sound (2, 16).
- Each update it follows the wolf: `flip` = the wolf's; x = wolf x − 45 (facing left) or + 45;
  y = wolf y + 3.
- A counter (`+0x06`) counts the updates from the spawn.
  - Animation 12 (the cloud grows, 25 updates), no attack box.
  - At the end of animation 12: animation 13, attack box `0x800F6AB4` = (−18, −10, 32, 20).
  - At counter 90: animation 14 (fades), attack box null; deleted at the end of it.
- So the cloud hurts from about update 26 to update 90 of the 90-update breath. In world space
  it covers x from wolf x − 63 to − 31 (facing left; mirrored facing right) and y from wolf y − 7
  to + 13.
- It is deleted when the wolf is in state 2 (dead), or when it is 32 px off screen (not for a
  carried wolf).

### Breath, slope (`var` 1: `0x800EB110`, `0x800EB29C`) [certain]

- As `var` 0, but the position and the facing are set once, at the spawn; it does not follow.
- Wolf id 0 facing left, or id 1 facing right: (x ∓ 41, y + 6), animations 15–17, box
  `0x800F6AB8` = (−18, −8, 32, 20). Wolf id 0 facing right, or id 1 facing left: (x ± 36, y − 17),
  animations 30–32, box `0x800F6ABC` = (−18, −12, 32, 20). So the cloud points down the slope or up
  the slope.
- `ContactMega` runs only in the loop part (between the end of the grow animation and counter 90).

### Cannon ball (`var` 2: `0x800EB3C8`, `0x800EB860`) [certain]

- **HP 2**, contact damage **3**, `dmg_tbl_normal`: one lemon (3) destroys it. Sound (2, 15).
- Boxes: terrain `0x800F6AC0` = (0, 0, 4, 4) (8 × 8 px), attack `0x800F6AC4` and hurt `0x800F6AC8`
  = (−4, −4, 8, 8). Animation 18. `flip` = the wolf's.
- Start position (wolf x, wolf y):

  | wolf step | facing left | facing right |
  |---|---|---|
  | 3 (flat) | x − 12, y − 12 | x + 12, y − 12 |
  | 6 | x − 16, y − 12 | x, y − 14 |
  | 7 | x, y − 14 | x + 16, y − 12 |

- Motion: `speed_y` **3 px/frame up**, gravity **0.094 px/frame²** (`0x1800`), `speed_x` =
  (player x − ball x) / **70** px/frame, fixed at the spawn. So the ball rises **48 px**, and after
  70 updates it is at the player's x of the spawn moment, about 16 px below its start height (near
  the body of a player who stands at the wolf's level).
- Each update: every 8 updates one smoke puff (effect type 9) at a random point of its terrain box
  (`0x8004A2B4`) [likely for "smoke"]; terrain (`collide_flags` from `CornerCheck`), a weapon kill
  or a hit on the player → one explosion and state 2; 16 px off screen → deleted.
- `CornerCheck` does not see the ice slope values 17–28 as solid (`docs/engine.md`, "Tile
  collision"), so a ball passes through the slope surface and ends in the solid cells below it
  [likely].

## Damage

- Taken: `dmg_tbl_normal`, 12 HP: 4 lemons, 2 half charges, 1 full charge, 3 sabre hits. No hit
  reaction (no step change); the hit flash is the engine's.
- Dealt: body 4, breath 4, cannon ball 3.

## Death

One explosion (`SpawnExplosion`), 5 debris (`SpawnJunk(5, {19, 20, 21, 22, 23})`), drop table
**17**, state 2. A squeeze has no drop. A carried wolf also dies (with the drop) when its block
breaks.

## Sounds

| bank, id | when |
|---|---|
| 2, 16 | a breath starts (both breath objects) |
| 2, 15 | a cannon ball starts |

The wolf itself plays no sound (no howl sound in this code).

## Summary for a reimplementation

- Static enemy, 32 × 32 px, HP 12, contact 4. It never moves; gravity keeps it on the floor or
  slope.
- Faces the player. Out of range: stand, and a 30-update howl every 200 updates.
- Breath wolf: player within 128 px in x (96 on a slope) → 90-update breath (a 32 × 20 px cloud
  45 px in front, damage 4, hurts from update 26 to 90), 40-update rest; one breath per 132
  updates.
- Cannon wolf: player within 180 px in x → recoil, ball after 12 updates; one ball per 92
  updates. The ball: 3 px/frame up, 0.094 px/frame² down, dx / 70 px/frame across, HP 2, damage
  3, ends on terrain or the player.
- On the ice slopes the weapon is the cannon for X and the breath for Zero.
- Death: explosion, 5 debris, drop table 17.

## Open questions

1. The kit run with `--type 6` walked only the visual type 5 code: it did not follow the state
   table `0x800F6A84` that the entry loads (the state table and the step table are back to back),
   and it showed the `var` 1 and `var` 2 tables of the dispatcher as one 6-entry table. The wolf
   functions were read with `--at`.
2. The carrier: stage object 7 (falling ice blocks and their generator) has no sheet. The meaning
   of its states 2 and 4 for the wolf is a guess. A released wolf keeps gravity 0; whether it can
   stay in the air when its block is gone is not checked.
3. Ids ≥ 6 run step 0. No record uses them in section 1.
4. `0x8004A2B4` (smoke puffs, effect type 9) is not named; the effect is a guess (shared with the
   Rescue Reploid bomb).
5. The official name is from a search summary of the Mega Man Knowledge Base, not from the page or
   the game text.

## Records (section 1, member 96, area 0) [certain]

Kind 0 spawns on all difficulties, 1 from Normal, 2 from Hard (`docs/engine.md`, "Difficulty
filter"). The floor value is the collision value of the cell under the feet.

| record | kind | id | x, y | floor |
|---|---|---|---|---|
| 29 | 1 | 0 | 768, 1088 | 25 (ice slope) |
| 30 | 2 | 3 | 968, 1008 | 33 |
| 31 | 1 | 0 | 1312, 936 | 26 |
| 32 | 2 | 0 | 1600, 784 | 26 |
| 33 | 1 | 0 | 1824, 720 | 25 |
| 34 | 2 | 0 | 2160, 552 | 25 |
| 35 | 2 | 0 | 2416, 503 | 25 |
| 36 | 1 | 0 | 2656, 456 | 25 |
| 37 | 0 | 3 | 3496, 336 | 60 |
| 38 | 2 | 3 | 3640, 304 | 60 |
| 39 | 1 | 3 | 3928, 400 | 60 |
| 40 | 0 | 1 | 4832, 688 | 28 |
| 41 | 1 | 1 | 4912, 1142 | 28 |
| 42 | 0 | 1 | 4768, 1100 | 27 |
