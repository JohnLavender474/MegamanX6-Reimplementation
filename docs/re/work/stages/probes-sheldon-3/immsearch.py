import sys, struct, os
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
lo, hi = int(sys.argv[2], 16), int(sys.argv[3], 16)
vals = [int(v, 0) for v in sys.argv[4:]]
win = 24
for pc in range(lo, hi, 4):
    w = struct.unpack_from('<I', r, pc & 0x1fffff)[0]
    op = w >> 26
    if op in (9, 13) and (w & 0xFFFF) == vals[0]:
        near = set()
        for d in range(-win, win + 1):
            w2 = struct.unpack_from('<I', r, (pc + 4 * d) & 0x1fffff)[0]
            if (w2 >> 26) in (9, 13):
                near.add(w2 & 0xFFFF)
        if all(v in near for v in vals[1:]):
            print(hex(pc), disassemble(w, pc))
