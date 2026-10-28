# Stage screens and collision (sample task D) — design

Date: 2026-10-18. Status: draft for review.

## Purpose

Prove that we can read stage map data from `ROCK_X6.DAT` without a RAM dump.
Decode the screens and tile info of the 23 stage members into a neutral JSON
file per member and a collision picture of every screen, so a person can check
by eye that the screens are pieces of a Mega Man stage.

The JSON is the first file of the pipeline's neutral middle: Stage 2 (the TMX
converter in Megaman-Maverick) will read it later.

## Constraint

Use only the task A output (`~/mmx-data/extracted/retail`). No emulator and no
RAM dump. If the data turns out not to be enough, stop and report; changing to
a RAM dump is a separate decision.

## Facts this design depends on

| Fact | Source | Status |
|---|---|---|
| A DAT member with a sub-header is `{u32 count, u32 size}`, then `count` × `{u32 type, u32 size}`. The first entry starts at `0x800`; each entry is padded to a `0x800` boundary | X4 editor `ARC.cs` (facts only); X6 check: `0x800 + sum(padded sizes) == member size` for 197/197 members | verified on X6 |
| Entry type `0` = screens: `u16` per cell, 16 × 16 cells per screen (`0x200` B). Low 14 bits = tile number, top 2 bits = flags (X4: flip) | X4 editor `Level.cs` | X6: all 23 type-0 sizes are multiples of `0x200` |
| Entry type `1` = tile info: `u32` per tile. Bits 0–5 = collision type; bits 8–15, 16–19, 20–23, 24–27 = palette and texture-position fields (not used here) | X4 editor `Level.cs` | X6: all 23 type-1 sizes are multiples of 4 |
| Tile 0 = empty | X4 editor `Level.cs` | X6: 36–73% of cells per member |
| In all 23 members, every tile number < tile count, and max tile number = tile count − 1 | probe, 2026-10-18 | observed |
| Stage members: 94–105 and 112–122 (23); each has exactly one type-0 and one type-1 entry | probe, 2026-10-18 | observed |
| Entry types are not unique inside a member in general (for example type `7` twice) | probe, 2026-10-18 | observed |

## Scope

In scope:

1. Split any DAT member with a sub-header into typed entries, with checks.
2. Decode screens and tile info; check tile numbers against the tile count.
3. For each stage member: `screens.json`, a contact sheet of all screens
   coloured by collision type, and a summary.
4. A legend image and an overall summary.

Out of scope:

- The layout (which screen goes where). X4 keeps it in the program file; it is
  the next task.
- Tile graphics (textures, palettes) and the flip flags' visual effect.
- Naming collision types. They stay numbers in this task.
- Which member is which stage.
- Enemies and other entry types.

## Repository layout (additions)

```
mmxdemake/
  arcentries.py   # split a DAT member into typed entries
  stagemap.py     # decode screens and tile info, check tile numbers
  collision.py    # CLI
tests/
  test_arcentries.py
  test_stagemap.py
  test_collision.py
  test_retail_collision.py   # runs only when MMX6_EXTRACTED is set
```

Imports: `arcentries` uses `problem`. `stagemap` uses `problem`. `collision`
uses `arcentries`, `stagemap`, `index`, `png`, `outdir`.

## Units

### `arcentries`

- `Entry(type: int, offset: int, size: int)`, frozen dataclass. `offset` is
  from the start of the member.
- `ArcError(Exception)`.
- `split(body: bytes) -> list[Entry]`. Raises `ArcError` with the numbers when:
  - `len(body) < 0x800`;
  - `count` is not in `1..63`;
  - `size` (word 1) is not `len(body)`;
  - the header list (`8 + 8 × count`) runs past `0x800`;
  - an entry runs past the end, or the last padded entry does not end exactly
    at `len(body)`.
- `entry_data(body, e) -> bytes`.
- `only(entries, type) -> Entry`: the single entry of that type. Raises
  `ArcError` when there is none or more than one.

### `stagemap`

- `decode_screens(data: bytes) -> list[list[int]]`: one list of 256 raw `u16`
  values per screen, in row order. Length must be a multiple of `0x200`.
- `decode_tile_info(data: bytes) -> list[int]`: raw `u32` values. Length must
  be a multiple of 4.
- `tile(v) = v & 0x3FFF`, `flags(v) = v >> 14`, `collision(w) = w & 0x3F`.
- `check(screens, info) -> list[Problem]`: one problem for each cell whose
  tile number is `>= len(info)`, naming the screen, the cell (`x`, `y`) and
  the number. At most 20 problems are listed, then a count of the rest.

### `collision` (CLI)

```
python3 -m mmxdemake.collision \
  --extracted ~/mmx-data/extracted/retail \
  --out ~/mmx-data/collision [--member 94] [--expect-stages 23]
```

- Paths may start with `~`.
- Load `index.json` (schema 1). Stage members are `rock_dat` records whose
  `observed.subentries` include types `0`, `1` and `0x10102`. Without
  `--member`, the count must equal `--expect-stages` (default 23).
- `--member N` accepts any member whose sub-entries include types `0` and `1`
  (a "screen set"), with or without `0x10102`. (Corrected 2026-10-18: the first
  draft used `0` + `1` for both, which matches 51 members. 27 of the extra 28
  pass the tile-range check and look like smaller screen sets; member 73 fails
  it, so its types `0` and `1` mean something else.)
- For each member: read the file, check its size and SHA-1 against the index,
  `split` it, take `only(entries, 0)` and `only(entries, 1)`, decode, `check`.
- Output, in `<out>.tmp/` then renamed to `<out>/`:
  - `mNNN/screens.json` (sorted keys, indent 2, ends with `\n`):

    ```json
    {
      "schema_version": 1,
      "source": {"member": 94, "sha1": "…",
                 "screens_entry": {"offset": 2048, "size": 70144},
                 "tile_info_entry": {"offset": …, "size": 12392}},
      "tile_count": 3098,
      "tile_info": [<u32>, …],
      "screens": [
        {"index": 0,
         "tile": [[16 ints], … 16 rows],
         "flags": [[16 ints], … 16 rows],
         "collision": [[16 ints], … 16 rows]}
      ]
    }
    ```

    `collision` of a cell is `collision(tile_info[tile])`. `tile_info` is kept
    raw for later tasks (graphics).
  - `mNNN/sheet.png`: screens in index order, 16 per row. Each screen is
    16 × 16 cells of 4 × 4 px, with a 2 px gap (colour `(60, 60, 60)`) around
    each screen. Cell colours: tile 0 = `(16, 16, 16)`; a non-zero tile with
    collision 0 = `(40, 40, 64)`; collision `c > 0` = `COLOURS[c]`.
  - `mNNN/summary.txt`: one header line
    `member 94: 137 screens, 3098 tiles, max tile 3097, empty cells 62%`,
    then one line per collision value present in the cells, most common first:
    `collision 56: 1234 cells #rrggbb`.
  - `legend.png`: an 8 × 8 grid of 16 × 16 px squares; the square at row
    `r`, column `c` shows `COLOURS[8r + c]` (value 0 shows `(40, 40, 64)`).
  - `summary.txt`: every member's header line, then `done`.
- `COLOURS`: a fixed table of 64 RGB colours, made by a fixed rule
  (hue = `(value × 0.618034) mod 1`, saturation 0.75, value 0.95, via
  `colorsys`), so the same value has the same colour in every picture.
- The output folder may hold only `mNNN` folders, `summary.txt` and
  `legend.png` (the `outdir` rule).

## Error handling

- Exit 0: all outputs written.
- Exit 1: a check failed after the inputs loaded (SHA-1 or size mismatch,
  `ArcError`, a decode length error, tile numbers out of range, the stage count
  differs from `--expect-stages`, `--member` is not a stage member), or an
  `OSError`. Each gives `error: member N: <what>`. Nothing is renamed into
  `<out>/`.
- Exit 2: bad arguments, `--out` refused by `outdir.check`, or `index.json`
  missing, of another schema, or not readable as an index.

## Testing

- `test_arcentries`: a member built by a test helper from `(type, bytes)` pairs
  splits into the right offsets and sizes (first at `0x800`, padding to
  `0x800`, an entry of exactly `0x800`); each `ArcError` case; `only` with
  zero, one and two entries of a type.
- `test_stagemap`: a screen with known `u16` values gives the right tile,
  flags and row order; length errors; `check` names screen, `x`, `y` and the
  number, and caps the list at 20 plus a count.
- `test_collision`: a fake extracted folder with 2 stage members and 1 other
  member. Cases: the default run writes both `mNNN/` folders, `legend.png` and
  `summary.txt`; one `screens.json` cell has the exact tile, flags and
  collision; one `sheet.png` pixel has the exact colour for an empty cell, a
  collision-0 tile and a collision-56 tile; a changed member byte gives exit 1;
  a tile number out of range gives exit 1 naming the cell; the wrong stage
  count gives exit 1; `--member` of a non-stage member gives exit 1; `--out`
  inside the repo gives exit 2; a missing `index.json` gives exit 2; `~` paths
  are expanded.
- `test_retail_collision`, only when `MMX6_EXTRACTED` is set: the default run
  exits 0 and writes 23 `mNNN/` folders, each with a `screens.json` whose
  screen count matches its summary line.

## Done when

1. The unit suite passes.
2. `MMX6_EXTRACTED=… python3 -m unittest tests.test_retail_collision` passes.
3. A real run writes `~/mmx-data/collision/` with 23 member folders.
4. The user looks at the sheets. Screens that show floors, walls, shafts and
   slopes are the pass. Sheets that look like static are reported as a
   finding.
5. `git status` shows no game data.

## Legal

- All code is new. X4 format facts come from reading the X4 editor's source
  (no license; read for facts, no code copied).
- Outputs are derived game data and stay in `~/mmx-data/`.
