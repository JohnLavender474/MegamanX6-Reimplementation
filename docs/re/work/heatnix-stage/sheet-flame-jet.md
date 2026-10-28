# Flame jet (stage 05, main object type 36): behaviour sheet

The flame jet is a fire nozzle in the walls of the first tower of Blaze Heatnix's stage. It shoots a
column of fire up or down, holds it, lets it die out, waits and fires again. The fire hurts the player
on contact (16 damage). It cannot be hit or destroyed. "Flame jet" is a descriptive name; no official
name was looked up (it is a stage hazard, not an entry of the enemy list) [guess for the name].

Overlay `05` (Blaze Heatnix, stage 3), code at `0x800E98C0`–`0x800EA07C`. Save state
`heatnix-1_start` (stage 3, area 0, Zero, Normal). No flame jet is live in the state; the animation
table was found by the code search. Kit: `python3 -m mmxdemake.rekit --name flame-jet --state
heatnix-1_start.state.gz --type 36`. Export: `flame-jet` (`enemy-7f9bbc9a47`). Units, tags and the
shared rules: `docs/engine.md`. Records: `docs/stages/heatnix.md`.

## Spawn and set-up (state 0, `0x800E9CF8`) [certain]

- `flags` |= `0x10`, `display` 0, layer = `mega.layer`, priority 7, `flip` 0 (also for the
  down jets: the down flame has its own sprites).
- **HP 12**, damage table `dmg_tbl_normal`, **contact damage 16**, `iframes` 0, `latch` 0.
- **No hurt box** (`+0x54` = 0 at all times) and no terrain box: no gravity, no tile collision.
- The attack box is the object's own 4 bytes at `+0x80`: x offset −8, width 16; the y offset and
  the height follow the sprite (below). `+0x84` (the "put out" flag) = 0.
- State 1, step 2 (FIRE), step2 0. Speeds 0.

## The `id` parameter [certain]

`var` = record `id`.

| bits | meaning |
|---|---|
| `0xF0` = 0 | the fire goes **up** from the point (animations 0, 1) |
| `0xF0` ≠ 0 | the fire goes **down** from the point (animations 2, 3) |
| `0x0F` = 0 | one long flame: held for 180 updates |
| `0x0F` = 1–7 | that many short bursts, each held for 14 updates |
| `0x0F` ≥ 8 | 1–4 short bursts, a new random count (`GetRNG() & 3` + 1) at each start of step 2 |

Records of this stage use `id` 0, 4, 16, 20 and 24 (`docs/stages/heatnix.md`).

## State machine [certain]

States (`0x800F3CEC`): 0 set-up, 1 main (`0x800E98C0`), 2 delete (`0x800E9DC0`, `DeleteObject2`).
Steps (`0x800F3CF8`): 0 `HitReactionDispatch` (standard; never reached here, no hit), 1 nothing,
2 FIRE (`0x800E9EAC`, sub-steps `0x800F3D08`), 3 PAUSE (`0x800E9DE8`, sub-steps `0x800F3D18`).

**Main** (state 1), in order:

1. `prev_x`, `prev_y` = x, y; `AnimeTick`; the step.
2. Attack box: none while the sprite is 13 or 27 (the empty end sprites), else `+0x80`.
3. `ContactWeapon`. It cannot hit (no hurt box; `WeaponBoxTest` returns 0 for a null box). The code
   after it sets `+0x84` = 1 on a hit by weapon id 1 or 54, or on a kill; this path is dead for this
   object unless other code writes `+0x84` (none found).
4. `ContactMega` (contact damage 16).
5. `OffScreenCheck2(160, 160)`: outside → state 2 (`DeleteObject2`; the record can spawn again).
   Else `display` 0 on sprites 13 and 27, else `DisplayCheck(48, 48)`.

### FIRE (step 2)

| sub | address | what |
|---|---|---|
| 0 | `0x800E9EE8` | `PlaySound(2, 22)`; animation 0 (up) or 2 (down); `+0x7E` = 0; burst count `+0x85` and hold time `+0x7C` from the `id` (table above: 1 and 180, n and 14, or random 1–4 and 14); sub 1 |
| 1 | `0x800E9FD4` | wait for the animation flag 1 (the flame is full); sub 2 |
| 2 | `0x800E9A04` | hold: `+0x7C` − 1 each update; at 0, animation 1 (up) or 3 (down), the flame dies out; sub 3. When `+0x84` is set: die out at once, burst count 1, `+0x7E` = 200 |
| 3 | `0x800E9B6C` | wait for the animation flag 1 (the flame is gone). Then burst count − 1: at 0, step 3, sub 0. Else `PlaySound(2, 22)`, animation 0 or 2 again, hold time 180 (`id & 0x0F` = 0) or 14; sub 1 |

Subs 1–3 also update the box each update: height h = `0x800F3CD0`[sprite]; up: y offset −h (the
box is from y − h to y); down: y offset 0 (from y to y + h).

| sprite | 0/14 | 1/15 | 2/16 | 3–7/17–21 | 8/22 | 9/23 | 10/24 | 11, 12/25, 26 | 13/27 |
|---|---|---|---|---|---|---|---|---|---|
| box height (px) | 16 | 40 | 56 | 72, 88, 88, 88, 88 | 72 | 56 | 24 | 8 | 1 (no box) |

So a full flame is a 16 × 88 px box.

### PAUSE (step 3)

| sub | address | what |
|---|---|---|
| 0 | `0x800E9E24` | animation 4 (sprite 13, empty); wait time `+0x7C` = `+0x7E` + 60; `+0x7E` = 0; HP = 12; `+0x84` = 0; sub 1 |
| 1 | `0x800E9E80` | `+0x7C` − 1; at 0: step 2, sub 0 |

### Timing [certain for the code; one update per frame is likely]

- Flame up: 16 updates (animation 0 to its flag frame), then the hold, then 24 updates (animation
  1 to its flag frame).
- One short burst: 16 + 14 + 24 = 54 updates. One long flame: 16 + 180 + 24 = 220 updates.
- After the last burst: 60 updates of pause (260 after a "put out"), then the next cycle.
- Examples: `id` 0 = 220 + 60 = 280 updates per cycle; `id` 4 or 20 = 4 × 54 + 60 = 276 updates.

## Animations

Table `0x800F3CBC` (5 animations; frames {ticks, flag, next, sprite}):

| # | what | sprites | ticks | flag 1 at | loop |
|---|---|---|---|---|---|
| 0 | up flame grows, then burns | 13 0 1 2 3 3 4 5 6 7 | 1 4 4 4 3 1 4 4 4 4 | frame 5 | to frame 6 |
| 1 | up flame dies out | 8 9 10 11 12 13 13 | 4 × 6, 1 | frame 6 | hold |
| 2 | down flame grows, then burns | 14 15 16 17 17 18 19 20 21 | 4 4 4 3 1 4 4 4 4 | frame 4 | to frame 5 |
| 3 | down flame dies out | 22 23 24 25 26 27 27 | 4 × 6, 1 | frame 6 | hold |
| 4 | off | 13 | 1 | – | hold |

Sprite bank 3 (type 36, 28 sprites). Note: animation 2 reaches its flag after 15 updates, one
update before animation 0 (16).

## Damage

- To the player: contact 16 before the difficulty scaling (`docs/engine.md`, "Damage to the
  player"). A full burn hits a player in a 16 × 88 px column.
- To the object: none. `dmg_tbl_normal` and HP 12 are set but unused (no hurt box).

## Death

None: no code path destroys it. It is deleted only off screen (state 2) and comes back with its
record.

## Sounds

| bank, id | when |
|---|---|
| 2, 22 | each start of a flame (sub 0 and each new burst in sub 3) |

## Summary for a reimplementation

- A fixed fire nozzle; `id` bits 4–7: 0 = up, else down; bits 0–3: 0 = one 180-frame flame, 1–7 =
  that many 14-frame bursts, 8–15 = 1–4 random bursts.
- Flame: grow 16 frames, hold, die 24 frames; 60 frames off between cycles.
- Hurt box: none. Attack box 16 px wide, height by sprite (up to 88 px), above (up) or below (down)
  the point. Contact damage 16.
- Sound (2, 22) at each flame start.
- Deleted when 160 px outside the view; respawns from its record.

## Open questions

1. What writes `+0x84` ("put out": the flame stops and the pause is 200 updates longer). The own
   code writes it only after a weapon hit, which the null hurt box makes impossible. Maybe a
   removed feature, or another object (not searched in the whole overlay).
2. The official name, if any.
3. The look in play (up or down nozzle, fire colour): the export `enemy-7f9bbc9a47` was drawn from
   the bank of the start state; no flame jet was live, so the palette is the bank palette [likely
   correct].
