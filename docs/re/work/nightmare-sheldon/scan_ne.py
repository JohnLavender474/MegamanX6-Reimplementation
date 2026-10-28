import sys, struct, os
sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
sys.path.insert(0, os.path.expanduser('~/mmx-data/x6/re/tools'))
from mmxdemake.savestate import read_ram
from mips import dis
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'),'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/'+sys.argv[1]), EXE)
lo, hi = int(sys.argv[2],16), int(sys.argv[3],16)
want = [int(x,16) for x in sys.argv[4:]]
for a in range(lo, hi, 4):
    w = struct.unpack_from('<I', r, a & 0x1fffff)[0]
    if (w & 0xffff) in want and (w>>26) >= 0x20 or ((w>>26)==0x09 and (w&0xffff) in want and ((w>>21)&31)==0):
        print(f'{a:08x}: {w:08x}  {dis(w,a)}')
