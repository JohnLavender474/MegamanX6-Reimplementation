# Palette and image render (sample task C) — design

Date: 2026-10-18. Status: draft for review.

## Purpose

Prove that we can decode PS1 graphics data from `ROCK_X6.DAT`. Turn the 19
palette + image pairs found in task C's probe into PNG files that a person can
check by eye, with exact unit tests for every decode rule.

This is the first task that reads what the bytes mean. Task A proved the bytes
themselves are correct, so a wrong picture can only come from a wrong decode
rule.

## Facts this design depends on

| Fact | Source | Status |
|---|---|---|
| `rock_dat/` members are byte-exact (TOC checked, all tails zero) | task A, `index.json` | verified |
| Members 164, 166, …, 202 are 512 B (20 palettes). Members 164–200 are each followed by an 8,192 B member (165, …, 201): 19 pairs. Member 202 is followed by member 203 (6,144 B, sub-header `{1, 6144, 0x15, 2724}`), which is not raw pixels | `index.json`, task C real run | observed (corrected 2026-10-18; the first draft said 20 pairs) |
| PS1 colour: 16 bits, little-endian. Bits 0–4 red, 5–9 green, 10–14 blue, bit 15 STP (semi-transparency flag). `0x0000` = fully transparent | PS1 GPU documentation (public) | standard |
| Pair 180/181: palette is a smooth dark-to-light ramp, colour 0 = `0x0000`; image is artwork at 8 bpp, 128 × 64, and noise at 4 bpp | probe, 2026-10-18 | observed on 1 pair (pair 164/165: colour 0 = `0x0000` too) |

## Scope

In scope:

1. Decode a 512 B member as 256 PS1 colours.
2. Decode a pixel member at 8 bpp or 4 bpp into colour indices.
3. Write RGBA PNG files with the standard library.
4. Find the 19 pairs from `index.json` and render each image and palette.
5. A manual mode for any palette member + pixel member + bpp + width.
6. Move task A's output-folder guard into a shared module used by both
   commands.

Out of scope:

- Rebuilding whole portraits from the blocks of a texture sheet.
- Any other `ROCK_X6.DAT` member.
- Semi-transparency blending. The STP bit is counted, not drawn.
- Runtime or emulator work.

## Repository layout (additions)

```
mmxdemake/
  psxcolor.py     # PS1 16-bit colour -> RGBA
  indexed.py      # pixel bytes -> colour indices (4 / 8 bpp)
  png.py          # RGBA rows -> PNG file (zlib, no dependencies)
  outdir.py       # shared output-folder guard and tmp -> out rename
  render.py       # CLI: find pairs, check inputs, write PNGs and summary
tests/
  pngread.py      # minimal PNG reader used only by tests
  test_psxcolor.py
  test_indexed.py
  test_png.py
  test_outdir.py
  test_render.py
  test_retail_render.py   # runs only when MMX6_EXTRACTED is set
```

Imports: `psxcolor`, `png` and `outdir` import no project module. `indexed`
uses `psxcolor`. `render` uses all of them plus `index`. `extract` changes to use
`outdir`; its behaviour does not change.

## Units

### `psxcolor`

- `to_rgba(c: int) -> tuple[int, int, int, int]`:
  - `r5 = c & 31`, `g5 = (c >> 5) & 31`, `b5 = (c >> 10) & 31`.
  - Each 5-bit value `v` becomes 8 bits as `(v << 3) | (v >> 2)`, so 0 → 0
    and 31 → 255.
  - Alpha is 0 when `c == 0x0000`, else 255. (`0x8000` is opaque black.)
- `decode_clut(data: bytes) -> list[int]`: little-endian `u16` values. The
  length must be even, else `ValueError`.
- `stats(clut: list[int]) -> dict`: `first_transparent` (bool), `stp`
  (count with bit 15 set), `black` (count whose RGB is 0, transparent
  included), `size`.

### `indexed`

- `unpack(data: bytes, bpp: int) -> list[int]`:
  - 8 bpp: one index per byte.
  - 4 bpp: two per byte, low nibble first.
  - Any other `bpp` raises `ValueError`.
- `dimensions(data_len: int, bpp: int, width: int) -> tuple[int, int]`:
  `pixels = data_len * 8 // bpp`. `width` must divide `pixels` exactly, else
  `ValueError` with both numbers.
- `to_rgba_rows(indices, clut, width, clut_offset=0) -> list[bytes]`: each
  index `i` becomes `to_rgba(clut[clut_offset + i])`. An index past the end
  of the palette raises `ValueError` naming the index and the palette size.
  For 4 bpp, `clut_offset = 16 × clut_row`.

### `png`

- `write_png(path, width, height, rows: list[bytes])`: 8-bit RGBA (colour
  type 6), one `IDAT`, filter type 0 on every row, `zlib.compress` level 9,
  CRC from `zlib.crc32`. Each row must be `width × 4` bytes, else
  `ValueError`.
- `tests/pngread.py` (test-only) parses the chunks, checks every CRC,
  decompresses `IDAT`, removes the filter bytes (type 0 only), and returns
  `(width, height, rows)`.

### `outdir`

Moves the guard written for task A's review fix out of `extract.py`:

- `check(out: Path, repo_root: Path, allowed: Callable[[str], bool]) -> str | None`:
  - Returns an error message, or `None` when `out` may be used.
  - Refuses `out` inside `repo_root`.
  - Refuses `out` or `<out>.tmp` when it is a symlink, a non-directory, or a
    directory that holds a name for which `allowed(name)` is false.
- `fresh_tmp(out) -> Path`: delete `<out>.tmp` if present, create it, return
  it.
- `publish(tmp, out)`: delete `out` if present, rename `tmp` to `out`.

`extract` passes `allowed = name in {"iso", "rock_bin", "rock_dat", "index.json"}`.
`render` passes `allowed = name.endswith(".png") or name == "summary.txt"`.

### `render` (CLI)

```
python3 -m mmxdemake.render \
  --extracted ~/mmx-data/extracted/retail \
  --out ~/mmx-data/renders
python3 -m mmxdemake.render --extracted … --out … \
  --palette 180 --pixels 181 --bpp 8 --width 128 [--clut-row 0]
```

- Paths may start with `~`; the tool expands it.
- Load `<extracted>/index.json` with `load_index` (schema 1).
- Pair mode (the default): a pair is `rock_dat` index `i` with size 512 and
  index `i + 1` with size 8192. The count must equal `--expect-pairs`
  (default 19). Each pair renders at 8 bpp, width 128.
- Manual mode: all of `--palette`, `--pixels`, `--bpp`, `--width` are given.
  `--clut-row` (default 0) applies to 4 bpp only.
- Before it decodes a member, it reads `rock_dat/NNN.bin` and checks its
  SHA-1 and size against the index record.
- Outputs, in `<out>.tmp/` then renamed to `<out>/`:
  - pair mode: `pairPPP_QQQ.png` (the image at true size) and `clutPPP.png`
    (256 × 256: each colour a 16 × 16 square, in index order, row by row);
  - manual mode: `palPPP_pixQQQ_<bpp>bpp_w<width>.png` and `clutPPP.png`;
  - `summary.txt`: the printed summary lines.
- One summary line per pair, for example
  `pair 180/181: 128x64 8bpp, clut[0] transparent, stp 255/256, black 1`,
  then `done: <out>`.

## Error handling

- Exit 0: all outputs written.
- Exit 1: a check failed after the inputs loaded: a member SHA-1 or size does
  not match the index, the pair count differs from `--expect-pairs`, a width
  does not divide the pixel count, an index is past the palette, or an
  `OSError` while writing. Each gives `error: <what>: <numbers>`. Nothing is
  renamed into `<out>/`.
- Exit 2: bad arguments (manual mode with some of its four options missing),
  `--out` refused by `outdir.check`, or `index.json` missing or of another
  schema.

## Testing

- `test_psxcolor`: `0x0000` → `(0,0,0,0)`; `0x8000` → `(0,0,0,255)`; `0x001F`
  → `(255,0,0,255)`; `0x03E0` → green; `0x7C00` → blue; `0x7FFF` → white;
  the 5→8 rule on 1 and 16; odd-length CLUT raises; `stats` counts.
- `test_indexed`: 8 bpp and 4 bpp order (byte `0x21` → `[1, 2]`); dimensions
  for 8192 B (8 bpp, 128 → 128 × 64; 4 bpp, 128 → 128 × 128); a width that
  does not divide raises; `clut_offset`; index past the palette raises.
- `test_png`: a 3 × 2 image round-trips through `tests/pngread.py` with
  exact bytes and good CRCs; a wrong row length raises; when Pillow is
  installed, Pillow opens the same file and gives the same RGBA pixels
  (skipped otherwise).
- `test_outdir`: the five task A refusal cases (inside the repo, foreign
  folder, foreign `.tmp`, file, symlink) through `check`, with both `allowed`
  rules.
- `test_render`: a fake extracted folder (an `index.json` from `build_index`
  plus `rock_dat/` files written by the test) with 2 pairs and an unrelated
  member. Cases: pair mode gives 4 PNGs and `summary.txt` with exact pixels
  in one image; a changed member byte gives exit 1 naming the member; the
  wrong pair count gives exit 1; manual mode at 4 bpp with `--clut-row 1`
  uses colours 16–31; manual mode with `--bpp` missing gives exit 2; `--out`
  inside the repo gives exit 2.
- All existing task A tests still pass after the `outdir` move.
- `test_retail_render`, only when `MMX6_EXTRACTED` points to the task A
  output: pair mode exits 0 and writes 40 PNGs and `summary.txt`, into a
  temporary folder under `~/mmx-data`.

## Done when

1. The unit suite passes.
2. `MMX6_EXTRACTED=… python3 -m unittest tests.test_retail_render` passes.
3. A real run writes `~/mmx-data/renders/` with 19 `pair*.png`, 19 `clut*.png`
   and `summary.txt`.
4. The user looks at the 19 images. Each one shows artwork, not noise. A pair
   that shows noise is reported as a finding (a different format), not
   hidden.
5. `git status` shows no game data.

## Legal

- All code is new. The PS1 colour format comes from public hardware
  documentation. No code is copied from the reference repos.
- Renders are game graphics. They stay in `~/mmx-data/`, never in the repo.
