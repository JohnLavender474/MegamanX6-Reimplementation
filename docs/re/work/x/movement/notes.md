# Notes: player ground movement (part 2a)

## Method

- Kit: `python3 -m mmxdemake.rekit --player --state intro-1_start.state.gz` → `../kit/`.
- Read in order: `PlayerUpdate`, steps 3, 4, 2, the tests (`0x80039C40`, `…C9C`, `…CFC`, `…D7C`,
  `…EB0`), the step entries (`0x8003AD18`, `…ADB4`, `…AE8C`, `…AED4`, `…B054`, `…A9C4`, `…B9AC`),
  the physics set-up (`0x8003D598`, `…D630`, `…D718`), the dash (step 12, sub-steps `0x80073C04`),
  the crouch (step 26), X's idle (`0x801EB61C`). `movement/fn.sh <addr>` prints a function from the
  kit. `PlayerMove` and `PlayerMoveAccel` with `rekit --at`.
- Animation timings from `~/mmx-data/x6/sprites/players/x/animations.json`.

## Checks against the save states

- `intro-1_start`, `yammark-1_start` (X), `turtloid-6_zero-player` (Zero): `+0xF0`…`+0x134` equal the
  values of `PlayerPhysicsDefaults`; `+0xEC` = 0 (no parts) in all three [certain].
- All three are in step 2 with animation 5. Zero has HP 11 of max 32: 11 ≥ 32 / 3 = 10, so
  animation 5, not 6 [consistent].
- `+0xE7`: X 1, Zero 5 (= `game+0x5E`). `game+0xC9` = `0x18` in all three (bit 3: the crouch).

## Corrections to the map (`docs/players/map.md`)

- Step 5 is Zero's stop pose; X goes from walk straight to stand.
- Step 12 is the ground dash (not hurt): entry `0x8003A9C4` after the dash input `0x80039EB0`.
- Step 26 is the crouch (down held, `game+0xC9` bit 3).
- Step 31 is the super jump (up + jump; X with form 2, Zero with skill bit 1).
- The words after `player_steps`: `0x80073C04`, `…C10`, `…C1C` are the sub-step tables of steps 12,
  13 and 14.
- `0x8001750C` is `StopSound(bank, id)`, not a boss-kill sound.

## Key facts and where

| fact | where |
|---|---|
| buttons held / pressed | `+0x7C` / `+0x80`; bits from the tests that use them |
| physics constants | `0x8003D598` (16.16 constants), bonuses `0x8003D630` |
| shooting animations = n + 112 | `0x8003F4C4`, kept in step by `0x8003F508` |
| dash: no move in sub-step 0 | `0x800357A8` has no move call; `PlayerMoveAccel` is in `0x80039FFC` |
| dash min speed and timer | `0x80039FFC`, `0x8003A9C4` |
| head wind 86/100 in `PlayerMoveAccel` | `0x8002CFB0` (`0x51EB851F` = /100) |

## Open

- `+0x87` (dash without the button); `0x8003D26C`, `0x8003F644`, `0x8003C6A4`, `0x8003C7D8`.
- Zero's idle and step 5; the part names; `game+0xC9`.
- Play checks: dash length and slide, walk-start time, landing cut.
