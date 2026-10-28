import re
p = "/home/johnlavender/mmx-data/x6/tmx/m122-infinity-mijinion-hidden-area/infinity-mijinion-hidden-area.tmx"
t = open(p).read()
m = re.search(r'<layer id="\d+" name="collision".*?<data encoding="csv">(.*?)</data>', t, re.S)
vals = [int(v) for v in m.group(1).replace("\n", "").split(",")]
W, H = 160, 64
cell = lambda c, r: (vals[r*W+c] - 1775) if vals[r*W+c] else 0
def sym(v):
    if v == 56: return "#"
    if v == 57: return "="
    if v == 59: return "|"
    if v == 46: return "!"
    if 41 <= v <= 47: return "\\"
    if 1 <= v <= 16: return "/"
    return " "
prio = " #=/\\!|"
marks = {(1872,384):"C",(896,416):"z",(816,384):"z",(224,304):"T",(48,312):"L",(8,320):"I",(1616,384):"D",(1936,384):"D",(2000,384):"S",(256,240):"Z",(1456,248):"B"}
for x,y in [(696,304),(920,336),(984,576),(1160,432),(1240,656),(1392,672),(1464,672),(1528,672),(1504,384)]: marks[(x,y)]="R"
for x,y in [(576,432),(784,304),(976,384),(1200,328),(1184,600),(1232,256),(1272,368),(1408,352),(1488,272),(1448,592),(1464,640),(1512,592),(1584,608)]: marks.setdefault((x,y),"v")
rows = []
for R in range(208//32, 736//32):
    line = ""
    for C in range(0, 2304//32):
        best = " "
        for dr in (0,1):
            for dc in (0,1):
                s = sym(cell(C*2+dc, R*2+dr))
                if prio.index(s) > prio.index(best): best = s
        for (x,y),ch in marks.items():
            if x//32 == C and y//32 == R: best = ch
        line += best
    rows.append(f"{R*32:5d} {line.rstrip()}")
print("\n".join(rows))
