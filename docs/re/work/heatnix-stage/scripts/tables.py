"""Print the entries of the update tables (scratchpad pointers) that point into the stage
overlay or the boss overlay. Usage (repo root, PYTHONPATH=.): python3 tables.py <state.gz>"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_state, read_ram
from mmxdemake.sprites import scratchpad

EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
DUMPS = Path.home() / "mmx-data/x6/dumps"
st = read_state(DUMPS / sys.argv[1])
ram = read_ram(DUMPS / sys.argv[1], EXE.read_bytes())
sp = scratchpad(st)
W = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
for name, off, n in (("main", 0x80, 77), ("visual", 0x84, 58), ("effect", 0x88, 40), ("logic", 0x8C, 45),
                     ("misc", 0x90, 64), ("stage", 0x94, 32), ("ray", 0x98, 8)):
    t = struct.unpack_from("<I", sp, off)[0]
    print(f"{name} table {t:#x}")
    for i in range(n):
        e = W(t + 4 * i)
        if e >= 0x800E9860:
            print(f"   {name} {i}: {e:#x}")
