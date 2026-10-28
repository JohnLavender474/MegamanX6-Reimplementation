# Greyscale Tile Render Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Draw the screens of the 23 stage members with their real tile textures in greyscale (`mNNN/screens.png`), plus a texture atlas of the used cells (`mNNN/pages.png`) and a per-page summary, in `~/mmx-data/tiles/`, from the task A output only.

**Architecture:** `texture` reads a tile's raw 16 × 16 values from the `0x10000` block by the X4 rules. `shade` turns raw values into RGBA; greyscale now, a palette later through the same function shape. The `tiles` command reuses task D's member selection (`collision.stage_records`, `collision.map_records`), `arcentries`, `stagemap`, `png` and `outdir`.

**Tech Stack:** Python 3.10 standard library only.

**Spec:** `docs/superpowers/specs/2026-10-18-tile-render-design.md`

## Global Constraints

- Python 3.10, standard library only. Tests: `python3 -m unittest discover -s tests` from the repo root. No `-I`.
- Texture block: entry type `0x10000`, exactly `0x40000` bytes. 4 bpp page `p` (0–7) at `p × 0x8000`, 128 bytes per row, low nibble = left pixel. 8 bpp page `p` (8–11): base `(p & 3) × 0x10000`; cell `cx < 8` in the first half, `cx ≥ 8` in the second half at `+0x8000`; each tile row is 16 bytes at `(cx % 8) × 16`. Pages above 11: not drawn.
- Tile info word: palette bits 8–15, cx bits 16–19, cy bits 20–23, page bits 24–27.
- Shade function shape: `shade(values, bits, palette) -> bytes` (1,024 bytes RGBA, row order). `grey`: 4 bpp `v × 17`, 8 bpp `v`, alpha 255.
- `screens.png`: 8 screens per row, 256 × 256 each, 4 px gap `(60, 60, 60)`; tile 0 and pages above 11 black.
- `pages.png`: pages 0–11 in 4 columns × 3 rows, 256 × 256 each, 2 px gap `(60, 60, 60)`; unused cells `(32, 0, 32)`.
- Every 8 bpp page is labelled `unverified` in summaries.
- Output folder may hold only `mNNN` folders and `summary.txt`.
- Exit codes: 0 ok; 1 a check failed after inputs loaded, or `OSError`; 2 bad arguments, `--out` refused, `index.json` missing or unreadable.
- No game data in the repo. No code comments. No code copied from reference repos.
- Commit to `master`. End each commit message with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. Two tiles that use the same texture cell with different palettes → the cell is drawn once in `pages.png`, without error. Pinned by the Task 2 fixture (tiles 1 and 4).
2. A tile on a page above 11 → black on the screen and counted as `not drawn`. Test in Task 2.
3. An 8 bpp tile and a 4 bpp tile that read the same bytes → the overlap count is right, and both readings come from the same bytes. Test in Task 2 (tile 2 and tile 5).
4. `--member` on a screen set without `0x10102` but with `0x10000` → it is drawn. Test in Task 2.
5. Large members (256 screens): the canvas is about 70 MB in memory and the run takes seconds, not minutes. Checked on real data in Task 3 by timing the run.

---

## File structure

```
mmxdemake/
  texture.py   # new
  shade.py     # new
  tiles.py     # new
tests/
  test_texture.py       # new
  test_shade.py         # new
  test_tiles.py         # new
  test_retail_tiles.py  # new
```

---

### Task 1: `texture` and `shade`

**Files:**
- Create: `mmxdemake/texture.py`, `mmxdemake/shade.py`
- Test: `tests/test_texture.py`, `tests/test_shade.py`

**Interfaces:**
- Produces:
  - `texture.BLOCK_SIZE = 0x40000`, `texture.TILE = 16`
  - `texture.TileRef(page: int, cx: int, cy: int, palette: int)`, frozen dataclass
  - `texture.tile_ref(w: int) -> TileRef`
  - `texture.bits(page: int) -> int | None`
  - `texture.raw_tile(block: bytes, ref: TileRef) -> list[int] | None` (raises `ValueError` on a wrong block size)
  - `texture.covered_4bpp_cells(ref: TileRef) -> set[tuple[int, int, int]]`
  - `shade.grey(values: list[int], bits: int, palette: int) -> bytes`

- [ ] **Step 1: Write the failing tests `tests/test_texture.py`**

```python
import unittest

from mmxdemake.texture import BLOCK_SIZE, TileRef, bits, covered_4bpp_cells, raw_tile, tile_ref


class RefTest(unittest.TestCase):
    def test_tile_ref(self):
        w = (3 << 24) | (2 << 20) | (1 << 16) | (5 << 8) | 0x38
        self.assertEqual(tile_ref(w), TileRef(3, 1, 2, 5))

    def test_bits(self):
        self.assertEqual([bits(p) for p in (0, 7, 8, 11, 12)], [4, 4, 8, 8, None])


class RawTileTest(unittest.TestCase):
    def setUp(self):
        self.block = bytearray(BLOCK_SIZE)

    def test_4bpp_low_nibble_first(self):
        row0 = 3 * 0x8000 + (2 * 16) * 128 + 1 * 8
        self.block[row0] = 0x21
        self.block[row0 + 128] = 0x0F
        values = raw_tile(bytes(self.block), TileRef(3, 1, 2, 0))
        self.assertEqual(len(values), 256)
        self.assertEqual(values[0:2], [1, 2])
        self.assertEqual(values[16:18], [15, 0])

    def test_8bpp_first_half(self):
        self.block[0x10000 + 2 * 16] = 200
        self.block[0x10000 + 128 + 2 * 16 + 15] = 9
        values = raw_tile(bytes(self.block), TileRef(9, 2, 0, 0))
        self.assertEqual((values[0], values[1], values[31]), (200, 0, 9))

    def test_8bpp_second_half(self):
        self.block[0x18000 + 1 * 16] = 77
        self.block[0x10000 + 9 * 16] = 5
        values = raw_tile(bytes(self.block), TileRef(9, 9, 0, 0))
        self.assertEqual(values[0], 77)

    def test_page_above_11_is_none(self):
        self.assertIsNone(raw_tile(bytes(self.block), TileRef(12, 0, 0, 0)))

    def test_wrong_block_size(self):
        with self.assertRaisesRegex(ValueError, "texture block is 10 bytes, need 262144"):
            raw_tile(b"\0" * 10, TileRef(0, 0, 0, 0))

    def test_covered_cells(self):
        self.assertEqual(covered_4bpp_cells(TileRef(9, 2, 4, 0)), {(2, 4, 4), (2, 5, 4)})
        self.assertEqual(covered_4bpp_cells(TileRef(9, 9, 4, 0)), {(3, 2, 4), (3, 3, 4)})
        self.assertEqual(covered_4bpp_cells(TileRef(11, 0, 0, 0)), {(6, 0, 0), (6, 1, 0)})
        self.assertEqual(covered_4bpp_cells(TileRef(2, 4, 0, 0)), set())


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Write the failing tests `tests/test_shade.py`**

```python
import unittest

from mmxdemake.shade import grey


class GreyTest(unittest.TestCase):
    def test_grey_4bpp(self):
        self.assertEqual(grey([0, 1, 15], 4, 99), bytes([0, 0, 0, 255, 17, 17, 17, 255, 255, 255, 255, 255]))

    def test_grey_8bpp(self):
        self.assertEqual(grey([0, 200, 255], 8, 0), bytes([0, 0, 0, 255, 200, 200, 200, 255, 255, 255, 255, 255]))


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 3: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p "test_[ts][eh]*.py" 2>&1 | grep Error`
Expected: `ModuleNotFoundError: No module named 'mmxdemake.texture'` and `… 'mmxdemake.shade'`

- [ ] **Step 4: Write `mmxdemake/texture.py`**

```python
from dataclasses import dataclass

BLOCK_SIZE = 0x40000
PAGE_4BPP = 0x8000
PAGE_8BPP = 0x10000
HALF_8BPP = 0x8000
ROW = 128
TILE = 16


@dataclass(frozen=True)
class TileRef:
    page: int
    cx: int
    cy: int
    palette: int


def tile_ref(w):
    return TileRef((w >> 24) & 0xF, (w >> 16) & 0xF, (w >> 20) & 0xF, (w >> 8) & 0xFF)


def bits(page):
    if page < 8:
        return 4
    if page <= 11:
        return 8
    return None


def raw_tile(block, ref):
    if len(block) != BLOCK_SIZE:
        raise ValueError(f"texture block is {len(block)} bytes, need {BLOCK_SIZE}")
    kind = bits(ref.page)
    if kind is None:
        return None
    values = []
    for r in range(TILE):
        y = ref.cy * TILE + r
        if kind == 4:
            start = ref.page * PAGE_4BPP + y * ROW + ref.cx * 8
            for b in block[start:start + 8]:
                values += (b & 15, b >> 4)
        else:
            base = (ref.page & 3) * PAGE_8BPP + (HALF_8BPP if ref.cx >= 8 else 0)
            start = base + y * ROW + (ref.cx % 8) * TILE
            values += block[start:start + TILE]
    return values


def covered_4bpp_cells(ref):
    if bits(ref.page) != 8:
        return set()
    page4 = 2 * (ref.page & 3) + (1 if ref.cx >= 8 else 0)
    x4 = 2 * (ref.cx % 8)
    return {(page4, x4, ref.cy), (page4, x4 + 1, ref.cy)}
```

- [ ] **Step 5: Write `mmxdemake/shade.py`**

```python
def grey(values, bits, palette):
    scale = 17 if bits == 4 else 1
    return b"".join(bytes((v * scale, v * scale, v * scale, 255)) for v in values)
```

- [ ] **Step 6: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 137 tests`, `OK (skipped=3)`

- [ ] **Step 7: Commit**

```bash
git add mmxdemake/texture.py mmxdemake/shade.py tests/test_texture.py tests/test_shade.py
git commit -m "Read raw tile values from the texture block and shade them in grey

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: `tiles` command

**Files:**
- Create: `mmxdemake/tiles.py`
- Test: `tests/test_tiles.py`

**Interfaces:**
- Consumes: `texture.*`, `shade.grey`, `arcentries.split/only/entry_data/ArcError`, `stagemap.SIDE/check/decode_screens/decode_tile_info/tile`, `collision.stage_records/map_records`, `index.load_index`, `png.write_png`, `outdir.check/fresh_tmp/publish`; tests use `synth.build_member`, `index.build_index/write_index`, `rockarc.observe_header`, `pngread.read_png`.
- Produces:
  - `tiles.REPO_ROOT`, `tiles.GAP_RGB`, `tiles.UNUSED_RGB`
  - `tiles.run(extracted: Path, out: Path, member: int | None = None, expect_stages: int = 23, log=print) -> int`
  - `tiles.main(argv: list[str] | None = None) -> int`

- [ ] **Step 1: Write the failing tests `tests/test_tiles.py`**

```python
import hashlib
import os
import struct
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import synth
from mmxdemake import tiles
from mmxdemake.index import build_index, write_index
from mmxdemake.rockarc import observe_header
from mmxdemake.tiles import GAP_RGB, UNUSED_RGB, main, run
from pngread import read_png

INFO = [0, (3 << 24) | (2 << 20) | (1 << 16), (9 << 24) | (2 << 16), 15 << 24,
        (3 << 24) | (2 << 20) | (1 << 16) | (7 << 8), (2 << 24) | (4 << 16)]


def block():
    b = bytearray(0x40000)
    b[3 * 0x8000 + (2 * 16) * 128 + 1 * 8] = 0x21
    b[0x10000 + 2 * 16] = 200
    return bytes(b)


def screens():
    s0 = [0] * 256
    s0[0], s0[1], s0[2], s0[3], s0[4], s0[5] = 1, 2, 0, 3, 4, 5
    return struct.pack("<512H", *(s0 + [1] * 256))


def stage_member(with_texture=True, with_objects=True):
    parts = [(0x10102, b"x" * 100)] if with_objects else []
    parts += [(0, screens()), (1, struct.pack("<6I", *INFO))]
    if with_texture:
        parts.append((0x10000, block()))
    return synth.build_member(parts)


def write_extracted(folder, members):
    (folder / "rock_dat").mkdir(parents=True, exist_ok=True)
    records = []
    for i, data in enumerate(members):
        rel = f"rock_dat/{i:03d}.bin"
        (folder / rel).write_bytes(data)
        records.append({"kind": "rock_dat", "path": rel, "index": i, "sector": 0, "size": len(data),
                        "sha1": hashlib.sha1(data).hexdigest(), "observed": observe_header(data, True)})
    write_index(folder / "index.json", build_index({"serial": "S", "revision": "r", "bin_sha1": "0" * 40}, records))


def pixel(rows, x, y):
    return tuple(rows[y][x * 4:x * 4 + 3])


class TilesTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.extracted = self.dir / "extracted"
        write_extracted(self.extracted, [b"\0" * 4096, stage_member(), stage_member()])
        self.lines = []

    def tearDown(self):
        self.tmp.cleanup()

    def go(self, out_name="tiles", **kw):
        self.lines = []
        kw.setdefault("expect_stages", 2)
        return run(self.extracted, self.dir / out_name, log=self.lines.append, **kw)

    def test_default_run(self):
        self.assertEqual(self.go(), 0, self.lines)
        out = self.dir / "tiles"
        self.assertEqual(sorted(p.name for p in out.iterdir()), ["m001", "m002", "summary.txt"])
        self.assertEqual(sorted(p.name for p in (out / "m001").iterdir()), ["pages.png", "screens.png", "summary.txt"])
        self.assertEqual((out / "summary.txt").read_text().splitlines(),
                         ["member 1: 2 screens, 5 tiles used", "member 2: 2 screens, 5 tiles used", "done"])
        self.assertFalse((self.dir / "tiles.tmp").exists())

    def test_screen_pixels(self):
        self.go()
        width, height, rows = read_png(self.dir / "tiles/m001/screens.png")
        self.assertEqual((width, height), (524, 264))
        self.assertEqual(pixel(rows, 0, 0), GAP_RGB)
        self.assertEqual(pixel(rows, 4, 4), (17, 17, 17))
        self.assertEqual(pixel(rows, 5, 4), (34, 34, 34))
        self.assertEqual(pixel(rows, 20, 4), (200, 200, 200))
        self.assertEqual(pixel(rows, 36, 4), (0, 0, 0))
        self.assertEqual(pixel(rows, 52, 4), (0, 0, 0))
        self.assertEqual(pixel(rows, 264, 4), (17, 17, 17))

    def test_pages_pixels(self):
        self.go()
        width, height, rows = read_png(self.dir / "tiles/m001/pages.png")
        self.assertEqual((width, height), (1034, 776))
        self.assertEqual(pixel(rows, 776 + 16, 2 + 32), (17, 17, 17))
        self.assertEqual(pixel(rows, 2 + 32, 2 + 32), UNUSED_RGB)
        self.assertEqual(pixel(rows, 260 + 32, 518), (200, 200, 200))
        self.assertEqual(pixel(rows, 518 + 64, 2), (136, 136, 136))

    def test_summary_lines(self):
        self.go()
        self.assertEqual((self.dir / "tiles/m001/summary.txt").read_text().splitlines(), [
            "member 1: 2 screens, 5 tiles used",
            "page 2 (4bpp): 1 tiles, 1 cells",
            "page 3 (4bpp): 2 tiles, 1 cells",
            "page 9 (8bpp, unverified): 1 tiles, 1 cells",
            "page 15 (not drawn): 1 tiles",
            "4bpp cells also read by 8bpp tiles: 1",
        ])

    def test_missing_texture_entry(self):
        write_extracted(self.extracted, [b"\0" * 4096, stage_member(with_texture=False), stage_member()])
        self.assertEqual(self.go(), 1)
        self.assertIn("error: member 1: 0 entries of type 0x10000, need exactly 1", self.lines)
        self.assertFalse((self.dir / "tiles").exists())

    def test_changed_member_fails(self):
        p = self.extracted / "rock_dat/002.bin"
        p.write_bytes(p.read_bytes()[:-1] + b"\x01")
        self.assertEqual(self.go(), 1)
        self.assertTrue(any(line.startswith("error: member 2: size") for line in self.lines))

    def test_wrong_stage_count(self):
        self.assertEqual(self.go(expect_stages=3), 1)
        self.assertIn("error: found 2 stage members, expected 3", self.lines)

    def test_member_mode_screen_set(self):
        write_extracted(self.extracted, [b"\0" * 4096, stage_member(), stage_member(), stage_member(with_objects=False)])
        self.assertEqual(self.go(member=3), 0, self.lines)
        self.assertTrue((self.dir / "tiles/m003/screens.png").exists())

    def test_rerun_replaces(self):
        self.assertEqual(self.go(), 0)
        self.assertEqual(self.go(member=2), 0, self.lines)
        self.assertFalse((self.dir / "tiles/m001").exists())

    def test_out_inside_repo_refused(self):
        inside = tiles.REPO_ROOT / "tiles-test-should-not-exist"
        self.assertEqual(run(self.extracted, inside, expect_stages=2, log=self.lines.append), 2)
        self.assertFalse(inside.exists())

    def test_missing_index(self):
        (self.extracted / "index.json").unlink()
        self.assertEqual(self.go(), 2)
        self.assertTrue(any(line.startswith("error:") and "index.json" in line for line in self.lines))

    def test_cli_expands_home(self):
        with mock.patch.dict(os.environ, {"HOME": str(self.dir)}), mock.patch("builtins.print"):
            code = main(["--extracted", "~/extracted", "--out", "~/cli-tiles", "--expect-stages", "2"])
        self.assertEqual(code, 0)
        self.assertTrue((self.dir / "cli-tiles/summary.txt").exists())


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_tiles.py 2>&1 | grep Error`
Expected: `ImportError: cannot import name 'tiles' from 'mmxdemake'`

- [ ] **Step 3: Write `mmxdemake/tiles.py`**

```python
import argparse
import hashlib
import re
import sys
from collections import Counter
from math import ceil
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.arcentries import ArcError, entry_data, only, split
from mmxdemake.collision import map_records, stage_records
from mmxdemake.index import load_index
from mmxdemake.png import write_png
from mmxdemake.shade import grey
from mmxdemake.stagemap import SIDE, check, decode_screens, decode_tile_info, tile
from mmxdemake.texture import BLOCK_SIZE, TILE, bits, covered_4bpp_cells, raw_tile, tile_ref

REPO_ROOT = Path(__file__).resolve().parents[1]
EXPECT_STAGES = 23
TEXTURE_TYPE = 0x10000
SCREEN_PX = SIDE * TILE
SCREENS_PER_ROW = 8
SCREEN_GAP = 4
PAGE_GAP = 2
PAGE_COLUMNS = 4
PAGES = 12
GAP_RGB = (60, 60, 60)
UNUSED_RGB = (32, 0, 32)
BLACK_RGB = (0, 0, 0)


class TilesFailed(Exception):
    pass


def _allowed(name):
    return name == "summary.txt" or re.fullmatch(r"m\d{3}", name) is not None


def _canvas(width, height, rgb):
    return [bytearray(bytes(rgb + (255,)) * width) for _ in range(height)]


def _fill(rgb):
    return bytes(rgb + (255,)) * (TILE * TILE)


def _blit(rows, x, y, rgba):
    stride = TILE * 4
    for r in range(TILE):
        rows[y + r][x * 4:x * 4 + stride] = rgba[r * stride:(r + 1) * stride]


def _shaded(block, ref, shade):
    values = raw_tile(block, ref)
    return None if values is None else shade(values, bits(ref.page), ref.palette)


def _screens_png(screens, info, block, shade):
    step = SCREEN_PX + SCREEN_GAP
    width = min(SCREENS_PER_ROW, len(screens)) * step + SCREEN_GAP
    height = ceil(len(screens) / SCREENS_PER_ROW) * step + SCREEN_GAP
    rows = _canvas(width, height, GAP_RGB)
    black = _fill(BLACK_RGB)
    cache = {}
    for n, screen in enumerate(screens):
        ox = (n % SCREENS_PER_ROW) * step + SCREEN_GAP
        oy = (n // SCREENS_PER_ROW) * step + SCREEN_GAP
        for k, v in enumerate(screen):
            t = tile(v)
            if t and t not in cache:
                cache[t] = _shaded(block, tile_ref(info[t]), shade)
            rgba = cache.get(t) if t else None
            _blit(rows, ox + (k % SIDE) * TILE, oy + (k // SIDE) * TILE, rgba or black)
    return width, height, [bytes(r) for r in rows]


def _pages_png(refs, block, shade):
    step = SCREEN_PX + PAGE_GAP
    width = PAGE_COLUMNS * step + PAGE_GAP
    height = ceil(PAGES / PAGE_COLUMNS) * step + PAGE_GAP
    rows = _canvas(width, height, GAP_RGB)
    unused = _fill(UNUSED_RGB)
    origin = {p: ((p % PAGE_COLUMNS) * step + PAGE_GAP, (p // PAGE_COLUMNS) * step + PAGE_GAP) for p in range(PAGES)}
    for p, (ox, oy) in origin.items():
        for cy in range(SIDE):
            for cx in range(SIDE):
                _blit(rows, ox + cx * TILE, oy + cy * TILE, unused)
    cells = {}
    for ref in refs:
        if bits(ref.page) is not None:
            cells.setdefault((ref.page, ref.cx, ref.cy), ref)
    for (p, cx, cy), ref in sorted(cells.items()):
        ox, oy = origin[p]
        _blit(rows, ox + cx * TILE, oy + cy * TILE, _shaded(block, ref, shade))
    return width, height, [bytes(r) for r in rows]


def _summary(i, screens, refs_by_tile):
    refs = list(refs_by_tile.values())
    lines = [f"member {i}: {len(screens)} screens, {len(refs)} tiles used"]
    per_page = Counter(r.page for r in refs)
    for p in sorted(per_page):
        b = bits(p)
        if b is None:
            lines.append(f"page {p} (not drawn): {per_page[p]} tiles")
            continue
        label = "4bpp" if b == 4 else "8bpp, unverified"
        cell_count = len({(r.cx, r.cy) for r in refs if r.page == p})
        lines.append(f"page {p} ({label}): {per_page[p]} tiles, {cell_count} cells")
    cells4 = {(r.page, r.cx, r.cy) for r in refs if bits(r.page) == 4}
    covered = set()
    for r in refs:
        covered |= covered_4bpp_cells(r)
    lines.append(f"4bpp cells also read by 8bpp tiles: {len(cells4 & covered)}")
    return lines


def _process(extracted, tmp, r, shade, log):
    i = r["index"]
    try:
        body = (extracted / r["path"]).read_bytes()
        sha1 = hashlib.sha1(body).hexdigest()
        if len(body) != r["size"] or sha1 != r["sha1"]:
            raise TilesFailed(f"member {i}: size {len(body)}, sha1 {sha1} do not match the index "
                              f"({r['size']}, {r['sha1']})")
        entries = split(body)
        screens_entry, info_entry, texture_entry = only(entries, 0), only(entries, 1), only(entries, TEXTURE_TYPE)
        screens = decode_screens(entry_data(body, screens_entry))
        info = decode_tile_info(entry_data(body, info_entry))
        block = entry_data(body, texture_entry)
        if len(block) != BLOCK_SIZE:
            raise ValueError(f"texture block is {len(block)} bytes, need {BLOCK_SIZE}")
    except (OSError, ArcError, ValueError) as e:
        raise TilesFailed(f"member {i}: {e}") from e
    if not screens:
        raise TilesFailed(f"member {i}: screens entry is empty")
    problems = check(screens, info)
    if problems:
        raise TilesFailed(f"member {i}: " + "; ".join(str(p) for p in problems))
    used = sorted({tile(v) for s in screens for v in s} - {0})
    refs_by_tile = {t: tile_ref(info[t]) for t in used}
    folder = tmp / f"m{i:03d}"
    folder.mkdir()
    write_png(folder / "screens.png", *_screens_png(screens, info, block, shade))
    write_png(folder / "pages.png", *_pages_png(refs_by_tile.values(), block, shade))
    lines = _summary(i, screens, refs_by_tile)
    (folder / "summary.txt").write_text("\n".join(lines) + "\n")
    log(lines[0])
    return lines[0]


def run(extracted, out, member=None, expect_stages=EXPECT_STAGES, log=print):
    extracted, out = Path(extracted), Path(out)
    refusal = outdir.check(out, REPO_ROOT, _allowed)
    if refusal:
        log(f"error: {refusal}")
        return 2
    try:
        records = [r for r in load_index(extracted / "index.json")["files"] if r["kind"] == "rock_dat"]
    except (OSError, ValueError, KeyError, TypeError, AttributeError) as e:
        log(f"error: {e}")
        return 2
    try:
        if member is None:
            chosen = stage_records(records)
            if len(chosen) != expect_stages:
                raise TilesFailed(f"found {len(chosen)} stage members, expected {expect_stages}")
        else:
            chosen = [r for r in map_records(records) if r["index"] == member]
            if not chosen:
                raise TilesFailed(f"member {member} is not a screen set (needs entry types 0 and 1)")
        tmp = outdir.fresh_tmp(out)
        headers = [_process(extracted, tmp, r, grey, log) for r in chosen]
        (tmp / "summary.txt").write_text("\n".join(headers + ["done"]) + "\n")
        outdir.publish(tmp, out)
    except (TilesFailed, OSError) as e:
        log(f"error: {e}")
        return 1
    log(f"done: {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.tiles")
    parser.add_argument("--extracted", required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--member", type=int)
    parser.add_argument("--expect-stages", type=int, default=EXPECT_STAGES)
    args = parser.parse_args(argv)
    return run(Path(args.extracted).expanduser(), Path(args.out).expanduser(), args.member,
               args.expect_stages, log=print)


if __name__ == "__main__":
    sys.exit(main())
```

- [ ] **Step 4: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 149 tests`, `OK (skipped=3)`

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/tiles.py tests/test_tiles.py
git commit -m "Add the tiles command that draws stage screens with real textures

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: Real-data test, real run, and docs

**Files:**
- Create: `tests/test_retail_tiles.py`
- Modify: `README.md`, `docs/process/process-log.md`

- [ ] **Step 1: Write `tests/test_retail_tiles.py`**

```python
import os
import tempfile
import unittest
from pathlib import Path

from mmxdemake.tiles import run

EXTRACTED = os.environ.get("MMX6_EXTRACTED")
WORK = Path(os.environ.get("MMX6_WORK", "~/mmx-data")).expanduser()


@unittest.skipUnless(EXTRACTED, "set MMX6_EXTRACTED to the task A output folder to run")
class RetailTilesTest(unittest.TestCase):
    def test_twenty_three_stage_members(self):
        with tempfile.TemporaryDirectory(dir=WORK) as t:
            out = Path(t) / "tiles"
            lines = []
            self.assertEqual(run(Path(EXTRACTED).expanduser(), out, log=lines.append), 0, lines)
            folders = sorted(p for p in out.iterdir() if p.is_dir())
            self.assertEqual(len(folders), 23)
            for folder in folders:
                self.assertEqual(sorted(p.name for p in folder.iterdir()), ["pages.png", "screens.png", "summary.txt"])


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the suite, then the real-data test**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 150 tests`, `OK (skipped=4)`

Run: `time (MMX6_EXTRACTED=~/mmx-data/extracted/retail python3 -m unittest tests.test_retail_tiles 2>&1 | grep -E "^(Ran|OK|FAILED)|Error")`
Expected: `Ran 1 test`, `OK`. Record the time for Review Focus item 5. If it fails, stop and find the cause; do not loosen a check to make it pass.

- [ ] **Step 3: Do the real run**

Run: `python3 -m mmxdemake.tiles --extracted ~/mmx-data/extracted/retail --out ~/mmx-data/tiles`
Expected: 23 `member …` lines, then `done: /home/johnlavender/mmx-data/tiles`, exit 0.

- [ ] **Step 4: Look at the output before reporting**

View `m094/screens.png`, `m094/pages.png` and two other members' `screens.png` (scale down with a throwaway script in the scratchpad if they are too big to view). Read each `summary.txt` overlap line. Record in the process log what the screens show, which pages look like noise, and the overlap counts.

- [ ] **Step 5: Append to `README.md`**

````markdown

Draw the stage screens with their real tile textures (greyscale; palettes are
not decoded yet):

```
python3 -m mmxdemake.tiles \
  --extracted ~/mmx-data/extracted/retail \
  --out ~/mmx-data/tiles
```

Each `mNNN/` folder holds `screens.png` (every screen at 256 × 256),
`pages.png` (the texture cells the screens use) and `summary.txt` (tiles and
cells per page; 8 bpp pages are marked unverified).
````

- [ ] **Step 6: Append to `docs/process/process-log.md`**

Add a section before "Lessons so far", titled `## 28. Build the tiles command (task E)`, with: what was built, the test counts, the run time, what the screens and pages show from Step 4, the overlap counts, the commit hashes, and that the user still has to look at the screens.

- [ ] **Step 7: Check that no game data is in the repo, then commit**

Run: `git status --short`
Expected: only `README.md`, `docs/process/process-log.md`, `tests/test_retail_tiles.py`.

```bash
git add README.md docs/process/process-log.md tests/test_retail_tiles.py
git commit -m "Add the real-data tiles test, usage notes and the process log entry

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```
