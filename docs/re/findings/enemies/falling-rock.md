# Falling rock (stage 08, main object type 31): behaviour sheet

A sand-coloured rock that appears in the air, blinks for one second, and then acts in one of
three ways, picked at random at each spawn: it drops and breaks on the floor; or it is a round
boulder that drops, rolls down slopes or toward the player and breaks after three landings or a
second against a wall; or it waits and then flies straight at the player. It has 8 HP and does 2
contact damage. Half of the records make nothing (a coin roll at each spawn). Ground Scaravich's
stage has 279 records of this type, the most of any type. "Falling rock" is a descriptive name; no
official name was found.

Overlay `08` (stage 5, area 0). Entry `0x800EF6B8`, state table `0x800F5D48`, step table
`0x800F5D54`. No save state has a live rock [certain for the four states]: all facts are from the
code and the record flags. Kit: `python3 -m mmxdemake.rekit --state scaravich-3_section.state.gz
--type 31`. Units, tags and shared rules: `docs/engine.md`. Records: `docs/stages/scaravich.md`.
Overlay 23 (Gate stage 2, stage 17) has the same 10 functions after masking (entry `0x800EA3A0`,
set-up `0x800E9E24`), sprite type 133, and 46 records (`docs/stages/gate-2.md`); one rock is live
in `gate-3_stage-2` (`var` 2, step 0) [certain].

## Spawn and set-up (state 0, `0x800EF13C`) [certain]

1. **Coin roll:** `GetRNG() & 1` = 0 → `DeleteObject3`. The record keeps its "spawned" bit, so it
   does not spawn again until the records are reset (death or stage start) [certain for the code].
   In `scaravich-3_section`, 18 of the 25 rock records of section 2 have flags `0x01` and no live
   object: these are coin losses [certain for the RAM; the cause is from the code]. So each time a
   record enters the spawn band it has a 1/2 chance, and after the first loss it stays off.
2. **Kind:** when `var` = 0 (all 279 records have `id` 0): `var` = `GetRNG() % 3` (0: 86/256, 1 and
   2: 85/256 each).
3. Flags `0x41`, state 1, step 2, `step2` 0, layer = `mega.layer`. Sprite type **133** (bank 5, the
   same bank as the rock blocks, stage object 6). Priority 33. Animation table `0x800F5CBC`.
4. **HP 8**, `dmg_tbl_normal`, contact damage **2**, `iframes` 0. Terrain box `0x800F5D28` = (0, 0,
   13, 13): 26 × 26 px. **No attack box and no hurt box yet** (set in step 0).
5. All speeds, `accel_x` and gravity 0. Animation 0 (`var` 0 and 2: the square rock, sprite 0) or 1
   (`var` 1: the rolling boulder, sprites 1–8).

## The `id` parameter

The record `id` is 0 in all records; the kind comes from the RNG (above). An `id` of 1 or 2 would fix
the kind [certain for the code; no record does it].

## State machine [certain]

States: 0 set-up, 1 main `0x800EF340`, 2 death `0x800EF704` (sound 2/84, `DeleteObject`). Steps
(`step2`, table `0x800F5D54`): 0 `0x800EF740` appear, 1 `0x800EF7B4` drop, 2 `0x800EF534`
boulder fall, 3 `0x800EF85C` boulder roll, 4 `0x800EF960` aim, 5 `0x800EFA10` fly.

### Main (state 1), in this order

1. Keep the old HP; `ContactWeapon`. A kill: `var` 1 → 2 debris pieces (animations 6, 7 or 8, 9 at
   random); `var` 0 or 2 → 2 debris (2, 3 or 4, 5); `SpawnExplosion`; `DropItem(table 18)`; state 2.
2. `ContactMega`; hit flash when the HP changed.
3. The step; `AnimeTick`.
4. `OffScreenCheck2(48, 48)` → `DeleteObject2`. Else: in step 0, `display` is toggled each update
   (the blink); in the other steps `DisplayCheck(48, 48)`.
5. `latch` (`+0x67`): 0 on the floor, else ±1 by the sign of `speed_y`. `CollideCheck`.

### Step 0: appear (`0x800EF740`)

Count `+0x88` up. After 60 updates (at 61): hurt box `0x800F5D08` = (−13, −13, 26, 26), attack box
`0x800F5D18` = (−11, −11, 22, 22). Then `var` 0 → step 1, gravity `0x2000` (0.125 px/frame²); `var` 1
→ step 2, the same gravity; `var` 2 → step 4. So for one second the rock blinks in place, cannot
hurt and cannot be hit.

### Step 1: drop (`var` 0, `0x800EF7B4`)

`MoveObject2`; `speed_y` is limited to 2 px/frame down. On the floor: sound 2/82, `SpawnExplosion`,
2 debris (2, 3 or 4, 5), state 2. So it breaks where it lands. No item roll on this break.

### Steps 2 and 3: boulder (`var` 1)

- **Fall** (step 2, `0x800EF534`): `MoveObject2`, fall limit 2 px/frame. On the floor: sound 2/82,
  `speed_y` 0, wall timer `+0x80` = 0, landings `+0x89` += 1. Then the tile value 24 px below the
  centre (`TerrainPointCheck(x, y + 24)`, switch `0x800E9900`):

  | value | slope | move |
  |---|---|---|
  | 1–4, 9, 10, 17–20, 25, 26 | rises to the right | `speed_x` −1.5 px/frame (to the left), `flip` `0x40` |
  | 5–8, 11, 12, 21–24, 27, 28 | falls to the right | +1.5 px/frame (to the right), `flip` 0 |
  | any other (flat floor) | – | 1.5 px/frame toward the player; `flip` toward the player |

  Then step 3. When the landing count is 3 or more: `SpawnExplosion`, 2 debris (6, 7 or 8, 9),
  state 2: **the boulder breaks on its third landing**.
- **Roll** (step 3, `0x800EF85C`): `MoveObject2`; sound 2/83 when `frame_count & 31` = 0. Not on
  the floor → step 2 (falls again, gravity 0.125, the next landing counts). On the floor: speed_y
  and gravity 0. Against a wall (`collide_flags` 1 or 2): `+0x80` += 1; at **60** updates against a
  wall (not reset until the next landing) → explosion, debris, state 2.
- The roll direction is fixed at each landing; the boulder does not turn on the floor.

### Steps 4 and 5: thrown rock (`var` 2)

- **Aim** (step 4, `0x800EF960`): wait 20 updates (X) or 40 updates (Zero, `mega+0x02` ≠ 0). Then
  aim at the player: speed **2 px/frame** on the line to the player (`ratan2`, `rcos` and `rsin` ×
  32), step 5.
- **Fly** (step 5, `0x800EFA10`): `MoveObject2` with no gravity. At any terrain contact
  (`collide_flags` ≠ 0): explosion, 2 debris (2, 3 or 4, 5), state 2.

## Animations (table `0x800F5CBC`, 19 entries) [certain for the data]

| # | sprites | use |
|---|---|---|
| 0 | 0 (1 frame, held) | square rock (`var` 0, 2) |
| 1 | 1 2 3 4 5 6 7 8 (6 frames each, loop) | rolling boulder (`var` 1) |
| 2–9 | 9–16 (one each) | debris pieces |
| 10–18 | 17–25 (one each) | not used by this code (the rock-block sprites and their debris) |

The kit cut the table at animation 1 (sprite 8); the table at `0x800F5CBC` has 19 pointers. Sprites:
`~/mmx-data/x6/sprites/banks/scaravich/scaravich-3_section/bank-5.png`. The `spriteexport` folder
`enemy-bf38650355` ("type 31") is a false match: it is drawn from bank 7 (the list entry of type 31),
but the code draws with sprite type 133 (bank 5) [certain for the code].

## Damage

- Taken: `dmg_tbl_normal`, HP 8: X 3 lemons (3 each), 2 half charges (7), 1 full charge; Zero 2
  sabre hits (4–6). Only after the 60-update blink.
- Dealt: contact 2 (before the difficulty scaling). There is no shot.

## Death

- Killed by a weapon: 2 debris, 1 explosion, `DropItem(table 18)`, then state 2: sound 2/84 and
  `DeleteObject` (the record can spawn again, with a new coin roll).
- Breaks by itself (landing, third landing, wall, flight end): explosion and 2 debris, **no item
  roll**, then the same state 2.

## Sounds [certain for the numbers]

| bank, id | when |
|---|---|
| 2, 82 | lands (drop rock, boulder) |
| 2, 83 | boulder rolls (every 32 frames) |
| 2, 84 | removed after any death or break |

## Summary for a reimplementation

1. A 26 × 26 px rock, 8 HP, contact 2. At each spawn: 50 % nothing (and the record stays off until
   a reset), else one of three kinds at random.
2. It blinks at its record point for 60 frames, harmless and not hittable.
3. Kind 0: falls (gravity 0.125, max 2 px/frame) and breaks on the floor.
4. Kind 1: a boulder that falls, then rolls at 1.5 px/frame down the slope it lands on, or toward
   the player on flat floor; it falls off ledges and breaks on its third landing or after 60 frames
   against a wall.
5. Kind 2: waits 20 frames (X) or 40 (Zero), then flies at 2 px/frame straight at where the player
   was, and breaks on the first terrain contact.
6. A weapon kill rolls drop table 18; a self-break does not.

## Open questions

1. Where the records are on the map (ceilings, rows of pairs 32 px apart), and whether the player
   sees them fall from the ceiling or appear in the open.
2. Whether the teleport into a section resets the record flags (then the coin losses come back).
3. The sound names of 2/82, 2/83, 2/84 in this stage.
4. The official name.
