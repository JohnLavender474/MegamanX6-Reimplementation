import collections
import json
import re
import sys
from pathlib import Path

ROOT = Path.home() / "mmx-data/x6/re/gate-stage-2"
tmx = next((ROOT / "tmx").glob("m113-*/*.tmx")).read_text()
first = int(re.search(r'<tileset firstgid="(\d+)" name="collision"', tmx).group(1))
m = re.search(r'<layer id="\d+" name="collision".*?<data encoding="csv">(.*?)</data>', tmx, re.S)
vals = [int(v) for v in m.group(1).replace("\n", "").split(",")]
W, H = 384, 144
cells = [[(v - first) if v else 0 for v in vals[r * W:(r + 1) * W]] for r in range(H)]
cnt = collections.Counter(c for row in cells for c in row)
print("counts", sorted(cnt.items(), key=lambda kv: -kv[1]))

step = int(sys.argv[1]) if len(sys.argv) > 1 else 4
x0 = int(sys.argv[2]) if len(sys.argv) > 2 else 0
x1 = int(sys.argv[3]) if len(sys.argv) > 3 else W * 16
sym = {0: " ", 56: "#", 57: "=", 58: "%", 59: "|", 60: "~", 63: "^", 32: "H", 33: "H", 34: "_", 16: "u", 46: "r"}
for v in range(1, 13):
    sym[v] = "/"
for v in range(17, 29):
    sym[v] = "\\"
for v in range(41, 45):
    sym[v] = "w"


def pick(block):
    vs = [v for v in block if v]
    if not vs:
        return " "
    for p in (63, 59, 60, 32, 33, 34, 46, 57, 58, 56):
        if p in vs:
            return sym[p]
    return sym.get(vs[0], "?")


cw, ch = step, 2
grid = []
for r in range(0, H, ch):
    line = []
    for c in range(x0 // 16, x1 // 16, cw):
        block = [cells[rr][cc] for rr in range(r, min(r + ch, H)) for cc in range(c, min(c + cw, W))]
        line.append(pick(block))
    grid.append(line)
objs = json.loads((next((ROOT / "stages").glob("m113-*/objects.json"))).read_text())["objects"]
MARK = {(3, 6): "C", (3, 0): "z", (3, 14): "T", (4, 8): "D", (0, 55): "M", (0, 15): "f", (0, 11): "s",
        (4, 3): "p", (0, 34): "4", (4, 5): "e", (0, 31): "o", (0, 47): "i"}
for o in objs:
    kind = o["kind"] & 0x0F
    key = (3 if kind == 3 else 4 if kind == 4 else 0, o["b"])
    chm = MARK.get(key)
    if not chm or not (x0 <= o["x"] < x1):
        continue
    gr, gc = o["y"] // 16 // ch, (o["x"] - x0) // 16 // cw
    if 0 <= gr < len(grid) and 0 <= gc < len(grid[0]):
        grid[gr][gc] = chm
used = [i for i, l in enumerate(grid) if any(c != " " for c in l)]
for i in range(min(used), max(used) + 1):
    print(f"{i * ch * 16:5d} " + "".join(grid[i]).rstrip())
odd = collections.defaultdict(list)
for r in range(H):
    for c in range(W):
        if cells[r][c] not in (0, 56):
            odd[cells[r][c]].append((c * 16, r * 16))
for k in sorted(odd):
    v = odd[k]
    xs = sorted(set(a for a, b in v))
    ys = sorted(set(b for a, b in v))
    print(k, len(v), "x", xs[0], "-", xs[-1] + 15, "y", ys[0], "-", ys[-1] + 15)
