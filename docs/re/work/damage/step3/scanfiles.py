import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.mips import disassemble
root = Path.home() / "mmx-data/x6/extracted/retail"
vals = [int(x, 0) for x in sys.argv[1:]]
files = [root / "iso/SLUS_013.95"] + sorted((root / "rock_bin").glob("*.bin"))
for f in files:
    d = f.read_bytes()
    exe = f.name.startswith("SLUS")
    for off in range(0, len(d) - 3, 4):
        w = struct.unpack_from("<I", d, off)[0]
        op = w >> 26
        if op in (0x09, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B, 0x0D, 0x0F) and (w & 0xFFFF) in vals:
            a = off + 0x80010000 - 0x800 if exe else off
            print(f.name, hex(a), disassemble(w, a))
