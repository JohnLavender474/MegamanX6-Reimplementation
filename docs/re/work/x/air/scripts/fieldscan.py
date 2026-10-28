import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble

state = Path.home() / "mmx-data/x6/dumps" / sys.argv[1]
exe = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()
ram = read_ram(state, exe)
off = int(sys.argv[2], 16)
kinds = sys.argv[3] if len(sys.argv) > 3 else "sw"
lo, hi = 0x80010000, int(sys.argv[4], 16) if len(sys.argv) > 4 else 0x80200000
ops = {"s": (0x28, 0x29, 0x2B), "l": (0x20, 0x21, 0x23, 0x24, 0x25)}
want = set()
for k in kinds:
    want.update(ops[k])
for a in range(lo, hi, 4):
    w = struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
    if (w >> 26) in want and (w & 0xFFFF) == off:
        print(f"{a:08x}: {disassemble(w, a)}")
