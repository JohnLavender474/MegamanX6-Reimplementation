import sys, struct, os
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
p = os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1])
d = read_state(p); r = ram_from(d, EXE, p); s = scratchpad(d)
def u32(a): return struct.unpack_from('<I', r, a & 0x1fffff)[0]
for name, off, types in (('main', 0x80, sys.argv[2]), ('stage', 0x94, sys.argv[3]), ('logic', 0x8C, sys.argv[4])):
    base = struct.unpack_from('<I', s, off)[0]
    for t in map(int, types.split(',')):
        e = u32(base + 4 * t)
        print(f'{name} {t}: table {base:#x} entry {e:#x} {"overlay" if e >= 0x800E9860 else "main program"}')
