import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
sys.path.insert(0, str(Path(__file__).parent))
from mmxdemake.mips import disassemble
import ram
root = Path.home() / "mmx-data/x6/extracted/retail"
offs = [int(x, 0) for x in sys.argv[1].split(",")]
def scan(name, d, base):
    n = len(d) // 4
    W = struct.unpack_from(f"<{n}I", d)
    for i in range(n):
        w = W[i]
        if w >> 26 == 0x2B and (w & 0xFFFF) in offs:
            rs = (w >> 21) & 31
            for j in range(max(0, i - 12), i):
                x = W[j]
                if x >> 26 == 0x23 and (x & 0xFFFF) == (w & 0xFFFF) and (x >> 21) & 31 == rs:
                    print(name, hex(base + 4 * i), disassemble(w, base + 4 * i), "| load at", hex(base + 4 * j))
                    break
exe = (root / "iso/SLUS_013.95").read_bytes()
scan("main", exe[0x800:], 0x80010000)
for f in sorted((root / "rock_bin").glob("*.bin")):
    scan(f.name, f.read_bytes(), 0)
