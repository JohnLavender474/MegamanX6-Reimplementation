import sys, s7probe as P
st = sys.argv[1]; lo = int(sys.argv[2], 16); hi = int(sys.argv[3], 16); out = sys.argv[4]
r = P.ram(st)
from mmxdemake.mips import disassemble
with open(out, 'w') as f:
    for pc in range(lo, hi, 4):
        x = P.w(r, pc)
        f.write(f'{pc:08x}: {x:08x}  {disassemble(x, pc)}\n')
