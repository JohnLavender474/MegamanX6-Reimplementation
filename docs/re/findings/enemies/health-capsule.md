# Health capsule (stage 09, stage-object type 13): short sheet

The health capsule is a pad on the floor. When the player stands on it, the player is held in place,
a column of blue energy rises from the pad, and the player's HP refills to the maximum. While this
happens, the acid rain does not drain HP. Most capsules can be used again and again; one breaks
after one use. There is no official name; "health capsule" is the name that the user gave the sprite
bank.

Overlay `09` (Rainy Turtloid), code at `0x800EE03C`–`0x800EE9E4`, save state `turtloid-1.state.gz`
(stage 6, area 0). Written with the RE kit (`python3 -m mmxdemake.rekit --state turtloid-1.state.gz
--class stage --type 13 --name health-capsule`) and `docs/re-guide.md`. Units, confidence tags and
the shared engine rules: `docs/engine.md`. Records: `docs/stages/turtloid.md`.

## Set-up (`0x800EE03C`) [certain]

- Sprite type 143 (bank 12 in this stage): sprites 0–7 the pad, 8–41 the energy column, 43–45 the
  debris. Draw priority 7. Animation 0.
- Terrain box `0x800F62D8` = (0, 0, 24, 8): a **48 × 16 px** body. No hurt box: it cannot be hit.
- It is a solid object (`SolidObject` each update): the player can stand on it.

## Use (step 0, sub-steps at `step2`) [certain for the code]

| sub-step | routine | what |
|---|---|---|
| 0 WAIT | `0x800EE5A8` | Waits until the player stands on the pad (test below). Then: `game+0x27` = 1, the player is moved to the pad's x and onto its top, `PlayerEvent(20, mega.flip)`: the player is asked to go to the event hold (`docs/engine.md`, "The player"). |
| 1 START | `0x800EE150` | Waits until the player's step (`mega+0x05`) is 20 (the event hold). `SetStopFlags(1)` (`0x8004E78C`): the stop flags `game+0x10`–`+0x17` and `+0x19` = 1, as `PauseObjects` does, so the other objects freeze [certain for the code; likely for the effect]. If the HP is already at the maximum: animation 2 and sub-step 2. Else: sound (bank 2, id 85), animation 1, a 31-update timer, two energy-column effects (below), sub-step 3. |
| 2 FULL | `0x800EE628` | Animation 2 plays to its end. Then the end (below). |
| 3 DELAY | `0x800EE698` | 31 updates. |
| 4 REFILL | `0x800EE27C` | **1 HP every 2 updates**, with sound (bank 0, id 13) on each tick, until the HP (`mega+0x5C`, bits 0–6) reaches the maximum (`game.hp_max`, `game+0x5B` + character). Bit 7 of the HP is kept. Then a 27-update timer and sub-step 5. |
| 5 HOLD | `0x800EE6CC` | 27 updates. Then the end. |

**End** (sub-steps 2 and 5): `PlayerEventEnd` (the player goes back to standing and takes input
again), `game+0x27` = 0, animation 0, `SetStopFlags(0)` (the objects move again), step 1.

**The standing test** (`0x800EE330`) is true when all of these are true:

- the player's step (`mega+0x05`) is 2 (stand), 3 (walk start), 4 (walk) or 20 (event hold). So the
  player must stand or walk on the pad; a jump, a fall or a dash onto it does not start it until the
  player stands or walks [likely];
- the player's terrain box is **at least 8 px inside both side edges** of the pad (so the player must
  be within the middle 32 px);
- the two terrain boxes touch or overlap vertically.

**The rain.** While `game+0x27` ≠ 0, the rain controller holds its drain timers at 0
(`docs/enemies/weather-analyze.md`). So the acid rain does not drain HP while the player uses the
capsule [certain for the code].

## After the use (step 1, `0x800EE748`) [certain for the code]

- `id` 0: waits until the standing test is false (the player has left), then back to WAIT. **It can
  be used again and again.**
- `id` 1: breaks at once: 3 debris pieces (`SpawnJunk`, animations 7, 8, 9), `display` 0, then
  `DeleteObject3`. `DeleteObject3` does not clear the record flags, so it probably does not come
  back [likely].

## The energy column (effect object type 14)

Two effect objects (`GetEffectObject`, flags `0x49`), `id` 0 (priority 1) and `id` 1 (priority 6). Each
keeps a pointer to the capsule (`+0x50`) and copies its position, layer and graphics. Code: set-up
`0x800EE7C8`, update `0x800EE978`, delete `0x800EE9C4` [certain for the code]:

- `id` 0: animation 3 (the column rises); then animation 5 (it shimmers) until the capsule reaches
  sub-step 5; then animation 6 (it fades); deleted at the end of the animation.
- `id` 1: animation 4 once, then deleted.

## Records in the Turtloid stage

| record | x, y | `id` |
|---|---|---|
| 41 | 1280, 584 | 0 |
| 42 | 1920, 648 | 0 |
| 43 | 2912, 408 | 0 |
| 44 | 8096, 408 | 0 |
| 45 | 8528, 712 | 1 (one use) |

Each is inside or next to a Weather Analyze rain zone (x 1248–1631, 1872–2879, 7792–8655).

## Summary for a reimplementation

- A solid 48 × 16 px pad. It cannot be hit and does no damage.
- When the player stands in its middle 32 px: hold the player on the pad, stop the rain drain, wait
  31 updates, then add 1 HP every 2 updates up to the maximum, then hold 27 updates and release.
  When the HP is already full, play a short animation and release.
- `id` 0 can be used again after the player steps off. `id` 1 breaks after one use.

## Open questions

1. Whether the `id` 1 capsule comes back after the player dies (the record flags are not cleared).
   A play test answers it.
