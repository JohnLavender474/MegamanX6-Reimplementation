import re, struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble

EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
DUMPS = Path.home() / "mmx-data/x6/dumps"
state = sys.argv[1] if len(sys.argv) > 1 else "wolfang-2_ice-slope.state.gz"
ram = read_ram(DUMPS / state, EXE.read_bytes())
lo, hi = 0x800E9860, 0x800FA000
lines = []
for pc in range(lo, hi, 4):
    w = struct.unpack_from("<I", ram, pc & 0x1FFFFF)[0]
    lines.append((pc, disassemble(w, pc)))
for i, (pc, d) in enumerate(lines):
    m = re.match(r"addiu (\w+),zero,6$", d)
    if not m:
        continue
    reg = m.group(1)
    for pc2, d2 in lines[i + 1:i + 12]:
        if re.match(rf"sb {reg},1\(\w+\)$", d2):
            print(f"{pc:08x}: {d}  ->  {pc2:08x}: {d2}")
            for pc3, d3 in lines[i - 14:i + 16]:
                print(f"    {pc3:08x}: {d3}")
            break
