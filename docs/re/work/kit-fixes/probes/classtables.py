import struct, sys
from pathlib import Path
from mmxdemake.rekit import _load
from mmxdemake.codewalk import is_function_start
from mmxdemake.savestate import ram_at

D = Path.home() / "mmx-data/x6/dumps"
X = Path.home() / "mmx-data/x6/extracted/retail"
for state in sys.argv[1:]:
    path, ram, scratch, regions = _load(D, X, state)
    for name, off in (("visual", 0x84), ("effect", 0x88), ("logic", 0x8C), ("misc", 0x90)):
        t = struct.unpack_from("<I", scratch, off)[0]
        n = 0
        while True:
            f = struct.unpack("<I", ram_at(ram, t + 4 * n, 4))[0]
            if not (any(lo <= f < hi for lo, hi in regions) and f % 4 == 0 and is_function_start(ram, f)):
                break
            n += 1
        print(state, name, hex(t), n)
