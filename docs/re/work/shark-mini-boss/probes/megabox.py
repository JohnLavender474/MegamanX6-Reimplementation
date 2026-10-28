import sys
sys.path.insert(0, __file__.rsplit("/",1)[0])
from objprobe import ram, s16, u8, u32, at
r = ram(sys.argv[1]); mg = 0x800970A0
for off in (0x50, 0x54, 0x68):
    p = u32(r, mg+off)
    b = at(r, p, 4) if p else b'\0\0\0\0'
    print(hex(off), hex(p), [x-256 if x>127 and i<2 else x for i,x in enumerate(b)])
print('collide', hex(u8(r, mg+0x70)), 'y', s16(r, mg+0xE))
