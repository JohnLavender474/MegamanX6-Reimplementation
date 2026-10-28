import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.mips import disassemble
X = Path.home()/"mmx-data/x6/extracted/retail"
EXE = (X/"iso/SLUS_013.95").read_bytes()
def ram(state):
    p = Path.home()/"mmx-data/x6/dumps"/state
    return ram_from(read_state(p), EXE, p)
def w32(r, a): return struct.unpack_from("<I", r, a - 0x80000000)[0]
def dis(r, a, n):
    out = []
    for i in range(n):
        pc = a + 4*i; w = w32(r, pc)
        out.append(f"{pc:08x}: {w:08x}  {disassemble(w, pc)}")
    return "\n".join(out)
def hexd(r, a, n):
    b = r[a-0x80000000:a-0x80000000+n]
    return "\n".join(f"{a+i:08x}: " + " ".join(f"{x:02x}" for x in b[i:i+16]) for i in range(0, n, 16))
if __name__ == "__main__":
    r = ram(sys.argv[1]); cmd = sys.argv[2]; a = int(sys.argv[3], 16); n = int(sys.argv[4])
    print(dis(r, a, n) if cmd == "d" else hexd(r, a, n))
