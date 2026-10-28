# Enemy sprites and animations from save states (task I) — design

Date: 2026-10-20. Status: draft for review.

## Purpose

Export the sprites and the animations of every animated object in a stage, from one save state per
stage area, in a form that Megaman-Maverick can use. Maverick keeps one PNG per animation: a
horizontal strip of frames of the same size (for example `frames/Enemies1/Bat/Fly.png`, 2 frames of
32 × 32).

No names are needed in advance. The tool finds each object type's animation table itself, and it
names each export by a fingerprint of its game data (`enemy-<hash>`). The same object in two stages
gets the same fingerprint, so it lands in one folder. The user adds official names later, in a names
file; the next run renames the folders.

## Decisions already made

- One save state per stage area is the input. The user makes these states anyway for the tiles,
  the collision and the objects. No state per enemy.
- The output is game data. It goes to a folder outside the repo. The repo gets only code, tests,
  docs and a small catalog of facts (stage names, enemy names, addresses).
- The tables come from three sources, in this order of trust:
  1. an address in the catalog (`tables`), the last resort for a type that the other two miss;
  2. live objects in the save state (an object holds its table at `+0x30`);
  3. the code search (`animfind`), over the stage overlay and over the main program (object types
     shared by many stages have their code there).
- As many objects as possible: when the sources give several tables for one type, each table is
  exported. Duplicate folders are acceptable.
- Each export is named by a fingerprint of the game's raw data (piece lists, texture cells, palette
  colours, frame timings), not of the drawn PNGs. So a later fix in the drawing does not change the
  names. Exports with the same fingerprint are one folder, with a list of all their sources.
- The output is one folder for all stages: `enemies/<name or id>/` per object, plus bank sheets
  and an index per stage. A run replaces only what its save state contains and keeps all else.
- Frames are exported for one facing: the object's facing byte `+0x15` = 0 (the game's left
  facing). The other facing is the whole frame mirrored (see "Facts"), so Maverick flips it.
- All animated stage object types are exported, not only enemies (also items, doors, platforms).
- Out of scope:
  - semi-transparency (object flag `0x10`; drawn opaque);
  - the disc source of the textures (process log section 54);
  - banks whose textures are not in VRAM at the save point (for example D-1000 in the intro start
    state: a boss-room state is needed);
  - palette changes during play (hit flash and similar);
  - free-standing effect objects (the visual and misc tables); shots and effects that the parent's
    table animates are included with the parent;
  - GIF or other animated previews (the repo has no image library; it writes PNG itself).

## Facts this design depends on

Addresses are RAM addresses (`0x80000000` + offset into the 2 MB RAM copy).

| Fact | Source | Status |
|---|---|---|
| Stage byte at `0x800CCEDC`, area byte at `0x800CCEDD` | `GetSpriteBank` `0x8002A47C`; intro 0/0, Yammark 1/0 | read in code; 2 states |
| Type list pointer: u32 at `0x800733C4 + 4 × (stage × 2 + area)`; the list is bytes, ended by `0xFF` | `GetSpriteBank` | read in code; 2 states |
| Bank index of object type `t` = first position of `t` in the type list; values ≥ `0x80` are not main types | `GetSpriteBank`; intro list | read in code |
| Sprite bank base: u32 at scratchpad `0x1F800020`; bank `i` sprite data = base + u32 at (base + 4 i) | spawner `0x8002A234`–`0x8002A250` | read in code |
| Scratchpad (1 KB) is at decompressed state offset `RAM_OFFSET + RAM_SIZE + 0x80000` | intro state | observed |
| Texture table: u32 pointer at `0x80095098`; bank `i` texture field = (u32 at table + 4 i) >> 7 | spawner `0x8002A1AC`–`0x8002A1C8` | read in code |
| Number of loaded textures = (sprite bank base − texture table) / 4 (intro and Yammark: 9; the words after them are the bank table) | intro, Yammark | observed on 2 states |
| Bank `i` CLUT word = ((482 + i ÷ 4) << 6) \| (4 × (i mod 4)) | spawner `0x8002A1D8`–`0x8002A224` | read in code |
| Sprite data: entry n = {u16 count, u16 offset}; its pieces are at sprite data + 4 × offset; the offset of entry 0 is the number of entries | intro: Junkroid 11, all 9 banks | read in code; observed |
| Piece = {u8 flags, u8 tile, s8 dx, s8 dy}; each piece is a 16 × 16, 4 bpp texture cell | live drawer `0x800232D4` | read in code |
| Palette of a piece: CLUT word + ((flags >> 2) & 3); x = (word & 63) × 16, y = word >> 6 | drawer | read in code |
| Normal piece (flags & `0x20` = 0): `t` = (tile \| (flags & 3) << 8) + texture field, then remapped by the object's byte 0 & `0x60` (table below) | drawer `0x800234D0`–`0x80023600` | read in code |
| Stage enemies have byte 0 = `0x41`, so remap mode `0x40` | spawner `0x8002A150` | read in code |
| Wide piece (flags & `0x20`): x offset = 2 × dx (+1 when flags & `0x80`); `t` = (tile \| (flags & 1) << 8) + texture field, no remap, no flip | drawer `0x8002342C`–`0x800234CC` | read in code |
| Cell of a remapped `t`: VRAM page `p` = `t` >> 8 (x = (p & 15) × 64, y = (p >> 4 & 1) × 256), u = (`t` & 15) × 16 texels, v = (`t` >> 4 & 15) × 16 | drawer: GPU tpage `0xE1000020 \| t >> 8`, u = `t` << 4, v = `t` & `0xF0` | read in code |
| Facing 0: a piece is mirrored left–right when flags & `0x40`, and flipped top–bottom when flags & `0x80`; position (x + dx, y + dy) | drawer `0x80023604`–`0x800237DC` | read in code |
| Facing ≠ 0: position x − dx − 16, and the left–right mirror is inverted: the whole frame is mirrored | drawer `0x800234D0`–`0x800234F8` | read in code |
| Draw order: the last piece is on top (the drawer chains the pieces in list order). Corrected on 2026-10-20: the first reading (first piece on top) came from the Junkroid, whose pieces barely overlap; D-1000 and Meta Wheel F show the right order clearly | drawer `0x800237E0`–`0x80023814`; intro boss-room state | read in code; observed |
| Colour 0 of a palette is transparent; colours through `psxcolor.to_rgba` | PS1; task C | fact |
| Animation table: u32 pointers, one per animation; each animation is a list of frames {u8 ticks, u8 flag, s8 next, u8 sprite} | `SetAnime` `0x800179A4` | read in code |
| A frame lasts `ticks` updates (0 → 256); then the current frame moves by `next` frames (relative); `next` = 0 holds the frame | `AnimeTick` `0x80017A04` | read in code |
| Junkroid (type 1): animation table `0x800FAB24`, 8 animations; animation 1 = sprites 1, 2, 3, 3, 2, 1, ticks 6, 4, 1, 15, 6, 6, flags 0, 0, 1, 0, 0, 2, hold; animation 7 = sprites 5, 6, 7 every 2 ticks, loop to frame 0 | intro start state | observed |
| Animation tables of intro types: 0 Meta Dridler `0x800FAA34`, 1 Junkroid `0x800FAB24`, 2 Meta Wheel F `0x800FACD4`, 5 Hover Gunner `0x800FADC4`, 48 D-1000 `0x800FB138` [likely] | stores to `+0x30` in overlay 02, linked to the main update table | read in code |
| The stage overlay is at `0x800E9860`; its number is the `rock_bin/<nn>.bin` whose first `0x800` bytes equal RAM there (intro 02, Yammark 03; one match each) | intro, Yammark | observed on 2 states |
| Main update table: u32 pointer at scratchpad `0x1F800080` (intro `0x800FA424`, Yammark `0x800F6780`); 77 entries (the visual table follows it, intro `0x800FA558`); an entry inside the overlay is a stage object type | spawner; intro, Yammark | read in code; 2 states |
| An object sets its animation table with `lui rX` + `addiu rX, rX, lo` + `sw rX, 0x30(rY)` (within 12 instructions) | intro: 9 such stores; Yammark: 7 | observed |
| Search result (method in `animfind`): intro types 0, 1, 2, 5, 55 found, 48 missed; Yammark types 3, 4, 23, 28, 37 found, 7 and 49 missed. Every found link agrees with the hand-read code for the 4 intro enemies | probe | observed on 2 states |
| Shared types: an entry of the main update table can point into the main program (text at `0x80010000`, length = executable size − `0x800`). Yammark type 15 (entry `0x80041AB8`) is one; the search over the main program links it to tables `0x80075FB0` and `0x8007789C`. Type 0 in Yammark's list fills empty positions 9–14 (as in the intro) | Yammark state; probe | observed on 1 state |
| A live object holds its own sprite source: `+0x3C` sprite data, `+0x40` texture field, `+0x42` CLUT word. D-1000 (type 48) sets these itself (texture `0x1E0`, CLUT `0x794C`) while its bank has no texture; its cells overwrite banks 5–8 | intro boss-room state | observed on 1 state |
| Stage objects (placement kind 4): 80 slots of `0xA4` bytes from `0x800C1220`, same layout as main objects; update table at scratchpad `+0x94`, 32 entries; exported from live objects only (2026-10-20 addition) | loop `0x800205A0`; Caterpirade and boss gate states | read in code; observed on 3 states |
| Main objects: 48 slots of `0x9C` bytes from `0x8008EF48`; byte 0 ≠ 0 in a used slot, byte 1 = type, u32 at `+0x30` = animation table | Junkroid state: slot 0 = `0x41`, type 1, table `0x800FAB24` (agrees with the search) | observed on 1 state |

Remap by byte 0 & `0x60`:

| mode | remapped `t` |
|---|---|
| `0x60` | ((`t` >> 4) << 8) + (`t` & 15) + `0x600` |
| `0x40` | q = `t` ÷ 160; (q << 8) + (`t` − 160 q) + `0x610` |
| `0x20` | q = `t` ÷ 80; (q << 8) + (`t` − 80 q) + `0x5B0` |
| `0` | `t` |

## Scope

In scope:

1. `sprites` (new): banks, sprite definitions, the remap, drawing one sprite to RGBA pixels.
2. `anims` (new): read an animation table; walk each animation into a frame list with a loop
   point.
3. `animfind` (new): find the stage overlay; find the animation table of each stage object type,
   from live objects and from the code.
4. `fingerprint` (new): the content ID of an export.
5. `spriteexport` (new): the command. Reads one save state and the catalog; writes the bank sheets,
   the animation strips and the stage index; merges with earlier runs; applies the names.
6. `catalog/enemies.json` (new): stage names, enemy names by ID, and table addresses as a last
   resort.

## Repository layout (additions)

```
mmxdemake/
  sprites.py        # banks, sprite definitions, remap, drawing
  anims.py          # animation tables and sequences
  animfind.py       # overlay, live objects and code search
  fingerprint.py    # content IDs
  spriteexport.py   # the command
catalog/
  enemies.json      # stage names, enemy names, last-resort addresses (facts only)
tests/
  test_sprites.py
  test_anims.py
  test_animfind.py
  test_fingerprint.py
  test_spriteexport.py
  test_retail_sprites.py
```

Changed: `tests/synth.py` (a save state with RAM, scratchpad and VRAM, if the current builder does
not cover the scratchpad), `README.md`, `docs/TODO.md`.

Imports: `sprites` uses `savestate`, `vram` and `psxcolor`. `anims` and `animfind` use
`savestate`. `fingerprint` uses `sprites`. `spriteexport` uses `sprites`, `anims`, `animfind`,
`fingerprint`, `png`, `digits` and `outdir`.

## Units

### `sprites`

- Constants: the addresses and numbers in "Facts" (`STAGE`, `AREA`, `TYPE_LISTS`, `BANK_BASE`,
  `TEXTURE_TABLE`, `CLUT_Y0 = 482`, `ENEMY_MODE = 0x40`, `CELL = 16`, `SCRATCHPAD_OFFSET`,
  `SCRATCHPAD_SIZE = 0x400`).
- `SpriteError(Exception)`.
- `scratchpad(state) -> bytes`.
- `Bank` (a small record): `index`, `type` (the byte in the type list), `data` (address),
  `count` (sprites), `texture` (texture field, or `None` when not loaded), `clut` (CLUT word).
- `banks(ram, scratch) -> list[Bank]`: one per type-list position before `0xFF`, and only while
  the bank table has an entry (position < offset of bank 0 / 4). `texture` is `None` for positions
  ≥ the number of loaded textures.
- `bank_of_type(banks, t) -> Bank | None`: the first bank with that type.
- `pieces(ram, data, n) -> list[(flags, tile, dx, dy)]`. Refuse when n ≥ count.
- `cell(t, flags, texture, mode) -> (page, u, v)`: the remap and the cell position, for normal and
  wide pieces.
- `cell_bytes(vram, page, u, v) -> bytes`: the 128 bytes of one 16 × 16 4 bpp cell, row by row.
- `palette_words(vram, clut) -> list[int]`: the 16 colour words of one palette.
- `draw(ram, vram, bank, n, mode=ENEMY_MODE) -> Image`. `Image` = (width, height, ox, oy, pixels):
  the tight box around the opaque pixels, `ox`, `oy` = the object's position inside it, `pixels` =
  rows of RGBA tuples. Facing 0, last piece on top. An empty sprite gives a 1 × 1 transparent
  image with `ox = oy = 0`.
- Refuse (`SpriteError`) when a cell or a palette lies outside VRAM, or when the bank has no
  texture.

### `anims`

- Constants: `MAX_ANIMS = 64`, `MAX_STEPS = 256`, `FPS = 60`.
- `AnimError(Exception)`.
- `read_table(ram, addr, lo, hi) -> list[int]`: the animation pointers from `addr`, until a word is
  not in `[lo, addr)` (frames come before the table, inside the overlay) or `MAX_ANIMS` is
  reached. `lo` and `hi` are the overlay bounds. Refuse when the first word fails the test.
- `walk(ram, ptr) -> (frames, loop_to)`: start at frame 0; record each frame as
  `{"sprite", "ticks", "flag"}` (`ticks` 0 → 256); `next` = 0 → stop, `loop_to = None`; else move
  by `next`; arriving at a frame already recorded → stop, `loop_to` = its position. Refuse after
  `MAX_STEPS` frames.

### `animfind`

- Constants: `OVERLAY_BASE = 0x800E9860`, `OVERLAY_PROBE = 0x800`, `MAIN_TABLE = 0x80` (scratchpad
  offset), `MAIN_TYPES = 77`, `ANIM_FIELD = 0x30`, `LOOKBACK = 12`, `PAIR_GAP = 5`, `RUN_MAX = 64`,
  `SLOTS = 48`, `SLOT_BASE = 0x8008EF48`, `SLOT_SIZE = 0x9C`.
- `FindError(Exception)`.
- `find_overlay(ram, overlays: dict[int, bytes]) -> (number, lo, hi)`: the overlay whose first
  `OVERLAY_PROBE` bytes equal RAM at `OVERLAY_BASE`; `lo = OVERLAY_BASE`, `hi = lo + len`. Refuse
  unless exactly one matches.
- `stage_types(ram, scratch, lo, hi) -> dict[int, int]`: main type → its update function, for the
  main table entries inside the overlay.
- `program_bounds(exe) -> (lo, hi)`: `0x80010000` and `0x80010000 + len(exe) − 0x800`.
- `live_tables(ram, regions) -> dict[int, set[int]]`: for each used slot (byte 0 ≠ 0) whose type is
  < `MAIN_TYPES` and whose `+0x30` value is inside one of the regions (overlay, main program):
  type → the set of those values.
- `stores(ram, lo, hi) -> list[(pc, table)]`: each `sw rX, 0x30(rY)` in the overlay where, in the
  `LOOKBACK` instructions before it, the last write of `rX` is `addiu rX, rX, lo16` and the
  `lui rX, hi16` before that; `table = (hi16 << 16) + signed lo16`, kept only when inside the
  overlay.
- `function_start(ram, pc, lo) -> int`: walk back from `pc` to the first word after a
  `jr ra` + delay slot, or to an `addiu sp, sp, −n`, whichever comes first; not below `lo`.
- `references(ram, lo, hi) -> dict[addr, set]`: for each overlay word:
  - `j` / `jal` target → the function that contains the instruction;
  - `lui` + `addiu` pair (the `addiu` within `PAIR_GAP` instructions, same base register) whose
    value is inside the overlay → that function;
  - a data word whose value is inside the overlay → `("data", its address)`.
- `owners(func, refs, entries) -> set[int]`: the main types that reach `func`. A function that is a
  main table entry gives its type. Else follow every reference back. A data reference stands for
  the run of consecutive in-overlay words around it (back up to `RUN_MAX` words): follow the code
  references to any word of that run. Each function is visited once.
- `searched_tables(ram, scratch, lo, hi) -> dict[int, list[int]]`: main type → the distinct tables
  whose stores it owns, in address order.
- `choose(catalog_table, live, searched) -> (list[(table, found)], note)`:
  - a catalog address → only it, `found = "catalog"`;
  - else every table from the live values and the search, in address order; `found = "live"` for
    a table that a live object holds, else `"search"`; with more than one, the note
    `<n> tables: <list>; each is exported`;
  - none → an empty list and the note `no live object and no table found by the search`.
- The search runs once per region (overlay, main program). A region only links the main types
  whose update function lies inside it.

### `fingerprint`

- Constants: `VERSION = 1`, `ID_HEX = 10`, `PREFIX = "enemy-"`.
- `content_id(ram, vram, bank, animations, mode) -> str`: `PREFIX` + the first `ID_HEX` hex digits
  of the SHA-1 of a canonical JSON text (sorted keys, no spaces) of:
  - `version`: `VERSION`;
  - `animations`: for each animation in table order, its frames as `[sprite, ticks, flag]` and
    its `loop_to`;
  - `sprites`: for each sprite number that the animations use, in number order, its pieces as
    `[flags & 0xF0, dx, dy, cell bytes as hex, 16 palette words]`.
- The tile number, the low flag bits (tile and palette selection), the addresses and the bank
  index are left out: they depend on where the stage loads the data, not on the content.

### `spriteexport`

- Arguments: `--state` (file name in `--dumps`), `--dumps` (default `~/mmx-data/x6/dumps`),
  `--extracted` (default as `learn`), `--catalog` (default the repo `catalog/`), `--root` (default
  `~/mmx-data/x6/sprites`).
- Output under `--root`:
  - `enemies/<folder>/`: `anim-<k>.png` per animation, and `animations.json`. `<folder>` is the
    catalog name of the ID, else the ID.
  - `banks/<stage name>/<state stem>/bank-<i>.png`: all sprites of each loaded bank in a grid,
    with the sprite number above each cell (`digits`).
  - `stages/<stage name>.json`: `{"states": {<state stem>: {"area", "state_sha1", "enemies":
    [ids], "skipped": [{"type", "reason"}]}}}`.
  - The stage name comes from the catalog (`stages`), else `stage-<s>`.
- `animations.json`: `{"id", "fps": 60, "cell": [w, h], "origin": [x, y], "animations":
  [{"index", "frames": [{"sprite", "ticks", "flag"}], "loop_to"}], "sources": [{"stage", "area",
  "type", "bank", "table", "found", "state", "state_sha1"}]}`. `sources` is sorted.
- Steps:
  1. Read the state, RAM (with the executable check of `read_ram`), scratchpad and VRAM;
     `find_overlay` with the overlays in `<extracted>/rock_bin`.
  2. `banks`; draw the bank sheets.
  3. The object types of this state: each type in the type list (< `0x80`) whose bank has a texture
     and whose update function is in the overlay or the main program. For each, `choose` the
     tables. No table → `skipped`, with the note.
  4. For each chosen table: `read_table` (bounded by the region that holds the table), `walk` each
     animation. Every sprite number must be < the bank's count; else that table is `skipped` (exit
     1 when the table came from the catalog). Then `content_id`. Tables with the same ID make one
     export with several sources.
  5. Frames: one cell per frame, left to right. All cells of one export have the same size: the
     union of the boxes of every sprite its animations use, placed by `ox`, `oy`.
  6. Merge with the earlier output:
     - an export whose folder already holds the same ID: its `sources` = old ∪ new (one entry
       per (stage, area, type, state)); the PNGs are replaced;
     - `stages/<stage name>.json`: this state's entry is replaced, the others stay;
     - `banks/<stage name>/<state stem>/` is replaced;
     - nothing else changes.
  7. Names: every folder in `enemies/` whose ID has a catalog name and whose folder name differs is
     renamed to the name. So a run after a names update renames all named folders.
  8. Everything is written to `<root>.tmp` (made fresh) first; then each item is moved into
     `<root>`, the old item removed before; then `<root>.tmp` is removed.
- `outdir.check` refuses a root inside the repo, or one that holds anything but `enemies`, `banks`
  and `stages`. An `enemies/` folder without a readable `animations.json` with an `id` refuses.
- Prints one line per bank and per export (folder, ID, sources with type and how found, number of
  animations), one line per skipped type with the reason, then a summary line:
  `<state>: stage <s> area <a>, <n> banks drawn, <m> without texture, <e> exports (<l> live,
  <f> search, <c> catalog), <k> skipped, <r> renamed`.

### `catalog/enemies.json`

```json
{
  "stages": {"0": "intro", "1": "yammark"},
  "names": {"enemy-0123456789": "junkroid"},
  "tables": [{"stage": 0, "type": 48, "animations": "0x800FB138"}]
}
```

- `stages`: stage number → folder name.
- `names`: ID → name. Names follow the official enemy list (see `docs/prior-work.md`); they are
  lower case with dashes (`[a-z0-9-]+`), unique, and do not start with `enemy-`.
- `tables`: the last resort for a type that live objects and the search both miss. Sorted by
  (stage, type).
- The first version has `stages` only. Names come after the first runs, when the IDs are known
  (the four intro enemies first). D-1000 is expected to come from a live object in a boss-room
  state, so no address is added in advance.

## Error handling

- **0**: done.
- **1**: a failed check (`SpriteError`, `AnimError`, `FindError`, `VramError`, a save state that
  fails the executable check, a catalog address that gives a bad table). Nothing in `<root>` is
  changed.
- **2**: bad arguments, a refused root (`outdir.check`), or a malformed catalog (bad JSON, a bad
  ID or name, a name used twice).
- A type without a loaded bank, without a table, or whose live or found table gives a bad sprite
  number is skipped with a line and recorded in the stage index, not an error.
- A failure while moving items in step 8 can leave a mix of old and new items. The message names
  the item, and a re-run of the same state repairs it.

## Testing

- `test_sprites` (synthetic RAM, scratchpad and VRAM):
  - `banks` reads the type list to `0xFF`, the counts from the offsets, `texture = None` past the
    loaded count, and the CLUT word for i = 0, 3, 4, 13.
  - `cell` gives the table results for each mode, for a wide piece, and at a page edge (t = 159 and
    160 in mode `0x40`).
  - `draw`: a 2-piece sprite with known cells and palette gives the expected pixels; an overlap
    shows the first piece; flags `0x40` and `0x80` mirror and flip the cell; a wide piece is
    placed at 2 × dx; colour 0 is transparent; the box and origin are right; a cell outside VRAM
    refuses; a bank without texture refuses.
- `test_anims`: a hold animation, a loop to frame 0, a loop to a middle frame, ticks 0 → 256, the
  table end at a word outside `[lo, addr)`, and a runaway list refused after `MAX_STEPS`.
- `test_animfind` (synthetic overlay code and slots):
  - `find_overlay` with one, zero and two matches;
  - `live_tables`: a used slot gives its table; a free slot, a type ≥ 77 and a table outside the
    regions are ignored;
  - a type whose update function is in the main program region is linked by a search over that
    region;
  - a store found through `lui` + `addiu` + `sw`, and one skipped when `rX` is written in between;
  - a type linked through a direct `jal`, through a pointer table (data run), and through two
    levels of calls; a function reached by two types gives both; a type with two tables lists both;
  - `choose`: a catalog address alone; otherwise all live and searched tables, each tagged, with a
    note when there are several; no source → an empty list.
- `test_fingerprint`: the same content at another tile number, bank index and address gives the
  same ID; a change in one palette colour, one cell byte, one tick or one `loop_to` gives another
  ID; the ID has the form `enemy-` + 10 hex digits.
- `test_spriteexport`:
  - a synthetic state and catalog give the expected files, JSON and strip sizes;
  - two types with the same content give one export with two sources;
  - a second state of another area adds its export and leaves the first export's files unchanged
    (same bytes, same modification time); its stage index keeps both states;
  - a second state with the same object in another stage adds a source to the same folder;
  - a re-run of the same state gives equal bytes and does not repeat a source;
  - a names update followed by a run renames the folder and keeps its content;
  - a catalog address overrides the live and searched tables;
  - a type with two live tables gives two exports;
  - a type whose code is in the main program is exported (found by the search over that region);
  - a root inside the repo, or holding a foreign file, gives exit 2; a malformed catalog (bad ID,
    name used twice) gives exit 2; a failed check gives exit 1 and changes nothing.
- `test_retail_sprites` (skipped unless `MMX6_DUMPS` and `MMX6_EXTRACTED` are set):
  - the intro start state gives 9 drawn banks, finds types 0, 1, 2 and 5 with the tables of the
    facts table, and the Junkroid animations of the facts table; Junkroid sprite 0 has a fixed size
    and opaque pixel count (numbers only, no game data in the repo);
  - the Junkroid state gives type 1 from a live object, and the same Junkroid ID as the start
    state;
  - the Yammark state finds types 3, 4, 23, 28 and 37 in the overlay, and type 15 in the main
    program.
