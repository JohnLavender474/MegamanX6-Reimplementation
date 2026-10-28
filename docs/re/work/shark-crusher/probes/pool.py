import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home()/'IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
X = Path.home()/'mmx-data/x6'
exe = (X/'extracted/retail/iso/SLUS_013.95').read_bytes()
ram = read_ram(X/'dumps'/sys.argv[1], exe)
def r(a, n): return ram[a&0x1fffff:(a&0x1fffff)+n]
ovs = {int(p.stem): p.read_bytes() for p in (X/'extracted/retail/rock_bin').glob('*.bin')}
for n, b in sorted(ovs.items()):
    m = sum(1 for i in range(0, min(len(b), 0x4000)) if r(0x800E9860+i,1)[0]==b[i])
    if m > 0x3000: print('overlay', n, 'matches', m, 'len', hex(len(b)))
print('stage/area', r(0x800CCEDC,2).hex())
for i in range(80):
    a = 0x800C1220 + i*0xA4
    o = r(a, 0xA4)
    if o[0]:
        x, y = struct.unpack_from('<hh', o, 0x1A)
        print(f'{i:2} {a:08x} ' + o[:0x20].hex(' '))
