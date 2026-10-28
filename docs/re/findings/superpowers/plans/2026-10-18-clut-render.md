# Palette and Image Render Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Render the 20 palette + image pairs in `ROCK_X6.DAT` (members 164/165 … 202/203 step 2) to PNG files in `~/mmx-data/renders/`, with exact unit tests for the PS1 colour rule, the pixel unpack rule and the PNG writer.

**Architecture:** Four small pure modules (`psxcolor`, `indexed`, `png`, `outdir`) and one command (`render`). The output-folder guard from task A moves out of `extract.py` into `outdir` so both commands share it. `render` reads task A's `index.json`, checks each input member's SHA-1, decodes, and writes to `<out>.tmp/` before a rename to `<out>/`.

**Tech Stack:** Python 3.10 standard library only (`struct`, `zlib`, `hashlib`, `argparse`, `pathlib`, `shutil`, `unittest`). Pillow is used only by one optional test, skipped when it is not installed.

**Spec:** `docs/superpowers/specs/2026-10-18-clut-render-design.md`

## Global Constraints

- Python 3.10, standard library only in `mmxdemake/`. Tests: `unittest`, run from the repo root with `python3 -m unittest discover -s tests`. Do not use `-I`.
- PS1 colour: little-endian `u16`; bits 0–4 red, 5–9 green, 10–14 blue, bit 15 STP. 5→8 bits: `(v << 3) | (v >> 2)`. Alpha 0 only for exactly `0x0000`; `0x8000` is opaque black.
- 4 bpp: low nibble first. 4 bpp palette offset = `16 × clut_row`.
- PNG: 8-bit RGBA (colour type 6), one `IDAT`, filter 0 on every row.
- Pairs: `rock_dat` index `i` size 512 and index `i + 1` size 8192; expected 20; render at 8 bpp, width 128.
- Output names: `pairPPP_QQQ.png`, `clutPPP.png` (256 × 256, 16 × 16 squares), `palPPP_pixQQQ_<bpp>bpp_w<width>.png`, `summary.txt`.
- Render output folder may hold only `*.png` and `summary.txt`. Extract output folder may hold only `iso`, `rock_bin`, `rock_dat`, `index.json`.
- Exit codes: 0 ok; 1 a check failed after inputs loaded, or an `OSError`; 2 bad arguments, `--out` refused, `index.json` missing or of another schema.
- No game data in the repo. Renders go to `~/mmx-data/`.
- No code comments. No code copied from the reference repos.
- Commit to `master`. End each commit message with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. `render` run before `extract` (no `index.json`) → exit 2 naming the missing file, not a traceback. Test in Task 4.
2. A member file listed in the index but missing on disk → exit 1 naming the file. Test in Task 4.
3. Paths given as `"~/…"` in quotes → expanded. Test in Task 4.
4. Manual mode with a palette member of odd length → exit 1 with an `error:` line, not a traceback. Test in Task 4.
5. The task A data-loss guard still protects `extract` after the move into `outdir` → the 9 task A guard tests still pass unchanged. Checked in Task 3.

---

## File structure

```
mmxdemake/
  psxcolor.py   # new
  indexed.py    # new
  png.py        # new
  outdir.py     # new (guard moved from extract.py)
  extract.py    # modified: uses outdir
  render.py     # new
tests/
  pngread.py            # new, test-only PNG reader
  test_psxcolor.py      # new
  test_indexed.py       # new
  test_png.py           # new
  test_outdir.py        # new
  test_render.py        # new
  test_retail_render.py # new
```

---

### Task 1: `psxcolor` and `indexed`

**Files:**
- Create: `mmxdemake/psxcolor.py`, `mmxdemake/indexed.py`
- Test: `tests/test_psxcolor.py`, `tests/test_indexed.py`

**Interfaces:**
- Produces:
  - `psxcolor.to_rgba(c: int) -> tuple[int, int, int, int]`
  - `psxcolor.decode_clut(data: bytes) -> list[int]` (raises `ValueError` on odd length)
  - `psxcolor.stats(clut: list[int]) -> dict` with keys `first_transparent`, `stp`, `black`, `size`
  - `indexed.unpack(data: bytes, bpp: int) -> list[int]` (raises `ValueError` unless bpp is 4 or 8)
  - `indexed.dimensions(data_len: int, bpp: int, width: int) -> tuple[int, int]` (raises `ValueError`)
  - `indexed.to_rgba_rows(indices: list[int], clut: list[int], width: int, clut_offset: int = 0) -> list[bytes]` (raises `ValueError`)

- [ ] **Step 1: Write the failing tests `tests/test_psxcolor.py`**

```python
import struct
import unittest

from mmxdemake.psxcolor import decode_clut, stats, to_rgba


class ToRgbaTest(unittest.TestCase):
    def test_transparent_and_opaque_black(self):
        self.assertEqual(to_rgba(0x0000), (0, 0, 0, 0))
        self.assertEqual(to_rgba(0x8000), (0, 0, 0, 255))

    def test_primaries(self):
        self.assertEqual(to_rgba(0x001F), (255, 0, 0, 255))
        self.assertEqual(to_rgba(0x03E0), (0, 255, 0, 255))
        self.assertEqual(to_rgba(0x7C00), (0, 0, 255, 255))
        self.assertEqual(to_rgba(0x7FFF), (255, 255, 255, 255))

    def test_five_to_eight_bits(self):
        self.assertEqual(to_rgba(1)[0], 8)
        self.assertEqual(to_rgba(16)[0], 132)
        self.assertEqual(to_rgba(31 << 5)[1], 255)


class ClutTest(unittest.TestCase):
    def test_decode_little_endian(self):
        self.assertEqual(decode_clut(struct.pack("<3H", 0, 0x7C00, 0x8001)), [0, 0x7C00, 0x8001])

    def test_odd_length_raises(self):
        with self.assertRaisesRegex(ValueError, "palette length 3 is odd"):
            decode_clut(b"\0\0\0")

    def test_stats(self):
        self.assertEqual(stats([0x0000, 0x8000, 0x801F, 0x7FFF]),
                         {"first_transparent": True, "stp": 2, "black": 2, "size": 4})
        self.assertFalse(stats([0x8000])["first_transparent"])


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Write the failing tests `tests/test_indexed.py`**

```python
import unittest

from mmxdemake.indexed import dimensions, to_rgba_rows, unpack


class UnpackTest(unittest.TestCase):
    def test_8bpp(self):
        self.assertEqual(unpack(bytes([0, 5, 255]), 8), [0, 5, 255])

    def test_4bpp_low_nibble_first(self):
        self.assertEqual(unpack(bytes([0x21, 0xF0]), 4), [1, 2, 0, 15])

    def test_bad_bpp(self):
        with self.assertRaisesRegex(ValueError, "bpp 2 not supported"):
            unpack(b"\0", 2)


class DimensionsTest(unittest.TestCase):
    def test_dimensions(self):
        self.assertEqual(dimensions(8192, 8, 128), (128, 64))
        self.assertEqual(dimensions(8192, 4, 128), (128, 128))

    def test_width_must_divide(self):
        with self.assertRaisesRegex(ValueError, "width 100 does not divide 8192 pixels"):
            dimensions(8192, 8, 100)


class RowsTest(unittest.TestCase):
    def test_rows_with_offset(self):
        clut = [0x0000, 0x001F, 0x7C00, 0x03E0]
        self.assertEqual(to_rgba_rows([0, 1, 2, 3], clut, 2),
                         [bytes([0, 0, 0, 0, 255, 0, 0, 255]), bytes([0, 0, 255, 255, 0, 255, 0, 255])])
        self.assertEqual(to_rgba_rows([0, 1], clut, 2, clut_offset=2),
                         [bytes([0, 0, 255, 255, 0, 255, 0, 255])])

    def test_index_past_palette(self):
        with self.assertRaisesRegex(ValueError, "index 5 is past the palette of 4 colours"):
            to_rgba_rows([1], [0, 0, 0, 0], 1, clut_offset=4)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 3: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p "test_[pi][sn]*.py"`
Expected: 2 errors, `ModuleNotFoundError: No module named 'mmxdemake.psxcolor'` and `… 'mmxdemake.indexed'`

- [ ] **Step 4: Write `mmxdemake/psxcolor.py`**

```python
import struct


def _expand(v):
    return (v << 3) | (v >> 2)


def to_rgba(c):
    return (_expand(c & 31), _expand((c >> 5) & 31), _expand((c >> 10) & 31), 0 if c == 0 else 255)


def decode_clut(data):
    if len(data) % 2:
        raise ValueError(f"palette length {len(data)} is odd")
    return list(struct.unpack(f"<{len(data) // 2}H", data))


def stats(clut):
    return {
        "first_transparent": bool(clut) and clut[0] == 0,
        "stp": sum(1 for c in clut if c & 0x8000),
        "black": sum(1 for c in clut if (c & 0x7FFF) == 0),
        "size": len(clut),
    }
```

- [ ] **Step 5: Write `mmxdemake/indexed.py`**

```python
from mmxdemake.psxcolor import to_rgba


def unpack(data, bpp):
    if bpp == 8:
        return list(data)
    if bpp == 4:
        return [v for b in data for v in (b & 15, b >> 4)]
    raise ValueError(f"bpp {bpp} not supported (4 or 8)")


def dimensions(data_len, bpp, width):
    pixels = data_len * 8 // bpp
    if width <= 0 or pixels % width:
        raise ValueError(f"width {width} does not divide {pixels} pixels")
    return width, pixels // width


def to_rgba_rows(indices, clut, width, clut_offset=0):
    rows = []
    for start in range(0, len(indices), width):
        row = bytearray()
        for i in indices[start:start + width]:
            k = clut_offset + i
            if k >= len(clut):
                raise ValueError(f"index {k} is past the palette of {len(clut)} colours")
            row += bytes(to_rgba(clut[k]))
        rows.append(bytes(row))
    return rows
```

- [ ] **Step 6: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 73 tests`, `OK (skipped=1)`

- [ ] **Step 7: Commit**

```bash
git add mmxdemake/psxcolor.py mmxdemake/indexed.py tests/test_psxcolor.py tests/test_indexed.py
git commit -m "Decode PS1 colours and 4 and 8 bpp pixel data

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: `png` writer and the test reader

**Files:**
- Create: `mmxdemake/png.py`, `tests/pngread.py`
- Test: `tests/test_png.py`

**Interfaces:**
- Produces:
  - `png.write_png(path: Path, width: int, height: int, rows: list[bytes]) -> None` (raises `ValueError`)
  - `pngread.read_png(path: Path) -> tuple[int, int, list[bytes]]` (test-only; asserts on a bad file)

- [ ] **Step 1: Write `tests/pngread.py`**

```python
import struct
import zlib
from pathlib import Path

SIGNATURE = b"\x89PNG\r\n\x1a\n"


def read_png(path):
    data = Path(path).read_bytes()
    assert data[:8] == SIGNATURE, "bad signature"
    pos, chunks = 8, []
    while pos < len(data):
        (n,) = struct.unpack_from(">I", data, pos)
        kind, body = data[pos + 4:pos + 8], data[pos + 8:pos + 8 + n]
        (crc,) = struct.unpack_from(">I", data, pos + 8 + n)
        assert crc == zlib.crc32(kind + body), f"bad CRC in {kind}"
        chunks.append((kind, body))
        pos += 12 + n
    assert chunks[0][0] == b"IHDR" and chunks[-1][0] == b"IEND", "bad chunk order"
    width, height, depth, ctype, _, _, _ = struct.unpack(">IIBBBBB", chunks[0][1])
    assert (depth, ctype) == (8, 6), "not 8-bit RGBA"
    raw = zlib.decompress(b"".join(body for kind, body in chunks if kind == b"IDAT"))
    stride = width * 4 + 1
    rows = []
    for y in range(height):
        line = raw[y * stride:(y + 1) * stride]
        assert line[0] == 0, "filter type is not 0"
        rows.append(line[1:])
    return width, height, rows
```

- [ ] **Step 2: Write the failing tests `tests/test_png.py`**

```python
import tempfile
import unittest
from pathlib import Path

from mmxdemake.png import write_png
from pngread import read_png

ROWS = [bytes([255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255]),
        bytes([0, 0, 0, 0, 255, 255, 255, 255, 10, 20, 30, 40])]


class PngTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.path = Path(self.tmp.name) / "a.png"

    def tearDown(self):
        self.tmp.cleanup()

    def test_round_trip(self):
        write_png(self.path, 3, 2, ROWS)
        self.assertEqual(read_png(self.path), (3, 2, ROWS))

    def test_wrong_row_length(self):
        with self.assertRaisesRegex(ValueError, "row 1 has 8 bytes, expected 12"):
            write_png(self.path, 3, 2, [ROWS[0], ROWS[1][:8]])

    def test_wrong_row_count(self):
        with self.assertRaisesRegex(ValueError, "1 rows, expected 2"):
            write_png(self.path, 3, 2, ROWS[:1])

    def test_pillow_reads_same_pixels(self):
        try:
            from PIL import Image
        except ImportError:
            self.skipTest("Pillow not installed")
        write_png(self.path, 3, 2, ROWS)
        with Image.open(self.path) as im:
            self.assertEqual((im.size, im.mode), ((3, 2), "RGBA"))
            pixels = list(im.getdata())
        expected = [tuple(row[i:i + 4]) for row in ROWS for i in range(0, 12, 4)]
        self.assertEqual(pixels, expected)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 3: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_png.py 2>&1 | grep Error`
Expected: `ModuleNotFoundError: No module named 'mmxdemake.png'`

- [ ] **Step 4: Write `mmxdemake/png.py`**

```python
import struct
import zlib
from pathlib import Path

SIGNATURE = b"\x89PNG\r\n\x1a\n"


def _chunk(kind, data):
    return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data))


def write_png(path, width, height, rows):
    if len(rows) != height:
        raise ValueError(f"{len(rows)} rows, expected {height}")
    for n, row in enumerate(rows):
        if len(row) != width * 4:
            raise ValueError(f"row {n} has {len(row)} bytes, expected {width * 4}")
    raw = b"".join(b"\0" + row for row in rows)
    header = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    data = SIGNATURE + _chunk(b"IHDR", header) + _chunk(b"IDAT", zlib.compress(raw, 9)) + _chunk(b"IEND", b"")
    Path(path).write_bytes(data)
```

- [ ] **Step 5: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 77 tests`, `OK (skipped=1)` (Pillow is installed here, so the Pillow test runs)

- [ ] **Step 6: Commit**

```bash
git add mmxdemake/png.py tests/pngread.py tests/test_png.py
git commit -m "Write RGBA PNG files with the standard library

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: `outdir`, and `extract` moved onto it

**Files:**
- Create: `mmxdemake/outdir.py`
- Modify: `mmxdemake/extract.py`
- Test: `tests/test_outdir.py`

**Interfaces:**
- Produces:
  - `outdir.tmp_of(out: Path) -> Path`
  - `outdir.check(out: Path, repo_root: Path, allowed: Callable[[str], bool]) -> str | None`
  - `outdir.fresh_tmp(out: Path) -> Path`
  - `outdir.publish(tmp: Path, out: Path) -> None`
- Consumes (in `extract.py`): the above. The error strings must stay exactly `--out {out} is inside the repo {repo_root}; game data must stay outside it` and `{path} exists and is not an earlier output of this tool; choose another --out`, because task A's tests match them.

- [ ] **Step 1: Write the failing tests `tests/test_outdir.py`**

```python
import tempfile
import unittest
from pathlib import Path

from mmxdemake import outdir

REPO = Path(__file__).resolve().parents[1]
RULES = {
    "extract": (lambda n: n in {"iso", "rock_bin", "rock_dat", "index.json"}, "iso"),
    "render": (lambda n: n.endswith(".png") or n == "summary.txt", "a.png"),
}


class CheckTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)

    def tearDown(self):
        self.tmp.cleanup()

    def each_rule(self):
        for name, (allowed, own) in RULES.items():
            with self.subTest(rule=name):
                yield allowed, own

    def test_new_and_empty_paths_are_usable(self):
        (self.dir / "empty").mkdir()
        for allowed, _ in self.each_rule():
            self.assertIsNone(outdir.check(self.dir / "new", REPO, allowed))
            self.assertIsNone(outdir.check(self.dir / "empty", REPO, allowed))

    def test_own_entries_are_usable(self):
        for allowed, own in self.each_rule():
            out = self.dir / f"own-{own}"
            out.mkdir()
            (out / own).mkdir()
            self.assertIsNone(outdir.check(out, REPO, allowed))

    def test_inside_repo_refused(self):
        for allowed, _ in self.each_rule():
            self.assertIn("is inside the repo", outdir.check(REPO / "x", REPO, allowed))

    def test_foreign_folder_refused(self):
        (self.dir / "f").mkdir()
        (self.dir / "f" / "notes.txt").write_text("keep")
        for allowed, _ in self.each_rule():
            self.assertIn("is not an earlier output", outdir.check(self.dir / "f", REPO, allowed))

    def test_foreign_tmp_refused(self):
        (self.dir / "o.tmp").mkdir()
        (self.dir / "o.tmp" / "notes.txt").write_text("keep")
        for allowed, _ in self.each_rule():
            self.assertIn("o.tmp exists", outdir.check(self.dir / "o", REPO, allowed))

    def test_file_refused(self):
        (self.dir / "o").write_text("x")
        for allowed, _ in self.each_rule():
            self.assertIn("is not an earlier output", outdir.check(self.dir / "o", REPO, allowed))

    def test_symlink_refused(self):
        (self.dir / "t").mkdir()
        (self.dir / "o").symlink_to(self.dir / "t")
        for allowed, _ in self.each_rule():
            self.assertIn("is not an earlier output", outdir.check(self.dir / "o", REPO, allowed))


class PublishTest(unittest.TestCase):
    def test_fresh_tmp_and_publish(self):
        with tempfile.TemporaryDirectory() as t:
            out = Path(t) / "o"
            out.mkdir()
            (out / "old.png").write_bytes(b"old")
            stale = outdir.tmp_of(out)
            stale.mkdir()
            (stale / "stale.png").write_bytes(b"stale")
            tmp = outdir.fresh_tmp(out)
            self.assertEqual(tmp, stale)
            self.assertEqual(list(tmp.iterdir()), [])
            (tmp / "new.png").write_bytes(b"new")
            outdir.publish(tmp, out)
            self.assertEqual([p.name for p in out.iterdir()], ["new.png"])
            self.assertFalse(tmp.exists())


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_outdir.py 2>&1 | grep Error`
Expected: `ImportError: cannot import name 'outdir' from 'mmxdemake'`

- [ ] **Step 3: Write `mmxdemake/outdir.py`**

```python
import shutil
from pathlib import Path


def tmp_of(out):
    out = Path(out)
    return out.with_name(out.name + ".tmp")


def _inside(path, folder):
    try:
        Path(path).resolve().relative_to(Path(folder).resolve())
        return True
    except ValueError:
        return False


def _usable(path, allowed):
    if path.is_symlink():
        return False
    if not path.exists():
        return True
    return path.is_dir() and all(allowed(p.name) for p in path.iterdir())


def check(out, repo_root, allowed):
    out = Path(out)
    if _inside(out, repo_root):
        return f"--out {out} is inside the repo {repo_root}; game data must stay outside it"
    for path in (out, tmp_of(out)):
        if not _usable(path, allowed):
            return f"{path} exists and is not an earlier output of this tool; choose another --out"
    return None


def fresh_tmp(out):
    tmp = tmp_of(out)
    if tmp.exists():
        shutil.rmtree(tmp)
    tmp.mkdir(parents=True)
    return tmp


def publish(tmp, out):
    out = Path(out)
    if out.exists():
        shutil.rmtree(out)
    Path(tmp).rename(out)
```

- [ ] **Step 4: Run the outdir tests**

Run: `python3 -m unittest discover -s tests -p test_outdir.py 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 8 tests`, `OK`

- [ ] **Step 5: Move `extract.py` onto `outdir`**

In `mmxdemake/extract.py`:

Replace the import line `import shutil` with nothing (delete the line), and add `from mmxdemake import outdir` above `from mmxdemake.cdimage import …`.

Replace this block:

```python
def _inside(path, folder):
    try:
        path.resolve().relative_to(folder)
        return True
    except ValueError:
        return False


def _replaceable(path):
    if path.is_symlink():
        return False
    if not path.exists():
        return True
    return path.is_dir() and {p.name for p in path.iterdir()} <= OUTPUT_ENTRIES
```

with nothing (delete it; keep `OUTPUT_ENTRIES`).

Replace this block in `run()`:

```python
    out = Path(out)
    if _inside(out, REPO_ROOT):
        log(f"error: --out {out} is inside the repo {REPO_ROOT}; game data must stay outside it")
        return 2
    tmp = out.with_name(out.name + ".tmp")
    for path in (out, tmp):
        if not _replaceable(path):
            log(f"error: {path} exists and is not an earlier output of this tool; choose another --out")
            return 2
```

with:

```python
    out = Path(out)
    refusal = outdir.check(out, REPO_ROOT, OUTPUT_ENTRIES.__contains__)
    if refusal:
        log(f"error: {refusal}")
        return 2
```

Replace `_produce(bin_path, out, tmp, ref, expected_sha1, bin_members, dat_members, log)` (the call in `run()`) with `_produce(bin_path, out, ref, expected_sha1, bin_members, dat_members, log)`.

Replace the start of `_produce`:

```python
def _produce(bin_path, out, tmp, ref, expected_sha1, bin_members, dat_members, log):
    if tmp.exists():
        shutil.rmtree(tmp)
    tmp.mkdir(parents=True)
```

with:

```python
def _produce(bin_path, out, ref, expected_sha1, bin_members, dat_members, log):
    tmp = outdir.fresh_tmp(out)
```

Replace the end of `_produce`:

```python
    if out.exists():
        shutil.rmtree(out)
    tmp.rename(out)
```

with:

```python
    outdir.publish(tmp, out)
```

- [ ] **Step 6: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 85 tests`, `OK (skipped=1)`. All task A tests, including the 9 guard tests in `test_extract.py`, pass unchanged.

- [ ] **Step 7: Commit**

```bash
git add mmxdemake/outdir.py mmxdemake/extract.py tests/test_outdir.py
git commit -m "Move the output-folder guard into a shared module

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: `render` command

**Files:**
- Create: `mmxdemake/render.py`
- Test: `tests/test_render.py`

**Interfaces:**
- Consumes: `outdir.check/fresh_tmp/publish`, `index.load_index`, `index.build_index` and `index.write_index` (tests), `psxcolor.decode_clut/stats/to_rgba`, `indexed.unpack/dimensions/to_rgba_rows`, `png.write_png`, `pngread.read_png` (tests).
- Produces:
  - `render.find_pairs(records: list[dict]) -> list[tuple[int, int]]`
  - `render.run(extracted: Path, out: Path, palette: int | None = None, pixels: int | None = None, bpp: int | None = None, width: int | None = None, clut_row: int = 0, expect_pairs: int = 20, log=print) -> int`
  - `render.main(argv: list[str] | None = None) -> int`

- [ ] **Step 1: Write the failing tests `tests/test_render.py`**

```python
import hashlib
import os
import struct
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from mmxdemake import render
from mmxdemake.index import build_index, write_index
from mmxdemake.render import main, run
from pngread import read_png

PAL = [0x0000, 0x001F, 0x7C00] + [0x8000 | k for k in range(3, 256)]
PAL[17], PAL[18] = 0x03E0, 0x7FFF


def write_extracted(folder, members):
    (folder / "rock_dat").mkdir(parents=True)
    records = []
    for i, data in enumerate(members):
        rel = f"rock_dat/{i:03d}.bin"
        (folder / rel).write_bytes(data)
        records.append({"kind": "rock_dat", "path": rel, "index": i, "sector": 0, "size": len(data),
                        "sha1": hashlib.sha1(data).hexdigest()})
    write_index(folder / "index.json", build_index({"serial": "S", "revision": "r", "bin_sha1": "0" * 40}, records))


class RenderTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.extracted = self.dir / "extracted"
        pal = struct.pack("<256H", *PAL)
        pix = bytes(k % 3 for k in range(8192))
        write_extracted(self.extracted, [b"\x01" * 100, pal, pix, pal, pix, bytes([0x21]) * 8192, b"\0" * 3])
        self.lines = []

    def tearDown(self):
        self.tmp.cleanup()

    def go(self, out_name="renders", **kw):
        self.lines = []
        return run(self.extracted, self.dir / out_name, log=self.lines.append, **kw)

    def test_pair_mode(self):
        self.assertEqual(self.go(expect_pairs=2), 0, self.lines)
        out = self.dir / "renders"
        self.assertEqual(sorted(p.name for p in out.iterdir()),
                         ["clut001.png", "clut003.png", "pair001_002.png", "pair003_004.png", "summary.txt"])
        width, height, rows = read_png(out / "pair001_002.png")
        self.assertEqual((width, height), (128, 64))
        self.assertEqual(rows[0][:12], bytes([0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 255]))
        self.assertEqual(read_png(out / "clut001.png")[:2], (256, 256))
        line = "pair 1/2: 128x64 8bpp, clut[0] transparent, stp 251/256, black 1"
        self.assertIn(line, self.lines)
        self.assertEqual((out / "summary.txt").read_text().splitlines()[0], line)
        self.assertFalse((self.dir / "renders.tmp").exists())

    def test_changed_member_fails(self):
        p = self.extracted / "rock_dat/002.bin"
        p.write_bytes(b"\x09" + p.read_bytes()[1:])
        self.assertEqual(self.go(expect_pairs=2), 1)
        self.assertTrue(any(line.startswith("error: rock_dat/002.bin:") for line in self.lines))
        self.assertFalse((self.dir / "renders").exists())

    def test_wrong_pair_count(self):
        self.assertEqual(self.go(expect_pairs=3), 1)
        self.assertIn("error: found 2 palette/image pairs, expected 3", self.lines)

    def test_manual_4bpp_clut_row(self):
        self.assertEqual(self.go(palette=1, pixels=5, bpp=4, width=128, clut_row=1), 0, self.lines)
        width, height, rows = read_png(self.dir / "renders/pal001_pix005_4bpp_w128.png")
        self.assertEqual((width, height), (128, 128))
        self.assertEqual(rows[0][:8], bytes([0, 255, 0, 255, 255, 255, 255, 255]))

    def test_manual_width_must_divide(self):
        self.assertEqual(self.go(palette=1, pixels=2, bpp=8, width=100), 1)
        self.assertIn("error: pair 1/2: width 100 does not divide 8192 pixels", self.lines)

    def test_manual_odd_palette(self):
        self.assertEqual(self.go(palette=6, pixels=2, bpp=8, width=128), 1)
        self.assertIn("error: pair 6/2: palette length 3 is odd", self.lines)

    def test_manual_needs_all_options(self):
        self.assertEqual(self.go(palette=1, pixels=2, width=128), 2)
        self.assertIn("error: manual mode needs all of --palette, --pixels, --bpp and --width", self.lines)

    def test_out_inside_repo_refused(self):
        inside = render.REPO_ROOT / "renders-test-should-not-exist"
        self.assertEqual(run(self.extracted, inside, expect_pairs=2, log=self.lines.append), 2)
        self.assertFalse(inside.exists())

    def test_missing_index(self):
        (self.extracted / "index.json").unlink()
        self.assertEqual(self.go(expect_pairs=2), 2)
        self.assertTrue(any(line.startswith("error:") and "index.json" in line for line in self.lines))

    def test_missing_member_file(self):
        (self.extracted / "rock_dat/004.bin").unlink()
        self.assertEqual(self.go(expect_pairs=2), 1)
        self.assertTrue(any(line.startswith("error:") and "004.bin" in line for line in self.lines))

    def test_cli_expands_home(self):
        with mock.patch.dict(os.environ, {"HOME": str(self.dir)}), mock.patch("builtins.print"):
            code = main(["--extracted", "~/extracted", "--out", "~/cli-renders", "--expect-pairs", "2"])
        self.assertEqual(code, 0)
        self.assertTrue((self.dir / "cli-renders/summary.txt").exists())


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_render.py 2>&1 | grep Error`
Expected: `ImportError: cannot import name 'render' from 'mmxdemake'`

- [ ] **Step 3: Write `mmxdemake/render.py`**

```python
import argparse
import hashlib
import sys
from math import ceil
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.index import load_index
from mmxdemake.indexed import dimensions, to_rgba_rows, unpack
from mmxdemake.png import write_png
from mmxdemake.psxcolor import decode_clut, stats, to_rgba

REPO_ROOT = Path(__file__).resolve().parents[1]
PALETTE_SIZE = 512
PIXELS_SIZE = 8192
PAIR_BPP = 8
PAIR_WIDTH = 128
EXPECT_PAIRS = 20
SWATCH = 16
SWATCH_COLUMNS = 16


class RenderFailed(Exception):
    pass


def _allowed(name):
    return name.endswith(".png") or name == "summary.txt"


def find_pairs(records):
    by_index = {r["index"]: r for r in records}
    return [(i, i + 1) for i in sorted(by_index)
            if by_index[i]["size"] == PALETTE_SIZE and i + 1 in by_index and by_index[i + 1]["size"] == PIXELS_SIZE]


def _member(extracted, by_index, i):
    r = by_index.get(i)
    if r is None:
        raise RenderFailed(f"rock_dat member {i} is not in the index")
    data = (extracted / r["path"]).read_bytes()
    sha1 = hashlib.sha1(data).hexdigest()
    if len(data) != r["size"] or sha1 != r["sha1"]:
        raise RenderFailed(f"{r['path']}: size {len(data)}, sha1 {sha1} do not match the index ({r['size']}, {r['sha1']})")
    return data


def _swatch_rows(clut):
    rows = []
    for y in range(ceil(len(clut) / SWATCH_COLUMNS) * SWATCH):
        row = bytearray()
        for x in range(SWATCH_COLUMNS):
            k = (y // SWATCH) * SWATCH_COLUMNS + x
            row += bytes(to_rgba(clut[k]) if k < len(clut) else (0, 0, 0, 0)) * SWATCH
        rows.append(bytes(row))
    return rows


def _render_one(tmp, extracted, by_index, pal_i, pix_i, bpp, width, clut_row, image_name, note):
    palette_data = _member(extracted, by_index, pal_i)
    pixel_data = _member(extracted, by_index, pix_i)
    try:
        clut = decode_clut(palette_data)
        w, h = dimensions(len(pixel_data), bpp, width)
        rows = to_rgba_rows(unpack(pixel_data, bpp), clut, w, 16 * clut_row if bpp == 4 else 0)
    except ValueError as e:
        raise RenderFailed(f"pair {pal_i}/{pix_i}: {e}") from e
    write_png(tmp / image_name, w, h, rows)
    swatch = _swatch_rows(clut)
    write_png(tmp / f"clut{pal_i:03d}.png", SWATCH_COLUMNS * SWATCH, len(swatch), swatch)
    s = stats(clut)
    first = "transparent" if s["first_transparent"] else "opaque"
    note(f"pair {pal_i}/{pix_i}: {w}x{h} {bpp}bpp, clut[0] {first}, stp {s['stp']}/{s['size']}, black {s['black']}")


def run(extracted, out, palette=None, pixels=None, bpp=None, width=None, clut_row=0,
        expect_pairs=EXPECT_PAIRS, log=print):
    extracted, out = Path(extracted), Path(out)
    manual = [palette, pixels, bpp, width]
    if any(v is not None for v in manual) and not all(v is not None for v in manual):
        log("error: manual mode needs all of --palette, --pixels, --bpp and --width")
        return 2
    refusal = outdir.check(out, REPO_ROOT, _allowed)
    if refusal:
        log(f"error: {refusal}")
        return 2
    try:
        records = [r for r in load_index(extracted / "index.json")["files"] if r["kind"] == "rock_dat"]
    except (OSError, ValueError) as e:
        log(f"error: {e}")
        return 2
    by_index = {r["index"]: r for r in records}
    lines = []

    def note(line):
        lines.append(line)
        log(line)

    try:
        if palette is None:
            pairs = find_pairs(records)
            if len(pairs) != expect_pairs:
                raise RenderFailed(f"found {len(pairs)} palette/image pairs, expected {expect_pairs}")
            jobs = [(p, q, PAIR_BPP, PAIR_WIDTH, 0, f"pair{p:03d}_{q:03d}.png") for p, q in pairs]
        else:
            jobs = [(palette, pixels, bpp, width, clut_row, f"pal{palette:03d}_pix{pixels:03d}_{bpp}bpp_w{width}.png")]
        tmp = outdir.fresh_tmp(out)
        for job in jobs:
            _render_one(tmp, extracted, by_index, *job, note)
        (tmp / "summary.txt").write_text("\n".join(lines) + "\n")
        outdir.publish(tmp, out)
    except (RenderFailed, OSError) as e:
        log(f"error: {e}")
        return 1
    log(f"done: {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.render")
    parser.add_argument("--extracted", required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--palette", type=int)
    parser.add_argument("--pixels", type=int)
    parser.add_argument("--bpp", type=int, choices=(4, 8))
    parser.add_argument("--width", type=int)
    parser.add_argument("--clut-row", type=int, default=0, choices=range(16))
    parser.add_argument("--expect-pairs", type=int, default=EXPECT_PAIRS)
    args = parser.parse_args(argv)
    return run(Path(args.extracted).expanduser(), Path(args.out).expanduser(), args.palette, args.pixels,
               args.bpp, args.width, args.clut_row, args.expect_pairs)


if __name__ == "__main__":
    sys.exit(main())
```

- [ ] **Step 4: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 96 tests`, `OK (skipped=1)`

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/render.py tests/test_render.py
git commit -m "Add the render command for palette and image pairs

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 5: Real-data test, real run, and docs

**Files:**
- Create: `tests/test_retail_render.py`
- Modify: `README.md` (append a render usage block)
- Modify: `docs/process/process-log.md` (append a section)

**Interfaces:**
- Consumes: `render.run`

- [ ] **Step 1: Write `tests/test_retail_render.py`**

```python
import os
import tempfile
import unittest
from pathlib import Path

from mmxdemake.render import run

EXTRACTED = os.environ.get("MMX6_EXTRACTED")
WORK = Path(os.environ.get("MMX6_WORK", "~/mmx-data")).expanduser()


@unittest.skipUnless(EXTRACTED, "set MMX6_EXTRACTED to the task A output folder to run")
class RetailRenderTest(unittest.TestCase):
    def test_renders_twenty_pairs(self):
        with tempfile.TemporaryDirectory(dir=WORK) as t:
            out = Path(t) / "renders"
            lines = []
            self.assertEqual(run(Path(EXTRACTED).expanduser(), out, log=lines.append), 0, lines)
            names = [p.name for p in out.iterdir()]
            self.assertEqual(sum(n.startswith("pair") for n in names), 20)
            self.assertEqual(sum(n.startswith("clut") for n in names), 20)
            self.assertIn("summary.txt", names)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the suite, then the real-data test**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 97 tests`, `OK (skipped=2)`

Run: `MMX6_EXTRACTED=~/mmx-data/extracted/retail python3 -m unittest tests.test_retail_render 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 1 test`, `OK`. If it fails, stop and report the `error:` lines; do not change checks to make it pass.

- [ ] **Step 3: Do the real render**

Run: `python3 -m mmxdemake.render --extracted ~/mmx-data/extracted/retail --out ~/mmx-data/renders`
Expected: 20 `pair …` lines, then `done: /home/johnlavender/mmx-data/renders`, exit 0. Copy the 20 lines into the process log in Step 5.

- [ ] **Step 4: Append to `README.md`**

````markdown

Render the 20 palette + image pairs found in `ROCK_X6.DAT`:

```
python3 -m mmxdemake.render \
  --extracted ~/mmx-data/extracted/retail \
  --out ~/mmx-data/renders
```

Manual mode for any other member pair:
`--palette N --pixels M --bpp 4|8 --width W [--clut-row R]`.
````

- [ ] **Step 5: Append to `docs/process/process-log.md`**

Add a section before "Lessons so far", titled `## 16. Build the render command (task C)`, with: what was built (the 5 modules and the guard move), how it was tested (test counts from Steps 2–3, the Pillow cross-check), the 20 summary lines, the commit hashes, and a note that the user still has to look at the 20 images.

- [ ] **Step 6: Check that no game data is in the repo, then commit**

Run: `git status --short`
Expected: only `README.md`, `docs/process/process-log.md`, `tests/test_retail_render.py`.

```bash
git add README.md docs/process/process-log.md tests/test_retail_render.py
git commit -m "Add the real-data render test, usage notes and the process log entry

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```
