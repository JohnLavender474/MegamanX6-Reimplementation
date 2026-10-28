# Stage screens with real tile textures, greyscale (sample task E) — design

Date: 2026-10-18. Status: draft for review.

## Purpose

Draw the screens of the 23 stage members with their real tile textures, so a
person can see stage art instead of collision blocks. Colours are greyscale in
this task; the palette source is not known yet and is a separate step. The
renderer takes the shading as an input, so a real palette can replace
greyscale later without changes to the renderer.

## Constraint

Use only the task A output. No emulator and no RAM dump.

## Facts this design depends on

| Fact | Source | Status |
|---|---|---|
| Entry `0x10000` of a stage member is the tile texture block, 262,144 bytes | X4 editor `Level.cs` (facts only); X6 member 94 | size verified on member 94 |
| 4 bpp pages 0–7: page `p` at `p × 0x8000`, 128 bytes per row, 256 rows; low nibble = left pixel | X4 editor; X6 member 94 pages 0–3 drawn as clean tile art | verified by eye (pages 0–3) |
| 8 bpp pages 8–11: page `p` is two halves of 128 bytes per row, at `(p & 3) × 0x10000` (columns 0–127) and `+ 0x8000` (columns 128–255) | X4 editor; X6 member 94 page 9 rebuilt as smooth art | verified by eye on page 9 only; pages 10, 11 unverified |
| Pages above 11 draw nothing | X4 editor | X6 member 94: 5 tiles on page 15 |
| Tile info word: bits 8–15 palette number, bits 16–19 cell x, bits 20–23 cell y, bits 24–27 page; a tile is the 16 × 16 cell at (x, y) in its page | X4 editor; X6 member 94 (all 256 cells used) | verified |
| Entry `0x16` (also 262,144 bytes) is not plain texture data | probe, 2026-10-18 | observed (striped noise in every reading) |
| Screens, tile info and stage members as in task D | task D | verified |

## Scope

In scope:

1. Read a tile's raw 16 × 16 values from the texture block (4 bpp and 8 bpp).
2. Turn raw values into RGBA through a shading function; greyscale now.
3. For each stage member: a texture atlas of the used cells, all screens drawn
   with their tiles, and a summary with per-page counts and the 4 bpp / 8 bpp
   overlap count.

Out of scope:

- Palettes and real colours.
- Flip flags (recorded in task D, not applied here, as in the X4 editor).
- Entry `0x16`.
- Layout, enemies, which member is which stage.

## Repository layout (additions)

```
mmxdemake/
  texture.py   # tile reference from tile info; raw tile values from the block
  shade.py     # raw values -> RGBA (greyscale now)
  tiles.py     # CLI
tests/
  test_texture.py
  test_shade.py
  test_tiles.py
  test_retail_tiles.py   # runs only when MMX6_EXTRACTED is set
```

Imports: `texture` and `shade` import no project module. `tiles` uses
`texture`, `shade`, `arcentries`, `stagemap`, `collision.stage_records`,
`collision.map_records`, `index`, `png`, `outdir`.

## Units

### `texture`

- `BLOCK_SIZE = 0x40000`, `PAGE_4BPP = 0x8000`, `ROW = 128`, `TILE = 16`.
- `TileRef(page, cx, cy, palette)`, frozen dataclass; `tile_ref(w)` reads the
  fields from a tile info word.
- `bits(page) -> int | None`: 4 for pages 0–7, 8 for pages 8–11, `None`
  above 11.
- `raw_tile(block, ref) -> list[int] | None`: 256 values in row order, or
  `None` when `bits(page)` is `None`. `block` must be `BLOCK_SIZE` bytes, else
  `ValueError`.
  - 4 bpp: for pixel row `r` and column `c`, the byte is at
    `page × 0x8000 + (cy × 16 + r) × 128 + cx × 8 + c // 2`; even `c` takes the
    low nibble, odd `c` the high nibble.
  - 8 bpp: with `x = cx × 16 + c` and `base = (page & 3) × 0x10000`, the byte
    is at `base + (cy × 16 + r) × 128 + x` when `x < 128`, else at
    `base + 0x8000 + (cy × 16 + r) × 128 + (x − 128)`.
- `covered_4bpp_cells(ref) -> set[tuple[int, int, int]]`: for an 8 bpp ref,
  the two 4 bpp cells `(page4, x4, y)` whose bytes it reads:
  `page4 = 2 × (page & 3) + (1 if cx >= 8 else 0)`, `x4 ∈ {2 × (cx % 8),
  2 × (cx % 8) + 1}`, `y = cy`. Empty for other refs.

### `shade`

- A shading function has the form `shade(values, bits, palette) -> bytes`:
  256 raw values in, 1,024 bytes of RGBA (row order) out.
- `grey(values, bits, palette)`: 4 bpp → `v × 17`; 8 bpp → `v`; alpha 255.
  `palette` is ignored.

### `tiles` (CLI)

```
python3 -m mmxdemake.tiles \
  --extracted ~/mmx-data/extracted/retail \
  --out ~/mmx-data/tiles [--member 94] [--expect-stages 23]
```

- Paths may start with `~`. Load `index.json` (schema 1).
- Without `--member`: `collision.stage_records`; the count must equal
  `--expect-stages` (default 23). With `--member`: `collision.map_records`.
- For each member: check size and SHA-1 against the index; `split`; take
  `only(entries, 0)`, `only(entries, 1)`, `only(entries, 0x10000)`; decode
  screens and tile info; `stagemap.check`.
- Output, in `<out>.tmp/` then renamed to `<out>/`:
  - `mNNN/screens.png`: all screens in index order, 8 per row, each 256 × 256
    (16 × 16 tiles of 16 × 16 px), with a 4 px gap of `(60, 60, 60)`. Tile 0
    and tiles whose page is above 11 are black `(0, 0, 0)`.
  - `mNNN/pages.png`: pages 0–11 in a 4 × 3 grid (page `p` at column `p % 4`,
    row `p // 4`), each 256 × 256, 2 px gap of `(60, 60, 60)`. Only the cells
    that a used tile references are drawn, each in its own page's reading;
    other cells are `(32, 0, 32)`.
  - `mNNN/summary.txt`:
    - `member 94: 137 screens, 3097 tiles used`
    - one line per page with used tiles, in page order:
      `page 2 (4bpp): 516 tiles, 256 cells`,
      `page 9 (8bpp, unverified): 138 tiles, 113 cells`,
      `page 15 (not drawn): 5 tiles`;
    - `4bpp cells also read by 8bpp tiles: N`.
    Every 8 bpp page is labelled `unverified`, because only one page of one
    member has been checked by eye.
  - `summary.txt`: every member's first line, then `done`.
- The output folder may hold only `mNNN` folders and `summary.txt`.

## Error handling

- Exit 0: all outputs written.
- Exit 1: a check failed after the inputs loaded (SHA-1 or size mismatch,
  `ArcError` including a missing `0x10000` entry, a texture block of the
  wrong size, a decode length error, tile numbers out of range, the stage
  count differs, `--member` is not a screen set), or an `OSError`. Each gives
  `error: member N: <what>` or `error: <what>`. Nothing is renamed into
  `<out>/`.
- Exit 2: bad arguments, `--out` refused by `outdir.check`, or `index.json`
  missing or not readable as an index.

## Testing

- `test_texture`: `tile_ref` fields; `bits` for 0, 7, 8, 11, 12; a 4 bpp tile
  at cell (1, 2) of page 3 reads the right bytes with low nibble first; an
  8 bpp tile at cx 2 reads the first half and at cx 9 the second half; page
  12 gives `None`; a block of the wrong size raises; `covered_4bpp_cells` for
  cx 2 and cx 9 on page 9.
- `test_shade`: `grey` on 4 bpp values 0, 1, 15 and 8 bpp values 0, 200, 255.
- `test_tiles`: a fake extracted folder with 2 stage members and 1 other
  member, using a texture block with known bytes. Cases: the default run
  writes both folders and `summary.txt`; one `screens.png` pixel per case
  (4 bpp tile, 8 bpp tile, tile 0, page 15); one `pages.png` pixel for a used
  cell and one for an unused cell; the summary lines, including the overlap
  count; a member without `0x10000` gives exit 1; a changed member byte gives
  exit 1; the wrong stage count gives exit 1; `--out` inside the repo gives
  exit 2; a missing `index.json` gives exit 2; a rerun replaces the output;
  `~` paths are expanded.
- `test_retail_tiles`, only when `MMX6_EXTRACTED` is set: the default run
  exits 0 and writes 23 member folders, each with `screens.png`, `pages.png`
  and `summary.txt`.

## Done when

1. The unit suite passes.
2. `MMX6_EXTRACTED=… python3 -m unittest tests.test_retail_tiles` passes.
3. A real run writes `~/mmx-data/tiles/` with 23 member folders.
4. The user looks at `screens.png` for a few members. Recognisable stage art
   (walls, doors, pipes, ground) is the pass. Screens that look like noise are
   reported, with the page numbers from the summary.
5. `git status` shows no game data.

## Legal

- All code is new. X4 format facts come from reading the X4 editor's source
  (facts only, no code copied).
- Outputs are derived game graphics and stay in `~/mmx-data/`.
