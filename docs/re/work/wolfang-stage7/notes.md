# Ice stack block (stage 2 stage object 7): working notes

Stage 2 (Blizzard Wolfang, overlay 04), stage object type 7, and stage logic type 8 (the ice shaft
controller that makes the rows). Proposed name `ice-stack-block`. Export `enemy-583d456ecb`.

## Files

| file | what |
|---|---|
| `sheet.md` | the proposed `docs/enemies/ice-stack-block.md` |
| `pseudo.c` | reconstructed C (plausible, not original, not compiled), with hazard comments |
| `doc-changes.md` | exact text for the other docs and catalogs |
| `helpers-proposed.json` | 40 names for `catalog/helpers.json` (38 overlay 4, 2 main program) |
| `kit/` | `python3 -m mmxdemake.rekit --state wolfang-5_ice-pit.state.gz --type 7 --class stage --name wolfang-stage7` |
| `at/` | `--at` listings: `0x80031F4C`, `0x80032BE8`, `0x80031DA8` (main program), `0x800EDF3C`, `0x800EE038`, `0x800EE208`, `0x800EE258` (stage logic 8) |
| `scripts/peek.py` | live stage object 7 slots, type-6 main objects, player and camera limits of a state |
| `scripts/find_spawns.py` | code in overlay 04 that writes a given type into byte 1 of an object |
| `sprites-root/` | `spriteexport` output for `wolfang-5_ice-pit` (private root) |

Run the scripts from the repo root with `PYTHONPATH=.`.

## Method

1. Read `CLAUDE.md`, `README.md`, `docs/re-guide.md`, the Wolfloid, ice block and ice fall
   controller sheets, the TODO item and [`~/mmx-data/x6/re/purple-wolf/notes.md`](../purple-wolf/notes.md).
2. Kit run: 21 functions, 3 state tables, 1 neighbour (misc type 9, the sparkle). The kit walked
   all of stage object 7. It did not walk two main-program calls of the main state
   (`0x80031F4C`, `0x80032BE8`); read with `--at`. `SolidObject` (`0x80031DA8`) was read to see how
   `+0x72` is set; `docs/engine.md` ("Solid objects", "Squeeze") has the player side.
3. Read all 13 `var` cases of the set-up and the 10 step2 functions.
4. Found who makes `var` 1–5: `scripts/find_spawns.py ... 7` lists 8 writers of type 7. Six are
   in stage object 7; two (`0x800EE0D4`, `0x800EE284`) are in stage logic type 8 (stage logic
   table at `0x800F64A0`, entry 8 = `0x800EE208`; states `0x800F6C34`). Read that object with
   `--at`.
5. Checked the live objects of `wolfang-5_ice-pit` (below), and the other four Wolfang states (no
   stage object 7 in them).
6. Looked at the map (`~/mmx-data/x6/stages/m096-blizzard-wolfang-section-1/stage.png`, cropped):
   pit 1 x 4992–5280, pillar 5280–5344 (top about y 1424), pit 2 x 5344–5632, pit floor y 1616;
   shaft x 5760–6000, floor y 1264, top about y 350.
7. Sprite export (below).

## Key facts and where they come from

- Set-up `0x800EBAA0`: `GetSpriteBank(134)` three times (texture, sprite data, palette); HP 32
  (`a2`… `sb v1,92`), contact 6, terrain `+0x80` from `0x800F6B90`, `+0xA2` = 1, `speed_y`
  `0xFFFF0000`. Case 12: `speed_y` `sll 1` (−2.0).
- The row (`var` 1–5) table `0x800F6BB0`: index `(var − 1) × 8 + i × 2`; gap table `0x800F6C10`
  indexed by `var − 1`. The two tables agree: the gap filler goes to the one place that has no
  block.
- Generator `0x800EC7DC`: `GetRNG` × `0xAAAAAAAB` → `% 6`; `slti 5312` picks the pit; `div` by 128
  or 64 (`game.difficulty` `sltiu 2`); `slti 18`/`slti 24`; full column `== 3`/`== 4`; x
  `48c + 5016`/`+ 5368`; y `1248`; end `slti 5281`/`5633`, camera `+0x24 == 4976`/`5328`, new value
  `5328`/`5712` into `+0x24` and `+0x26`.
- Fall `0x800EC628`: `slti 1296`; `TileCollisionAt` at x ± 32 → `contact_damage` = 127;
  `StageBlockAt` corners → `prev_y`; floor bit 8 → step2 7, sound (2, 17).
- Crush (main `0x800EBF18`): `+0x72 & 4`, copy of `mega` (`0x150` + 4 bytes) to `0x800F76EC`,
  `CollideCheck`, copy `collide_flags & 8`.
- Freeze `0x800EC4DC`: `+0x84 == 32` → `SetAnime 2/3`; `anime.flag == 1` → `CopyTiles`,
  `cam.redraw`, state 2.
- Shaft controller: `GetRNG` × `0x66666667` `>> 1` → `% 5`, `+ 1`; rows at 5880; y `1032 − 144k`
  (`k × 9 << 4`); later rows y 312 every `% 144` (`0x38E38E39 >> 5` = / 144); max 16 (`slti 16`).

## Check against the save state (`wolfang-5_ice-pit`) [certain]

`scripts/peek.py wolfang-5_ice-pit.state.gz`:

```
player x 5179 y 1595 chara 0 state 1 step 2 hp 30
difficulty 1, camera target limits [4976, 4976, 1456, 1248]
stage 0 var 0  state 1 step2 9 act4 2 pos (4992, 1376) 84 173 86..8B [0, 0, 0, 1, 0, 1]
stage 1 var 12 state 1 step2 6 act4 3 pos (5160, 1336) prev_y 1334 spd (0, -2) cd 6 7C 800c1220
stage 2 var 12 state 1 step2 7 act4 5 pos (5256, 1592) prev_y 1590 spd (0, 0) coll 09 disp 1
main 2 type 6 var 5 state 1 step 3 pos (5256, 1551) prev_y 1549 80 = 800c1368 (stage slot 2)
```

- Generator in step2 9 (making blocks); camera target x locked to 4976 [matches the wait code].
- Normal (difficulty 1): period 128. Counter 173: blocks at counter 0 and 128, so 2 blocks
  (`act4` 2) [matches]. Column counts: column 3 and column 5 have 1 each [matches the blocks'
  `act4` 3 and 5].
- Block 1: x 5160 = 5016 + 48 × 3 [matches]. Made at counter 128, so 45 updates ago (the set-up
  update and 44 falls of 2 px): 1248 + 88 = 1336 [matches]. `prev_y` 1334 [matches 2 px/update].
- Block 2: x 5256 = 5016 + 48 × 5 [matches]. Made at counter 0 (173 updates ago): the set-up
  update and 172 falls: 1248 + 344 = 1592 = its floor y (16 × 99 + 8; bottom 1616) [matches]; it
  landed in this update (step2 7, `prev_y` 1590).
- Wolf: `var` 5 (cannon), step 3, `+0x80` = slot 2 [matches]; y 1551 = 1590 − 39 (it followed the
  block's y of the update before) [matches the wolf code].
- Both blocks have contact damage 6: no well yet (the columns beside them are empty).

### Where the "opening" is in this state

There is no fixed opening in the pits: the generator picks a random column and only skips a full
column. In this state:

- X is at x 5179 (terrain box half width 11: x 5168–5190), in **column 3** (x 5136–5184), right
  under block 1. Block 1 is at y 1336 (bottom 1360). X's head is at y 1578 (box centre +2, half
  height 19). The block touches it after (1554 − 1336) / 2 = **109 updates (1.8 s)**. If X stays
  on the floor there, the block breaks and X takes **6** damage (no well: columns 2 and 4 are
  empty) [likely for the damage, from the engine squeeze rule].
- Column 5 (x 5232–5280) has a landed block with a cannon Wolfloid; it becomes tiles in about 48
  updates, and then the wolf is a normal cannon wolf on top of it.
- Free columns now: **0, 1, 2 and 4** (x 4992–5136 and 5184–5232). The next block comes at
  counter 256, in **83 updates**, in a random column (columns 3 and 5 are not full, so all 6 can
  be picked).

The user's words ("one opening among the blocks that X will need to jump through") describe the
ice shaft rows (`var` 1–5), not the pit. A row has 5 places and 4 blocks; the gap is random. The
shaft is further on (x 5760–6000, above y 1264). This state does not show it; that part is from
the code only.

## Sprites

`python3 -m mmxdemake.spriteexport --state wolfang-5_ice-pit.state.gz --root
~/mmx-data/x6/re/wolfang-stage7/sprites-root`:

- New: `enemy-583d456ecb` (stage type 7, live, table `0x800F6B6C`, 8 animations, cell 63 × 63,
  origin (31, 32)). Not in `~/mmx-data/x6/sprites/enemies/`.
- All other exports (`ice-block`, `wolfloid`, `nightmare-virus`, `nightmare-virus-orb`,
  `nightmare-shadow`, `teleport-crystal`, `enemy-78c8f556f5`, `enemy-cd98e86270`) have the same
  IDs as the shared folders: nothing new.
- Checked by eye: each sprite is in one piece and the colours are right (dark blue ice with white
  icicles; the flash frames are light blue; the sparkle is cyan). Sprite 14 is empty (the end
  frame of the sparkle). The sheet `stage-7-live-0.png` and `bank-4.png` are the same file (bank 4
  = sprite type 134). **The sheets are not broken.**
- The sparkle (misc type 9) uses the same sprite source (animation 4), so it needs no export of
  its own.

## Hazards

- **Parent pointers with no test.** A pit block keeps `+0x7C` = its generator. The generator
  deletes itself (state 2) when the player leaves the pit; a block that breaks later decrements
  two bytes of that slot (`act4` and a column byte), which may then be free or reused. Rare in
  the game (a block falls for at most about 180 updates; the player leaves the pit only when it
  is full enough) [likely]. A reimplementation should hold a handle and test it.
- **Row pointer.** A row block reads its row each update (`act4` 1 or 4, and writes its y into
  it). The row deletes itself in the update it lands, after it set `act4` = 1; the blocks run
  after the row in the slot order only if their slots are later. A deleted row reads `act4` 0,
  so a block that missed the landing would fall on forever without a terrain test [guess: the row
  is made first (lower slot) and its blocks after it with `GetStageObjectAfter`, so the order is
  safe]. A reimplementation should let the row tell its blocks directly.
- **Counts without a slot.** The generator adds 1 to the counts also when `GetStageObjectAfter`
  returned 0. Then a column can never be filled to its maximum and the last blocks never come.
  Safe while slots are free.
- **Data that ends the freeze.** Step2 3 ends at the event flag of animation 2 or 3; animations 0
  and 1 have no flag. A changed table would freeze the block at once or never.
- **The shaft controller clears all type 7 slots** (`memset`), also a block that a wolf rides. The
  wolf then reads a free slot: state 0 (not 2 or 4) and y 0, so it moves to y −39 [likely]. Rare.
- **Gap filler in a solid place.** Step2 4 deletes the filler when (x, y) is solid, so it never
  fills an occupied cell; it does not test the player (the player is not a solid rectangle), and
  that is how it kills.
- No recursion: no function calls itself or a caller.

## Open questions

1. No state shows a crush or the shaft. The squeeze damage (6, 16, 127) is from the engine rule.
2. `mega.special` = 2 (`mega+0xE0`): both entries do nothing then. Meaning not known.
3. Camera `+0x2C` (shaft controller: +48 once, −48 once): not in the camera table of
   `docs/engine.md` [guess: a vertical view offset].
4. `var` 6–9 (step2 0, the sliding block): no maker in overlay 04 (the `find_spawns.py` list).
5. The extra gap fillers at row x ± 144: what they close on the map.
6. `SolidMainObjects` (`0x80031F4C`) calls `0x800323D4`, `0x80032D24` and `0x8002E664`; they were
   not read (names are guesses from the pattern of `SolidObject`).
7. The official name: none found; "ice stack block" is descriptive.
