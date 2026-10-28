import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.mips import disassemble
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
state = sys.argv[1]; a = int(sys.argv[2], 16); n = int(sys.argv[3])
p = Path.home()/"mmx-data/x6/dumps"/state
r = ram_from(read_state(p), exe, p)
for i in range(n):
    pc = a + 4*i; w = struct.unpack_from("<I", r, pc - 0x80000000)[0]
    print(f"{pc:08x}: {w:08x}  {disassemble(w, pc)}")
