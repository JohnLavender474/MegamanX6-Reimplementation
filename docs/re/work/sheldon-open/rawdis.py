import sys, struct, os
sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.mips import disassemble
data = open(sys.argv[1], 'rb').read(); base = int(sys.argv[2], 16)
with open(sys.argv[3], 'w') as f:
    for o in range(0, len(data) - 3, 4):
        x = struct.unpack_from('<I', data, o)[0]; pc = base + o
        f.write(f'{pc:08x}: {x:08x}  {disassemble(x, pc)}\n')
