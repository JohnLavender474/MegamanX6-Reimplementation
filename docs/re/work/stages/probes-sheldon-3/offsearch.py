import sys, os, struct
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
lo, hi = int(sys.argv[2], 16), int(sys.argv[3], 16)
offs = {int(v, 16) for v in sys.argv[4:]}
for pc in range(lo, hi, 4):
    w = struct.unpack_from('<I', r, pc & 0x1fffff)[0]
    if (w >> 26) in (0x28, 0x29, 0x2b) and (w & 0xFFFF) in offs:
        print(hex(pc), disassemble(w, pc))
