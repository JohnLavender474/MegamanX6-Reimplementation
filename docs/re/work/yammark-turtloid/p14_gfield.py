import struct, sys
from pathlib import Path
import probe_ram
from mmxdemake.mips import disassemble
offs = [int(x, 16) for x in sys.argv[1].split(",")]
srcs = [("exe", (Path.home()/"mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()[0x800:], 0x80010000)]
for f in sorted((Path.home()/"mmx-data/x6/extracted/retail/rock_bin").iterdir()):
    srcs.append((f.name, f.read_bytes(), 0))
OPS = {0x20:'lb',0x21:'lh',0x23:'lw',0x24:'lbu',0x25:'lhu',0x28:'sb',0x29:'sh',0x2B:'sw'}
for name, b, base in srcs:
    n = len(b)//4
    w = struct.unpack_from(f"<{n}I", b)
    for i, x in enumerate(w):
        op = x >> 26; imm = x & 0xFFFF
        if op in OPS and imm in offs:
            rs = (x >> 21) & 31
            ctx = [w[k] for k in range(max(0,i-30), i)]
            ok = False
            for k, y in enumerate(ctx):
                if (y >> 26) == 0x09 and (y & 0xFFFF) == 0xCED0 and ((y >> 16) & 31) == rs:
                    ok = True
            if ok:
                print(f"{name} {base+4*i:#x} {disassemble(x, base+4*i)}")
