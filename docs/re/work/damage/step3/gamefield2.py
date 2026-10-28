import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.mips import disassemble
root = Path.home() / "mmx-data/x6/extracted/retail"
offs = [int(x, 0) for x in sys.argv[1].split(",")]
ops = {int(x, 0) for x in sys.argv[2].split(",")}
files = [root / "iso/SLUS_013.95"] + sorted((root / "rock_bin").glob("*.bin"))
for f in files:
    d = f.read_bytes()
    exe = f.name.startswith("SLUS")
    last = -999
    for i in range(len(d) // 4):
        w = struct.unpack_from("<I", d, 4 * i)[0]
        op = w >> 26
        a = 4 * i + 0x80010000 - 0x800 if exe else 4 * i
        if op in (9, 0x23, 0x24, 0x20) and (w & 0xFFFF) == 0xCED0:
            last = i
        if op in ops and (w & 0xFFFF) in offs and i - last < 30:
            print(f.name, hex(a), disassemble(w, a), "game base", i - last, "insns before")
