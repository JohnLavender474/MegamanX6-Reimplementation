from wp import *
import re
r=load(sys.argv[1]); lo=int(sys.argv[2],16); hi=int(sys.argv[3],16); pat=re.compile(sys.argv[4])
for pc in range(lo,hi,4):
    t=disassemble(u32(r,pc),pc)
    if pat.search(t): print(f"{pc:08x} {t}")
