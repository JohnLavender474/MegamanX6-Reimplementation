import json
s = json.load(open('/home/johnlavender/mmx-data/x6/collision/m099/screens.json'))['screens']
L = json.load(open('/home/johnlavender/mmx-data/x6/stages/m099-metal-shark-player-section-2/layout.json'))['layers'][0]
o = json.load(open('/home/johnlavender/mmx-data/x6/stages/m099-metal-shark-player-section-2/objects.json'))['objects']
def col(x, y):
    scr = L[y // 256][x // 256]
    return s[scr]['collision'][(y % 256) // 16][(x % 256) // 16]
from collections import Counter
c0 = Counter(); ci = Counter(); cp = Counter(); cu = Counter()
for r in o:
    if r['b'] == 35 and r['kind'] in (0, 1, 2):
        x, y = r['x'], r['y']
        w = 2 if r['id'] == 0 else 1
        for i in range(w):
            c0[col(x - 16 + 16*i, y - 16)] += 1
            ci[col(x - 16 + 16*i, y + 464)] += 1
            cp[col(x - 16 + 16*i, y + 496)] += 1
            cu[col(x - 16 + 16*i, y)] += 1
print('ids', Counter(r['id'] for r in o if r['b']==35 and r['kind'] in (0,1,2)))
print('ys', sorted(Counter(r['y'] for r in o if r['b']==35 and r['kind'] in (0,1,2)).items()))
print('dest (y-16)', c0); print('row below dest (y)', cu); print('ice src (y+464)', ci); print('plain src (y+496)', cp)
