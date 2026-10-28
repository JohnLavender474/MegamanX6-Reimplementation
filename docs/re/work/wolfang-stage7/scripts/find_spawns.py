"""List code in the stage overlay that writes type N into byte 1 of an object
(addiu rX,zero,N then sb rX,1(..)), with the var byte written near it.
Usage (repo root, PYTHONPATH=.): find_spawns.py <state> <type>"""
import re, struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble

EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
DUMPS = Path.home() / "mmx-data/x6/dumps"
state, typ = sys.argv[1], int(sys.argv[2])
ram = read_ram(DUMPS / state, EXE.read_bytes())
lo, hi = 0x800E9860, 0x800FA000
lines = []
for pc in range(lo, hi, 4):
    w = struct.unpack_from("<I", ram, pc & 0x1FFFFF)[0]
    lines.append((pc, disassemble(w, pc)))
for i, (pc, d) in enumerate(lines):
    m = re.match(rf"addiu (\w+),zero,{typ}$", d)
    if not m:
        continue
    reg = m.group(1)
    for pc2, d2 in lines[i + 1:i + 12]:
        if re.match(rf"sb {reg},1\(\w+\)$", d2):
            near = [f"{p:08x}: {x}" for p, x in lines[max(0, i - 8):i + 12] if ",2(" in x and x.startswith("sb")]
            print(f"{pc:08x}: {d} -> {pc2:08x}: {d2}   var stores: {near}")
            break
