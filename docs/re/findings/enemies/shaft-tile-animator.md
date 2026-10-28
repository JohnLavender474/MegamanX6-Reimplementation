# Shaft tile animator and room dimmer (stage 08, stage logic type 29): short sheet

Stage logic 29 has two jobs in Ground Scaravich's stage. `var` 0–3 animate the decoration
layer (BG layer 1) of the two vertical shafts (sections 2 and 3): they copy blocks of tiles in a
loop. `var` 4 makes the lights of sections 0 and 1 go dark and come back in a cycle of about 9.5 s.
Neither can hurt the player or be hit. Descriptive names; no official name.

Overlay `08`, entry `0x800EA2EC`, state table `0x800F4EA4` (0 set-up `0x800EA08C`, 1 tile loop
`0x800EA328`, 2 dimmer `0x800EA3D4`, 3 empty `0x800EA738`). 19 records in the second list
(`fixed_set_tbl`): `var` 0 × 2, 1 × 12, 2 × 2, 3 × 2, 4 × 1 (at 0, 0). All 19 are live in all four
save states [certain]. The kit walked past the end of the step table `0x800F4EB4` (6 real entries)
into other code; the 121 "functions" of its output are mostly other objects.

## When it acts [certain for the code]

- `var` 0–3 (state 1): only while the player is at x 1569–2623 and y 1–959 (the two shafts).
  `var` 1 then always runs; `var` 0, 2, 3 run only while `OffScreenCheck2(obj, rx, ry)` is 0, with
  (rx, ry) from table `0x800F4E8C` (6 bytes per `var`: layer, rx, ry): `var` 0 (544, 112), 2 (112,
  64), 3 (512, 112).
- `var` 4 (state 2): only while the player is at x 1–1919 and y 1505–2607 (sections 0 and 1).

## Tile loop (`var` 0–3, `0x800EA740`)

- Set-up: layer = table byte (1 for all four), script pointer from `0x800F4E7C`[`var`], the first
  script entry to `+0x18`…`+0x1B`.
- A script entry is 4 bytes: {frames, 0, next (signed, in entries), block}. When the frame count
  reaches 0: go to the next entry and `CopyTiles(layer 1, x, y, src)` with src =
  `0x800F4CA8` + `var` × 96 + block × 8 ({x, y, w, h} of a store in the map), then set the
  redraw flag of camera layer 1 (`+0x4C`).

| `var` | records | script (frames per block) | block size (cells) |
|---|---|---|---|
| 0 | 2, at (1616, 0), (2256, 0) | 15, 10, 15, 10, loop | 22 × 7 |
| 1 | 12, two diagonal rows (x + 64, y + 16 each) | 4, 4, 4, 4, loop | 4 × 6 |
| 2 | 2, at (1840, 208), (2496, 208) | 7, 7, 7, loop | 7 × 4 |
| 3 | 2, at (1632, 528), (2272, 528) | 100, then 6 frames per block | 20 × 7 |

The copies go to layer 1 (decoration). The tile collision is read from layer 0 only
(`docs/engine.md`, "Tile collision"), so these animations do not change the terrain [likely]. What
they show (machinery, falling sand) is not checked.

## Room dimmer (`var` 4, `0x800EA3D4`, steps `0x800F4EB4`)

- Set-up (once per stage, flag `game+0x32`): copy the 32 palettes from slot 147 (512 colours) to a
  backup, and the backup to `0x800F62F0`. Level `+0x14` = 32.
- Each step calls `PaletteBlend(0x800F66F0, 0x800F62F0, 512, level)` and `PaletteCopy(147, ...,
  32)`. `PaletteBlend` multiplies each colour by (1 − level / 32): level 0 = full colour, 31 or more
  = black [certain for the code].

| step | function | what | frames |
|---|---|---|---|
| 0 | `0x800EA448` | level −1 every 2nd frame, 32 → 0 (lights come on) | 64 |
| 1 | `0x800EA4E0` | flicker: level 32 and 0 in turn | about 9 |
| 2 | `0x800EA58C` | hold full colour | 301 |
| 3 | `0x800EA5C4` | flicker | about 9 |
| 4 | `0x800EA670` | level +1 every 2nd frame, 0 → 33 (lights go out) | 66 |
| 5 | `0x800EA70C` | hold black | 121 |

So the 32 palettes from slot 147 are dark for about 2 s in each cycle of about 9.5 s. In
`scaravich-3_section` (player in the hub) the dimmer is in state 2 with level 32 [certain].
Which art uses palettes 147–178 is not checked [open].

## Open questions

1. What the shaft animations show, and the full contents of the four scripts.
2. Which art the dimmer darkens, and whether it changes the play (enemies hard to see).
3. Whether `game+0x32` is cleared at the stage start.
