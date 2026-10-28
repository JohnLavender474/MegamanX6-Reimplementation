import sys, os, struct
from mmxdemake.savestate import read_ram
from mmxdemake.codewalk import constants, is_function_start
from mmxdemake.mips import disassemble
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
lo, hi = int(sys.argv[2], 16), int(sys.argv[3], 16)
want = [tuple(int(v, 16) for v in w.split('-')) if '-' in w else (int(w, 16), int(w, 16) + 1) for w in sys.argv[4:]]
def u32(a): return struct.unpack_from('<I', r, a & 0x1fffff)[0]
def fstart(pc):
    a = pc
    while a > lo:
        if is_function_start(r, a): return a
        a -= 4
    return None
for pc, v in constants(r, lo, hi):
    if any(a <= v < b for a, b in want):
        print(f'{pc:#x} in fn {fstart(pc) or 0:#x}: {v:#x}  {disassemble(u32(pc), pc)}')
