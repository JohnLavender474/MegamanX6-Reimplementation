import sys
from ramdis import ram, u32, u16, u8
r = ram(sys.argv[1])
stage = int(sys.argv[2])
import struct
for tbl, nm in ((0x8007349C, 'enemy_set'), (0x80073554, 'fixed_set')):
    for area in range(2):
        p = u32(r, tbl + stage * 8 + area * 4)
        print(f'{nm} stage {stage} area {area}: {p:#x}')
        if not (0x80000000 <= p < 0x80200000): continue
        i = 0
        while True:
            rec = bytes(r[(p & 0x1FFFFF) + 8 * i:(p & 0x1FFFFF) + 8 * i + 8])
            fl, ty, idv, kind = rec[0], rec[1], rec[2], rec[3]
            x, y = struct.unpack_from('<hh', rec, 4)
            if (kind & 0x0F) == 15 or i > 400:
                print('  end', i); break
            if len(sys.argv) < 4 or ty == int(sys.argv[3]) and (kind & 0xF) <= 2:
                print(f'  {i:3d} fl {fl:02x} type {ty:3d} id {idv:3d} kind {kind:02x} x {x:5d} y {y:5d}')
            i += 1
