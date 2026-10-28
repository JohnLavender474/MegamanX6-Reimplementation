import sys, struct, os
sys.path.insert(0, os.path.expanduser('~/mmx-data/x6/re/tools'))
from mips import dis
R = os.path.expanduser('~/mmx-data/x6/extracted/retail')
name, off, before, after = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3]), int(sys.argv[4])
base = int(sys.argv[5], 16) if len(sys.argv) > 5 else 0
path = os.path.join(R, 'iso/SLUS_013.95') if name == 'exe' else os.path.join(R, 'rock_bin', name)
skip = 0x800 if name == 'exe' else 0
b = open(path, 'rb').read()[skip:]
for o in range(off - 4 * before, off + 4 * after, 4):
    w = struct.unpack_from('<I', b, o)[0]
    print(f'{base + o:08x}: {dis(w, base + o)}')
