from img import *
import os
for ovl in [None]+list(range(2,59)):
    p=X6+f'/extracted/retail/rock_bin/{ovl:02d}.bin' if ovl is not None else None
    if p and not os.path.exists(p): continue
    bases=[0x800E9860] if ovl is not None and ovl<43 else [0x800FA000]
    if ovl is None: bases=[None]
    for base in bases:
        r=image(stage_ovl=ovl,stage_base=base) if base and base!=0x800FA000 else (image(boss_ovl=ovl) if base else image())
        if base is None: lo,hi=0x80010000,0x80080000
        else: lo=base; hi=base+os.path.getsize(p)
        for pc in range(lo,hi,4):
            d=dis(w(r,pc),pc)
            for off in (1080,1081,1082,1083):
                if d.endswith(f',{off}(a0)') or f',{off}(' in d:
                    if d.split()[0] in ('sb','sh','sw','swl','swr','lbu','lb'):
                        print(ovl,line(r,pc))
