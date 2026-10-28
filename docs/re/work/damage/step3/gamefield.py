import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.mips import disassemble
root = Path.home() / "mmx-data/x6/extracted/retail"
lo, hi = int(sys.argv[1], 0), int(sys.argv[2], 0)
files = [root / "iso/SLUS_013.95"] + sorted((root / "rock_bin").glob("*.bin"))
for f in files:
    d = f.read_bytes()
    exe = f.name.startswith("SLUS")
    base = {}
    n = len(d) // 4
    for i in range(n):
        w = struct.unpack_from("<I", d, 4 * i)[0]
        op, rs, rt = w >> 26, (w >> 21) & 31, (w >> 16) & 31
        a = 4 * i + 0x80010000 - 0x800 if exe else 4 * i
        if op == 9 and (w & 0xFFFF) == 0xCED0:
            base[rt] = i
            continue
        if op in (0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B) and rs in base and i - base[rs] < 40:
            off = w & 0xFFFF
            if lo <= off <= hi:
                print(f.name, hex(a), disassemble(w, a))
