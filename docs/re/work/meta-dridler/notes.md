# Notes: Meta Dridler

Behaviour sheet: [`~/IdeaProjects/mmx-demake-tools/docs/enemies/meta-dridler.md`](../../findings/enemies/meta-dridler.md). Method and tools
as in `../hover-gunner/notes.md`.

Entry point: main table `0x800FA424` entry 0 = `0x800EA1D8`. The effects: `GetMiscObject` + type 0
(misc table `0x800FA814` entry 0 = `0x800EA224`).

## Evidence files (`disasm/`)

| file | contents |
|---|---|
| `dridler_overlay02_800e9f1c-800eac8c.txt` | all drill and effect code |
| `dridler_data_800fa99c-800faadc.txt` | boxes, animations, debris list, state tables, tile-copy blocks |
| `h_tilecopy_shake_explosion.txt` | `0x80027BA0` (tile block copy), `0x80029478` (quake), `0x80049EBC` (random explosion) |

## Checked

- The nine records and their ids and kinds in the intro list.
- The fall time (47 frames for 128 px) by simulating `MoveObject2` with the start speed and gravity.
- The layer work stride `0x54` (from `0x8002CAB4`), used for the `BGLAYER` struct size.

## Open questions

1. The rock block. Checked with `layout.json` and the collision screens: the template cells at
   y 1136 are rock tiles 2237–2248 (top row art only, two rows solid); the destination cells at
   y 880–927 are empty, above the floor surface at y 944. A picture with both areas boxed:
   `~/mmx-data/x6/preview/dridler-rock-block.png`. Open: a 128 px drop from y 736 leaves the tip at
   y 902, 42 px above the floor, so these drills may never land and the block may never appear.
   Watch drills 46–48 in play. (The first reading, "breaks a hole in the floor", was wrong: the copy
   goes from y 1136 to y 880, and the source is solid.)
2. `0x800971F8 + 0x4C = 1` after the copy: probably a "redraw the map" request.
3. The quake condition (`+0x20 == +0x0E` of the layer-0 camera work) and the meaning of the three
   quake arguments.
4. Sounds bank 2, ids 1, 2, 3: names.
5. Sprites: which sprite is the drill body and which are the effects. Effect 0 is drawn in front
   (priority 34) while the drill rises and behind (32) while it drops.
6. The drop warning lasts through the drop, the rise warning through the rise: check in play.
