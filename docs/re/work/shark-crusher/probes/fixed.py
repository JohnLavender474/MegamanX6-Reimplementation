import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home()/'IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
from mmxdemake.objects import list_at
X = Path.home()/'mmx-data/x6'
ram = read_ram(X/'dumps'/sys.argv[1], (X/'extracted/retail/iso/SLUS_013.95').read_bytes())
for area in (0, 1):
    p = struct.unpack_from('<I', ram, (0x80073554 + 4*8 + area*4) & 0x1fffff)[0]
    print('area', area, hex(p))
    try: d = list_at(ram, p - 0x80000000)
    except Exception as e: print(e); continue
    for k in range(0, len(d), 8):
        f, t, i, kind, x, y = struct.unpack_from('<BBBBHH', d, k)
        print(f'  {k//8:3} flags {f:02x} type {t:3} id {i:3} kind {kind:02x} x {x} y {y}')
