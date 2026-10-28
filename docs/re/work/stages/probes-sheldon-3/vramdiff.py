import sys
from pathlib import Path
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram, ROW
a = read_vram(read_state(Path.home() / 'mmx-data/x6/dumps' / sys.argv[1]))
b = read_vram(read_state(Path.home() / 'mmx-data/x6/dumps' / sys.argv[2]))
B = 32
for y0 in range(0, 512, B):
    line = ''
    for x0 in range(0, 1024, B):
        n = 0
        for y in range(y0, y0 + B):
            o = y * ROW + x0 * 2
            if a[o:o + 2 * B] != b[o:o + 2 * B]:
                n += 1
        line += '.' if n == 0 else ('#' if n > B // 2 else '+')
    print(f'{y0:3d} {line}')
