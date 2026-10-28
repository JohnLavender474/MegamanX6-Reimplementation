import struct
from pathlib import Path
import probe_ram
from mmxdemake.mips import disassemble
d = Path.home()/"mmx-data/x6/extracted/retail/rock_bin"
for f in sorted(d.iterdir()):
    b = f.read_bytes()
    n = len(b)//4
    w = struct.unpack_from(f"<{n}I", b)
    for i, x in enumerate(w):
        if (x >> 26) in (0x20,0x24,0x28,0x29,0x25,0x21) and (x & 0xFFFF) in (0x8D, 0x8C, 0xCF5D, 0xCF5C):
            win = [disassemble(w[i-k], 0) for k in range(1, 14) if i-k >= 0]
            if any(",56(" in s for s in win) or any("-12592" in s or "-12451" in s or "-12452" in s for s in win):
                print(f.name, hex(i*4), disassemble(x, 0))
