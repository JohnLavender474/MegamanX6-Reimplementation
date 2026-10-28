import json, sys
from pathlib import Path
from collections import Counter
L = json.load(open(Path.home()/'mmx-data/x6/stages/probes-shark-1/render-final/m098-metal-shark-player-section-1/layout.json'))
S = json.load(open(Path.home()/'mmx-data/x6/collision/m098/screens.json'))['screens']
lay = L['layers'][0]
def col(x, y):
    sc = lay[y//256][x//256]
    return S[sc]['collision'][(y%256)//16][(x%256)//16]
y0, y1 = int(sys.argv[1]), int(sys.argv[2])
vals = Counter()
print('x: first non-zero tile top in y range, and value')
for x in range(1456, 5056, 16):
    for y in range(y0, y1, 16):
        v = col(x, y)
        if v: print(x, y, v); vals[v]+=1; break
print(vals)
