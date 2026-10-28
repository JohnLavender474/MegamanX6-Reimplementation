"""ASCII collision map of the TMX collision layer, 32 px per character, with record markers."""
import sys
import xml.etree.ElementTree as ET
from collections import Counter
t = ET.parse(sys.argv[1]).getroot()
W, H = int(t.get('width')), int(t.get('height'))
first = {ts.get('name') or ts.get('source'): int(ts.get('firstgid')) for ts in t.findall('tileset')}
col = next(l for l in t.findall('layer') if l.get('name') == 'collision')
gids = [int(v) for v in col.find('data').text.replace('\n', '').split(',')]
cg = max(g for g in first.values() if any(gid >= g for gid in gids if gid))
cg = [g for n, g in first.items() if 'coll' in str(n).lower()]
cg = cg[0] if cg else min(first.values())
vals = [g - cg if g else 0 for g in gids]
print('collision counts', sorted(Counter(vals).items()))
step = int(sys.argv[2]) if len(sys.argv) > 2 else 2
x0c, x1c = (int(sys.argv[3]) // 16, int(sys.argv[4]) // 16) if len(sys.argv) > 4 else (0, W)
y0c, y1c = (int(sys.argv[5]) // 16, int(sys.argv[6]) // 16) if len(sys.argv) > 6 else (0, H)
rank = {0: 0}
def ch(v):
    if v == 0: return ' '
    if v == 56: return '#'
    if v == 57: return '='
    if v == 58: return '~'
    if v == 59: return '|'
    if v == 60: return '+'
    if v == 63: return '^'
    if 32 <= v <= 34: return 'H'
    return '?'
order = {'|': 9, '~': 8, '^': 7, '+': 6, '?': 5, 'H': 4, '=': 3, '#': 2, ' ': 0}
for y in range(y0c, y1c, step):
    row = ''
    for x in range(x0c, x1c, step):
        cs = [ch(vals[(y + dy) * W + x + dx]) for dy in range(step) for dx in range(step) if y + dy < H and x + dx < W]
        row += max(cs, key=lambda c: order[c])
    if row.strip():
        print(f'{y * 16:5d} {row}')
others = Counter(v for v in vals if ch(v) == '?')
print('other values', others)
