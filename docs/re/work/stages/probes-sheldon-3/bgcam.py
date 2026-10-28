import sys, os, struct
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
for st in sys.argv[1:]:
    r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + st), EXE)
    out = [st.split('_')[0]]
    for l in range(3):
        b = 0x971F8 + l * 0x54
        c = r[b:b + 0x54]
        out.append(f'L{l} st {c[4]}/{c[5]}/{c[6]} pos {struct.unpack_from("<i", c, 8)[0]:#x},{struct.unpack_from("<i", c, 12)[0]:#x} '
                   f'44={c[0x44]} raw30-54={c[0x2c:0x54].hex()}')
    print('\n  '.join(out))
