# Proposed changes from `heatnix-5_snake-rising` (Nightmare Snake `var` 9, the rising room)

Evidence: `notes.md` in this folder. Each change gives the exact old text and the new text.

## `docs/enemies/nightmare-snake.md`

### 1. Save states (lines 12–15)

Old:

```
`~/mmx-data/x6/dumps/`): `heatnix-1_start.state.gz` (the stage start: `var` 5 and `var` 4 wait in
state 0) and `heatnix-2_mini-boss.state.gz` (the fight runs: Zero at (366, 618), the ring at
(617, 584), rolling right at full speed). Written with the RE kit (`python3 -m mmxdemake.rekit
```

New:

```
`~/mmx-data/x6/dumps/`): `heatnix-1_start.state.gz` (the stage start: `var` 5 and `var` 4 wait in
state 0), `heatnix-2_mini-boss.state.gz` (the fight runs: Zero at (366, 618), the ring at
(617, 584), rolling right at full speed) and `heatnix-5_snake-rising.state.gz` (`var` 9 in the
rising room, step 6 sub 4: the ring at (3024, 2380) falls back into the flame; Zero at (2901, 2234);
checkpoint 5). Written with the RE kit (`python3 -m mmxdemake.rekit
```

### 2. The `var` table, row 8, 9 (line 84)

Old:

```
| 8, 9 | records 25, 24, `id` 8 (3021, 2976), `id` 9 (3021, 2224); 8 is deleted at checkpoint 5, 9 at checkpoint 4 | (3024, layer 0 camera y + 352) | step 10, then 6 | leaps out of rising lava at x 2944 or 3104 (two lava kill boxes, visual 29 `var` 6) [likely] |
```

New:

```
| 8, 9 | records 25, 24, `id` 8 (3021, 2976), `id` 9 (3021, 2224); 8 is deleted at checkpoint 5, 9 at checkpoint 4 | `var` 9: at once, camera y limits 2160, ring at (3024, 2512); `var` 8: camera y limits 2816, player y ≥ 2800, starts when the player is at x < 2944 and y < 2912 or at x 3009–3071 and y < 2880, ring at (3024, 3168) | step 10, then 6 | the rising room: the view goes up over a purple flame (BG layer 2) that kills; the ring leaps out of it at x 2944 or 3104 (see MAGMA LEAP) [certain for `var` 9: `heatnix-5_snake-rising`; `var` 8 runs the same code] |
```

### 3. Line 87

Old:

```
`var` 4 and 6–10 are not in a save state yet; their rows come from the code only.
```

New:

```
`var` 9 is checked in `heatnix-5_snake-rising`. `var` 4, 6, 7, 8 and 10 are not in a save state
yet; their rows come from the code only.
```

### 4. New section: insert before `### GEM SHOTS (gem step 1) [certain]` (line 145)

```
### MAGMA INTRO (step 10, `var` 8/9) [certain for the code]

The set-up gives `var` 8/9 no terrain box. The player is held (`PlayerEvent(20, …)`) at the
set-up. Sub-steps `0x800F3FE0`:

| sub | what |
|---|---|
| 0 | When the player is in step 20: two kill boxes (visual 29 `var` 6) at x 2944 and 3104 (body `+0x84`, `+0x88`); **BG layer 2 mode 9, layer 2 position (960, 1600)**, redraw; sound (5, 1), `Rumble(4)`. |
| 1 | Layer 2 y + 0.5 per update to 1712 (224 updates): the purple flame comes up from the bottom of the screen. `Quake(3, 2, 1)` every 4 updates; at the end sound (5, 3). |
| 2 | The ring rises 1 px per update from camera + 352 to camera + 192 (160 updates), `Quake(3, 2, 1)` every 4 updates, sound (2, 3) every 32. |
| 3 | HP bar on, `boss_hp_max` 127, `BossLevel`, `game+0x25` = 0, `game.boss` = the body. |
| 4 | +1 HP every 2nd update from 32 to 127, tick (0, 14) every 3 updates. At 127: `PlayerEventEnd`, step 6, sub 4 (it falls first), `speed_y` −1, gravity +1/16, sound (5, 1), `Rumble(4)`. |

About 575 updates (9.6 s) in all [likely]. The view does not move during the intro.

### MAGMA LEAP (step 6, `var` 8/9): the rising room [certain for the code; `heatnix-5_snake-rising` agrees]

Each update, after the sub-step:

1. **The scroll.** While `mega.special` (`mega+0xE0`) = 0 and `cam.target_min_y` ≥ 945: on every 4th
   update (`frame_count & 3` = 0) both camera y limits − 1 and the body y − 1. So the view goes up
   **0.25 px/frame** (15 px/s) and the ring keeps its place on the screen. During a special attack
   the scroll stops. From 2160 (`var` 9) to 944 it takes 4864 updates (81 s); then the view stops at
   the top corridor and sound (5, 3) plays each update.
2. Player y < `cam.target_min_y` − 16: player y = `cam.target_min_y` − 16 (no exit above the view).
3. Kill boxes: y = `cam.target_min_y` + 192.

| sub | what |
|---|---|
| 0 | Random x: 2944 or 3104 (`GetRNG() & 1`); `+0x8E` counts the picks in a row (low nibble 3104, high nibble 2944); a third pick on the same side goes to the other side. `speed_y` +1, gravity −1/16. Sub 2. |
| 2 | Wait 90 updates (below the view). Rise 1 → 3 px/frame (+1/16 per update). Below camera + 144, slow down by 1/16 per update. At the top: wait 90, sub 4. |
| 4 | Hang 90 updates at the top with `+0x8D` = 1 (the gems may shoot). Then fall 1 → 2 px/frame. Below camera + 416: wait 90, sub 0. |

Sub-steps 1 and 3 only add 1 and are not reached. Screen y of the body point [likely: computed]:
hidden at about 418, the top at about 72 (the ring then spans about −8 to 136, just above the
flame), a cycle of about 508 updates (8.5 s). Jet flame `var` 2 (below) shows while it rises,
`var` 3 (above) while it falls; none during the waits. The gems fire only during the hang (90 of
every 120 updates can hold a volley: 0 or 1 volley per leap). The ring has no terrain box and goes
through the platforms.

**The flame** [certain for the values; the draw order is likely]: BG layer 2 (mode 9: x = layer 0 x
+ ox, y fixed at 1712). It stays at one place on the screen while the view goes up: the art top is
at screen y 128–130 and it fills the view below. Four frames, 576 px wide, at layer 2 x 960, 1568,
2176 and 2784 (y 1840–2255): every 10 updates the kill box at x 2944 steps `+0x92` (0–3) and writes
layer 2 ox = `snake_lava_waves[+0x92]` − 2848 (a 40-update loop). The flame cells have cell bits 14
and 15 set and all their colours have the semi-transparency bit; layer 0 behind them is opaque, so
the game draws them in front, semi-transparent [likely].

**The kill line:** the two kill boxes (−80, −56, 160, 112) at x 2944 and 3104 cover the full shaft
(x 2880–3168) from camera + 136 to camera + 248: screen y 136 down. Contact 127 = instant death
(not scaled), except while the player has `iframes` or `no_hit` (`docs/damage.md`). No collision
value kills: the tower has only 0, 56 and 59.

**The end** (step 8, each update): layer 2 y − 1 and the kill boxes + 1 (the flame sinks); after 112
updates sound (5, 3) and layer 2 hidden; at death sub 4 (the ring hidden): camera y limits
944–2816 (the tower opens), body state 3 (`game+0x26` = 1: gate `id` 14 at (3200, 1024) can open),
the kill boxes deleted.
```

### 5. Jet flames (the sentence "`var` 5 uses only sub-steps 1 and 3, …")

Old:

```
(`frame_count & 31` = 0). A flame deletes itself when the body is in state 3. `var` 5 uses only
sub-steps 1 and 3, so only flames 0 and 1 are ever drawn. The state agrees:
the body rolls right, flame 0 is drawn, 1–3 are not.
```

New:

```
(`frame_count & 31` = 0). A flame deletes itself when the body is in state 3. `var` 5 uses only
sub-steps 1 and 3, so only flames 0 and 1 are ever drawn. The state agrees:
the body rolls right, flame 0 is drawn, 1–3 are not. `var` 8/9 use sub-steps 2 (rise) and 4 (fall),
so flames 2 and 3; `heatnix-5_snake-rising` (sub 4) shows flame 3 only.
```

The old text is lines 202–204 of the current sheet (checked).

### 6. Kill boxes (line 218)

Old:

```
**Kill boxes** (`var` 6, 7; the lava fights of `var` 8–10): contact **127**, attack box `var` 6
(−80, −56, 160, 112), `var` 7 (−80, −64, 160, 128); never drawn [certain for the code; not in a state].
```

New:

```
**Kill boxes** (`var` 6 for `var` 8/9, `var` 7 for `var` 10): contact **127**, attack box `var` 6
(−80, −56, 160, 112), `var` 7 (−80, −64, 160, 128); never drawn. Step 2: y = `+0x90` (the body
writes it); the box at x 2944 (`var` 6) or 3296 (`var` 7) also animates the BG layer 2 flame (ox =
`snake_lava_waves[n]` − 2848 or − 3232, n + 1 every 10 updates). `var` 7 ends when the player x ≥
3825 (layer 0 `+0x2E` + 64) [certain for the code; `var` 6 checked in `heatnix-5_snake-rising`].
```

### 7. Open question 1 (lines 294–295)

Old:

```
1. The other snakes (`var` 4, 6–10): the rows above come from the code only; save states of these
   fights would check them (the lava of `var` 8–10 is on BG layer 2).
```

New:

```
1. The other snakes (`var` 4, 6, 7, 8, 10): the rows above come from the code only; save states of
   these fights would check them. `var` 9 is checked. Open for `var` 8/9: the sounds (5, 1) and
   (5, 3), and the game's draw order and blend of the BG layer 2 flame.
```

## `docs/stages/heatnix.md`

### 1. Save-state table: add a row after `heatnix-2_mini-boss` (line 15)

```
| `heatnix-5_snake-rising` | 2901, 2234 | 12880 | checkpoint 5, the upper route: the rising room of Nightmare Snake `var` 9 (camera x 2848–2880, y limits 2155 and going up 1 px per 4 updates). Main type 25 `var` 9 and its gems `var` 0–3, visual 29 `var` 0–3 (jets) and `var` 6 × 2 (the kill boxes of the flame); BG layer 2 mode 9 at (2784, 1712) is the purple flame; `stage_texture_swap` 2 (entry `0x16` loaded) |
```

### 2. The map, item 8 (lines 59–60)

Old:

```
8. **The second tower** (x 2848–3200, y 944–3072) goes up; Nightmare Bug and Nightmare Virus
   records.
```

New:

```
8. **The second tower** (x 2848–3200, y 944–3072) goes up; Nightmare Bug and Nightmare Virus
   records. It is the **rising room** of Nightmare Snake `var` 9 (from checkpoint 5, y 2160) or
   `var` 8 (from checkpoint 4, y 2816): the view goes up 1 px every 4 updates to y 944 over a purple
   flame that kills (`docs/enemies/nightmare-snake.md`, MAGMA LEAP). Platforms: layer 0 solid cells
   (collision 56); no kill collision value in the tower.
```

### 3. BG layers paragraph (lines 69–71)

Old:

```
BG layers (from the TMX): layer 1 mode 5 (fixed on the screen; code moves it), layer 2 mode 1 (half
speed). The layer-2 camera of checkpoint 6 is (928, 1664), not half of the layer-0 camera (3200,
1056), so code also moves layer 2 in the later parts [certain for the table].
```

New:

```
BG layers (from the TMX): layer 1 mode 5 (fixed on the screen; code moves it), layer 2 mode 1 (half
speed). The layer-2 camera of checkpoint 6 is (928, 1664), not half of the layer-0 camera (3200,
1056), so code also moves layer 2 in the later parts [certain for the table]. In the rising room
Nightmare Snake `var` 8/9 set layer 2 to mode 9 and use it as the purple flame (four frames of
576 px at layer 2 x 960, 1568, 2176, 2784, y 1840–2255; ox stepped every 10 updates)
[certain; `heatnix-5_snake-rising`].
```

### 4. The purple screens (lines 88–93)

Old:

```
  because the colours come from the start state. So they are not block stores and not unused
  screens [likely]: they are the backgrounds of the part after the swap. Their colours and their
  place on the screen (layer 1 mode 5 is set by code) need a state after checkpoint 3.
```

New:

```
  because the colours come from the start state. So they are not block stores and not unused
  screens [likely]: they are the backgrounds of the part after the swap. The layer 2 screens of
  rows 7–8 are the flame of the rising room: four animation frames (x 960, 1568, 2176, 2784), cell
  bits 14 and 15 set, all colours semi-transparent; in `heatnix-5_snake-rising` they are purple
  and drawn at layer 2 y 1712 (mode 9) [certain for the values]. Rows 11–12 of layer 2 and the
  layer 1 screens are not checked in a state yet.
```

### 5. Nightmare Snake rooms (lines 391–393)

Old:

```
player is kept at y ≥ 432 until it passes x 944 [certain for the code]. Records 22–26 are main 25
`id` 6, 7, 9, 8, 10 (more snake fights; no save state yet), and the second list has `id` 5 (the
first fight) and `id` 4 (the next snake, which gate 7 makes again).
```

New:

```
player is kept at y ≥ 432 until it passes x 944 [certain for the code]. Records 22–26 are main 25
`id` 6, 7, 9, 8, 10 (more snake fights; `id` 9 is in `heatnix-5_snake-rising`, the others have no
save state yet), and the second list has `id` 5 (the first fight) and `id` 4 (the next snake,
which gate 7 makes again). The rising room (`id` 8 or 9): see the map, item 8.
```

### 6. Open question 1 (lines 402–404)

Old:

```
1. The colours and screen place of the BG screens after the texture swap, and the
   `texture_split` regions: a save state after checkpoint 3 (the shaft room or a route) will show
   entry `0x16` in VRAM.
```

New:

```
1. The colours and screen place of the BG screens after the texture swap, and the
   `texture_split` regions: `heatnix-5_snake-rising` has entry `0x16` in VRAM (`stage_texture_swap`
   2) and shows the layer 2 flame (rows 7–8); the layer 1 screens and layer 2 rows 11–12 are still
   open.
```

## `docs/engine.md`

### "A boss on a BG layer" (line 454): add at the end of the paragraph

After:

```
is within 96 px of the view [certain; `heatnix-2_mini-boss`: body (617, 584), layer 1 (−105, −72)].
```

add:

```
Nightmare Snake `var` 8/9 also use BG layer 2 as a hazard: mode 9 with y fixed at 1712, so the
purple flame stays on the screen while the code moves the layer 0 y limits up 1 px every 4 updates;
the frames are four places in the layer 2 map that a visual object picks through `+0x40` (ox); two
invisible objects with contact 127 are the kill line [certain; `heatnix-5_snake-rising`]. Its cells
have bits 14 and 15 set and semi-transparent colours, and it is seen in front of an opaque layer 0:
cell bit 15 draws in front [likely], bit 14 is the semi-transparent draw [guess; the BG draw code
is not read].
```

## `README.md` (optional)

Line 196–197 says the bit 15 cells are drawn "in a later group, likely in front of the sprites".
The flame of `heatnix-5_snake-rising` is evidence that bit 15 cells are drawn in front of layer 0
(see above). No change needed until the draw code is read.

## `catalog/helpers.json`

Add the entries of `helpers-proposed.json` (9 functions, 5 globals; no name or address clash with
the current file, checked). Replace the notes of three present entries (in the same file, section
`replace_notes`):

- `0x800EBDA8` `SnakeMagmaIntroStart`: "offsets (960, 1600)" is wrong; the code writes the layer 2
  **position** (`+0x0A`, `+0x0E`), not `+0x40`/`+0x42`.
- `0x800EBAD0` `SnakeMagmaLeap`: add the stop at camera y 945, `mega.special`, and the player y
  clamp.
- `0x800EC77C` `SnakeDeath`: add the `var` 8/9 tail.

## Shared sprites

Nothing to do: the 8 exports of this state have IDs already in `~/mmx-data/x6/sprites/enemies/`
and are pixel-identical (`notes.md`, Sprites). A run on the shared root adds only a stage 3 source
line. The four flame crops `sprites-root/flame-layer2-frame0.png` … `frame3.png` are not
`spriteexport` sheets; copy them to `enemies/nightmare-snake/` only if the demake wants them.
