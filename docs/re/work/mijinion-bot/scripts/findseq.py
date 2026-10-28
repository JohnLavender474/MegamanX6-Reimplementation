import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_ram
from pathlib import Path
EXE = Path('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95').expanduser().read_bytes()
ram = read_ram(sys.argv[1], EXE)
pat = b''.join(struct.pack('<I', int(x, 16)) for x in sys.argv[2:])
i = ram.find(pat)
while i >= 0:
    print(hex(0x80000000 + i)); i = ram.find(pat, i + 1)
