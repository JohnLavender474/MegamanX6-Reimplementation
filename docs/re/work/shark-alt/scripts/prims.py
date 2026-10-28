"""Find GPU sprite primitives in RAM whose screen rectangle covers a point."""
import sys, struct
sys.path.insert(0, '.')
from probe_live import load
r = load(sys.argv[1]); px, py = int(sys.argv[2]), int(sys.argv[3])
hits = []
for a in range(0, 0x200000 - 40, 4):
    code = r[a + 7]
    if code & 0xFC == 0x2C:
        xs = [struct.unpack_from('<hh', r, a + 8 + 8 * i) for i in range(4)]
        x0 = min(p[0] for p in xs); x1 = max(p[0] for p in xs); y0 = min(p[1] for p in xs); y1 = max(p[1] for p in xs)
        if x0 <= px <= x1 and y0 <= py <= y1 and 0 < x1 - x0 < 64 and 0 < y1 - y0 < 64:
            clut, tpage = struct.unpack_from('<H', r, a + 14)[0], struct.unpack_from('<H', r, a + 22)[0]
            hits.append((hex(0x80000000 + a), 'FT4', xs, hex(clut), hex(tpage)))
    elif code & 0xFC in (0x64, 0x74, 0x7C):
        x, y = struct.unpack_from('<hh', r, a + 8)
        if code & 0xFC == 0x64:
            w, h = struct.unpack_from('<hh', r, a + 16)
        else:
            w = h = 8 if code & 0xFC == 0x74 else 16
        if x <= px < x + w and y <= py < y + h and 0 < w < 64 and 0 < h < 64:
            hits.append((hex(0x80000000 + a), 'SPRT', (x, y, w, h), hex(struct.unpack_from('<H', r, a + 14)[0])))
for h in hits[:40]:
    print(h)
print(len(hits))
