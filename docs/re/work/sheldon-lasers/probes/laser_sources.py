import os
import struct
import sys

sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import ram_from, read_state
from mmxdemake.sprites import banks, scratchpad, type_list

EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
CLASSES = [('main', 0x8008EF48, 48, 0x9C), ('visual', 0x80093C98, 32, 0xA0), ('misc', 0x800CD410, 96, 0x60),
           ('misc2', 0x800964A0, 32, 0x60), ('stage', 0x800C1220, 80, 0xA4)]


def u16(r, a):
    return struct.unpack_from('<H', r, a & 0x1fffff)[0]


def s16(r, a):
    return struct.unpack_from('<h', r, a & 0x1fffff)[0]


def u32(r, a):
    return struct.unpack_from('<I', r, a & 0x1fffff)[0]


for st in sys.argv[1:]:
    path = os.path.expanduser('~/mmx-data/x6/dumps/' + st)
    data = read_state(path)
    r = ram_from(data, EXE, path)
    scratch = scratchpad(data)
    base = struct.unpack_from('<I', scratch, 0x20)[0]
    entries = u32(r, base) // 4
    print('==', st, 'stage', r[0xCCEDC], 'area', r[0xCCEDD], 'base', hex(base), 'entries', entries,
          'types', type_list(r))
    for i in range(entries):
        d = base + u32(r, base + 4 * i)
        print(f'  entry {i}: data {d:#x} count {u16(r, d + 2)}')
    for b in banks(r, scratch):
        print('  bank', b.index, 'type', b.type, 'tex', b.texture, 'clut', hex(b.clut), 'count', b.count)
    for name, cb, n, sz in CLASSES:
        for i in range(n):
            a = cb + i * sz
            if r[a & 0x1fffff] == 0:
                continue
            t = r[(a & 0x1fffff) + 1]
            if name == 'stage' or (name == 'main' and t != 13 and t != 32) or (name == 'visual' and t != 11) or (
                    name.startswith('misc') and t != 7):
                continue
            print(f'  {name}[{i}] @{a:#x} flags {r[a & 0x1fffff]:#x} type {t} var {r[(a & 0x1fffff) + 2]} '
                  f'st {r[(a & 0x1fffff) + 4]} x {s16(r, a + 10)} y {s16(r, a + 14)} flip {r[(a & 0x1fffff) + 0x15]:#x} '
                  f'prio {r[(a & 0x1fffff) + 0x16]} anim {u32(r, a + 0x30):#x} data {u32(r, a + 0x3C):#x} '
                  f'tex {u16(r, a + 0x40):#x} clut {u16(r, a + 0x42):#x} spr {r[(a & 0x1fffff) + 0x47]} '
                  f'w54 {s16(r, a + 0x54)} w56 {s16(r, a + 0x56)} x2 {s16(r, a + 8)} y2 {s16(r, a + 12)}')
