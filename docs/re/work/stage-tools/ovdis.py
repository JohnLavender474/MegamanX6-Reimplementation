import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "mmx-data/x6/re/tools"))
from mips import dis
ex = Path.home() / "mmx-data/x6/extracted/retail"
ov = (ex / f"rock_bin/{int(sys.argv[1]):02d}.bin").read_bytes()
a, b = int(sys.argv[2], 16), int(sys.argv[3], 16)
for pc in range(a, b, 4):
    w = struct.unpack_from("<I", ov, pc - 0x800E9860)[0]
    print(f"{pc:08x}: {w:08x}  {dis(w, pc)}")
