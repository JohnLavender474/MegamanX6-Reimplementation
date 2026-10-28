import sys, os
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
for st in sys.argv[1:]:
    r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + st), EXE)
    print(st.split('_')[0], 'cp', r[0xCCEED], ' '.join(r[0xC4778 + 0x30 * i:0xC4778 + 0x30 * i + 8].hex() for i in range(4)))
