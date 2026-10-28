import sys, struct, os
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
a = int(sys.argv[2], 16)
n = int(sys.argv[3])
for i in range(n):
    pc = a + 4 * i
    w = struct.unpack_from('<I', r, pc & 0x1fffff)[0]
    print(f'{pc:08x}: {w:08x}  {disassemble(w, pc)}')
