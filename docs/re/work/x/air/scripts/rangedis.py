import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble

state = Path.home() / "mmx-data/x6/dumps" / sys.argv[1]
exe = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()
ram = read_ram(state, exe)
lo, hi = int(sys.argv[2], 16), int(sys.argv[3], 16)
if len(sys.argv) > 4 and sys.argv[4] == "words":
    for a in range(lo, hi, 4):
        print(f"{a:08x}: {struct.unpack_from('<I', ram, a & 0x1FFFFF)[0]:08x}")
    sys.exit()
for a in range(lo, hi, 4):
    w = struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
    print(f"{a:08x}: {w:08x}  {disassemble(w, a)}")
