import os
import struct
import sys
from pathlib import Path

sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import ram_from, read_state
from mmxdemake.sprites import SpriteError, banks, scratchpad

EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
CLASSES = [('visual', 0x80093C98, 32, 0xA0), ('misc', 0x800CD410, 96, 0x60), ('misc2', 0x800964A0, 32, 0x60),
           ('effect', 0x80091298, 96, 0x70)]


def u16(r, a):
    return struct.unpack_from('<H', r, a & 0x1fffff)[0]


def u32(r, a):
    return struct.unpack_from('<I', r, a & 0x1fffff)[0]


for path in sorted(Path('~/mmx-data/x6/dumps').expanduser().glob('*.state.gz')):
    data = read_state(path)
    r = ram_from(data, EXE, path)
    scratch = scratchpad(data)
    base = struct.unpack_from('<I', scratch, 0x20)[0]
    try:
        bl = banks(r, scratch)
    except SpriteError as e:
        print(path.name, 'error', e)
        continue
    entries = u32(r, base) // 4
    entry_of = {base + u32(r, base + 4 * i): i for i in range(entries)}
    bank_data = {b.data for b in bl}
    found = {}
    for name, cb, n, sz in CLASSES:
        for i in range(n):
            a = cb + i * sz
            if r[a & 0x1fffff] == 0:
                continue
            d = u32(r, a + 0x3C)
            if not d or d in bank_data:
                continue
            key = (name, r[(a & 0x1fffff) + 1], d, u16(r, a + 0x40), u16(r, a + 0x42) & 0x7fff, r[a & 0x1fffff] & 0x60)
            found.setdefault(key, []).append(r[(a & 0x1fffff) + 0x47])
    print(path.name, 'stage', r[0xCCEDC], 'banks', len(bl), 'entries', entries)
    for key, spr in sorted(found.items()):
        name, t, d, tex, clut, mode = key
        print(f'   {name} type {t} data {d:#x} entry {entry_of.get(d)} tex {tex:#x} clut {clut:#x} mode {mode:#x} '
              f'sprites {sorted(set(spr))}')
