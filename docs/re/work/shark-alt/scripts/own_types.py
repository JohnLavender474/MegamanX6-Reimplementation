"""List the type-table entries of each class that point into the loaded stage overlay."""
import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
from mmxdemake.codewalk import class_table
from mmxdemake.rekit import CLASSES
ex = Path.home() / "mmx-data/x6/extracted/retail"
p = Path.home() / "mmx-data/x6/dumps" / sys.argv[1]
data = read_state(p)
ram = ram_from(data, (ex / "iso/SLUS_013.95").read_bytes(), p)
scr = scratchpad(data)
size = len((ex / f"rock_bin/{int(sys.argv[2]):02d}.bin").read_bytes())
lo, hi = 0x800E9860, 0x800E9860 + size
for cls, spec in CLASSES.items():
    t = class_table(ram, scr, spec[1])
    n = spec[0] if isinstance(spec[0], int) else 80
    own = []
    for i in range(n):
        e = struct.unpack_from("<I", ram, (t + 4 * i) & 0x1FFFFF)[0]
        if lo <= e < hi:
            own.append((i, hex(e)))
    print(cls, hex(t), own)
