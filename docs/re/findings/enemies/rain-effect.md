# Rain effect (stage 03, main object type 37): short sheet

This object is **rain driven by wind**. It is a pure effect: it cannot hurt the player and cannot be
hit. One invisible generator object makes small rain sprites near the top of the view. Each sprite is
a 32 × 32 px group of blue streaks, and it falls down and to the left. While the player is in the
rain zone, the generator also sets a player flag. With this flag set, the player moves to the right
at 3/4 speed (a head wind). The object appears only when the stage's Nightmare Effect value is 6.
There is no official name. "Rain effect" is a descriptive name.

Overlay `03` (Commander Yammark), code at `0x800EF304`–`0x800EF810`, save state
`yammark-1_start.state.gz` (stage 1, area 0). Written with the RE kit (`python3 -m mmxdemake.rekit
--state ~/mmx-data/x6/dumps/yammark-1_start.state.gz --type 37`) and `docs/re-guide.md`. Units,
confidence tags and the shared engine rules: `docs/engine.md`. Stage: `docs/stages/yammark.md`.

## When it appears

- **Spawn condition.** The set-up (state 0, `0x800EF304`) reads the byte `0x800CD30A`. When this byte
  is not **6**, the object deletes itself at once (`DeleteObject3`) [certain]. The byte is
  `nightmare_effect`, and its value is the stage whose pick caused the effect (`docs/engine.md`,
  "Nightmare Effect") [certain]. Value 6 comes from the pick of stage 6 (Rainy Turtloid), which
  writes 6 to stages 1, 5 and 5: so the rain can come to Yammark's stage (1) and to stage 5, the two
  stages in the zone table (below) [certain for the code]. The byte is 0 in the Yammark states. Other
  overlay code tests the same byte for 7 (Nightmare shadows, Turtloid) and 8 (the darkness effect,
  `docs/enemies/darkness-effect.md`).
- **Two kinds by `var`:** `var` 0 is the generator; `var` 1 is one rain sprite [certain].
- **The record.** The generator comes from the second record list of Yammark section 1
  (`fixed_set_tbl`, list `0x800F66D0`): its last record before the end is kind 0, type 37, `id` 0,
  at (0, 0) [certain]. So it is a main object that spawns with the stage; it deletes itself when
  the effect is not 6 (`docs/stages/yammark.md`, "The second record list").

### Generator (`var` 0) [certain]

- No boxes, HP 0, contact damage 0, draw priority 0, `display` 0: it is never drawn. It loads its
  graphics from its sprite bank (bank 8).
- Each frame it tests the player position against 8 rectangles (table `0x800F7900`, 10 bytes each:
  `{s16 x0, s16 y0, s16 x1, s16 y1, u8 stage, pad}`). A rectangle matches when `game.stage`
  (`0x800CCEDC`) is its stage and `x0 ≤ mega.x ≤ x1` and `y0 ≤ mega.y ≤ y1` (`0x800EF4D4`).
- When a rectangle matches: it sets `mega+0x150` (`0x800971F0`) to 1 and counts a timer down from 5.
  At 0 it makes one rain sprite (`GetMainObject`, flags `0x41`, type 37, `var` 1, copies the
  parent's graphics and layer) and sets the timer to 5 again. So **one rain sprite every 5 frames**.
- When no rectangle matches: it sets `mega+0x150` to 0. The timer stops.
- It has no screen test and never deletes itself.

The rain zones (all 8 records of the table) [certain]:

| # | stage | x | y |
|---|---|---|---|
| 0 | 1 | 3200–4800 | 1648–2160 |
| 1 | `0x13` | 0–1616 | 224–544 |
| 2 | 5 | 2256–2592 | 0–896 |
| 3 | 5 | 2896–3856 | 1392–1744 |
| 4 | 5 | 960–1920 | 2336–2688 |
| 5 | 5 | 384–1312 | 2800–3376 |
| 6 | 5 | 2400–3440 | 2672–3472 |
| 7 | `0x15` | 16–608 | 240–912 |

In Yammark's stage (`game.stage` = 1) only zone 0 can match: the last part of section 1, before the
boss gates at x 4848. Zones 2–6 are for stage 5, which the same pick (stage 6) gives the effect
[certain for the pick table]. `0x13` and `0x15` are stages 19 and 21, two hidden areas [likely: the
stage numbers exist (`docs/engine.md`, "Boss overlay"), but `StageStartReset` copies the effect
only for stages below 19, so these zones may never match].

The copies in overlay 08 (stage 5, entry `0x800EFD04`, zones `0x800F5D98`) and overlay 29 (stage 21
area 0, entry `0x800EB008`, zones `0x800EC64C`) have the same 8 zones. Zones 2–6 are sections 3, 7,
1, 4 and 5 of Ground Scaravich's stage. Zone 7 (x 16–608, y 240–912) is exactly the main room of
Scaravich's hidden area (`docs/stages/scaravich-alt.md`). A hidden area keeps the effect value of
the stage before it, so with effect 6 from stage 5 the rain can be live there [likely: no state
with effect 6].

### Rain sprite (`var` 1) [certain]

- Start: x = camera x + 2 × `GetRNG()` + (`GetRNG()` & 31), that is camera x + 0 … 541 px; y = camera
  y − 16 px. `GetRNG` (`0x8002D2D4`) gives 0–255. The camera is `camera_layer0` (`0x800971F8`), x at
  `+0x0A`, y at `+0x0E`.
- Speed: `speed_x` = `speed_y` = `0xFFF90000` = −7 px/frame. So it moves **7 px/frame left and
  7 px/frame down** (45°). No gravity, no terrain box: it falls through the stage.
- Draw priority 0, layer = `mega.layer`. Drawn when inside the view plus 32 px (`DisplayCheck(32,
  32)`).
- It is deleted (state 2, `DeleteObject3`) when x < camera x − 16, or y > camera y + 256, or
  `OffScreenCheck2(336, 48)` is 1. A sprite lives about 39 frames (272 px down at 7 px/frame), so
  about 8 rain sprites exist at a time.

## Effect on the player [certain for the code]

`mega+0x150` (`0x800971F0`) is read only by `0x8002D5D0`, a move function in the main program. It is
called 30 times from the player's state code (`0x80035190`–`0x8003AEF8`; that code uses offsets
above `0x9C`, so its object is the player [likely]). It does `y −= speed_y` and:

- flag ≠ 0 and the player's `flip` ≠ 0 (facing right [likely]): `x += speed_x × 3/4` (rounded toward
  0);
- else: `x += speed_x`.

So in the rain zone the player moves at **75 % speed while facing right**, against the wind. Facing
left, the speed is not changed. The function is shared by all stages; the flag is set only by this
object here.

## Animations (table `0x800F78E8`)

| # | what | frames |
|---|---|---|
| 0 | a 32 × 32 px group of blue and white rain streaks, from top right to bottom left | sprites 0–4, 3 frames each, loop (15 frames) |

`SetAnime(obj, 0)` is the only animation call. No event flag is tested.

## Damage

- To the player: **none**. No attack box, contact damage 0, and the code does not call `ContactMega`
  [certain]. The rain is not acid rain here.
- From the player: none. No hurt box and no `ContactWeapon` call [certain].
- The only effect on play is the 3/4 speed to the right (above).

## The other rain: Turtloid's acid-rain controller (stage logic 11, overlay 09)

Rainy Turtloid's stage has its own rain, with no rain sprites. The controller is stage-logic type 11
(`0x800ED510`), placed by the Turtloid second list (kind 3, type 11, `id` 0). Its states and the HP
drain are in `docs/enemies/weather-analyze.md`, "The acid rain". What it draws [certain for the
code]:

- **The rain curtain is BG layer 1.** Layer 1 of the Turtloid layout holds 5 rain frames, one per
  screen row (rows 0–4, columns 0–1; each frame fills the 320 × 240 view). Layer 1 runs camera mode 5
  (`0x800297B4`, it does nothing), so only the controller moves it. At the stage start the
  checkpoint record puts layer 1 at (320, 0), right of the rain art [likely: so the curtain is not
  seen]. At the change dry → rain (state 1, `0x800ED69C`) the controller sets layer 1 to (0, 0),
  the redraw request (`cam1+0x4C`) and its flag `+0x1E` = 1.
- **Animation.** While `+0x1E` ≠ 0, the dispatcher counts `+0x14` each update; every 4 updates it
  sets layer 1 y = frame × 256, frame = 0 … 4 (a 20-update loop), and asks for a redraw. The flag is
  never cleared in the states read, so the frames run on after the rain stops; the palette makes them
  invisible.
- **Palettes.** State 0 (`0x800ED5C4`) saves the 16 colours of source palette 16 (scratchpad
  `+0x30` + `0x200`) to `0x800F6BA4`. The fade states 2 and 4 count `+0x1A` between 0 and 30. Each
  update they write `PaletteDarken(0x800F6BC4, 0x800F6BA4, 16, 30 − n, 30)` (`0x8002B410`: each
  channel × (1 − k/30)) to palette 99 with `PaletteSet` (`0x8002B3D4`), so the curtain fades in over
  30 updates. `0x800ED18C` also blends a list of stage palettes (count and indices at source
  `+0x460`) toward their rain colours by n/30.
- State 0 also sets `stage_texture_swap` = 1 when `game.checkpoint` = 3 [certain for the code; why
  is open].

## Open questions

1. Not seen in play: whether the 3/4 speed applies to dashes and air moves (all 30 callers were not
   read).
2. Whether zones 1 and 7 (stages 19 and 21) match in play (a save state with effect 6 in the
   hidden area).
3. Turtloid: why the rain controller asks for the texture swap at checkpoint 3.
