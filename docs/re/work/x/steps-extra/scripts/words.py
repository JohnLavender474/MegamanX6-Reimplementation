import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
p = Path.home()/"mmx-data/x6/dumps"/sys.argv[1]
r = ram_from(read_state(p), exe, p)
a, n = int(sys.argv[2], 16), int(sys.argv[3])
size = int(sys.argv[4]) if len(sys.argv) > 4 else 4
fmt = {1: "<B", 2: "<H", 4: "<I"}[size]
vals = [struct.unpack_from(fmt, r, (a & 0x1FFFFF) + size*k)[0] for k in range(n)]
for k in range(0, n, 8):
    print(f"{a + size*k:08x} [{k:3d}]: " + " ".join(f"{v:0{2*size}x}" for v in vals[k:k+8]))
