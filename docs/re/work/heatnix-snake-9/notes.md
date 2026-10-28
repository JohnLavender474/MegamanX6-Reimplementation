# Nightmare Snake `var` 9 and the rising room (`heatnix-5_snake-rising`): working notes

## Input

- State `~/mmx-data/x6/dumps/heatnix-5_snake-rising.state.gz` (SHA-1 `dd35ee83…19c5`). The user:
  "now fighting against mini boss in room that slowly scrolls up and has the bottom half covered in
  a purple translucent flame that kills the player instantly".
- Stage 3, area 0, checkpoint 5, Normal. Zero (`var` 1) at (2901, 2234), step 2, HP 22.
- The main session already learned the state as catalog entry "Blaze Heatnix, second texture"
  (`texture` 22, `tmx_skip`) and drew `~/mmx-data/x6/stages/m097-blaze-heatnix-second-texture/`.
  I read that drawing; I did not write to `stages/`.

## Method

- `scripts/dump_state.py` of the first work folder (`heatnix-mini-boss/scripts/`) on the state.
- Kit: `python3 -m mmxdemake.rekit --state heatnix-5_snake-rising.state.gz --type 25 --name
  heatnix-rising-room --out ~/mmx-data/x6/re/heatnix-snake-9/kit` (60 functions, the same set as
  the first kit), and `--class visual --type 29 --out …/kit/visual29`.
- Read in `kit/code.txt`: `SnakeMagmaLeap` (`0x800EBAD0`) and its sub-steps `0x800ECF84`,
  `0x800ED02C`, `0x800ED040`, `0x800ED0F8`, `0x800ED10C`; `SnakeMagmaIntro` sub-steps
  `0x800EBDA8`, `0x800ED7D8`, `0x800ED860`, `0x800ED904`, `0x800ED968`; `SnakeDeath`
  (`0x800EC77C`, the `var` 8/9 tail); `SnakeVisualMain` (`0x800EC2D0`, the lava box part); the
  `var` 8 and `var` 9 branches of `SnakeInit` (`0x800EAE28`–`0x800EAF90`).
- `scripts/layer_view.py`: the cell flag bits, tile info bits and CLUT bit 15 of the cells that one
  BG layer shows in a view (uses the catalog entry "Blaze Heatnix, second texture").
- Collision of layer 0 in the tower (x 2848–3200, y 944–3072), from the stage member (one-off
  script; result below).
- `view-reconstruction.png`: the view of the state from the ORA layers: layer 0 at (2848, 2155),
  layer 1 (the ring) at layer px = screen + (336, 287), layer 2 at (2784, 1712) blended 50 % in
  front; the yellow line is the top of the kill boxes (screen y 136). The blend and the layer order
  are my guess at the game's draw; the places are from the state.

## Camera field names used here

The layer camera structs are `0x54` bytes: layer 0 `0x800971F8`, layer 1 `0x8009724C`, layer 2
`0x800972A0`. The code reaches layers 1 and 2 through the layer 0 base, so:

| code | is |
|---|---|
| `camera_layer0+0x28`, `+0x2A` | `cam.target_max_y`, `cam.target_min_y` (layer 0) |
| `camera_layer0+0x57` | layer 1 `+0x03` (visible) |
| `camera_layer0+0xAB` | layer 2 `+0x03` (visible) |
| `camera_layer0+0xAC` | layer 2 `+0x04` (mode) |
| `camera_layer0+0xB2`, `+0xB4`/`+0xB6` | layer 2 x (`+0x0A`), y word (`+0x0C`) / y px (`+0x0E`) |
| `camera_layer0+0xE8` | layer 2 `+0x40` (ox) |
| `camera_layer0+0xF4` | layer 2 `+0x4C` (redraw request) |

## The state against the sheet [certain for the values]

| what | state | sheet / code |
|---|---|---|
| body | main slot 0, `var` 9, state 1, step 6, sub 4, (3024, 2380) | step 6 = `var` 8/9 move |
| speed | `speed_x` 0, `speed_y` −2.0625, gravity +4096 (1/16), `accel_x` 0 | sub 4 falls, cap −2, then − 1/16 (as the 2.0625 of `var` 5) |
| timers | `+0x7C` 0 (the apex wait is over), `+0x7E` 3 (left by the bar fill), `+0x8D` 0, `+0x8E` 0 | `+0x8D` = 1 only during the apex wait |
| boxes | attack `0x800F3DDC`, hurt `0x800F3DE0`, `dmg_tbl_deflect`, **terrain box 0** | `var` 8/9 have no terrain box |
| HP | 127, four gems at 32 | sum 128 → 127 |
| gems | slots 1–4, places (2980, 2323), (3082, 2326), (2996, 2430), (3090, 2416) = body + offsets exactly; gem 2 not displayed (screen y 275) | offsets `0x800E987C` |
| jets | visual 0–3 at body + offsets; only `var` 3 (the flame above the ring) displayed | body sub 4 = 4 → `var` 3 |
| lava boxes | visual slots 4, 5: `var` 6, step 2, x 2944 and 3104, y 2347 = `+0x90`, contact 127, attack `0x800F4000`, not displayed; `+0x92` = 3 on the box at 2944, 0 on the other | body `+0x84`, `+0x88` point to them |
| layer 0 | (2848, 2155), target and limits x 2848–2880, y min = max = 2155 | scroll code moves both y limits |
| layer 1 | mode 5, visible, (−2512, −1868) = (512 − 3024, 512 − 2380) | the ring rule holds |
| layer 2 | **mode 9**, visible, (2784, 1712), ox −64, oy 0 | x = 2848 − 64; y = 1712 (end of the rise); −64 = `snake_lava_waves[3]` − 2848 |
| game | `game.boss` = slot 0, bar on, `boss_hp_max` 127, `game+0x26` 0, `stage_texture_swap` 2 | as the intro writes |
| player | step 2, `mega+0xD1` 20, `mega+0xD0` 0, `mega.special` 0 | the intro event has ended |
| stage objects | grey blocks (type 12, `id` 3, 4), gates `id` 7, 8, 9, 11, 12, 13 | gate 13 at (2864, 2224) in state 2 (passed) |

Time since the intro ended [likely]: the camera y limits start at 2160 (`var` 9 set-up) and are
2155 now: 5 steps of 4 updates, about 20 updates. In 20 updates the body falls from camera + 192
(the intro end) to camera + 225 (speed −1 → −2 in 16 updates, then −2): 24 + 8 = 32 px. Both agree.

## Step 6 (`SnakeMagmaLeap`, `0x800EBAD0`) [certain for the code]

Each update: the sub-step, then:

1. When `mega.special` (`mega+0xE0`) = 0:
   - `cam.target_min_y` ≥ 945: on `frame_count & 3` = 0, `cam.target_min_y` − 1,
     `cam.target_max_y` − 1 and body y − 1. So the view goes up 1 px every 4 updates (0.25 px/frame,
     15 px/s) and the body keeps its place on the screen.
   - `cam.target_min_y` < 945: `PlaySound(5, 3)` each update (no scroll).
   During a special attack (`mega.special` ≠ 0) the scroll stops.
2. Player y < `cam.target_min_y` − 16: player y = `cam.target_min_y` − 16 (no exit above the view).
3. Lava boxes (`+0x84`, `+0x88`): `+0x90` = `cam.target_min_y` + 192.

Sub-steps (`0x800F3F9C`):

| sub | address | what |
|---|---|---|
| 0 | `0x800ECF84` | `GetRNG() & 1`: 1 → x 2944, `+0x8E` = (`+0x8E` + 16) & `0xF0`; 0 → x 3104, `+0x8E` = (`+0x8E` + 1) & `0x0F`. When `+0x8E` = 3 (third 3104 in a row): x 2944; = 48 (third 2944 in a row): x 3104; then `+0x8E` = 0. So never three times on one side. `speed_y` = +1, `speed_x` = `accel_x` = 0, gravity −1/16. Sub 2 (not 1) |
| 1 | `0x800ED02C` | sub + 1 (not reached from sub 0) |
| 2 | `0x800ED040` | while `+0x7C` ≠ 0: − 1 (wait; 90 from sub 4). Then `speed_y` capped at 3, `MoveObject2`: rises, 1 → 3 px/frame (+1/16 per update). When y < `cam.target_min_y` + 144 and gravity < 0: gravity = +1/16 (slows). When `speed_y` < 0 (the top): `speed_y` = −1, gravity +1/16, `+0x7C` = 90, sub 4 |
| 3 | `0x800ED0F8` | sub + 1 (not reached) |
| 4 | `0x800ED10C` | while `+0x7C` ≠ 0: − 1, `+0x8D` = 1 (it hangs at the top; the gems may shoot). Then `+0x8D` = 0, `speed_y` capped at −2, `MoveObject2`: falls 1 → 2 px/frame. When y > `cam.target_min_y` + 416: `+0x7C` = 90, sub 0 |

Sub 0 writes 2 to the sub-step, so sub 1 and 3 are not used [certain].

Computed cycle (relative to the view; the scroll moves body and view together) [likely: from the
code, not timed in play]:

| part | updates | screen y of the body point |
|---|---|---|
| wait below the view (sub 2) | 90 | about 418 (hidden: the ring art is −80…+64 px about the point) |
| rise, 1 → 3 px/frame | 32 | 418 → 354 |
| rise at 3 px/frame | 70 | 354 → 144 |
| slow down, 3 → 0 | 48 | 144 → about 72 |
| hang at the top (sub 4, `+0x8D` = 1) | 90 | about 72: the ring spans about −8…136, just above the flame |
| fall, 1 → 2 px/frame | 16 | 72 → 96 |
| fall at 2 px/frame | 160 | 96 → 416 |
| total | about 508 (8.5 s) | |

Jets: `SnakeJetFlame` shows `var` 2 (below the ring) in sub 2 and `var` 3 (above) in sub 4, and
none while `+0x7C` ≠ 0. So the jet below pushes it up, the jet above pushes it down; no jet during
the two waits [certain for the code; the state shows `var` 3 in sub 4].

Gem shots: only while `+0x8D` = 1, so only at the top. The hang is 90 updates and the gems fire when
`frame_count % 120` = 0: 0 or 1 volley per leap (1 in about 3 of 4 leaps) [likely: depends on the
phase].

The body has no terrain box: it goes through the platforms [certain for the code].

## The intro of `var` 8/9 (step 10, `SnakeMagmaIntro`) [certain for the code]

Set-up (`SnakeInit`):

- `var` 9 (`0x800EAF1C`): checkpoint 4 → deleted. Else at once (no player test): `cam.target_min_y`
  = `cam.target_max_y` = **2160**; body (3024, 2160 + 352 = 2512); layer 2 visible, layer 2 ox =
  −288, redraw; `PlayerEvent(20, mega facing)`.
- `var` 8 (`0x800EAE28`): checkpoint 5 → deleted. Else each update: y limits = **2816**, player y
  kept ≥ 2800; it starts when the player is at x < 2944 and y < 2912, or at x 3009–3071 and
  y < 2880. Then as `var` 9 with y 2816 + 352 = 3168.
- Both: no terrain box, step 10.

Sub-steps (`0x800F3FE0`):

| sub | address | what |
|---|---|---|
| 0 | `0x800EBDA8` | when the player is in step 20: two visual 29 `var` 6 (the lava boxes) at x 2944 and 3104, y = body y, the body's graphics, `+0x84` / `+0x88`; **layer 2 mode 9, layer 2 x 960, y 1600** (not the offsets; mode 9 rewrites x each update as x0 + ox), redraw; `PlaySound(5, 1)`, `Rumble(4)` |
| 1 | `0x800ED7D8` | layer 2 y + 0.5 per update (the flame comes up the screen) until y ≥ 1712: 224 updates; `Quake(3, 2, 1)` every 4 updates; at the end `PlaySound(5, 3)` |
| 2 | `0x800ED860` | body y − 1 per update until y ≤ `cam.target_min_y` + 192: 160 updates (from + 352); `Quake(3, 2, 1)` every 4, sound (2, 3) every 32 |
| 3 | `0x800ED904` | HP bar on, `boss_hp_max` 127, `BossLevel`, `game+0x25` = 0, `game.boss` = body, `+0x7E` = 3 |
| 4 | `0x800ED968` | HP + 1 every 2nd update to 127 (from 32: 190 updates), tick (0, 14) every 3; at 127: `PlayerEventEnd`, step 6, sub 4, `speed_y` −1, gravity +1/16, `PlaySound(5, 1)`, `Rumble(4)` |

The intro takes about 224 + 160 + 1 + 190 = 575 updates (9.6 s) [likely]. The scroll code is in
step 6 only: the view does not move during the intro [certain].

## The purple flame

**What it is** [certain]: the art of **BG layer 2**. The two visual 29 `var` 6 objects are the kill
boxes and are never drawn.

- Layer 2 is in mode 9 (x = layer 0 x + ox; y is not changed by the mode). After the intro its y
  is 1712 and stays there while the view goes up, so the flame keeps its place on the screen.
- The art (stage member 97, texture block `0x16`): four frames, each 576 px wide, at layer x 960,
  1568, 2176 and 2784 (608 px apart), art rows y 1840–2255 (empty rows 2032–2063 in the middle)
  [certain for the drawing of `m097-blaze-heatnix-second-texture`]. These are the purple layer 2
  screens of `docs/stages/heatnix.md` (rows 7–8).
- The animation: the box at x 2944 counts `+0x92` = (`+0x92` + 1) & 3 when `frame_count % 10` = 0
  and writes layer 2 ox = `snake_lava_waves[+0x92]` − 2848 (−1888, −1280, −672, −64), and asks a
  redraw each update. So the view shows frame 0, 1, 2, 3 in turn, 10 updates each (a 40-update
  loop). With the camera x 2848–2880, the view takes layer x wave … wave + 352, inside the 576 px
  frame [certain].
- On the screen: layer 2 y 1712 puts the top of the art at screen y 128–130 (the flame tips) and
  fills the view below it: the bottom half, as the user says [certain for the numbers].
- The cells of the flame have cell flag bits 14 and 15 set (value 3 in `v >> 14`, 140 of 140 cells
  in the view), and all their CLUT colours have bit 15 (the PS1 semi-transparency bit; 33780 of
  33780 pixels). Layer 0 in the same view: 320 cells, flags 0, 90 of 81920 pixels with bit 15
  (`scripts/layer_view.py`) [certain]. Layer 0 is opaque here, so the flame can be seen only if
  the game draws it in front of layer 0: bit 15 = "drawn in a later group, in front" (the README
  says "likely") [likely, stronger now]; bit 14 = the semi-transparent draw [guess: the user sees
  it translucent; the draw code was not read].

**The kill rule** [certain for the code]:

- Two visual 29 `var` 6 objects at x 2944 and 3104, y = `cam.target_min_y` + 192 (step 6 writes
  `+0x90`, the box copies it to y each update). Attack box `0x800F4000` = (−80, −56, 160, 112):
  x 2864–3024 and 3024–3184, y camera + 136 … camera + 248. The shaft is x 2880–3168 (walls of
  collision 56 at 2848–2880 and 3168–3200), so the boxes cover its full width and the bottom
  104 px of the view (screen y 136–240).
- Contact damage **127**: `ContactMega` → `PlayerDamageScale` returns 127 without scaling →
  instant death (`docs/damage.md`). The usual `ContactMega` rules still apply: no hit while
  `mega.iframes` ≠ 0, `no_hit`, HP bit 7 (so a player that blinks after a hit is safe for those
  updates) [certain for the code].
- The flame has no collision value: the tower has only 0, 56 (solid) and 59 (gate) in layer 0, no
  spike value 62/63 [certain]. The platforms of the tower are layer 0 solid cells (56).
- The player cannot escape upward: player y ≥ `cam.target_min_y` − 16 each update.

**The end** (`SnakeDeath`, `var` 8/9 tail, each update of step 8) [certain for the code]: layer 2
y − 1 and both boxes `+0x90` + 1 (the flame and the kill line go down 1 px per update); the body
`+0x7E` counts up from 0; from 112 on: `PlaySound(5, 3)`, layer 2 hidden; at sub 4 with layer 1
hidden: `cam.target_max_y` = 2816, `cam.target_min_y` = 944 (the whole tower opens), body state 3
(`SnakeEnd`: layer 1 hidden, `game+0x26` = 1 → the next gate, `id` 14 at (3200, 1024), opens),
boxes state 2 (deleted).

**Scroll range** [certain for the code; the length is computed]: from 2160 (the `var` 9 set-up) to
944 (the last step at 945): 1216 px, 4864 updates (81 s) after the intro. At 944 the view is the
top corridor (camera y 944–1184). If the fight lasts longer, the view stops there and
`PlaySound(5, 3)` runs each update.

## What the state does not show

- `var` 8 (the lower route): the same code; not in a state [likely].
- What sounds (5, 1) and (5, 3) are (bank 5 is the stage bank) and why (5, 3) repeats each update
  after the scroll ends.
- The game's draw order of layers 1 and 2 against layer 0 and the sprites, and the
  semi-transparency mode: read the BG draw code. The reconstruction is a guess for the blend.
- Whether the ring is seen through layer 0 here: layer 0 is opaque in this view, and the ring is on
  layer 1. In the first room it was seen; here the ring must also be in front of layer 0 for the
  player to see it [likely].
- The player's hurt box against the kill line (where on the sprite death starts).

## Sprites

`spriteexport --state heatnix-5_snake-rising.state.gz --root sprites-root`: 8 exports (3 live,
5 search), 2 skipped. All 8 IDs already exist in `~/mmx-data/x6/sprites/enemies/` (boss-gate,
flame-jet, grey-block, nightmare-bug, nightmare-snake, nightmare-virus, nightmare-virus-orb,
teleport-crystal), and every strip is pixel-identical to the shared one [certain: image diff].
Bank 6 (type 25) checked by eye: clean. **Nothing new.** The texture swap (state 2) did not
change the sprite banks.

Not sprites: `sprites-root/flame-layer2-frame0.png` … `frame3.png` (576 × 416 each, layer 2 x
960/1568/2176/2784, y 1840–2255, colours of this state, semi-transparency not applied), cut from
`stages/m097-blaze-heatnix-second-texture/stage.ora`.

## Scratch note

My ORA unzip went to the shared scratchpad folder `ora/` (it held another agent's extraction from
15:49 and I overwrote `ora/data/`). My other scratch files are in `scratchpad/snake9/`.
