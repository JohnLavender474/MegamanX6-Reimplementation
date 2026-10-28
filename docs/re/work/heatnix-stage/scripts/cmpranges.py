"""Compare two code ranges in two save states word by word. Immediates of I-type
instructions and jump targets are masked, so relocated code compares equal; branch offsets
are masked too. Usage (repo root, PYTHONPATH=.):
python3 cmpranges.py <stateA> <startA hex> <stateB> <startB hex> <length hex>"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram

EXE = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()
DUMPS = Path.home() / "mmx-data/x6/dumps"
sa, a0, sb, b0, n = sys.argv[1], int(sys.argv[2], 16), sys.argv[3], int(sys.argv[4], 16), int(sys.argv[5], 16)
ra, rb = read_ram(DUMPS / sa, EXE), read_ram(DUMPS / sb, EXE)


def norm(w):
    op = w >> 26
    if op in (2, 3):
        return w & 0xFC000000
    if op != 0:
        return w & 0xFFFF0000
    return w


same = diff = raw = 0
for i in range(0, n, 4):
    wa = struct.unpack_from("<I", ra, (a0 + i) & 0x1FFFFF)[0]
    wb = struct.unpack_from("<I", rb, (b0 + i) & 0x1FFFFF)[0]
    raw += wa == wb
    if norm(wa) == norm(wb):
        same += 1
    else:
        diff += 1
        print(f"  {a0 + i:08x} {wa:08x}  {b0 + i:08x} {wb:08x}")
print(f"{n // 4} words: {same} equal after masking, {diff} differ; {raw} equal raw")
