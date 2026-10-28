import sys
from img import *
for ovl in [None]+list(range(2,59)):
    try: r=image(stage_ovl=ovl)
    except FileNotFoundError: continue
    lo,hi=(0x80010000,0x80080000) if ovl is None else (0x800E9860,0x800E9860+len(open(X6+f'/extracted/retail/rock_bin/{ovl:02d}.bin','rb').read()))
    for pc in range(lo,hi,4):
        d=dis(w(r,pc),pc)
        if d.endswith(',-12592') and d.startswith('addiu'):
            ctx=[dis(w(r,pc+4*k),pc+4*k) for k in range(-6,8)]
            js=[c for c in ctx if c.startswith('jal ')]
            if js and any(('0x4' in c and 'addiu a' in c and ('a1' in c or 'a2' in c)) or ('ori a' in c) for c in ctx):
                print(ovl,hex(pc),js,[c for c in ctx if 'a1' in c or 'a2' in c][:4])
