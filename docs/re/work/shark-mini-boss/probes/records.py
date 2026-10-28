import sys, struct
sys.path.insert(0, __file__.rsplit("/",1)[0])
from objprobe import ram, at
r = ram(sys.argv[1])
for base in (0x800F7BD8, 0x800F7E58):
    a = base; i = 0
    while True:
        fl, t, idd, kind, x, y = struct.unpack('<BBBBhh', at(r, a, 8))
        if (kind & 0xF) == 15: break
        if (kind & 0xF) in (0,1,2) and t in (16,) or (kind & 0xF) == 4 and t in (10,) or (kind&0xF)==3 and t==6 or (kind&0xF)==6 and t==3:
            print(f'{base:#x} rec {i}: flags {fl:02x} type {t} id {idd} kind {kind:02x} at ({x}, {y})')
        if (kind & 0xF) == 3 and t == 6: pass
        a += 8; i += 1
    print('end', i)
