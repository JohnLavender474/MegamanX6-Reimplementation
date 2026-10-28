import sys, struct, os
sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
sys.path.insert(0, os.path.expanduser('~/mmx-data/x6/re/tools'))
from mmxdemake.savestate import read_ram
from mips import dis
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'),'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/'+sys.argv[1]), EXE)
lo, hi, imm = int(sys.argv[2],16), int(sys.argv[3],16), int(sys.argv[4])
for a in range(lo, hi, 4):
    w = struct.unpack_from('<I', r, a & 0x1fffff)[0]
    if (w>>26) in (0x09,0x0a,0x0b) and ((w>>21)&31)==0 and (w&0xffff)==imm:
        ctx=[]
        for k in range(-3,5):
            pc=a+4*k; ww=struct.unpack_from('<I', r, pc & 0x1fffff)[0]; ctx.append(f'   {pc:08x}: {dis(ww,pc)}')
        print('\n'.join(ctx)); print()
