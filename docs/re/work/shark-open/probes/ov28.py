import sys, struct
sys.path.insert(0, '.')
from sp import *
from pathlib import Path
ram = load('shark-1_ride')
base = 0x800E9860
for stg, area, ov in [(20,1,28),(20,0,27),(19,0,25),(19,1,26),(21,0,29),(21,1,30),(22,0,31),(22,1,32)]:
    ovb = (Path.home()/f'mmx-data/x6/extracted/retail/rock_bin/{ov:02d}.bin').read_bytes()
    for name, tbl in (('enemy', 0x8007349C), ('fixed', 0x80073554)):
        p = w(ram, tbl + stg*8 + area*4)
        off = p - base
        recs = []
        if 0 <= off < len(ovb):
            o = off
            while o + 8 <= len(ovb):
                a_, b_, i_, k_ = ovb[o:o+4]
                x, y = struct.unpack_from('<hh', ovb, o+4)
                if k_ & 0xF == 15: break
                recs.append((k_ & 0xF, b_, i_, x, y)); o += 8
        k7 = [r for r in recs if r[0] == 7]
        rides = [r for r in recs if r[0] == 3 and r[1] == 30]
        kinds = {}
        for r in recs: kinds[(r[0], r[1])] = kinds.get((r[0], r[1]), 0) + 1
        print(f'stage {stg} area {area} ov {ov} {name} {p:08x} n={len(recs)} kind7={k7} logic30={rides}')
        if stg == 20 and area == 1: print('   ', sorted(kinds.items()))
