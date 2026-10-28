import re, sys, collections
p = "/home/johnlavender/mmx-data/x6/tmx/m122-infinity-mijinion-hidden-area/infinity-mijinion-hidden-area.tmx"
t = open(p).read()
m = re.search(r'<layer id="\d+" name="collision".*?<data encoding="csv">(.*?)</data>', t, re.S)
vals = [int(v) for v in m.group(1).replace("\n", "").split(",")]
W, H = 160, 64
cells = [[(v - 1775) if v else 0 for v in vals[r*W:(r+1)*W]] for r in range(H)]
cnt = collections.Counter(c for row in cells for c in row)
print("counts", sorted(cnt.items()))
sym = {0: " ", 56: "#", 57: "=", 59: "|", 63: "^", 45: "-", 46: "!", 47: "%", 16: "u"}
for v in range(1, 13): sym[v] = "/"
for v in range(41, 45): sym[v] = "w"
grid = [[sym.get(c, "?") for c in row] for row in cells]
marks = [(1872,384,"C"),(896,416,"z"),(816,384,"z"),(224,304,"T"),(696,304,"R"),(920,336,"R"),(984,576,"R"),(1160,432,"R"),(1240,656,"R"),(1392,672,"R"),(1464,672,"R"),(1528,672,"R"),(1504,384,"R"),
 (576,432,"v"),(784,304,"v"),(976,384,"v"),(1200,328,"v"),(1184,600,"v"),(1232,256,"v"),(1272,368,"v"),(1408,352,"v"),(1488,272,"v"),(1448,592,"v"),(1464,640,"v"),(1512,592,"v"),(1584,608,"v"),
 (1456,248,"B"),(48,312,"L"),(8,320,"I"),(1616,384,"D"),(1936,384,"D"),(2000,384,"S"),(256,240,"Z")]
for x, y, ch in marks:
    grid[y//16][x//16] = ch
x0 = int(sys.argv[1]) if len(sys.argv) > 1 else 0
x1 = int(sys.argv[2]) if len(sys.argv) > 2 else 160
used = [r for r in range(H) if any(c != " " for c in grid[r][x0:x1])]
for r in range(min(used), max(used)+1):
    print(f"{r*16:5d} " + "".join(grid[r][x0:x1]))
odd = collections.defaultdict(list)
for r in range(H):
    for c in range(W):
        if cells[r][c] not in (0, 56):
            odd[cells[r][c]].append((c*16, r*16))
for k, v in odd.items():
    xs = sorted(set(a for a, b in v)); ys = sorted(set(b for a, b in v))
    print(k, len(v), "x", xs[0], "-", xs[-1]+15, "y", ys[0], "-", ys[-1]+15)
