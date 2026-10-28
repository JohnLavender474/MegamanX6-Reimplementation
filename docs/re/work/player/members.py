import sys, struct
sys.path.insert(0, "/home/johnlavender/mmx-data/x6/probe/sprites")
from pathlib import Path
from wordlz import decode
D = Path.home()/"mmx-data/x6/extracted/retail/rock_dat"
def frameset(b, o):
    try:
        first = struct.unpack_from("<I", b, o)[0] & 0xFFFFF
        n = first // 4
        if not 8 <= n <= 1000 or first % 4: return None
        end = 0
        for i in range(n):
            e = struct.unpack_from("<I", b, o+4*i)[0]
            out, pos = decode(b, o + (e & 0xFFFFF), 1 << 16)
            if len(out) != (e >> 20) * 128: return None
            end = max(end, pos)
        return n, end - o
    except Exception:
        return None
for m in range(80, 95):
    p = D/f"{m:03d}.bin"
    if not p.exists(): continue
    b = p.read_bytes()
    found = []
    o = 0
    while o < len(b) - 4:
        fs = frameset(b, o)
        if fs: found.append((o, *fs)); o += fs[1]; o = (o + 3) & ~3
        else: o += 4
    print(f"member {m}: {len(b)} bytes, head {b[:16].hex(' ')}, frame sets (offset, frames, bytes): {found}")
