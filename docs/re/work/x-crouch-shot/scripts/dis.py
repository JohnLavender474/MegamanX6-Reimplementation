import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
from mmxdemake.mips import disassemble
r = read_state(sys.argv[1])[RAM_OFFSET:RAM_OFFSET + RAM_SIZE]
start = int(sys.argv[2], 16); n = int(sys.argv[3])
for i in range(n):
    a = start + 4 * i
    w = struct.unpack('<I', r[(a & 0x1FFFFF):(a & 0x1FFFFF) + 4])[0]
    print(f'{a:08x}: {w:08x}  {disassemble(w, a)}')
