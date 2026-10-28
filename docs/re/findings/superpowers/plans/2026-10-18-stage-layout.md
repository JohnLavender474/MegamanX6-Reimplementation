# Stage Layout Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Turn a PCSXR save state into a verified stage entry in `catalog/stages.json` (and the state into `catalog/savestates.json`), then draw each entry from the disc data as `layout.json`, `stage.png` and `collision.png` in `~/mmx-data/stages/`.

**Architecture:** `savestate` reads RAM from a gzip save state. `ramlayout` holds pure functions: size fields, member detection by full tile info, the layout at the fixed RAM address, the edge check, the disc search, and the differing cells. `learn` and `stages` are CLIs that join them with the existing modules.

**Tech Stack:** Python 3.10 standard library only.

**Spec:** `docs/superpowers/specs/2026-10-18-stage-layout-design.md`

## Global Constraints

- Python 3.10, standard library only. Tests: `python3 -m unittest discover -s tests` from the repo root. No `-I`.
- Save state: gzip; header starts `STv4 PCSXR`; RAM = decompressed `[0x9025, 0x9025 + 0x200000)`; check RAM `[0x11000, 0x11400)` == program file `[0x1800, 0x1C00)`.
- RAM addresses: width `0x800cd338` (u8), height `0x800cd339` (u8), size `0x8008ec10` (u16 LE), layout `0x8009741e`. `ram_at` uses the low 21 bits.
- Layout: 3 layers × width × height bytes, layer after layer, row by row. Row 0 of layer 0 is skipped in checks and disc search.
- Edge check: layers 0 and 1, skip row 0 of layer 0; "live" screens have a solid mask neither all 0 nor all 1; refuse when `h_joins < 10`, `h_score < 0.85`, or `v_joins >= 3 and v_score < 0.85`.
- Member detection: the whole tile info entry must occur in RAM for exactly one screen set (`collision.map_records`).
- Disc search: `layout[w:]` in every `rock_bin/NN.bin`; exactly one hit; offset = hit − w.
- Catalog files: lists, `sort_keys=True`, indent 2, trailing `\n`. `savestates.json` sorted by `name`; `stages.json` sorted by `(member, name)`.
- Stage folders: `mNNN-<slug>`, slug = lower-case name with runs of non `[a-z0-9]` replaced by `-`, stripped of `-`.
- Exit codes: 0 ok; 1 failed check or `OSError` during work; 2 bad arguments, missing save state, `--out` refused, unreadable index or catalog.
- No game data in the repo: the catalog holds names, hashes and numbers only. No code comments. No code copied from reference repos.
- Commit to `master`. End each commit message with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. The real intro state passes the 0.85 edge check with margin (spec: 0.929 / 0.944). Checked by the real-data test in Task 5.
2. A save state from a different game or a corrupt file → exit 1 with a clear `error:` line, never a traceback. Tests in Tasks 1 and 3.
3. Re-running `learn` with the same name replaces the entry (no duplicates); a new name adds one. Test in Task 3.
4. A manifest entry whose name matches but whose SHA-1 differs → refuse, so a renamed or replaced file cannot silently take over an entry. Test in Task 3.
5. `stages` never writes into the repo, and refuses layout cells that are not screen numbers when the catalog's differing cells are missing. Tests in Task 4.

---

## File structure

```
catalog/                    # created by learn in Task 5
mmxdemake/
  savestate.py   ramlayout.py   learn.py   stages.py      # new
tests/
  synth.py                  # modified: fake_exe, fake_ram, build_state
  stagefix.py               # new: shared fake stage
  test_savestate.py  test_ramlayout.py  test_learn.py  test_stages.py  test_retail_stages.py
```

---

### Task 1: `savestate`

**Files:**
- Create: `mmxdemake/savestate.py`
- Modify: `tests/synth.py` (add `import gzip` at the top; append helpers)
- Test: `tests/test_savestate.py`

**Interfaces:**
- Produces: `savestate.RAM_OFFSET`, `RAM_SIZE`, `HEADER`, `SaveStateError`, `read_ram(path, exe) -> bytes`, `ram_at(ram, addr, n) -> bytes`; `synth.fake_exe() -> bytes`, `synth.fake_ram(exe, width, height, layout, tile_info, tile_info_at=0x1B0000) -> bytes`, `synth.build_state(ram, header=b"STv4 PCSXR v1.9.94") -> bytes` (gzip bytes)

- [ ] **Step 1: Add the helpers to `tests/synth.py`**

Add `import gzip` to the imports at the top. Append:

```python


def fake_exe():
    return bytes((k * 7 + 3) % 256 for k in range(0x2000))


def fake_ram(exe, width, height, layout, tile_info, tile_info_at=0x1B0000):
    ram = bytearray(0x200000)
    ram[0x11000:0x11400] = exe[0x1800:0x1C00]
    ram[0xCD338] = width
    ram[0xCD339] = height
    struct.pack_into("<H", ram, 0x8EC10, width * height)
    ram[0x9741E:0x9741E + len(layout)] = layout
    ram[tile_info_at:tile_info_at + len(tile_info)] = tile_info
    return bytes(ram)


def build_state(ram, header=b"STv4 PCSXR v1.9.94"):
    return gzip.compress(header.ljust(32, b"\0") + b"\0" * (0x9025 - 32) + ram + b"\0" * 64)
```

- [ ] **Step 2: Write the failing tests `tests/test_savestate.py`**

```python
import gzip
import tempfile
import unittest
from pathlib import Path

import synth
from mmxdemake.savestate import SaveStateError, ram_at, read_ram


class ReadRamTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.path = Path(self.tmp.name) / "s.state.gz"
        self.exe = synth.fake_exe()
        self.ram = synth.fake_ram(self.exe, 3, 4, b"\x01\x02", b"\x09")

    def tearDown(self):
        self.tmp.cleanup()

    def test_reads_back(self):
        self.path.write_bytes(synth.build_state(self.ram))
        self.assertEqual(read_ram(self.path, self.exe), self.ram)

    def test_not_gzip(self):
        self.path.write_bytes(b"hello")
        with self.assertRaisesRegex(SaveStateError, "not a gzip save state"):
            read_ram(self.path, self.exe)

    def test_wrong_header(self):
        self.path.write_bytes(synth.build_state(self.ram, header=b"XXXX"))
        with self.assertRaisesRegex(SaveStateError, "is not a PCSXR save state"):
            read_ram(self.path, self.exe)

    def test_too_short(self):
        self.path.write_bytes(gzip.compress(b"STv4 PCSXR" + b"\0" * 100))
        with self.assertRaisesRegex(SaveStateError, "too short"):
            read_ram(self.path, self.exe)

    def test_program_mismatch(self):
        self.path.write_bytes(synth.build_state(self.ram))
        with self.assertRaisesRegex(SaveStateError, "does not hold the X6 program text"):
            read_ram(self.path, bytes(0x2000))


class RamAtTest(unittest.TestCase):
    def test_ram_at(self):
        ram = bytearray(0x200000)
        ram[0xCD338] = 26
        self.assertEqual(ram_at(bytes(ram), 0x800CD338, 1), b"\x1a")
        self.assertEqual(ram_at(bytes(ram), 0xCD338, 2), b"\x1a\x00")


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 3: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_savestate.py 2>&1 | grep Error`
Expected: `ModuleNotFoundError: No module named 'mmxdemake.savestate'`

- [ ] **Step 4: Write `mmxdemake/savestate.py`**

```python
import gzip
from pathlib import Path

RAM_OFFSET = 0x9025
RAM_SIZE = 0x200000
HEADER = b"STv4 PCSXR"
TEXT_RAM = 0x11000
TEXT_EXE = 0x1800
TEXT_LEN = 0x400


class SaveStateError(Exception):
    pass


def read_ram(path, exe):
    path = Path(path)
    raw = path.read_bytes()
    try:
        data = gzip.decompress(raw)
    except (OSError, EOFError) as e:
        raise SaveStateError(f"{path}: not a gzip save state ({e})") from e
    if not data.startswith(HEADER):
        raise SaveStateError(f"{path}: header {data[:16]!r} is not a PCSXR save state")
    if len(data) < RAM_OFFSET + RAM_SIZE:
        raise SaveStateError(f"{path}: {len(data)} bytes, too short for 2 MB of RAM at {RAM_OFFSET:#x}")
    ram = data[RAM_OFFSET:RAM_OFFSET + RAM_SIZE]
    if ram[TEXT_RAM:TEXT_RAM + TEXT_LEN] != exe[TEXT_EXE:TEXT_EXE + TEXT_LEN]:
        raise SaveStateError(f"{path}: RAM does not hold the X6 program text at {TEXT_RAM:#x}")
    return ram


def ram_at(ram, addr, n):
    offset = addr & 0x1FFFFF
    return ram[offset:offset + n]
```

- [ ] **Step 5: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 156 tests`, `OK (skipped=4)`

- [ ] **Step 6: Commit**

```bash
git add mmxdemake/savestate.py tests/synth.py tests/test_savestate.py
git commit -m "Read RAM from PCSXR save states

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: `ramlayout` and the shared fake stage

**Files:**
- Create: `mmxdemake/ramlayout.py`, `tests/stagefix.py`
- Test: `tests/test_ramlayout.py`

**Interfaces:**
- Consumes: `savestate.ram_at`, `stagemap.tile`, `stagemap.collision`, `stagemap.SIDE`
- Produces: `ramlayout.LAYOUT_RAM`, `LayoutError`, `read_size(ram) -> (int, int)`, `find_member(ram, tile_infos: dict[int, bytes]) -> int`, `read_layout(ram, w, h) -> bytes`, `solid_masks(screens, info) -> list[list[list[int]]]`, `edge_score(layout, w, h, masks) -> (float, int, float, int)`, `check_layout(layout, w, h, masks, screen_count) -> (float, int, float, int)`, `find_on_disc(layout, w, overlays: dict[int, bytes]) -> (int, int)`, `differing_cells(disc, ram_layout, w, h) -> list[tuple[int, int, int]]`; `stagefix.W`, `H`, `INFO`, `SCREENS`, `stage_member()`, `other_member()`, `ram_layout()`, `disc_layout()`, `overlay_bytes()`, `write_extracted(folder, members)`, `write_state(path, layout=None)`

- [ ] **Step 1: Write `tests/stagefix.py`**

```python
import hashlib
import struct

import synth
from mmxdemake.index import build_index, write_index
from mmxdemake.rockarc import observe_header

W, H = 12, 2
OVERLAY = 3
OFFSET = 100
INFO = [0, 0x38]
SCREENS = [[0] * 256,
           [1 if y >= 8 else 0 for y in range(16) for x in range(16)],
           [1 if x < 8 else 0 for y in range(16) for x in range(16)]]


def texture():
    block = bytearray(0x40000)
    block[0:8] = bytes([0x11] * 8)
    return bytes(block)


def pack_info(info):
    return struct.pack(f"<{len(info)}I", *info)


def stage_member():
    screens = struct.pack(f"<{len(SCREENS) * 256}H", *[v for s in SCREENS for v in s])
    return synth.build_member([(0x10102, b"x" * 100), (0, screens), (1, pack_info(INFO)), (0x10000, texture())])


def other_member():
    return synth.build_member([(0, struct.pack("<256H", *([0] * 256))), (1, pack_info([0, 0x39, 0x3A]))])


def ram_layout():
    return bytes([0] * W + [1] * W + [0] * (W * H) + [2] * (W * H))


def disc_layout():
    layout = bytearray(ram_layout())
    layout[0:W] = b"\xff" * W
    return bytes(layout)


def overlay_bytes():
    return b"\xee" * OFFSET + disc_layout() + b"\xee" * 50


def write_extracted(folder, members):
    for sub in ("rock_dat", "rock_bin", "iso"):
        (folder / sub).mkdir(parents=True, exist_ok=True)
    (folder / "iso/SLUS_013.95").write_bytes(synth.fake_exe())
    for i in range(6):
        (folder / f"rock_bin/{i:02d}.bin").write_bytes(overlay_bytes() if i == OVERLAY else b"\xdd" * 64)
    records = []
    for i, data in enumerate(members):
        rel = f"rock_dat/{i:03d}.bin"
        (folder / rel).write_bytes(data)
        records.append({"kind": "rock_dat", "path": rel, "index": i, "sector": 0, "size": len(data),
                        "sha1": hashlib.sha1(data).hexdigest(), "observed": observe_header(data, True)})
    write_index(folder / "index.json", build_index({"serial": "S", "revision": "r", "bin_sha1": "0" * 40}, records))


def write_state(path, layout=None):
    ram = synth.fake_ram(synth.fake_exe(), W, H, ram_layout() if layout is None else layout, pack_info(INFO))
    path.write_bytes(synth.build_state(ram))
```

- [ ] **Step 2: Write the failing tests `tests/test_ramlayout.py`**

```python
import struct
import unittest

import stagefix
import synth
from mmxdemake.ramlayout import (
    LayoutError, check_layout, differing_cells, find_member, find_on_disc, read_layout, read_size, solid_masks,
)
from stagefix import H, INFO, SCREENS, W


def make_ram(width=W, height=H, size=None):
    ram = bytearray(synth.fake_ram(synth.fake_exe(), width, height, stagefix.ram_layout(), stagefix.pack_info(INFO)))
    if size is not None:
        struct.pack_into("<H", ram, 0x8EC10, size)
    return bytes(ram)


class SizeTest(unittest.TestCase):
    def test_read_size(self):
        self.assertEqual(read_size(make_ram()), (12, 2))

    def test_size_fields_disagree(self):
        with self.assertRaisesRegex(LayoutError, "width 12, height 2, size 5 do not agree"):
            read_size(make_ram(size=5))
        with self.assertRaisesRegex(LayoutError, "do not agree"):
            read_size(make_ram(width=0))


class MemberTest(unittest.TestCase):
    def test_find_member(self):
        ram = make_ram()
        other = stagefix.pack_info([0, 0x39, 0x3A])
        self.assertEqual(find_member(ram, {1: stagefix.pack_info(INFO), 2: other}), 1)
        with self.assertRaisesRegex(LayoutError, "0 stage members"):
            find_member(ram, {2: other})
        with self.assertRaisesRegex(LayoutError, r"2 stage members have their tile info in RAM \(\[1, 5\]\)"):
            find_member(ram, {1: stagefix.pack_info(INFO), 5: stagefix.pack_info(INFO)})


class LayoutTest(unittest.TestCase):
    def setUp(self):
        self.masks = solid_masks(SCREENS, INFO)

    def test_solid_masks(self):
        self.assertEqual((self.masks[1][8][0], self.masks[1][7][0]), (1, 0))
        self.assertEqual((self.masks[2][0][7], self.masks[2][0][8]), (1, 0))

    def test_read_layout(self):
        self.assertEqual(read_layout(make_ram(), W, H), stagefix.ram_layout())

    def test_check_passes(self):
        self.assertEqual(check_layout(stagefix.ram_layout(), W, H, self.masks, 3), (1.0, 11, 0.0, 0))
        self.assertEqual(check_layout(stagefix.disc_layout(), W, H, self.masks, 3), (1.0, 11, 0.0, 0))

    def test_check_refuses_bad_joins(self):
        bad = bytearray(stagefix.ram_layout())
        bad[W:2 * W] = bytes([1, 2] * 6)
        with self.assertRaisesRegex(LayoutError, "screens do not join: horizontal 0.500 over 11 joins"):
            check_layout(bytes(bad), W, H, self.masks, 3)

    def test_check_refuses_out_of_range(self):
        bad = bytearray(stagefix.ram_layout())
        bad[W] = 9
        with self.assertRaisesRegex(LayoutError, "1 layout cells are not screen numbers"):
            check_layout(bytes(bad), W, H, self.masks, 3)

    def test_find_on_disc(self):
        overlay = stagefix.overlay_bytes()
        self.assertEqual(find_on_disc(stagefix.ram_layout(), W, {0: b"\0" * 10, 3: overlay}), (3, 100))
        with self.assertRaisesRegex(LayoutError, "found 0 times"):
            find_on_disc(stagefix.ram_layout(), W, {0: b"\0" * 10})
        with self.assertRaisesRegex(LayoutError, "found 2 times"):
            find_on_disc(stagefix.ram_layout(), W, {3: overlay, 4: overlay})

    def test_differing_cells(self):
        self.assertEqual(differing_cells(stagefix.disc_layout(), stagefix.ram_layout(), W, H),
                         [(0, x, 0) for x in range(W)])


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 3: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_ramlayout.py 2>&1 | grep Error`
Expected: `ModuleNotFoundError: No module named 'mmxdemake.ramlayout'`

- [ ] **Step 4: Write `mmxdemake/ramlayout.py`**

```python
import struct

from mmxdemake.savestate import ram_at
from mmxdemake.stagemap import SIDE, collision, tile

LAYOUT_WIDTH = 0x800CD338
LAYOUT_HEIGHT = 0x800CD339
LAYOUT_SIZE = 0x8008EC10
LAYOUT_RAM = 0x8009741E
LAYERS = 3
THRESHOLD = 0.85
MIN_H_JOINS = 10
MIN_V_JOINS = 3


class LayoutError(Exception):
    pass


def read_size(ram):
    w = ram_at(ram, LAYOUT_WIDTH, 1)[0]
    h = ram_at(ram, LAYOUT_HEIGHT, 1)[0]
    size = struct.unpack("<H", ram_at(ram, LAYOUT_SIZE, 2))[0]
    if not w or not h or size != w * h:
        raise LayoutError(f"layout size fields width {w}, height {h}, size {size} do not agree")
    return w, h


def find_member(ram, tile_infos):
    found = sorted(m for m, info in tile_infos.items() if info and ram.find(info) >= 0)
    if len(found) != 1:
        raise LayoutError(f"{len(found)} stage members have their tile info in RAM ({found}); need exactly 1")
    return found[0]


def read_layout(ram, w, h):
    return ram_at(ram, LAYOUT_RAM, LAYERS * w * h)


def solid_masks(screens, info):
    return [[[1 if collision(info[tile(s[y * SIDE + x])]) else 0 for x in range(SIDE)] for y in range(SIDE)]
            for s in screens]


def _edges(mask):
    return (tuple(row[-1] for row in mask), tuple(row[0] for row in mask), tuple(mask[-1]), tuple(mask[0]))


def _match(a, b):
    return sum(1 for p, q in zip(a, b) if p == q) / len(a)


def edge_score(layout, w, h, masks):
    live = {s for s, m in enumerate(masks) if any(map(any, m)) and not all(map(all, m))}
    edges = {s: _edges(masks[s]) for s in live}
    hs = vs = 0.0
    hn = vn = 0
    for layer in (0, 1):
        base = layer * w * h
        for y in range(h):
            if layer == 0 and y == 0:
                continue
            for x in range(w):
                a = layout[base + y * w + x]
                if a not in live:
                    continue
                if x + 1 < w:
                    b = layout[base + y * w + x + 1]
                    if b in live:
                        hs += _match(edges[a][0], edges[b][1])
                        hn += 1
                if y + 1 < h:
                    b = layout[base + (y + 1) * w + x]
                    if b in live:
                        vs += _match(edges[a][2], edges[b][3])
                        vn += 1
    return (hs / hn if hn else 0.0), hn, (vs / vn if vn else 0.0), vn


def check_layout(layout, w, h, masks, screen_count):
    bad = [k for k, v in enumerate(layout) if k >= w and v >= screen_count]
    if bad:
        k = bad[0]
        raise LayoutError(f"{len(bad)} layout cells are not screen numbers (first: layer {k // (w * h)}, "
                          f"x {(k % (w * h)) % w}, y {(k % (w * h)) // w}, value {layout[k]}); "
                          f"the fixed RAM address {LAYOUT_RAM:#x} may be wrong for this stage")
    hsc, hj, vsc, vj = edge_score(layout, w, h, masks)
    if hj < MIN_H_JOINS or hsc < THRESHOLD or (vj >= MIN_V_JOINS and vsc < THRESHOLD):
        raise LayoutError(f"screens do not join: horizontal {hsc:.3f} over {hj} joins, vertical {vsc:.3f} "
                          f"over {vj} joins; the fixed RAM address {LAYOUT_RAM:#x} may be wrong for this stage")
    return hsc, hj, vsc, vj


def _all_hits(data, needle):
    k = data.find(needle)
    while k >= 0:
        yield k
        k = data.find(needle, k + 1)


def find_on_disc(layout, w, overlays):
    needle = bytes(layout[w:])
    hits = [(i, k - w) for i, data in sorted(overlays.items()) for k in _all_hits(data, needle) if k >= w]
    if len(hits) != 1:
        raise LayoutError(f"layout found {len(hits)} times in the overlays; need exactly 1")
    return hits[0]


def differing_cells(disc, ram_layout, w, h):
    return [(k // (w * h), (k % (w * h)) % w, (k % (w * h)) // w)
            for k in range(len(ram_layout)) if disc[k] != ram_layout[k]]
```

- [ ] **Step 5: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 166 tests`, `OK (skipped=4)`

- [ ] **Step 6: Commit**

```bash
git add mmxdemake/ramlayout.py tests/stagefix.py tests/test_ramlayout.py
git commit -m "Find and check a stage layout in RAM and on disc

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: `learn` command

**Files:**
- Create: `mmxdemake/learn.py`
- Test: `tests/test_learn.py`

**Interfaces:**
- Consumes: `savestate.read_ram/SaveStateError`, `ramlayout.*`, `arcentries.split/only/entry_data/ArcError`, `collision.map_records`, `index.load_index`, `stagemap.decode_screens/decode_tile_info`
- Produces: `learn.REPO_ROOT`, `learn.DEFAULT_CATALOG`, `learn.run(state, name, note="", dumps=..., extracted=..., catalog=DEFAULT_CATALOG, log=print) -> int`, `learn.main(argv=None) -> int`

- [ ] **Step 1: Write the failing tests `tests/test_learn.py`**

```python
import hashlib
import json
import os
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import stagefix
from mmxdemake.learn import main, run

ENTRY = {
    "name": "Stage A", "member": 1, "width": 12, "height": 2, "overlay": 3, "offset": 100,
    "save_state": "a.state.gz", "edge": {"h": 1.0, "h_joins": 11, "v": 0.0, "v_joins": 0},
    "differing_cells": [[0, x, 0] for x in range(12)],
}


class LearnTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.extracted = self.dir / "extracted"
        stagefix.write_extracted(self.extracted, [b"\0" * 4096, stagefix.stage_member(), stagefix.other_member()])
        self.dumps = self.dir / "dumps"
        self.dumps.mkdir()
        stagefix.write_state(self.dumps / "a.state.gz")
        self.sha1 = hashlib.sha1((self.dumps / "a.state.gz").read_bytes()).hexdigest()
        self.catalog = self.dir / "catalog"
        self.lines = []

    def tearDown(self):
        self.tmp.cleanup()

    def go(self, state="a.state.gz", name="Stage A"):
        self.lines = []
        return run(state, name, "n", self.dumps, self.extracted, self.catalog, self.lines.append)

    def stages(self):
        return json.loads((self.catalog / "stages.json").read_text())

    def test_writes_catalog(self):
        self.assertEqual(self.go(), 0, self.lines)
        self.assertEqual(self.lines, [
            "Stage A: member 1, 12x2, overlay 3 @0x64, edges 1.000/0.000, 12 cells differ (layer 0 row 0)"])
        self.assertEqual(self.stages(), [dict(ENTRY, save_state_sha1=self.sha1)])
        self.assertEqual(json.loads((self.catalog / "savestates.json").read_text()),
                         [{"name": "a.state.gz", "sha1": self.sha1, "note": "n"}])
        self.assertTrue((self.catalog / "stages.json").read_text().endswith("]\n"))

    def test_second_run_replaces(self):
        self.go()
        self.go()
        self.assertEqual(len(self.stages()), 1)
        self.assertEqual(self.go(name="Stage B"), 0)
        self.assertEqual([e["name"] for e in self.stages()], ["Stage A", "Stage B"])

    def test_manifest_sha_conflict(self):
        self.catalog.mkdir()
        (self.catalog / "savestates.json").write_text(json.dumps([{"name": "a.state.gz", "sha1": "0" * 40, "note": ""}]))
        self.assertEqual(self.go(), 1)
        self.assertIn(f"error: manifest has a.state.gz with sha1 {'0' * 40}, the file has {self.sha1}", self.lines)

    def test_missing_state(self):
        self.assertEqual(self.go(state="missing.state.gz"), 2)
        self.assertTrue(self.lines[0].startswith("error: save state"))

    def test_bad_layout(self):
        bad = bytearray(stagefix.ram_layout())
        bad[12:24] = bytes([1, 2] * 6)
        stagefix.write_state(self.dumps / "a.state.gz", bytes(bad))
        self.assertEqual(self.go(), 1)
        self.assertTrue(self.lines[0].startswith("error: screens do not join"))
        self.assertFalse((self.catalog / "stages.json").exists())

    def test_cli_expands_home(self):
        with mock.patch.dict(os.environ, {"HOME": str(self.dir)}), mock.patch("builtins.print"):
            code = main(["--state", "a.state.gz", "--name", "Stage A", "--dumps", "~/dumps",
                         "--extracted", "~/extracted", "--catalog", "~/catalog"])
        self.assertEqual(code, 0)
        self.assertTrue((self.catalog / "stages.json").exists())


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_learn.py 2>&1 | grep Error`
Expected: `ModuleNotFoundError: No module named 'mmxdemake.learn'`

- [ ] **Step 3: Write `mmxdemake/learn.py`**

```python
import argparse
import hashlib
import json
import sys
from pathlib import Path

from mmxdemake.arcentries import ArcError, entry_data, only, split
from mmxdemake.collision import map_records
from mmxdemake.index import load_index
from mmxdemake.ramlayout import (
    LayoutError, check_layout, differing_cells, find_member, find_on_disc, read_layout, read_size, solid_masks,
)
from mmxdemake.savestate import SaveStateError, read_ram
from mmxdemake.stagemap import decode_screens, decode_tile_info

REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CATALOG = REPO_ROOT / "catalog"
DEFAULT_DUMPS = Path.home() / "mmx-data/dumps"
DEFAULT_EXTRACTED = Path.home() / "mmx-data/extracted/retail"


class LearnFailed(Exception):
    pass


def _load_list(path):
    return json.loads(path.read_text()) if path.exists() else []


def _write_list(path, items, key):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(sorted(items, key=key), sort_keys=True, indent=2) + "\n")


def _member_body(extracted, r):
    body = (extracted / r["path"]).read_bytes()
    if hashlib.sha1(body).hexdigest() != r["sha1"]:
        raise LearnFailed(f"member {r['index']}: sha1 does not match the index")
    return body


def run(state, name, note="", dumps=DEFAULT_DUMPS, extracted=DEFAULT_EXTRACTED, catalog=DEFAULT_CATALOG, log=print):
    state_path = Path(dumps) / state
    extracted, catalog = Path(extracted), Path(catalog)
    if not state_path.is_file():
        log(f"error: save state {state_path} does not exist")
        return 2
    try:
        records = [r for r in load_index(extracted / "index.json")["files"] if r["kind"] == "rock_dat"]
        manifest = _load_list(catalog / "savestates.json")
        entries = _load_list(catalog / "stages.json")
    except (OSError, ValueError, KeyError, TypeError, AttributeError) as e:
        log(f"error: {e}")
        return 2
    try:
        sha1 = hashlib.sha1(state_path.read_bytes()).hexdigest()
        old = next((m for m in manifest if m["name"] == state), None)
        if old and old["sha1"] != sha1:
            raise LearnFailed(f"manifest has {state} with sha1 {old['sha1']}, the file has {sha1}")
        ram = read_ram(state_path, (extracted / "iso/SLUS_013.95").read_bytes())
        w, h = read_size(ram)
        candidates = {r["index"]: r for r in map_records(records)}
        bodies = {i: _member_body(extracted, r) for i, r in candidates.items()}
        infos = {i: entry_data(b, only(split(b), 1)) for i, b in bodies.items()}
        member = find_member(ram, infos)
        entries_of = split(bodies[member])
        screens = decode_screens(entry_data(bodies[member], only(entries_of, 0)))
        info = decode_tile_info(infos[member])
        layout = read_layout(ram, w, h)
        hsc, hj, vsc, vj = check_layout(layout, w, h, solid_masks(screens, info), len(screens))
        overlays = {int(p.stem): p.read_bytes() for p in sorted((extracted / "rock_bin").glob("*.bin"))}
        overlay, offset = find_on_disc(layout, w, overlays)
        diff = differing_cells(overlays[overlay][offset:offset + len(layout)], layout, w, h)
    except (SaveStateError, LayoutError, ArcError, LearnFailed, ValueError, OSError) as e:
        log(f"error: {e}")
        return 1
    entry = {"name": name, "member": member, "width": w, "height": h, "overlay": overlay, "offset": offset,
             "save_state": state, "save_state_sha1": sha1,
             "edge": {"h": round(hsc, 3), "h_joins": hj, "v": round(vsc, 3), "v_joins": vj},
             "differing_cells": [list(c) for c in diff]}
    manifest = [m for m in manifest if m["name"] != state] + [{"name": state, "sha1": sha1, "note": note}]
    entries = [e for e in entries if e["name"] != name] + [entry]
    try:
        _write_list(catalog / "savestates.json", manifest, key=lambda m: m["name"])
        _write_list(catalog / "stages.json", entries, key=lambda e: (e["member"], e["name"]))
    except OSError as e:
        log(f"error: {e}")
        return 1
    where = " (layer 0 row 0)" if diff and all(l == 0 and y == 0 for l, _, y in diff) else ""
    log(f"{name}: member {member}, {w}x{h}, overlay {overlay} @{offset:#x}, edges {hsc:.3f}/{vsc:.3f}, "
        f"{len(diff)} cells differ{where}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.learn")
    parser.add_argument("--state", required=True)
    parser.add_argument("--name", required=True)
    parser.add_argument("--note", default="")
    parser.add_argument("--dumps", default=str(DEFAULT_DUMPS))
    parser.add_argument("--extracted", default=str(DEFAULT_EXTRACTED))
    parser.add_argument("--catalog", default=str(DEFAULT_CATALOG))
    args = parser.parse_args(argv)
    return run(args.state, args.name, args.note, Path(args.dumps).expanduser(), Path(args.extracted).expanduser(),
               Path(args.catalog).expanduser(), log=print)


if __name__ == "__main__":
    sys.exit(main())
```

- [ ] **Step 4: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 172 tests`, `OK (skipped=4)`

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/learn.py tests/test_learn.py
git commit -m "Add the learn command that turns a save state into a stage entry

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: `stages` command

**Files:**
- Create: `mmxdemake/stages.py`
- Test: `tests/test_stages.py`

**Interfaces:**
- Consumes: `arcentries.*`, `collision.COLOURS/EMPTY_RGB`, `index.load_index`, `png.write_png`, `shade.grey`, `stagemap.SIDE/collision/decode_screens/decode_tile_info/tile`, `texture.BLOCK_SIZE/TILE/bits/raw_tile/tile_ref`, `outdir.*`, `learn.DEFAULT_CATALOG`; tests use `stagefix`, `pngread.read_png`.
- Produces: `stages.REPO_ROOT`, `stages.slug(name) -> str`, `stages.run(extracted, out, catalog=DEFAULT_CATALOG, log=print) -> int`, `stages.main(argv=None) -> int`

- [ ] **Step 1: Write the failing tests `tests/test_stages.py`**

```python
import json
import tempfile
import unittest
from pathlib import Path

import stagefix
from mmxdemake import stages
from mmxdemake.collision import COLOURS, EMPTY_RGB
from mmxdemake.stages import run, slug
from pngread import read_png

ENTRY = {
    "name": "Stage A", "member": 1, "width": 12, "height": 2, "overlay": 3, "offset": 100,
    "save_state": "a.state.gz", "save_state_sha1": "0" * 40,
    "edge": {"h": 1.0, "h_joins": 11, "v": 0.0, "v_joins": 0},
    "differing_cells": [[0, x, 0] for x in range(12)],
}


def pixel(rows, x, y):
    return tuple(rows[y][x * 4:x * 4 + 3])


class StagesTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.extracted = self.dir / "extracted"
        stagefix.write_extracted(self.extracted, [b"\0" * 4096, stagefix.stage_member()])
        self.catalog = self.dir / "catalog"
        self.catalog.mkdir()
        self.write_catalog([ENTRY])
        self.lines = []

    def tearDown(self):
        self.tmp.cleanup()

    def write_catalog(self, entries):
        (self.catalog / "stages.json").write_text(json.dumps(entries))

    def go(self):
        self.lines = []
        return run(self.extracted, self.dir / "stages", self.catalog, self.lines.append)

    def test_slug(self):
        self.assertEqual(slug("Intro, section 1"), "intro-section-1")

    def test_output_files(self):
        self.assertEqual(self.go(), 0, self.lines)
        out = self.dir / "stages"
        self.assertEqual(sorted(p.name for p in out.iterdir()), ["m001-stage-a", "summary.txt"])
        self.assertEqual(sorted(p.name for p in (out / "m001-stage-a").iterdir()),
                         ["collision.png", "layout.json", "stage.png", "summary.txt"])
        self.assertEqual((out / "summary.txt").read_text().splitlines(),
                         ["Stage A: member 1, 12x2, overlay 3 @0x64", "done"])
        layout = json.loads((out / "m001-stage-a/layout.json").read_text())
        self.assertEqual((layout["width"], layout["height"], layout["source"]), (12, 2, {"overlay": 3, "offset": 100}))
        self.assertEqual(layout["layers"][0], [[0] * 12, [1] * 12])
        self.assertEqual(layout["layers"][2], [[2] * 12, [2] * 12])

    def test_stage_pixels(self):
        self.go()
        width, height, rows = read_png(self.dir / "stages/m001-stage-a/stage.png")
        self.assertEqual((width, height), (3072, 512))
        self.assertEqual(pixel(rows, 0, 0), (17, 17, 17))
        self.assertEqual(pixel(rows, 128, 0), (0, 0, 0))
        self.assertEqual(pixel(rows, 128, 384), (17, 17, 17))
        self.assertEqual(pixel(rows, 128, 385), (0, 0, 0))

    def test_collision_pixels(self):
        self.go()
        width, height, rows = read_png(self.dir / "stages/m001-stage-a/collision.png")
        self.assertEqual((width, height), (768, 128))
        self.assertEqual(pixel(rows, 0, 0), EMPTY_RGB)
        self.assertEqual(pixel(rows, 0, 64), EMPTY_RGB)
        self.assertEqual(pixel(rows, 0, 96), COLOURS[0x38])

    def test_layout_cell_out_of_range(self):
        self.write_catalog([dict(ENTRY, differing_cells=[])])
        self.assertEqual(self.go(), 1)
        self.assertTrue(self.lines[0].startswith("error: Stage A: 12 layout cells are not screen numbers"))

    def test_empty_catalog(self):
        self.write_catalog([])
        self.assertEqual(self.go(), 1)
        self.assertIn("error: catalog has no stage entries", self.lines)

    def test_out_inside_repo_refused(self):
        inside = stages.REPO_ROOT / "stages-test-should-not-exist"
        self.assertEqual(run(self.extracted, inside, self.catalog, self.lines.append), 2)
        self.assertFalse(inside.exists())


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_stages.py 2>&1 | grep Error`
Expected: `ImportError: cannot import name 'stages' from 'mmxdemake'`

- [ ] **Step 3: Write `mmxdemake/stages.py`**

```python
import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.arcentries import ArcError, entry_data, only, split
from mmxdemake.collision import COLOURS, EMPTY_RGB
from mmxdemake.index import load_index
from mmxdemake.learn import DEFAULT_CATALOG, DEFAULT_EXTRACTED
from mmxdemake.png import write_png
from mmxdemake.shade import grey
from mmxdemake.stagemap import SIDE, collision, decode_screens, decode_tile_info, tile
from mmxdemake.texture import BLOCK_SIZE, TILE, bits, raw_tile, tile_ref

REPO_ROOT = Path(__file__).resolve().parents[1]
SCREEN_PX = SIDE * TILE
COLLISION_PX = 4
LAYERS = 3


class StagesFailed(Exception):
    pass


def slug(name):
    return re.sub(r"[^a-z0-9]+", "-", name.lower()).strip("-")


def _allowed(name):
    return name == "summary.txt" or re.fullmatch(r"m\d{3}-[a-z0-9-]+", name) is not None


def _layers(entry, overlay, screen_count):
    w, h = entry["width"], entry["height"]
    raw = bytearray(overlay[entry["offset"]:entry["offset"] + LAYERS * w * h])
    if len(raw) != LAYERS * w * h:
        raise StagesFailed(f"{entry['name']}: overlay {entry['overlay']} ends before the layout")
    for layer, x, y in entry["differing_cells"]:
        raw[layer * w * h + y * w + x] = 0
    bad = sum(1 for v in raw if v >= screen_count)
    if bad:
        raise StagesFailed(f"{entry['name']}: {bad} layout cells are not screen numbers")
    return [[[raw[l * w * h + y * w + x] for x in range(w)] for y in range(h)] for l in range(LAYERS)]


def _stage_png(layers, w, h, screens, info, block):
    width, height = w * SCREEN_PX, h * SCREEN_PX
    rows = [bytearray(b"\0\0\0\xff" * width) for _ in range(height)]
    cache = {}
    stride = TILE * 4
    for layer in (2, 1, 0):
        for y in range(h):
            for x in range(w):
                s = layers[layer][y][x]
                if not s:
                    continue
                for k, v in enumerate(screens[s]):
                    t = tile(v)
                    if not t:
                        continue
                    if t not in cache:
                        ref = tile_ref(info[t])
                        values = raw_tile(block, ref)
                        cache[t] = None if values is None else grey(values, bits(ref.page), ref.palette)
                    rgba = cache[t]
                    if rgba is None:
                        continue
                    px = x * SCREEN_PX + (k % SIDE) * TILE
                    py = y * SCREEN_PX + (k // SIDE) * TILE
                    for r in range(TILE):
                        rows[py + r][px * 4:px * 4 + stride] = rgba[r * stride:(r + 1) * stride]
    return width, height, [bytes(r) for r in rows]


def _collision_png(layer0, w, h, screens, info):
    width, height = w * SIDE * COLLISION_PX, h * SIDE * COLLISION_PX
    rows = [bytearray(bytes(EMPTY_RGB + (255,)) * width) for _ in range(height)]
    for y in range(h):
        for x in range(w):
            s = layer0[y][x]
            if not s:
                continue
            for k, v in enumerate(screens[s]):
                t = tile(v)
                rgb = EMPTY_RGB if t == 0 else COLOURS[collision(info[t])]
                block = bytes(rgb + (255,)) * COLLISION_PX
                px = (x * SIDE + k % SIDE) * COLLISION_PX
                py = (y * SIDE + k // SIDE) * COLLISION_PX
                for r in range(COLLISION_PX):
                    rows[py + r][px * 4:(px + COLLISION_PX) * 4] = block
    return width, height, [bytes(r) for r in rows]


def _draw(extracted, tmp, entry, record):
    body = (extracted / record["path"]).read_bytes()
    if hashlib.sha1(body).hexdigest() != record["sha1"]:
        raise StagesFailed(f"{entry['name']}: member {entry['member']} sha1 does not match the index")
    entries = split(body)
    screens = decode_screens(entry_data(body, only(entries, 0)))
    info = decode_tile_info(entry_data(body, only(entries, 1)))
    block = entry_data(body, only(entries, 0x10000))
    if len(block) != BLOCK_SIZE:
        raise StagesFailed(f"{entry['name']}: texture block is {len(block)} bytes, need {BLOCK_SIZE}")
    overlay = (extracted / f"rock_bin/{entry['overlay']:02d}.bin").read_bytes()
    layers = _layers(entry, overlay, len(screens))
    w, h = entry["width"], entry["height"]
    folder = tmp / f"m{entry['member']:03d}-{slug(entry['name'])}"
    folder.mkdir()
    doc = {"schema_version": 1, "name": entry["name"], "member": entry["member"], "width": w, "height": h,
           "source": {"overlay": entry["overlay"], "offset": entry["offset"]}, "layers": layers}
    (folder / "layout.json").write_text(json.dumps(doc, sort_keys=True) + "\n")
    write_png(folder / "stage.png", *_stage_png(layers, w, h, screens, info, block))
    write_png(folder / "collision.png", *_collision_png(layers[0], w, h, screens, info))
    line = f"{entry['name']}: member {entry['member']}, {w}x{h}, overlay {entry['overlay']} @{entry['offset']:#x}"
    (folder / "summary.txt").write_text(line + "\n")
    return line


def run(extracted, out, catalog=DEFAULT_CATALOG, log=print):
    extracted, out, catalog = Path(extracted), Path(out), Path(catalog)
    refusal = outdir.check(out, REPO_ROOT, _allowed)
    if refusal:
        log(f"error: {refusal}")
        return 2
    try:
        records = {r["index"]: r for r in load_index(extracted / "index.json")["files"] if r["kind"] == "rock_dat"}
        entries = json.loads((catalog / "stages.json").read_text())
    except (OSError, ValueError, KeyError, TypeError, AttributeError) as e:
        log(f"error: {e}")
        return 2
    try:
        if not entries:
            raise StagesFailed("catalog has no stage entries")
        tmp = outdir.fresh_tmp(out)
        lines = []
        for entry in entries:
            record = records.get(entry["member"])
            if record is None:
                raise StagesFailed(f"{entry['name']}: member {entry['member']} is not in the index")
            line = _draw(extracted, tmp, entry, record)
            log(line)
            lines.append(line)
        (tmp / "summary.txt").write_text("\n".join(lines + ["done"]) + "\n")
        outdir.publish(tmp, out)
    except (StagesFailed, ArcError, ValueError, KeyError, OSError) as e:
        log(f"error: {e}")
        return 1
    log(f"done: {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.stages")
    parser.add_argument("--extracted", default=str(DEFAULT_EXTRACTED))
    parser.add_argument("--out", required=True)
    parser.add_argument("--catalog", default=str(DEFAULT_CATALOG))
    args = parser.parse_args(argv)
    return run(Path(args.extracted).expanduser(), Path(args.out).expanduser(), Path(args.catalog).expanduser(),
               log=print)


if __name__ == "__main__":
    sys.exit(main())
```

- [ ] **Step 4: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 179 tests`, `OK (skipped=4)`

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/stages.py tests/test_stages.py
git commit -m "Add the stages command that draws whole stages from the catalog

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 5: Real data, the first catalog entry, and docs

**Files:**
- Create: `tests/test_retail_stages.py`, `catalog/savestates.json`, `catalog/stages.json` (written by `learn`)
- Modify: `README.md`, `docs/process/process-log.md`

- [ ] **Step 1: Write `tests/test_retail_stages.py`**

```python
import json
import os
import tempfile
import unittest
from pathlib import Path

from mmxdemake import learn, stages

EXTRACTED = os.environ.get("MMX6_EXTRACTED")
DUMPS = os.environ.get("MMX6_DUMPS")
WORK = Path(os.environ.get("MMX6_WORK", "~/mmx-data")).expanduser()


@unittest.skipUnless(EXTRACTED and DUMPS, "set MMX6_EXTRACTED and MMX6_DUMPS to run")
class RetailStagesTest(unittest.TestCase):
    def test_intro_section_1(self):
        with tempfile.TemporaryDirectory(dir=WORK) as t:
            catalog = Path(t) / "catalog"
            lines = []
            code = learn.run("intro-1_start.state.gz", "Intro, section 1", "", Path(DUMPS).expanduser(),
                             Path(EXTRACTED).expanduser(), catalog, lines.append)
            self.assertEqual(code, 0, lines)
            entry = json.loads((catalog / "stages.json").read_text())[0]
            self.assertEqual((entry["member"], entry["width"], entry["height"], entry["overlay"], entry["offset"]),
                             (94, 26, 6, 2, 0x107C6))
            self.assertEqual(len(entry["differing_cells"]), 19)
            self.assertTrue(all(l == 0 and y == 0 for l, _, y in entry["differing_cells"]))
            self.assertGreaterEqual(entry["edge"]["h"], 0.85)
            self.assertGreaterEqual(entry["edge"]["v"], 0.85)
            out = Path(t) / "stages"
            self.assertEqual(stages.run(Path(EXTRACTED).expanduser(), out, catalog, lines.append), 0, lines)
            self.assertTrue((out / "m094-intro-section-1/stage.png").exists())


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the suite, then the real-data test**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 180 tests`, `OK (skipped=5)`

Run: `MMX6_EXTRACTED=~/mmx-data/extracted/retail MMX6_DUMPS=~/mmx-data/dumps python3 -m unittest tests.test_retail_stages 2>&1 | grep -E "^(Ran|OK|FAILED)|Error"`
Expected: `Ran 1 test`, `OK`. If it fails, stop and find the cause; do not loosen a check.

- [ ] **Step 3: Learn the intro state into the repo catalog**

Run: `python3 -m mmxdemake.learn --state intro-1_start.state.gz --name "Intro, section 1" --note "X standing at the start of the intro stage"`
Expected: `Intro, section 1: member 94, 26x6, overlay 2 @0x107c6, edges 0.9xx/0.9xx, 19 cells differ (layer 0 row 0)`, exit 0; `catalog/savestates.json` and `catalog/stages.json` exist.

- [ ] **Step 4: Draw the stages**

Run: `python3 -m mmxdemake.stages --out ~/mmx-data/stages`
Expected: `Intro, section 1: member 94, 26x6, overlay 2 @0x107c6`, then `done: /home/johnlavender/mmx-data/stages`, exit 0. Compare `stage.png` with `~/mmx-data/preview/intro-1/stage.png` (they should look the same).

- [ ] **Step 5: Append to `README.md`**

````markdown

Stage layouts come from PCSXR save states (kept outside the repo in
`~/mmx-data/dumps/`; the repo keeps only `catalog/`):

```
python3 -m mmxdemake.learn --state intro-1_start.state.gz --name "Intro, section 1"
python3 -m mmxdemake.stages --out ~/mmx-data/stages
```

`learn` finds the loaded stage member, the layout and its place on disc, and
adds an entry to `catalog/stages.json` (and the save state to
`catalog/savestates.json`). `stages` draws every entry: `layout.json`,
`stage.png` (greyscale) and `collision.png` (layer 0).
````

- [ ] **Step 6: Append to `docs/process/process-log.md`**

Add a section before "Lessons so far", titled `## 37. Build the stage layout tool (task F)`, with: what was built, the test counts, the `learn` output line on the intro, the catalog files, the comparison with the hand-built preview, and the commit hashes.

- [ ] **Step 7: Check that no game data is in the repo, then commit**

Run: `git status --short` and `cat catalog/stages.json`
Expected: only `README.md`, `docs/process/process-log.md`, `tests/test_retail_stages.py`, `catalog/`; the catalog holds names, hashes and numbers only.

```bash
git add README.md docs/process/process-log.md tests/test_retail_stages.py catalog/savestates.json catalog/stages.json
git commit -m "Add the intro catalog entry, the real-data stages test and usage notes

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```
