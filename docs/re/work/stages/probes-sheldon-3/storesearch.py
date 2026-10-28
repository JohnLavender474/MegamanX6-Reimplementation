import sys, os, struct
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
vals = {int(v, 16) for v in sys.argv[2:]}
for pc in range(0x80010000, 0x80100000, 4):
    w = struct.unpack_from('<I', r, pc & 0x1fffff)[0]
    if (w >> 26) in (9, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2b) and (w & 0xFFFF) in vals:
        print(hex(pc), disassemble(w, pc))
