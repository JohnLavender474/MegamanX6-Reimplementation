# Proposed changes from `shark-10_alt` (stage 20 area 1), not applied

Agent: shark-alt. Work folder [`~/mmx-data/x6/re/shark-alt/`](./). Evidence: `notes.md`.

Order of work for the main session:

1. Apply the code change in `mmxdemake/ramlayout.py` and the test change in
   `tests/test_ramlayout.py` (below). Run `python3 -m unittest discover -s tests`.
2. `python3 -m mmxdemake.learn --state shark-10_alt.state.gz --name "Metal Shark Player, hidden area"`.
   Expected output: `member 118, 15x6, overlay 28 @0x8b28, edges 0.986/1.000, 0 cells differ, 31
   objects @0x8c38, palettes @(0, 486) from 211 tiles`. Expected entry: see "`catalog/stages.json`".
3. Apply the catalog changes (`enemies.json`, `object-names.json`) and `helpers-proposed.json`.
4. Draw: `stages --out <scratch>` and copy `m118-metal-shark-player-hidden-area/` into
   [`~/mmx-data/x6/stages/`](../stages/); `tmx --out ~/mmx-data/x6/tmx`. Then `spriteexport --state
   shark-10_alt.state.gz` (it adds only `banks/shark-alt/shark-10_alt/`, `stages/shark-alt.json`
   and the new source in 5 `animations.json` files; no new ID).
5. Add `docs/stages/shark-alt.md` = `area-page.md` of the work folder.
6. Apply the doc changes below.

## New file `docs/stages/shark-alt.md`

Copy [`~/mmx-data/x6/re/shark-alt/area-page.md`](area-page.md) as it is.

## `catalog/stages.json` (written by `learn` after the code change)

```
{"differing_cells": [], "edge": {"h": 0.986, "h_joins": 27, "v": 1.0, "v_joins": 3}, "height": 6, "member": 118, "name": "Metal Shark Player, hidden area", "objects": {"count": 31, "offset": 35896, "overlay": 28, "table_area": 1, "table_entry": 20}, "offset": 35624, "overlay": 28, "palettes": {"agree": 0.991, "cameras": [[0, 224], null, null], "clut_x": 0, "clut_y": 486, "matches": 211}, "save_state": "shark-10_alt.state.gz", "save_state_sha1": "114fb5c590494f5a3bc9dc0ee53f3e58ec1371f6", "width": 15}
```

and in `catalog/savestates.json`: `{"name": "shark-10_alt.state.gz", "note": "", "sha1":
"114fb5c590494f5a3bc9dc0ee53f3e58ec1371f6"}`.

## `mmxdemake/ramlayout.py`

Old:

```
def _without_cells(data):
    out = bytearray(data)
    out[2::4] = bytes(len(out[2::4]))
    return bytes(out)
```

New:

```
def _without_cells(data):
    out = bytearray(data)
    out[2::4] = bytes(len(out[2::4]))
    out[3::4] = bytes(len(out[3::4]))
    return bytes(out)
```

## `tests/test_ramlayout.py`

Old:

```
        ram[k + 4 * 1 + 2] ^= 0x35
        self.assertEqual(find_member(bytes(ram), {1: info}), 1)
```

New:

```
        ram[k + 4 * 1 + 2] ^= 0x35
        self.assertEqual(find_member(bytes(ram), {1: info}), 1)
        ram[k + 4 * 2 + 3] ^= 0x01
        self.assertEqual(find_member(bytes(ram), {1: info}), 1)
```

## `README.md`

Old:

```
`catalog/savestates.json`). It checks the RAM layout by how well the solid
```

New:

```
`catalog/savestates.json`). A member counts as loaded when its tile info is in
RAM with the cell numbers (bytes 2 and 3 of each 4-byte word) masked, because
animated tiles change them; the belt of the Metal Shark Player hidden area
(member 118) changes cell numbers above 255. It checks the RAM layout by how well the solid
```

## `catalog/enemies.json`

Old:

```
"22": "sheldon-alt", "22.1": "mijinion-alt"},
```

New:

```
"22": "sheldon-alt", "22.1": "mijinion-alt", "20.1": "shark-alt"},
```

## `catalog/object-names.json`

Old:

```
    "119": {"main:37": "RainEffect", "stage:12": "GreyBlock"},
```

New:

```
    "118": {"main:1": "Junkroid", "main:27": "MettaurD2", "stage:9": "PressBlock", "stage:11": "JunkBlock",
            "stage:15": "NightmareCube", "logic:15": "BeltUpdateAlt", "logic:30": "RideSpawner",
            "kind7:2": "PressUpdate", "kind7:4": "AutoScrollAlt"},
    "119": {"main:37": "RainEffect", "stage:12": "GreyBlock"},
```

## `docs/stages/shark.md`

Old:

```
| `id` 1 | (1552, 1584), the right end of the bottom room, beside the Light capsule | `0x41` | stage 20 area 1: the hidden area ("Another Route") of this stage | 28 (25 + 2 × (20 − 19) + 1; its record lists parse as the lists of stage 20 area 1) |
```

New:

```
| `id` 1 | (1552, 1584), the right end of the bottom room, beside the Light capsule | `0x41` | stage 20 area 1: the hidden area ("Another Route") of this stage ([`shark-alt.md`](shark-alt.md)) | 28 (25 + 2 × (20 − 19) + 1; confirmed by `shark-10_alt`) |
```

Old:

```
portal are past the large gap [Fandom, "Recycle Lab", read through the MediaWiki API]. Overlay 28 has
the Nightmare Press, a kind-7 type 4 object, junk blocks, Nightmare Cubes and a Ride Armor place
(`docs/enemies/nightmare-press.md`, "Other users"; `docs/players/ride-armor.md`). The teleporter
```

New:

```
portal are past the large gap [Fandom, "Recycle Lab", read through the MediaWiki API]. Overlay 28 has
the Nightmare Press, a belt, a forced scroll (kind-7 type 4), junk blocks, Nightmare Cubes and a Ride
Armor place: [`shark-alt.md`](shark-alt.md) (save state `shark-10_alt`). The teleporter
```

Old:

```
5. The bottom room, and the state after teleporter crystal `id` 1 (the code says stage 20 area 1,
   overlay 28: the hidden area).
```

New:

```
5. The bottom room. (done: `shark-10_alt`, the state after teleporter crystal `id` 1: stage 20 area
   1, overlay 28, [`shark-alt.md`](shark-alt.md).)
```

Old:

```
3. (answered from the code, "The teleporter crystals") Crystal `id` 0 leads to area 1; crystal `id`
   1 leads to the hidden area, stage 20 area 1. A save state after each would confirm it.
```

New:

```
3. (answered, "The teleporter crystals") Crystal `id` 0 leads to area 1 (confirmed by
   `shark-2_section-2`); crystal `id` 1 leads to the hidden area, stage 20 area 1 (confirmed by
   `shark-10_alt`, [`shark-alt.md`](shark-alt.md)).
```

## `docs/stages/scaravich-alt.md`

Old:

```
overlay"). The other hidden areas that have pages are stage 21 area 1 (Turtloid,
```

New:

```
overlay"). The other hidden areas that have pages are stage 20 area 1 (Metal Shark Player,
[`shark-alt.md`](shark-alt.md)), stage 21 area 1 (Turtloid,
```

## `docs/engine.md`

Old:

```
  main type 55 `var` 1 [certain]. Overlays of the hidden areas: stage 21 area 0 = 29, stage 21
  area 1 = 30, stage 22 area 0 = 31, stage 22 area 1 = 32; all fit overlay = 25 + 2 × (stage − 19)
  + area [likely; four points].
```

New:

```
  main type 55 `var` 1 [certain]. Overlays of the hidden areas: stage 20 area 1 = 28
  (`shark-10_alt`), stage 21 area 0 = 29, stage 21 area 1 = 30, stage 22 area 0 = 31, stage 22
  area 1 = 32; all fit overlay = 25 + 2 × (stage − 19) + area [certain: `stage_overlays` (`0x8006DB78`)[38–45]
  = 25–32, and five save states agree]. Their stage members are 118 (stage 20 area 1) to
  122 (stage 22 area 1), in stage and area order, so 115–117 are likely stage 19 areas 0 and 1 and
  stage 20 area 0 [likely].
```

Old:

```
| kind-7 objects (stage 7 background switch, stage 4 Nightmare Press, stage 4 area 1 forced scroll `AutoScroll`) |
```

New:

```
| kind-7 objects (stage 7 background switch, stage 4 Nightmare Press, stage 4 area 1 forced scroll `AutoScroll`, stage 20 area 1 forced scroll `AutoScrollAlt` (type 4)) |
```

Old:

```
    (`docs/enemies/nightmare-press.md`); at the top rest (offset 16) that is y0 + 16 in area 0
    (`var` 32) and y0 in area 1 (`var` 16).
```

New:

```
    (`docs/enemies/nightmare-press.md`); at the top rest (offset 16) that is y0 + 16 in area 0
    (`var` 32) and y0 in area 1 (`var` 16). In stage 20 area 1 the top rest is offset 32 (`var`
    32), so y0 (`shark-10_alt`: layer 1 y 224 = 224 + 32 − 32) [certain].
```

## `docs/enemies/nightmare-press.md`

Old:

```
| 0 | intro: waits while the player step is 0 (beam-in), then moves down | +2.0 for 8 updates (16 px) | count 0 | speed + slow, timer 180, sub 1 |
```

New:

```
| 0 | intro: waits while the player step is 0 (beam-in), then moves down | +2.0 for 8 updates (16 px); stage ≥ 19: 16 updates (32 px) | count 0 | speed + slow, timer 180, sub 1 |
```

Old:

```
  Route "the Nightmare Press and conveyor belt are active, and the screen auto-scrolls" [Fandom]. So
  the stage ≥ 19 case of the kill line (+ 240, deleted at x ≥ 2896 or checkpoint 1) is that area.
```

New:

```
  Route "the Nightmare Press and conveyor belt are active, and the screen auto-scrolls" [Fandom]. So
  the stage ≥ 19 case of the kill line (+ 240, deleted at x ≥ 2896 or checkpoint 1) is that area.
  The save state `shark-10_alt` confirms it ([`docs/stages/shark-alt.md`](../stages/shark-alt.md),
  "The press"). The set-up (`0x800472D0`) gives a stage ≥ 19 a 32 px intro (count 16), so the top
  rest is offset 32 and the kill line is offset + 208 = y 240–320, the same world line as in stage 4.
  In `shark-10_alt`: sub 1, timer 143, offset 32, layer 1 y 224 [certain]. The teeth there (`var`
  66, 68, 70, 82) and the safe floors are in the stage page.
```

Old:

```
- The stage ≥ 19 case is stage 20 area 1 and the "checkpoint 2" stage is stage 18 area 0 ("Other
  users"); no save state of either. (Area 1 of stage 4: see "Section 2".)
```

New:

```
- The stage ≥ 19 case is stage 20 area 1 (save state `shark-10_alt`, the press in its first top
  wait) and the "checkpoint 2" stage is stage 18 area 0 ("Other users"; no save state). (Area 1 of
  stage 4: see "Section 2".)
```

## `docs/enemies/nightmare-cube.md`

Old:

```
2. The cubes of the hidden area stage 20 area 1 (overlay 28 has 3 records of stage object 15): the
   same code is likely. (Stage 7 and stage 22: the same code, `nightmare-crate.md`.)
```

New:

```
2. (answered) The cubes of the hidden area stage 20 area 1 (records 22–24, brown, `id` 2): the
   overlay 28 code is the overlay 07 code after relocation, so they also need `nightmare_effect` 5
   and ride the belt there (the stage ≥ 19 test) [certain for the code;
   `docs/stages/shark-alt.md`]. (Stage 7 and stage 22: the same code, `nightmare-crate.md`.)
```

## `docs/players/ride-armor.md`

Old:

```
  `~/mmx-data/x6/sprites/banks/shark/shark-1_ride/actor2-0-live-0.png`. No other save state has
  a live actor2.
```

New:

```
  `~/mmx-data/x6/sprites/banks/shark/shark-1_ride/actor2-0-live-0.png`. `shark-10_alt` (stage 20
  area 1) has the empty ride of type 1 at (32, 352): the same export ID and the same pixels
  (`banks/shark-alt/shark-10_alt/actor2-1-live-0.png`), texture `0x0140`, CLUT `0x788C`
  (`docs/stages/shark-alt.md`).
```

## `docs/enemies/conveyor-belt.md`

Old:

```
## Summary for a reimplementation

1. Mark the belt surface tiles.
```

New:

```
## Stage 20 area 1 (overlay 28) [certain for the code]

The hidden area of the stage ([`docs/stages/shark-alt.md`](../stages/shark-alt.md), save state
`shark-10_alt`) has a belt and a forced scroll of its own. Both are variants of the code above.

**The belt** (stage logic 15, `BeltUpdateAlt`, `0x800F0984`; record 25 of `fixed_set_tbl`
`0x800F2598`). Cells 58 at y 416, x 96–2751 (one run); the frame store is at x 96–223, y 512–575
(table `0x800F3524`, tile ids saved to `0x800F35AC` by `0x800F04C0`); one end roller (table
`0x800F34FC`: x 2736, y 400, 1 × 2 cells; sources `0x800F3504`, x 2736–2863, y 560). The tile turn
(`0x800F0754`) and the roller draw (`0x800F089C`, frame `0x800F35EC`) are as in section 2. The
differences from `BeltUpdate`:

1. Only `mega+0xE0` = 2 returns at the top. There is no test of the beam-in step (0) and no camera
   lock (the forced scroll locks the camera).
2. **End rule:** camera x ≥ 2752 → `PlaySound(5, 3)`, delete (not the checkpoint). The camera max x
   is 2576 before boss gate 37, so the belt ends when gate 37 moves the camera to x 2848 [likely].
3. The push needs player step ≠ 20, state 1 and HP ≠ 0, then the same foot-cell test (X x −= 1).
4. **The Ride Armor rides the belt:** when `actor2` has a terrain box (`+0x68` ≠ 0), no left-wall
   flag (`+0x70` & 2 = 0) and its foot cell is 58, `actor2.x` −= 1. In section 2 no `actor2`
   exists.

**The forced scroll** (kind-7 type 4, `AutoScrollAlt`, `0x800E9864`, states `0x800F2678`; record
26). Nothing while `mega+0xCC` ≠ 0 or `mega+0xE0` = 2.

| state | does | next |
|---|---|---|
| 0 | checkpoint 1: delete | target (`+0x14`) = camera max x (2576); state 1 |
| 1 | waits for X's x ≥ 128 (not for a step) | walk speed `mega+0xF4` += 0.5 (2.0 → 2.5); state 2 |
| 2 | while camera x < target: camera x += 0.5, min x = max x = camera x (current and target) | camera x ≥ target: walk speed −= 0.5, delete |

So the scroll takes 2576 / 0.5 = 5 152 updates (86 s), and it ends by itself at the target (in
section 2, `game+0x26` ends it). In `shark-10_alt` it waits in state 1 (X at x 79, target 2576).

Other riders in overlay 28: the junk block (records 25, 26, `id` 128) has the overlay 06 code with
the belt test, and the Nightmare Cube (stage object 15) uses its stage ≥ 19 belt test here. The
Mettaurs D2 on the belt are not carried.

## Summary for a reimplementation

1. Mark the belt surface tiles.
```
