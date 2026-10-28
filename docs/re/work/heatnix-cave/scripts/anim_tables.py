"""Print animation tables {ticks, flag, next, sprite} and the flame-jet box height table from a state.
Usage (repo root, PYTHONPATH=.): python3 anim_tables.py <state>"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram
ram = read_ram(Path.home() / "mmx-data/x6/dumps" / sys.argv[1],
               (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes())
W = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
def table(addr, n, maxf=16):
    for k in range(n):
        p = W(addr + 4 * k)
        fr = []
        for f in range(maxf):
            t, fl, nx, sp = struct.unpack_from("<BBbB", ram, (p + 4 * f) & 0x1FFFFF)
            fr.append((t, fl, nx, sp))
            if nx <= 0:
                break
        print(f"  anim {k} @ {p:#x}: {fr}")
print("flame jet 0x800F3CBC"); table(0x800F3CBC, 5)
print("box heights 0x800F3CD0", list(ram[0x800F3CD0 & 0x1FFFFF:(0x800F3CD0 & 0x1FFFFF) + 28]))
print("grey block 0x800F3DB0"); table(0x800F3DB0, 1, 24)
