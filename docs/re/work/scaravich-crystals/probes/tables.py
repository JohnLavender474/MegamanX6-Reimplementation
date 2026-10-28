import struct
from ramdis import ram, u32
r = ram('scaravich-1_start.state.gz')
def h(a, n): return struct.unpack('<%dh' % n, bytes(r[a & 0x1FFFFF:(a & 0x1FFFFF) + 2 * n]))
print('setup jtbl by var 0x800E9874:', [hex(u32(r, 0x800E9874 + 4 * i)) for i in range(15)])
print('death jtbl by var-10 0x800E98B4:', [hex(u32(r, 0x800E98B4 + 4 * i)) for i in range(5)])
print('section entry table 0x800F5754 (player x,y; cam0 x,y; cam1 x,y; cam2 x,y; ...; limits min_x,max_x,min_y,max_y at +16):')
for s in range(8): print(' ', s, h(0x800F5754 + 32 * s, 16))
print('return table 0x800F5910 (main 12):')
for s in range(4): print(' ', s, h(0x800F5910 + 32 * s, 16))
print('totem pole place 0x800F5990:', [h(0x800F5990 + 4 * i, 2) for i in range(4)])
print('0x800E9864 head offsets:', h(0x800E9864, 5))
