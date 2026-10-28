# Sprite Export Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** From one save state per stage area, export every animated stage object type as Maverick-style animation strips plus `animations.json`, named by a content fingerprint, merged into one output root across runs, with official names applied later from a names file.

**Architecture:** `sprites` reads banks and sprite definitions from RAM and draws a sprite from VRAM (with the drawer's tile remap). `anims` reads animation tables and walks them. `animfind` finds the stage overlay and each type's table from live objects and from the overlay code. `fingerprint` hashes the raw content. `spriteexport` is the command: it collects, writes to `<root>.tmp`, and moves the items it owns into `<root>`.

**Tech Stack:** Python 3.10 standard library only.

**Spec:** `docs/superpowers/specs/2026-10-20-sprite-export-design.md`

## Global Constraints

- Python 3.10, standard library only. Tests: `python3 -m unittest discover -s tests` from the repo root; one file: `python3 -m unittest discover -s tests -p <file>`.
- Baseline before Task 1: `Ran 256 tests`, `OK (skipped=6)`.
- RAM addresses are masked with `& 0x1FFFFF` (`savestate.ram_at`).
- Scratchpad: decompressed state offset `RAM_OFFSET + RAM_SIZE + 0x80000` (= `0x289025`), `0x400` bytes.
- Remap (byte 0 & `0x60`): `0x60` → `((t >> 4) << 8) + (t & 15) + 0x600`; `0x40` → `q = t // 160; (q << 8) + t − 160q + 0x610`; `0x20` → `q = t // 80; (q << 8) + t − 80q + 0x5B0`; `0` → `t`. Stage enemies use `0x40`.
- Bank `i` CLUT word = `((482 + i // 4) << 6) | (4 * (i % 4))`.
- First piece on top. Facing 0. A colour word of 0 is transparent.
- Animation frame `{u8 ticks, u8 flag, s8 next, u8 sprite}`; ticks 0 → 256; next 0 → hold.
- Content ID: `enemy-` + first 10 hex digits of SHA-1 of the canonical JSON (`sort_keys=True`, `separators=(",", ":")`).
- Exit codes: 0 done; 1 failed check (nothing in `<root>` changed); 2 bad arguments, refused root or malformed catalog.
- No game data in the repo. No code comments. No code copied from reference repos.
- Commit to `master`. End each commit message with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Spec deltas (decided while planning)

- `stores`: the constant may come through another register: `addiu rX, rS, lo` with the last earlier write of `rS` being `lui rS, hi` (the spec's `addiu rX, rX` is the case `rS = rX`). This only adds matches; Task 6 checks that the four intro tables still come out.
- `owners` takes `(ram, func, refs, entries, lo, hi)`: it needs RAM and the overlay bounds to walk a data run. `entries` maps a function to a set of types.
- A type `< 0x80` whose update function is in neither the overlay nor the main program is recorded in `skipped` with the reason `update function is in neither the stage overlay nor the main program`.
- A table that fails its checks is skipped alone; its `skipped` entry also has `"table"`.

## Review Focus

1. A real bank can hold a sprite whose cell or palette is outside VRAM; the whole run then exits 1. Expected: a clear message naming bank and sprite. Task 6 runs both retail states and shows whether this happens.
2. Two runs that name the same folder for different IDs (a names-file edit) must refuse before any file moves. Test: Task 5 `test_folder_conflict_refuses`.
3. A partial publish (an `OSError` while moving) must say which root and that a re-run repairs it. Covered by the message in `publish`; no test (needs a failing filesystem).
4. A table found by search that points at garbage must skip that type, not stop the run; a catalog table that points at garbage must stop the run. Tests: Task 5 `test_catalog_bad_table_exit_1`, `test_bad_search_table_skipped`.
5. Re-running the same state must not change bytes or duplicate sources. Test: Task 5 `test_rerun_same_bytes`.
6. The main-program search can link a type to tables that belong to another object (a shared helper). The user accepts duplicates; Task 6 checks by eye that the exports of Yammark type 15 show one object.

---

## File structure

```
mmxdemake/
  sprites.py  anims.py  animfind.py  fingerprint.py  spriteexport.py   # new
catalog/
  enemies.json                                                        # new
tests/
  spritefix.py                                                        # new (fixtures)
  test_sprites.py  test_anims.py  test_animfind.py
  test_fingerprint.py  test_spriteexport.py  test_retail_sprites.py   # new
  synth.py                                                            # modified
README.md  docs/TODO.md  docs/process/process-log.md                  # modified
```

---

### Task 1: fixtures and `sprites`

**Files:**
- Modify: `tests/synth.py`
- Create: `tests/spritefix.py`, `tests/test_sprites.py`, `mmxdemake/sprites.py`

**Interfaces:**
- Produces: `synth.build_state(ram, header=..., vram=None, scratch=None)`; `spritefix.build(stage=0, area=0, live=True, colour=RED, loaded=2, twin=False) -> (ram, scratch, vram, overlay)` and the constants and code helpers below; `sprites.Bank`, `Image`, `SpriteError`, `scratchpad`, `stage_area`, `type_list`, `clut_word`, `banks`, `bank_of_type`, `pieces`, `remap`, `cell`, `cell_bytes`, `palette_words`, `loaded`, `draw`, `ENEMY_MODE`.

- [ ] **Step 1: Extend `synth.build_state`**

Replace the function in `tests/synth.py`:

```python
def build_state(ram, header=b"STv4 PCSXR v1.9.94", vram=None, scratch=None):
    data = header.ljust(32, b"\0") + b"\0" * (0x9025 - 32) + ram
    if scratch is not None:
        data += b"\0" * (0x9025 + 0x200000 + 0x80000 - len(data)) + scratch
    if vram is not None:
        data += b"\0" * (0x29B749 - 0x408 - len(data)) + struct.pack("<I", 1) + b"\0" * 0x404 + vram
    return gzip.compress(data + b"\0" * 64)
```

- [ ] **Step 2: Write `tests/spritefix.py`**

```python
import struct
from pathlib import Path

import synth

OV = 0x800E9860
OV_SIZE = 0x4000
TYPE_LIST = 0x80100000
BASE = 0x80110000
MAIN = OV + 0x3000
FUNC1 = OV + 0x100
FUNC2 = OV + 0x200
FRAMES1 = OV + 0x2000
TABLE1 = OV + 0x2100
FRAMES2 = OV + 0x2200
TABLE2 = OV + 0x2300
SLOT0 = 0x8008EF48
EXE_FUNC = 0x80010100
EXE_FRAMES = 0x80010700
EXE_TABLE = 0x80010800
EXE_TEXT = bytes((i * 7 + 3) % 256 for i in range(0x400))
EXE = bytes(0x1800) + EXE_TEXT
RED, GREEN, BLUE, WHITE = 0x001F, 0x03E0, 0x7C00, 0x7FFF
V0, A0, A1 = 2, 4, 5
JR_RA = 0x03E00008
NOP = 0


def put(buf, addr, data):
    o = addr & 0x1FFFFF
    buf[o:o + len(data)] = data


def word(buf, addr, value):
    put(buf, addr, struct.pack("<I", value & 0xFFFFFFFF))


def lui(rt, imm):
    return (15 << 26) | (rt << 16) | (imm & 0xFFFF)


def addiu(rt, rs, imm):
    return (9 << 26) | (rs << 21) | (rt << 16) | (imm & 0xFFFF)


def sw(rt, offset, base):
    return (0x2B << 26) | (base << 21) | (rt << 16) | (offset & 0xFFFF)


def jal(target):
    return (3 << 26) | ((target >> 2) & 0x3FFFFFF)


def addu(rd, rs, rt):
    return (rs << 21) | (rt << 16) | (rd << 11) | 0x21


PROLOGUE = addiu(29, 29, -24)


def la(rt, addr, rs=None):
    lo = addr & 0xFFFF
    hi = ((addr >> 16) + (1 if lo & 0x8000 else 0)) & 0xFFFF
    rs = rt if rs is None else rs
    return [lui(rs, hi), addiu(rt, rs, lo)]


def code(buf, addr, words):
    for k, w in enumerate(words):
        word(buf, addr + 4 * k, w)


def setter(table):
    return [PROLOGUE] + la(V0, table) + [sw(V0, 0x30, A0), JR_RA, NOP]


def clut_xy(i):
    return 16 * (4 * (i % 4)), 482 + i // 4


def put_cell(vram, page, u, v, f):
    for y in range(16):
        for x in range(0, 16, 2):
            o = ((page >> 4) * 256 + v + y) * 2048 + ((page & 15) * 64 + u // 4) * 2 + x // 2
            vram[o] = f(x, y) | (f(x + 1, y) << 4)


def put_palette(vram, i, words):
    x, y = clut_xy(i)
    struct.pack_into(f"<{len(words)}H", vram, y * 2048 + x * 2, *words)


def piece(flags, tile, dx, dy):
    return struct.pack("<BBbb", flags, tile, dx, dy)


def frame(ticks, flag, step, sprite):
    return struct.pack("<BBbB", ticks, flag, step, sprite)


def cell0(x, y):
    return 2 if x == 0 else (4 if y == 0 else 1)


def build(stage=0, area=0, live=True, colour=RED, loaded=2, twin=False, shared=False):
    ram = bytearray(0x200000)
    put(ram, 0x80011000, EXE_TEXT)
    put(ram, 0x800CCEDC, bytes([stage, area]))
    word(ram, 0x800733C4 + 4 * (stage * 2 + area), TYPE_LIST)
    put(ram, TYPE_LIST, bytes([0x80, 1, 2, 0xFF]))
    loaded = 3 if twin or shared else loaded
    table = BASE - 4 * loaded
    word(ram, 0x80095098, table)
    for i, v in enumerate([0, 0x800, 0x800 if twin else 0x1000][:loaded]):
        word(ram, table + 4 * i, v)
    for i, off in enumerate([12, 0x40, 0x80]):
        word(ram, BASE + 4 * i, off)
    put(ram, BASE + 12, struct.pack("<HH", 0, 1))
    bank1 = struct.pack("<HHHH", 2, 2, 1, 4) + piece(0, 0, -16, -16) + piece(0, 1, 0, -16) + piece(0x40, 0, -8, -8)
    put(ram, BASE + 0x40, bank1)
    put(ram, BASE + 0x80, bank1 if twin else struct.pack("<HH", 1, 1) + piece(0, 0, 0, 0))
    code(ram, FUNC1, setter(TABLE1))
    code(ram, FUNC2, setter(TABLE1 if twin else TABLE2))
    word(ram, MAIN + 4, FUNC1)
    word(ram, MAIN + 8, FUNC2)
    put(ram, FRAMES1, frame(6, 0, 1, 0) + frame(4, 1, -1, 1) + frame(0, 2, 0, 1))
    word(ram, TABLE1, FRAMES1)
    word(ram, TABLE1 + 4, FRAMES1 + 8)
    put(ram, FRAMES2, frame(1, 0, 0, 0))
    word(ram, TABLE2, FRAMES2)
    if shared:
        code(ram, EXE_FUNC, setter(EXE_TABLE))
        put(ram, EXE_FRAMES, frame(1, 0, 0, 0))
        word(ram, EXE_TABLE, EXE_FRAMES)
        word(ram, MAIN + 8, EXE_FUNC)
    if live:
        put(ram, SLOT0, bytes([0x41, 1]))
        word(ram, SLOT0 + 0x30, TABLE1)
    scratch = bytearray(0x400)
    struct.pack_into("<I", scratch, 0x20, BASE)
    struct.pack_into("<I", scratch, 0x80, MAIN)
    vram = bytearray(0x100000)
    put_cell(vram, 6, 0, 32, cell0)
    put_cell(vram, 6, 16, 32, lambda x, y: 3)
    put_cell(vram, 0, 0, 16, lambda x, y: 3)
    colours = [0, colour, GREEN, BLUE, WHITE]
    put_palette(vram, 1, colours)
    if twin:
        put_palette(vram, 2, colours)
    o = OV & 0x1FFFFF
    return bytes(ram), bytes(scratch), bytes(vram), bytes(ram[o:o + OV_SIZE])


def write_world(folder, states, overlay):
    folder = Path(folder)
    extracted, dumps = folder / "extracted", folder / "dumps"
    (extracted / "iso").mkdir(parents=True)
    (extracted / "rock_bin").mkdir()
    dumps.mkdir()
    (extracted / "iso/SLUS_013.95").write_bytes(EXE)
    (extracted / "rock_bin/02.bin").write_bytes(overlay)
    (extracted / "rock_bin/03.bin").write_bytes(b"\x01" * 0x900)
    for name, (ram, scratch, vram) in states.items():
        (dumps / name).write_bytes(synth.build_state(ram, vram=vram, scratch=scratch))
    return extracted, dumps
```

- [ ] **Step 3: Write the failing tests `tests/test_sprites.py`**

```python
import gzip
import struct
import unittest

import spritefix as F
import synth
from mmxdemake.sprites import (Bank, SpriteError, bank_of_type, banks, cell, clut_word, draw, palette_words,
                               scratchpad, stage_area)

G, R, B, W, C = ((0, 255, 0, 255), (255, 0, 0, 255), (0, 0, 255, 255), (255, 255, 255, 255), (0, 0, 0, 0))


def row(*parts):
    out = []
    for colour, n in parts:
        out += [colour] * n
    return out


class ScratchpadTest(unittest.TestCase):
    def test_reads_scratchpad(self):
        ram, scratch, vram, _ = F.build()
        state = gzip.decompress(synth.build_state(ram, vram=vram, scratch=scratch))
        self.assertEqual(scratchpad(state), scratch)

    def test_too_short(self):
        with self.assertRaisesRegex(SpriteError, "too short for the scratchpad at 0x289025"):
            scratchpad(b"\0" * 0x100)


class BanksTest(unittest.TestCase):
    def setUp(self):
        self.ram, self.scratch, self.vram, _ = F.build()

    def test_banks(self):
        self.assertEqual(stage_area(self.ram), (0, 0))
        self.assertEqual(banks(self.ram, self.scratch), [
            Bank(0, 0x80, F.BASE + 12, 1, 0, 0x7880),
            Bank(1, 1, F.BASE + 0x40, 2, 0x10, 0x7884),
            Bank(2, 2, F.BASE + 0x80, 1, None, 0x7888),
        ])

    def test_clut_words(self):
        self.assertEqual([clut_word(i) for i in (0, 3, 4, 8, 13)], [0x7880, 0x788C, 0x78C0, 0x7900, 0x7944])

    def test_bank_of_type(self):
        bank_list = banks(self.ram, self.scratch)
        self.assertEqual(bank_of_type(bank_list, 2).index, 2)
        self.assertIsNone(bank_of_type(bank_list, 9))

    def test_no_type_list(self):
        ram = bytearray(self.ram)
        F.word(ram, 0x800733C4, 0)
        with self.assertRaisesRegex(SpriteError, "stage 0 area 0 has no type list"):
            banks(bytes(ram), self.scratch)

    def test_tables_disagree(self):
        ram = bytearray(self.ram)
        F.word(ram, 0x80095098, F.BASE + 4)
        with self.assertRaisesRegex(SpriteError, "do not agree"):
            banks(bytes(ram), self.scratch)


class CellTest(unittest.TestCase):
    def test_modes(self):
        self.assertEqual(cell(0, 159, 0, 0x40), (6, 240, 160))
        self.assertEqual(cell(0, 160, 0, 0x40), (7, 0, 16))
        self.assertEqual(cell(1, 0x23, 0, 0x60), (0x18, 48, 0))
        self.assertEqual(cell(0, 81, 0, 0x20), (6, 16, 176))
        self.assertEqual(cell(2, 0x34, 0, 0), (2, 64, 48))

    def test_wide(self):
        self.assertEqual(cell(0x21, 5, 0x10, 0x40), (1, 80, 16))

    def test_outside(self):
        with self.assertRaisesRegex(SpriteError, "texture cell 0x2010 is outside the 4 bpp pages"):
            cell(0, 0, 0x2010, 0)

    def test_palette_outside(self):
        with self.assertRaisesRegex(SpriteError, "palette 0x8000 is outside VRAM"):
            palette_words(bytes(0x100000), 512 << 6)


class DrawTest(unittest.TestCase):
    def setUp(self):
        ram, self.scratch, vram, _ = F.build()
        self.ram, self.vram = bytearray(ram), bytearray(vram)

    def bank(self, i=1):
        return banks(bytes(self.ram), self.scratch)[i]

    def draw(self, n, bank=None):
        return draw(bytes(self.ram), bytes(self.vram), bank or self.bank(), n)

    def test_two_pieces(self):
        im = self.draw(0)
        self.assertEqual((im.width, im.height, im.ox, im.oy), (32, 16, 16, 16))
        self.assertEqual(im.pixels[0], row((G, 1), (W, 15), (B, 16)))
        self.assertEqual(im.pixels[5], row((G, 1), (R, 15), (B, 16)))

    def test_mirror(self):
        im = self.draw(1)
        self.assertEqual((im.width, im.height, im.ox, im.oy), (16, 16, 8, 8))
        self.assertEqual(im.pixels[0], row((W, 15), (G, 1)))
        self.assertEqual(im.pixels[1], row((R, 15), (G, 1)))

    def test_flip(self):
        F.put(self.ram, F.BASE + 0x50, F.piece(0x80, 0, -8, -8))
        im = self.draw(1)
        self.assertEqual(im.pixels[0], row((G, 1), (R, 15)))
        self.assertEqual(im.pixels[15], row((G, 1), (W, 15)))

    def test_wide(self):
        F.put(self.ram, F.BASE + 0x50, F.piece(0x20, 0, -8, -8))
        im = self.draw(1)
        self.assertEqual((im.width, im.height, im.ox, im.oy), (16, 16, 16, 8))
        self.assertEqual(im.pixels[0], row((B, 16)))
        F.put(self.ram, F.BASE + 0x50, F.piece(0xA0, 0, -8, -8))
        self.assertEqual(self.draw(1).ox, 15)

    def test_first_piece_on_top(self):
        F.put(self.ram, F.BASE + 0x4C, F.piece(0, 1, -8, -16))
        im = self.draw(0)
        self.assertEqual(im.width, 24)
        self.assertEqual(im.pixels[5], row((G, 1), (R, 15), (B, 8)))

    def test_colour_zero_transparent(self):
        x, y = F.clut_xy(1)
        struct.pack_into("<H", self.vram, y * 2048 + x * 2 + 4, 0)
        im = self.draw(0)
        self.assertEqual((im.width, im.ox), (31, 15))

    def test_empty_sprite(self):
        im = self.draw(0, self.bank(0))
        self.assertEqual(im, (1, 1, 0, 0, [[C]]))

    def test_no_texture(self):
        with self.assertRaisesRegex(SpriteError, r"bank 2 \(type 2\) has no texture in VRAM"):
            self.draw(0, self.bank(2))

    def test_out_of_range(self):
        with self.assertRaisesRegex(SpriteError, r"bank 1: sprite 2 is out of range \(count 2\)"):
            self.draw(2)

    def test_cell_outside(self):
        with self.assertRaisesRegex(SpriteError, "outside the 4 bpp pages"):
            self.draw(0, self.bank()._replace(texture=0x3000))


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 4: Run to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_sprites.py`
Expected: ERROR, `ModuleNotFoundError: No module named 'mmxdemake.sprites'`.

- [ ] **Step 5: Write `mmxdemake/sprites.py`**

```python
import struct
from collections import namedtuple

from mmxdemake.psxcolor import to_rgba
from mmxdemake.savestate import RAM_OFFSET, RAM_SIZE, ram_at

STAGE = 0x800CCEDC
AREA = 0x800CCEDD
TYPE_LISTS = 0x800733C4
TEXTURE_TABLE = 0x80095098
BANK_BASE = 0x20
SCRATCHPAD_OFFSET = RAM_OFFSET + RAM_SIZE + 0x80000
SCRATCHPAD_SIZE = 0x400
LIST_END = 0xFF
MAX_TYPES = 255
MAX_TEXTURES = 64
CLUT_Y0 = 482
ENEMY_MODE = 0x40
CELL = 16
CELL_BYTES = 8
ROW = 2048
VRAM_H = 512
PAGES = 32
WIDE = 0x20
MIRROR = 0x40
FLIP = 0x80
CLEAR = (0, 0, 0, 0)

Bank = namedtuple("Bank", "index type data count texture clut")
Image = namedtuple("Image", "width height ox oy pixels")


class SpriteError(Exception):
    pass


def _u8(ram, addr):
    return ram_at(ram, addr, 1)[0]


def _u16(ram, addr):
    return struct.unpack("<H", ram_at(ram, addr, 2))[0]


def _u32(ram, addr):
    return struct.unpack("<I", ram_at(ram, addr, 4))[0]


def scratchpad(state):
    data = state[SCRATCHPAD_OFFSET:SCRATCHPAD_OFFSET + SCRATCHPAD_SIZE]
    if len(data) != SCRATCHPAD_SIZE:
        raise SpriteError(f"save state is {len(state)} bytes, too short for the scratchpad at {SCRATCHPAD_OFFSET:#x}")
    return data


def stage_area(ram):
    return _u8(ram, STAGE), _u8(ram, AREA)


def type_list(ram):
    stage, area = stage_area(ram)
    ptr = _u32(ram, TYPE_LISTS + 4 * (stage * 2 + area))
    if not ptr:
        raise SpriteError(f"stage {stage} area {area} has no type list")
    types = []
    while len(types) < MAX_TYPES:
        t = _u8(ram, ptr + len(types))
        if t == LIST_END:
            return types
        types.append(t)
    raise SpriteError(f"type list at {ptr:#x} has no end marker")


def clut_word(i):
    return ((CLUT_Y0 + i // 4) << 6) | (4 * (i % 4))


def banks(ram, scratch):
    base = struct.unpack_from("<I", scratch, BANK_BASE)[0]
    table = _u32(ram, TEXTURE_TABLE)
    if base < table or (base - table) % 4 or (base - table) // 4 > MAX_TEXTURES:
        raise SpriteError(f"texture table {table:#x} and sprite bank base {base:#x} do not agree")
    textures = (base - table) // 4
    entries = _u32(ram, base) // 4
    out = []
    for i, t in enumerate(type_list(ram)[:entries]):
        data = base + _u32(ram, base + 4 * i)
        texture = _u32(ram, table + 4 * i) >> 7 if i < textures else None
        out.append(Bank(i, t, data, _u16(ram, data + 2), texture, clut_word(i)))
    return out


def bank_of_type(bank_list, t):
    return next((b for b in bank_list if b.type == t), None)


def pieces(ram, bank, n):
    if not 0 <= n < bank.count:
        raise SpriteError(f"bank {bank.index}: sprite {n} is out of range (count {bank.count})")
    count = _u16(ram, bank.data + 4 * n)
    offset = _u16(ram, bank.data + 4 * n + 2)
    return [struct.unpack("<BBbb", ram_at(ram, bank.data + 4 * (offset + j), 4)) for j in range(count)]


def remap(t, mode):
    if mode == 0x60:
        return ((t >> 4) << 8) + (t & 15) + 0x600
    if mode == 0x40:
        q = t // 160
        return (q << 8) + t - 160 * q + 0x610
    if mode == 0x20:
        q = t // 80
        return (q << 8) + t - 80 * q + 0x5B0
    return t


def cell(flags, tile, texture, mode):
    if flags & WIDE:
        t = ((tile | (flags & 1) << 8) + texture) & 0xFFFF
    else:
        t = remap(((tile | (flags & 3) << 8) + texture) & 0xFFFF, mode) & 0xFFFF
    if t >> 8 >= PAGES:
        raise SpriteError(f"texture cell {t:#x} is outside the 4 bpp pages")
    return t >> 8, (t & 15) * CELL, ((t >> 4) & 15) * CELL


def cell_bytes(vram, page, u, v):
    x = (page & 15) * 64 + u // 4
    y = (page >> 4) * 256 + v
    return b"".join(vram[(y + r) * ROW + x * 2:(y + r) * ROW + x * 2 + CELL_BYTES] for r in range(CELL))


def palette_words(vram, clut):
    x, y = (clut & 63) * 16, clut >> 6
    if y >= VRAM_H:
        raise SpriteError(f"palette {clut:#x} is outside VRAM")
    return list(struct.unpack_from("<16H", vram, y * ROW + x * 2))


def loaded(bank):
    if bank.texture is None:
        raise SpriteError(f"bank {bank.index} (type {bank.type}) has no texture in VRAM")
    return bank.texture


def _texel(data, x, y):
    return (data[y * CELL_BYTES + x // 2] >> (4 * (x & 1))) & 15


def draw(ram, vram, bank, n, mode=ENEMY_MODE):
    texture = loaded(bank)
    canvas = {}
    for flags, tile, dx, dy in reversed(pieces(ram, bank, n)):
        page, u, v = cell(flags, tile, texture, mode)
        data = cell_bytes(vram, page, u, v)
        colours = palette_words(vram, bank.clut + ((flags >> 2) & 3))
        wide = flags & WIDE
        x0 = 2 * dx + (1 if flags & FLIP else 0) if wide else dx
        for yy in range(CELL):
            sy = 15 - yy if flags & FLIP and not wide else yy
            for xx in range(CELL):
                sx = 15 - xx if flags & MIRROR and not wide else xx
                c = colours[_texel(data, sx, sy)]
                if c:
                    canvas[(x0 + xx, dy + yy)] = to_rgba(c)
    if not canvas:
        return Image(1, 1, 0, 0, [[CLEAR]])
    left = min(x for x, _ in canvas)
    top = min(y for _, y in canvas)
    width = max(x for x, _ in canvas) - left + 1
    height = max(y for _, y in canvas) - top + 1
    pixels = [[canvas.get((left + x, top + y), CLEAR) for x in range(width)] for y in range(height)]
    return Image(width, height, -left, -top, pixels)
```

- [ ] **Step 6: Run the tests**

Run: `python3 -m unittest discover -s tests -p test_sprites.py`
Expected: `Ran 21 tests`, `OK`.

- [ ] **Step 7: Run the whole suite**

Run: `python3 -m unittest discover -s tests 2>&1 | tail -3`
Expected: `Ran 277 tests`, `OK (skipped=6)`.

- [ ] **Step 8: Commit**

```bash
git add tests/synth.py tests/spritefix.py tests/test_sprites.py mmxdemake/sprites.py
git commit -m "Add sprite banks and the sprite drawer with the tile remap

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: `anims`

**Files:**
- Create: `tests/test_anims.py`, `mmxdemake/anims.py`

**Interfaces:**
- Consumes: `spritefix.put`, `word`, `frame`, `OV`, `OV_SIZE`.
- Produces: `anims.AnimError`, `read_table(ram, addr, lo, hi) -> list[int]`, `walk(ram, ptr) -> (list[dict], int | None)`, `FPS = 60`, `MAX_STEPS = 256`.

- [ ] **Step 1: Write the failing tests `tests/test_anims.py`**

```python
import unittest

import spritefix as F
from mmxdemake.anims import AnimError, read_table, walk

LO, HI = F.OV, F.OV + F.OV_SIZE
P = F.OV + 0x100
T = F.OV + 0x800


def ram_with(frames, at=P):
    ram = bytearray(0x200000)
    F.put(ram, at, b"".join(F.frame(*f) for f in frames))
    return bytes(ram)


class WalkTest(unittest.TestCase):
    def test_hold(self):
        frames, loop = walk(ram_with([(5, 0, 1, 3), (7, 0, 0, 4)]), P)
        self.assertEqual(frames, [{"sprite": 3, "ticks": 5, "flag": 0}, {"sprite": 4, "ticks": 7, "flag": 0}])
        self.assertIsNone(loop)

    def test_loop_to_start(self):
        frames, loop = walk(ram_with([(2, 0, 1, 5), (2, 0, 1, 6), (2, 1, -2, 7)]), P)
        self.assertEqual([f["sprite"] for f in frames], [5, 6, 7])
        self.assertEqual(loop, 0)

    def test_loop_to_middle(self):
        frames, loop = walk(ram_with([(1, 0, 1, 0), (1, 0, 1, 1), (1, 0, -1, 2)]), P)
        self.assertEqual([f["sprite"] for f in frames], [0, 1, 2])
        self.assertEqual(loop, 1)

    def test_zero_ticks(self):
        frames, _ = walk(ram_with([(0, 2, 0, 1)]), P)
        self.assertEqual(frames, [{"sprite": 1, "ticks": 256, "flag": 2}])

    def test_runaway(self):
        with self.assertRaisesRegex(AnimError, "more than 256 frames without a hold or a loop"):
            walk(ram_with([(1, 0, 1, 0)] * 300), P)


class ReadTableTest(unittest.TestCase):
    def ram(self, words):
        ram = bytearray(0x200000)
        for k, w in enumerate(words):
            F.word(ram, T + 4 * k, w)
        return bytes(ram)

    def test_reads_to_end(self):
        self.assertEqual(read_table(self.ram([P, P + 8, 0]), T, LO, HI), [P, P + 8])

    def test_stops_at_word_after_table(self):
        self.assertEqual(read_table(self.ram([P, T + 0x10]), T, LO, HI), [P])

    def test_first_word_bad(self):
        with self.assertRaisesRegex(AnimError, "first word 0x0 is not a frame list"):
            read_table(self.ram([0]), T, LO, HI)

    def test_table_outside(self):
        with self.assertRaisesRegex(AnimError, "is outside the overlay"):
            read_table(self.ram([P]), HI + 4, LO, HI)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_anims.py`
Expected: ERROR, `No module named 'mmxdemake.anims'`.

- [ ] **Step 3: Write `mmxdemake/anims.py`**

```python
import struct

from mmxdemake.savestate import ram_at

MAX_ANIMS = 64
MAX_STEPS = 256
FPS = 60
LONGEST = 256


class AnimError(Exception):
    pass


def read_table(ram, addr, lo, hi):
    if not lo <= addr < hi:
        raise AnimError(f"animation table {addr:#x} is outside the overlay {lo:#x}..{hi:#x}")
    out = []
    while len(out) < MAX_ANIMS:
        (p,) = struct.unpack("<I", ram_at(ram, addr + 4 * len(out), 4))
        if not lo <= p < addr:
            break
        out.append(p)
    if not out:
        raise AnimError(f"animation table {addr:#x}: first word {p:#x} is not a frame list "
                        f"between {lo:#x} and {addr:#x}")
    return out


def walk(ram, ptr):
    frames, seen, i = [], {}, 0
    while i not in seen:
        if len(frames) == MAX_STEPS:
            raise AnimError(f"animation {ptr:#x}: more than {MAX_STEPS} frames without a hold or a loop")
        ticks, flag, step, sprite = struct.unpack("<BBbB", ram_at(ram, ptr + 4 * i, 4))
        seen[i] = len(frames)
        frames.append({"sprite": sprite, "ticks": ticks or LONGEST, "flag": flag})
        if step == 0:
            return frames, None
        i += step
    return frames, seen[i]
```

- [ ] **Step 4: Run the tests**

Run: `python3 -m unittest discover -s tests -p test_anims.py`
Expected: `Ran 9 tests`, `OK`.

- [ ] **Step 5: Run the whole suite, then commit**

Run: `python3 -m unittest discover -s tests 2>&1 | tail -3` — Expected: `Ran 286 tests`, `OK (skipped=6)`.

```bash
git add tests/test_anims.py mmxdemake/anims.py
git commit -m "Add the animation table reader

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: `animfind`

**Files:**
- Create: `tests/test_animfind.py`, `mmxdemake/animfind.py`

**Interfaces:**
- Consumes: `spritefix` code helpers and `build`.
- Produces: `animfind.FindError`, `find_overlay(ram, overlays) -> (n, lo, hi)`, `program_bounds(exe) -> (lo, hi)`, `stage_types(ram, scratch, lo, hi) -> dict[int, int]`, `live_tables(ram, regions) -> dict[int, set[int]]`, `stores(ram, lo, hi) -> list[(pc, table)]`, `function_start(ram, pc, lo)`, `references(ram, lo, hi)`, `owners(ram, func, refs, entries, lo, hi) -> set[int]`, `searched_tables(ram, scratch, lo, hi) -> dict[int, list[int]]`, `choose(catalog_table, live, searched) -> (list[(table, found)], note | None)`, `MAIN_TYPES = 77`.

- [ ] **Step 1: Write the failing tests `tests/test_animfind.py`**

```python
import struct
import unittest

import spritefix as F
from mmxdemake.animfind import FindError, choose, find_overlay, live_tables, program_bounds, searched_tables, stores

LO, HI = F.OV, F.OV + F.OV_SIZE
PLO, PHI = program_bounds(F.EXE)


def blank():
    ram = bytearray(0x200000)
    scratch = bytearray(0x400)
    struct.pack_into("<I", scratch, 0x80, F.MAIN)
    return ram, scratch


def entry(ram, t, func):
    F.word(ram, F.MAIN + 4 * t, func)


class OverlayTest(unittest.TestCase):
    def setUp(self):
        self.ram, _, _, self.ov = F.build()

    def test_one_match(self):
        self.assertEqual(find_overlay(self.ram, {2: self.ov, 3: b"\1" * 0x900}), (2, LO, HI))

    def test_no_match(self):
        with self.assertRaisesRegex(FindError, r"0 overlays match RAM at 0x800e9860 \(\[\]\); need exactly 1"):
            find_overlay(self.ram, {3: b"\1" * 0x900})

    def test_two_matches(self):
        with self.assertRaisesRegex(FindError, r"2 overlays match .*\(\[2, 4\]\)"):
            find_overlay(self.ram, {2: self.ov, 4: self.ov})


class LiveTest(unittest.TestCase):
    def test_live(self):
        ram, _ = blank()
        slots = [(0x41, 1, F.TABLE1), (0, 2, F.TABLE1), (0x41, 80, F.TABLE1), (0x41, 3, 0x80200000),
                 (0x41, 1, F.TABLE1), (0x41, 5, F.TABLE2)]
        for s, (head, t, table) in enumerate(slots):
            a = F.SLOT0 + s * 0x9C
            F.put(ram, a, bytes([head, t]))
            F.word(ram, a + 0x30, table)
        self.assertEqual(live_tables(bytes(ram), [(LO, HI)]), {1: {F.TABLE1}, 5: {F.TABLE2}})


class StoresTest(unittest.TestCase):
    def test_found(self):
        ram, _ = blank()
        F.code(ram, F.FUNC1, F.setter(F.TABLE1))
        self.assertEqual(stores(bytes(ram), LO, HI), [(F.FUNC1 + 12, F.TABLE1)])

    def test_other_register(self):
        ram, _ = blank()
        F.code(ram, F.FUNC1, [F.PROLOGUE] + F.la(F.A1, F.TABLE1, rs=F.V0) + [F.sw(F.A1, 0x30, F.A0)])
        self.assertEqual(stores(bytes(ram), LO, HI), [(F.FUNC1 + 12, F.TABLE1)])

    def test_overwritten(self):
        ram, _ = blank()
        F.code(ram, F.FUNC1, [F.PROLOGUE] + F.la(F.V0, F.TABLE1) + [F.addu(F.V0, F.V0, 0), F.sw(F.V0, 0x30, F.A0)])
        self.assertEqual(stores(bytes(ram), LO, HI), [])

    def test_outside_overlay(self):
        ram, _ = blank()
        F.code(ram, F.FUNC1, F.setter(0x80200000))
        self.assertEqual(stores(bytes(ram), LO, HI), [])


class OwnersTest(unittest.TestCase):
    def test_direct_entry(self):
        ram, scratch, _, _ = F.build()
        self.assertEqual(searched_tables(ram, scratch, LO, HI), {1: [F.TABLE1], 2: [F.TABLE2]})

    def test_through_calls(self):
        ram, scratch = blank()
        top, mid, low = LO + 0x100, LO + 0x200, LO + 0x300
        F.code(ram, top, [F.PROLOGUE, F.jal(mid), F.NOP, F.JR_RA, F.NOP])
        F.code(ram, mid, [F.PROLOGUE, F.jal(low), F.NOP, F.JR_RA, F.NOP])
        F.code(ram, low, F.setter(F.TABLE1))
        entry(ram, 3, top)
        self.assertEqual(searched_tables(bytes(ram), scratch, LO, HI), {3: [F.TABLE1]})

    def test_through_pointer_table(self):
        ram, scratch = blank()
        top, a, b, states = LO + 0x100, LO + 0x200, LO + 0x300, LO + 0x1000
        F.code(ram, top, [F.PROLOGUE] + F.la(F.V0, states) + [F.JR_RA, F.NOP])
        F.code(ram, a, [F.PROLOGUE, F.JR_RA, F.NOP])
        F.code(ram, b, F.setter(F.TABLE1))
        F.word(ram, states, a)
        F.word(ram, states + 4, b)
        entry(ram, 4, top)
        self.assertEqual(searched_tables(bytes(ram), scratch, LO, HI), {4: [F.TABLE1]})

    def test_shared_helper(self):
        ram, scratch = blank()
        one, two, helper = LO + 0x100, LO + 0x200, LO + 0x300
        F.code(ram, one, [F.PROLOGUE, F.jal(helper), F.NOP, F.JR_RA, F.NOP])
        F.code(ram, two, [F.PROLOGUE, F.jal(helper), F.NOP, F.JR_RA, F.NOP])
        F.code(ram, helper, F.setter(F.TABLE1))
        entry(ram, 5, one)
        entry(ram, 6, two)
        self.assertEqual(searched_tables(bytes(ram), scratch, LO, HI), {5: [F.TABLE1], 6: [F.TABLE1]})

    def test_two_tables(self):
        ram, scratch = blank()
        top, s1, s2 = LO + 0x100, LO + 0x200, LO + 0x300
        F.code(ram, top, [F.PROLOGUE, F.jal(s1), F.NOP, F.jal(s2), F.NOP, F.JR_RA, F.NOP])
        F.code(ram, s1, F.setter(F.TABLE2))
        F.code(ram, s2, F.setter(F.TABLE1))
        entry(ram, 7, top)
        self.assertEqual(searched_tables(bytes(ram), scratch, LO, HI), {7: [F.TABLE1, F.TABLE2]})

    def test_program_region(self):
        ram, scratch, _, _ = F.build(shared=True)
        self.assertEqual((PLO, PHI), (0x80010000, 0x80011400))
        self.assertEqual(searched_tables(ram, scratch, PLO, PHI), {2: [F.EXE_TABLE]})
        self.assertEqual(searched_tables(ram, scratch, LO, HI), {1: [F.TABLE1]})


class ChooseTest(unittest.TestCase):
    def test_catalog_first(self):
        self.assertEqual(choose(0x10, {0x20}, [0x30]), ([(0x10, "catalog")], None))

    def test_live_and_search_agree(self):
        self.assertEqual(choose(None, {0x20}, [0x20]), ([(0x20, "live")], None))

    def test_every_table_kept(self):
        self.assertEqual(choose(None, {0x20}, [0x30]),
                         ([(0x20, "live"), (0x30, "search")], "2 tables: 0x20, 0x30; each is exported"))
        self.assertEqual(choose(None, {0x28, 0x20}, None),
                         ([(0x20, "live"), (0x28, "live")], "2 tables: 0x20, 0x28; each is exported"))
        self.assertEqual(choose(None, None, [0x30, 0x40]),
                         ([(0x30, "search"), (0x40, "search")], "2 tables: 0x30, 0x40; each is exported"))

    def test_search(self):
        self.assertEqual(choose(None, None, [0x30]), ([(0x30, "search")], None))

    def test_nothing(self):
        self.assertEqual(choose(None, None, None), ([], "no live object and no table found by the search"))


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_animfind.py`
Expected: ERROR, `No module named 'mmxdemake.animfind'`.

- [ ] **Step 3: Write `mmxdemake/animfind.py`**

```python
import struct

from mmxdemake.savestate import ram_at

OVERLAY_BASE = 0x800E9860
OVERLAY_PROBE = 0x800
PROGRAM_BASE = 0x80010000
EXE_HEADER = 0x800
MAIN_TABLE = 0x80
MAIN_TYPES = 77
ANIM_FIELD = 0x30
LOOKBACK = 12
PAIR_GAP = 5
RUN_MAX = 64
SLOTS = 48
SLOT_BASE = 0x8008EF48
SLOT_SIZE = 0x9C
JR_RA = 0x03E00008
STACK_ADJUST = 0x27BD
RA = 31
OP_J = 2
OP_JAL = 3
OP_ADDIU = 9
OP_LUI = 15
OP_SW = 0x2B
NO_WRITE = {0x08, 0x0C, 0x0D, 0x11, 0x13, 0x18, 0x19, 0x1A, 0x1B}


class FindError(Exception):
    pass


def _u32(ram, addr):
    return struct.unpack("<I", ram_at(ram, addr, 4))[0]


def _s16(x):
    return (x & 0xFFFF) - ((x & 0x8000) << 1)


def _rs(x):
    return (x >> 21) & 31


def _rt(x):
    return (x >> 16) & 31


def _written(x):
    op = x >> 26
    if op == 0:
        return None if x & 63 in NO_WRITE else (x >> 11) & 31
    if op == OP_JAL:
        return RA
    if 8 <= op <= 15 or 0x20 <= op <= 0x26:
        return _rt(x)
    if 0x10 <= op <= 0x13 and _rs(x) in (0, 2):
        return _rt(x)
    return None


def _hexes(values):
    return ", ".join(f"{v:#x}" for v in values)


def find_overlay(ram, overlays):
    probe = ram_at(ram, OVERLAY_BASE, OVERLAY_PROBE)
    matches = [n for n, body in sorted(overlays.items()) if body[:OVERLAY_PROBE] == probe]
    if len(matches) != 1:
        raise FindError(f"{len(matches)} overlays match RAM at {OVERLAY_BASE:#x} ({matches}); need exactly 1")
    n = matches[0]
    return n, OVERLAY_BASE, OVERLAY_BASE + len(overlays[n])


def program_bounds(exe):
    return PROGRAM_BASE, PROGRAM_BASE + len(exe) - EXE_HEADER


def stage_types(ram, scratch, lo, hi):
    table = struct.unpack_from("<I", scratch, MAIN_TABLE)[0]
    out = {}
    for t in range(MAIN_TYPES):
        func = _u32(ram, table + 4 * t)
        if lo <= func < hi:
            out[t] = func
    return out


def live_tables(ram, regions):
    out = {}
    for s in range(SLOTS):
        a = SLOT_BASE + s * SLOT_SIZE
        head = ram_at(ram, a, 2)
        table = _u32(ram, a + ANIM_FIELD)
        if head[0] and head[1] < MAIN_TYPES and any(lo <= table < hi for lo, hi in regions):
            out.setdefault(head[1], set()).add(table)
    return out


def _last_write(ram, pc, reg, lo, limit):
    for k in range(1, limit + 1):
        a = pc - 4 * k
        if a < lo:
            break
        y = _u32(ram, a)
        if _written(y) == reg:
            return a, y
    return None, None


def _constant(ram, pc, reg, lo):
    a, y = _last_write(ram, pc, reg, lo, LOOKBACK)
    if y is None or y >> 26 != OP_ADDIU:
        return None
    _, z = _last_write(ram, a, _rs(y), lo, LOOKBACK - (pc - a) // 4)
    if z is None or z >> 26 != OP_LUI:
        return None
    return (((z & 0xFFFF) << 16) + _s16(y)) & 0xFFFFFFFF


def stores(ram, lo, hi):
    out = []
    for pc in range(lo, hi, 4):
        x = _u32(ram, pc)
        if x >> 26 != OP_SW or x & 0xFFFF != ANIM_FIELD:
            continue
        table = _constant(ram, pc, _rt(x), lo)
        if table is not None and lo <= table < hi:
            out.append((pc, table))
    return out


def function_start(ram, pc, lo):
    a = pc & ~3
    while a > lo:
        x = _u32(ram, a)
        if x >> 16 == STACK_ADJUST and x & 0x8000:
            return a
        if _u32(ram, a - 8) == JR_RA:
            return a
        a -= 4
    return lo


def references(ram, lo, hi):
    refs = {}
    for o in range(lo, hi, 4):
        x = _u32(ram, o)
        op = x >> 26
        if op in (OP_J, OP_JAL):
            target = (o & 0xF0000000) | ((x & 0x3FFFFFF) << 2)
            refs.setdefault(target, set()).add(function_start(ram, o, lo))
        elif lo <= x < hi:
            refs.setdefault(x, set()).add(("data", o))
        elif op == OP_LUI:
            for k in range(1, PAIR_GAP + 1):
                y = _u32(ram, o + 4 * k)
                if y >> 26 == OP_ADDIU and _rs(y) == _rt(x):
                    value = (((x & 0xFFFF) << 16) + _s16(y)) & 0xFFFFFFFF
                    if lo <= value < hi:
                        refs.setdefault(value, set()).add(function_start(ram, o, lo))
                    break
    return refs


def _run_callers(ram, addr, refs, lo, hi):
    out, a = [], addr
    for _ in range(RUN_MAX):
        out.extend(r for r in refs.get(a, ()) if not isinstance(r, tuple))
        if not lo <= _u32(ram, a - 4) < hi:
            break
        a -= 4
    return out


def owners(ram, func, refs, entries, lo, hi):
    found, seen, todo = set(), set(), [func]
    while todo:
        f = todo.pop()
        if f in seen:
            continue
        seen.add(f)
        if f in entries:
            found |= entries[f]
            continue
        for r in refs.get(f, ()):
            todo.extend(_run_callers(ram, r[1], refs, lo, hi) if isinstance(r, tuple) else [r])
    return found


def searched_tables(ram, scratch, lo, hi):
    entries = {}
    for t, func in stage_types(ram, scratch, lo, hi).items():
        entries.setdefault(func, set()).add(t)
    refs = references(ram, lo, hi)
    out = {}
    for pc, table in stores(ram, lo, hi):
        for t in owners(ram, function_start(ram, pc, lo), refs, entries, lo, hi):
            out.setdefault(t, set()).add(table)
    return {t: sorted(v) for t, v in sorted(out.items())}


def choose(catalog_table, live, searched):
    if catalog_table is not None:
        return [(catalog_table, "catalog")], None
    live = set(live or ())
    tables = sorted(live | set(searched or ()))
    if not tables:
        return [], "no live object and no table found by the search"
    note = f"{len(tables)} tables: {_hexes(tables)}; each is exported" if len(tables) > 1 else None
    return [(t, "live" if t in live else "search") for t in tables], note
```

- [ ] **Step 4: Run the tests**

Run: `python3 -m unittest discover -s tests -p test_animfind.py`
Expected: `Ran 19 tests`, `OK`.

- [ ] **Step 5: Run the whole suite, then commit**

Run: `python3 -m unittest discover -s tests 2>&1 | tail -3` — Expected: `Ran 305 tests`, `OK (skipped=6)`.

```bash
git add tests/test_animfind.py mmxdemake/animfind.py
git commit -m "Find animation tables from live objects and from the overlay code

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: `fingerprint`

**Files:**
- Create: `tests/test_fingerprint.py`, `mmxdemake/fingerprint.py`

**Interfaces:**
- Consumes: `sprites.banks`, `cell`, `cell_bytes`, `palette_words`, `pieces`, `loaded`, `clut_word`; `anims.read_table`, `walk`.
- Produces: `fingerprint.content_id(ram, vram, bank, animations, mode) -> str`, `PREFIX = "enemy-"`. `animations` is a list of `(frames, loop_to)` from `walk`.

- [ ] **Step 1: Write the failing tests `tests/test_fingerprint.py`**

```python
import copy
import re
import struct
import unittest

import spritefix as F
from mmxdemake.anims import read_table, walk
from mmxdemake.fingerprint import content_id
from mmxdemake.sprites import banks, clut_word

LO, HI = F.OV, F.OV + F.OV_SIZE


class FingerprintTest(unittest.TestCase):
    def setUp(self):
        ram, scratch, vram, _ = F.build()
        self.ram, self.vram = bytearray(ram), bytearray(vram)
        self.bank = banks(ram, scratch)[1]
        self.anims = [walk(ram, p) for p in read_table(ram, F.TABLE1, LO, HI)]
        self.id = self.make()

    def make(self, bank=None, anims=None):
        return content_id(bytes(self.ram), bytes(self.vram), bank or self.bank, anims or self.anims, 0x40)

    def test_form(self):
        self.assertRegex(self.id, r"^enemy-[0-9a-f]{10}$")

    def test_same_content_elsewhere(self):
        moved = self.bank._replace(index=5, data=F.BASE + 0x200, texture=self.bank.texture + 2, clut=clut_word(5))
        o = self.bank.data & 0x1FFFFF
        F.put(self.ram, moved.data, bytes(self.ram[o:o + 0x14]))
        F.put_cell(self.vram, 6, 32, 32, F.cell0)
        F.put_cell(self.vram, 6, 48, 32, lambda x, y: 3)
        x, y = F.clut_xy(1)
        F.put_palette(self.vram, 5, list(struct.unpack_from("<16H", self.vram, y * 2048 + x * 2)))
        self.assertEqual(self.make(moved), self.id)

    def test_palette_colour_changes_id(self):
        x, y = F.clut_xy(1)
        struct.pack_into("<H", self.vram, y * 2048 + x * 2 + 2, F.GREEN)
        self.assertNotEqual(self.make(), self.id)

    def test_cell_byte_changes_id(self):
        self.vram[32 * 2048 + 384 * 2 + 3] ^= 0x11
        self.assertNotEqual(self.make(), self.id)

    def test_tick_changes_id(self):
        anims = copy.deepcopy(self.anims)
        anims[0][0][0]["ticks"] += 1
        self.assertNotEqual(self.make(anims=anims), self.id)

    def test_loop_changes_id(self):
        anims = copy.deepcopy(self.anims)
        anims[0] = (anims[0][0], None)
        self.assertNotEqual(self.make(anims=anims), self.id)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_fingerprint.py`
Expected: ERROR, `No module named 'mmxdemake.fingerprint'`.

- [ ] **Step 3: Write `mmxdemake/fingerprint.py`**

```python
import hashlib
import json

from mmxdemake.sprites import cell, cell_bytes, loaded, palette_words, pieces

VERSION = 1
ID_HEX = 10
PREFIX = "enemy-"
KEPT_FLAGS = 0xF0


def content_id(ram, vram, bank, animations, mode):
    texture = loaded(bank)
    used = sorted({f["sprite"] for frames, _ in animations for f in frames})
    sprites = []
    for n in used:
        parts = []
        for flags, tile, dx, dy in pieces(ram, bank, n):
            page, u, v = cell(flags, tile, texture, mode)
            colours = palette_words(vram, bank.clut + ((flags >> 2) & 3))
            parts.append([flags & KEPT_FLAGS, dx, dy, cell_bytes(vram, page, u, v).hex(), colours])
        sprites.append([n, parts])
    doc = {
        "version": VERSION,
        "animations": [[[[f["sprite"], f["ticks"], f["flag"]] for f in frames], loop] for frames, loop in animations],
        "sprites": sprites,
    }
    text = json.dumps(doc, sort_keys=True, separators=(",", ":"))
    return PREFIX + hashlib.sha1(text.encode()).hexdigest()[:ID_HEX]
```

- [ ] **Step 4: Run the tests**

Run: `python3 -m unittest discover -s tests -p test_fingerprint.py`
Expected: `Ran 6 tests`, `OK`.

- [ ] **Step 5: Run the whole suite, then commit**

Run: `python3 -m unittest discover -s tests 2>&1 | tail -3` — Expected: `Ran 311 tests`, `OK (skipped=6)`.

```bash
git add tests/test_fingerprint.py mmxdemake/fingerprint.py
git commit -m "Add content IDs for sprite exports

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 5: `spriteexport` and the catalog

**Files:**
- Create: `tests/test_spriteexport.py`, `mmxdemake/spriteexport.py`, `catalog/enemies.json`

**Interfaces:**
- Consumes: everything above; `outdir.check`, `outdir.fresh_tmp`; `png.write_png`; `digits.text_mask`; `learn.DEFAULT_*`; `savestate.read_state`, `ram_from`; `vram.read_vram`; `tests/pngread.read_png`.
- Produces: `spriteexport.run(state, dumps, extracted, catalog, root, log=print) -> int`, `main(argv=None)`, `load_catalog(catalog) -> (stages, names, tables)`.

- [ ] **Step 1: Write the failing tests `tests/test_spriteexport.py`**

```python
import json
import tempfile
import unittest
from pathlib import Path

import spritefix as F
import synth
from mmxdemake.spriteexport import REPO_ROOT, run
from pngread import read_png


class ExportTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.root = self.dir / "sprites"
        self.catalog = self.dir / "catalog"
        self.catalog.mkdir()
        self.set_catalog({"stages": {"0": "intro", "1": "yammark"}})
        self.lines = []

    def tearDown(self):
        self.tmp.cleanup()

    def set_catalog(self, doc):
        (self.catalog / "enemies.json").write_text(json.dumps(doc))

    def world(self, **states):
        overlay = None
        built = {}
        for name, kwargs in states.items():
            ram, scratch, vram, overlay = F.build(**kwargs)
            built[name + ".state.gz"] = (ram, scratch, vram)
        self.extracted, self.dumps = F.write_world(self.dir / "w", built, overlay)

    def add_state(self, name, **kwargs):
        ram, scratch, vram, _ = F.build(**kwargs)
        (self.dumps / (name + ".state.gz")).write_bytes(synth.build_state(ram, vram=vram, scratch=scratch))

    def run_state(self, name, root=None):
        self.lines = []
        return run(name + ".state.gz", self.dumps, self.extracted, self.catalog, root or self.root,
                   log=self.lines.append)

    def folders(self):
        return sorted(p.name for p in (self.root / "enemies").iterdir())

    def doc(self, folder):
        return json.loads((self.root / "enemies" / folder / "animations.json").read_text())

    def test_first_run(self):
        self.world(s1={})
        self.assertEqual(self.run_state("s1"), 0, self.lines)
        [folder] = self.folders()
        self.assertRegex(folder, r"^enemy-[0-9a-f]{10}$")
        doc = self.doc(folder)
        self.assertEqual(doc["id"], folder)
        self.assertEqual((doc["fps"], doc["cell"], doc["origin"]), (60, [32, 24], [16, 16]))
        self.assertEqual(doc["animations"], [
            {"index": 0, "frames": [{"sprite": 0, "ticks": 6, "flag": 0}, {"sprite": 1, "ticks": 4, "flag": 1}],
             "loop_to": 0},
            {"index": 1, "frames": [{"sprite": 1, "ticks": 256, "flag": 2}], "loop_to": None},
        ])
        [src] = doc["sources"]
        self.assertEqual({k: src[k] for k in ("stage", "area", "type", "bank", "table", "found", "state")},
                         {"stage": 0, "area": 0, "type": 1, "bank": 1, "table": "0x800eb960", "found": "live",
                          "state": "s1.state.gz"})
        self.assertEqual(read_png(self.root / "enemies" / folder / "anim-0.png")[:2], (64, 24))
        self.assertEqual(read_png(self.root / "enemies" / folder / "anim-1.png")[:2], (32, 24))
        banks = sorted(p.name for p in (self.root / "banks/intro/s1").iterdir())
        self.assertEqual(banks, ["bank-0.png", "bank-1.png"])
        index = json.loads((self.root / "stages/intro.json").read_text())
        self.assertEqual(index["states"]["s1"]["enemies"], [folder])
        self.assertEqual(index["states"]["s1"]["skipped"], [{"type": 2, "reason": "no texture loaded"}])
        self.assertIn("s1.state.gz: stage 0 area 0, 2 banks drawn, 1 without texture, 1 exports "
                      "(1 live, 0 search, 0 catalog), 1 skipped, 0 renamed", self.lines)
        self.assertFalse((self.dir / "sprites.tmp").exists())

    def test_search_when_not_live(self):
        self.world(s1={"live": False})
        self.assertEqual(self.run_state("s1"), 0, self.lines)
        self.assertEqual(self.doc(self.folders()[0])["sources"][0]["found"], "search")

    def test_twins_share_one_export(self):
        self.world(s1={"twin": True})
        self.assertEqual(self.run_state("s1"), 0, self.lines)
        [folder] = self.folders()
        self.assertEqual([s["type"] for s in self.doc(folder)["sources"]], [1, 2])

    def test_second_area_adds_and_keeps(self):
        self.world(s1={})
        self.run_state("s1")
        [first] = self.folders()
        before = {p.name: (p.read_bytes(), p.stat().st_mtime_ns) for p in (self.root / "enemies" / first).iterdir()}
        self.add_state("s2", area=1, colour=F.GREEN)
        self.assertEqual(self.run_state("s2"), 0, self.lines)
        self.assertEqual(len(self.folders()), 2)
        after = {p.name: (p.read_bytes(), p.stat().st_mtime_ns) for p in (self.root / "enemies" / first).iterdir()}
        self.assertEqual(after, before)
        index = json.loads((self.root / "stages/intro.json").read_text())
        self.assertEqual(sorted(index["states"]), ["s1", "s2"])
        self.assertEqual(index["states"]["s2"]["area"], 1)

    def test_other_stage_adds_source(self):
        self.world(s1={})
        self.run_state("s1")
        self.add_state("s3", stage=1)
        self.assertEqual(self.run_state("s3"), 0, self.lines)
        [folder] = self.folders()
        self.assertEqual([(s["stage"], s["state"]) for s in self.doc(folder)["sources"]],
                         [(0, "s1.state.gz"), (1, "s3.state.gz")])
        self.assertTrue((self.root / "banks/yammark/s3/bank-1.png").exists())

    def test_rerun_same_bytes(self):
        self.world(s1={})
        self.run_state("s1")
        [folder] = self.folders()
        first = {p.name: p.read_bytes() for p in (self.root / "enemies" / folder).iterdir()}
        self.assertEqual(self.run_state("s1"), 0, self.lines)
        again = {p.name: p.read_bytes() for p in (self.root / "enemies" / folder).iterdir()}
        self.assertEqual(again, first)

    def test_names_rename(self):
        self.world(s1={})
        self.run_state("s1")
        [cid] = self.folders()
        self.set_catalog({"stages": {"0": "intro"}, "names": {cid: "junkroid"}})
        self.add_state("s2", area=1, colour=F.GREEN)
        self.assertEqual(self.run_state("s2"), 0, self.lines)
        self.assertIn("junkroid", self.folders())
        self.assertNotIn(cid, self.folders())
        self.assertEqual(self.doc("junkroid")["id"], cid)
        self.assertIn(f"renamed enemies/{cid} -> enemies/junkroid", self.lines)

    def test_folder_conflict_refuses(self):
        self.world(s1={})
        self.run_state("s1")
        [cid] = self.folders()
        self.add_state("s2", area=1, colour=F.GREEN)
        self.run_state("s2")
        other = [f for f in self.folders() if f != cid][0]
        (self.root / "enemies" / other).rename(self.root / "enemies" / "junkroid")
        self.set_catalog({"stages": {"0": "intro"}, "names": {cid: "junkroid"}})
        self.assertEqual(self.run_state("s1"), 1)
        self.assertIn(f"enemies/junkroid holds {other}, not {cid}", self.lines[-1])
        self.assertIn(cid, self.folders())

    def test_catalog_table(self):
        self.world(s1={})
        self.set_catalog({"stages": {"0": "intro"},
                          "tables": [{"stage": 0, "type": 1, "animations": f"0x{F.TABLE2:08X}"}]})
        self.assertEqual(self.run_state("s1"), 0, self.lines)
        src = self.doc(self.folders()[0])["sources"][0]
        self.assertEqual((src["found"], src["table"]), ("catalog", "0x800ebb60"))

    def test_two_tables_two_exports(self):
        self.world(s1={})
        ram, scratch, vram, _ = F.build()
        ram = bytearray(ram)
        F.put(ram, F.SLOT0 + 0x9C, bytes([0x41, 1]))
        F.word(ram, F.SLOT0 + 0x9C + 0x30, F.TABLE2)
        (self.dumps / "s1.state.gz").write_bytes(synth.build_state(bytes(ram), vram=vram, scratch=scratch))
        self.assertEqual(self.run_state("s1"), 0, self.lines)
        self.assertEqual(len(self.folders()), 2)
        self.assertIn(f"type 1: 2 tables: {F.TABLE1:#x}, {F.TABLE2:#x}; each is exported", self.lines)

    def test_shared_type_from_program(self):
        self.world(s1={"shared": True})
        self.assertEqual(self.run_state("s1"), 0, self.lines)
        sources = [s for f in self.folders() for s in self.doc(f)["sources"]]
        self.assertEqual(sorted((s["type"], s["found"], s["table"]) for s in sources),
                         [(1, "live", "0x800eb960"), (2, "search", "0x80010800")])

    def test_catalog_bad_table_exit_1(self):
        self.world(s1={})
        self.set_catalog({"stages": {"0": "intro"},
                          "tables": [{"stage": 0, "type": 1, "animations": f"0x{F.OV + 0x3800:08X}"}]})
        self.assertEqual(self.run_state("s1"), 1)
        self.assertFalse(self.root.exists())

    def test_bad_search_table_skipped(self):
        self.world(s1={"live": False})
        ram, scratch, vram, _ = F.build(live=False)
        ram = bytearray(ram)
        F.put(ram, F.FRAMES1, F.frame(1, 0, 0, 9))
        (self.dumps / "s1.state.gz").write_bytes(synth.build_state(bytes(ram), vram=vram, scratch=scratch))
        self.assertEqual(self.run_state("s1"), 0, self.lines)
        index = json.loads((self.root / "stages/intro.json").read_text())
        self.assertEqual(index["states"]["s1"]["skipped"][0]["type"], 1)
        self.assertIn("uses sprite 9", index["states"]["s1"]["skipped"][0]["reason"])

    def test_root_inside_repo(self):
        self.world(s1={})
        self.assertEqual(self.run_state("s1", root=REPO_ROOT / "out-sprites"), 2)
        self.assertIn("inside the repo", self.lines[-1])

    def test_root_with_foreign_file(self):
        self.world(s1={})
        self.root.mkdir()
        (self.root / "notes.txt").write_text("x")
        self.assertEqual(self.run_state("s1"), 2)

    def test_malformed_catalog(self):
        self.world(s1={})
        self.set_catalog({"names": {"bad": "junkroid"}})
        self.assertEqual(self.run_state("s1"), 2)
        self.set_catalog({"names": {"enemy-0000000000": "a", "enemy-1111111111": "a"}})
        self.assertEqual(self.run_state("s1"), 2)
        self.assertIn("name 'a' is used twice", self.lines[-1])

    def test_bad_state_name(self):
        self.world(s1={})
        self.assertEqual(run("../s1.state.gz", self.dumps, self.extracted, self.catalog, self.root,
                             log=self.lines.append), 2)

    def test_failed_check_changes_nothing(self):
        self.world(s1={})
        self.run_state("s1")
        snapshot = sorted(str(p.relative_to(self.root)) for p in self.root.rglob("*"))
        (self.extracted / "iso/SLUS_013.95").write_bytes(bytes(0x1C00))
        self.assertEqual(self.run_state("s1"), 1)
        self.assertIn("does not hold the X6 program text", self.lines[-1])
        self.assertEqual(sorted(str(p.relative_to(self.root)) for p in self.root.rglob("*")), snapshot)


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run to verify they fail**

Run: `python3 -m unittest discover -s tests -p test_spriteexport.py`
Expected: ERROR, `No module named 'mmxdemake.spriteexport'`.

- [ ] **Step 3: Write `mmxdemake/spriteexport.py`**

```python
import argparse
import hashlib
import json
import re
import shutil
import sys
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.animfind import (FindError, choose, find_overlay, live_tables, program_bounds, searched_tables,
                                stage_types)
from mmxdemake.anims import FPS, AnimError, read_table, walk
from mmxdemake.digits import text_mask
from mmxdemake.fingerprint import PREFIX, content_id
from mmxdemake.learn import DEFAULT_CATALOG, DEFAULT_DUMPS, DEFAULT_EXTRACTED
from mmxdemake.png import write_png
from mmxdemake.savestate import SaveStateError, ram_from, read_state
from mmxdemake.sprites import ENEMY_MODE, SpriteError, banks, draw, scratchpad, stage_area
from mmxdemake.vram import VramError, read_vram

REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ROOT = Path.home() / "mmx-data/x6/sprites"
EXE = "iso/SLUS_013.95"
SUFFIX = ".state.gz"
TOP = ("enemies", "banks", "stages")
NAME = re.compile(r"[a-z0-9-]+")
CONTENT_ID = re.compile(r"enemy-[0-9a-f]{10}")
ADDRESS = re.compile(r"0x[0-9A-Fa-f]{8}")
FIRST_MISC = 0x80
COLUMNS = 8
PAD = 2
LABEL = 7
BACK = bytes((40, 40, 60, 255))
INK = bytes((255, 255, 255, 255))
NO_TEXTURE = "no texture loaded"
NO_CODE = "update function is in neither the stage overlay nor the main program"


class ExportFailed(Exception):
    pass


def load_catalog(catalog):
    doc = json.loads((Path(catalog) / "enemies.json").read_text())
    if not isinstance(doc, dict) or set(doc) - {"stages", "names", "tables"}:
        raise ValueError("enemies.json must be an object with only stages, names and tables")
    stages, names, tables = doc.get("stages", {}), doc.get("names", {}), doc.get("tables", [])
    if not isinstance(stages, dict) or not isinstance(names, dict) or not isinstance(tables, list):
        raise ValueError("enemies.json: stages and names must be objects, tables a list")
    for k, v in stages.items():
        if not re.fullmatch(r"\d+", k) or not isinstance(v, str) or not NAME.fullmatch(v):
            raise ValueError(f"enemies.json: bad stage entry {k!r}: {v!r}")
    for k, v in names.items():
        if not CONTENT_ID.fullmatch(k):
            raise ValueError(f"enemies.json: {k!r} is not an ID")
        if not isinstance(v, str) or not NAME.fullmatch(v) or v.startswith(PREFIX):
            raise ValueError(f"enemies.json: bad name {v!r} for {k}")
    values = list(names.values())
    twice = sorted({v for v in values if values.count(v) > 1})
    if twice:
        raise ValueError(f"enemies.json: name {twice[0]!r} is used twice")
    by_type = {}
    for e in tables:
        if not (isinstance(e, dict) and set(e) == {"stage", "type", "animations"}
                and all(type(e[k]) is int for k in ("stage", "type"))
                and isinstance(e["animations"], str) and ADDRESS.fullmatch(e["animations"])):
            raise ValueError(f"enemies.json: bad tables entry {e!r}")
        by_type[(e["stage"], e["type"])] = int(e["animations"], 16)
    return {int(k): v for k, v in stages.items()}, names, by_type


def _read_id(folder):
    try:
        cid = json.loads((folder / "animations.json").read_text())["id"]
    except (OSError, ValueError, KeyError, TypeError):
        return None
    return cid if isinstance(cid, str) and CONTENT_ID.fullmatch(cid) else None


def existing(root):
    found = {}
    enemies = Path(root) / "enemies"
    if enemies.is_dir():
        for p in sorted(enemies.iterdir()):
            cid = _read_id(p) if p.is_dir() and NAME.fullmatch(p.name) else None
            if cid is None or cid in found:
                raise ValueError(f"{p} is not an earlier export; choose another --root")
            found[cid] = p.name
    return found


def collect(ram, scratch, vram, regions, stage, area, tables, state, sha1, log):
    bank_list = banks(ram, scratch)
    types, searched = {}, {}
    for lo, hi in regions:
        types.update(stage_types(ram, scratch, lo, hi))
        searched.update(searched_tables(ram, scratch, lo, hi))
    live = live_tables(ram, regions)
    exports, skipped, seen = {}, [], set()
    for bank in bank_list:
        t = bank.type
        if t >= FIRST_MISC or t in seen:
            continue
        seen.add(t)
        reason = NO_CODE if t not in types else NO_TEXTURE if bank.texture is None else None
        if reason:
            skipped.append({"type": t, "reason": reason})
            continue
        choices, note = choose(tables.get((stage, t)), live.get(t), searched.get(t))
        if note:
            log(f"type {t}: {note}")
        if not choices:
            skipped.append({"type": t, "reason": note})
            continue
        for table, found in choices:
            try:
                region = next(((lo, hi) for lo, hi in regions if lo <= table < hi), None)
                if region is None:
                    raise AnimError(f"type {t}: table {table:#x} is outside the overlay and the main program")
                anims = [walk(ram, p) for p in read_table(ram, table, *region)]
                used = sorted({f["sprite"] for frames, _ in anims for f in frames})
                if used[-1] >= bank.count:
                    raise AnimError(f"type {t}: table {table:#x} uses sprite {used[-1]}, "
                                    f"bank {bank.index} has {bank.count}")
                cid = content_id(ram, vram, bank, anims, ENEMY_MODE)
                frames = {n: draw(ram, vram, bank, n) for n in used}
            except (AnimError, SpriteError) as e:
                if found == "catalog":
                    raise
                log(f"type {t}: skipped: {e}")
                skipped.append({"type": t, "table": f"{table:#010x}", "reason": str(e)})
                continue
            source = {"stage": stage, "area": area, "type": t, "bank": bank.index, "table": f"{table:#010x}",
                      "found": found, "state": state, "state_sha1": sha1}
            export = exports.setdefault(cid, {"id": cid, "animations": anims, "frames": frames, "sources": []})
            export["sources"].append(source)
    return bank_list, exports, skipped


def geometry(frames):
    left = max(im.ox for im in frames.values())
    top = max(im.oy for im in frames.values())
    right = max(im.width - im.ox for im in frames.values())
    bottom = max(im.height - im.oy for im in frames.values())
    return left + right, top + bottom, left, top


def _blit(canvas, im, x0, y0):
    for y, line in enumerate(im.pixels):
        for x, p in enumerate(line):
            if p[3]:
                o = (x0 + x) * 4
                canvas[y0 + y][o:o + 4] = bytes(p)


def strip(frames, sequence, w, h, ox, oy):
    canvas = [bytearray(w * len(sequence) * 4) for _ in range(h)]
    for k, f in enumerate(sequence):
        im = frames[f["sprite"]]
        _blit(canvas, im, k * w + ox - im.ox, oy - im.oy)
    return w * len(sequence), h, [bytes(r) for r in canvas]


def sheet(ram, vram, bank):
    images = [draw(ram, vram, bank, n) for n in range(bank.count)]
    cw = max(max(im.width for im in images), len(str(bank.count - 1)) * 4) + 2 * PAD
    ch = max(im.height for im in images) + LABEL + 2 * PAD
    cols = min(COLUMNS, len(images))
    rows = -(-len(images) // cols)
    canvas = [bytearray(BACK * (cw * cols)) for _ in range(ch * rows)]
    for n, im in enumerate(images):
        x0, y0 = (n % cols) * cw + PAD, (n // cols) * ch + PAD
        for y, line in enumerate(text_mask(str(n))):
            for x, bit in enumerate(line):
                if bit:
                    canvas[y0 + y][(x0 + x) * 4:(x0 + x) * 4 + 4] = INK
        _blit(canvas, im, x0, y0 + LABEL)
    return cw * cols, ch * rows, [bytes(r) for r in canvas]


def _key(source):
    return source["stage"], source["area"], source["type"], source["state"]


def merged(old, new):
    out = {_key(s): s for s in old}
    out.update({_key(s): s for s in new})
    return [out[k] for k in sorted(out)]


def _replace(src, dst):
    dst.parent.mkdir(parents=True, exist_ok=True)
    if dst.is_dir():
        shutil.rmtree(dst)
    elif dst.exists():
        dst.unlink()
    src.rename(dst)


def publish(root, stage_name, stem, area, sha1, ram, vram, bank_list, exports, skipped, names, before, log):
    targets = {cid: names.get(cid, cid) for cid in exports}
    renames = {cid: names[cid] for cid, folder in before.items()
               if cid not in exports and cid in names and names[cid] != folder}
    owner = {folder: cid for cid, folder in before.items()}
    for cid, folder in list(targets.items()) + list(renames.items()):
        other = owner.get(folder)
        if other is not None and other != cid:
            raise ExportFailed(f"enemies/{folder} holds {other}, not {cid}")
    tmp = outdir.fresh_tmp(root)
    bank_dir = tmp / "banks" / stage_name / stem
    bank_dir.mkdir(parents=True)
    drawn = without = 0
    for bank in bank_list:
        if bank.texture is None:
            without += 1
            log(f"bank {bank.index}: type {bank.type}, no texture")
        elif bank.count:
            write_png(bank_dir / f"bank-{bank.index}.png", *sheet(ram, vram, bank))
            drawn += 1
            log(f"bank {bank.index}: type {bank.type}, {bank.count} sprites")
    for cid, export in exports.items():
        folder = tmp / "enemies" / targets[cid]
        folder.mkdir(parents=True)
        w, h, ox, oy = geometry(export["frames"])
        for k, (frames, _) in enumerate(export["animations"]):
            write_png(folder / f"anim-{k}.png", *strip(export["frames"], frames, w, h, ox, oy))
        old = []
        if cid in before:
            old = json.loads((root / "enemies" / before[cid] / "animations.json").read_text())["sources"]
        doc = {"id": cid, "fps": FPS, "cell": [w, h], "origin": [ox, oy],
               "animations": [{"index": k, "frames": frames, "loop_to": loop}
                              for k, (frames, loop) in enumerate(export["animations"])],
               "sources": merged(old, export["sources"])}
        (folder / "animations.json").write_text(json.dumps(doc, indent=2, sort_keys=True) + "\n")
        log(f"{targets[cid]}: {cid}, {len(export['animations'])} animations, "
            + ", ".join(f"type {s['type']} ({s['found']})" for s in export["sources"]))
    index_path = root / "stages" / f"{stage_name}.json"
    index = json.loads(index_path.read_text()) if index_path.exists() else {"states": {}}
    index["states"][stem] = {"area": area, "state_sha1": sha1, "enemies": sorted(exports), "skipped": skipped}
    (tmp / "stages").mkdir()
    (tmp / "stages" / index_path.name).write_text(json.dumps(index, indent=2, sort_keys=True) + "\n")
    try:
        for cid in exports:
            if cid in before:
                shutil.rmtree(root / "enemies" / before[cid])
            _replace(tmp / "enemies" / targets[cid], root / "enemies" / targets[cid])
        _replace(bank_dir, root / "banks" / stage_name / stem)
        _replace(tmp / "stages" / index_path.name, index_path)
        for cid, name in renames.items():
            (root / "enemies" / before[cid]).rename(root / "enemies" / name)
            log(f"renamed enemies/{before[cid]} -> enemies/{name}")
        shutil.rmtree(tmp)
    except OSError as e:
        raise ExportFailed(f"moving into {root} failed ({e}); run the same state again to repair it") from e
    return drawn, without, len(renames)


def run(state, dumps, extracted, catalog, root, log=print):
    dumps, extracted, root = Path(dumps), Path(extracted), Path(root)
    if Path(state).name != state or not state.endswith(SUFFIX):
        log(f"error: --state {state!r} must be a file name ending in {SUFFIX}")
        return 2
    try:
        stage_names, names, tables = load_catalog(catalog)
        refusal = outdir.check(root, REPO_ROOT, lambda n: n in TOP)
        if refusal:
            raise ValueError(refusal)
        before = existing(root)
    except (OSError, ValueError) as e:
        log(f"error: {e}")
        return 2
    try:
        path = dumps / state
        data = read_state(path)
        exe = (extracted / EXE).read_bytes()
        ram = ram_from(data, exe, path)
        scratch, vram = scratchpad(data), read_vram(data)
        overlays = {int(p.stem): p.read_bytes() for p in sorted((extracted / "rock_bin").glob("*.bin"))}
        _, lo, hi = find_overlay(ram, overlays)
        stage, area = stage_area(ram)
        sha1 = hashlib.sha1(path.read_bytes()).hexdigest()
        regions = [(lo, hi), program_bounds(exe)]
        bank_list, exports, skipped = collect(ram, scratch, vram, regions, stage, area, tables, state, sha1, log)
        drawn, without, renamed = publish(root, stage_names.get(stage, f"stage-{stage}"), state[:-len(SUFFIX)],
                                          area, sha1, ram, vram, bank_list, exports, skipped, names, before, log)
    except (SaveStateError, SpriteError, AnimError, FindError, VramError, ExportFailed, OSError, ValueError) as e:
        log(f"error: {e}")
        return 1
    found = [s["found"] for e in exports.values() for s in e["sources"]]
    log(f"{state}: stage {stage} area {area}, {drawn} banks drawn, {without} without texture, "
        f"{len(exports)} exports ({found.count('live')} live, {found.count('search')} search, "
        f"{found.count('catalog')} catalog), {len(skipped)} skipped, {renamed} renamed")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.spriteexport")
    parser.add_argument("--state", required=True)
    parser.add_argument("--dumps", default=str(DEFAULT_DUMPS))
    parser.add_argument("--extracted", default=str(DEFAULT_EXTRACTED))
    parser.add_argument("--catalog", default=str(DEFAULT_CATALOG))
    parser.add_argument("--root", default=str(DEFAULT_ROOT))
    args = parser.parse_args(argv)
    return run(args.state, Path(args.dumps).expanduser(), Path(args.extracted).expanduser(),
               Path(args.catalog).expanduser(), Path(args.root).expanduser(), log=print)


if __name__ == "__main__":
    sys.exit(main())
```

- [ ] **Step 4: Write `catalog/enemies.json`**

```json
{
  "stages": {"0": "intro", "1": "yammark"},
  "names": {},
  "tables": []
}
```

- [ ] **Step 5: Run the tests**

Run: `python3 -m unittest discover -s tests -p test_spriteexport.py`
Expected: `Ran 18 tests`, `OK`.

- [ ] **Step 6: Run the whole suite, then commit**

Run: `python3 -m unittest discover -s tests 2>&1 | tail -3` — Expected: `Ran 329 tests`, `OK (skipped=6)`.

```bash
git add tests/test_spriteexport.py mmxdemake/spriteexport.py catalog/enemies.json
git commit -m "Add the sprite export command

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 6: real data, docs and names

**Files:**
- Create: `tests/test_retail_sprites.py`
- Modify: `README.md`, `docs/TODO.md`, `docs/process/process-log.md`, `catalog/enemies.json`

**Interfaces:**
- Consumes: `spriteexport.run`; `sprites`, `animfind`, `anims` on the real states.

- [ ] **Step 1: Write `tests/test_retail_sprites.py`**

```python
import json
import os
import tempfile
import unittest
from pathlib import Path

from mmxdemake.animfind import find_overlay, program_bounds, searched_tables
from mmxdemake.anims import read_table, walk
from mmxdemake.savestate import ram_from, read_state
from mmxdemake.spriteexport import run
from mmxdemake.sprites import banks, draw, scratchpad
from mmxdemake.vram import read_vram

DUMPS = os.environ.get("MMX6_DUMPS")
EXTRACTED = os.environ.get("MMX6_EXTRACTED")
WORK = Path(os.environ.get("MMX6_WORK", "~/mmx-data/x6")).expanduser()
INTRO = {0: 0x800FAA34, 1: 0x800FAB24, 2: 0x800FACD4, 5: 0x800FADC4}


def load(name):
    path = Path(DUMPS).expanduser() / name
    data = read_state(path)
    exe = (Path(EXTRACTED).expanduser() / "iso/SLUS_013.95").read_bytes()
    ram = ram_from(data, exe, path)
    overlays = {int(p.stem): p.read_bytes() for p in (Path(EXTRACTED).expanduser() / "rock_bin").glob("*.bin")}
    return ram, scratchpad(data), read_vram(data), find_overlay(ram, overlays), exe


@unittest.skipUnless(DUMPS and EXTRACTED, "set MMX6_DUMPS and MMX6_EXTRACTED to run")
class RetailSpritesTest(unittest.TestCase):
    def test_intro_tables_and_junkroid(self):
        ram, scratch, vram, (n, lo, hi), _ = load("intro-1_start.state.gz")
        self.assertEqual(n, 2)
        found = searched_tables(ram, scratch, lo, hi)
        self.assertEqual({t: found.get(t) for t in INTRO}, {t: [a] for t, a in INTRO.items()})
        anims = [walk(ram, p) for p in read_table(ram, INTRO[1], lo, hi)]
        self.assertEqual(len(anims), 8)
        frames, loop = anims[1]
        self.assertEqual([(f["sprite"], f["ticks"], f["flag"]) for f in frames],
                         [(1, 6, 0), (2, 4, 0), (3, 1, 1), (3, 15, 0), (2, 6, 0), (1, 6, 2)])
        self.assertIsNone(loop)
        self.assertEqual(([f["sprite"] for f in anims[7][0]], anims[7][1]), ([5, 6, 7], 0))
        bank = [b for b in banks(ram, scratch) if b.type == 1][0]
        im = draw(ram, vram, bank, 0)
        self.assertEqual((im.width, im.height, im.ox, im.oy), (66, 58, 32, 27))
        self.assertEqual(sum(1 for line in im.pixels for p in line if p[3]), 2422)

    def test_yammark_tables(self):
        ram, scratch, _, (n, lo, hi), exe = load("yammark-1_start.state.gz")
        self.assertEqual(n, 3)
        self.assertLessEqual({3, 4, 23, 28, 37}, set(searched_tables(ram, scratch, lo, hi)))
        self.assertEqual(searched_tables(ram, scratch, *program_bounds(exe)).get(15), [0x80075FB0, 0x8007789C])

    def test_export_live_and_search_agree(self):
        with tempfile.TemporaryDirectory(dir=WORK) as t:
            root, lines = Path(t) / "sprites", []
            catalog = Path(__file__).resolve().parents[1] / "catalog"
            for name in ("intro-1_start.state.gz", "intro-2_junkroid.state.gz"):
                self.assertEqual(run(name, Path(DUMPS).expanduser(), Path(EXTRACTED).expanduser(), catalog, root,
                                     log=lines.append), 0, lines)
            self.assertIn("intro-1_start.state.gz: stage 0 area 0, 9 banks drawn", "\n".join(lines))
            docs = [json.loads(p.read_text()) for p in (root / "enemies").glob("*/animations.json")]
            junk = [d for d in docs if any(s["type"] == 1 for s in d["sources"])]
            self.assertEqual(len(junk), 1)
            self.assertEqual(sorted(s["found"] for s in junk[0]["sources"] if s["type"] == 1), ["live", "search"])
            self.assertEqual(len(docs), len({d["id"] for d in docs}))


if __name__ == "__main__":
    unittest.main()
```

- [ ] **Step 2: Run it with the real data**

Run: `MMX6_DUMPS=~/mmx-data/x6/dumps MMX6_EXTRACTED=~/mmx-data/x6/extracted/retail python3 -m unittest discover -s tests -p test_retail_sprites.py -v`
Expected: `Ran 3 tests`, `OK`. If the intro export stops on a `SpriteError` from a bank sheet, that is Review Focus item 1: record the bank and sprite in the ledger as a ruling, and decide (from the spec) whether the bank sheet skips that sprite with a line.

- [ ] **Step 3: Run the command on both intro states and the Yammark state; look at the output**

```bash
for s in intro-1_start intro-2_junkroid yammark-1_start; do python3 -m mmxdemake.spriteexport --state $s.state.gz; done
ls ~/mmx-data/x6/sprites/enemies
```
Expected: exit 0 each; one folder per export; the Junkroid folder has 8 `anim-*.png`. Open the four intro exports' `anim-0.png` and check by eye that they show Meta Dridler, Junkroid, Meta Wheel F and Hover Gunner.

- [ ] **Step 4: Add the four intro names**

Put the four IDs seen in Step 3 into `catalog/enemies.json` `names` (types 0 → `meta-dridler`, 1 → `junkroid`, 2 → `meta-wheel-f`, 5 → `hover-gunner`), then run once more:

```bash
python3 -m mmxdemake.spriteexport --state intro-1_start.state.gz
ls ~/mmx-data/x6/sprites/enemies
```
Expected: the four named folders; their ID folders are gone (this run exports them, so it writes them under the names and removes the old ID folders).

- [ ] **Step 5: Docs**

- `README.md`: a "Sprites and animations" section with the command, the output layout (`enemies/`, `banks/`, `stages/`), the names file, and the note that one save state per stage area is needed.
- `docs/TODO.md`, "Enemy sprites": tick "Animation sequences" and "Sprite sheet exporter"; add "Name the Yammark exports" and "Boss-room states for D-1000 and Yammark".
- `docs/process/process-log.md`: section 55, "Sprite and animation exporter": what it does, the three table sources, the content IDs, the run results (banks drawn, exports, skipped types with reasons) for the three states.

- [ ] **Step 6: Run the whole suite, then commit**

Run: `python3 -m unittest discover -s tests 2>&1 | tail -3` — Expected: `Ran 332 tests`, `OK (skipped=9)`.

```bash
git add tests/test_retail_sprites.py README.md docs/TODO.md docs/process/process-log.md catalog/enemies.json
git commit -m "Run the sprite exporter on the real states and name the intro enemies

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```
