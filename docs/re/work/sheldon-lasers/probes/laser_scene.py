import os
import struct
import sys

sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.png import write_png
from mmxdemake.savestate import ram_from, read_state
from mmxdemake.sprites import Bank, draw, with_source
from mmxdemake.vram import read_vram

EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
POOLS = [(0x8008EF48, 48, 0x9C, (13,)), (0x800CD410, 96, 0x60, (7,))]


def u16(r, a):
    return struct.unpack_from('<H', r, a & 0x1fffff)[0]


def s16(r, a):
    return struct.unpack_from('<h', r, a & 0x1fffff)[0]


def u32(r, a):
    return struct.unpack_from('<I', r, a & 0x1fffff)[0]


st, out, x0, y0, w, h = sys.argv[1], sys.argv[2], *map(int, sys.argv[3:7])
path = os.path.expanduser('~/mmx-data/x6/dumps/' + st)
data = read_state(path)
r = ram_from(data, EXE, path)
vram = read_vram(data)
objs = []
for base, n, size, types in POOLS:
    for i in range(n):
        a = base + i * size
        if r[a & 0x1fffff] == 0 or r[(a & 0x1fffff) + 1] not in types:
            continue
        d, tex, clut = u32(r, a + 0x3C), u16(r, a + 0x40), u16(r, a + 0x42) & 0x7fff
        bank = with_source(r, Bank(-1, 0, d, 0, None, clut), d, tex, clut)
        objs.append((-r[(a & 0x1fffff) + 0x16], s16(r, a + 10), s16(r, a + 14), r[(a & 0x1fffff) + 0x15],
                     draw(r, vram, bank, r[(a & 0x1fffff) + 0x47])))
canvas = [bytearray(bytes((40, 40, 60, 255)) * w) for _ in range(h)]
for _, x, y, flip, im in sorted(objs, key=lambda o: o[0]):
    for yy, line in enumerate(im.pixels):
        for xx, p in enumerate(line):
            if not p[3]:
                continue
            rx = xx - im.ox
            sx = x - x0 + (-rx - 1 if flip & 0x40 else rx)
            sy = y - y0 + yy - im.oy
            if 0 <= sx < w and 0 <= sy < h:
                canvas[sy][sx * 4:sx * 4 + 4] = bytes(p)
write_png(out, w, h, [bytes(c) for c in canvas])
