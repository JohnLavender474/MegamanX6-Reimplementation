# Darkness effect (stages 03 and 09, ray object type 5): short sheet

This object is a **screen darkening** for the Nightmare Effect with the value 8. It is a pure effect:
it cannot hurt the player and cannot be hit. While the player is inside a zone, four large
semi-transparent quads darken the view, except a lit area that sways from side to side. The quads
fade in when the player enters the zone and fade out when the player leaves it. There is no
official name. "Darkness effect" is a descriptive name.

Code: overlay `03` (Commander Yammark) entry `0x800EE56C`, state table `0x800F77A4`; overlay `09`
(Rainy Turtloid) entry `0x800EF69C`, state table `0x800F63EC`. The two copies are the same code with
other addresses [certain: every instruction of state 0 and state 1 matches]. The class is the ray
class (`0x8009C9B0`, 64 slots of `0x60`, update table at scratchpad `+0x98`; see
`docs/enemies/nightmare-virus.md`, "The ray class"). Save states `yammark-1_start`, `turtloid-1`;
read with `python3 -m mmxdemake.rekit --state <state> --at <address>`. Units and tags:
`docs/engine.md`. Records: `docs/stages/yammark.md`, `docs/stages/turtloid.md`.

## When it appears

- **Records.** Record kind 6 makes a ray object (`kind_table` `0x8007347C`, entry 6 =
  `GetRayObject`). The second record list (`fixed_set_tbl`) of each stage has three kind-6 records
  of type 5, all at (0, 0): Yammark `id` 0, 1, 2; Turtloid `id` 3, 4, 5 [certain].
- **Condition.** State 0 (`0x800EE154` in overlay 03, `0x800EF284` in overlay 09) deletes the object
  (`DeleteObject3`) when `nightmare_effect` (`0x800CD30A`) is not **8** [certain]. Value 8 means
  "caused by the pick of stage 8": the stage-8 pick writes 8 to `game.stage_effect` of stages 1, 6
  and 6 (`docs/engine.md`, "Nightmare Effect"). So the effect can occur only in Yammark's and
  Turtloid's stages, and both have the records [certain]. No save state has the value 8, so no
  object of this type is live in any state.

## Set-up (state 0) [certain]

- `var` < 16 (the record): it makes four ray objects of type 5 with `var` = `id` + 16, + 32, + 48
  and + 64, then goes to state 2 (it deletes itself). Each child gets the parent's flags byte (`0x59`).
- A child: flags `0x59` (bit `0x10`: semi-transparent), `+0x36` = 18 (draw layer), `+0x37` = −1
  (screen coordinates), colour `+0x34` = 0. Its zone is entry `var & 15` of the zone table
  (below). It is anchored at a screen point: `var` 48–79 at (160, 240), the bottom centre, with
  sway `+0x40` = 40 and step `+0x42` = +1; `var` 16–47 at (160, 0), the top centre, with sway −60
  and step −1.

## Update (state 1) [certain]

1. The sway `+0x40` changes by the step each update; when it leaves −128 … 128 the step changes
   sign. So the edges of the lit area swing about ±128 px around their start.
2. The zone test: the player (`mega+0x0A`, `+0x0E`) is inside when `|x − cx| ≤ w` and `|y − cy| ≤ h`.
   Inside: the fade counter `+0x38` goes up by 1 per update, up to 63. Outside: it goes down by 1;
   at 0 the colour is 0 and the object is not drawn.
3. The four corners (`+0x16` … `+0x32`, offsets from the anchor) by `var` group. Each group covers
   one side of the view; the inner edge runs from the anchor to x = 160 ± (sway + 120) on the far
   edge. Groups 64–79 and 48–63 cover the right and the left side from the bottom centre; groups
   16–31 and 32–47 do the same from the top centre.
4. The colour: `+0x34` = entry `counter / 4` of a 16-step grey table (`0x800F63CC` in overlay 09:
   `0x0000`, `0x0421`, … `0x3DEF`, that is 0 to 15 of 31 per channel). So a full fade takes 64
   updates.
5. `display` = 1.

## Drawing [certain for the code; the look is likely]

The ray draw (`0x80023ED8`, called for each displayed ray object without flag bit `0x02`) makes one
flat quad (GPU code `0x28`, `0x2A` with flag bit `0x10`). The blend mode is flags bits 5–7 (here 2),
passed to `GetTPage` (`0x80068DD4`). Mode 2 of the PS1 GPU is "screen minus quad colour". So each
quad subtracts up to 15/31 from every colour channel: the view gets darker outside a lit area that
sways [likely]. Not checked in play.

## Zones (table `0x800F638C` in overlay 09, `0x800F7744` in overlay 03; 8 bytes `{s16 cx, cy, w, h}`)

The table is the same in both overlays [certain]. The code indexes it as `base + var × 8` with a
base for each group, so all four children of one record read entry `id & 15`.

| `id` | stage | centre (cx, cy) | half size (w, h) | player zone |
|---|---|---|---|---|
| 0 | Yammark | 800, 960 | 800, 240 | x 0–1600, y 720–1200 |
| 1 | Yammark | 2096, 1328 | 1120, 120 | x 976–3216, y 1208–1448 |
| 2 | Yammark | 1920, 1920 | 960, 240 | x 960–2880, y 1680–2160 |
| 3 | Turtloid | 1440, 480 | 160, 240 | x 1280–1600, y 240–720 |
| 4 | Turtloid | 2400, 360 | 480, 360 | x 1920–2880, y 0–720 |
| 5 | Turtloid | 6240, 590 | 2720, 360 | x 3520–8960, y 230–950 |
| 6 | (no record found) | 560, 480 | 560, 480 | x 0–1120, y 0–960 |
| 7 | (no record found) | 808, 344 | 808, 136 | x 0–1616, y 208–480 |

## Damage

None. No boxes; the class has no contact test.

## Open questions

1. Which stage uses zones 6 and 7 (perhaps a hidden area).
2. Check the darkening and the sway in play (a save state in Yammark's or Turtloid's stage after a
   stage-8 pick).
3. The kit names `0x800EE1D4` in overlay 03 `RideInit` (an overlay-06 name). It is a branch target
   inside this set-up (`catalog/helpers.json` applies overlay names to every overlay).
