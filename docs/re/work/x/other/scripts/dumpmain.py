import struct, sys
from pathlib import Path
from mmxdemake.learn import DEFAULT_DUMPS, DEFAULT_EXTRACTED
from mmxdemake.savestate import read_state, ram_from, ram_at
from mmxdemake.spriteexport import EXE
from mmxdemake.mips import disassemble

state, lo, hi, out = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), sys.argv[4]
path = Path(DEFAULT_DUMPS) / state
ram = ram_from(read_state(path), (Path(DEFAULT_EXTRACTED) / EXE).read_bytes(), path)
b = ram_at(ram, lo, hi - lo)
with open(out, "w") as f:
    for i in range(0, hi - lo, 4):
        w = struct.unpack_from("<I", b, i)[0]
        pc = lo + i
        f.write("%08x: %08x  %s\n" % (pc, w, disassemble(w, pc)))
