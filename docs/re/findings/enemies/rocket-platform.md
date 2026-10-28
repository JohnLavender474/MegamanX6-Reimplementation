# Rocket platform (stage 09, stage-object type 5): short sheet

The rocket platform is a tall metal block with a rocket flame under it. It floats over the sea and
the player can stand on it. It moves slowly to and fro, horizontally or vertically, and stops for a
moment at each end. It cannot hurt the player. There is no official name; "rocket platform" is the
name that the user gave the sprite bank.

Overlay `09` (Rainy Turtloid), code at `0x800ECD20`–`0x800ED00C` and `0x800ED014`–`0x800ED18C`, save
state `turtloid-1.state.gz` (stage 6, area 0). Written with the RE kit (`python3 -m mmxdemake.rekit
--state turtloid-1.state.gz --class stage --type 5 --name rocket-platform`) and `docs/re-guide.md`.
Units, confidence tags and the shared engine rules: `docs/engine.md`. Records:
`docs/stages/turtloid.md`. Overlay 23 (Gate stage 2, stage 17) has the same 5 functions after
masking (entry `0x800E9B8C`, sprite type 131) and 5 records (`docs/stages/gate-2.md`).

## Set-up (`0x800ECD20`) [certain]

- Sprite type 131 (bank 6 in this stage). Draw priority 7. It takes the player's layer.
- Terrain box `0x800F6090` = (0, 0, 14, 28): a **28 × 56 px** body. Hurt box `0x800F608C` =
  (−14, −28, 28, 56).
- The start position is kept in `work.7C` (x) and `work.7E` (y).
- The record `id` picks the motion:

| `id` | motion | start | animation |
|---|---|---|---|
| 0 | horizontal, start x ± 96 px | 1 px/frame toward the player's side (left when the player's x is smaller, else right) | 1 |
| ≠ 0 | vertical, start y ± 80 px | 1 px/frame up | 0 |

## Update (`0x800ECF40`) [certain for the code]

Each update:

1. The motion step (below), but not when `mega+0xE0` = 2. The meaning of that value is not known.
2. `AnimeTick`; `DisplayCheck` (48, 64).
3. `SolidObject`: the player is pushed out of the body and carried on top. **Skipped while
   `mega+0xE5` ≠ 0**, that is for 75 updates after the player is hurt (as for the Caterpirade): during
   that time the player goes through the platform [certain for the code; check in play].

Motion (`0x800ED014` horizontal, `0x800ED0B4` vertical):

- It moves 1 px per frame. When it reaches the end of its range (start ± 96 px horizontally, start
  ± 80 px vertically), the speed is reversed.
- At each reversal it **stops for 40 frames** (`work.84`), then moves the other way. The vertical
  platform sets animation 0 when it starts to move up and animation 1 when it starts to move down.
- So one full cycle takes 2 × (192 + 40) = 464 frames horizontally and 2 × (160 + 40) = 400 frames
  vertically [likely, ±2 frames].

Its own code never goes to its delete state (state 2, `0x800ECFEC`: `display` 0, `DeleteObject`).

## Records in the Turtloid stage

Five records, at x 4248–4880 (screens 16–19) over the sea: `id` 1 at (4248, 384) and (4880, 336);
`id` 0 at (4368, 368), (4560, 256) and (4752, 320).

## Summary for a reimplementation

- A solid 28 × 56 px block that the player can stand on. No damage either way.
- `id` 0: moves horizontally ±96 px around its start at 1 px/frame, first toward the player.
- `id` ≠ 0: moves vertically ±80 px around its start at 1 px/frame, first up.
- Stops for 40 frames at each end.
- Not solid while the player flashes after a hit.

## Open questions

1. The hurt box, the damage table (`dmg_tbl_capsule`) and the contact damage (10) are set, but no code
   of the platform uses them. Check if a shot can hit it.
2. The meaning of `mega+0xE0` = 2, which stops the motion.
3. The unnamed calls `0x80032B34`, `0x80032BE8`, `0x8003208C` and `0x80031F4C` (also called by the
   health capsule; probably solid-object helpers).
