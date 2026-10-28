import struct
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
        op = x >> 26; imm = x & 0xFFFF
        if (op == 0x09 and imm in (0x8D, 0xCF5D, 0xCF5C)) or (op in (0x29, 0x2B) and imm in (0x8C, 0xCF5C)):
            ctx = [disassemble(w[k], base+4*k) for k in range(max(0,i-40), min(n, i+8))]
            if any("-12592" in s for s in ctx) or imm >= 0xCF00:
                print(f"--- {name} {base+4*i:#x} {disassemble(x, base+4*i)}")
