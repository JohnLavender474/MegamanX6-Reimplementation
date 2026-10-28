import struct, sys
from pathlib import Path
import probe_ram
from mmxdemake.mips import disassemble
name, off, a, b = sys.argv[1], int(sys.argv[2],16), int(sys.argv[3]), int(sys.argv[4])
p = Path.home()/"mmx-data/x6/extracted/retail/rock_bin"/name
w = p.read_bytes()
for i in range(off//4 - a, off//4 + b):
    x = struct.unpack_from("<I", w, 4*i)[0]
    print(f"{4*i:06x}: {disassemble(x, 4*i)}")
