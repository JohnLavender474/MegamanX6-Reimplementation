import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
sys.path.insert(0, str(Path(__file__).parent))
from mmxdemake.mips import disassemble
import ram
root = Path.home() / "mmx-data/x6/extracted/retail"
LUIS = {0x20: "0x200000", 0x40: "0x400000", 0x100: "0x1000000", 0x200: "0x2000000", 0x80: "0x800000"}
def scan(name, d, base):
    n = len(d) // 4
    W = struct.unpack_from(f"<{n}I", d)
    for i in range(n):
        w = W[i]
        if w >> 26 == 0x0F and (w & 0xFFFF) in LUIS:
            ctx = range(max(0, i - 40), min(n, i + 12))
            parts = [j for j in ctx if W[j] >> 26 == 0x23 and (W[j] & 0xFFFF) in (0xEC, 0x74, 0x70, 0x78)]
            if parts:
                a = base + 4 * i
                print(name, LUIS[w & 0xFFFF], hex(a), "|", disassemble(W[parts[0]], base + 4 * parts[0]))
exe = (root / "iso/SLUS_013.95").read_bytes()
scan("main", exe[0x800:], 0x80010000)
for st, nm in (("sheldon-1_lemons.state.gz", "X-ovl"), ("turtloid-6_zero-player.state.gz", "Zero-ovl")):
    r = ram.load(st)
    scan(nm, bytes(r[0x1EA000:0x1F4000]), 0x801EA000)
for f in sorted((root / "rock_bin").glob("*.bin")):
    scan(f.name + "(file offset)", f.read_bytes(), 0)
