# Stage Screens and Collision Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Decode the screens (entry type 0) and tile info (entry type 1) of the 23 stage members of `ROCK_X6.DAT` into `screens.json`, a collision contact sheet and a summary per member, in `~/mmx-data/collision/`, using only the task A output.

**Architecture:** `arcentries` cuts any DAT member into typed entries by the proven `0x800` rule. `stagemap` decodes screens and tile info and checks tile numbers. The `collision` command finds the stage members in `index.json`, checks each member's SHA-1, decodes, and writes outputs through the shared `outdir` guard (`<out>.tmp/` → `<out>/`).

**Tech Stack:** Python 3.10 standard library only (`struct`, `colorsys`, `json`, `hashlib`, `re`, `argparse`, `unittest`). Reuses `png.write_png`, `outdir`, `index.load_index`, `rockarc.observe_header` (tests), `tests/pngread.py` (tests).

**Spec:** `docs/superpowers/specs/2026-10-18-collision-map-design.md`

## Global Constraints

- Python 3.10, standard library only. Tests: `python3 -m unittest discover -s tests` from the repo root. No `-I`.
- Member sub-header: `{u32 count, u32 size}` then `count` × `{u32 type, u32 size}`. First entry at `0x800`; each entry padded to `0x800`; end must equal the member size. `count` in `1..63`. (The spec's "header list runs past `0x800`" check cannot trigger with `count ≤ 63`, so it is not coded.)
- Screens: `u16` per cell, 256 cells per screen (`0x200` B), row order. `tile = v & 0x3FFF`, `flags = v >> 14`. Tile info: `u32`; `collision = w & 0x3F`. A cell's collision is `collision(tile_info[tile])`.
- Expected stage members: 23 (`--expect-stages`).
- Sheet: 16 screens per row; cell 4 × 4 px; 2 px gap; gap `(60, 60, 60)`; tile 0 `(16, 16, 16)`; non-zero tile with collision 0 `(40, 40, 64)`; collision `c > 0` → `COLOURS[c]`, where `COLOURS[c] = round(255 × colorsys.hsv_to_rgb((c × 0.618034) mod 1, 0.75, 0.95))` and `COLOURS[0] = (40, 40, 64)`.
- Legend: 128 × 128, 8 × 8 squares of 16 px, square (r, c) = `COLOURS[8r + c]`.
- Output folder may hold only `mNNN` folders, `summary.txt`, `legend.png`.
- Exit codes: 0 ok; 1 a check failed after the inputs loaded, or an `OSError`; 2 bad arguments, `--out` refused, `index.json` missing or unreadable as an index.
- No game data in the repo. No code comments. No code copied from reference repos.
- Commit to `master`. End each commit message with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. Index records with no `observed` field → not treated as stage members, exit 1 with a count message, no traceback. Test in Task 3.
2. A second run into the same `--out` → replaces the earlier output (the `outdir` rule allows `mNNN`). Test in Task 3.
3. A stage member whose screens entry is not the first entry → found by type, not position. Pinned by the Task 3 fixture (type `0x10102` comes first).
4. `--member` with a number that is not a stage member → exit 1 naming it. Test in Task 3.
5. `main()` output is quiet under `mock.patch("builtins.print")`, because `main` passes `log=print` at call time. Test in Task 3.

---

## File structure

```
mmxdemake/
  arcentries.py   # new
  stagemap.py     # new
  collision.py    # new
tests/
  synth.py                    # modified: add build_member
  test_arcentries.py          # new
  test_stagemap.py            # new
  test_collision.py           # new
  test_retail_collision.py    # new
```

---

### Task 1: `arcentries`

**Files:**
- Create: `mmxdemake/arcentries.py`
- Modify: `tests/synth.py` (append `build_member`)
- Test: `tests/test_arcentries.py`

**Interfaces:**
- Produces:
  - `arcentries.HEADER = 0x800`
  - `arcentries.Entry(type: int, offset: int, size: int)`, frozen dataclass
  - `arcentries.ArcError(Exception)`
  - `arcentries.split(body: bytes) -> list[Entry]`
  - `arcentries.entry_data(body: bytes, e: Entry) -> bytes`
  - `arcentries.only(entries: list[Entry], type_: int) -> Entry`
  - `synth.build_member(entries: list[tuple[int, bytes]]) -> bytes`

- [ ] **Step 1: Append `build_member` to `tests/synth.py`**

```python


def build_member(entries):
    header = bytearray(2048)
    body = b""
    for k, (entry_type, data) in enumerate(entries):
        struct.pack_into("<II", header, 8 + 8 * k, entry_type, len(data))
        body += data.ljust(ceil(len(data) / 2048) * 2048, b"\0")
    struct.pack_into("<II", header, 0, len(entries), 2048 + len(body))
    return bytes(header) + body
```

- [ ] **Step 2: Write the failing tests `tests/test_arcentries.py`**

```python
import struct
import unittest

import synth
from mmxdemake.arcentries import ArcError, Entry, entry_data, only, split


class SplitTest(unittest.TestCase):
    def setUp(self):
        self.parts = [(0x10102, b"a" * 10), (0, b"b" * 0x800), (1, b"c" * 0x801)]
        self.body = synth.build_member(self.parts)

    def test_split_offsets(self):
        entries = split(self.body)
        self.assertEqual(entries, [Entry(0x10102, 0x800, 10), Entry(0, 0x1000, 0x800), Entry(1, 0x1800, 0x801)])
        self.assertEqual(len(self.body), 0x2800)
        self.assertEqual([entry_data(self.body, e) for e in entries], [d for _, d in self.parts])

    def test_too_short(self):
        with self.assertRaisesRegex(ArcError, "member is 100 bytes, below the 0x800 header"):
            split(b"\0" * 100)

    def test_bad_count(self):
        for count in (0, 64):
            b = bytearray(self.body)
            struct.pack_into("<I", b, 0, count)
            with self.assertRaisesRegex(ArcError, f"entry count {count} is not in 1..63"):
                split(bytes(b))

    def test_size_mismatch(self):
        with self.assertRaisesRegex(ArcError, f"header size {0x2800} != member size {0x3000}"):
            split(self.body + b"\0" * 0x800)

    def test_entry_past_end(self):
        b = bytearray(self.body)
        struct.pack_into("<I", b, 8 + 8 * 2 + 4, 0x5000)
        with self.assertRaisesRegex(ArcError, "entry 2 \\(type 0x1\\) at 6144 size 20480 runs past the member end 10240"):
            split(bytes(b))

    def test_end_mismatch(self):
        b = bytearray(self.body + b"\0" * 0x800)
        struct.pack_into("<I", b, 4, len(b))
        with self.assertRaisesRegex(ArcError, f"entries end at {0x2800}, member size is {0x3000}"):
            split(bytes(b))

    def test_only(self):
        entries = [Entry(7, 0, 1), Entry(0, 0, 1), Entry(7, 0, 1)]
        self.assertEqual(only(entries, 0), Entry(0, 0, 1))
        with self.assertRaisesRegex(ArcError, "0 entries of type 0x1, need exactly 1"):
            only(entries, 1)
        with self.assertRaisesRegex(ArcError, "2 entries of type 0x7, need exactly 1"):
            only(entries, 7)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 3: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_arcentries.py 2>&1 | grep Error`
Expected: `ModuleNotFoundError: No module named 'mmxdemake.arcentries'`

- [ ] **Step 4: Write `mmxdemake/arcentries.py`**

```python
import struct
from dataclasses import dataclass
from math import ceil

HEADER = 0x800
MAX_COUNT = 64


@dataclass(frozen=True)
class Entry:
    type: int
    offset: int
    size: int


class ArcError(Exception):
    pass


def split(body):
    if len(body) < HEADER:
        raise ArcError(f"member is {len(body)} bytes, below the 0x800 header")
    count, size = struct.unpack_from("<II", body)
    if not 0 < count < MAX_COUNT:
        raise ArcError(f"entry count {count} is not in 1..63")
    if size != len(body):
        raise ArcError(f"header size {size} != member size {len(body)}")
    entries, offset = [], HEADER
    for k in range(count):
        entry_type, entry_size = struct.unpack_from("<II", body, 8 + 8 * k)
        if offset + entry_size > len(body):
            raise ArcError(f"entry {k} (type {entry_type:#x}) at {offset} size {entry_size} "
                           f"runs past the member end {len(body)}")
        entries.append(Entry(entry_type, offset, entry_size))
        offset += ceil(entry_size / HEADER) * HEADER
    if offset != len(body):
        raise ArcError(f"entries end at {offset}, member size is {len(body)}")
    return entries


def entry_data(body, e):
    return body[e.offset:e.offset + e.size]


def only(entries, type_):
    found = [e for e in entries if e.type == type_]
    if len(found) != 1:
        raise ArcError(f"{len(found)} entries of type {type_:#x}, need exactly 1")
    return found[0]
```

- [ ] **Step 5: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 104 tests`, `OK (skipped=2)`

- [ ] **Step 6: Commit**

```bash
git add mmxdemake/arcentries.py tests/synth.py tests/test_arcentries.py
git commit -m "Split DAT members into typed entries

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: `stagemap`

**Files:**
- Create: `mmxdemake/stagemap.py`
- Test: `tests/test_stagemap.py`

**Interfaces:**
- Consumes: `problem.Problem`
- Produces:
  - `stagemap.SIDE = 16`, `stagemap.SCREEN_BYTES = 0x200`
  - `stagemap.decode_screens(data: bytes) -> list[list[int]]` (raises `ValueError`)
  - `stagemap.decode_tile_info(data: bytes) -> list[int]` (raises `ValueError`)
  - `stagemap.tile(v: int) -> int`, `stagemap.flags(v: int) -> int`, `stagemap.collision(w: int) -> int`
  - `stagemap.check(screens: list[list[int]], info: list[int]) -> list[Problem]`

- [ ] **Step 1: Write the failing tests `tests/test_stagemap.py`**

```python
import struct
import unittest

from mmxdemake.stagemap import check, collision, decode_screens, decode_tile_info, flags, tile


class ScreensTest(unittest.TestCase):
    def test_screen_order_and_fields(self):
        values = [0] * 256
        values[1 * 16 + 1] = 0x8005
        screens = decode_screens(struct.pack("<256H", *values))
        self.assertEqual(len(screens), 1)
        self.assertEqual(screens[0][17], 0x8005)
        self.assertEqual((tile(0x8005), flags(0x8005)), (5, 2))
        self.assertEqual((tile(0x3FFF), flags(0xC000)), (0x3FFF, 3))

    def test_two_screens(self):
        screens = decode_screens(struct.pack("<512H", *range(512)))
        self.assertEqual([s[0] for s in screens], [0, 256])

    def test_screen_length_error(self):
        with self.assertRaisesRegex(ValueError, "screens entry is 100 bytes, not a multiple of 512"):
            decode_screens(b"\0" * 100)


class TileInfoTest(unittest.TestCase):
    def test_tile_info_and_collision(self):
        self.assertEqual(decode_tile_info(struct.pack("<2I", 0x0B0A0338, 7)), [0x0B0A0338, 7])
        self.assertEqual((collision(0x0B0A0338), collision(0xFF)), (56, 63))

    def test_tile_info_length_error(self):
        with self.assertRaisesRegex(ValueError, "tile info entry is 6 bytes, not a multiple of 4"):
            decode_tile_info(b"\0" * 6)


class CheckTest(unittest.TestCase):
    def test_check_names_cell(self):
        screen = [0] * 256
        screen[2 * 16 + 3] = 9
        self.assertEqual([str(p) for p in check([screen], [0] * 4)],
                         ["screen 0 cell (3,2): tile 9 is not below the tile count 4"])
        self.assertEqual(check([screen], [0] * 10), [])

    def test_check_caps_list(self):
        problems = [str(p) for p in check([[9] * 256], [0] * 4)]
        self.assertEqual(len(problems), 21)
        self.assertEqual(problems[-1], "screens: 236 more cells out of range")


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_stagemap.py 2>&1 | grep Error`
Expected: `ModuleNotFoundError: No module named 'mmxdemake.stagemap'`

- [ ] **Step 3: Write `mmxdemake/stagemap.py`**

```python
import struct

from mmxdemake.problem import Problem

SIDE = 16
SCREEN_BYTES = 0x200
CELLS = SIDE * SIDE
MAX_LISTED = 20


def decode_screens(data):
    if len(data) % SCREEN_BYTES:
        raise ValueError(f"screens entry is {len(data)} bytes, not a multiple of {SCREEN_BYTES}")
    values = struct.unpack(f"<{len(data) // 2}H", data)
    return [list(values[i:i + CELLS]) for i in range(0, len(values), CELLS)]


def decode_tile_info(data):
    if len(data) % 4:
        raise ValueError(f"tile info entry is {len(data)} bytes, not a multiple of 4")
    return list(struct.unpack(f"<{len(data) // 4}I", data))


def tile(v):
    return v & 0x3FFF


def flags(v):
    return v >> 14


def collision(w):
    return w & 0x3F


def check(screens, info):
    bad = [(s, n % SIDE, n // SIDE, tile(v))
           for s, screen in enumerate(screens) for n, v in enumerate(screen) if tile(v) >= len(info)]
    problems = [Problem(f"screen {s} cell ({x},{y})", f"tile {t} is not below the tile count {len(info)}")
                for s, x, y, t in bad[:MAX_LISTED]]
    if len(bad) > MAX_LISTED:
        problems.append(Problem("screens", f"{len(bad) - MAX_LISTED} more cells out of range"))
    return problems
```

- [ ] **Step 4: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 111 tests`, `OK (skipped=2)`

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/stagemap.py tests/test_stagemap.py
git commit -m "Decode stage screens and tile info and check tile numbers

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: `collision` command

**Files:**
- Create: `mmxdemake/collision.py`
- Test: `tests/test_collision.py`

**Interfaces:**
- Consumes: `arcentries.split/only/entry_data/ArcError`, `stagemap.*`, `index.load_index`, `index.build_index/write_index` (tests), `rockarc.observe_header` (tests), `png.write_png`, `outdir.check/fresh_tmp/publish`, `pngread.read_png` (tests), `synth.build_member` (tests).
- Produces:
  - `collision.COLOURS: list[tuple[int, int, int]]` (64 entries)
  - `collision.EMPTY_RGB`, `collision.BACKGROUND_RGB`, `collision.GAP_RGB`
  - `collision.REPO_ROOT: Path`
  - `collision.stage_records(records: list[dict]) -> list[dict]`
  - `collision.run(extracted: Path, out: Path, member: int | None = None, expect_stages: int = 23, log=print) -> int`
  - `collision.main(argv: list[str] | None = None) -> int`

- [ ] **Step 1: Write the failing tests `tests/test_collision.py`**

```python
import hashlib
import json
import os
import struct
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import synth
from mmxdemake import collision
from mmxdemake.collision import BACKGROUND_RGB, COLOURS, EMPTY_RGB, GAP_RGB, main, run
from mmxdemake.index import build_index, write_index
from mmxdemake.rockarc import observe_header
from pngread import read_png

INFO = [0x00, 0x38, 0x00, 0x39]


def stage_member(cell_values=None):
    screen0 = [0] * 256
    screen0[1], screen0[2], screen0[3] = 1, 2, 0x4003
    if cell_values:
        for k, v in cell_values.items():
            screen0[k] = v
    screens = struct.pack("<512H", *(screen0 + [1] * 256))
    return synth.build_member([(0x10102, b"x" * 100), (0, screens), (1, struct.pack("<4I", *INFO))])


def write_extracted(folder, members, observed=True):
    (folder / "rock_dat").mkdir(parents=True, exist_ok=True)
    records = []
    for i, data in enumerate(members):
        rel = f"rock_dat/{i:03d}.bin"
        (folder / rel).write_bytes(data)
        record = {"kind": "rock_dat", "path": rel, "index": i, "sector": 0, "size": len(data),
                  "sha1": hashlib.sha1(data).hexdigest()}
        if observed:
            record["observed"] = observe_header(data, True)
        records.append(record)
    write_index(folder / "index.json", build_index({"serial": "S", "revision": "r", "bin_sha1": "0" * 40}, records))


def pixel(rows, x, y):
    return tuple(rows[y][x * 4:x * 4 + 3])


class CollisionTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.extracted = self.dir / "extracted"
        write_extracted(self.extracted, [b"\0" * 4096, stage_member(), stage_member()])
        self.lines = []

    def tearDown(self):
        self.tmp.cleanup()

    def go(self, out_name="collision", **kw):
        self.lines = []
        kw.setdefault("expect_stages", 2)
        return run(self.extracted, self.dir / out_name, log=self.lines.append, **kw)

    def test_default_run(self):
        self.assertEqual(self.go(), 0, self.lines)
        out = self.dir / "collision"
        self.assertEqual(sorted(p.name for p in out.iterdir()), ["legend.png", "m001", "m002", "summary.txt"])
        self.assertEqual(sorted(p.name for p in (out / "m001").iterdir()), ["screens.json", "sheet.png", "summary.txt"])
        width, height, rows = read_png(out / "legend.png")
        self.assertEqual((width, height), (128, 128))
        self.assertEqual(pixel(rows, 16, 0), COLOURS[1])
        self.assertEqual(pixel(rows, 0, 0), BACKGROUND_RGB)
        self.assertEqual((out / "summary.txt").read_text().splitlines()[-1], "done")
        self.assertFalse((self.dir / "collision.tmp").exists())

    def test_screens_json_cell(self):
        self.go()
        doc = json.loads((self.dir / "collision/m001/screens.json").read_text())
        s0 = doc["screens"][0]
        self.assertEqual((s0["tile"][0][3], s0["flags"][0][3], s0["collision"][0][3]), (3, 1, 57))
        self.assertEqual((s0["tile"][0][1], s0["collision"][0][1]), (1, 56))
        self.assertEqual(doc["tile_count"], 4)
        self.assertEqual(doc["tile_info"], INFO)
        self.assertEqual(doc["source"]["member"], 1)
        self.assertEqual(doc["source"]["screens_entry"], {"offset": 0x1000, "size": 1024})
        self.assertEqual(doc["source"]["tile_info_entry"], {"offset": 0x1800, "size": 16})
        self.assertEqual(len(doc["screens"]), 2)

    def test_sheet_pixels(self):
        self.go()
        width, height, rows = read_png(self.dir / "collision/m001/sheet.png")
        self.assertEqual((width, height), (134, 68))
        self.assertEqual(pixel(rows, 0, 0), GAP_RGB)
        self.assertEqual(pixel(rows, 2, 2), EMPTY_RGB)
        self.assertEqual(pixel(rows, 6, 2), COLOURS[56])
        self.assertEqual(pixel(rows, 10, 2), BACKGROUND_RGB)
        self.assertEqual(pixel(rows, 14, 5), COLOURS[57])
        self.assertEqual(pixel(rows, 68, 2), COLOURS[56])

    def test_summary(self):
        self.go()
        lines = (self.dir / "collision/m001/summary.txt").read_text().splitlines()
        self.assertEqual(lines[0], "member 1: 2 screens, 4 tiles, max tile 3, empty cells 49%")
        self.assertEqual(lines[1], "collision 56: 257 cells #%02x%02x%02x" % COLOURS[56])
        self.assertEqual(lines[2], "collision 0: 254 cells #282840")
        self.assertEqual(lines[3], "collision 57: 1 cells #%02x%02x%02x" % COLOURS[57])
        self.assertIn(lines[0], self.lines)

    def test_changed_member_fails(self):
        p = self.extracted / "rock_dat/001.bin"
        p.write_bytes(p.read_bytes()[:-1] + b"\x01")
        self.assertEqual(self.go(), 1)
        self.assertTrue(any(line.startswith("error: member 1: size") for line in self.lines))
        self.assertFalse((self.dir / "collision").exists())

    def test_tile_out_of_range(self):
        write_extracted(self.extracted, [b"\0" * 4096, stage_member({5: 7}), stage_member()])
        self.assertEqual(self.go(), 1)
        self.assertIn("error: member 1: screen 0 cell (5,0): tile 7 is not below the tile count 4", self.lines)

    def test_wrong_stage_count(self):
        self.assertEqual(self.go(expect_stages=3), 1)
        self.assertIn("error: found 2 stage members, expected 3", self.lines)

    def test_records_without_observed(self):
        write_extracted(self.extracted, [b"\0" * 4096, stage_member(), stage_member()], observed=False)
        self.assertEqual(self.go(), 1)
        self.assertIn("error: found 0 stage members, expected 2", self.lines)

    def test_member_not_stage(self):
        self.assertEqual(self.go(member=0), 1)
        self.assertIn("error: member 0 is not a stage member (needs entry types 0 and 1)", self.lines)

    def test_single_member(self):
        self.assertEqual(self.go(member=2), 0, self.lines)
        self.assertEqual(sorted(p.name for p in (self.dir / "collision").iterdir()),
                         ["legend.png", "m002", "summary.txt"])

    def test_rerun_replaces(self):
        self.assertEqual(self.go(), 0)
        self.assertEqual(self.go(member=2), 0, self.lines)
        self.assertFalse((self.dir / "collision/m001").exists())

    def test_out_inside_repo_refused(self):
        inside = collision.REPO_ROOT / "collision-test-should-not-exist"
        self.assertEqual(run(self.extracted, inside, expect_stages=2, log=self.lines.append), 2)
        self.assertFalse(inside.exists())

    def test_missing_index(self):
        (self.extracted / "index.json").unlink()
        self.assertEqual(self.go(), 2)
        self.assertTrue(any(line.startswith("error:") and "index.json" in line for line in self.lines))

    def test_cli_expands_home(self):
        with mock.patch.dict(os.environ, {"HOME": str(self.dir)}), mock.patch("builtins.print") as printed:
            code = main(["--extracted", "~/extracted", "--out", "~/cli-collision", "--expect-stages", "2"])
        self.assertEqual(code, 0)
        self.assertTrue((self.dir / "cli-collision/summary.txt").exists())
        self.assertTrue(printed.called)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_collision.py 2>&1 | grep Error`
Expected: `ImportError: cannot import name 'collision' from 'mmxdemake'`

- [ ] **Step 3: Write `mmxdemake/collision.py`**

```python
import argparse
import colorsys
import hashlib
import json
import re
import sys
from collections import Counter
from math import ceil
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.arcentries import ArcError, entry_data, only, split
from mmxdemake.index import load_index
from mmxdemake.png import write_png
from mmxdemake.stagemap import SIDE, check, collision, decode_screens, decode_tile_info, flags, tile

REPO_ROOT = Path(__file__).resolve().parents[1]
SCHEMA_VERSION = 1
EXPECT_STAGES = 23
SCREENS_PER_ROW = 16
CELL = 4
GAP = 2
LEGEND_SQUARE = 16
GAP_RGB = (60, 60, 60)
EMPTY_RGB = (16, 16, 16)
BACKGROUND_RGB = (40, 40, 64)


def _colour(value):
    r, g, b = colorsys.hsv_to_rgb((value * 0.618034) % 1, 0.75, 0.95)
    return round(r * 255), round(g * 255), round(b * 255)


COLOURS = [BACKGROUND_RGB] + [_colour(v) for v in range(1, 64)]


class StageFailed(Exception):
    pass


def _allowed(name):
    return name in ("summary.txt", "legend.png") or re.fullmatch(r"m\d{3}", name) is not None


def stage_records(records):
    return [r for r in records
            if {0, 1} <= {t for t, _ in r.get("observed", {}).get("subentries", [])}]


def _cell_rgb(v, info):
    if tile(v) == 0:
        return EMPTY_RGB
    return COLOURS[collision(info[tile(v)])]


def _sheet(screens, info):
    step = SIDE * CELL + GAP
    width = min(SCREENS_PER_ROW, len(screens)) * step + GAP
    height = ceil(len(screens) / SCREENS_PER_ROW) * step + GAP
    rows = [bytearray(bytes(GAP_RGB + (255,)) * width) for _ in range(height)]
    for n, screen in enumerate(screens):
        ox = (n % SCREENS_PER_ROW) * step + GAP
        oy = (n // SCREENS_PER_ROW) * step + GAP
        for k, v in enumerate(screen):
            block = bytes(_cell_rgb(v, info) + (255,)) * CELL
            x0 = (ox + (k % SIDE) * CELL) * 4
            y0 = oy + (k // SIDE) * CELL
            for dy in range(CELL):
                rows[y0 + dy][x0:x0 + CELL * 4] = block
    return width, height, [bytes(r) for r in rows]


def _legend():
    rows = []
    for r in range(8):
        row = b"".join(bytes(COLOURS[8 * r + c] + (255,)) * LEGEND_SQUARE for c in range(8))
        rows += [row] * LEGEND_SQUARE
    return 8 * LEGEND_SQUARE, 8 * LEGEND_SQUARE, rows


def _grid(screen, fn):
    return [[fn(screen[y * SIDE + x]) for x in range(SIDE)] for y in range(SIDE)]


def _process(extracted, tmp, r, log):
    i = r["index"]
    try:
        body = (extracted / r["path"]).read_bytes()
        sha1 = hashlib.sha1(body).hexdigest()
        if len(body) != r["size"] or sha1 != r["sha1"]:
            raise StageFailed(f"member {i}: size {len(body)}, sha1 {sha1} do not match the index "
                              f"({r['size']}, {r['sha1']})")
        entries = split(body)
        screens_entry, info_entry = only(entries, 0), only(entries, 1)
        screens = decode_screens(entry_data(body, screens_entry))
        info = decode_tile_info(entry_data(body, info_entry))
    except (OSError, ArcError, ValueError) as e:
        raise StageFailed(f"member {i}: {e}") from e
    if not screens:
        raise StageFailed(f"member {i}: screens entry is empty")
    problems = check(screens, info)
    if problems:
        raise StageFailed(f"member {i}: " + "; ".join(str(p) for p in problems))
    folder = tmp / f"m{i:03d}"
    folder.mkdir()
    doc = {
        "schema_version": SCHEMA_VERSION,
        "source": {"member": i, "sha1": r["sha1"],
                   "screens_entry": {"offset": screens_entry.offset, "size": screens_entry.size},
                   "tile_info_entry": {"offset": info_entry.offset, "size": info_entry.size}},
        "tile_count": len(info),
        "tile_info": info,
        "screens": [{"index": n, "tile": _grid(s, tile), "flags": _grid(s, flags),
                     "collision": _grid(s, lambda v: collision(info[tile(v)]))}
                    for n, s in enumerate(screens)],
    }
    (folder / "screens.json").write_text(json.dumps(doc, sort_keys=True, indent=2) + "\n")
    write_png(folder / "sheet.png", *_sheet(screens, info))
    cells = [v for s in screens for v in s]
    empty = sum(1 for v in cells if tile(v) == 0)
    header = (f"member {i}: {len(screens)} screens, {len(info)} tiles, "
              f"max tile {max(tile(v) for v in cells)}, empty cells {empty / len(cells):.0%}")
    counts = Counter(collision(info[tile(v)]) for v in cells)
    lines = [header] + [f"collision {c}: {n} cells #%02x%02x%02x" % COLOURS[c]
                        for c, n in sorted(counts.items(), key=lambda kv: (-kv[1], kv[0]))]
    (folder / "summary.txt").write_text("\n".join(lines) + "\n")
    log(header)
    return header


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
    stages = stage_records(records)
    try:
        if member is None:
            if len(stages) != expect_stages:
                raise StageFailed(f"found {len(stages)} stage members, expected {expect_stages}")
            chosen = stages
        else:
            chosen = [r for r in stages if r["index"] == member]
            if not chosen:
                raise StageFailed(f"member {member} is not a stage member (needs entry types 0 and 1)")
        tmp = outdir.fresh_tmp(out)
        headers = [_process(extracted, tmp, r, log) for r in chosen]
        write_png(tmp / "legend.png", *_legend())
        (tmp / "summary.txt").write_text("\n".join(headers + ["done"]) + "\n")
        outdir.publish(tmp, out)
    except (StageFailed, OSError) as e:
        log(f"error: {e}")
        return 1
    log(f"done: {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.collision")
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
Expected: `Ran 125 tests`, `OK (skipped=2)`

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/collision.py tests/test_collision.py
git commit -m "Add the collision command for stage screens

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: Real-data test, real run, and docs

**Files:**
- Create: `tests/test_retail_collision.py`
- Modify: `README.md`, `docs/process/process-log.md`

**Interfaces:**
- Consumes: `collision.run`

- [ ] **Step 1: Write `tests/test_retail_collision.py`**

```python
import json
import os
import tempfile
import unittest
from pathlib import Path

from mmxdemake.collision import run

EXTRACTED = os.environ.get("MMX6_EXTRACTED")
WORK = Path(os.environ.get("MMX6_WORK", "~/mmx-data")).expanduser()


@unittest.skipUnless(EXTRACTED, "set MMX6_EXTRACTED to the task A output folder to run")
class RetailCollisionTest(unittest.TestCase):
    def test_twenty_three_stage_members(self):
        with tempfile.TemporaryDirectory(dir=WORK) as t:
            out = Path(t) / "collision"
            lines = []
            self.assertEqual(run(Path(EXTRACTED).expanduser(), out, log=lines.append), 0, lines)
            folders = sorted(p for p in out.iterdir() if p.is_dir())
            self.assertEqual(len(folders), 23)
            for folder in folders:
                doc = json.loads((folder / "screens.json").read_text())
                header = (folder / "summary.txt").read_text().splitlines()[0]
                self.assertIn(f": {len(doc['screens'])} screens,", header)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the suite, then the real-data test**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 126 tests`, `OK (skipped=3)`

Run: `MMX6_EXTRACTED=~/mmx-data/extracted/retail python3 -m unittest tests.test_retail_collision 2>&1 | grep -E "^(Ran|OK|FAILED)|Error"`
Expected: `Ran 1 test`, `OK`. If it fails, stop and find the cause before changing anything; do not loosen a check to make it pass.

- [ ] **Step 3: Do the real run**

Run: `python3 -m mmxdemake.collision --extracted ~/mmx-data/extracted/retail --out ~/mmx-data/collision`
Expected: 23 `member …` lines, then `done: /home/johnlavender/mmx-data/collision`, exit 0.

- [ ] **Step 4: Look at the sheets before reporting**

Build a contact view of a few `mNNN/sheet.png` files with a throwaway script in the scratchpad (Pillow is fine there) and look at it. Record in the process log whether the screens look like stage pieces (floors, walls, shafts, slopes) or like static.

- [ ] **Step 5: Append to `README.md`**

````markdown

Decode stage screens and collision for the 23 stage members:

```
python3 -m mmxdemake.collision \
  --extracted ~/mmx-data/extracted/retail \
  --out ~/mmx-data/collision
```

Each `mNNN/` folder holds `screens.json` (tile, flags and collision per cell),
`sheet.png` (all screens coloured by collision value) and `summary.txt`.
`legend.png` shows the 64 collision colours (row × 8 + column = value).
````

- [ ] **Step 6: Append to `docs/process/process-log.md`**

Add a section before "Lessons so far", titled `## 22. Build the collision command (task D)`, with: what was built, the test counts, the 23 header lines (or a short table), what the sheets look like from Step 4, the commit hashes, and that the user still has to look at the sheets.

- [ ] **Step 7: Check that no game data is in the repo, then commit**

Run: `git status --short`
Expected: only `README.md`, `docs/process/process-log.md`, `tests/test_retail_collision.py`.

```bash
git add README.md docs/process/process-log.md tests/test_retail_collision.py
git commit -m "Add the real-data collision test, usage notes and the process log entry

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```
