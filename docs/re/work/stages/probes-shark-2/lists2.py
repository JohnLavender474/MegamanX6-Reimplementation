import sys, struct, os
from collections import Counter
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
def u16(a): return struct.unpack_from('<H', r, a & 0x1fffff)[0]
def u32(a): return struct.unpack_from('<I', r, a & 0x1fffff)[0]
K = {0: 'main', 1: 'main', 2: 'main', 3: 'logic', 4: 'stage', 5: 'misc', 6: 'ray', 7: 'kind7'}
for name, tbl in (('enemy_set', 0x8007349C), ('fixed_set', 0x80073554)):
    a = u32(tbl + 4 * 8 + 1 * 4)
    print(name, hex(a))
    n = 0
    c = Counter()
    while True:
        b = r[a & 0x1fffff:(a & 0x1fffff) + 8]
        fl, ty, i, k = b[:4]
        x, y = struct.unpack_from('<HH', r, (a & 0x1fffff) + 4)
        if k & 15 == 15:
            print(f'  {n:3d} end'); break
        c[(K.get(k & 15), ty, k & 15)] += 1
        print(f'  {n:3d} {a:#x} a {fl:02x} type {ty:3d} id {i:3d} kind {k:#04x} ({K.get(k & 15)}) x {x:5d} y {y:5d}')
        n += 1; a += 8
    for kk, v in sorted(c.items(), key=str): print('   ', kk, v)
z = u32(0x80079D70 + (4 * 2 + 1) * 4)
print('camera zones at', hex(z))
for j in range(6):
    v = [u16(z + j * 16 + 2 * q) for q in range(8)]
    print('  zone', j, v)
