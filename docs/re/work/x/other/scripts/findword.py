import struct, sys
from pathlib import Path
from mmxdemake.learn import DEFAULT_DUMPS, DEFAULT_EXTRACTED
from mmxdemake.savestate import read_state, ram_from, ram_at
from mmxdemake.spriteexport import EXE

state = sys.argv[1]
path = Path(DEFAULT_DUMPS) / state
ram = ram_from(read_state(path), (Path(DEFAULT_EXTRACTED) / EXE).read_bytes(), path)
b = ram_at(ram, 0x80000000, 0x200000)
for spec in sys.argv[2:]:
    w = struct.pack("<I", int(spec, 16))
    i = b.find(w)
    while i >= 0:
        if i % 4 == 0:
            print(spec, "at %08x" % (0x80000000 + i))
        i = b.find(w, i + 1)
