import sys, struct, os
sys.path.insert(0, os.path.expanduser('~/mmx-data/x6/re/tools'))
from mips import dis
R = os.path.expanduser('~/mmx-data/x6/extracted/retail')
imms = {int(x, 16) for x in sys.argv[1:]}
files = [('exe', os.path.join(R, 'iso/SLUS_013.95'), 0x800)] + [(f, os.path.join(R, 'rock_bin', f), 0) for f in sorted(os.listdir(os.path.join(R, 'rock_bin')))]
for name, path, skip in files:
    b = open(path, 'rb').read()[skip:]
    for o in range(0, len(b) - 3, 4):
        w = struct.unpack_from('<I', b, o)[0]
        op = w >> 26
        if op in (0x08, 0x09, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2b) and (w & 0xffff) in imms:
            print(name, hex(o), dis(w, o))
