import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
sys.path.insert(0, "/home/johnlavender/mmx-data/x6/probe/sprites")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from wordlz import decode
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
M = 0x970A0
for state in sys.argv[1:]:
    p = Path.home()/"mmx-data/x6/dumps"/state
    r = ram_from(read_state(p), exe, p)
    t = struct.unpack_from("<I", r, M+0x38)[0]
    tb = t - 0x80000000
    first = struct.unpack_from("<I", r, tb)[0] & 0xFFFFF
    n = first // 4
    ok = bad = 0; tiles = 0; end = 0; errs = []
    for i in range(n):
        e = struct.unpack_from("<I", r, tb + 4*i)[0]
        cnt, off = e >> 20, e & 0xFFFFF
        try:
            out, pos = decode(r, tb + off)
            if len(out) == cnt * 128: ok += 1
            else: bad += 1; errs.append((i, cnt, len(out)))
            tiles += cnt; end = max(end, pos)
        except Exception as ex:
            bad += 1; errs.append((i, cnt, str(ex)))
    print(f"{state}: var {r[M+2]} table {t:#x} frames {n} ok {ok} bad {bad} tiles {tiles} "
          f"blob {end - tb} bytes (ends {end + 0x80000000:#x}) want {r[M+0x47]} loaded {r[M+0x48]} buf {r[M+0x49]}")
    if errs: print("  first errors:", errs[:5])
