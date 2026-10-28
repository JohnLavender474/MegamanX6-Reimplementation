"""ASCII map of layer 0 collision (32 px per character) with record markers."""
import sys
import xml.etree.ElementTree as ET
t = ET.parse(sys.argv[1]).getroot()
W, H = int(t.get('width')), int(t.get('height'))
col = next(l for l in t.findall('layer') if l.get('name') == 'collision')
g = [int(v) for v in col.find('data').text.replace('\n', '').split(',')]
v = [x - 1128 if x else 0 for x in g]
sym = {56: '#', 57: '=', 58: '~', 59: '|'}
order = {'|': 5, '~': 4, '=': 3, '#': 2, ' ': 0}
S = 32
y0, y1, x1 = 192, 464, 3520
grid = {}
for y in range(y0, y1, S):
    for x in range(0, x1, S):
        cs = [sym.get(v[(yy // 16) * W + xx // 16], ' ') for yy in (y, y + 16) for xx in (x, x + 16)]
        grid[(x, y)] = max(cs, key=lambda c: order[c])
marks = {'CheckpointMarker': 'C', 'TalkZone': 'T', 'RescueReploid': 'R', 'main_27': 'M', 'NightmareVirus': 'v',
         'stage_15': 'Q', 'stage_11': 'J', 'BossGate': 'D', 'BossSpawner': 'S', 'logic_30': 'A'}
for o in t.find("objectgroup[@name='records']").findall('object'):
    x, y = float(o.get('x')), float(o.get('y'))
    k = (int(x) // S * S, int(y) // S * S)
    if k in grid:
        grid[k] = marks.get(o.get('name'), '?')
grid[(79 // S * S, 363 // S * S)] = 'X'
print('       ' + ''.join(str((x // 320) % 10) if x % 320 == 0 else ' ' for x in range(0, x1, S)))
for y in range(y0, y1, S):
    print(f'{y:5d}  ' + ''.join(grid[(x, y)] for x in range(0, x1, S)).rstrip())
