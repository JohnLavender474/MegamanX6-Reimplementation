import os
import struct
import sys

sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.psxcolor import to_rgba
from mmxdemake.savestate import ram_from, read_state
from mmxdemake.sprites import SpriteError, Bank, draw, scratchpad, with_source
from mmxdemake.vram import read_vram

EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
MISC = (0x800CD410, 96, 0x60)
CAMERA = 0x80097202
ORIGINS = [(0, 0), (0, 240), (0, 256), (320, 0), (256, 0), (512, 0), (0, 16), (0, 272)]


def u16(r, a):
    return struct.unpack_from('<H', r, a & 0x1fffff)[0]


def s16(r, a):
    return struct.unpack_from('<h', r, a & 0x1fffff)[0]


def u32(r, a):
    return struct.unpack_from('<I', r, a & 0x1fffff)[0]


def pixel(vram, x, y):
    return to_rgba(struct.unpack_from('<H', vram, y * 2048 + x * 2)[0])


for st in sys.argv[1:]:
    path = os.path.expanduser('~/mmx-data/x6/dumps/' + st)
    data = read_state(path)
    r = ram_from(data, EXE, path)
    vram = read_vram(data)
    cx, cy = s16(r, CAMERA), s16(r, CAMERA + 4)
    print('==', st, 'camera', cx, cy)
    pieces = []
    for i in range(MISC[1]):
        a = MISC[0] + i * MISC[2]
        if r[a & 0x1fffff] == 0 or r[(a & 0x1fffff) + 1] != 7:
            continue
        d = u32(r, a + 0x3C)
        tex, clut = u16(r, a + 0x40), u16(r, a + 0x42) & 0x7fff
        bank = with_source(r, Bank(-1, 7, d, 0, None, clut), d, tex, clut)
        if not pieces:
            bad = []
            for n in range(bank.count):
                try:
                    draw(r, vram, bank, n)
                except SpriteError as e:
                    bad.append((n, str(e)))
            print('  source', hex(d), 'count', bank.count, 'draw errors', bad)
        n = r[(a & 0x1fffff) + 0x47]
        im = draw(r, vram, bank, n)
        pieces.append((i, s16(r, a + 10), s16(r, a + 14), r[(a & 0x1fffff) + 0x15], n, im))
    for i, x, y, flip, n, im in pieces:
        best = []
        for ox0, oy0 in ORIGINS:
            for shift in ((0,) if not flip else (0, -1, 1)):
                hit = tot = 0
                for yy, line in enumerate(im.pixels):
                    for xx, p in enumerate(line):
                        if not p[3]:
                            continue
                        rx = xx - im.ox
                        sx = x - cx + (-rx + shift if flip else rx)
                        sy = y - cy + yy - im.oy
                        if not (0 <= sx < 320 and 0 <= sy < 240):
                            continue
                        tot += 1
                        hit += pixel(vram, ox0 + sx, oy0 + sy)[:3] == tuple(p)[:3]
                if tot:
                    best.append((hit / tot, hit, tot, (ox0, oy0), shift))
        best.sort(reverse=True)
        print(f'  misc[{i}] sprite {n} at {x},{y} flip {flip:#x} size {im.width}x{im.height} origin {im.ox},{im.oy}'
              f' best {best[:2]}')
