import sys
sys.path.insert(0, '.')
from sp import *
ram = load(sys.argv[1])
t = int(sys.argv[2], 16)
cells = []
for i in range(80):
    c = b(ram, t + 2*i); d = b(ram, t + 2*i + 1)
    cells.append(f'{i}:{c}/{d}')
print(' '.join(cells))
