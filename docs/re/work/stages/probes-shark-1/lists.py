import sys, struct, os
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
stage, area = int(sys.argv[2]), int(sys.argv[3])
def u16(a): return struct.unpack_from('<H', r, a & 0x1fffff)[0]
def s16(a): return struct.unpack_from('<h', r, a & 0x1fffff)[0]
def u32(a): return struct.unpack_from('<I', r, a & 0x1fffff)[0]
e = u32(0x8007349C + stage * 8 + area * 4)
f = u32(0x80073554 + stage * 8 + area * 4)
print('enemy_set', hex(e), 'fixed_set', hex(f))
a = f
n = 0
while True:
    b = r[a & 0x1fffff:(a & 0x1fffff) + 8]
    fl, ty, i, k = b[0], b[1], b[2], b[3]
    x, y = struct.unpack_from('<HH', b, 4)
    print(f'  {n:3d} {a:#x} flags {fl:02x} type {ty} id {i} kind {k} x {x} y {y}')
    n += 1
    a += 8
    if k & 0x0F == 15 or n > 80:
        break
z = u32(0x80079D70 + (stage * 2 + area) * 4)
print('camera zones at', hex(z))
for j in range(16):
    v = [u16(z + j * 16 + 2 * q) for q in range(8)]
    print('  zone', j, v)
