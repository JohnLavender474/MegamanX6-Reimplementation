import sys, os
from pathlib import Path
from mmxdemake.arcentries import split, entry_data
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
b = (Path.home() / 'mmx-data/x6/extracted/retail/rock_dat/102.bin').read_bytes()
ents = split(b)
for st in sys.argv[1:]:
    r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + st), EXE)
    out = []
    for e in ents:
        d = entry_data(b, e)
        probe = d[0x100:0x140] if len(d) > 0x140 else d[:32]
        i = r.find(probe)
        hits = []
        while i >= 0 and len(hits) < 3:
            start = i - (0x100 if len(d) > 0x140 else 0)
            full = r[start:start + len(d)] == d
            hits.append(f'{0x80000000 + start:#x}{"=" if full else "~"}')
            i = r.find(probe, i + 1)
        out.append(f'{e.type:#x}:{",".join(hits) or "-"}')
    print(st, ' '.join(out))
