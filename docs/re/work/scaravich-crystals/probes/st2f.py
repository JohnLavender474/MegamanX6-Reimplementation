from ramdis import ram, u32
from mmxdemake.mips import disassemble
r = ram('scaravich-1_start.state.gz')
for a in range(0x80010000, 0x80100000, 4):
    w = u32(r, a); op = w >> 26
    if op in (0x28, 0x20, 0x24) and (w & 0xFFFF) in (0x2F, 0xCEFF):
        print(hex(a), disassemble(w, a))
