import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home()/'IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
X = Path.home()/'mmx-data/x6'
exe = (X/'extracted/retail/iso/SLUS_013.95').read_bytes()
st = read_state(X/'dumps'/sys.argv[1]); ram = ram_from(st, exe, sys.argv[1]); sp = scratchpad(st)
def w(a): return struct.unpack_from('<I', ram, a&0x1fffff)[0]
for off, n, name in ((0x8C, 45, 'logic'), (0x94, 32, 'stage')):
    t = struct.unpack_from('<I', sp, off)[0]
    print(name, hex(t), ' '.join(f'{i}:{w(t+4*i):08x}' for i in range(n)))
for off, n, name in ((0x80, 77, 'main'), (0x84, 58, 'visual'), (0x90, 64, 'misc'), (0x88, 40, 'effect')):
    t = struct.unpack_from('<I', sp, off)[0]
    print(name, hex(t), ' '.join(f'{i}:{w(t+4*i):08x}' for i in range(n) if 0x800E9860 <= w(t+4*i) < 0x800F4020))
print('---')
for off in range(0x80, 0xA0, 4):
    t = struct.unpack_from('<I', sp, off)[0]
    if not (0x80000000 <= t < 0x80200000): continue
    for i in range(80):
        if w(t+4*i) in (0x80046e14, 0x80046dd4): print('scratch', hex(off), 'table', hex(t), 'index', i, hex(w(t+4*i)))
print([hex(w(0x80071300+4*i)) for i in range(48)])
