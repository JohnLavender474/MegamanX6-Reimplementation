"""Disassemble RAM of a save state. Usage (repo root, PYTHONPATH=.): python3 dis.py <state> <start hex> <end hex>"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
ram = read_ram(Path.home() / "mmx-data/x6/dumps" / sys.argv[1],
               (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes())
a, e = int(sys.argv[2], 16), int(sys.argv[3], 16)
while a < e:
    wd = struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
    print(f"{a:08x}: {wd:08x}  {disassemble(wd, a)}")
    a += 4
