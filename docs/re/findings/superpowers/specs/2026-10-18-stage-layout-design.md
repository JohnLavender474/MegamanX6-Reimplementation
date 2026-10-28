# Stage layouts from save states (task F) — design

Date: 2026-10-18. Status: draft for review.

## Purpose

Turn each PCSXR save state taken inside a stage into one verified stage entry
(which DAT member, which overlay and offset holds its layout, width, height),
then draw whole stages from the disc data: the neutral layout, the stage with
textures, and the collision map.

## Decisions already made

- Save states stay outside the repo, as named files in `~/mmx-data/dumps/`
  (they hold the full RAM and video memory: game data). The repo keeps a
  manifest of them (name, SHA-1, notes).
- The first save state is `intro-1_start.state.gz` (intro stage, section 1, X
  standing at the start). It is the real-data test.
- Static discovery of layouts is out of scope: a blind search gave false
  matches (process log, section 34).

## Facts this design depends on

| Fact | Source | Status |
|---|---|---|
| PCSXR save state: gzip; decompressed it starts with `STv4 PCSXR`; main RAM (2 MB) starts at offset `0x9025` | intro and 2024 states | verified on 2 states |
| The X6 program loads at RAM `0x10000` (`0x80010000`); program file offset = RAM offset − `0x10000` + `0x800` | `mmx6` memory map; intro state | verified |
| `layoutWidth` = RAM `0x800cd338` (u8), `layoutHeight` = `0x800cd339` (u8), `layoutSize` = `0x8008ec10` (u16) = width × height | reference symbols; intro state (26, 6, 156) | verified on 1 state |
| The working layout in RAM is at `0x8009741e`: 3 layers of width × height bytes, one byte per cell (a screen number), layer after layer, row by row | intro state | observed on 1 state; assumed fixed |
| The same layout is in an overlay member (`rock_bin/NN.bin`); the bytes match RAM except row 0 of layer 0 | intro: overlay 02, offset `0x107c6`; 19 differing cells, all in row 0 of layer 0 | observed on 1 state |
| Layer 0 holds the collision; layer 1 is mostly decoration; layer 2 is background | intro drawings | observed on 1 stage |
| A stage member's tile info (entry type 1) is in RAM while its stage runs | intro: member 94 at `0x801b26cc`, no other member | observed on 1 state |

## Scope

In scope:

1. Read RAM from a save state, with checks.
2. `learn`: from one save state, find the member, size, layout and its disc
   location; write one entry to `catalog/stages.json` and the save state to
   `catalog/savestates.json`.
3. `stages`: for each catalog entry, write the neutral layout, the stage
   image (greyscale textures) and the layer 0 collision map.

Out of scope: object placements (the 8-byte records after a layout),
palettes, static discovery, video memory.

## Repository layout (additions)

```
catalog/
  savestates.json   # manifest of save states (no game data)
  stages.json       # verified stage entries (no game data)
mmxdemake/
  savestate.py      # save state -> RAM
  ramlayout.py      # size, member, layout and disc location from RAM
  learn.py          # CLI: save state -> catalog entry
  stages.py         # CLI: catalog -> layout.json, stage.png, collision.png
tests/
  test_savestate.py
  test_ramlayout.py
  test_learn.py
  test_stages.py
  test_retail_stages.py   # runs only when MMX6_EXTRACTED and MMX6_DUMPS are set
```

Imports: `savestate` imports no project module. `ramlayout` uses `savestate`
(`ram_at`) and `stagemap`.
`learn` uses `savestate`, `ramlayout`, `arcentries`, `stagemap`, `collision`
(member selection), `index`. `stages` uses `arcentries`, `stagemap`,
`texture`, `shade`, `collision` (colours), `png`, `outdir`, `index`.

## Units

### `savestate`

- `RAM_OFFSET = 0x9025`, `RAM_SIZE = 0x200000`, `HEADER = b"STv4 PCSXR"`.
- `SaveStateError(Exception)`.
- `read_ram(path, exe) -> bytes`: gunzip; refuse when the header is wrong,
  when the data is shorter than `RAM_OFFSET + RAM_SIZE`, or when RAM
  `0x11000..0x11400` is not equal to program file bytes `0x1800..0x1C00`
  (the text of the X6 program). Each refusal names the file and the reason.
- `ram_at(ram, addr, n) -> bytes`: `addr` may be a KSEG0 address
  (`0x80xxxxxx`); the low 21 bits are the RAM offset.

### `ramlayout`

- Constants: `LAYOUT_WIDTH = 0x800cd338`, `LAYOUT_HEIGHT = 0x800cd339`,
  `LAYOUT_SIZE = 0x8008ec10`, `LAYOUT_RAM = 0x8009741e`, `LAYERS = 3`.
- `LayoutError(Exception)`.
- `read_size(ram) -> (w, h)`: refuse when `w` or `h` is 0 or
  `layoutSize != w × h`.
- `find_member(ram, tile_infos: dict[int, bytes]) -> int`: a member matches
  when its whole tile info entry occurs in RAM (intro: only member 94). Exactly one
  member must match; else refuse, listing the matches.
- `read_layout(ram, w, h) -> bytes`: `3 × w × h` bytes from `LAYOUT_RAM`.
- `edge_score(layout, w, h, masks) -> (h_score, h_joins, v_score, v_joins)`:
  over layers 0 and 1, skipping row 0 of layer 0, for each pair of
  neighbouring cells that both hold a screen with an edge (a screen whose
  solid mask is neither all-empty nor all-solid): the share of the 16 edge
  cells whose solid state matches (right edge vs left edge; bottom vs top).
  `masks[s]` is screen `s`'s 16 × 16 solid mask (collision ≠ 0).
- `check_layout(layout, w, h, masks, screen_count)`: refuse when a cell
  outside row 0 of layer 0 is `>= screen_count`, or when `h_joins < 10`, or
  when `h_score < 0.85`, or when `v_joins >= 3` and `v_score < 0.85`. The
  message says that the fixed RAM address may be wrong for this stage.
  (Threshold from the intro state: the real layout scores 0.929 / 0.944 over
  61 / 39 joins on layer 0; 200 shuffles of layer 0 score at most 0.715 /
  0.710. A first draft used 0.95, which would refuse the real layout.)
- `find_on_disc(layout, w, overlays: dict[int, bytes]) -> (overlay, offset)`:
  search every overlay for `layout[w:]` (everything after row 0 of layer 0);
  exactly one match must exist; `offset = match − w`. Else refuse with the
  number of matches.
- `differing_cells(disc_bytes, ram_layout, w, h) -> list[(layer, x, y)]`.

### `learn` (CLI)

```
python3 -m mmxdemake.learn --state intro-1_start.state.gz --name "Intro, section 1" \
  [--note "X at the start"] [--dumps ~/mmx-data/dumps] \
  [--extracted ~/mmx-data/extracted/retail] [--catalog <repo>/catalog]
```

- Reads RAM (`savestate.read_ram`, with the program file from
  `<extracted>/iso/SLUS_013.95`), the size, the member (candidates: every
  screen set, `collision.map_records`), the layout from RAM, checks it, finds
  it on disc (overlays `rock_bin/00.bin`…`58.bin`), and lists the differing
  cells.
- Writes `catalog/savestates.json`: a list sorted by `name`, each
  `{"name", "sha1", "note"}`; an existing entry with the same name is
  replaced only when the SHA-1 is the same, else refuse.
- Writes `catalog/stages.json`: a list sorted by `member`, then `name`, each
  `{"name", "member", "width", "height", "overlay", "offset",
  "save_state", "save_state_sha1", "edge": {"h", "h_joins", "v", "v_joins"},
  "differing_cells": [[layer, x, y], …]}` (scores rounded to 3 decimals).
  An entry with the same `name` is replaced.
- Both files: sorted keys, indent 2, end with `\n`.
- Prints one line, for example
  `Intro, section 1: member 94, 26x6, overlay 2 @0x107c6, edges 0.998/1.000, 19 cells differ (layer 0 row 0)`.

### `stages` (CLI)

```
python3 -m mmxdemake.stages --extracted ~/mmx-data/extracted/retail \
  --out ~/mmx-data/stages [--catalog <repo>/catalog]
```

- For each entry in `stages.json`: read the member's screens, tile info and
  texture block; read `3 × w × h` bytes from the overlay at the offset; set
  the differing cells to 0; refuse a cell `>= screen_count`.
- Output, through `outdir` (`<out>.tmp/` → `<out>/`), one folder per entry,
  named `mNNN-<slug>` (slug: the name in lower case, runs of other characters
  replaced by `-`):
  - `layout.json`: `{"schema_version": 1, "name", "member", "width",
    "height", "source": {"overlay", "offset"}, "layers": [3 × [h × [w ints]]]}`;
  - `stage.png`: `w × 256` by `h × 256`, black background, layers drawn
    2, then 1, then 0, each screen's tiles drawn with greyscale shading,
    tile 0 and pages above 11 not drawn (transparent over what is below);
  - `collision.png`: layer 0 only, 4 × 4 px per tile, colours as in task D
    (`EMPTY_RGB` for tile 0 and for empty cells, `COLOURS[c]` otherwise);
  - `summary.txt`: one line with name, member, size and source.
- `summary.txt` at the top: every entry's line, then `done`.
- The output folder may hold only `mNNN-*` folders and `summary.txt`.

## Error handling

- Exit 0: done.
- Exit 1: a check failed (`SaveStateError`, `LayoutError`, `ArcError`, SHA-1
  mismatch of a member, a manifest name with a different SHA-1, a layout cell
  out of range), or an `OSError`. One `error:` line with the reason.
- Exit 2: bad arguments, a missing save state file, `--out` refused, a
  missing or unreadable `index.json` or catalog file.

## Testing

- `test_savestate`: a fake state (gzip of header + padding + fake RAM holding
  fake program bytes at `0x11000`) reads back; wrong header, short data and
  program mismatch each refuse; `ram_at` with a KSEG0 address.
- `test_ramlayout`: `read_size` good and bad; `find_member` with one, zero and
  two matches; `edge_score` and `check_layout` on a fake member whose screens
  join (pass) and a shuffled layout (refuse); a cell out of range refuses;
  `find_on_disc` with one, zero and two matches; `differing_cells`.
- `test_learn`: a fake extracted folder (index, member, overlay), a fake save
  state and a temporary catalog: the run writes both catalog files with the
  exact entry; a second run with the same name replaces the entry; a state
  whose name is in the manifest with a different SHA-1 refuses; a missing
  state file gives exit 2.
- `test_stages`: a temporary catalog with one entry and a fake extracted
  folder: output folder name and files; one `stage.png` pixel from layer 0
  over layer 2; a differing cell is empty; one `collision.png` pixel;
  `--out` inside the repo gives exit 2.
- `test_retail_stages`, only when `MMX6_EXTRACTED` and `MMX6_DUMPS` are set,
  with a temporary catalog: `learn` on `intro-1_start.state.gz` gives member
  94, 26 × 6, overlay 2, offset `0x107c6`, and 19 differing cells, all in
  layer 0 row 0; then `stages` exits 0 and writes `m094-intro-section-1/`.

## Done when

1. The unit suite passes; the real-data test passes.
2. `learn` on `intro-1_start.state.gz` writes the first entries of
   `catalog/savestates.json` and `catalog/stages.json` in the repo
   (committed: names, hashes and positions only).
3. `stages` writes `~/mmx-data/stages/m094-intro-section-1/`, and the user
   confirms `stage.png` shows the intro stage's first section.
4. `git status` shows no game data.

## Legal

- No save state, RAM, screen data or layout bytes go into the repo. The
  catalog holds names, hashes, member numbers, offsets and sizes.
- RAM addresses come from the reference symbols (read for facts) and our
  own dump.
