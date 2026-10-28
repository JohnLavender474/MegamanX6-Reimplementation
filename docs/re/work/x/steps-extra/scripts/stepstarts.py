import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.codewalk import is_function_start, function_end
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
D = Path.home()/"mmx-data/x6/dumps"
u32 = lambda r, a: struct.unpack_from("<I", r, a)[0]
rams = {}
for n in ("intro-1_start.state.gz", "turtloid-6_zero-player.state.gz"):
    p = D/n; rams[n[:5]] = ram_from(read_state(p), exe, p)
r = rams["intro"]
for k in range(32, 78):
    t = u32(r, 0x73ACC + 4*k)
    row = [f"{k:2d} {t:08x}"]
    for key, rr in rams.items():
        s = is_function_start(rr, t)
        try: e = function_end(rr, t)
        except Exception as ex: e = None
        row.append(f"{key}: start={int(s)} end={e and hex(e)} first={u32(rr, t & 0x1FFFFF):08x}")
    print("  ".join(row))
