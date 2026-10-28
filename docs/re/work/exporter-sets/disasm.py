import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.mips import disassemble
X = Path.home() / "mmx-data/x6/extracted/retail"
D = Path.home() / "mmx-data/x6/dumps"
path = D / sys.argv[1]
ram = ram_from(read_state(path), (X / "iso/SLUS_013.95").read_bytes(), path)
def u32(a): return struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
for arg in sys.argv[2:]:
    a, n = (arg.split(":") + ["60"])[:2]
    a = int(a, 16)
    print(f"; --- {a:08x}")
    for k in range(int(n)):
        pc = a + 4 * k
        print(f"{pc:08x}: {u32(pc):08x}  {disassemble(u32(pc), pc)}")
