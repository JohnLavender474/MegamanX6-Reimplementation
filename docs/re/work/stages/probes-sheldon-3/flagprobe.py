import sys, os, struct
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
from mmxdemake.mips import disassemble
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
for st in sys.argv[1:]:
    p = os.path.expanduser('~/mmx-data/x6/dumps/' + st)
    d = read_state(p); r = ram_from(d, EXE, p); s = scratchpad(d)
    print(st, 'eab0', r[0x8EAB0], 'd6a', struct.unpack_from('<H', r, 0x90D6A)[0],
          'sp+44', hex(struct.unpack_from('<I', s, 0x44)[0]), 'sp+40', hex(struct.unpack_from('<I', s, 0x40)[0]),
          'sp+48', hex(struct.unpack_from('<I', s, 0x48)[0]))
r = ram_from(read_state(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1])), EXE, '')
for pc in range(0x80010000, 0x80100000, 4):
    w = struct.unpack_from('<I', r, pc & 0x1fffff)[0]
    if (w & 0xFFFF) in (0xEAB0, 0x0D6A) and (w >> 26) in (0x20, 0x21, 0x24, 0x25, 0x28, 0x29):
        print(hex(pc), disassemble(w, pc))
for pc in range(0x80010000, 0x80100000, 4):
    w = struct.unpack_from('<I', r, pc & 0x1fffff)[0]
    if (w >> 26) == 3 and ((w & 0x3FFFFFF) << 2) | 0x80000000 in (0x800663FC, 0x80027A50):
        print('call', hex(pc), disassemble(w, pc))
