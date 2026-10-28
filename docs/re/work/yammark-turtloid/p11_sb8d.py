import struct, sys
from pathlib import Path
import probe_ram
from mmxdemake.mips import disassemble
srcs = [("exe", (Path.home()/"mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()[0x800:], 0x80010000)]
for f in sorted((Path.home()/"mmx-data/x6/extracted/retail/rock_bin").iterdir()):
    srcs.append((f.name, f.read_bytes(), 0))
for name, b, base in srcs:
    n = len(b)//4
    w = struct.unpack_from(f"<{n}I", b)
    for i, x in enumerate(w):
        if (x >> 26) == 0x28 and (x & 0xFFFF) == 0x8D:
            ctx = [disassemble(w[k], base+4*k) for k in range(max(0,i-40), i+1)]
            if any("-12592" in s or "28832" in s for s in ctx):
                print(f"--- {name} {base+4*i:#x} {disassemble(x, base+4*i)}")
                print("\n".join(ctx[-25:]))
