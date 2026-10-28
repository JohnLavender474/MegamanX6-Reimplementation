import sys
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_ram
from pathlib import Path
EXE = Path('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95').expanduser().read_bytes()
ram = read_ram(sys.argv[1], EXE)
a = int(sys.argv[2], 16); n = int(sys.argv[3], 0); w = int(sys.argv[4]) if len(sys.argv) > 4 else 16
for i in range(0, n, w):
    o = (a + i) & 0x1FFFFF
    print(f'{a+i:08x}: ' + ' '.join(f'{b:02x}' for b in ram[o:o+min(w, n-i)]))
