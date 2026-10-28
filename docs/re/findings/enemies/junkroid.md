# Junkroid (stage 02, main object type 1): behaviour sheet

Junkroid is the green, legless, half-broken robot that sits on the floor in the intro stage and shoots
a single slow shot. Its shot is a separate visual object (type 0 in the intro visual table).

Overlay `02` (intro). Read by hand from the disassembly (before the RE kit). Units, confidence tags
and the shared engine rules: `docs/engine.md`. Records (25–30): `docs/stages/intro.md`.

## Spawn and set-up [certain]

- HP 12, contact damage 4, draw priority 6. All speeds 0; latch 0; collision flags 0.
- Faces the player once (equal x → right).
- Animation 0 (idle, sprite 0). Shot timer = 1. State MAIN, sub-state IDLE.
- If difficulty ≥ 2 (Hard), `id` is forced to 1.

## The `id` parameter [certain]

- `id = 1`: active. Junkroid turns to face the player and shoots.
- `id = 0`: dormant. It faces the player once at spawn, then never turns and never shoots. It can still
  fall, hurt the player by contact, take damage and die. On Hard, `id` becomes 1.

## Geometry [certain for the numbers; likely for the reading]

| Box | Data | Area | In blocks |
|---|---|---|---|
| Hurt box and contact box | (−12, −20, 24, 40) | X − 12 … X + 12, Y − 20 … Y + 20 | 1.5 × 2.5 |
| Terrain box | centre (0, +16), half size 16 × 8 | X − 16 … X + 16, Y + 8 … Y + 24 | 2 × 1 |

So the origin is 24 px (1.5 blocks) above the feet, and the hurt box ends 4 px above the floor.

## State machine

Standard states (`docs/engine.md`): 0 INIT `0x800EAFFC`, 1 MAIN `0x800EAC8C`, 2 and 3 `0x800EB10C`
(delete; the two delete variants). `CollideCheck` after the state.

MAIN, in this order, every update [certain]:

1. Save the previous position.
2. Run the sub-state (IDLE, SHOOT or FALL).
3. Facing: if `id != 0` and the current sprite is 0 (the idle pose), face the player. Junkroid does not
   turn during the shoot animation.
4. Fall check (standard): gravity `0x4200` (0.258 px/frame²), horizontal acceleration 0.
5. Weapon test. Killed: death with both drop rolls (see Death), then KILLED.
6. Else, squeezed (floor + ceiling, or both walls): death without the second drop roll, then KILLED.
7. Else: contact test. Off screen 64 → OFFSCREEN; visible 32.

### IDLE (sub-state 1, `0x800EB14C`)

- If `id == 0`: do nothing.
- If `|player.x − junkroid.x| < 112` px (7 blocks): go to SHOOT. There is **no** vertical range
  check: Junkroid shoots at a player above or below it.

### SHOOT (sub-state 2, `0x800EAE0C`)

- If `|dx| ≥ 113` px: animation 0, shot timer = 120, go to IDLE (1 px hysteresis: enter at < 112, leave
  at > 112). The animation is not ticked on this update.
- Shot timer: if it is not 0, decrement it; when it becomes 0, start animation 1 (shoot).
- Event flag 1 in the current frame: play the shot sound, then make the shot.
- Event flag 2: animation 0, shot timer = 120, go to IDLE.
- Tick the animation.

### FALL (sub-state 3, `0x800EB1A0`)

- On the floor: go to IDLE, clear the speeds and the latch.
- Else `MoveObject2` (gravity 0.258 px/frame², fall speed limit 6.5 px/frame).
- Landing does not reset the shot timer.

## Shoot animation and timing

Animation 1 frames [certain]:

| frame | duration | flag | sprite |
|---|---|---|---|
| 0 | 6 | – | 1 |
| 1 | 4 | – | 2 |
| 2 | 1 | **1 = fire** | 3 |
| 3 | 15 | – | 3 |
| 4 | 6 | – | 2 |
| 5 | 6 | **2 = end** | 1 |

From the code order (event check before the tick) [certain]:

- The shot comes out 10 updates after the animation starts; Junkroid is back in IDLE after 32.
- **Shot interval while the player stays in range: 153 updates ≈ 2.55 s.**
- **First shot:** the init timer is 1, so the first shot comes out 11 updates (≈ 0.18 s) after the
  player enters the range.
- When the player leaves the range, the timer is set to 120. When the player comes back, the next
  animation starts after about 121 updates (≈ 2.0 s). If the player leaves before the fire frame, the
  shot is cancelled.

Other animations [certain]: 0 = idle (sprite 0); 2, 3, 4, 5 = sprites 8, 9, 10, 7 (the 4 debris
pieces); 6 = shot (sprite 4); 7 = shot on Hard (sprites 5, 6, 7, 2 updates each, loop).

## The shot (visual object type 0)

Made with `GetVisualObject`; copies the parent (`docs/engine.md`) [certain].

Set-up (`0x800EB23C`) [certain]:

- Speed ±2 px/frame (`0x20000`) in the facing direction at the fire frame. No vertical speed, no
  gravity.
- Position: X ± 16 px (forward), Y − 12 px: 36 px (2.25 blocks) above Junkroid's floor.
- Contact box and hurt box (−4, −4, 8, 8). Terrain box half size 6 × 6.
- HP 1, contact damage **1**, draw priority 4.
- Easy and Normal: animation 6, `dmg_tbl_minor`: any damaging shot destroys it.
- Hard: animation 7, `dmg_tbl_deflect`: the player's shots bounce off and it cannot be shot down
  [likely, from `0x800319EC`].

Main (`0x800EB340`), every update [certain]: animation tick; `x += speed_x`; then the standard shot
ends (`docs/engine.md`): the player (explosion), a weapon kill (explosion), terrain (explosion), 32 px
off screen (no effect). Visible within 16 px. No lifetime limit.

## Damage

- Taken: `dmg_tbl_normal`, 12 HP: 4 lemons, 2 half charges, 1 full charge, 3 sabre hits. Ids 21 and 23
  do 1 (12 hits), id 22 does 32. No invincibility; 1-update flash; no knock-back [certain].
- Dealt: body 4, shot 1 (before the difficulty scaling).

## Death [certain]

When a weapon kills it:

1. Drop roll, table 21.
2. `SpawnExplosion` at Junkroid.
3. 4 debris pieces (`SpawnJunk`, animations 2, 3, 4, 5).
4. Second drop roll, table 14.
5. KILLED: deleted, the record can spawn it again.

When squeezed: steps 1–3 and 5 (no table-14 roll).

## Sounds [certain for the numbers]

| event | bank | id |
|---|---|---|
| fires (Easy and Normal) | 2 | 26 |
| fires (Hard) | 2 | 33 |
| player's shot deflected by the Hard shot | 0 | 6 |

## Summary for a reimplementation

- Static enemy, 12 HP, on the floor (falls with gravity if spawned in the air). Hurt box 1.5 × 2.5 blocks.
- Active variant: faces the player whenever it is in its idle pose. When `|dx| < 7 blocks` (any height),
  it fires one horizontal shot every 2.55 s, the first about 0.18 s after the player comes in range.
  Leaving the range resets the wait to 2.0 s.
- Shot: 0.5 × 0.5 block, 7.5 blocks/s, straight, spawns 1 block forward and 0.75 block up, 1 damage,
  bursts on walls and on the player, can be shot down (except on Hard).
- Contact damage 4.
- Death: explosion, 4 debris pieces, two drop rolls (tables 21 and 14).
