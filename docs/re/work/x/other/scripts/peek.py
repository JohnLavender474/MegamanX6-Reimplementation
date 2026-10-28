import struct, sys
from pathlib import Path
from mmxdemake.learn import DEFAULT_DUMPS, DEFAULT_EXTRACTED
from mmxdemake.savestate import read_state, ram_from, ram_at
from mmxdemake.spriteexport import EXE

state = sys.argv[1]
path = Path(DEFAULT_DUMPS) / state
ram = ram_from(read_state(path), (Path(DEFAULT_EXTRACTED) / EXE).read_bytes(), path)
for spec in sys.argv[2:]:
    a, n = spec.split(":")
    a = int(a, 16); n = int(n)
    b = ram_at(ram, a, n)
    for i in range(0, n, 16):
        row = b[i:i+16]
        words = " ".join("%08x" % w for w in struct.unpack("<%dI" % (len(row) // 4), row[: len(row) // 4 * 4]))
        print("%08x: %s | %s" % (a + i, row.hex(" "), words))
