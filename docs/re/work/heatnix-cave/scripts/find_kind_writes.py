"""Find stores to byte 3 (record kind) near a constant 0x20/0x30/0x40/0x60 or 0xF0 mask, in the main program
and the loaded overlay. Usage (repo root, PYTHONPATH=.): python3 find_kind_writes.py <state>"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
ram = read_ram(Path.home() / "mmx-data/x6/dumps" / sys.argv[1],
               (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes())
W = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
for start, end in ((0x80010000, 0x80070000), (0x800E9860, 0x800F4000)):
    for a in range(start, end, 4):
        wd = W(a)
        if wd >> 26 == 0x28 and (wd & 0xFFFF) == 3:
            near = [disassemble(W(p), p) for p in range(a - 24, a + 4, 4)]
            txt = " | ".join(near)
            if any(k in txt for k in ("0x20", "0x30", "0x40", "0x60", "0xf0", ",32", ",48", ",16")):
                print(f"{a:08x}: {txt}")
