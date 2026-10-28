"""Draw a player frame set with another sprite-directory entry (for example 5 for set 0, 19 for set 1).

usage: python3 alt_entry.py STATE.gz FRAME_ENTRY SPRITE_ENTRY OUT.png [first last]
Run from the mmx-demake-tools repo root (it imports mmxdemake). Read only.
"""
import gzip
import os
import struct
import sys

sys.path.insert(0, os.getcwd())
from mmxdemake.savestate import read_ram, ram_at
from mmxdemake.sprites import Bank, draw, scratchpad
from mmxdemake.vram import read_vram
from mmxdemake.packedframes import unpack, place, frame_count
from mmxdemake.spriteexport import grid
from mmxdemake.png import write_png

EXE = os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95')


def u32(r, a):
    return struct.unpack('<I', ram_at(r, a, 4))[0]


def u16(r, a):
    return struct.unpack('<H', ram_at(r, a, 2))[0]


def main():
    path, fe, se, out = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4]
    data = gzip.open(path).read()
    ram = read_ram(path, open(EXE, 'rb').read())
    vram = read_vram(data)
    s = scratchpad(data)
    fdir, sdir = struct.unpack_from('<I', s, 0x14)[0], struct.unpack_from('<I', s, 0x1C)[0]
    frames = (fdir + u32(ram, fdir + 4 * fe)) & 0xFFFFFFFF
    sdata = (sdir + u32(ram, sdir + 4 * se)) & 0xFFFFFFFF
    mega = 0x800970A0
    tex, clut, mode = u16(ram, mega + 0x40), u16(ram, mega + 0x42), ram_at(ram, mega, 1)[0] & 0x60
    bank = Bank(-1, None, sdata, u16(ram, sdata + 2), tex, clut)
    n = min(bank.count, frame_count(ram, frames, 1))
    lo, hi = (int(sys.argv[5]), int(sys.argv[6])) if len(sys.argv) > 6 else (0, n - 1)
    images = []
    for i in range(lo, hi + 1):
        tiles, packed, _ = unpack(ram, frames, i)
        images.append(draw(ram, place(vram, tiles, packed, tex), bank, i, mode))
    write_png(out, *grid(images))
    print(f'{out}: frames {frames:#x} sprites {sdata:#x} count {bank.count}, drew {lo}-{hi}')


main()
