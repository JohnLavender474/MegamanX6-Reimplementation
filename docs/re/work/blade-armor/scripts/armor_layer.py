"""Draw X's armour layer (the second sprite entry, mega+0xDC) as the game does in 0x80025984,
and a composite of the body layer and the armour layer.

usage: python3 armor_layer.py STATE.gz SET OUTDIR [first last]
SET 0: frames entry 0, body sprites entry 0, armour sprites entry 5.
SET 1: frames entry 10, body sprites entry 18, armour sprites entry 19.
Writes OUTDIR/armor-SET.png, body-SET.png, both-SET.png (armour on top) and under-SET.png
(body on top). Run from the mmx-demake-tools repo root. Read only.
"""
import gzip
import os
import struct
import sys

sys.path.insert(0, os.getcwd())
import mmxdemake.sprites as sp
from mmxdemake.savestate import read_ram, ram_at
from mmxdemake.vram import read_vram
from mmxdemake.packedframes import unpack, place
from mmxdemake.spriteexport import grid
from mmxdemake.png import write_png

EXE = os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95')
LAYER_CLUTS = 0x80071E68
FAKE = 0x7F00
SETS = {0: (0, 0, 5), 1: (10, 18, 19)}

real_palette_words = sp.palette_words
clut_map = {}


def palette_words(vram, clut, n=16):
    return real_palette_words(vram, clut_map.get(clut, clut), n)


sp.palette_words = palette_words


def u32(r, a):
    return struct.unpack('<I', ram_at(r, a, 4))[0]


def u16(r, a):
    return struct.unpack('<H', ram_at(r, a, 2))[0]


def merge(top, under):
    canvas = {}
    for im in (under, top):
        for y in range(im.height):
            for x in range(im.width):
                c = im.pixels[y][x]
                if c[3]:
                    canvas[(x - im.ox, y - im.oy)] = c
    return sp._image(canvas)


def main():
    path, which, out = sys.argv[1], int(sys.argv[2]), sys.argv[3]
    data = gzip.open(path).read()
    ram = read_ram(path, open(EXE, 'rb').read())
    vram = read_vram(data)
    s = sp.scratchpad(data)
    fdir, sdir = struct.unpack_from('<I', s, 0x14)[0], struct.unpack_from('<I', s, 0x1C)[0]
    fe, be, ae = SETS[which]
    frames = (fdir + u32(ram, fdir + 4 * fe)) & 0xFFFFFFFF
    body_data = (sdir + u32(ram, sdir + 4 * be)) & 0xFFFFFFFF
    armor_data = (sdir + u32(ram, sdir + 4 * ae)) & 0xFFFFFFFF
    mega = 0x800970A0
    tex, clut, mode = u16(ram, mega + 0x40), u16(ram, mega + 0x42), ram_at(ram, mega, 1)[0] & 0x60
    for k in range(4):
        clut_map[FAKE + k] = u16(ram, LAYER_CLUTS + 2 * k)
    body = sp.Bank(-1, None, body_data, u16(ram, body_data + 2), tex, clut)
    armor = sp.Bank(-1, None, armor_data, u16(ram, armor_data + 2), tex, FAKE)
    n = min(body.count, armor.count)
    lo, hi = (int(sys.argv[4]), int(sys.argv[5])) if len(sys.argv) > 5 else (0, n - 1)
    a_img, b_img, both, under = [], [], [], []
    for i in range(lo, hi + 1):
        tiles, packed, _ = unpack(ram, frames, i)
        v = place(vram, tiles, packed, tex)
        a = sp.draw(ram, v, armor, i, mode)
        b = sp.draw(ram, v, body, i, mode)
        a_img.append(a)
        b_img.append(b)
        both.append(merge(a, b))
        under.append(merge(b, a))
    os.makedirs(out, exist_ok=True)
    for name, imgs in (('armor', a_img), ('body', b_img), ('both', both), ('under', under)):
        write_png(os.path.join(out, f'{name}-{which}.png'), *grid(imgs))
    print(f'set {which}: frames {frames:#x} body {body_data:#x} armour {armor_data:#x}, '
          f'cluts {[hex(clut_map[FAKE + k]) for k in range(4)]}, drew {lo}-{hi}')


main()
