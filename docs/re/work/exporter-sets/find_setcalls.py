import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.mips import disassemble
from mmxdemake.animfind import function_start
X = Path.home() / "mmx-data/x6/extracted/retail"
D = Path.home() / "mmx-data/x6/dumps"
path = D / sys.argv[1]
lo, hi = int(sys.argv[2], 16), int(sys.argv[3], 16)
ram = ram_from(read_state(path), (X / "iso/SLUS_013.95").read_bytes(), path)
u32 = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
for pc in range(lo, hi, 4):
    w = u32(pc)
    if w >> 26 == 0x23 and w & 0xFFFF == 0x20:
        prev = [u32(pc - 4 * k) for k in range(1, 4)]
        if any(p >> 26 == 15 and p & 0xFFFF == 0x1F80 for p in prev):
            ctx = [f"{pc + 4*k:08x} {disassemble(u32(pc + 4*k), pc + 4*k)}" for k in range(0, 7)]
            print(f"func {function_start(ram, pc, lo):08x}:", " | ".join(ctx))
