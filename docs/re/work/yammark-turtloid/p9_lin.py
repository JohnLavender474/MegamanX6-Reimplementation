import sys
from probe_ram import *
from mmxdemake.mips import disassemble
ram, scr = load(sys.argv[1])
a, b = int(sys.argv[2], 16), int(sys.argv[3], 16)
for pc in range(a, b, 4):
    print(f"{pc:08x}: {disassemble(u32(ram, pc), pc)}")
