# Object Placement Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Find each stage's object list through the RAM pointer at `0x8007349c`, record its disc location in `catalog/stages.json`, and draw `objects.json`, `objects.png` and an object table for each stage.

**Architecture:** `objects` holds pure functions: read the list from RAM, read a list from any buffer up to its end record, find it on the disc, and decode the records. `digits` is a 3 × 5 pixel font. `learn` adds the list location to each entry. `stages` reads the list from the disc and draws the markers on a dimmed copy of `stage.png`.

**Tech Stack:** Python 3.10 standard library only.

**Spec:** `docs/superpowers/specs/2026-10-19-object-placement-design.md`

## Global Constraints

- Python 3.10, standard library only. Tests: `python3 -m unittest discover -s tests` from the repo root. No `-I`.
- List pointer: u32 LE at RAM `0x8007349c`. Valid when `0x80000000 <= p < 0x80200000` and `p % 2 == 0`.
- Record: 8 bytes, `struct` format `<BBBBHH`, fields `a, b, id, kind, x, y`. End record: `kind == 0x0f`. At most 1024 records before the end record.
- Bounds: refuse `x >= width × 256` or `y >= height × 256`. (0, 0) is valid.
- Disc search: the list bytes, including the end record, must occur exactly once in all `rock_bin/NN.bin`.
- Catalog field: `"objects": {"overlay": int, "offset": int, "count": int}`. It is optional in `stages`.
- `objects.png`: `stage.png` with each RGB channel halved (`v >> 1`), alpha 255. Marker 9 × 9 centred on (x, y), 1 px black border, fill `KIND_COLOURS[kind]`. Label: the record index, white digits at scale 2, 1 px black outline, 2 px right of the marker (left side when it would pass the right edge), vertically centred. Pixels outside the image are cut.
- Exit codes: 0 ok; 1 failed check; 2 bad arguments or malformed catalog.
- No game data in the repo. No code comments. No code copied from reference repos.
- Commit to `master`. End each commit message with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. The pointer at `0x8007349c` is fixed for the intro only. In another stage it may point at something else. The result must then be a clear refusal (no disc hit, or no end record), not a wrong catalog entry. Tests: Task 2 (pointer checks) and Task 3 (list not on disc).
2. Objects at (0, 0) or near an image edge: the marker and label are cut, with no crash. Test: Task 4 (record 2 at (0, 0); record 1 near the right edge flips its label).
3. A catalog entry from task F with no `objects` field still draws, and says `objects not learned`. Test: Task 4.
4. A catalog `objects.offset` that is wrong gives exit 1 and one `error:` line, not a traceback. Test: Task 4.
5. Running `learn` again on the intro must keep the save-state note. `learn` replaces the manifest note with `--note`, so Task 5 passes the old note again.

---

## File structure

```
mmxdemake/
  digits.py      objects.py                       # new
  learn.py       stages.py                        # modified
tests/
  test_digits.py test_objects.py                  # new
  stagefix.py    test_learn.py   test_stages.py   test_retail_stages.py   # modified
```

---

### Task 1: `digits`

**Files:**
- Create: `mmxdemake/digits.py`
- Test: `tests/test_digits.py`

**Interfaces:**
- Produces: `digits.GLYPHS: dict[str, tuple[str, ...]]`, `digits.text_mask(s: str) -> list[list[int]]` (5 rows; 3 columns per digit, 1 empty column between digits; `ValueError` for a non-digit).

- [ ] **Step 1: Write the failing test**

`tests/test_digits.py`:

```python
import unittest

from mmxdemake.digits import GLYPHS, text_mask


class DigitsTest(unittest.TestCase):
    def test_glyph_shape(self):
        self.assertEqual(sorted(GLYPHS), list("0123456789"))
        for rows in GLYPHS.values():
            self.assertEqual([len(r) for r in rows], [3] * 5)

    def test_text_mask(self):
        self.assertEqual(text_mask("10"), [
            [0, 1, 0, 0, 1, 1, 1],
            [1, 1, 0, 0, 1, 0, 1],
            [0, 1, 0, 0, 1, 0, 1],
            [0, 1, 0, 0, 1, 0, 1],
            [1, 1, 1, 0, 1, 1, 1],
        ])

    def test_rejects_letter(self):
        with self.assertRaises(ValueError):
            text_mask("a")


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run it to verify it fails**

Run: `python3 -m unittest tests.test_digits 2>&1 | tail -3`
Expected: FAIL with `ModuleNotFoundError: No module named 'mmxdemake.digits'`.

- [ ] **Step 3: Write `mmxdemake/digits.py`**

```python
GLYPHS = {
    "0": ("111", "101", "101", "101", "111"),
    "1": ("010", "110", "010", "010", "111"),
    "2": ("111", "001", "111", "100", "111"),
    "3": ("111", "001", "111", "001", "111"),
    "4": ("101", "101", "111", "001", "001"),
    "5": ("111", "100", "111", "001", "111"),
    "6": ("111", "100", "111", "101", "111"),
    "7": ("111", "001", "001", "001", "001"),
    "8": ("111", "101", "111", "101", "111"),
    "9": ("111", "101", "111", "001", "111"),
}


def text_mask(s):
    bad = [c for c in s if c not in GLYPHS]
    if bad or not s:
        raise ValueError(f"text {s!r} is not only digits")
    rows = []
    for r in range(5):
        rows.append([int(bit) for bit in "0".join(GLYPHS[c][r] for c in s)])
    return rows
```

- [ ] **Step 4: Run it to verify it passes**

Run: `python3 -m unittest tests.test_digits 2>&1 | tail -3`
Expected: `Ran 3 tests`, `OK`.

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/digits.py tests/test_digits.py
git commit -m "Add a 3 x 5 pixel digit font

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: `objects`

**Files:**
- Create: `mmxdemake/objects.py`
- Test: `tests/test_objects.py`

**Interfaces:**
- Consumes: `savestate.ram_at(ram, addr, n) -> bytes`.
- Produces: `objects.LIST_POINTER`, `RECORD`, `END_KIND`, `MAX_RECORDS`, `SCREEN_PX`, `ObjectError`, `list_at(buf, start) -> bytes` (from `start` up to and including the end record), `read_list(ram) -> bytes`, `find_list(data, overlays: dict[int, bytes]) -> (int, int)`, `decode(data, w, h) -> list[dict]` with keys `index, a, b, id, kind, x, y`.

- [ ] **Step 1: Write the failing test**

`tests/test_objects.py`:

```python
import struct
import unittest

from mmxdemake.objects import END_KIND, MAX_RECORDS, ObjectError, decode, find_list, list_at, read_list

END = struct.pack("<BBBBHH", 0, 0, 0, END_KIND, 0, 0)


def rec(a, b, i, kind, x, y):
    return struct.pack("<BBBBHH", a, b, i, kind, x, y)


LIST = rec(0, 6, 1, 3, 0x840, 0x440) + rec(0, 0, 2, 0, 10, 20) + rec(0, 0, 0, 3, 0, 0) + END


def ram_with(pointer, data=LIST, at=0x190000):
    ram = bytearray(0x200000)
    struct.pack_into("<I", ram, 0x7349C, pointer)
    ram[at:at + len(data)] = data
    return bytes(ram)


class ReadListTest(unittest.TestCase):
    def test_read_list(self):
        self.assertEqual(read_list(ram_with(0x80190000)), LIST)

    def test_pointer_zero(self):
        with self.assertRaisesRegex(ObjectError, "pointer 0x0 at 0x8007349c is not a RAM address"):
            read_list(ram_with(0))

    def test_pointer_above_ram(self):
        with self.assertRaisesRegex(ObjectError, "is not a RAM address"):
            read_list(ram_with(0x80200000))

    def test_pointer_odd(self):
        with self.assertRaisesRegex(ObjectError, "is not a RAM address"):
            read_list(ram_with(0x80190001))

    def test_no_end_in_max_records(self):
        body = rec(0, 0, 0, 1, 0, 0) * (MAX_RECORDS + 1)
        with self.assertRaisesRegex(ObjectError, f"no end record in {MAX_RECORDS} records"):
            read_list(ram_with(0x80100000, body, 0x100000))

    def test_runs_to_end_of_ram(self):
        with self.assertRaisesRegex(ObjectError, "runs to the end of the data"):
            read_list(ram_with(0x801FFFF8, rec(0, 0, 0, 1, 0, 0), 0x1FFFF8))

    def test_list_at_offset(self):
        self.assertEqual(list_at(b"\xee" * 6 + LIST + b"\xee" * 9, 6), LIST)


class FindListTest(unittest.TestCase):
    def test_one_hit(self):
        self.assertEqual(find_list(LIST, {0: b"\xdd" * 40, 2: b"\xee" * 5 + LIST}), (2, 5))

    def test_no_hit(self):
        with self.assertRaisesRegex(ObjectError, "found 0 times"):
            find_list(LIST, {0: b"\xdd" * 40})

    def test_two_hits(self):
        with self.assertRaisesRegex(ObjectError, "found 2 times"):
            find_list(LIST, {0: LIST, 1: LIST})


class DecodeTest(unittest.TestCase):
    def test_fields(self):
        self.assertEqual(decode(LIST, 26, 6)[0],
                         {"index": 0, "a": 0, "b": 6, "id": 1, "kind": 3, "x": 0x840, "y": 0x440})
        self.assertEqual(len(decode(LIST, 26, 6)), 3)

    def test_origin_is_valid(self):
        self.assertEqual(decode(LIST, 26, 6)[2], {"index": 2, "a": 0, "b": 0, "id": 0, "kind": 3, "x": 0, "y": 0})

    def test_outside_stage(self):
        with self.assertRaisesRegex(ObjectError, r"object 0 at \(256, 0\) is outside the 1x1 stage"):
            decode(rec(0, 0, 0, 1, 256, 0) + END, 1, 1)
        with self.assertRaisesRegex(ObjectError, r"object 0 at \(0, 256\) is outside the 1x1 stage"):
            decode(rec(0, 0, 0, 1, 0, 256) + END, 1, 1)

    def test_no_end_record(self):
        with self.assertRaisesRegex(ObjectError, "no end record"):
            decode(rec(0, 0, 0, 1, 0, 0), 1, 1)

    def test_bad_length(self):
        with self.assertRaisesRegex(ObjectError, "not a multiple of 8"):
            decode(LIST + b"\0", 26, 6)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run it to verify it fails**

Run: `python3 -m unittest tests.test_objects 2>&1 | tail -3`
Expected: FAIL with `ModuleNotFoundError: No module named 'mmxdemake.objects'`.

- [ ] **Step 3: Write `mmxdemake/objects.py`**

```python
import struct

from mmxdemake.savestate import ram_at

LIST_POINTER = 0x8007349C
RECORD = 8
END_KIND = 0x0F
MAX_RECORDS = 1024
SCREEN_PX = 256
RAM_BASE = 0x80000000
RAM_END = 0x80200000
FORMAT = "<BBBBHH"
FIELDS = ("a", "b", "id", "kind", "x", "y")


class ObjectError(Exception):
    pass


def list_at(buf, start):
    for n in range(MAX_RECORDS + 1):
        k = start + n * RECORD
        if k + RECORD > len(buf):
            raise ObjectError(f"object list at {start:#x} runs to the end of the data without an end record")
        if buf[k + 3] == END_KIND:
            return bytes(buf[start:k + RECORD])
    raise ObjectError(f"object list at {start:#x} has no end record in {MAX_RECORDS} records")


def read_list(ram):
    (p,) = struct.unpack("<I", ram_at(ram, LIST_POINTER, 4))
    if not RAM_BASE <= p < RAM_END or p % 2:
        raise ObjectError(f"object list pointer {p:#x} at {LIST_POINTER:#x} is not a RAM address")
    return list_at(ram, p - RAM_BASE)


def find_list(data, overlays):
    hits = []
    for i, body in sorted(overlays.items()):
        k = body.find(data)
        while k >= 0:
            hits.append((i, k))
            k = body.find(data, k + 1)
    if len(hits) != 1:
        raise ObjectError(f"object list found {len(hits)} times in the overlays; need exactly 1")
    return hits[0]


def decode(data, w, h):
    if len(data) % RECORD:
        raise ObjectError(f"object list is {len(data)} bytes, not a multiple of {RECORD}")
    records = []
    for index in range(len(data) // RECORD):
        r = dict(zip(FIELDS, struct.unpack_from(FORMAT, data, index * RECORD)), index=index)
        if r["kind"] == END_KIND:
            return records
        if r["x"] >= w * SCREEN_PX or r["y"] >= h * SCREEN_PX:
            raise ObjectError(f"object {index} at ({r['x']}, {r['y']}) is outside the {w}x{h} stage")
        records.append(r)
    raise ObjectError("object list has no end record")
```

Note on `list_at`: the loop runs `MAX_RECORDS + 1` times, so that a list of 1024 records plus its end record is accepted, and 1025 records with no end record is refused.

- [ ] **Step 4: Run it to verify it passes**

Run: `python3 -m unittest tests.test_objects 2>&1 | tail -3`
Expected: `Ran 15 tests`, `OK`.

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/objects.py tests/test_objects.py
git commit -m "Add the object list reader and decoder

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: `learn` records the object list

**Files:**
- Modify: `tests/stagefix.py`, `mmxdemake/learn.py`
- Test: `tests/test_learn.py`

**Interfaces:**
- Consumes: `objects.ObjectError`, `read_list`, `find_list`, `decode` (Task 2).
- Produces: `stagefix.OBJECTS` (list of `(a, b, id, kind, x, y)`), `stagefix.OBJECTS_OFFSET = 172`, `stagefix.LIST_AT = 0x190000`, `stagefix.objects_bytes(objects=OBJECTS) -> bytes` (records plus end record), `stagefix.write_state(path, layout=None, objects=None)`; catalog entries with `"objects": {"overlay", "offset", "count"}`.

- [ ] **Step 1: Change the fixture `tests/stagefix.py`**

Add after `SCREENS = [...]`:

```python
OBJECTS = [(0, 6, 1, 3, 40, 300), (0, 0, 2, 0, 3060, 10), (0, 0, 0, 3, 0, 0)]
OBJECTS_OFFSET = OFFSET + 3 * W * H
LIST_AT = 0x190000
```

Add after `disc_layout()`:

```python
def objects_bytes(objects=OBJECTS):
    return b"".join(struct.pack("<BBBBHH", *o) for o in objects) + struct.pack("<BBBBHH", 0, 0, 0, 0x0F, 0, 0)
```

Replace `overlay_bytes()`:

```python
def overlay_bytes():
    return b"\xee" * OFFSET + disc_layout() + objects_bytes() + b"\xee" * 50
```

Replace `write_state()`:

```python
def write_state(path, layout=None, objects=None):
    ram = bytearray(synth.fake_ram(synth.fake_exe(), W, H, ram_layout() if layout is None else layout,
                                   pack_info(INFO)))
    data = objects_bytes() if objects is None else objects
    struct.pack_into("<I", ram, 0x7349C, 0x80000000 + LIST_AT)
    ram[LIST_AT:LIST_AT + len(data)] = data
    path.write_bytes(synth.build_state(bytes(ram)))
```

- [ ] **Step 2: Write the failing tests in `tests/test_learn.py`**

Change `ENTRY` (add the `objects` key):

```python
ENTRY = {
    "name": "Stage A", "member": 1, "width": 12, "height": 2, "overlay": 3, "offset": 100,
    "save_state": "a.state.gz", "edge": {"h": 1.0, "h_joins": 11, "v": 0.0, "v_joins": 0},
    "differing_cells": [[0, x, 0] for x in range(12)],
    "objects": {"overlay": 3, "offset": 172, "count": 3},
}
```

In `test_writes_catalog`, change the expected line:

```python
        self.assertEqual(self.lines, [
            "Stage A: member 1, 12x2, overlay 3 @0x64, edges 1.000/0.000, 12 cells differ (layer 0 row 0), "
            "3 objects @0xac"])
```

Add after `test_bad_layout`:

```python
    def test_object_list_not_on_disc(self):
        stagefix.write_state(self.dumps / "a.state.gz", objects=stagefix.objects_bytes([(0, 0, 9, 1, 10, 10)]))
        self.assertEqual(self.go(), 1)
        self.assertIn("error: object list found 0 times in the overlays; need exactly 1", self.lines)
        self.assertFalse((self.catalog / "stages.json").exists())
```

- [ ] **Step 3: Run them to verify they fail**

Run: `python3 -m unittest tests.test_learn 2>&1 | tail -3`
Expected: FAILED (failures=2): `test_writes_catalog` (no `objects` key, line without objects) and `test_object_list_not_on_disc` (exit 0).

- [ ] **Step 4: Change `mmxdemake/learn.py`**

Add the import after the `index` import:

```python
from mmxdemake.objects import ObjectError, decode, find_list, read_list
```

In `run`, after the `diff = differing_cells(...)` line, inside the `try`:

```python
        data = read_list(ram)
        obj_overlay, obj_offset = find_list(data, overlays)
        count = len(decode(data, w, h))
```

Change the `except` line to:

```python
    except (SaveStateError, LayoutError, ObjectError, ArcError, LearnFailed, ValueError, OSError) as e:
```

Replace the `entry = {...}` statement with:

```python
    entry = {"name": name, "member": member, "width": w, "height": h, "overlay": overlay, "offset": offset,
             "save_state": state, "save_state_sha1": sha1,
             "edge": {"h": round(hsc, 3), "h_joins": hj, "v": round(vsc, 3), "v_joins": vj},
             "differing_cells": [list(c) for c in diff],
             "objects": {"overlay": obj_overlay, "offset": obj_offset, "count": count}}
```

Change the final log call to:

```python
    log(f"{name}: member {member}, {w}x{h}, overlay {overlay} @{offset:#x}, edges {hsc:.3f}/{vsc:.3f}, "
        f"{len(diff)} cells differ{where}, {count} objects @{obj_offset:#x}")
```

- [ ] **Step 5: Run the learn tests and the suite**

Run: `python3 -m unittest tests.test_learn 2>&1 | tail -3`
Expected: `Ran 11 tests`, `OK`.

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 207 tests`, `OK (skipped=5)`.

- [ ] **Step 6: Commit**

```bash
git add tests/stagefix.py tests/test_learn.py mmxdemake/learn.py
git commit -m "Record the object list location in learn

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: `stages` draws the objects

**Files:**
- Modify: `mmxdemake/stages.py`
- Test: `tests/test_stages.py`

**Interfaces:**
- Consumes: `objects.ObjectError`, `list_at`, `decode` (Task 2); `digits.text_mask` (Task 1); `stagefix.OBJECTS_OFFSET` (Task 3).
- Produces: `stages.KIND_COLOURS: list[tuple[int, int, int]]` (16 entries); per-stage `objects.json`, `objects.png`, object rows in `summary.txt`.

- [ ] **Step 1: Write the failing tests in `tests/test_stages.py`**

Change the import line to:

```python
from mmxdemake.stages import KIND_COLOURS, run, slug
```

Add after `ENTRY`:

```python
OBJ_ENTRY = dict(ENTRY, objects={"overlay": 3, "offset": stagefix.OBJECTS_OFFSET, "count": 3})
```

In `test_output_files`, change the expected top summary:

```python
        self.assertEqual((out / "summary.txt").read_text().splitlines(),
                         ["Stage A: member 1, 12x2, overlay 3 @0x64, objects not learned", "done"])
```

Add these tests before `test_layout_cell_out_of_range`:

```python
    def test_objects_files(self):
        self.write_catalog([OBJ_ENTRY])
        self.assertEqual(self.go(), 0, self.lines)
        folder = self.dir / "stages/m001-stage-a"
        self.assertEqual(sorted(p.name for p in folder.iterdir()),
                         ["collision.png", "layout.json", "objects.json", "objects.png", "stage.png", "summary.txt"])
        doc = json.loads((folder / "objects.json").read_text())
        self.assertEqual(doc["source"], {"overlay": 3, "offset": 172})
        self.assertEqual(doc["objects"][0], {"index": 0, "a": 0, "b": 6, "id": 1, "kind": 3, "x": 40, "y": 300})
        self.assertEqual(len(doc["objects"]), 3)
        lines = (folder / "summary.txt").read_text().splitlines()
        self.assertEqual(lines[0], "Stage A: member 1, 12x2, overlay 3 @0x64, 3 objects")
        self.assertEqual(lines[1], "objects: 3")
        self.assertEqual(lines[2].split(), ["index", "kind", "id", "b", "a", "x", "y", "screen"])
        self.assertEqual(lines[3].split(), ["0", "3", "1", "6", "0", "40", "300", "0,1"])
        self.assertEqual(lines[4].split(), ["1", "0", "2", "0", "0", "3060", "10", "11,0"])

    def test_objects_pixels(self):
        self.write_catalog([OBJ_ENTRY])
        self.go()
        _, _, stage = read_png(self.dir / "stages/m001-stage-a/stage.png")
        width, height, rows = read_png(self.dir / "stages/m001-stage-a/objects.png")
        self.assertEqual((width, height), (3072, 512))
        self.assertEqual(pixel(rows, 40, 300), KIND_COLOURS[3])
        self.assertEqual(pixel(rows, 36, 300), (0, 0, 0))
        self.assertEqual(pixel(rows, 47, 295), (255, 255, 255))
        self.assertEqual(pixel(rows, 3050, 5), (255, 255, 255))
        self.assertEqual(pixel(rows, 0, 0), KIND_COLOURS[3])
        self.assertEqual(pixel(rows, 1500, 100), tuple(v >> 1 for v in pixel(stage, 1500, 100)))
        self.assertEqual(pixel(rows, 0, 511), tuple(v >> 1 for v in pixel(stage, 0, 511)))

    def test_objects_count_mismatch(self):
        self.write_catalog([dict(ENTRY, objects={"overlay": 3, "offset": stagefix.OBJECTS_OFFSET, "count": 4})])
        self.assertEqual(self.go(), 1)
        self.assertIn("error: Stage A: 3 objects on disc, the catalog says 4", self.lines)

    def test_objects_bad_offset(self):
        self.write_catalog([dict(ENTRY, objects={"overlay": 3, "offset": 0, "count": 3})])
        self.assertEqual(self.go(), 1)
        self.assertTrue(self.lines[0].startswith("error: Stage A: object list at 0x0"), self.lines)

    def test_objects_malformed(self):
        for bad in ({"overlay": 3}, "x", {"overlay": 3, "offset": "0", "count": 3}):
            self.write_catalog([dict(ENTRY, objects=bad)])
            self.assertEqual(self.go(), 2, bad)
            self.assertIn("error: catalog entry 'Stage A' has a malformed objects field", self.lines)
```

Check of the pixel values (fixture: 12 × 2 screens, 3072 × 512 px):
- Record 0 (index 0, kind 3) at (40, 300): marker x 36–44. The label starts at x = 40 + 4 + 2 + 1 = 47, top = 300 − 5 = 295. The glyph `0` has row 0 col 0 set, so (47, 295) is white.
- Record 1 (index 1, kind 0) at (3060, 10): 3067 + 6 > 3072, so the label goes left: start = 3060 − 6 − 6 = 3048, top = 5. The glyph `1` has row 0 col 1 set, so x 3050–3051 at y 5–6 is white.
- Record 2 (index 2, kind 3) at (0, 0): the marker is cut, and (0, 0) is fill.
- (0, 511) is far from every marker and label.

- [ ] **Step 2: Run them to verify they fail**

Run: `python3 -m unittest tests.test_stages 2>&1 | tail -3`
Expected: FAIL with `ImportError: cannot import name 'KIND_COLOURS'`.

- [ ] **Step 3: Change `mmxdemake/stages.py`**

Add imports (keep alphabetical order of modules):

```python
from mmxdemake.digits import text_mask
```

after the `collision` import, and

```python
from mmxdemake.objects import ObjectError, decode, list_at
```

after the `learn` import.

Add constants after `REQUIRED = (...)`:

```python
OBJECT_KEYS = ("overlay", "offset", "count")
MARKER = 4
GAP = 2
SCALE = 2
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
HALF = bytes(v >> 1 for v in range(256))
KIND_COLOURS = [(255, 64, 64), (64, 255, 64), (64, 128, 255), (255, 255, 64), (255, 64, 255), (64, 255, 255),
                (255, 160, 32), (160, 255, 32), (32, 160, 255), (255, 32, 160), (160, 32, 255), (32, 255, 160),
                (200, 200, 200), (255, 128, 128), (128, 255, 128), (128, 128, 255)]
```

In `_validate`, inside the `for e in entries:` loop, after the `differing_cells` loop:

```python
        if "objects" in e:
            o = e["objects"]
            if not isinstance(o, dict) or not all(isinstance(o.get(k), int) for k in OBJECT_KEYS):
                raise ValueError(f"catalog entry {name!r} has a malformed objects field")
```

Add after `_collision_png`:

```python
def _objects(extracted, entry, w, h):
    o = entry["objects"]
    body = (extracted / f"rock_bin/{o['overlay']:02d}.bin").read_bytes()
    try:
        records = decode(list_at(body, o["offset"]), w, h)
    except ObjectError as e:
        raise StagesFailed(f"{entry['name']}: {e}") from e
    if len(records) != o["count"]:
        raise StagesFailed(f"{entry['name']}: {len(records)} objects on disc, the catalog says {o['count']}")
    return records


def _objects_png(width, height, stage_rows, records):
    rows = []
    for r in stage_rows:
        row = bytearray(r.translate(HALF))
        row[3::4] = b"\xff" * width
        rows.append(row)

    def put(x, y, rgb):
        if 0 <= x < width and 0 <= y < height:
            rows[y][x * 4:x * 4 + 3] = bytes(rgb)

    for rec in records:
        cx, cy = rec["x"], rec["y"]
        for dy in range(-MARKER, MARKER + 1):
            for dx in range(-MARKER, MARKER + 1):
                edge = abs(dx) == MARKER or abs(dy) == MARKER
                put(cx + dx, cy + dy, BLACK if edge else KIND_COLOURS[rec["kind"] % len(KIND_COLOURS)])
        mask = text_mask(str(rec["index"]))
        lw, lh = len(mask[0]) * SCALE, len(mask) * SCALE
        left = cx + MARKER + GAP + 1
        if left + lw > width:
            left = cx - MARKER - GAP - lw
        top = cy - lh // 2
        on = [(left + c * SCALE + i, top + r * SCALE + j) for r, line in enumerate(mask)
              for c, bit in enumerate(line) if bit for i in range(SCALE) for j in range(SCALE)]
        for x, y in on:
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    put(x + dx, y + dy, BLACK)
        for x, y in on:
            put(x, y, WHITE)
    return [bytes(r) for r in rows]


def _object_rows(records):
    head = f"{'index':>5} {'kind':>4} {'id':>3} {'b':>3} {'a':>3} {'x':>5} {'y':>5} screen"
    return [head] + [f"{r['index']:5d} {r['kind']:4d} {r['id']:3d} {r['b']:3d} {r['a']:3d} {r['x']:5d} {r['y']:5d} "
                     f"{r['x'] // SCREEN_PX},{r['y'] // SCREEN_PX}" for r in records]
```

Note: `kind` is one byte and `KIND_COLOURS` has 16 entries, so the index is taken modulo 16. `decode` does not refuse kinds above 15 (the intro has only 0–5).

`SCREEN_PX` in `stages.py` is already `SIDE * TILE` = 256, so `_object_rows` uses it.

In `_draw`, replace the line that writes `stage.png` and the lines from `line = ...` to `return line`:

```python
    width, height, stage_rows = _stage_png(layers, w, h, screens, info, block)
    write_png(folder / "stage.png", width, height, stage_rows)
    write_png(folder / "collision.png", *_collision_png(layers[0], w, h, screens, info))
    line = f"{entry['name']}: member {entry['member']}, {w}x{h}, overlay {entry['overlay']} @{entry['offset']:#x}"
    table = []
    if "objects" in entry:
        records = _objects(extracted, entry, w, h)
        o = entry["objects"]
        obj_doc = {"schema_version": 1, "name": entry["name"], "member": entry["member"],
                   "source": {"overlay": o["overlay"], "offset": o["offset"]}, "objects": records}
        (folder / "objects.json").write_text(json.dumps(obj_doc, sort_keys=True) + "\n")
        write_png(folder / "objects.png", width, height, _objects_png(width, height, stage_rows, records))
        line += f", {len(records)} objects"
        table = [f"objects: {len(records)}"] + _object_rows(records)
    else:
        line += ", objects not learned"
    (folder / "summary.txt").write_text("\n".join([line] + table) + "\n")
    return line
```

(This replaces the old `write_png(folder / "stage.png", *_stage_png(...))` and `write_png(folder / "collision.png", ...)` lines, so `collision.png` is written once.)

- [ ] **Step 4: Run the stages tests and the suite**

Run: `python3 -m unittest tests.test_stages 2>&1 | tail -3`
Expected: `Ran 15 tests`, `OK`.

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 212 tests`, `OK (skipped=5)`.

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/stages.py tests/test_stages.py
git commit -m "Draw object markers and write the object list in stages

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 5: Real data, the catalog update, and docs

**Files:**
- Modify: `tests/test_retail_stages.py`, `catalog/stages.json` (written by `learn`), `README.md`, `docs/TODO.md`, `docs/process/process-log.md`

- [ ] **Step 1: Extend `tests/test_retail_stages.py`**

After the line `self.assertGreaterEqual(entry["edge"]["v"], 0.85)`, add:

```python
            self.assertEqual(entry["objects"], {"overlay": 2, "offset": 0x109B4, "count": 65})
```

After the line `self.assertTrue((out / "m094-intro-section-1/stage.png").exists())`, add:

```python
            doc = json.loads((out / "m094-intro-section-1/objects.json").read_text())
            self.assertEqual({k: doc["objects"][0][k] for k in ("a", "b", "id", "kind", "x", "y")},
                             {"a": 0, "b": 6, "id": 1, "kind": 3, "x": 0x840, "y": 0x440})
            self.assertTrue((out / "m094-intro-section-1/objects.png").exists())
```

- [ ] **Step 2: Run the real-data test**

Run: `MMX6_EXTRACTED=~/mmx-data/extracted/retail MMX6_DUMPS=~/mmx-data/dumps python3 -m unittest tests.test_retail_stages 2>&1 | grep -E "^(Ran|OK|FAILED)|Error"`
Expected: `Ran 1 test`, `OK`. If it fails, stop and find the cause. Do not loosen a check.

- [ ] **Step 3: Learn the intro again into the repo catalog (keep the note)**

Run: `python3 -m mmxdemake.learn --state intro-1_start.state.gz --name "Intro" --note "X standing at the start of the intro stage (the intro has one section)"`
Expected: `Intro: member 94, 26x6, overlay 2 @0x107c6, edges 0.931/0.942, 19 cells differ (layer 0 row 0), 65 objects @0x109b4`, exit 0.

Run: `git diff catalog/`
Expected: only the new `objects` field in `stages.json`. `savestates.json` is unchanged.

- [ ] **Step 4: Draw the stages**

Run: `python3 -m mmxdemake.stages --out ~/mmx-data/stages`
Expected: `Intro: member 94, 26x6, overlay 2 @0x107c6, 65 objects`, then `done: /home/johnlavender/mmx-data/stages`, exit 0. `~/mmx-data/stages/m094-intro/objects.png` exists.

- [ ] **Step 5: Update `README.md`**

Replace the sentence that ends with `` `stage.png` (greyscale) and `collision.png` (layer 0). `` with:

```markdown
`stage.png` (greyscale), `collision.png` (layer 0), and, when the entry has an
object list, `objects.json` and `objects.png` (each object as a square coloured
by its kind, with its number; the numbers are listed in the folder's
`summary.txt`).
```

- [ ] **Step 6: Add an object legend to `docs/TODO.md`**

Insert before `## Deferred minor findings (from reviews)`:

```markdown
## Object legend (name each object kind and id)

Status: open. Started 2026-10-19 on the intro stage (65 objects).

Each stage folder has `objects.png` (one numbered square for each object,
coloured by kind) and an object table in `summary.txt`. The meaning of `kind`,
`id`, `a` and `b` is not known yet.

To do:

- [ ] Confirm on the intro `objects.png` that the markers are on real objects
  (this also checks that `x` and `y` are stage pixels from the top-left).
- [ ] Name the intro objects by number; from that, name each `kind` and `id`.
- [ ] Repeat on the next stage (Commander Yammark) and check that the
  pointer at `0x8007349c` and the meanings are the same.
- [ ] Find what `a` and `b` mean (`b` is 0–8, 14 or 48 in the intro).
- [ ] Look at the other pointers next to `0x8007349c` (`0x800734a4`,
  `0x800734ac`, …).
```

In `## Deferred minor findings (from reviews)`, delete the line `- The 8-byte records after each layout (probably object placements).`

- [ ] **Step 7: Add a process-log section**

Insert a section `## 41. Task G: objects on the intro map` before the `---` that comes before `## Lessons so far` in `docs/process/process-log.md`. It holds the `learn` output line, the object count, where `objects.png` is, and the next action (the user names the markers). Write the facts from Steps 2–4 only.

- [ ] **Step 8: Run the suite and commit**

Run: `python3 -m unittest discover -s tests 2>&1 | grep -E "^(Ran|OK|FAILED)"`
Expected: `Ran 212 tests`, `OK (skipped=5)`.

Run: `git status --short`
Expected: only `tests/test_retail_stages.py`, `catalog/stages.json`, `README.md`, `docs/TODO.md`, `docs/process/process-log.md`. No game data.

```bash
git add tests/test_retail_stages.py catalog/stages.json README.md docs/TODO.md docs/process/process-log.md
git commit -m "Learn the intro object list and add the object legend

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```
