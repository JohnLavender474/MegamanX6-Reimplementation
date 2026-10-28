from probe import *
from mmxdemake.mips import disassemble
import sys
r=load(STATES[2])
lo=int(sys.argv[1],16); hi=int(sys.argv[2],16); pats=sys.argv[3:]
for pc in range(lo,hi,4):
    w=u32(r,pc); t=disassemble(w,pc)
    if any(p in t for p in pats): print(f"{pc:08x}: {w:08x}  {t}")
