"""List loads and stores with a given 16-bit offset in a RAM range of a save state, with
the instructions before and after. Usage (repo root, PYTHONPATH=.):
python3 scanimm.py <state.gz> <imm hex> <lo hex> <hi hex> [context]"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble

EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
DUMPS = Path.home() / "mmx-data/x6/dumps"
ram = read_ram(DUMPS / sys.argv[1], EXE.read_bytes())
imm, lo, hi = (int(x, 16) for x in sys.argv[2:5])
ctx = int(sys.argv[5]) if len(sys.argv) > 5 else 3
W = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
for a in range(lo, hi, 4):
    w = W(a)
    if (w >> 26) in (0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B) and (w & 0xFFFF) == imm:
        print(f"-- {a:08x}")
        for p in range(a - 4 * ctx, a + 4 * (ctx + 1), 4):
            print(f"   {p:08x}: {disassemble(W(p), p)}")
