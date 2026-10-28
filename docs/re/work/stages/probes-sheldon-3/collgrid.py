import sys
sys.path.insert(0, __import__('os').path.dirname(__file__))
from livecoll import load, cell
st, x0, x1, y0, y1 = sys.argv[1], *map(int, sys.argv[2:6])
r, layout, live, _ = load(st)
print('      ' + ''.join(f'{x // 16 % 100:>3d}' for x in range(x0, x1, 16)))
for y in range(y0, y1, 16):
    print(f'{y:5d} ' + ''.join(f'{cell(layout, live, x, y)[2]:>3d}' if cell(layout, live, x, y)[2] else '  .' for x in range(x0, x1, 16)))
