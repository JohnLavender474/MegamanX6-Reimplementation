# The "purple Zero" in `wolfang-2_ice-slope`: notes

## Result

The purple (magenta) Zero next to X is a **Nightmare shadow**: main object type 26, `var` 10, the
same object as `docs/enemies/nightmare-shadow.md` and the export `nightmare-shadow`
(`enemy-62d9963d2c`). It is not the Zero Nightmare boss (main type 53, boss overlay 43,
`enemy-71f965d1d8`), and it is not a palette variant. The export already lists this state as a live
source. The code in overlay 04 is a copy of the overlay 09 code, with the same immediates and the same
data. So the sprites, animations and behaviour are **already covered** [certain].

What is new is a set of corrections and answers for the existing sheet (see `doc-changes.md`):

1. The sheet says that `var` 10 (player X) is an "X shadow". It is not: `var` 10 uses animations 0
   and 1 (sprites 1 and 0), and these sprites are a **Zero** silhouette (long hair, helmet fins)
   [certain for `var` 10 → animation 1 in this state; likely for the shape, by eye and by the user's
   report "purple Zero" with X as the player]. So X sees Zero shadows. `var` 11 (player Zero) uses
   sprites 3 and 2, in cyan; that they show X is a [guess] (the shapes are not clear).
2. The cyan colour is in the sprite data: every piece of sprites 2 and 3 has piece flags `0x04`, so
   `DrawObject4` uses CLUT `+0x42` + 1. The code copies one CLUT to all shadows, and the sprite data
   picks the second palette [certain]. This answers open question 4 of the sheet.
3. The spawners come from the **fixed record list** (`fixed_set_tbl`, `0x80073554`), kind 0, type 26,
   `id` = `var` [certain]. This answers open question 1.
4. Nightmare Effect 7 comes from the pick of stage 7 (Shield Sheldon): its targets are `2 6 6`, so
   stages 2 (Wolfang) and 6 (Turtloid) get 7 [certain for the bytes; this is the rule already in
   `docs/engine.md`]. The weapon ids 7 and 16 that kill a shadow are Guard Shell, Sheldon's weapon
   (`docs/players/weapons.md`, slot 7) [likely]. This answers open questions 2 and 3.

## Method

- Live objects of all classes: `scripts/live.py wolfang-2_ice-slope.state.gz`.
- `spriteexport --state wolfang-2_ice-slope.state.gz --root sprites-root` (this folder). The
  `nightmare-shadow` strips are byte-identical to the shared export
  (`~/mmx-data/x6/sprites/enemies/nightmare-shadow/anim-0.png`, `anim-2.png`, checked with `cmp`).
  All sheets checked by eye. The other exports in `sprites-root` are the normal stage 2 objects; none
  of them is new for this topic.
- Kit: `python3 -m mmxdemake.rekit --state wolfang-2_ice-slope.state.gz --type 26 --name
  wolfang-zero-nightmare --out ~/mmx-data/x6/re/wolfang-zero-nightmare/kit` (11 functions, 3 state
  tables). `kit/code.txt` was compared with `../nightmare-shadow/kit/code.txt` after the addresses
  were removed: 562 lines each, the only differences are the table addresses and the names on the
  notes.
- Data: the 0x68 bytes at `0x800F6D90` (overlay 04) and `0x800F6328` (overlay 09) are the same
  boxes and animation table; only the pointers differ.
- Bank sprites: `sprites.pieces` on bank 7 (piece flags, see below), palettes from VRAM.
- Fixed record lists of all dumped stage areas: `scripts/fixed26.py 26`.
- Extra kits (scratch only): type 54 (Wolfang boss, see below) and type 19 (Nightmare Effect 3).
- The preview picture at offset `0x25` of this state is black with a red frame and the slot digit;
  it does not show the scene. The magenta Zero comes from the user's report and the live object.

## Save-state values (`wolfang-2_ice-slope`)

- Stage 2 area 0, character 0 (X), form 1 (Falcon), X at (640, 1131).
- `nightmare_effect` (`0x800CD30A`) = 7. `game.stage_effect` (stages 0–8) = `0 6 7 4 2 4 7 5 4`;
  `game.effect_new` = `4 4 4` (the last pick was stage 2 itself; its targets are `4 4 4`). The same
  values are in `wolfang-1_x-sabre` and `wolfang-3_falling-ice`. So stage 2 has effect 7 from an
  earlier pick of stage 7, and its portrait has no Nightmare mark (the rule in `docs/engine.md`).
- Type list: `128 15 141 6 134 17 19 26 144 28 0 0 0 0 54 54`. Type 26 is bank 7 (sprite data
  `0x80158204`, 4 sprites, texture `0x440`, CLUT `0x78CC`).

| slot | type | `var` | state / step | position | what |
|---|---|---|---|---|---|
| main 0 | 26 | 0 | 1 / 1 (SPAWN) | (0, 0) | spawner, zone 321 ≤ x ≤ 2559: the player is in it |
| main 1 | 26 | 1 | 0 / 0 | (0, 0) | spawner, zone 3201 ≤ x ≤ 4159: waits |
| main 2 | 26 | 10 | 1 / 2 (DASH) | (606, 1131) | **the purple Zero**: animation 1 (sprite 0), CLUT `0x78CC`, `display` 1 |
| main 3 | 6 | 0 | 1 / 6 | (768, 1119) | `purple-wolf` (an ordinary enemy) |

The shadow is at the player's height (y 1131), 34 px behind X, in its dash. It was made 32 px in
front of X and dashes at the point where X was at the end of its stand [likely, from the position].

## Sprites and palettes of bank 7

| sprite | pieces | piece flags | CLUT | colour | shape |
|---|---|---|---|---|---|
| 0 | 10 (tiles 10–19) | `0x00` | `0x78CC` | magenta | Zero, dash (hair behind) [likely] |
| 1 | 10 (tiles 0–9) | `0x00` | `0x78CC` | magenta | Zero, standing (helmet fins, long hair) [likely] |
| 2 | 7 (tiles 20–26) | `0x04` | `0x78CD` | cyan | dash pose, X [guess] |
| 3 | 9 (tiles 27–35) | `0x04` | `0x78CD` | cyan | standing pose, X [guess] |

- CLUT `0x78CC`: colour 0 = `0x0000`, colours 1–15 = `0xFC1F` (r 248, g 0, b 248).
- CLUT `0x78CD`: colour 0 = `0x0000`, colours 1–15 = `0xEFE9` (r 72, g 248, b 216).
- The same words are in `turtloid-1` (CLUT `0x7944`/`0x7945`, bank 13), and the bank 13 sheet of
  `turtloid-1` is the same picture. So the colours are fixed data, not a stage palette.
- Both colours have **bit 15 set** (the colour word, not the `+0x42` field). On the PlayStation this
  is the semi-transparency bit of a texel; the shadow is semi-transparent only if the draw command
  has semi-transparency on. `DrawObject4` was not read for this [open]. The `+0x42` field has no
  bit 15 (no hit flash).
- The shadows are larger than the player: sprite 1 is about 64 px high; X is about 40 px.

## Spawners: the fixed record list

`scripts/fixed26.py 26` (one state per stage area):

| stage, area | list | records (index, `id`) | state |
|---|---|---|---|
| 2, 0 (Wolfang) | `0x800F6124` | (11, 0), (12, 1) | `wolfang-1_x-sabre` |
| 6, 0 (Turtloid) | `0x800F5378` | (10, 2), (11, 3) | `turtloid-1` |
| 21, 1 (Turtloid alternative) | `0x800EF1DC` | (2, 4) | `turtloid-4_alt` |

- The records are kind 0, type 26, x = y = 0. The spawn code (`SpawnView` runs the fixed list) makes
  them as main objects with `var` = `id`. That is why main 0 and 1 are at (0, 0) [certain for the
  records; likely for the path through `SpawnView`].
- So the zone table in the shared code is one table for all stages: `var` 0–1 stage 2, `var` 2–3
  stage 6, `var` 4 stage 21 area 1. No dumped list has `var` 5 [open].
- No other code in the main program or overlay 04 makes a main object of type 26 (search for
  `addiu rX, zero, 26` near `jal GetMainObject`: only the spawner itself, `0x800EFB40`).
- `nightmare_effect` is read only by the type 26 set-up (`0x800EF750`) in overlay 04 and the main
  program. `game+0x43A` is also read at `0x800EEEC0` (type 19, below).

## How Nightmare Effect 7 gets to stage 2

1. The stage pick of stage 7 (Shield Sheldon) writes 7 to `game.stage_effect`[2] and
   `game.stage_effect`[6] (targets `2 6 6`, table `0x800F0F14`, overlay 16) — the rule in
   `docs/engine.md`. Later picks (here stage 2, targets `4 4 4`) do not write over stage 2.
2. `StageStartReset` copies `game.stage_effect`[2] = 7 to `nightmare_effect` at the start of
   stage 2 area 0.
3. The fixed list of stage 2 area 0 makes the two spawners (`var` 0, 1). Their set-up
   (`0x800EF738`) deletes them unless `nightmare_effect` = 7.
4. Spawner 0 (321 ≤ x ≤ 2559) counts 300/400/500 frames while X is in its zone, then makes a shadow
   with `var` 10 (X) or 11 (Zero) by `mega+0x02`. `var` 10 shows the magenta Zero.

Stage 6 works the same way with `var` 2 and 3; its sheet zones are the same table.

## Other objects in the type list (not the purple Zero)

- **Type 54, banks 14 and 15, no texture**: the Blizzard Wolfang boss [likely]. Its kit (120
  functions, entry `0x800F2E8C`) calls `BossLevel`, `BossMusicRequest`, `TextOpen` and
  `PlayerEvent`; its record is index 66, (6485, 416), at the end of the stage. Its texture is
  loaded at the boss room. Not related to the shadow.
- **Type 19 (bank 6, export `enemy-78c8f556f5`)**: its set-up (`0x800EEEA0`, state table
  `0x800F6D5C`) deletes the object unless `game+0x43A` = 3. So it is the stage 2 object of
  Nightmare Effect 3 (the pick of stage 3, targets `2 8 8`) [certain for the test]. Its record is in
  the fixed list (index 15, `id` 1). `docs/engine.md` does not list value 3 yet. The table
  `0x800F6D58` that `spriteexport` takes as its animation table is next to the state table; not read.

## Open

1. `var` 5 (zone 321 ≤ x ≤ 2143, y < 1072): which stage list uses it.
2. Whether the shadows are drawn semi-transparent (bit 15 in the colour words; the draw mode of
   `DrawObject4` was not read).
3. The shape of sprites 2 and 3 (`var` 11, player Zero): X is a guess. A state with Zero as the
   player in stage 2 or 6 with effect 7 would show it.
4. Type 19 of stage 2 (Nightmare Effect 3) has no sheet.
