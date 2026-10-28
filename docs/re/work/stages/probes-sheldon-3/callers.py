import sys, os, struct
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
tg = {int(a, 16) for a in sys.argv[2:]}
for pc in range(0x80010000, 0x80100000, 4):
    w = struct.unpack_from('<I', r, pc & 0x1fffff)[0]
    if (w >> 26) in (2, 3) and (((w & 0x3FFFFFF) << 2) | 0x80000000) in tg:
        print(hex(pc), disassemble(w, pc))
