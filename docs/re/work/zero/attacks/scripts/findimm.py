from zprobe import *
from mmxdemake.mips import disassemble
r=load(sys.argv[1]); imm=int(sys.argv[2],16)
for a in range(0x80010000,0x80200000,4):
    w=u32(r,a); op=w>>26
    if op in (0x28,0x24,0x20) and (w&0xffff)==imm:
        print(hex(a), disassemble(w,a))
