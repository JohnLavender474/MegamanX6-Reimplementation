import sys, struct
sys.path.insert(0, '.')
from sp import *
from pathlib import Path
ram = load('shark-1_ride')
base = 0x800E9860
bins = {}
for f in sorted((Path.home()/'mmx-data/x6/extracted/retail/rock_bin').glob('*.bin')):
    bins[int(f.stem)] = f.read_bytes()
for stg in range(0, 23):
    for area in (0, 1):
        p = w(ram, 0x80073554 + stg*8 + area*4)
        for ov, ovb in bins.items():
            off = p - base
            if not (0 <= off < len(ovb) - 8): continue
            recs = []; o = off; ok = False
            while o + 8 <= len(ovb) and len(recs) < 300:
                k_ = ovb[o+3]
                if k_ & 0xF == 15: ok = True; break
                recs.append((k_ & 0xF, ovb[o+1], ovb[o+2])); o += 8
            if not ok: continue
            k7 = [r for r in recs if r[0] == 7 and r[1] == 2]
            if k7 and all(r[0] in (3,4,5,6,7) for r in recs):
                print(stg, area, 'ov', ov, f'{p:08x}', 'n', len(recs), k7)
