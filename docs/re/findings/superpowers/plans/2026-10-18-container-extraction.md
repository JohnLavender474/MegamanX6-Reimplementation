# Container Extraction Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Extract the 9 ISO files, 59 `ROCK_X6.BIN` members and 243 `ROCK_X6.DAT` members from the Mega Man X6 v1.1 disc image into `~/mmx-data/extracted/retail/`, with an `index.json`, and verify every output.

**Architecture:** Small stdlib-only Python package `mmxdemake`. Each layer of the disc (raw sectors → ISO9660 → `{sector,size}` archives) is one module with pure functions. `extract.py` runs the layers in order, writes to `<out>.tmp/`, and renames it to `<out>/` only after every hard check passes. Unit tests use synthetic images built in memory. One opt-in test runs on the real disc.

**Tech Stack:** Python 3.10 standard library only (`struct`, `hashlib`, `json`, `argparse`, `pathlib`, `shutil`, `unittest`).

**Spec:** `docs/superpowers/specs/2026-10-18-container-extraction-design.md`

## Global Constraints

- Python 3.10, standard library only. No `pip install`.
- Tests: `unittest`. Run from the repo root: `python3 -m unittest discover -s tests`. Do not use `-I`.
- No game data in the repo. Tests use synthetic data or write to a temporary folder outside the repo.
- No code copied from `mmx6`, `mmx4`, `MegaManX6Recomp`, `TeheManX4_Editor` or `MegaManX6_PS1_Modding`. The `mmx6` manifest is used only as a list of hashes.
- Disc identity: BIN SHA-1 `d4f7e08371027a87a3bf13311db5a4c56733f4ea`, serial `SLUS-01395`, revision `v1.1`.
- Sector: 2352 B raw. Form 1 user data = 2048 B at offset 24. Form 2 output = 2336 B at offset 16.
- File form comes only from the directory record XA attribute (bit `0x1000` = Form 2). First-sector submode is an observation.
- `ROCK_X6.BIN`: 59 members, header 2 sectors, tail padding must be zero. `ROCK_X6.DAT`: 243 members, header 2 sectors, tail padding is an observation.
- Output names: `iso/<path>`, `rock_bin/NN.bin` (2 digits), `rock_dat/NNN.bin` (3 digits), `index.json` with `schema_version: 1`.
- Exit codes: 0 ok, 1 failed hard check, 2 bad arguments / identity mismatch / missing BIN / `--out` inside the repo.
- Do not write code comments.
- Commit to `master`. End each commit message with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. A stale `<out>.tmp/` from an earlier failed run → the next run deletes it first, so old files cannot appear in the new output. Test in Task 5.
2. Paths given as `"~/…"` in quotes (the shell does not expand them) → the CLI expands `~`. Test in Task 5.
3. A CUE written on Windows (CRLF line ends, UTF-8 BOM) → it parses the same as a Unix CUE. Test in Task 1.
4. A CUE whose `FILE` names a BIN that does not exist → exit 2 with the missing path in the message, not a traceback. Test in Task 5.
5. `--out` inside the `mmx-demake-tools` repo → exit 2 before any write, so `git add -A` cannot pick up game data. Test in Task 5.

---

## File structure

```
mmxdemake/
  __init__.py     # empty
  problem.py      # Problem dataclass
  cdimage.py      # CUE, BIN sectors, identity hash
  iso9660.py      # file system walk, file reading
  rockarc.py      # {sector,size} archives, checks, header observation
  index.py        # index.json build / write / load
  verify.py       # reference manifest load and compare
  extract.py      # run() and CLI main()
tests/
  synth.py
  test_cdimage.py
  test_iso9660.py
  test_rockarc.py
  test_index_verify.py
  test_extract.py
  test_retail.py
```

---

### Task 1: Package, `Problem`, and `cdimage`

**Files:**
- Create: `mmxdemake/__init__.py`, `mmxdemake/problem.py`, `mmxdemake/cdimage.py`
- Create: `tests/synth.py` (sector builder and disc writer only; Task 2 and 3 add to it)
- Test: `tests/test_cdimage.py`

**Interfaces:**
- Produces:
  - `problem.Problem(where: str, message: str)`, frozen dataclass, `str(p) == f"{where}: {message}"`
  - `cdimage.DiscError(Exception)`
  - `cdimage.parse_cue(cue_path: Path) -> Path`
  - `cdimage.DiscImage(bin_path: Path)`: context manager; `.sector_count: int`; `.raw(lba) -> bytes` (2352); `.form1(lba) -> bytes` (2048); `.form2_raw(lba) -> bytes` (2336); `.submode(lba) -> int`
  - `cdimage.sha1_file(path: Path) -> str`
  - `cdimage.check_identity(bin_path: Path, expected_sha1: str) -> None` (raises `DiscError`)
  - constants `SECTOR_RAW = 2352`, `FORM1_DATA = 2048`, `FORM2_RAW = 2336`
  - `synth.sector(lba: int, payload: bytes = b"", submode: int = SUBMODE_DATA) -> bytes`
  - `synth.write_disc(folder: Path, image: bytes, name: str = "Synth Disc (v1.1)") -> Path` (returns the CUE path)
  - `synth.SUBMODE_DATA = 0x08`, `SUBMODE_AUDIO = 0x04`, `SUBMODE_FORM2 = 0x20`, `SUBMODE_REALTIME = 0x40`

- [ ] **Step 1: Write `tests/synth.py` (sector builder)**

```python
import struct
from math import ceil
from pathlib import Path

SYNC = b"\x00" + b"\xff" * 10 + b"\x00"
SUBMODE_AUDIO = 0x04
SUBMODE_DATA = 0x08
SUBMODE_FORM2 = 0x20
SUBMODE_REALTIME = 0x40


def _bcd(n):
    return ((n // 10) << 4) | (n % 10)


def sector(lba, payload=b"", submode=SUBMODE_DATA):
    assert len(payload) <= 2328
    absolute = lba + 150
    header = bytes([_bcd(absolute // 4500), _bcd((absolute // 75) % 60), _bcd(absolute % 75), 2])
    subheader = bytes([0, 0, submode, 0]) * 2
    return SYNC + header + subheader + payload.ljust(2328, b"\0")


def write_disc(folder, image, name="Synth Disc (v1.1)"):
    folder = Path(folder)
    (folder / f"{name}.bin").write_bytes(image)
    cue = folder / f"{name}.cue"
    cue.write_text(f'FILE "{name}.bin" BINARY\n  TRACK 01 MODE2/2352\n    INDEX 01 00:00:00\n')
    return cue
```

- [ ] **Step 2: Write the failing tests `tests/test_cdimage.py`**

```python
import hashlib
import tempfile
import unittest
from pathlib import Path

import synth
from mmxdemake.cdimage import DiscError, DiscImage, check_identity, parse_cue, sha1_file
from mmxdemake.problem import Problem


class ParseCueTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)

    def tearDown(self):
        self.tmp.cleanup()

    def write(self, text, encoding="utf-8"):
        p = self.dir / "x.cue"
        p.write_bytes(text.encode(encoding))
        return p

    def test_returns_bin_next_to_cue(self):
        cue = self.write('FILE "A (USA) (v1.1).bin" BINARY\n  TRACK 01 MODE2/2352\n    INDEX 01 00:00:00\n')
        self.assertEqual(parse_cue(cue), self.dir / "A (USA) (v1.1).bin")

    def test_windows_line_ends_and_bom(self):
        cue = self.write('FILE "A.bin" BINARY\r\n  TRACK 01 MODE2/2352\r\n    INDEX 01 00:00:00\r\n', "utf-8-sig")
        self.assertEqual(parse_cue(cue), self.dir / "A.bin")

    def test_rejects_two_tracks(self):
        cue = self.write('FILE "A.bin" BINARY\n  TRACK 01 MODE2/2352\n  TRACK 02 AUDIO\n')
        with self.assertRaisesRegex(DiscError, "1 FILE and 1 TRACK"):
            parse_cue(cue)

    def test_rejects_other_mode(self):
        cue = self.write('FILE "A.bin" BINARY\n  TRACK 01 MODE1/2048\n')
        with self.assertRaisesRegex(DiscError, "MODE2/2352"):
            parse_cue(cue)


class DiscImageTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.image = b"".join(synth.sector(i, bytes([i]) * 2048) for i in range(3))
        self.image += synth.sector(3, b"\x07" * 2324, synth.SUBMODE_FORM2 | synth.SUBMODE_AUDIO)
        self.bin = self.dir / "d.bin"
        self.bin.write_bytes(self.image)

    def tearDown(self):
        self.tmp.cleanup()

    def test_reads_sectors(self):
        with DiscImage(self.bin) as disc:
            self.assertEqual(disc.sector_count, 4)
            self.assertEqual(disc.raw(1), self.image[2352:4704])
            self.assertEqual(disc.form1(2), b"\x02" * 2048)
            self.assertEqual(disc.form2_raw(3), self.image[3 * 2352 + 16:4 * 2352])
            self.assertEqual(disc.submode(0), synth.SUBMODE_DATA)
            self.assertEqual(disc.submode(3), synth.SUBMODE_FORM2 | synth.SUBMODE_AUDIO)

    def test_lba_outside_image(self):
        with DiscImage(self.bin) as disc:
            with self.assertRaisesRegex(DiscError, "LBA 4 outside image"):
                disc.raw(4)

    def test_size_not_multiple_of_sector(self):
        self.bin.write_bytes(self.image + b"\0")
        with self.assertRaisesRegex(DiscError, "not a multiple of 2352"):
            DiscImage(self.bin)

    def test_identity(self):
        good = hashlib.sha1(self.image).hexdigest()
        self.assertEqual(sha1_file(self.bin), good)
        check_identity(self.bin, good)
        with self.assertRaisesRegex(DiscError, f"sha1 {good} != expected 0+"):
            check_identity(self.bin, "0" * 40)


class ProblemTest(unittest.TestCase):
    def test_str(self):
        self.assertEqual(str(Problem("ROCK_X6.DAT TOC", "gap")), "ROCK_X6.DAT TOC: gap")


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 3: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_cdimage.py -v`
Expected: ERROR, `ModuleNotFoundError: No module named 'mmxdemake'`

- [ ] **Step 4: Write the implementation**

`mmxdemake/__init__.py`: empty file.

`mmxdemake/problem.py`:

```python
from dataclasses import dataclass


@dataclass(frozen=True)
class Problem:
    where: str
    message: str

    def __str__(self):
        return f"{self.where}: {self.message}"
```

`mmxdemake/cdimage.py`:

```python
import hashlib
from pathlib import Path

SECTOR_RAW = 2352
FORM1_DATA = 2048
FORM2_RAW = 2336
USER_OFFSET = 24
SUBHEADER_OFFSET = 16
SUBMODE_OFFSET = 18


class DiscError(Exception):
    pass


def parse_cue(cue_path):
    cue_path = Path(cue_path)
    text = cue_path.read_bytes().decode("utf-8-sig")
    lines = [line.strip() for line in text.splitlines() if line.strip()]
    files = [line for line in lines if line.upper().startswith("FILE ")]
    tracks = [line for line in lines if line.upper().startswith("TRACK ")]
    if len(files) != 1 or len(tracks) != 1:
        raise DiscError(f"{cue_path}: need exactly 1 FILE and 1 TRACK, found {len(files)} and {len(tracks)}")
    mode = tracks[0].split()[-1].upper()
    if mode != "MODE2/2352":
        raise DiscError(f"{cue_path}: track mode is {mode}, need MODE2/2352")
    line = files[0]
    first, last = line.find('"'), line.rfind('"')
    if first < 0 or last <= first:
        raise DiscError(f"{cue_path}: FILE line has no quoted name: {line}")
    return cue_path.parent / line[first + 1:last]


class DiscImage:
    def __init__(self, bin_path):
        self.path = Path(bin_path)
        size = self.path.stat().st_size
        if size % SECTOR_RAW:
            raise DiscError(f"{self.path}: size {size} is not a multiple of {SECTOR_RAW}")
        self.sector_count = size // SECTOR_RAW
        self._file = open(self.path, "rb")

    def close(self):
        self._file.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def raw(self, lba):
        if not 0 <= lba < self.sector_count:
            raise DiscError(f"LBA {lba} outside image (0..{self.sector_count - 1})")
        self._file.seek(lba * SECTOR_RAW)
        return self._file.read(SECTOR_RAW)

    def form1(self, lba):
        return self.raw(lba)[USER_OFFSET:USER_OFFSET + FORM1_DATA]

    def form2_raw(self, lba):
        return self.raw(lba)[SUBHEADER_OFFSET:SUBHEADER_OFFSET + FORM2_RAW]

    def submode(self, lba):
        return self.raw(lba)[SUBMODE_OFFSET]


def sha1_file(path):
    h = hashlib.sha1()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def check_identity(bin_path, expected_sha1):
    actual = sha1_file(bin_path)
    if actual != expected_sha1:
        raise DiscError(f"{bin_path}: sha1 {actual} != expected {expected_sha1}")
```

- [ ] **Step 5: Run the tests to verify they pass**

Run: `python3 -m unittest discover -s tests -p test_cdimage.py -v`
Expected: 9 tests, `OK`

- [ ] **Step 6: Commit**

```bash
git add mmxdemake/__init__.py mmxdemake/problem.py mmxdemake/cdimage.py tests/synth.py tests/test_cdimage.py
git commit -m "Read CUE files and raw disc sectors

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: `iso9660`

**Files:**
- Create: `mmxdemake/iso9660.py`
- Modify: `tests/synth.py` (add the ISO builder at the end)
- Test: `tests/test_iso9660.py`

**Interfaces:**
- Consumes: `cdimage.DiscImage`, `cdimage.DiscError`
- Produces:
  - `iso9660.IsoFile(path: str, lba: int, size: int, form: int, first_submode: int)`, frozen dataclass; `path` uses `/`, no `;1`
  - `iso9660.walk(disc: DiscImage) -> list[IsoFile]`, sorted by `path`
  - `iso9660.iter_file(disc: DiscImage, f: IsoFile) -> Iterator[bytes]` (one chunk per sector)
  - `iso9660.read_file(disc: DiscImage, f: IsoFile) -> bytes`
  - `synth.build_iso(entries: list[tuple]) -> tuple[bytes, dict[str, bytes]]`. Each entry is `(path, data, form)` or `(path, data, form, first_submode)`. Returns the image and, for each path, the bytes `read_file` must return.

- [ ] **Step 1: Add the ISO builder to `tests/synth.py`**

Append:

```python
XA_FORM1 = 0x0800
XA_FORM2 = 0x1000
XA_DIR = 0x8000


def _both16(v):
    return struct.pack("<H", v) + struct.pack(">H", v)


def _both32(v):
    return struct.pack("<I", v) + struct.pack(">I", v)


def dir_record(name, lba, size, is_dir, xa_attr=None):
    pad = b"\0" if len(name) % 2 == 0 else b""
    xa = b"" if xa_attr is None else struct.pack(">HHH", 0, 0, xa_attr) + b"XA" + b"\0" * 6
    body = (b"\0" + _both32(lba) + _both32(size) + b"\0" * 7 + bytes([2 if is_dir else 0, 0, 0])
            + _both16(1) + bytes([len(name)]) + name + pad + xa)
    return bytes([len(body) + 1]) + body


def build_iso(entries):
    dirs = sorted({e[0].split("/")[0] for e in entries if "/" in e[0]})
    dir_lba = {d: 19 + i for i, d in enumerate(dirs)}
    next_lba = 19 + len(dirs)
    sectors, expected, placed = {}, {}, []
    for entry in entries:
        path, data, form = entry[:3]
        first_submode = entry[3] if len(entry) > 3 else None
        lba = next_lba
        if form == 1:
            count = max(1, ceil(len(data) / 2048))
            for k in range(count):
                sm = first_submode if k == 0 and first_submode is not None else SUBMODE_DATA
                sectors[lba + k] = sector(lba + k, data[k * 2048:(k + 1) * 2048], sm)
            expected[path] = data
            size = len(data)
        else:
            count = max(1, ceil(len(data) / 2324))
            for k in range(count):
                default = SUBMODE_FORM2 | SUBMODE_AUDIO | SUBMODE_REALTIME
                sm = first_submode if k == 0 and first_submode is not None else default
                sectors[lba + k] = sector(lba + k, data[k * 2324:(k + 1) * 2324], sm)
            expected[path] = b"".join(sectors[lba + k][16:] for k in range(count))
            size = count * 2048
        placed.append((path, lba, size, form))
        next_lba += count

    def files_in(folder):
        out = b""
        for path, lba, size, form in placed:
            parent, _, name = path.rpartition("/")
            if parent == folder:
                attr = XA_FORM2 if form == 2 else XA_FORM1
                out += dir_record(name.encode() + b";1", lba, size, False, attr)
        return out

    root = dir_record(b"\x00", 18, 2048, True, XA_DIR) + dir_record(b"\x01", 18, 2048, True, XA_DIR)
    for d in dirs:
        root += dir_record(d.encode(), dir_lba[d], 2048, True, XA_DIR)
    root += files_in("")
    assert len(root) <= 2048
    sectors[18] = sector(18, root)
    for d in dirs:
        sub = dir_record(b"\x00", dir_lba[d], 2048, True, XA_DIR) + dir_record(b"\x01", 18, 2048, True, XA_DIR)
        sub += files_in(d)
        sectors[dir_lba[d]] = sector(dir_lba[d], sub)

    pvd = bytearray(2048)
    pvd[0] = 1
    pvd[1:6] = b"CD001"
    pvd[6] = 1
    pvd[8:19] = b"PLAYSTATION"
    pvd[156:190] = dir_record(b"\x00", 18, 2048, True)
    sectors[16] = sector(16, bytes(pvd))
    sectors[17] = sector(17, b"\xffCD001\x01")
    image = b"".join(sectors.get(lba, sector(lba)) for lba in range(next_lba))
    return image, expected
```

- [ ] **Step 2: Write the failing tests `tests/test_iso9660.py`**

```python
import tempfile
import unittest
from pathlib import Path

import synth
from mmxdemake.cdimage import DiscError, DiscImage
from mmxdemake.iso9660 import IsoFile, iter_file, read_file, walk

STR_SUBMODE = synth.SUBMODE_DATA | synth.SUBMODE_REALTIME


class IsoTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.cnf = b"BOOT=test\n"
        self.big = bytes(range(256)) * 12
        self.video = b"\x11" * 3000
        image, self.expected = synth.build_iso([
            ("SYSTEM.CNF", self.cnf, 1),
            ("A.BIN", self.big, 1),
            ("STR/V.STR", self.video, 2, STR_SUBMODE),
        ])
        self.bin = self.dir / "d.bin"
        self.bin.write_bytes(image)
        self.disc = DiscImage(self.bin)

    def tearDown(self):
        self.disc.close()
        self.tmp.cleanup()

    def test_walk_lists_files_sorted(self):
        files = walk(self.disc)
        self.assertEqual([f.path for f in files], ["A.BIN", "STR/V.STR", "SYSTEM.CNF"])
        a = files[0]
        self.assertEqual((a.size, a.form), (len(self.big), 1))

    def test_form_comes_from_directory_not_sector(self):
        v = next(f for f in walk(self.disc) if f.path == "STR/V.STR")
        self.assertEqual(v.form, 2)
        self.assertEqual(v.first_submode, STR_SUBMODE)
        self.assertEqual(v.size, 2 * 2048)

    def test_read_form1_truncates_to_size(self):
        for f in walk(self.disc):
            if f.form == 1:
                self.assertEqual(read_file(self.disc, f), self.expected[f.path])

    def test_read_form2_is_2336_per_sector(self):
        v = next(f for f in walk(self.disc) if f.path == "STR/V.STR")
        data = read_file(self.disc, v)
        self.assertEqual(len(data), 2 * 2336)
        self.assertEqual(data, self.expected["STR/V.STR"])

    def test_iter_file_matches_read_file(self):
        for f in walk(self.disc):
            self.assertEqual(b"".join(iter_file(self.disc, f)), read_file(self.disc, f))

    def test_no_volume_descriptor(self):
        raw = bytearray(self.bin.read_bytes())
        raw[16 * 2352 + 24 + 1:16 * 2352 + 24 + 6] = b"XXXXX"
        self.bin.write_bytes(bytes(raw))
        with DiscImage(self.bin) as disc:
            with self.assertRaisesRegex(DiscError, "no ISO9660 primary volume descriptor"):
                walk(disc)

    def test_isofile_is_frozen(self):
        f = IsoFile("A", 1, 2, 1, 8)
        with self.assertRaises(Exception):
            f.lba = 3


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 3: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_iso9660.py -v`
Expected: ERROR, `ModuleNotFoundError: No module named 'mmxdemake.iso9660'`

- [ ] **Step 4: Write `mmxdemake/iso9660.py`**

```python
import struct
from dataclasses import dataclass
from math import ceil

from mmxdemake.cdimage import FORM1_DATA, DiscError

PVD_LBA = 16
XA_FORM2 = 0x1000
FLAG_DIRECTORY = 0x02


@dataclass(frozen=True)
class IsoFile:
    path: str
    lba: int
    size: int
    form: int
    first_submode: int


def _u32(b, off):
    return struct.unpack_from("<I", b, off)[0]


def _read_extent(disc, lba, length):
    return b"".join(disc.form1(lba + i) for i in range(ceil(length / FORM1_DATA)))


def _xa_attributes(record, name_len):
    start = 33 + name_len + (1 if name_len % 2 == 0 else 0)
    su = record[start:]
    if len(su) >= 14 and su[6:8] == b"XA":
        return int.from_bytes(su[4:6], "big")
    return None


def walk(disc):
    pvd = disc.form1(PVD_LBA)
    if pvd[0] != 1 or pvd[1:6] != b"CD001":
        raise DiscError(f"LBA {PVD_LBA}: no ISO9660 primary volume descriptor")
    root = pvd[156:190]
    out = []
    _walk_dir(disc, _u32(root, 2), _u32(root, 10), "", out, set())
    return sorted(out, key=lambda f: f.path)


def _walk_dir(disc, lba, length, prefix, out, seen):
    if lba in seen:
        raise DiscError(f"directory loop at LBA {lba} ({prefix or '/'})")
    seen.add(lba)
    data = _read_extent(disc, lba, length)
    pos = 0
    while pos < length:
        n = data[pos]
        if n == 0:
            pos = (pos // FORM1_DATA + 1) * FORM1_DATA
            continue
        record = data[pos:pos + n]
        pos += n
        name_len = record[32]
        raw_name = record[33:33 + name_len]
        if raw_name in (b"\x00", b"\x01"):
            continue
        name = raw_name.decode("ascii").split(";")[0]
        entry_lba, entry_size, flags = _u32(record, 2), _u32(record, 10), record[25]
        if flags & FLAG_DIRECTORY:
            _walk_dir(disc, entry_lba, entry_size, prefix + name + "/", out, seen)
            continue
        attr = _xa_attributes(record, name_len)
        if attr is None:
            raise DiscError(f"{prefix}{name}: directory record has no XA attribute")
        form = 2 if attr & XA_FORM2 else 1
        out.append(IsoFile(prefix + name, entry_lba, entry_size, form, disc.submode(entry_lba)))


def iter_file(disc, f):
    count = ceil(f.size / FORM1_DATA)
    if f.form == 2:
        for k in range(count):
            yield disc.form2_raw(f.lba + k)
        return
    remaining = f.size
    for k in range(count):
        yield disc.form1(f.lba + k)[:min(FORM1_DATA, remaining)]
        remaining -= FORM1_DATA


def read_file(disc, f):
    return b"".join(iter_file(disc, f))
```

- [ ] **Step 5: Run the tests to verify they pass**

Run: `python3 -m unittest discover -s tests -v`
Expected: 16 tests, `OK`

- [ ] **Step 6: Commit**

```bash
git add mmxdemake/iso9660.py tests/synth.py tests/test_iso9660.py
git commit -m "Walk the ISO9660 file system and read files by XA form

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: `rockarc`

**Files:**
- Create: `mmxdemake/rockarc.py`
- Modify: `tests/synth.py` (add the archive builder at the end)
- Test: `tests/test_rockarc.py`

**Interfaces:**
- Consumes: `problem.Problem`
- Produces:
  - `rockarc.SECTOR = 2048`
  - `rockarc.Member(index: int, sector: int, size: int)`, frozen dataclass
  - `rockarc.parse_toc(header: bytes) -> list[Member]`
  - `rockarc.check_toc(name: str, members: list[Member], header: bytes, file_sectors: int, header_sectors: int = 2, expected_count: int | None = None) -> list[Problem]`
  - `rockarc.read_member(data: bytes, m: Member) -> bytes` (raises `ValueError` past the end)
  - `rockarc.tail_padding_is_zero(data: bytes, m: Member) -> bool`
  - `rockarc.observe_header(body: bytes, tail_zero: bool) -> dict` with keys `tail_zero`, and when `len(body) >= 8` also `word0`, `word1`, and `subentries: list[list[int]]` only when `0 < word0 < 64`, `word1 == len(body)` and `8 + 8 * word0 <= len(body)`
  - `synth.build_archive(payloads: list[bytes], header_sectors: int = 2) -> bytes`
  - `synth.payload(i: int, size: int) -> bytes`

- [ ] **Step 1: Add the archive builder to `tests/synth.py`**

Append:

```python
def payload(i, size):
    return bytes((i * 31 + k) % 251 + 1 for k in range(size))


def build_archive(payloads, header_sectors=2):
    header = bytearray(header_sectors * 2048)
    body = b""
    next_sector = header_sectors
    for i, p in enumerate(payloads):
        struct.pack_into("<II", header, i * 8, next_sector, len(p))
        count = ceil(len(p) / 2048)
        body += p.ljust(count * 2048, b"\0")
        next_sector += count
    return bytes(header) + body
```

- [ ] **Step 2: Write the failing tests `tests/test_rockarc.py`**

```python
import struct
import unittest

import synth
from mmxdemake.rockarc import (
    Member, check_toc, observe_header, parse_toc, read_member, tail_padding_is_zero,
)


def sectors(data):
    return -(-len(data) // 2048)


class TocTest(unittest.TestCase):
    def setUp(self):
        self.payloads = [synth.payload(0, 5000), synth.payload(1, 2048), synth.payload(2, 10)]
        self.data = synth.build_archive(self.payloads)
        self.header = self.data[:4096]

    def problems(self, data=None, header=None, expected_count=3):
        data = self.data if data is None else data
        header = data[:4096] if header is None else header
        return [str(p) for p in check_toc("T", parse_toc(header), header, sectors(data), expected_count=expected_count)]

    def test_parse(self):
        self.assertEqual(parse_toc(self.header), [Member(0, 2, 5000), Member(1, 5, 2048), Member(2, 6, 10)])

    def test_good_archive_has_no_problems(self):
        self.assertEqual(self.problems(), [])

    def test_gap(self):
        h = bytearray(self.header)
        struct.pack_into("<I", h, 16, 7)
        self.assertIn("T TOC: member 2 starts at sector 7, expected 6", self.problems(header=bytes(h)))

    def test_overlap(self):
        h = bytearray(self.header)
        struct.pack_into("<I", h, 8, 4)
        self.assertIn("T TOC: member 1 starts at sector 4, expected 5", self.problems(header=bytes(h)))

    def test_first_member_not_after_header(self):
        h = bytearray(self.header)
        struct.pack_into("<I", h, 0, 3)
        self.assertIn("T TOC: member 0 starts at sector 3, expected 2", self.problems(header=bytes(h)))

    def test_missing_terminator(self):
        header = struct.pack("<IIII", 2, 100, 3, 100)
        found = [str(p) for p in check_toc("T", parse_toc(header), header, 4)]
        self.assertIn("T TOC: no zero pair ends the list", found)

    def test_nonzero_header_tail(self):
        h = bytearray(self.header)
        h[100] = 1
        self.assertIn("T TOC: header byte 100 after the zero pair is not zero", self.problems(header=bytes(h)))

    def test_last_member_short_of_file_end(self):
        data = self.data + b"\0" * 2048
        self.assertIn("T TOC: last member ends at sector 7, file has 8 sectors", self.problems(data=data))

    def test_member_too_small(self):
        data = synth.build_archive([synth.payload(0, 100), synth.payload(1, 3)])
        self.assertIn("T TOC: member 1 size 3 is below 4", self.problems(data=data, expected_count=2))

    def test_count_mismatch(self):
        self.assertIn("T TOC: 3 members, expected 59", self.problems(expected_count=59))


class MemberTest(unittest.TestCase):
    def setUp(self):
        self.payloads = [synth.payload(0, 5000), synth.payload(1, 10)]
        self.data = synth.build_archive(self.payloads)
        self.members = parse_toc(self.data[:4096])

    def test_read_member(self):
        self.assertEqual([read_member(self.data, m) for m in self.members], self.payloads)

    def test_read_past_end(self):
        with self.assertRaisesRegex(ValueError, "past the end"):
            read_member(self.data, Member(9, 6, 5000))

    def test_tail_padding(self):
        self.assertTrue(tail_padding_is_zero(self.data, self.members[0]))
        d = bytearray(self.data)
        d[2 * 2048 + 5000] = 9
        self.assertFalse(tail_padding_is_zero(bytes(d), self.members[0]))


class ObserveTest(unittest.TestCase):
    def test_subentries_when_header_matches(self):
        body = struct.pack("<IIIIII", 2, 64, 5, 16, 7, 8) + bytes(40)
        self.assertEqual(observe_header(body, True),
                         {"tail_zero": True, "word0": 2, "word1": 64, "subentries": [[5, 16], [7, 8]]})

    def test_no_subentries_when_size_differs(self):
        body = struct.pack("<II", 1, 999) + bytes(56)
        self.assertEqual(observe_header(body, False), {"tail_zero": False, "word0": 1, "word1": 999})

    def test_no_subentries_when_list_runs_past_body(self):
        body = struct.pack("<II", 10, 16) + bytes(8)
        self.assertNotIn("subentries", observe_header(body, True))

    def test_short_body(self):
        self.assertEqual(observe_header(b"\1\2\3\4", True), {"tail_zero": True})


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 3: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_rockarc.py -v`
Expected: ERROR, `ModuleNotFoundError: No module named 'mmxdemake.rockarc'`

- [ ] **Step 4: Write `mmxdemake/rockarc.py`**

```python
import struct
from dataclasses import dataclass
from math import ceil

from mmxdemake.problem import Problem

SECTOR = 2048
MAX_SUBENTRIES = 64


@dataclass(frozen=True)
class Member:
    index: int
    sector: int
    size: int


def parse_toc(header):
    members = []
    for i in range(len(header) // 8):
        sector, size = struct.unpack_from("<II", header, i * 8)
        if sector == 0 and size == 0:
            break
        members.append(Member(i, sector, size))
    return members


def check_toc(name, members, header, file_sectors, header_sectors=2, expected_count=None):
    where = f"{name} TOC"
    problems = []
    terminator = len(members) * 8
    if terminator + 8 > len(header):
        problems.append(Problem(where, "no zero pair ends the list"))
    else:
        tail = header[terminator + 8:]
        nonzero = next((i for i, b in enumerate(tail) if b), None)
        if nonzero is not None:
            problems.append(Problem(where, f"header byte {terminator + 8 + nonzero} after the zero pair is not zero"))
    if expected_count is not None and len(members) != expected_count:
        problems.append(Problem(where, f"{len(members)} members, expected {expected_count}"))
    if not members:
        problems.append(Problem(where, "no members"))
        return problems
    if members[0].sector != header_sectors:
        problems.append(Problem(where, f"member 0 starts at sector {members[0].sector}, expected {header_sectors}"))
    for a, b in zip(members, members[1:]):
        expected = a.sector + ceil(a.size / SECTOR)
        if b.sector != expected:
            problems.append(Problem(where, f"member {b.index} starts at sector {b.sector}, expected {expected}"))
    last = members[-1]
    end = last.sector + ceil(last.size / SECTOR)
    if end != file_sectors:
        problems.append(Problem(where, f"last member ends at sector {end}, file has {file_sectors} sectors"))
    for m in members:
        if m.size < 4:
            problems.append(Problem(where, f"member {m.index} size {m.size} is below 4"))
    return problems


def read_member(data, m):
    start = m.sector * SECTOR
    if start + m.size > len(data):
        raise ValueError(f"member {m.index} runs past the end of the archive")
    return data[start:start + m.size]


def tail_padding_is_zero(data, m):
    start = m.sector * SECTOR + m.size
    end = (m.sector + ceil(m.size / SECTOR)) * SECTOR
    return not any(data[start:end])


def observe_header(body, tail_zero):
    observed = {"tail_zero": tail_zero}
    if len(body) < 8:
        return observed
    word0, word1 = struct.unpack_from("<II", body)
    observed["word0"], observed["word1"] = word0, word1
    if 0 < word0 < MAX_SUBENTRIES and word1 == len(body) and 8 + 8 * word0 <= len(body):
        observed["subentries"] = [list(struct.unpack_from("<II", body, 8 + 8 * k)) for k in range(word0)]
    return observed
```

- [ ] **Step 5: Run the tests to verify they pass**

Run: `python3 -m unittest discover -s tests -v`
Expected: 33 tests, `OK`

- [ ] **Step 6: Commit**

```bash
git add mmxdemake/rockarc.py tests/synth.py tests/test_rockarc.py
git commit -m "Parse and check sector,size archives and observe member headers

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: `index` and `verify`

**Files:**
- Create: `mmxdemake/index.py`, `mmxdemake/verify.py`
- Test: `tests/test_index_verify.py`

**Interfaces:**
- Consumes: `problem.Problem`
- Produces:
  - `index.SCHEMA_VERSION = 1`
  - `index.build_index(medium: dict, files: list[dict]) -> dict` (sorts `files` by `path`)
  - `index.dumps(index: dict) -> str` (sorted keys, indent 2, ends with `\n`)
  - `index.write_index(path: Path, index: dict) -> None`
  - `index.load_index(path: Path) -> dict` (raises `ValueError` on another schema version)
  - `verify.load_reference(path: Path) -> dict[str, tuple[str, int]]` (path → (sha1, size))
  - `verify.compare(index: dict, reference: dict) -> tuple[list[Problem], list[str]]` (problems, output paths with no reference)

- [ ] **Step 1: Write the failing tests `tests/test_index_verify.py`**

```python
import json
import tempfile
import unittest
from pathlib import Path

from mmxdemake.index import SCHEMA_VERSION, build_index, dumps, load_index, write_index
from mmxdemake.verify import compare, load_reference

MEDIUM = {"serial": "S", "revision": "r", "bin_sha1": "b" * 40}


def rec(path, sha1="a" * 40, size=10, kind="iso"):
    return {"kind": kind, "path": path, "sha1": sha1, "size": size}


class IndexTest(unittest.TestCase):
    def test_sorted_and_deterministic(self):
        a = build_index(MEDIUM, [rec("rock_dat/001.bin"), rec("iso/A")])
        b = build_index(MEDIUM, [rec("iso/A"), rec("rock_dat/001.bin")])
        self.assertEqual([r["path"] for r in a["files"]], ["iso/A", "rock_dat/001.bin"])
        self.assertEqual(dumps(a), dumps(b))
        self.assertTrue(dumps(a).endswith("}\n"))
        self.assertEqual(a["schema_version"], SCHEMA_VERSION)

    def test_round_trip_and_schema_check(self):
        with tempfile.TemporaryDirectory() as t:
            p = Path(t) / "index.json"
            idx = build_index(MEDIUM, [rec("iso/A")])
            write_index(p, idx)
            self.assertEqual(load_index(p), idx)
            p.write_text(json.dumps({"schema_version": 2, "files": []}))
            with self.assertRaisesRegex(ValueError, "schema_version 2"):
                load_index(p)


class VerifyTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.ref = Path(self.tmp.name) / "retail.jsonl"
        lines = [
            {"kind": "iso", "path": "iso/A", "sha1": "a" * 40, "size": 10},
            {"kind": "rock", "path": "rock/07.bin", "sha1": "c" * 40, "size": 4},
            {"kind": "rock-stored", "path": "rock/07.stored", "sha1": "d" * 40, "size": 4},
        ]
        self.ref.write_text("".join(json.dumps(x) + "\n" for x in lines) + "\n")

    def tearDown(self):
        self.tmp.cleanup()

    def test_load_reference_maps_rock_paths(self):
        self.assertEqual(load_reference(self.ref), {"iso/A": ("a" * 40, 10), "rock_bin/07.bin": ("c" * 40, 4)})

    def test_all_match(self):
        idx = build_index(MEDIUM, [rec("iso/A"), rec("rock_bin/07.bin", "c" * 40, 4, "rock_bin"),
                                   rec("rock_dat/000.bin", kind="rock_dat")])
        problems, unreferenced = compare(idx, load_reference(self.ref))
        self.assertEqual(problems, [])
        self.assertEqual(unreferenced, ["rock_dat/000.bin"])

    def test_mismatch_and_missing(self):
        idx = build_index(MEDIUM, [rec("iso/A", "e" * 40, 11)])
        problems = [str(p) for p in compare(idx, load_reference(self.ref))[0]]
        self.assertEqual(problems, [
            f"iso/A: size 11 != reference 10",
            f"iso/A: sha1 {'e' * 40} != reference {'a' * 40}",
            "rock_bin/07.bin: in reference, missing from output",
        ])


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_index_verify.py -v`
Expected: ERROR, `ModuleNotFoundError: No module named 'mmxdemake.index'`

- [ ] **Step 3: Write `mmxdemake/index.py`**

```python
import json
from pathlib import Path

SCHEMA_VERSION = 1


def build_index(medium, files):
    return {"schema_version": SCHEMA_VERSION, "medium": medium, "files": sorted(files, key=lambda r: r["path"])}


def dumps(index):
    return json.dumps(index, sort_keys=True, indent=2) + "\n"


def write_index(path, index):
    Path(path).write_text(dumps(index))


def load_index(path):
    index = json.loads(Path(path).read_text())
    version = index.get("schema_version")
    if version != SCHEMA_VERSION:
        raise ValueError(f"{path}: schema_version {version}, this tool reads {SCHEMA_VERSION}")
    return index
```

- [ ] **Step 4: Write `mmxdemake/verify.py`**

```python
import json
from pathlib import Path

from mmxdemake.problem import Problem


def load_reference(path):
    out = {}
    for line in Path(path).read_text().splitlines():
        if not line.strip():
            continue
        r = json.loads(line)
        if r["kind"] == "iso":
            out[r["path"]] = (r["sha1"], r["size"])
        elif r["kind"] == "rock":
            out["rock_bin/" + r["path"].split("/", 1)[1]] = (r["sha1"], r["size"])
    return out


def compare(index, reference):
    by_path = {r["path"]: r for r in index["files"]}
    problems = []
    for path, (sha1, size) in sorted(reference.items()):
        r = by_path.get(path)
        if r is None:
            problems.append(Problem(path, "in reference, missing from output"))
            continue
        if r["size"] != size:
            problems.append(Problem(path, f"size {r['size']} != reference {size}"))
        if r["sha1"] != sha1:
            problems.append(Problem(path, f"sha1 {r['sha1']} != reference {sha1}"))
    unreferenced = sorted(set(by_path) - set(reference))
    return problems, unreferenced
```

- [ ] **Step 5: Run the tests to verify they pass**

Run: `python3 -m unittest discover -s tests -v`
Expected: 38 tests, `OK`

- [ ] **Step 6: Commit**

```bash
git add mmxdemake/index.py mmxdemake/verify.py tests/test_index_verify.py
git commit -m "Write index.json and compare outputs to a reference manifest

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 5: `extract` (run and CLI)

**Files:**
- Create: `mmxdemake/extract.py`
- Test: `tests/test_extract.py`

**Interfaces:**
- Consumes: everything from Tasks 1–4.
- Produces:
  - `extract.MEDIUM: dict` (`serial`, `revision`, `bin_sha1`)
  - `extract.REPO_ROOT: Path`
  - `extract.run(cue: Path, out: Path, reference: Path | None = None, expected_sha1: str = MEDIUM["bin_sha1"], bin_members: int = 59, dat_members: int = 243, log=print) -> int`
  - `extract.main(argv: list[str] | None = None) -> int`
  - CLI: `python3 -m mmxdemake.extract --cue CUE --out DIR [--reference JSONL]`

- [ ] **Step 1: Write the failing tests `tests/test_extract.py`**

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
from mmxdemake import extract
from mmxdemake.extract import main, run

STR_SUBMODE = synth.SUBMODE_DATA | synth.SUBMODE_REALTIME


def dat_member_with_header(size):
    return struct.pack("<IIII", 1, size, 5, size - 16) + bytes(size - 16)


class ExtractTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.bin_payloads = [synth.payload(i, s) for i, s in enumerate([3000, 2048, 17])]
        self.dat_payloads = [dat_member_with_header(4096), synth.payload(9, 100)]
        self.make_disc(synth.build_archive(self.bin_payloads), synth.build_archive(self.dat_payloads))
        self.lines = []

    def tearDown(self):
        self.tmp.cleanup()

    def make_disc(self, rock_bin, rock_dat, include_dat=True):
        entries = [("SYSTEM.CNF", b"BOOT=x\n", 1), ("ROCK_X6.BIN", rock_bin, 1),
                   ("STR/CAPLOGO.STR", b"\x22" * 5000, 2, STR_SUBMODE)]
        if include_dat:
            entries.append(("ROCK_X6.DAT", rock_dat, 1))
        image, _ = synth.build_iso(entries)
        self.cue = synth.write_disc(self.dir, image)
        self.sha1 = hashlib.sha1(image).hexdigest()

    def go(self, out_name="out", reference=None, sha1=None):
        self.lines = []
        return run(self.cue, self.dir / out_name, reference, sha1 or self.sha1, 3, 2, self.lines.append)

    def reference_from(self, out, break_path=None):
        lines = []
        for r in json.loads((out / "index.json").read_text())["files"]:
            sha1 = "0" * 40 if r["path"] == break_path else r["sha1"]
            if r["kind"] == "iso":
                lines.append({"kind": "iso", "path": r["path"], "sha1": sha1, "size": r["size"]})
            elif r["kind"] == "rock_bin":
                lines.append({"kind": "rock", "path": "rock/" + r["path"].split("/")[1], "sha1": sha1, "size": r["size"]})
        p = self.dir / "ref.jsonl"
        p.write_text("".join(json.dumps(x) + "\n" for x in lines))
        return p

    def test_extracts_everything(self):
        self.assertEqual(self.go(), 0, self.lines)
        out = self.dir / "out"
        self.assertEqual([(out / f"rock_bin/{i:02d}.bin").read_bytes() for i in range(3)], self.bin_payloads)
        self.assertEqual([(out / f"rock_dat/{i:03d}.bin").read_bytes() for i in range(2)], self.dat_payloads)
        self.assertEqual((out / "iso/SYSTEM.CNF").read_bytes(), b"BOOT=x\n")
        files = {r["path"]: r for r in json.loads((out / "index.json").read_text())["files"]}
        self.assertEqual(len(files), 4 + 3 + 2)
        self.assertEqual(files["rock_dat/000.bin"]["observed"]["subentries"], [[5, 4080]])
        self.assertNotIn("subentries", files["rock_dat/001.bin"]["observed"])
        self.assertEqual(files["iso/STR/CAPLOGO.STR"]["form"], 2)
        self.assertEqual(files["iso/STR/CAPLOGO.STR"]["observed"], {"first_submode": STR_SUBMODE})
        self.assertEqual(files["iso/STR/CAPLOGO.STR"]["size"], 3 * 2336)
        self.assertTrue(any(line.startswith("note: STR/CAPLOGO.STR") for line in self.lines))
        self.assertIn("ROCK_X6.DAT: 2 members, TOC ok, 2/2 tail-zero (observation)", self.lines)
        self.assertFalse((self.dir / "out.tmp").exists())

    def test_second_run_gives_same_index(self):
        self.go()
        first = (self.dir / "out/index.json").read_bytes()
        self.assertEqual(self.go(), 0)
        self.assertEqual((self.dir / "out/index.json").read_bytes(), first)

    def test_reference_match(self):
        self.go("first")
        ref = self.reference_from(self.dir / "first")
        self.assertEqual(self.go(reference=ref), 0, self.lines)
        self.assertIn("verify: 7 reference hashes match; 2 outputs have no reference", self.lines)

    def test_reference_mismatch_fails_without_output(self):
        self.go("first")
        ref = self.reference_from(self.dir / "first", break_path="rock_bin/01.bin")
        self.assertEqual(self.go(reference=ref), 1)
        self.assertTrue(any("rock_bin/01.bin: sha1" in line for line in self.lines))
        self.assertFalse((self.dir / "out").exists())

    def test_identity_mismatch(self):
        self.assertEqual(self.go(sha1="0" * 40), 2)
        self.assertFalse((self.dir / "out.tmp").exists())

    def test_dat_gap_fails(self):
        dat = bytearray(synth.build_archive(self.dat_payloads))
        struct.pack_into("<I", dat, 8, 9)
        self.make_disc(synth.build_archive(self.bin_payloads), bytes(dat))
        self.assertEqual(self.go(), 1)
        self.assertIn("error: ROCK_X6.DAT TOC: member 1 starts at sector 9, expected 4", self.lines)
        self.assertFalse((self.dir / "out").exists())

    def test_bin_tail_must_be_zero(self):
        rock_bin = bytearray(synth.build_archive(self.bin_payloads))
        rock_bin[2 * 2048 + 3000] = 1
        self.make_disc(bytes(rock_bin), synth.build_archive(self.dat_payloads))
        self.assertEqual(self.go(), 1)
        self.assertIn("error: ROCK_X6.BIN member 0: bytes after size in last sector are not zero", self.lines)

    def test_missing_dat(self):
        self.make_disc(synth.build_archive(self.bin_payloads), None, include_dat=False)
        self.assertEqual(self.go(), 1)
        self.assertIn("error: ISO: ROCK_X6.DAT not found on disc", self.lines)

    def test_stale_tmp_is_removed(self):
        stale = self.dir / "out.tmp"
        stale.mkdir()
        (stale / "junk.bin").write_bytes(b"x")
        self.assertEqual(self.go(), 0)
        self.assertFalse((self.dir / "out/junk.bin").exists())

    def test_missing_bin(self):
        (self.dir / "Synth Disc (v1.1).bin").unlink()
        self.assertEqual(self.go(), 2)
        self.assertTrue(any("Synth Disc (v1.1).bin" in line for line in self.lines))

    def test_out_inside_repo_is_refused(self):
        inside = extract.REPO_ROOT / "out-test-should-not-exist"
        self.assertEqual(run(self.cue, inside, None, self.sha1, 3, 2, self.lines.append), 2)
        self.assertFalse(inside.exists())
        self.assertFalse(inside.with_name(inside.name + ".tmp").exists())

    def test_cli_expands_home(self):
        with mock.patch.dict(os.environ, {"HOME": str(self.dir)}), \
                mock.patch.object(extract, "MEDIUM", dict(extract.MEDIUM, bin_sha1=self.sha1)), \
                mock.patch.object(extract, "BIN_MEMBERS", 3), mock.patch.object(extract, "DAT_MEMBERS", 2):
            code = main(["--cue", "~/Synth Disc (v1.1).cue", "--out", "~/cli-out"])
        self.assertEqual(code, 0)
        self.assertTrue((self.dir / "cli-out/index.json").exists())


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_extract.py -v`
Expected: ERROR, `ImportError: cannot import name 'extract' from 'mmxdemake'`

- [ ] **Step 3: Write `mmxdemake/extract.py`**

```python
import argparse
import hashlib
import shutil
import sys
from math import ceil
from pathlib import Path

from mmxdemake.cdimage import FORM1_DATA, DiscError, DiscImage, check_identity, parse_cue
from mmxdemake.index import build_index, write_index
from mmxdemake.iso9660 import iter_file, walk
from mmxdemake.problem import Problem
from mmxdemake.rockarc import (
    SECTOR, check_toc, observe_header, parse_toc, read_member, tail_padding_is_zero,
)
from mmxdemake.verify import compare, load_reference

MEDIUM = {"serial": "SLUS-01395", "revision": "v1.1", "bin_sha1": "d4f7e08371027a87a3bf13311db5a4c56733f4ea"}
BIN_MEMBERS = 59
DAT_MEMBERS = 243
HEADER_SECTORS = 2
SUBMODE_FORM2 = 0x20
REPO_ROOT = Path(__file__).resolve().parents[1]


class StageFailed(Exception):
    def __init__(self, problems):
        super().__init__(f"{len(problems)} problems")
        self.problems = problems


def _inside(path, folder):
    try:
        path.resolve().relative_to(folder)
        return True
    except ValueError:
        return False


def _extract_iso(disc, tmp, log):
    records = []
    files = walk(disc)
    for f in files:
        dest = tmp / "iso" / f.path
        dest.parent.mkdir(parents=True, exist_ok=True)
        h = hashlib.sha1()
        written = 0
        with open(dest, "wb") as out:
            for chunk in iter_file(disc, f):
                out.write(chunk)
                h.update(chunk)
                written += len(chunk)
        records.append({
            "kind": "iso", "path": f"iso/{f.path}", "lba": f.lba, "sectors": ceil(f.size / FORM1_DATA),
            "form": f.form, "size": written, "sha1": h.hexdigest(),
            "observed": {"first_submode": f.first_submode},
        })
        if bool(f.first_submode & SUBMODE_FORM2) != (f.form == 2):
            log(f"note: {f.path}: directory says Form {f.form}, first sector submode {f.first_submode:#04x}")
    log(f"ISO: {len(files)} files")
    return records, {f.path for f in files}


def _split(tmp, name, kind, width, expected, hard_tail, log):
    data = (tmp / "iso" / name).read_bytes()
    header = data[:HEADER_SECTORS * SECTOR]
    members = parse_toc(header)
    problems = check_toc(name, members, header, ceil(len(data) / SECTOR), HEADER_SECTORS, expected)
    if problems:
        raise StageFailed(problems)
    (tmp / kind).mkdir()
    records, tail_bad = [], []
    for m in members:
        body = read_member(data, m)
        rel = f"{kind}/{m.index:0{width}d}.bin"
        (tmp / rel).write_bytes(body)
        tail_zero = tail_padding_is_zero(data, m)
        if not tail_zero:
            tail_bad.append(Problem(f"{name} member {m.index}", "bytes after size in last sector are not zero"))
        record = {"kind": kind, "path": rel, "index": m.index, "sector": m.sector, "size": m.size,
                  "sha1": hashlib.sha1(body).hexdigest()}
        if not hard_tail:
            record["observed"] = observe_header(body, tail_zero)
        records.append(record)
    if hard_tail and tail_bad:
        raise StageFailed(tail_bad)
    zero = len(members) - len(tail_bad)
    log(f"{name}: {len(members)} members, TOC ok, {zero}/{len(members)} tail-zero{'' if hard_tail else ' (observation)'}")
    return records


def run(cue, out, reference=None, expected_sha1=MEDIUM["bin_sha1"], bin_members=BIN_MEMBERS,
        dat_members=DAT_MEMBERS, log=print):
    out = Path(out)
    if _inside(out, REPO_ROOT):
        log(f"error: --out {out} is inside the repo {REPO_ROOT}; game data must stay outside it")
        return 2
    try:
        bin_path = parse_cue(cue)
        if not bin_path.exists():
            raise DiscError(f"{cue}: FILE names {bin_path}, which does not exist")
        check_identity(bin_path, expected_sha1)
    except (DiscError, OSError) as e:
        log(f"error: {e}")
        return 2
    log("identity: ok")
    tmp = out.with_name(out.name + ".tmp")
    if tmp.exists():
        shutil.rmtree(tmp)
    tmp.mkdir(parents=True)
    try:
        with DiscImage(bin_path) as disc:
            records, names = _extract_iso(disc, tmp, log)
        missing = [n for n in ("ROCK_X6.BIN", "ROCK_X6.DAT") if n not in names]
        if missing:
            raise StageFailed([Problem("ISO", f"{n} not found on disc") for n in missing])
        records += _split(tmp, "ROCK_X6.BIN", "rock_bin", 2, bin_members, True, log)
        records += _split(tmp, "ROCK_X6.DAT", "rock_dat", 3, dat_members, False, log)
    except StageFailed as e:
        for p in e.problems:
            log(f"error: {p}")
        return 1
    except DiscError as e:
        log(f"error: {e}")
        return 1
    medium = dict(MEDIUM, bin_sha1=expected_sha1)
    index = build_index(medium, records)
    write_index(tmp / "index.json", index)
    if reference is None:
        log("verify: no reference given; only structural checks ran")
    else:
        ref = load_reference(reference)
        problems, unreferenced = compare(index, ref)
        if problems:
            for p in problems:
                log(f"error: {p}")
            return 1
        log(f"verify: {len(ref)} reference hashes match; {len(unreferenced)} outputs have no reference")
    if out.exists():
        shutil.rmtree(out)
    tmp.rename(out)
    log(f"done: {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.extract")
    parser.add_argument("--cue", required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--reference")
    args = parser.parse_args(argv)
    reference = Path(args.reference).expanduser() if args.reference else None
    return run(Path(args.cue).expanduser(), Path(args.out).expanduser(), reference,
               MEDIUM["bin_sha1"], BIN_MEMBERS, DAT_MEMBERS)


if __name__ == "__main__":
    sys.exit(main())
```

- [ ] **Step 4: Run the tests to verify they pass**

Run: `python3 -m unittest discover -s tests -v`
Expected: 50 tests, `OK`

- [ ] **Step 5: Commit**

```bash
git add mmxdemake/extract.py tests/test_extract.py
git commit -m "Add the extract command that runs all stages and verifies outputs

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 6: Real-disc test, real run, and docs

**Files:**
- Create: `tests/test_retail.py`
- Modify: `README.md` (add a "Usage" section)
- Modify: `docs/process/process-log.md` (append the implementation step; this file is the user's rolling log)

**Interfaces:**
- Consumes: `extract.run`, `extract.MEDIUM`, `index.load_index`

- [ ] **Step 1: Write `tests/test_retail.py`**

```python
import os
import tempfile
import unittest
from pathlib import Path

from mmxdemake.extract import run
from mmxdemake.index import load_index

CUE = os.environ.get("MMX6_CUE")
REFERENCE = Path(os.environ.get(
    "MMX6_REFERENCE", "~/IdeaProjects/mmx-reference/mmx6/manifest/retail.jsonl")).expanduser()
WORK = Path(os.environ.get("MMX6_WORK", "~/mmx-data")).expanduser()


@unittest.skipUnless(CUE, "set MMX6_CUE to the v1.1 CUE to run")
class RetailTest(unittest.TestCase):
    def test_full_extraction(self):
        with tempfile.TemporaryDirectory(dir=WORK) as t:
            lines = []
            out = Path(t) / "retail"
            self.assertEqual(run(Path(CUE).expanduser(), out, REFERENCE, log=lines.append), 0, lines)
            files = load_index(out / "index.json")["files"]
            kinds = [r["kind"] for r in files]
            self.assertEqual((kinds.count("iso"), kinds.count("rock_bin"), kinds.count("rock_dat")), (9, 59, 243))
            self.assertIn("verify: 68 reference hashes match; 243 outputs have no reference", lines)
            first = (out / "index.json").read_bytes()
            self.assertEqual(run(Path(CUE).expanduser(), out, REFERENCE, log=lines.append), 0, lines)
            self.assertEqual((out / "index.json").read_bytes(), first)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run the whole suite without the disc**

Run: `python3 -m unittest discover -s tests -v`
Expected: 51 tests, `OK (skipped=1)`

- [ ] **Step 3: Run the real-disc test**

Run: `MMX6_CUE="$HOME/mmx-data/disc/Mega Man X6 (USA) (v1.1).cue" python3 -m unittest tests.test_retail -v`
Expected: 1 test, `OK`. If it fails, stop. Report the `error:` lines from the assertion message; do not change checks to make it pass.

- [ ] **Step 4: Do the real extraction**

Run:
```bash
python3 -m mmxdemake.extract \
  --cue "~/mmx-data/disc/Mega Man X6 (USA) (v1.1).cue" \
  --out ~/mmx-data/extracted/retail \
  --reference ~/IdeaProjects/mmx-reference/mmx6/manifest/retail.jsonl
```
Expected output lines, in order: `identity: ok`, one or more `note:` lines for `.STR` files, `ISO: 9 files`, `ROCK_X6.BIN: 59 members, TOC ok, 59/59 tail-zero`, `ROCK_X6.DAT: 243 members, TOC ok, N/243 tail-zero (observation)`, `verify: 68 reference hashes match; 243 outputs have no reference`, `done: …`. Exit code 0.

- [ ] **Step 5: Check that no game data is in the repo**

Run: `git status --short`
Expected: only `README.md`, `tests/test_retail.py` and `docs/process/` appear.

- [ ] **Step 6: Add a "Usage" section to `README.md`**

Append:

````markdown
## Usage

Run from the repo root. Python 3.10, no dependencies.

```
python3 -m mmxdemake.extract \
  --cue "~/mmx-data/disc/Mega Man X6 (USA) (v1.1).cue" \
  --out ~/mmx-data/extracted/retail \
  --reference ~/IdeaProjects/mmx-reference/mmx6/manifest/retail.jsonl
```

Output: `iso/` (the 9 disc files), `rock_bin/NN.bin` (59 overlays),
`rock_dat/NNN.bin` (243 data members) and `index.json`.

Tests: `python3 -m unittest discover -s tests`. The real-disc test runs only
with `MMX6_CUE` set to the CUE path.
````

- [ ] **Step 7: Append the step to `docs/process/process-log.md`**

Add a section before "Lessons so far", titled `## 9. Build the container extractor (task A)`, with: what we built (the 6 modules, one per disc layer), why (each layer has its own check), the result (the summary lines from Step 4 with the real `N` for DAT tail-zero), the evidence (test counts from Steps 2 and 3, commit hashes from Tasks 1–6), and any surprise met while building.

- [ ] **Step 8: Commit**

```bash
git add README.md tests/test_retail.py docs/process/process-log.md docs/process/disc-image-explained.md
git commit -m "Add the real-disc test, usage notes and the process docs

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```
