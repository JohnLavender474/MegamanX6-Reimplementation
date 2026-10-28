import sys, struct, os
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
for arg in sys.argv[2:]:
    v = struct.pack('<I', int(arg, 16))
    i = r.find(v)
    hits = []
    while i >= 0 and len(hits) < 20:
        if i % 4 == 0:
            hits.append(hex(0x80000000 + i))
        i = r.find(v, i + 1)
    print(arg, hits)
